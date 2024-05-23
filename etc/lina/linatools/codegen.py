#!/usr/bin/env python3


import json, os, re, shutil, subprocess, sys
from run import Configurator, parseJsonFiles


# The code parsed by parseCode might have an unroll factor of 1, which actually means no unroll explored (i.e. factor 1)
# This function below readjusts whenever unroll is considered 0, to be actually 1
def max1(val):
	return max(1, int(val))


def parseCodeGenJsonFiles(experiment, k, baseExpRelPath=""):
	jsonContent = None
	jsonPath = os.path.join("sources", experiment, k, "{}.codegen.json".format(k))

	if "" == baseExpRelPath:
		with open(jsonPath, "r") as jsonF:
			jsonContent = json.loads(jsonF.read())
	else:
		patchJsonContent = None

		if os.path.exists(jsonPath):
			with open(jsonPath, "r") as jsonF:
				patchJsonContent = json.loads(jsonF.read())

		with open(os.path.join("sources", experiment, k, baseExpRelPath, "{}.codegen.json".format(k)), "r") as jsonF:
			jsonContent = json.loads(jsonF.read())

			if patchJsonContent is not None:
				Configurator.patch(jsonContent, patchJsonContent)

	return jsonContent


class CodeGen(object):
	_codeRegex = re.compile(r"([pf])(\d*.?\d+)((_l\d+\.\d+\.\d+\.\d+)+)((_a[^\.]+\.[^\.]+\.\d+)*)")
	_loopsRegex = r"(_l\d+\.\d+\.\d+\.\d+)"
	_arraysRegex = r"(_a[^\.]+\.[^\.]+\.\d+)"
	_loopRegex = re.compile(r"_l(\d+)\.(\d+)\.(\d+)\.(\d+)")
	_arrayRegex = re.compile(r"_a([^\.]+)\.([^\.]+)\.(\d+)")
	_clkIDMap = {
		"zcu104": {
			"75.0": "0",
			"100.0": "1",
			"150.0": "2",
			"200.0": "3",
			"300.0": "4",
			"400.0": "5",
			"600.0": "6"
		}
	}
	_ddrDataBusWidth = {
		"zcu104": 16
	}

	def parseCode(f):
		decoded = CodeGen._codeRegex.match(f)
		pOrF = decoded.group(1)
		perFreq = decoded.group(2)
		loops = decoded.group(3)
		arrays = decoded.group(5)
		loopsDecoded = re.findall(CodeGen._loopsRegex, loops)
		arraysDecoded = re.findall(CodeGen._arraysRegex, arrays)

		code = {}
		if "f" == pOrF:
			code["frequency"] = perFreq
		elif "p" == pOrF:
			code["period"] = perFreq
		else:
			raise RuntimeError("Invalid code found: {}".format(f))

		code["loops"] = []
		for l in loopsDecoded:
			loopDecoded = CodeGen._loopRegex.match(l)
			code["loops"].append({"id": loopDecoded.group(1), "depth": loopDecoded.group(2), "pipelining": loopDecoded.group(3), "unrolling": loopDecoded.group(4)})

		code["arrays"] = []
		for a in arraysDecoded:
			arrayDecoded = CodeGen._arrayRegex.match(a)
			code["arrays"].append({"name": arrayDecoded.group(1), "type": arrayDecoded.group(2), "factor": arrayDecoded.group(3)})

		return code

	def _assembleMultiplyExpression(factor, increment):
		if isinstance(factor, int) and 1 == int(factor):
			if isinstance(increment, int):
				return "{}".format(increment)
			else:
				return "({})".format(increment)
		elif isinstance(increment, int) and 1 == int(increment):
			if isinstance(factor, int):
				return "{}".format(factor)
			else:
				return "({})".format(factor)
		else:
			return "({} * {})".format(factor, increment)

	def _transformManual(experiment, kernel, code, rbwUnroll, explVec, rbwAuto, srcFolder, dstFolder, jsonContent, cgJsonContent):
		# XXX Notice that codegen works in-place if both srcFolder and dstFolder are None
		_srcFolder = os.path.join("outcomes", experiment, kernel, code, "vivado") if srcFolder is None else srcFolder
		_dstFolder = os.path.join("outcomes", experiment, kernel, code, "vivado") if dstFolder is None else dstFolder

		# Create the code for this manual transformation variant
		variantCode = "rbwUnroll{}_explVec{}".format(int(rbwUnroll), int(explVec))
		print("[codegen] Code for this variant: {}".format(variantCode))

		if not (rbwUnroll or explVec):
			raise RuntimeError("At least rbwUnroll or explVec must be set for this internal function")

		# Get the repository folder, or complain if not set
		if "CDG_ROOTD" not in os.environ:
			raise RuntimeError("$CDG_ROOTD is not set (did you forget to source setenv.sh?")
		_repoFolder = os.path.join(os.environ["CDG_ROOTD"], experiment, kernel, code, variantCode)
		print("[codegen] Source folder for this variant: {}".format(_srcFolder))
		print("[codegen] Physical location of the files: {}".format(_repoFolder))
		print("[codegen]           Symlink of the files: {}".format(_dstFolder))

		parsedCode = CodeGen.parseCode(code)
		freqPerStr = parsedCode["frequency"] if "frequency" in parsedCode else "{:.1f}".format(1000.0 / float(parsedCode["period"]))

		# Some sanity check will be performed along the way for auto-rbw
		if rbwAuto:
			# XXX This is considering that there is only one loop nest on the kernel function
			innerDepth = len(parsedCode["loops"])
			for lop in parsedCode["loops"]:
				if innerDepth == int(lop["depth"]):
					continue

				if max1(lop["unrolling"]) > 1 or "1" == lop["pipelining"]:
					raise RuntimeError("auto-rbw mode is set and unroll/pipeline was requested for other than the innermost loop. Currently unsupported")

		bankFlags = ""
		if "banking" in jsonContent and jsonContent["banking"]:
			bankFlags = "--max_memory_ports {} {}".format(
				kernel, " ".join(["--sp {0}_1.m_axi_gmem{1}:bank{1}".format(kernel, x) for x in range(len(jsonContent["arrays"]))])
			)

		# Patches may be activated through the codegen json file. Otherwise, standard values are applied
		patches = {
			"en-expl-unroll-writeback-loops": False,
			"ds-readwrite-loops-pipeline": False
		}
		if "patches" in cgJsonContent:
			for patch in cgJsonContent["patches"]:
				patches[patch] = cgJsonContent["patches"][patch]
		# Some flags might be disabled due to other optimisations
		if patches["en-expl-unroll-writeback-loops"]:
			if "banking" in jsonContent and jsonContent["banking"]:
				print("[codegen] Patch \"en-expl-unroll-writeback-loops\" was enabled")
			else:
				print("[codegen] WARNING: Could not enable \"en-expl-unroll-writeback-loops\" because DDR banking is disabled")
				print("[codegen]          Please search for INFO-001 on the codegen's source file for more info")
				patches["en-expl-unroll-writeback-loops"] = False
		if patches["ds-readwrite-loops-pipeline"]:
			print("[codegen] Patch \"ds-readwrite-loops-pipeline\" was enabled")

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

		# For all files listed in the clsource and makefile tags of the codegen json file, codegen reads
		# from _srcFolder and writes to _dstFolder. They can be the same place (i.e. in-place transform),
		# however this could be nasty, one file might be transformed to a soft-link to the repository! Either way...
		# If manual transformations are enabled (which is the case for this function), codegen
		# will generate all files in _repoFolder instead of _dstFolder, and _dstFolder will only receive
		# soft-links to the _repoFolder files. If the files in _repoFolder already exists, only the symlinks
		# are created.
		# This repository of manual transformations is physically located and generated by codegen
		# in the path defined by the $CDG_ROOTD environment variable. This variable must be set.
		#
		# NOTE: Please note that the files are physically located in the $CDG_ROOTD folder. If more
		# than one project in your DSE hierarchy (e.g. using cirith) uses a same physical file from
		# $CDG_ROOTD, modifying this file will affect all projects, since only soft links are generated.
		# If you want to create any differentation between the manual projects, I suggest you change
		# the name of the kernel's source folder (e.g. base_1, base_2 or whatevs).
		#
		# NOTE: Also note that each project is located in a folder within $CDG_ROOTD in the following
		# pattern: $CDG_ROOTD/<experiment>/<kernel>/<code>/rbwUnrollX_explVecY, where <experiment>,
		# <kernel> are self-explanatory, <code> is the design point code (e.g. f200_l0.1.0.0_...),
		# and X and Y are the design knobs for the read-before-write unroll transformation (rbwUnroll)
		# and explicit vectorisation (explVec). X = 1 or Y = 1 means enabled, 0 otherwise. This means
		# that when Lina parameters are varied, it will not be differentiable from the code repository
		# side. If you want to have 2 unique manual transformations that use the same <code>, for example
		# one using repeated points with Lina and another not, please use different <experiment>s to isolate.
		if os.path.exists(_repoFolder):
			print("[codegen] Variant folder found. Reusing it...")
		else:
			print("[codegen] WARNING: variant folder was not found. Generating folders...")
			os.makedirs(_repoFolder) 

			# Open the vivado Makefile and replace the keywords
			# XXX This section is similar to the logic of transform(). Therefore most of its comments were removed
			#     Comments left are relevant to this section only

			kwords = {
				"<CLKID>": CodeGen._clkIDMap[jsonContent["platform"]][freqPerStr],
				"<PLATFORM>": jsonContent["platform"],
				"<FREQ>": "{}MHz".format(freqPerStr),
				"<BANKFLAGS>": bankFlags
			}

			print("[codegen] Applying \"makefile\" file transforms...")
			for mkf in mkfList:
				mkfContent = None
				with open(os.path.join(_srcFolder, mkf)) as mkfF:
					mkfContent = mkfF.read()
	
				for kw in kwords:
					mkfContent = mkfContent.replace(kw, kwords[kw])

				if not os.path.exists(os.path.dirname(os.path.join(_repoFolder, mkf))):
					os.makedirs(os.path.dirname(os.path.join(_repoFolder, mkf)))
				with open(os.path.join(_repoFolder, mkf), "w") as mkfF:
					mkfF.write(mkfContent)

			kwords = {}
			kwords["<HEADER>"] = (
				"\t/**\n"
				"\t * Kernel automatically filled using CodeGen\n"
				"\t * This version REQUIRES manual intervention!\n"
				"\t * First of all, the line right after this comment must be deleted\n"
				"\t * in order for this code to be compilable.\n"
				"\t * This is done to prevent you from compiling without checking this code first\n"
				"\t * (remember, manual intervention required! sorry :s)\n"
				"\t * \n"
				"\t * Manual intervention required in this code:\n"
			)
			if rbwUnroll:
				kwords["<HEADER>"] += (
					"\t *\n"
					"\t * * The loops indicated with <MANUAL UNROLL FACTOR=<FACTOR>> must be manually unrolled.\n"
					"\t *   Remove the <MANUAL UNROLL FACTOR=<FACTOR>> tag, readjust the trip count on the loop\n"
					"\t *   header (i.e. divide it by the unroll factor <FACTOR>) and then manually replicate the\n"
					"\t *   loop body by the unroll factor. This should only bring significant advantages if you are\n"
					"\t *   able to place all off-chip reads in the unrolled body before all writes (i.e. code movement).\n"
					"\t *   Please check the memanalysis.rpt report generated by Lina (which is present on the root folder\n"
					"\t *   of this Vivado project) for more information about which loops are potentially infringing the\n"
					"\t *   read-before-write policy (more specifically, look for warnings WARN-002 and WARN-003).\n"
				)
			if explVec:
				kwords["<HEADER>"] += (
					"\t *\n"
					"\t * * Perform explicit vectorisation of the offchip arrays. First, read the memanalysis.rpt file\n"
					"\t *   generated by Lina which is supplied in the root of this Vivado project. Look for successful\n"
					"\t *   vectorisation attempts (more specifically, look for INFO-003 messages). For each array with\n"
					"\t *   detected vectorisation, modify the kernel argument datatypes to reflect the vectorisation value\n"
					"\t *   (e.g. int8, float4). Then, modify the code to access these vectorial values instead of the single\n"
					"\t *   elements. However, note the following: in order to ensure that maximum performance is reached,\n"
					"\t *   each loop iteration must read or write to an offchip array in a sequential fashion with no overlap\n"
					"\t *   or gaps (and then Vivado/SDx will detect a memory burst). If this is not possible in this kernel's\n"
					"\t *   logic, bad luck :( Lina might have traced a very lucky dataset that identified a vectorisation\n"
					"\t *   possibility. You might need to perform a single read or write to these vectorised arrays at each\n"
					"\t *   loop iteration, and then internally glue your logic to temporary variables that access these\n"
					"\t *   vectorised words.\n"
					"{}"
					"\t *   NOTE: If banking is active for this kernel, Vivado/SDx might actually automatically perform\n"
					"\t *         the vectorisation. Check the latency report of Vivado to detect if auto-vectorisation\n"
					"\t *         was performed. There is no guarantee that an automatic vectorisation will have better\n"
					"\t *         performance than your manual one (and vice-versa).\n"
					"\t *\n".format(
						"\n\t *   For better performance, read-before-write manual unrolls might also be of interest (see how to\n"
						"\t *   activate rbwUnroll in codegen).\n" if not rbwAuto else ""
					)
				)
			if rbwAuto:
				kwords["<HEADER>"] += (
					"\t *\n"
					"\t * * Auto-read-before-write unroll is active for this kernel. If if was coded correctly, you\n"
					"\t *   should see an explicitly unrolled loop below.\n"
				)
			if patches["en-expl-unroll-writeback-loops"]:
				kwords["<HEADER>"] += (
					"\t *\n"
					"\t * * \"en-expl-unroll-writeback-loops\" patch was enabled. Request for all writeback loops\n"
					"\t *   is performed. This should increase the off-chip memory bandwidth usage, drastically reducing\n"
					"\t *   the writeback latency.\n"
				)
			if patches["ds-readwrite-loops-pipeline"]:
				kwords["<HEADER>"] += (
					"\t *\n"
					"\t * * \"ds-readwrite-loops-pipeline\" patch was enabled. This disables the pipeline attribute\n"
					"\t *   for read and writeback loops.\n"
				)
			kwords["<HEADER>"] += (
				"\t * GOOD LUCK! :)\n"
				"\t */\n"
				"\t0 = 0; // Delete this line!\n"
				"\n"
			)
			kwords["<FOOTER>"] = ""

			header2 = ""
			for arr in jsonContent["arrays"]:
				if "offchip" in jsonContent and arr in jsonContent["offchip"]:
					kwords["<ARR_{}>".format(arr)] = arr
					kwords["<ARRQ_{}>".format(arr)] = "__global"

					continue
				else:
					kwords["<ARR_{}>".format(arr)] = "l{}".format(arr)
					kwords["<ARRQ_{}>".format(arr)] = "__local"

				_stillPatch = patches["en-expl-unroll-writeback-loops"]
				if jsonContent["arrays"][arr]["size"] >= CodeGen._ddrDataBusWidth[jsonContent["platform"]]:
					print("[codegen] WARNING: Data type size of array \"{}\" is >= than the selected platform's DDR bus width ({} >= {} bytes)".format(
						arr, jsonContent["arrays"][arr]["size"], CodeGen._ddrDataBusWidth[jsonContent["platform"]]
					))
					print("                   disqualifies unrolling writeback loops")
					print("                   patch \"en-expl-unroll-writeback-loops\" will be disabled")
					_stillPatch = False
				if CodeGen._ddrDataBusWidth[jsonContent["platform"]] % jsonContent["arrays"][arr]["size"] != 0:
					print("[codegen] WARNING: Data type size ({} bytes) of array \"{}\" is not aligned with the platform's DDR bus width ({} bytes)".format(
						jsonContent["arrays"][arr]["size"], arr, CodeGen._ddrDataBusWidth[jsonContent["platform"]]
					))
					print("                   disqualifies unrolling writeback loops")
					print("                   patch \"en-expl-unroll-writeback-loops\" will be disabled")
					_stillPatch = False

				dpArr = None
				for arr2 in parsedCode["arrays"]:
					if arr2["name"] == arr:
						dpArr = arr2
				if dpArr is None:
					raise RuntimeError("Array info not found in design point code")
				
				kwords["<HEADER>"] += "\t{} l{}[{}]".format(jsonContent["arrays"][arr]["type"], arr, jsonContent["arrays"][arr]["words"])
				if "none" == dpArr["type"]:
					kwords["<HEADER>"] += ";\n"
				elif "complete" == dpArr["type"]:
					kwords["<HEADER>"] += " __attribute__((xcl_array_partition(complete,1)));\n"
				else:
					kwords["<HEADER>"] += " __attribute__((xcl_array_partition({},{},1)));\n".format(dpArr["type"], dpArr["factor"])

				if "writeonly" not in jsonContent["arrays"][arr] or not jsonContent["arrays"][arr]["writeonly"]:
					header2 += "\t{0}for(unsigned __lbi = 0; __lbi < {1}; __lbi++) l{2}[__lbi] = {2}[__lbi];\n".format(
						"__attribute__((xcl_pipeline_loop)) " if not patches["ds-readwrite-loops-pipeline"] else "",
						jsonContent["arrays"][arr]["words"], arr
					)

				if "readonly" not in jsonContent["arrays"][arr] or not jsonContent["arrays"][arr]["readonly"]:
					kwords["<FOOTER>"] += "\t{0}{1}for(unsigned __lbi = 0; __lbi < {2}; __lbi++) {3}[__lbi] = l{3}[__lbi];\n".format(
						"__attribute__((xcl_pipeline_loop)) " if not patches["ds-readwrite-loops-pipeline"] else "",
						"__attribute__((opencl_unroll_hint({}))) ".format(
							int(CodeGen._ddrDataBusWidth[jsonContent["platform"]] / jsonContent["arrays"][arr]["size"])
						) if _stillPatch else "",
						jsonContent["arrays"][arr]["words"], arr
					)
			if len(header2) > 0:
				kwords["<HEADER>"] += "\n{}".format(header2)

			# XXX Do not write unroll attribute if auto-rbw is enabled
			for lop in parsedCode["loops"]:
				# The unroll keyword depends on the type of manual transformation requested
				unrollKeyword = "<MANUAL UNROLL FACTOR={}> ".format(max1(lop["unrolling"])) if rbwUnroll else "__attribute__((opencl_unroll_hint({}))) ".format(max1(lop["unrolling"]))

				kwords["<LOOP_{}_{}>".format(lop["id"], lop["depth"])] = "{}{}".format(
					"__attribute__((xcl_pipeline_loop)) " if "1" == lop["pipelining"] else "",
					unrollKeyword if (not rbwAuto and (max1(lop["unrolling"]) > 1)) else "",
				)

			print("[codegen] Applying \"clsource\" file transforms...")
			for src in srcList:
				srcContent = None
				with open(os.path.join(_srcFolder, src)) as srcF:
					srcContent = srcF.read()

				# Before we proceed, if auto-rbw is enabled, perform manual unroll
				if rbwAuto:
					# TODO This must be re-thought if more than one loop nest is explored!
					innerL = None
					for idx, lop in enumerate(parsedCode["loops"]):
						if len(parsedCode["loops"]) == int(lop["depth"]):
							innerL = idx

					print("[codegen] Performing explicit unroll with factor {}".format(max1(parsedCode["loops"][innerL]["unrolling"])))

					# Separate the content
					beforeLines = []
					preambleLines = []
					readLines = []
					writeLines = []
					afterLines = []
					splitState = 0
					# Iterate over the kernel file, split using the <L...> tags
					for line in srcContent.split("\n"):
						# Beginning of loop preamble found
						if "<LPREAMB>" in line:
							if "<LPREAMB>" in tokensFound:
								raise RuntimeError("More than one \"<LPREAMB>\" tag found in kernel files. Please use just one in all files!")
							tokensFound.add("<LPREAMB>")
							splitState = 1
							continue
						# Beginning of loop body found
						elif "<LBEGIN>" in line:
							if "<LBEGIN>" in tokensFound:
								raise RuntimeError("More than one \"<LBEGIN>\" tag found in kernel files. Please use just one in all files!")
							tokensFound.add("<LBEGIN>")
							splitState = 2
							continue
						# Read-write frontier found
						elif "<LFRONT>" in line:
							if "<LFRONT>" in tokensFound:
								raise RuntimeError("More than one \"<LFRONT>\" tag found in kernel files. Please use just one in all files!")
							tokensFound.add("<LFRONT>")
							splitState = 3
							continue
						# End of loop body found
						elif "<LEND>" in line:
							if "<LEND>" in tokensFound:
								raise RuntimeError("More than one \"<LEND>\" tag found in kernel files. Please use just one in all files!")
							tokensFound.add("<LEND>")
							splitState = 4
							continue

						if 0 == splitState:
							beforeLines.append(line)
						elif 1 == splitState:
							preambleLines.append(line)
						elif 2 == splitState:
							readLines.append(line)
						elif 3 == splitState:
							writeLines.append(line)
						elif 4 == splitState:
							afterLines.append(line)

					# Only proceed if any tag was found
					if len(tokensFound) > 0:
						# All tags should appear at once
						if len(tokensFound) != 4:
							raise RuntimeError("<LPREAMB> ... <LBEGIN> ... <LFRONT> ... <LEND> should all be in a single file")

						newContent = list(beforeLines)
						# Explicitly unroll preamble lines
						for factor in range(max1(parsedCode["loops"][innerL]["unrolling"])):
							for line in preambleLines:
								newContent.append(
									line.replace("<LITER>", "({}{})".format("{} * ".format(factor + 1) if factor > 0 else "", cgJsonContent["autorbw"]["incr"]))
										.replace("<LCTR>", "{}".format(factor))
								)
						# Explicitly unroll read lines
						for factor in range(max1(parsedCode["loops"][innerL]["unrolling"])):
							for line in readLines:
								newContent.append(
									line.replace("<LITER>", CodeGen._assembleMultiplyExpression(factor + 1, cgJsonContent["autorbw"]["incr"]))
										.replace("<LCTR>", "{}".format(factor))
								)
						# Explicitly unroll write lines
						for factor in range(max1(parsedCode["loops"][innerL]["unrolling"])):
							for line in writeLines:
								newContent.append(
									line.replace("<LITER>", CodeGen._assembleMultiplyExpression(factor + 1, cgJsonContent["autorbw"]["incr"]))
										.replace("<LCTR>", "{}".format(factor))
								)
						newContent += afterLines
						# Update the source file finally
						srcContent = "\n".join(newContent)

					# Also add the loop increment tag
					kwords["<LINCR>"] = CodeGen._assembleMultiplyExpression(max1(parsedCode["loops"][innerL]["unrolling"]), cgJsonContent["autorbw"]["incr"])

				for kw in kwords:
					srcContent = srcContent.replace(kw, kwords[kw])

				if not os.path.exists(os.path.dirname(os.path.join(_repoFolder, src))):
					os.makedirs(os.path.dirname(os.path.join(_repoFolder, src)))
				with open(os.path.join(_repoFolder, src), "w") as srcF:
					srcF.write(srcContent)

				# Open this file for edit using your favourite editor (only if $EDITOR is set)
				if "EDITOR" in os.environ and os.environ["EDITOR"] in ["nano", "vim", "vi", "emacs", "mousepad", "gedit", "kedit", "edit"]:
					print("[codegen] Opening file \"{}\" for editing/viewing...".format(os.path.join(_repoFolder, src)))
					subprocess.run([os.environ["EDITOR"], os.path.join(_repoFolder, src)])
				else:
					print("[codegen] No $EDITOR variable set for default text editor (or editor not recognised). Won't open file \"{}\"".format(os.path.join(_repoFolder, src)))

			print("[codegen] Copying the Lina memory model analysis report...")
			shutil.copy2(
				os.path.join("workspace", experiment, kernel, code, "{}_loop-{}_memanalysis.rpt".format(kernel, jsonContent["loopid"])),
				os.path.join(_repoFolder, "memanalysis.rpt")
			)

			print("[codegen] Project created! Physical location: {}".format(_repoFolder))
			print("[codegen] Please perform your manual transformations there before synthesising it!")

		print("[codegen] Now generating the soft-links in the destination folder...")
		for fil in [*mkfList, *srcList, "memanalysis.rpt"]:
			if not os.path.exists(os.path.dirname(os.path.join(_dstFolder, fil))):
				os.makedirs(os.path.dirname(os.path.join(_dstFolder, fil)))
			elif os.path.exists(os.path.join(_dstFolder, fil)) and "CDG_REMOVE_FILES" in os.environ and "yes" == os.environ["CDG_REMOVE_FILES"]:
				print("[codegen] WARNING: An existing file was found in the destination point of the soft-link {}. Removing it due to CDG_REMOVE_FILES=yes...".format(os.path.join(_dstFolder, fil)))
				os.remove(os.path.join(_dstFolder, fil))
			os.symlink(os.path.join(_repoFolder, fil), os.path.join(_dstFolder, fil))

		print("[codegen] Soft-linking done! You can check the files now")


	def transform(experiment, kernel, code, rbwUnroll=None, explVec=None, srcFolder=None, dstFolder=None):
		# Read the json config files (following inheritance)
		jsonContent, baseExpRelPath = parseJsonFiles(experiment, kernel)
		cgJsonContent = parseCodeGenJsonFiles(experiment, kernel, baseExpRelPath)

		# Set the internal transformation flags according to arguments
		_rbwUnroll = ("rbwunroll" in cgJsonContent and cgJsonContent["rbwunroll"]) if rbwUnroll is None else rbwUnroll
		_explVec = ("explvec" in cgJsonContent and cgJsonContent["explvec"]) if explVec is None else explVec

		# If the json file contains the "autorbw" tag, this will make CodeGen enter a special mode where it is considered
		# that the kernel is written in a specific format for automatic explicit unroll with read before writes
		# This only works if the kernel is written in this format!
		# TODO describe the format. Tags:
		# TODO <LINCR> : the value that increments the for loop. This format considers a canonical loop form
		# TODO           for(i = 0; i < XXXX; i += <LINCR>)
		# TODO <LPREAMB>: place at beginning of loop body. Put here variables declarations
		# TODO <LBEGIN>: place at beginning of the read segment of the loop body (this mode only supports one loop level!)
		# TODO <LEND>: place at end of loop body
		# TODO <LFRONT>: place at the frontier. Before this, only DDR reads, after this, only DDR writes
		# TODO <LITER>: ID that codegen will generate used to differentiate each unrolled loop body. Use this to annotate varibles
		# TODO <LCTR>: iteration "counter" during unrolling. Use this to annotate and differentiate variables
		_rbwAuto = "autorbw" in cgJsonContent
		if _rbwAuto:
			# auto-rbw should not be enabled together with rbwUnroll (why manual if it is automatic already?)
			if _rbwUnroll:
				raise RuntimeError("RBWUNROLL is enabled together with \"autorbw\" tag in the json file. Both are mutually exclusive")
			print("[codegen] NOTE: \"autorbw\" tag found in cfg file. Switching to auto-rbw mode, expecting kernel files to be in the required format")

		# If any manual optimisation is on, run special transform
		if _rbwUnroll or _explVec:
			CodeGen._transformManual(experiment, kernel, code, _rbwUnroll, _explVec, _rbwAuto, srcFolder, dstFolder, jsonContent, cgJsonContent)
		# If not, run usual transformation
		else:
			# Apply final transformation

			# XXX Notice that codegen works in-place if both srcFolder and dstFolder are None
			_srcFolder = os.path.join("outcomes", experiment, kernel, code, "vivado") if srcFolder is None else srcFolder
			_dstFolder = os.path.join("outcomes", experiment, kernel, code, "vivado") if dstFolder is None else dstFolder

			# Parse the code
			parsedCode = CodeGen.parseCode(code)
			freqPerStr = parsedCode["frequency"] if "frequency" in parsedCode else "{:.1f}".format(1000.0 / float(parsedCode["period"]))

			# Some sanity check will be performed along the way for auto-rbw
			if _rbwAuto:
				# XXX This is considering that there is only one loop nest on the kernel function
				innerDepth = len(parsedCode["loops"])
				for lop in parsedCode["loops"]:
					# Ignore innermost loop
					if innerDepth == int(lop["depth"]):
						continue

					# We support loop nests with auto-rbw, but only the innermost loop should be optimised
					if max1(lop["unrolling"]) > 1 or "1" == lop["pipelining"]:
						raise RuntimeError("auto-rbw mode is set and unroll/pipeline was requested for other than the innermost loop. Currently unsupported")

			# Get banking flag
			bankFlags = ""
			if "banking" in jsonContent and jsonContent["banking"]:
				bankFlags = "--max_memory_ports {} {}".format(
					kernel, " ".join(["--sp {0}_1.m_axi_gmem{1}:bank{1}".format(kernel, x) for x in range(len(jsonContent["arrays"]))])
				)

			# Patches may be activated through the codegen json file. Otherwise, standard values are applied
			patches = {
				"en-expl-unroll-writeback-loops": False,
				"ds-readwrite-loops-pipeline": False
			}
			if "patches" in cgJsonContent:
				for patch in cgJsonContent["patches"]:
					patches[patch] = cgJsonContent["patches"][patch]
			# Some flags might be disabled due to other optimisations
			# XXX [INFO-001] "en-expl-unroll-writeback-loops" should only bring benefit if Vivado/SDx performs auto-vectorisation,
			# which so far appears to happen only then DDR banking is active. Without auto-vectorisation, this unroll
			# might actually make Vivado lose its orientation and make the writeback loop even worse. Therefore if banking
			# is disabled, we also disable this patch
			if patches["en-expl-unroll-writeback-loops"]:
				if "banking" in jsonContent and jsonContent["banking"]:
					print("[codegen] Patch \"en-expl-unroll-writeback-loops\" was enabled")
				else:
					print("[codegen] WARNING: Could not enable \"en-expl-unroll-writeback-loops\" because DDR banking is disabled")
					print("[codegen]          Please search for INFO-001 on the codegen's source file for more info")
					patches["en-expl-unroll-writeback-loops"] = False
			if patches["ds-readwrite-loops-pipeline"]:
				print("[codegen] Patch \"ds-readwrite-loops-pipeline\" was enabled")

			# Open the vivado Makefile and replace the keywords

			kwords = {
				# <CLKID>: will be filled with the CLK id respective to the selected clock for this point 
				"<CLKID>": CodeGen._clkIDMap[jsonContent["platform"]][freqPerStr],
				# <PLATFORM>: will be filled with the platform code (e.g. zcu104)
				"<PLATFORM>": jsonContent["platform"],
				# <FREQ>: will be filled with the frequency in MHz
				"<FREQ>": "{}MHz".format(freqPerStr),
				# <BANKFLAGS>: the bank flags to be passed to XOCC -c step
				"<BANKFLAGS>": bankFlags
			}

			# Multiple files can have the keywords replaced treated as "makefile"s
			mkfList = []
			if isinstance(cgJsonContent["makefile"], str):
				mkfList.append(cgJsonContent["makefile"])
			else:
				mkfList = cgJsonContent["makefile"]

			for mkf in mkfList:
				# XXX Notice that read is separated from write because in-place modification is possible (i.e. src == dst)!
				# Read the file content
				mkfContent = None
				with open(os.path.join(_srcFolder, mkf)) as mkfF:
					mkfContent = mkfF.read()

				# Do the replacements!
				for kw in kwords:
					mkfContent = mkfContent.replace(kw, kwords[kw])

				# Write back to the makefile (generating the folder hierarchy)
				if not os.path.exists(os.path.dirname(os.path.join(_dstFolder, mkf))):
					os.makedirs(os.path.dirname(os.path.join(_dstFolder, mkf)))
				with open(os.path.join(_dstFolder, mkf), "w") as mkfF:
					mkfF.write(mkfContent)

			# Open the kernel source files and replace the keywords
			# <HEADER>  : place right after kernel function header. Here are placed array declarations,
			#             off-chip memory buffer loops and partitioning info. Also comments about manual
			#             efforts are also placed here!
			# <LOOP_X_Y>: place right before every "for/while" keyword. X means loop sequential ID (start at 0),
			#             and Y means loop depth (start at 1, where 1 is topmost level)
			# <ARR_X>   : replace every mention to the off-chip arrays in the kernel with this tag, where X is
			#             the array name
			# <ARRQ_X>  : this tag will contain the correct memory space qualifier for the array X, either global
			#             or local
			# <FOOTER>  : place right before the closing brackeet of the kernel. Here are placed final off-chip
			#             memory transfers

			kwords = {}
			# Initialise header and footer
			kwords["<HEADER>"] = (
				"\t/**\n"
				"\t * Kernel automatically filled using CodeGen\n"
				"\t * This version requires no manual intervention\n"
			)
			if _rbwAuto:
				kwords["<HEADER>"] += (
					"\t *\n"
					"\t * * Auto-read-before-write unroll is active for this kernel. If if was coded correctly, you\n"
					"\t *   should see an explicitly unrolled loop below.\n"
				)
			if patches["en-expl-unroll-writeback-loops"]:
				kwords["<HEADER>"] += (
					"\t *\n"
					"\t * * \"en-expl-unroll-writeback-loops\" patch was enabled. Request for all writeback loops\n"
					"\t *   is performed. This should increase the off-chip memory bandwidth usage, drastically reducing\n"
					"\t *   the writeback latency.\n"
				)
			if patches["ds-readwrite-loops-pipeline"]:
				kwords["<HEADER>"] += (
					"\t *\n"
					"\t * * \"ds-readwrite-loops-pipeline\" patch was enabled. This disables the pipeline attribute\n"
					"\t *   for read and writeback loops.\n"
				)
			kwords["<HEADER>"] += (
				"\t */\n"
				"\n"
			)
			kwords["<FOOTER>"] = ""

			# Fill in array data
			header2 = ""
			for arr in jsonContent["arrays"]:
				# Substitute the array tags according to its on/off-chip access
				if "offchip" in jsonContent and arr in jsonContent["offchip"]:
					# For offchip, the array tag becomes access to global
					kwords["<ARR_{}>".format(arr)] = arr
					kwords["<ARRQ_{}>".format(arr)] = "__global"

					# We don't need to do anything else for this one
					continue
				else:
					# For onchip, the array tag becomes access to local
					kwords["<ARR_{}>".format(arr)] = "l{}".format(arr)
					kwords["<ARRQ_{}>".format(arr)] = "__local"

				_stillPatch = patches["en-expl-unroll-writeback-loops"]
				if jsonContent["arrays"][arr]["size"] >= CodeGen._ddrDataBusWidth[jsonContent["platform"]]:
					print("[codegen] WARNING: Data type size of array \"{}\" is >= than the selected platform's DDR bus width ({} >= {} bytes)".format(
						arr, jsonContent["arrays"][arr]["size"], CodeGen._ddrDataBusWidth[jsonContent["platform"]]
					))
					print("                   disqualifies unrolling writeback loops")
					print("                   patch \"en-expl-unroll-writeback-loops\" will be disabled")
					_stillPatch = False
				if CodeGen._ddrDataBusWidth[jsonContent["platform"]] % jsonContent["arrays"][arr]["size"] != 0:
					print("[codegen] WARNING: Data type size ({} bytes) of array \"{}\" is not aligned with the platform's DDR bus width ({} bytes)".format(
						jsonContent["arrays"][arr]["size"], arr, CodeGen._ddrDataBusWidth[jsonContent["platform"]]
					))
					print("                   disqualifies unrolling writeback loops")
					print("                   patch \"en-expl-unroll-writeback-loops\" will be disabled")
					_stillPatch = False

				# Get design point info for this array
				dpArr = None
				for arr2 in parsedCode["arrays"]:
					if arr2["name"] == arr:
						dpArr = arr2
				if dpArr is None:
					raise RuntimeError("Array info not found in design point code")
					
				kwords["<HEADER>"] += "\t{} l{}[{}]".format(jsonContent["arrays"][arr]["type"], arr, jsonContent["arrays"][arr]["words"])
				if "none" == dpArr["type"]:
					kwords["<HEADER>"] += ";\n"
				elif "complete" == dpArr["type"]:
					kwords["<HEADER>"] += " __attribute__((xcl_array_partition(complete,1)));\n"
				else:
					kwords["<HEADER>"] += " __attribute__((xcl_array_partition({},{},1)));\n".format(dpArr["type"], dpArr["factor"])

				if "writeonly" not in jsonContent["arrays"][arr] or not jsonContent["arrays"][arr]["writeonly"]:
					header2 += "\t{0}for(unsigned __lbi = 0; __lbi < {1}; __lbi++) l{2}[__lbi] = {2}[__lbi];\n".format(
						"__attribute__((xcl_pipeline_loop)) " if not patches["ds-readwrite-loops-pipeline"] else "",
						jsonContent["arrays"][arr]["words"], arr
					)

				if "readonly" not in jsonContent["arrays"][arr] or not jsonContent["arrays"][arr]["readonly"]:
					kwords["<FOOTER>"] += "\t{0}{1}for(unsigned __lbi = 0; __lbi < {2}; __lbi++) {3}[__lbi] = l{3}[__lbi];\n".format(
						"__attribute__((xcl_pipeline_loop)) " if not patches["ds-readwrite-loops-pipeline"] else "",
						"__attribute__((opencl_unroll_hint({}))) ".format(
							int(CodeGen._ddrDataBusWidth[jsonContent["platform"]] / jsonContent["arrays"][arr]["size"])
						) if _stillPatch else "",
						jsonContent["arrays"][arr]["words"], arr
					)
			if len(header2) > 0:
				kwords["<HEADER>"] += "\n{}".format(header2)

			# Now write loop directives
			# XXX Do not write unroll attribute if auto-rbw is enabled
			for lop in parsedCode["loops"]:
				kwords["<LOOP_{}_{}>".format(lop["id"], lop["depth"])] = "{}{}".format(
					"__attribute__((xcl_pipeline_loop)) " if "1" == lop["pipelining"] else "",
					"__attribute__((opencl_unroll_hint({}))) ".format(max1(lop["unrolling"])) if (not _rbwAuto and (max1(lop["unrolling"]) > 1)) else "",
				)

			# Multiple files can have the keywords replaced treated as "clsources"s
			srcList = []
			if isinstance(cgJsonContent["clsource"], str):
				srcList.append(cgJsonContent["clsource"])
			else:
				srcList = cgJsonContent["clsource"]

			# The <LBEGIN> ... <LFRONT> ... <LEND> triad must be used all in just a single file and just once!
			tokensFound = set()
			for src in srcList:
				# XXX Notice that read is separated from write because in-place modification is possible (i.e. src == dst)!
				# Read the file content
				srcContent = None
				with open(os.path.join(_srcFolder, src)) as srcF:
					srcContent = srcF.read()

				# Before we proceed, if auto-rbw is enabled, perform manual unroll
				if _rbwAuto:
					# TODO This must be re-thought if more than one loop nest is explored!
					innerL = None
					for idx, lop in enumerate(parsedCode["loops"]):
						if len(parsedCode["loops"]) == int(lop["depth"]):
							innerL = idx

					# Separate the content
					beforeLines = []
					preambleLines = []
					readLines = []
					writeLines = []
					afterLines = []
					splitState = 0
					# Iterate over the kernel file, split using the <L...> tags
					for line in srcContent.split("\n"):
						# Beginning of loop preamble found
						if "<LPREAMB>" in line:
							if "<LPREAMB>" in tokensFound:
								raise RuntimeError("More than one \"<LPREAMB>\" tag found in kernel files. Please use just one in all files!")
							tokensFound.add("<LPREAMB>")
							splitState = 1
							continue
						# Beginning of loop body found
						elif "<LBEGIN>" in line:
							if "<LBEGIN>" in tokensFound:
								raise RuntimeError("More than one \"<LBEGIN>\" tag found in kernel files. Please use just one in all files!")
							tokensFound.add("<LBEGIN>")
							splitState = 2
							continue
						# Read-write frontier found
						elif "<LFRONT>" in line:
							if "<LFRONT>" in tokensFound:
								raise RuntimeError("More than one \"<LFRONT>\" tag found in kernel files. Please use just one in all files!")
							tokensFound.add("<LFRONT>")
							splitState = 3
							continue
						# End of loop body found
						elif "<LEND>" in line:
							if "<LEND>" in tokensFound:
								raise RuntimeError("More than one \"<LEND>\" tag found in kernel files. Please use just one in all files!")
							tokensFound.add("<LEND>")
							splitState = 4
							continue

						if 0 == splitState:
							beforeLines.append(line)
						elif 1 == splitState:
							preambleLines.append(line)
						elif 2 == splitState:
							readLines.append(line)
						elif 3 == splitState:
							writeLines.append(line)
						elif 4 == splitState:
							afterLines.append(line)

					# Only proceed if any tag was found
					if len(tokensFound) > 0:
						# All tags should appear at once
						if len(tokensFound) != 4:
							raise RuntimeError("<LPREAMB> ... <LBEGIN> ... <LFRONT> ... <LEND> should all be in a single file")

						newContent = list(beforeLines)
						# Explicitly unroll preamble lines
						for factor in range(max1(parsedCode["loops"][innerL]["unrolling"])):
							for line in preambleLines:
								newContent.append(
									line.replace("<LITER>", CodeGen._assembleMultiplyExpression(factor, cgJsonContent["autorbw"]["incr"]))
										.replace("<LCTR>", "{}".format(factor))
								)
						# Explicitly unroll read lines
						for factor in range(max1(parsedCode["loops"][innerL]["unrolling"])):
							for line in readLines:
								newContent.append(
									line.replace("<LITER>", CodeGen._assembleMultiplyExpression(factor, cgJsonContent["autorbw"]["incr"]))
										.replace("<LCTR>", "{}".format(factor))
								)
						# Explicitly unroll write lines
						for factor in range(max1(parsedCode["loops"][innerL]["unrolling"])):
							for line in writeLines:
								newContent.append(
									line.replace("<LITER>", CodeGen._assembleMultiplyExpression(factor, cgJsonContent["autorbw"]["incr"]))
										.replace("<LCTR>", "{}".format(factor))
								)
						newContent += afterLines
						# Update the source file finally
						srcContent = "\n".join(newContent)

					# Also add the loop increment tag
					kwords["<LINCR>"] = CodeGen._assembleMultiplyExpression(max1(parsedCode["loops"][innerL]["unrolling"]), cgJsonContent["autorbw"]["incr"])

				# Do the replacements!
				for kw in kwords:
					srcContent = srcContent.replace(kw, kwords[kw])

				# Write back to the makefile (generating the folder hierarchy)
				if not os.path.exists(os.path.dirname(os.path.join(_dstFolder, src))):
					os.makedirs(os.path.dirname(os.path.join(_dstFolder, src)))
				with open(os.path.join(_dstFolder, src), "w") as srcF:
					srcF.write(srcContent)


if "__main__" == __name__:
	if len(sys.argv) != 8:
		sys.stderr.write("Usage: {} EXPERIMENT KERNEL CODE RBWUNROLL EXPLVEC SRCFOLDER DSTFOLDER\n".format(sys.argv[0]))
		exit(-1)

	experiment = sys.argv[1]
	kernel = sys.argv[2]
	code = sys.argv[3]
	rbwUnroll = int(sys.argv[4]) != 0
	explVec = int(sys.argv[5]) != 0
	srcFolder = sys.argv[6]
	dstFolder = sys.argv[7]

	CodeGen.transform(experiment, kernel, code, rbwUnroll, explVec, srcFolder, dstFolder)
