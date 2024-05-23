#!/usr/bin/env python3


import json, os, re, shutil, subprocess, sys
from codegen import CodeGen, parseCodeGenJsonFiles
from run import Configurator, parseJsonFiles


appName = os.path.splitext(sys.argv[0])[0].replace("./", "")
geps = 0.001
resLimits = {
	"zcu104": {
		"lut": 230400.0,
		"ff": 460800.0,
		"dsp": 1728.0,
		"bram": 624.0
	}
}

def printWithTag(*args, **kwargs):
	sys.stdout.write("\033[1;34m[paretogen]\033[0m ")
	sys.stdout.write(*args, **kwargs)


def printErrWithTag(*args, **kwargs):
	sys.stderr.write("\033[1;31m[paretogen]\033[0m ")
	sys.stderr.write(*args, **kwargs)


def printWarnWithTag(*args, **kwargs):
	sys.stderr.write("\033[1;35m[paretogen]\033[0m ")
	sys.stderr.write(*args, **kwargs)


def parseParetoGenJsonFiles(experiment, k, baseExpRelPath=""):
	jsonContent = None
	jsonPath = os.path.join("sources", experiment, k, "{}.paretogen.json".format(k))

	if "" == baseExpRelPath:
		with open(jsonPath, "r") as jsonF:
			jsonContent = json.loads(jsonF.read())
	else:
		patchJsonContent = None

		if os.path.exists(jsonPath):
			with open(jsonPath, "r") as jsonF:
				patchJsonContent = json.loads(jsonF.read())

		with open(os.path.join("sources", experiment, k, baseExpRelPath, "{}.paretogen.json".format(k)), "r") as jsonF:
			jsonContent = json.loads(jsonF.read())

			if patchJsonContent is not None:
				Configurator.patch(jsonContent, patchJsonContent)

	return jsonContent


def loadCSV(csvFileName):
	csv = {}

	with open(csvFileName, "r") as csvF:
		csvContent = csvF.readlines()
		cols = csvContent[0].rstrip().split(",")
		for row in csvContent[1:]:
			cells = row.rstrip().split(",")
			csv[cells[0]] = {}
			for i in range(1, len(cols)):
				csv[cells[0]][cols[i]] = cells[i]

	return csv


def parseVivadoReport(rptFileName, parsedCode, noInputLoops, noOutputLoops):
	parseStartRegex = r" *\| *Loop Name *\| *min *\| *max *\| *Latency *\| *achieved *\| *target *\| *Count *\| *Pipelined *\|"
	parseRegex = re.compile(r" *\| *- Loop (\d+) *\| *([^ ]+) *\| *([^ ]+) *\| *([^\|]+) *\| *([^ ]+) *\| *([^ ]+) *\| *([^ ]+) *\| *([^ ]+) *\|")
	parseFinishRegex = r" *\+-*\+-*\+-*\+-*\+-*\+-*\+-*\+-*\+"
	parseBStartRegex = r" *\| *min *\| *max *\| *min *\| *max *\| *Type *\|"
	parseBRegex = re.compile(r" *\| *(\d+) *\| *(\d+) *\| *(\d+) *\| *(\d+) *\| *[^\|]+ *\|")
	parse2StartRegex = r" *\| *Name *\| *BRAM_18K *\| *DSP48E *\| *FF *\| *LUT *\| *URAM *\|"
	parse2Regex = re.compile(r" *\| *Total *\| *(\d+) *\| *(\d+) *\| *(\d+) *\| *(\d+) *\| *(\d+) *\|")
	parseState = 0

	numPoints = 0
	db = {}

	with open(rptFileName, "r") as rpt:
		allLines = rpt.readlines()

		# Get latency info from the general latency table.
		for l in allLines:
			# Search for the cycle report table
			if 0 == parseState:
				if re.search(parseBStartRegex, l) is not None:
					parseState = 1
			# Cycle report table found, eating the border line
			elif 1 == parseState:
				parseState = 2
			# Parsing cycle count
			elif 2 == parseState:
				lineDecoded = parseBRegex.match(l)

				if lineDecoded is None:
					parseState = 0
					break
				else:
					db["latency"] = {"min": int(lineDecoded.group(1)), "max": int(lineDecoded.group(2))}
					db["il"] = "---"
					db["iia"] = "---"
					db["iit"] = "---"

		if parseState != 0:
			raise RuntimeError("Retrieval failed (parseState left as {})".format(parseState))
		if "latency" not in db:
			raise RuntimeError("Total latency count not found in report")

		# Get iteration latency info
		allInfo = []
		for l in allLines:
			# Search for the cycle report table
			if 0 == parseState:
				if re.search(parseStartRegex, l) is not None:
					parseState = 1
			# Cycle report table found, eating the border line
			elif 1 == parseState:
				parseState = 2
			# Parsing cycle count
			elif 2 == parseState:
				lineDecoded = parseRegex.match(l)

				if lineDecoded is None:
					# If the line was not recognised, this may be a subloop count. We just ignore it.
					# If this is the border of the table, then we break out
					if re.search(parseFinishRegex, l) is not None:
						parseState = 0
						break
				else:
					ilSplit = lineDecoded.group(4).split("~")

					elem = {}
					elem["latency"] = {"min": int(lineDecoded.group(2)), "max": int(lineDecoded.group(3))}
					elem["il"] = int(lineDecoded.group(4)) if 1 == len(ilSplit) else "[{}; {}]".format(int(ilSplit[0]), int(ilSplit[1]))
					elem["iia"] = int(lineDecoded.group(5)) if lineDecoded.group(5) != "-" else "---"
					elem["iit"] = int(lineDecoded.group(6)) if lineDecoded.group(6) != "-" else "---"
					elem["autopipeline"] = lineDecoded.group(8)
					allInfo.append(elem)

					if int(lineDecoded.group(1)) != len(allInfo):
						raise RuntimeError("Loop count not following an incremental sequence")

		# Some sanity check
		if parseState != 0:
			raise RuntimeError("Retrieval failed for (parseState left as {})".format(parseState))
		if len(allInfo) - (noInputLoops + noOutputLoops) != 1:
			raise RuntimeError("Project loop count differs from NIN + NOUT + 1")

		# Now we handle all this latency info we just found
		db["il"] = allInfo[noInputLoops]["il"]
		db["iia"] = allInfo[noInputLoops]["iia"]
		db["iit"] = allInfo[noInputLoops]["iit"]
		db["autopipeline"] = allInfo[noInputLoops]["autopipeline"]
		# We subtract the loops responsible for moving data in and out according to NIN and NOUT
		for idx in range(noInputLoops):
			db["latency"]["min"] -= allInfo[idx]["latency"]["min"]
			db["latency"]["max"] -= allInfo[idx]["latency"]["max"]
		for idx in range(len(allInfo) - noOutputLoops, len(allInfo)):
			db["latency"]["min"] -= allInfo[idx]["latency"]["min"]
			db["latency"]["max"] -= allInfo[idx]["latency"]["max"]

		# Now search for the resources
		for l in allLines:
			# Search for the resources report table
			if 0 == parseState:
				if re.search(parse2StartRegex, l) is not None:
					parseState = 1
			# Resources report table found, now finding for the line containing the totals
			elif 1 == parseState:
				lineDecoded = parse2Regex.match(l)

				if lineDecoded is not None:
					db["resources-hls"] = {
						"tramb": int(lineDecoded.group(1)),
						"dsp48": int(lineDecoded.group(2)),
						"ff": int(lineDecoded.group(3)),
						"tlut": int(lineDecoded.group(4)),
						"uram": int(lineDecoded.group(5))
					}

					parseState = 0
					break

		if parseState != 0:
			raise RuntimeError("Retrieval failed for (parseState left as {})".format(parseState))

	anyPipelineSet = False
	for l in parsedCode["loops"]:
		if "1" == l["pipelining"]:
			anyPipelineSet = True

	if "autopipeline" in db and "yes" == db["autopipeline"] and not anyPipelineSet:
		raise RuntimeError("Auto-pipeline was triggered when it shouldn't")

	return db


# Comodo algorithm for pareto find
def getParetoFrontier(db, objectives, includeEqual=False, eps=0.001):
	# Create arrays
	markedPoints = []
	paretoFrontier = []

	# Iterate through all unmarked points
	for f in db:
		# Skip this point was already marked
		if f in markedPoints:
			continue

		# Compare against all other points
		for g in db:
			# Skip this point if already marked or if it's the same as f
			if g in markedPoints or f == g:
				continue

			# Check if g is inside the positive envelope of f
			# (i.e. o(g) >= o(f) for all objective functions o(.))
			isEqual = True
			inPositiveEnvelope = True
			for obj in objectives:
				if abs(float(db[g][obj]) - float(db[f][obj])) > eps:
					isEqual = False
				if db[g][obj] < db[f][obj]:
					inPositiveEnvelope = False

			# If positive, g is marked non-pareto optimal
			if includeEqual:
				if not isEqual and inPositiveEnvelope:
					markedPoints.append(g)
			else:
				if inPositiveEnvelope:
					markedPoints.append(g)

	# All points that were not marked are in the pareto frontier
	for f in db:
		if f not in markedPoints:
			paretoFrontier.append(f)

	return paretoFrontier


def pareto(options, experiment, kernels):
	printWithTag("Selected \"pareto\" command\n")

	if not os.path.exists("outcomes"):
		printWithTag("Creating \"outcomes\" folder\n")
		os.mkdir("outcomes")
	if not os.path.exists(os.path.join("outcomes", experiment)):
		printWithTag("Creating \"outcomes/{}\" folder\n".format(experiment))
		os.mkdir(os.path.join("outcomes", experiment))

	for k in kernels:
		printWithTag("\n")
		printWithTag("Selected kernel \"{}\"\n".format(k))

		if not os.path.exists(os.path.join("csvs", experiment, "{}.csv".format(k))):
			raise FileNotFoundError("CSV result file is missing for this kernel (did you collect the results using run.py?)")

		# Read the json config files (following inheritance)
		jsonContent, baseExpRelPath = parseJsonFiles(experiment, k)
		pgJsonContent = parseParetoGenJsonFiles(experiment, k, baseExpRelPath)

		repeated = "repeated" in pgJsonContent and pgJsonContent["repeated"]
		objectives = pgJsonContent["objectives"] if "objectives" in pgJsonContent else ["exectime", "lut", "ff", "dsp", "bram"]

		printWarnWithTag("Include repeated points in Pareto? {}\n".format(repeated))
		printWarnWithTag("Objectives sought: {}\n".format(objectives))

		if os.path.exists(os.path.join("outcomes", experiment, "{}.out".format(k))):
			raise FileExistsError("Pareto set file already exists")

		db = loadCSV(os.path.join("csvs", experiment, "{}.csv".format(k)))
		printWarnWithTag("CSV file {}.csv successfully loaded ({} valid design point(s))\n".format(k, len(db)))

		pfront = getParetoFrontier(db, objectives, repeated)
		printWarnWithTag("Found Pareto frontier ({} element(s))\n".format(len(pfront)))

		with open(os.path.join("outcomes", experiment, "{}.out".format(k)), "w") as outF:
			for elem in pfront:
				outF.write("{}\n".format(elem))

		printWarnWithTag("Found Pareto set saved to \"outcomes/{}/{}.out\"\n".format(experiment, k))


def prepare(options, experiment, kernels):
	printWithTag("Selected \"prepare\" command\n")

	for k in kernels:
		printWithTag("\n")
		printWithTag("Selected kernel \"{}\"\n".format(k))

		fullOutcomePath = os.path.join("outcomes", experiment, k)

		if not os.path.exists(os.path.join("outcomes", experiment, "{}.out".format(k))):
			raise FileNotFoundError("Pareto set file is missing for this kernel (did you run the \"pareto\" command?)")

		# Read the json config files (following inheritance)
		jsonContent, baseExpRelPath = parseJsonFiles(experiment, k)
		pgJsonContent = parseParetoGenJsonFiles(experiment, k, baseExpRelPath)
		cgJsonContent = parseCodeGenJsonFiles(experiment, k, baseExpRelPath)
		mkfList = []
		if isinstance(cgJsonContent["makefile"], str):
			mkfList.append(cgJsonContent["makefile"])
		else:
			mkfList = cgJsonContent["makefile"]
		srcList = []
		if isinstance(cgJsonContent["clsource"], str):
			srcList.append(cgJsonContent["clsource"])
		else:
			srcList = cgJsonContent["clsource"]

		priority = pgJsonContent["priority"] if "priority" in pgJsonContent else "exectime"
		enableRBWUnroll = "compcodegenmode" in pgJsonContent and pgJsonContent["compcodegenmode"]["rbwunroll"]
		enableExplVec = "compcodegenmode" in pgJsonContent and pgJsonContent["compcodegenmode"]["explvec"]

		printWarnWithTag("Priority objective: {}\n".format(priority))
		printWarnWithTag("Enable read-before-write unroll optimisation? {}\n".format(enableRBWUnroll))
		printWarnWithTag("Enable explicit vectorisation optimisation? {}\n".format(enableExplVec))

		if os.path.exists(fullOutcomePath):
			raise FileExistsError("Outcome folder already exists: {}".format(fullOutcomePath))
		printWarnWithTag("Creating \"{}\" folder\n".format(fullOutcomePath))
		os.mkdir(fullOutcomePath)

		# Load CSV file to get the metrics for each design point
		db = loadCSV(os.path.join("csvs", experiment, "{}.csv".format(k)))
		printWarnWithTag("CSV file {}.csv successfully loaded ({} valid design point(s))\n".format(k, len(db)))

		# Open the .out file to get the Pareto frontier
		pfront = None
		with open(os.path.join("outcomes", experiment, "{}.out".format(k))) as pfrontF:
			pfront = [x.strip() for x in pfrontF.readlines()]

		# Now look for the one (or multiple) with the smallest metric
		smallestV = float(2 ** 63)
		smallestP = []
		for point in pfront:
			if float(db[point][priority]) < smallestV:
				smallestV = float(db[point][priority])
				smallestP = [point]
			elif abs(float(db[point][priority]) - smallestV) < geps:
				smallestP.append(point)

		for point in smallestP:
			pointFolder = os.path.join("outcomes", experiment, k, point)

			# Create point folder
			os.mkdir(pointFolder)
			# Copy the general Makefile to the point folder
			shutil.copy2(os.path.join("sources", experiment, k, baseExpRelPath, "Makefile"), os.path.join(pointFolder, "Makefile"))
			# Copy the vivado folder to the point folder
			shutil.copytree(os.path.join("sources", experiment, k, baseExpRelPath, "vivado"), os.path.join(pointFolder, "vivado"), symlinks=True)

			# The files listed in the clsource and makefile tags of the json file are deleted; these are generated by CodeGen!
			for fil in [*mkfList, *srcList]:
				os.remove(os.path.join(pointFolder, "vivado", fil))

			# Start CodeGen for this point
			CodeGen.transform(experiment, k, point, enableRBWUnroll, enableExplVec, os.path.join("sources", experiment, k, baseExpRelPath, "vivado"))

			printWarnWithTag("Code {} generated\n".format(point))

		printWarnWithTag("Finished preparing projects for kernel {} (experiment {})\n".format(k, experiment))


def decide(options, experiment, kernels):
	printWithTag("Selected \"decide\" command\n")

	#modEnv = os.environ
	#if options["PATH"][1] is not None:
	#	modEnv["PATH"] = "{}:{}".format(options["PATH"][1], modEnv["PATH"])

	for k in kernels:
		printWithTag("\n")
		printWithTag("Selected kernel \"{}\"\n".format(k))
		filteredListDir = [x for x in os.listdir(os.path.join("outcomes", experiment, k)) if x != "opt"]

		# If there is just one point, no need to decide
		if 1 == len(filteredListDir):
			printWarnWithTag("No need to decide, there is just one code: {}\n".format(filteredListDir[0]))
			os.symlink(filteredListDir[0], os.path.join("outcomes", experiment, k, "opt"))
		else:
			paretoPreparedDB = {}
			for code in filteredListDir:
				codePath = os.path.join("outcomes", experiment, k, code)

				printWithTag("\n")
				printWithTag("Starting HLS for code \"{}\"\n".format(code))
				# Open make and pipe it to a "tee" process (hacky hack)
				mkeP = subprocess.Popen(["make", "vivado-decide"],
					cwd=codePath,# env=modEnv,
					stderr=subprocess.STDOUT,
					stdout=subprocess.PIPE
				)
				teeP = subprocess.Popen(["tee", os.path.join(codePath, "make.vivado-decide.out")], stdin=mkeP.stdout)
				mkeP.stdout.close()
				teeRet = teeP.communicate()
				# Mimic check=True behaviour from subprocess.run()
				mkeRet = mkeP.poll()
				if mkeRet:
					raise subprocess.CalledProcessError(mkeRet, ["make", "vivado-decide"], output=teeRet[0], stderr=teeRet[1])

				# Read the json config files (following inheritance)
				jsonContent, baseExpRelPath = parseJsonFiles(experiment, k)
				pgJsonContent = parseParetoGenJsonFiles(experiment, k, baseExpRelPath)
				objectives = pgJsonContent["objectives"] if "objectives" in pgJsonContent else ["exectime", "lut", "ff", "dsp", "bram"]

				# Parse the code
				parsedCode = CodeGen.parseCode(code)
				period = float(parsedCode["period"]) if "period" in parsedCode else (1000.0 / float(parsedCode["frequency"]))

				# Count the amount of input and output loops
				noInputLoops = 0
				noOutputLoops = 0
				for arr in jsonContent["arrays"]:
					if not ("offchip" in jsonContent and arr in jsonContent["offchip"]):
						# This array will have an input loop only if it is not write-only
						if not ("writeonly" in jsonContent["arrays"][arr] and jsonContent["arrays"][arr]["writeonly"]):
							noInputLoops += 1
						# This array will have an output loop only if it is not read-only
						if not ("readonly" in jsonContent["arrays"][arr] and jsonContent["arrays"][arr]["readonly"]):
							noOutputLoops += 1
				printWarnWithTag("Counted {} buffering input loops from json config file\n".format(noInputLoops))
				printWarnWithTag("Counted {} buffering output loops from json config file\n".format(noOutputLoops))
				# XXX Also add to the count other loops as reported in the json file
				if "vivado-otherloops" in jsonContent:
					otherLoopsBefore = jsonContent["vivado-otherloops"][0]
					otherLoopsAfter = jsonContent["vivado-otherloops"][1]
					noInputLoops += otherLoopsBefore
					noOutputLoops += otherLoopsAfter
					printWarnWithTag("Considering {} non-explored loops before the explored one according to json config file\n".format(otherLoopsBefore))
					printWarnWithTag("Considering {} non-explored loops after the explored one according to json config file\n".format(otherLoopsAfter))

				# Parse Vivado report
				vivDB = parseVivadoReport(os.path.join(codePath, "{}.verbose.rpt".format(k)), parsedCode, noInputLoops, noOutputLoops)
				printWarnWithTag("Vivado report parsed\n")
				printWarnWithTag("      Latency (max): {}\n".format(vivDB["latency"]["max"]))
				printWarnWithTag("    Exec. time (ns): {}\n".format(vivDB["latency"]["max"] * period))
				printWarnWithTag("               LUTs: {}\n".format(vivDB["resources-hls"]["tlut"]))
				printWarnWithTag("                FFs: {}\n".format(vivDB["resources-hls"]["ff"]))
				printWarnWithTag("               DSPs: {}\n".format(vivDB["resources-hls"]["dsp48"]))
				printWarnWithTag("              BRAMs: {}\n".format(vivDB["resources-hls"]["tramb"]))
				if "uram" in vivDB["resources-hls"]:
					printWarnWithTag("              URAMs: {}\n".format(vivDB["resources-hls"]["uram"]))

				paretoPreparedDB[code] = {
					"exectime": float(vivDB["latency"]["max"] * period),
					"lut": int(vivDB["resources-hls"]["tlut"]),
					"ff": int(vivDB["resources-hls"]["ff"]),
					"dsp": int(vivDB["resources-hls"]["dsp48"]),
					"bram": int(vivDB["resources-hls"]["tramb"])
				}

			# We don't include repeated here. Enough is enough!
			# (actually, we are now including repeated, and we filter them out below using lowest-freq criteria)
			finalPFront = getParetoFrontier(paretoPreparedDB, objectives, True)
			printWarnWithTag("Found {} candidate(s) for best point\n".format(len(finalPFront)))

			# If it is just one candidate point, then we done!
			if 1 == len(finalPFront):
				printWarnWithTag("Point decided: {}\n".format(filteredListDir[0]))
				os.symlink(finalPFront[0], os.path.join("outcomes", experiment, k, "opt"))
			# If not, we still have one more trick: select the one with the lowest normalised objective "consumption"
			else:
				# Select the points that have the lowest execution time and the lowest accumulated normalised resource consumption (of considered objectives)
				selectedPoints = []
				lowestExecTime = 2 ** 62
				lowestAccValue = (float(2 ** 62), [])
				for point in finalPFront:
					# Special case for exectime, we do not normalise the metric, we just get the smallest
					if "exectime" in objectives:
						# Ignore it if we have already found a point with smaller exectime
						if paretoPreparedDB[point]["exectime"] > lowestExecTime:
							printWarnWithTag("Point {} skipped due to its execution time: {:.2f}\n".format(point, paretoPreparedDB[point]["exectime"]))
							continue

					accValue = 0.0
					for obj in objectives:
						# We do not consider exectime here
						if "exectime" == obj:
							continue

						accValue += paretoPreparedDB[point][obj] / resLimits[jsonContent["platform"]][obj]

					if abs(accValue - lowestAccValue[0]) < geps:
						lowestAccValue[1].append(point)
					elif accValue < lowestAccValue[0]:
						lowestAccValue = (accValue, [point])

					printWarnWithTag("Normalised accumulated resource metric for point {}: {:.2f}\n".format(point, accValue))

				# Now the final touch. For all the repeated normalised points, we select the one with the lowest frequency
				lowestFrequency = 2 ** 62
				finalPoint = None
				for point in lowestAccValue[1]:
					# Parse the code
					parsedCode = CodeGen.parseCode(code)
					frequency = float(parsedCode["frequency"]) if "frequency" in parsedCode else (1000.0 / float(parsedCode["period"]))
					if frequency < lowestFrequency:
						lowestFrequency = frequency
						finalPoint = point

				printWarnWithTag("Decided point with smallest normalised value and lowest frequency ({:.2f} MHz): {}\n".format(lowestFrequency, finalPoint))
				os.symlink(finalPoint, os.path.join("outcomes", experiment, k, "opt"))

		printWarnWithTag("Optimised project is located at \"{}\"\n".format(os.path.join("outcomes", experiment, k, "opt")))
		printWarnWithTag("Finished deciding optimised projects for kernel {} (experiment {})\n".format(k, experiment))


if "__main__" == __name__:
	options = {
		"SILENT": [str, "no"],
		# XXX PATH option is disabled here because Lina is not used here, just Vivado
		# and Vivado requires also setup of $LD_LIBRARY_PATH and other variables (e.g. $XILINX_SDX).
		# Therefore it is better to set these outside of this tool. Cirith already handles that
		#"PATH": [str, None],
	}
	filteredArgv = [x for x in sys.argv[1:] if "=" not in x]
	for x in sys.argv[1:]:
		if "=" in x:
			xSplit = x.split("=")

			if xSplit[0] not in options:
				raise KeyError("Option not found: {}".format(xSplit[0]))

			options[xSplit[0]][1] = options[xSplit[0]][0](xSplit[1])
			print("INFO: Option {} set to {}".format(xSplit[0], options[xSplit[0]][1]))

	if len(filteredArgv) < 2:
		sys.stderr.write("Usage: {} [OPTION=VALUE]... COMMAND EXPERIMENT [KERNELS]...\n".format(appName))
		sys.stderr.write("    [OPTION=VALUE]... change options (e.g. UNCERTAINTY=27.0):\n")
		sys.stderr.write("                          SILENT      all spawned subprocesses will be muted\n")
		sys.stderr.write("                                      (*.out files are still written)\n")
		sys.stderr.write("                                      (use SILENT=yes to enable)\n")
		sys.stderr.write("                                      DEFAULT: no\n")
		sys.stderr.write("    COMMAND           may be\n")
		sys.stderr.write("                          pareto\n")
		sys.stderr.write("                          prepare\n")
		sys.stderr.write("                          decide\n")
		sys.stderr.write("    EXPERIMENT        the experiment to consider (i.e. the folders in \"csvs/*\")\n")
		sys.stderr.write("    [KERNELS]...      the kernels to run the command on\n")
		sys.stderr.write("                      (will consider all in the experiment when omitted)\n")
		exit(-1)

	command = filteredArgv[0]
	experiment = filteredArgv[1]
	kernels = [os.path.splitext(x)[0] for x in os.listdir(os.path.join("csvs", experiment))] if 2 == len(filteredArgv) else filteredArgv[2:]

	if "pareto" == command:
		pareto(options, experiment, kernels)
	elif "prepare" == command:
		prepare(options, experiment, kernels)
	elif "decide" == command:
		decide(options, experiment, kernels)
	else:
		raise RuntimeError("Invalid command received: {}".format(command))
