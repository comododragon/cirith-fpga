# cirith-ungol (FPGA)

A framework to explore, synthesise, and profile FPGA projects using the
[Lina](https://github.com/comododragon/linaii) estimator.


## Table of Contents

1. [Licence](#licence)
	1. [Parboil Benchmark Licence](#parboil-benchmark-licence)
1. [Publications](#publications)
1. [Basic Concepts](#basic-concepts)
1. [Preparation](#preparation)
1. [Quick Hands-on](#quick-hands-on)
1. [Usage](#usage)
	1. [Commands](#commands)
		1. [generate](#generate)
		1. [explore](#explore)
		1. [decide](#decide)
		1. [synth](#synth)
		1. [expand](#expand)
		1. [stat](#stat)
	1. [Options](#options)
		1. [-h | --help](#-h----help)
		1. [-s SRC | --source=SRC](#-s-src----sourcesrc)
		1. [-e EXP | --experiment=EXP](#-e-exp----experimentexp)
		1. [-f | --force](#-f----force)
		1. [-F | --force-really](#-f----force-really)
		1. [-m MASK | --mask=MASK](#-m-mask----maskmask)
		1. [-k KNOB | --knob=KNOB](#-k-knob----knobknob)
1. [Environment Variables](#environment-variables)
1. [Exploration Variants](#exploration-variants)
1. [Post-decision Code Transformation](#post-decision-code-transformation)
1. [Current Available Kernels](#current-available-kernels)
1. [Framework Structure](#framework-structure)
	1. [sources](#sources)
		1. [lina](#lina)
		1. [vivado](#vivado)
		1. [Makefile](#makefile)
		1. [histo.codegen.json](#histocodegenjson)
		1. [histo.json](#histojson)
		1. [histo.paretogen.json](#histoparetogenjson)
	1. [etc](#etc)
		1. [configs](#configs)
			1. [Automatic Variables](#automatic-variables)
			1. [File Format](#file-format)
		1. [lina](#lina-1)
			1. [ParetoGen](#paretogen)
			1. [CodeGen](#codegen)
		1. [parboil](#parboil)
	1. [experiments](#experiments)
		1. [workspace](#workspace)
		1. [sources](#sources-1)
		1. [outcomes](#outcomes)
		1. [knob0, knob1, ...](#knob0-knob1-)
		1. [csvs](#csvs)
		1. [codegen.py, paretogen.py, run.py, utils, ...](#codegenpy-paretogenpy-runpy-utils-)
1. [Acknowledgments](#acknowledgments)


## Licence

This tool is shared under GPL-3.0 licence.

Please check Lina's repository for description on its licence.

### Parboil Benchmark Licence

This repository includes modified parts of the Parboil benchmark. It also require some files to be
downloaded from Parboil's website. See `./etc/parboil/README`.

Parboil is shared under the Illinois Open Source License, reproduced below:
```
-------------------------------------------------------------------------------------------------
Illinois Open Source License
University of Illinois/NCSA
Open Source License

(C) Copyright 2010 The Board of Trustees of the University of Illinois. All rights reserved.

Developed by:
IMPACT Research Group
University of Illinois, Urbana Champaign
-------------------------------------------------------------------------------------------------

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and
associated documentation files (the "Software"), to deal with the Software without restriction,
including without limitation the rights to use, copy, modify, merge, publish, distribute,
sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

Redistributions of source code must retain the above copyright notice, this list of conditions and
the following disclaimers.

Redistributions in binary form must reproduce the above coptright notice, this list of conditions
and the following disclaimers in the documentation and/or other materials provided with the
distribution.

Neither the names of the IMPACT Research Group, the University of Illinois, nor the names of its
contributors may be used to endorse or promote products derived from this Software without specific
prior written permission. THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE CONTRIBUTORS OR COPYRIGHT HOLDERS BE LIABLE FOR
ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS WITH THE SOFTWARE.
```


## Publications

Cirith and Lina Mark 2 are referenced in the latest paper:

* A. Bannwart Perina, J. Becker and V. Bonato (in press), "Memory Aware Design Optimisation for High-Level Synthesis," in Journal of Signal Processing Systems, 2024, doi: **IN PRESS**.
	* In press, please wait for further updates.

Details can also be found in my PhD thesis:

* A. Bannwart Perina, "Lina: a fast design optimisation tool for software-based FPGA programming," doctoral dissertation, Universidade de São Paulo, 2022, doi: [10.11606/T.55.2022.tde-23082022-101507](https://doi.org/10.11606/T.55.2022.tde-23082022-101507).

### Other Related Publications

Lina Mark 1:

* A. Bannwart Perina, A. Silitonga, J. Becker and V. Bonato, "Fast Resource and Timing Aware Design Optimisation for High-Level Synthesis," in IEEE Transactions on Computers, 2021, doi: [10.1109/TC.2021.3112260](https://doi.org/10.1109/TC.2021.3112260).

A much simpler validation with an early version of Lina was presented in our FPT-2019 paper:

* A. Bannwart Perina, J. Becker and V. Bonato, "Lina: Timing-Constrained High-Level Synthesis Performance Estimator for Fast DSE," 2019 International Conference on Field-Programmable Technology (ICFPT), 2019, pp. 343-346, doi: [10.1109/ICFPT47387.2019.00063](https://doi.org/10.1109/ICFPT47387.2019.00063).

Several parts of Lina were inherited from Lin-analyzer. For more information about Lin-analyzer, please see:

* G. Zhong, A. Prakash, Y. Liang, T. Mitra and S. Niar, "Lin-Analyzer: A high-level performance analysis tool for FPGA-based accelerators," 2016 53nd ACM/EDAC/IEEE Design Automation Conference (DAC), 2016, pp. 1-6, doi: [10.1145/2897937.2898040](https://doi.org/10.1145/2897937.2898040).


## Basic Concepts

Cirith is a tool for design space exploration of kernels. There are some key words and concepts
that define its functionality:

* A **kernel** is a piece of code or function that is intended to be optimised using cirith;
* A **source** is a folder/project containing the basic files for the design space exploration of a given **kernel**. The **source** can be used to generate different explorations, considering some varying parameters;
* An **experiment** is prepared based on a **source** project, and it is used to perform the proper exploration. Multiple **experiments** can exist based on the same **source** project;
* A **setup** is a variation of an **experiment**. One **experiment** can have multiple **setups**, and they are usually related to some fine tuning on the DSE tool (e.g. lina). Each **setup** can be executed as a separate sub-exploration;
* A **knob** defines a possible code transformation to be performed after DSE;
* The **frontend** is the cirith tool itself, a Python script. Cirith provides you with basic functionality to perform DSE, profiling, etc. The hard work is done by the **backend**;
* The **backend** is all the tools and files responsible for doing the actual stuff. It is composed of many scripts, tools, source codes, etc. The integration between **frontend** and **backend** is done via [configuration files](#configs).

> **NOTE:** cirith provides some preliminar system for semi-automatic deployment to the FPGA board.
This system is not included here, but parts of this repository are partially prepared for this
logic. There is no current documentation for this part. Nonetheless, projects synthesised using
the `synth` command can still be used, they just need to be manually transferred to the board, using
for example an SD card.

> **NOTE:** cirith does not perform automatic switching between Lina with and without
`cachedaemon`. This is left as future work. As of now, two different setups of cirith (i.e. two
separate folders) are needed.


## Preparation

Ensure you have Xilinx SDSoC 2018.2 installed. Also ensure you have proper libraries for Zynq
UltraScale+ platform.

> **NOTE:** A valid SDSoC 2018.2 licence is required to synthesise the projects.

Please follow the instructions on the READMEs below to prepare all tools and files:
- `./etc/lina/README`
- `./etc/parboil/README`

With all files in place, run the command below to copy some files and create symlinks:
```
./prepare.sh
```

The script will ask if you want to assemble this repo to use the new version of Lina that includes
the cache daemon, or the "old"-new version that does not include it. Please see Lina's repository
at https://www.github.com/comododragon/linaii.git for further information. Default is the first
option.

Then, open file `./setenv.sh` and set environment variables according to your system.

Then, source `setenv.sh` to prepare the environment variables. This must be done each time a new
console is opened, prior to running `cirith`:
```
source ./setenv.sh
```

unless you put these variables on an `.rc` file or something similar.


## Quick Hands-on

Below is a quick tutorial to get you going, using `sgemm` kernel, source `zcu104`:

1. Set-up your environment according to [Preparation](#preparation);
1. Open a terminal, cd to the folder where you assembled the `cirith` repository;
1. Source `./setenv.sh`:
	```
	$ source ./setenv.sh
	```
1. Generate experiment:
	```
	$ ./cirith fpga generate sgemm
	```
1. Start exploration. It will run four setups (trace + exploration each):
	```
	$ ./cirith fpga explore sgemm
	```
1. After exploration, decide the best points:
	```
	$ ./cirith fpga decide sgemm
	```
1. Synthesise the estimated best points:
	```
	$ ./cirith fpga synth sgemm
	```

At end, there should be four synthesised projects, one for each explored setup. They will be located
at `experiments/fpga/sgemm/default/*/knob0/benchmarks/sgemm/build/sdx_zcu104_hw/zcu104/sd_card`.
The content of these folders can be copied to a microSD card and booted on the zcu104 board.

To run the application, simply call the executable with the kernel name. In this case for example:
```
$ ./sgemm
```


## Usage

```
cirith fpga COMMAND [OPTION]... KERNEL
```

### Commands

See [here](#options) to understand the options, and [here](#environment-variables) to understand
the subprocesses environments.

#### generate

Generate a new experiment from the kernel source.

* **optional OPTIONs:** `-h`, `-s`, `-e`, `-f`, `-F`, `-m`;
* **default subprocesses environment:** `lina` (`$CIR_LI_*` variables).

#### explore

Perform design space exploration on an experiment.

* **optional OPTIONs:** `-h`, `-e`, `-m`;
* **default subprocesses environment:** `lina` (`$CIR_LI_*` variables).

#### decide

Given an explored design space, decide the best point(s).

* **optional OPTIONs:** `-h`, `-e`, `-m`;
* **default subprocesses environment:** `vivado` (`$CIR_VI_*` variables).

#### synth

Synthesise the points selected with "decide".

* **optional OPTIONs:** `-h`, `-e`, `-m`, `-k`;
* **default subprocesses environment:** `vivado` (`$CIR_VI_*` variables).

#### expand

Expand a decided design point: apply some code transformations.

* **optional OPTIONs:** `-h`, `-e`, `-m`, `-k`;
* **default subprocesses environment:** `vivado` (`$CIR_VI_*` variables).

#### stat

Show information about an experiment.

* **optional OPTIONs:** `-h`, `-e`;
* **default subprocesses environment:** `none`.

### Options

#### -h | --help

Show help.

#### -s SRC | --source=SRC

Use `SRC` as the source for the experiment (folder path is `$PWD/sources/fpga/KERNEL/SRC`).

**Default is "base".**

#### -e EXP | --experiment=EXP

Use `EXP` as the destination experiment (folder path is `$PWD/experiments/fpga/KERNEL/EXP`).

**Default is "default".**

#### -f | --force

Run `COMMAND` for this `KERNEL` `EXP`, even if it ran before already (will ask for confirmation
though).

#### -F | --force-really

Run `COMMAND` for this `KERNEL` `EXP`, even if it ran before already **(NO CONFIRMATION ASKED!!!!)**

#### -m MASK | --mask=MASK

Use `MASK` to select which variants of an experiment to perform the `COMMAND`. See
[Exploration Variants](#exploration-variants) for more information and defaults.

The default value is only used for the "generate" command. All other commands will iterate over
all generated variants in the experiment if no `-m` | `--mask` is set.

#### -k KNOB | --knob=KNOB

Perform a post-exploration (actually post-decision) code transformation using knob `KNOB`. See
[Post-decision Code Transformation](#post-decision-code-transformation) for more information.

**Default is 0.**


## Environment Variables

Cirith uses custom environment variables to set up the paths to the tools like CUDA, Vivado, etc.
These variables have as prefix `CIR_XX_*`, where `XX` is the prefix (midfix?) for a specific tool.

Before running any command, cirith sets up different environments for each tool. Each `CIR_XX_*`
variable replaces the respective variable without the suffix. For example, `$CIR_XX_FOO` will
replace `$FOO` when environment for tool `XX` is selected.

Supported tools for FPGA are:

* **none:** use the standard environment;
* **vivado:** use the environment set by `$CIR_VI_*` variables;
* **lina:** use the environment set by `$CIR_LI_*` variables.

The default environment for each command is defined within each command's description. You can
force another environment by using the `environment=` option in your [configuration file](#configs).
For example:

```
[generate]
environment=vivado
```

> **HINT:** One can use the `setenv.sh` script to set these variables. Check the script's content
for further info!


## Exploration Variants

To provide more flexibility in the explorations, cirith allows for each experiment to have
different variants self-contained. Each variant usually consists of a small different setup in your
design space exploration, for example enabling or disabling certain heuristics.

One could argue that these could be considered optimisation knobs suitable for the design space
exploration itself, however usually these setups that compose each variant are not intended to be
explored for every kernel.

This variant scheme is specific to each tool and must be implemented in cirith, or can also be
disabled by providing support to just a single variant.

There is currently a single variant scheme implemented only, for lina. Each variant is identified
by a code in the form of:

```
bX.vY.kZ.pW
```

Where:

* **bX:** enable/disable on-chip buffering of all arrays. `b0` is disabled, `b1` is enabled;
* **vY:** enable/disable automatic vectorisation analysis. `v0` is disabled, `v1` is enabled;
* **kZ:** enable/disable DDR banking. `k0` is disabled, `k1` is enabled;
* **pW:** select Lina DDR scheduling policy. `p0` is conservative, `p1` is permissive.

For more details about what these mean, check [lina](https://github.com/comododragon/linaii).

As an example, to define a specific variant during cirith execution, use `-m|--mask=b0.v0.k1.p1`.

You can also select more than one. For example to select both banking policies, use
`-m|--mask=b0.v0.k01.p1`. This means that both `k0` and `k1` will be considered).

Projects in cirith should be able to differentiate between variants, usually using different
folders inside the experiment folder.

**Default value if no** `-m` **argument is supplied is:** `b0.v01.k1.p01`.

> **NOTE:** You can use mask to select more than one variant to be manipulated on a single cirith
run. However, these variants must exist (i.e. the set of variants you pass through `-m` when
running any command other than "generate" and "collect" must be a sub-set of the set you used
during the "generate" option). The "collect" command always iterates over all generated variants.


## Post-decision Code Transformation

After executing "decide" for one or more variants of an experiment, you can proceed to directly
synthesising the selected point.

However, there might be some code manipulations that can improve final performance (e.g. by
relaxing constraints of the HLS optimiser). You can activate these manipulations by using knobs.

After running "decide", you can expand one or more variants using an integer knob ID. This knob ID
is passed to the backend which your backend logic can use to implement different code
manipulations.

Each knob setup should be self-contained within a variant, at least with the files that
communicates with cirith.

In summary, an FPGA experiment is composed of one or more variants, and each variant is composed of
one or more setups via knobs.

You can only select a single knob at a time using the `-k` option.

If no code manipulation is desired, use the default knob only.

In fact if the "synth" command is executed right after "decide", the "expand" command will be
automatically executed with the default knob value of 0. This is similar to running "decide", then
"expand" with `-k|--knob=0` and then "synth" with `-k|--knob=0`.

The following knobs are currently supported with the Lina backend:

* **Knob 0:** Only standard SDx/Vivado directives are used;
* **Knob 1:** Loop body is transformed to have all offchip reads before writes (RBWUNROLL);
* **Knob 2:** Loop body is transformed to use vectorised offchip access (EXPLVEC);
	* Manual intervention is required. This knob is currently not explored;
* **Knob 3:** knobs 1 and 2 together.


## Current Available Kernels

The kernels available for exploration are: `histo`, `lbm_small`, `mri_q` and `sgemm`.


## Framework Structure

A good way to dive into details about this repository is by studying its folder structure. The
following sub-sections presents each folder and their main roles.

### sources

The `sources` folder is where all template projects for exploration should be placed. Each folder
within `sources/fpga` refers to a kernel/application. Within each kernel/application folder, there
can be or more **source** projects folders. Each of those **source** projects should contain all
the needed files for generating DSE projects for actual exploration.

There is no specific formatting for these **source** folders. It is up to the developer to make the
proper interface between the contents of these folders, and cirith's frontend. Interface is usually
done using the [configuration files](#configs).

The existing projects can be used as headstart. Let us use `histo` as example.

The **sources** for **histo** kernel are located in `sources/fpga/histo` folder. There are four
**source** folders. These are exploration projects for the same application, but with some differences:

- The `zcu104` folder contains the basic exploration project for the **histo** kernel on the **zcu104** platform;
- The `zcu104_bsl` is very similar to `zcu104`, but it only contains the basic configuration for the kernel. This means that no exploration is performed. This is used as a **baseline** performance analysis;
- The `zcu104_rbw` contains a modified version that was manually adjusted for handling the **read-before-write** pattern (see my [thesis](#publications) for more details on this pattern);
- The `zcu104_rbw_bsl` is the `zcu104_bsl` counterpart but considering the **read-before-write** pattern. That means, it is a baseline project, no exploration is performed here.

All four folders have the same structure.

#### lina

The `lina` folder contains the files required for the design space exploration itself, using Lina.
It is composed of a `Makefile` to prepare the executables for Lina trace/analysis, and the source
files modified from the Parboil benchmark.

#### vivado

The `vivado` folder contains the files required for testing the application on the board. It is
also composed of a modified version of Parboil's **histo**. The application was adapted and put
together with supporting files to allow Vivado synthesis and board deployment.

#### Makefile

The top-level `Makefile` on this folder is responsible for providing some global recipes for both
the Lina and Vivado projects.

#### histo.codegen.json

The file `histo.codegen.json` is a configuration file for the **codegen** supporting tool. Refer to
[its section](#codegen) for more details on how to use it.

#### histo.json

The file `histo.json` is the major configuration file for the design space exploration. It specifies
which loops should be tested for unroll/pipeline, which/how arrays should be partitioned, and so on.
Most of its details are presented
[here](https://github.com/comododragon/lina?tab=readme-ov-file#json-description-file) and
[here](https://github.com/comododragon/linaii?tab=readme-ov-file#the-script).

There are some additional keywords that can be added to this JSON file, specific to the tools used
here with cirith:

* `vivado-otherloops`: a two-element list of integers, used to specify how many loops there are before and after the loop to be explored;
	* Some functions may have some pre or post-processing loops. These should be specified using this keyword, in order to properly perform pareto analysis;
	* Default is `[0, 0]`;
* `part`: part number of the platform to be used during board test;
* `readonly`: boolean attribute that can be added to an array definition. Specifies that such array is read-only;
* `writeonly`: boolean attribute that can be added to an array definition. Specifies that such array is write-only;
* `offchip`: a list, specifying which of the defined arrays are placed off-chip.

#### histo.paretogen.json

This file is used to configure the Pareto set generation after exploration. Refer to
[**paretogen**](#paretogen) for more details on how to use it.


### etc

The `etc` folder contains several supporting files, split within sub-folders. The sub-folders are
presented below.

#### configs

The `configs` folder contains configuration files that explain to cirith how explorations should
happen. Each folder inside `etc/configs/fpga` represents a kernel to be explored, and inside each
folder, at least one `cfg` files is present.

The `cfg` files are configuration files that translates to cirith how each step of the exploration
should be executed. These files use the INI format.

> **Note:** there should be at least a `global.cfg` file, containing default values. Other `cfg`
files can be added, and these can have overriding values. This separation into multiple `cfg` files
can be used when one wants to have different exploration variants for a single kernel. 

##### Automatic Variables

During interpretation of the `cfg` files, cirith performs some variable replacements. The keywords
presented below are replaced by the actual values related to the current cirith execution:

* `$(KERNEL)`: the **kernel** name;
	* **Example:** `histo`;
* `$(SRC)`: the **source** being used;
	* **Example:** `zcu104_bsl`;
* `$(EXP)`: current exploration **experiment**;
	* **Example:** `default_bsl`;
* `$(ROOTD)`: cirith root folder;
* `$(TIMESTAMP)`: timestamp string, when current cirith command started;
	* **Example:** `2021-10-10_17-24-17.873855`;
* `$(LSETUP)`: current **setup** represented as a string;
	* **Example:** `b0.v0.k1.p0`;
* `$(LBUFFER)`: current buffer configuration on current **setup**, either `0` or `1`;
	* Not used in current explorations (i.e. `0`);
* `$(LVECTORISE)`: current vectorisation configuration on current **setup**, either `0` or `1`;
	* If `0`, lina does not consider vectorisation during estimation. If `1`, vectorisation is considered (`--f-vec` argument on lina);
* `$(LBANKING)`: current banking configuration on current **setup**, either `0` or `1`;
	* If `0`, lina does not consider DDR banking during estimation. If `1`, banking is considered;
* `$(LPOLICY)`: current banking configuration on current **setup**, either `0` or `1`;
	* See [here](https://github.com/comododragon/linaii/tree/master?tab=readme-ov-file#ddr-scheduling-policies);
* `$(KNOB)`: current post-decision **knob** in consideration;
	* See [Post-decision Code Transformation](#post-decision-code-transformation) for more information.

##### File Format

The following configuration sections are expected. Each section expects one or more key-value pairs
as well:

* `[defaults]`: some default values to be assumed by cirith;
	* `source`: standard source to be considered when `-s | --source` argument is not passed;
		* **Example:** `source=zcu104`;
	* `knob`: standard post-exploration knob to be considered when `-k | --knob` argument is not passed;
		* **Example:** `knob=0`;
* `[generate]`: parameters for cirith's [generate](#generate) command;
	* `execute`: command to be executed when `generate` command is called. The command can be split into multiple lines;
		* This command can be a call to a script, for example. It should be used to prepare an exploration project on `experiments/fpga/$(KERNEL)/$(EXP)`, using `sources/fpga/$(KERNEL)/$(SRC)` as a base. Refer to `etc/parboil/cirithtools/fpga-generate` for a working example;
* `[explore]`: parameters for cirith's [explore](#explore) command;
	* `cwd`: working directory where exploration should happen;
		* **Example:** `cwd=./experiments/fpga/$(KERNEL)/$(EXP)/$(LSETUP)`;
	* `execute`: command to be executed when `explore` command is called. The command can be split into multiple lines;
		* This is the command to perform actual design space exploration. Refer to `etc/configs/fpga/histo/global.cfg` for a working example;
* `[decide]`: parameters for cirith's [decide](#decide) command;
	* `cwd`: working directory where decision should happen;
		* **Example:** `cwd=./experiments/fpga/$(KERNEL)/$(EXP)/$(LSETUP)`;
	* `execute`: command to be executed when `decide` command is called. The command can be split into multiple lines;
		* This is the command to finalise the exploration results and decide for a best point. Refer to `etc/parboil/cirithtools/fpga-decide` for a working example;
* `[synth]`: parameters for cirith's [synth](#synth) command;
	* `cwd`: working directory where synthesis should happen;
		* **Example:** `cwd=./experiments/fpga/$(KERNEL)/$(EXP)/$(LSETUP)/knob$(KNOB)`;
	* `execute`: command to be executed when `synth` command is called. The command can be split into multiple lines;
		* This is the command to perform design synthesis of the selected point with `decide`;
* `[expand]`: parameters for cirith's [expand](#expand) command;
	* `cwd`: working directory where expansion should happen;
		* **Example:** `cwd=./experiments/fpga/$(KERNEL)/$(EXP)/$(LSETUP)`;
	* `execute`: command to be executed when `expand` command is called. The command can be split into multiple lines;
		* This is the command to expand and perform code transformation after a best point has been decided. Refer to `etc/parboil/cirithtools/fpga-expand` for a working example;
* `[stat]`: parameters for cirith's [stat](#stat) command;
	* `cwd`: working directory where stats should be retrieved;
		* **Example:** `cwd=./experiments/fpga/$(KERNEL)/$(EXP)/$(LSETUP)/knob$(KNOB)`;
	* `execute`: command to be executed when `stat` command is called. The command can be split into multiple lines;
		* This is the command that presents several stats about the experiment. Refer to `etc/parboil/cirithtools/fpga-stat` for a working example.

#### lina

This is the folder where the lina toolchain should be placed. Usually that means:

1. Downloading the right installer script for Lina Mark 2:
	* For version with `cachedaemon`, use [this](https://raw.githubusercontent.com/comododragon/linaii/cachedaemon/misc/compiler.sh);
	* For version without `cachedaemon`, use [this](https://raw.githubusercontent.com/comododragon/linaii/master/misc/compiler.sh);
1. Place it on this folder, and run it to download and build lina.

Some tools are provided in folder `linatools`, that are used together with lina to perform the
exploration steps. These are presented below.

##### ParetoGen

ParetoGen is used to derive the design points that are pareto-optimal according to Lina DSE. That
is, this tool is intended to be executed AFTER lina exploration has been performed.

Its execution command-line is quite similar to `run.py`. Below is a brief description:

```
Usage: paretogen [OPTION=VALUE]... COMMAND EXPERIMENT [KERNELS]...
    [OPTION=VALUE]... change options (e.g. UNCERTAINTY=27.0):
                          SILENT      all spawned subprocesses will be muted
                                      (*.out files are still written)
                                      (use SILENT=yes to enable)
                                      DEFAULT: no
    COMMAND           may be
                          pareto
                          prepare
                          decide
    EXPERIMENT        the experiment to consider (i.e. the folders in "csvs/*")
    [KERNELS]...      the kernels to run the command on
                      (will consider all in the experiment when omitted)
```

Refer to `etc/parboil/cirithtools/fpga-decide` for usage example.

ParetoGen is configured using a JSON file, a working example can be found at
`sources/fpga/histo/zcu104/histo.paretogen.json`. Below is a description of the expected elements:

* `repeated`: `true` if design points with exactly the same metrics should be added to the Pareto front, or `false` otherwise;
* `objectives`: a list of objectives to consider during Pareto front generation. Available options are: `exectime`, `lut`, `ff`, `dsp`, and `bram`;
* `priority`: specify which objective should have larger priority for tiebreaking;
* `compcodegenmode`: currently not fully supported. Simply set as `{"rbwunroll": false, "explvec": false}`.

##### CodeGen

CodeGen is the tool used to generate the final kernel code that can be synthesised using
SDSoC / Vivado HLS.

All source code that should pass through CodeGen must have some supporting tags. These are used by
CodeGen for location and proper code generation. Here are some examples of codes using these tags:

* `sources/fpga/histo/zcu104_rbw/vivado/benchmarks/histo/src/sdx_zcu104/histo.cl`;
* `sources/fpga/histo/zcu104_rbw/vivado/benchmarks/histo/src/sdx_zcu104/Makefile`;
* `sources/fpga/histo/zcu104_rbw/vivado/benchmarks/histo/src/sdx_zcu104/hls_hook.tcl`;
* `sources/fpga/histo/zcu104_rbw/vivado/Makefile`.

CodeGen is configured using a JSON file, a working example can be found at
`sources/fpga/histo/zcu104/histo.codegen.json`. Below is a description of the expected elements:

* `makefile`: a list of paths containing all build files that should have some tags replaced;
* `clsource`: the OpenCL kernel file that is used as template for code generation;
* `autorbw`: if present, CodeGen enters a special mode called autorbw. See below for more info. This mode expects a single element on a dictionary:
	* `incr`: an integer or string that indicates the loop increment on the loop that should go through `autorbw`. It can be a number, or a preprocessor macro used in the code (in that case, `incr` should be a string);
* `patches`: a dictionary of booleans, enabling/disabling some possible code patches. See below for more information. Possible patches are `en-expl-unroll-writeback-loops` and `ds-readwrite-loops-pipeline`.

The `autorbw` mode is a special mode of codegen where the input OpenCL code is heavily modified to
position all off-chip reads before all off-chip writes. Details of why this can be beneficial are
available on my [thesis](#publications). This mode REQUIRES the input OpenCL code to be written on
a special format. The following tags are required on code:

* `<LINCR>`: the value that increments the `for` loop. This format considers a canonical loop form: `for(i = 0; i < XXXX; i += <LINCR>)`;
* `<LPREAMB>`: place at beginning of loop body. Put here variables declarations;
* `<LBEGIN>`: place at beginning of the read segment of the loop body (this mode only supports one loop level!);
* `<LEND>`: place at end of loop body;
* `<LFRONT>`: place at the frontier. Before this, only DDR reads, after this, only DDR writes;
* `<LITER>`: ID that codegen will generate used to differentiate each unrolled loop body. Use this to annotate variables;
* `<LCTR>`: iteration "counter" during unrolling. Use this to annotate and differentiate variables.

Example of an OpenCL kernel on this format can be found in
`sources/fpga/histo/zcu104_rbw/vivado/benchmarks/histo/src/sdx_zcu104/histo.cl`.

**Patches** are certain small code transformations that CodeGen can do to improve performance or
facilitate certain HLS optimisations. Currently there are two possible patches:

* `en-expl-unroll-writeback-loops`: if `true`, writeback loops (i.e. loops to write the on-chip kernel data back to DDR memory) are explicitly unrolled;
	* This patch should only bring benefit if Vivado/SDx performs auto-vectorisation, which so far appears to happen only then DDR banking is active. Without auto-vectorisation, this unroll might actually make Vivado lose its orientation and make the writeback loop even worse. Therefore if banking is disabled, this patch is also disabled;
* `ds-readwrite-loops-pipeline`: if `true`, pipeline is disabled for read and writeback loops (that is, all loops that move data between on- and off-chip memories before and after the processing).

Patches might be disabled for specific reasons. If this happens, CodeGen will inform.

Some of the code transformation here expects some post-manipulation done by the developer. Since
this could be some amount of work that the developer will not want to lose due to re-running
exploration and so on, CodeGen actually stores codes to be manually manipulated on a separate
**code repository**. There is a big comment on CodeGen's code explaining this:

```
For all files listed in the clsource and makefile tags of the codegen json file, codegen reads
from _srcFolder and writes to _dstFolder. They can be the same place (i.e. in-place transform),
however this could be nasty, one file might be transformed to a soft-link to the repository!
Either way...

If manual transformations are enabled, codegen will generate all files in _repoFolder instead
of _dstFolder, and _dstFolder will only receive soft-links to the _repoFolder files. If the
files in _repoFolder already exists, only the symlinks are created.

This repository of manual transformations is physically located and generated by codegen
in the path defined by the $CDG_ROOTD environment variable. This variable must be set.

NOTE: Please note that the files are physically located in the $CDG_ROOTD folder. If more
than one project in your DSE hierarchy (e.g. using cirith) uses a same physical file from
$CDG_ROOTD, modifying this file will affect all projects, since only soft links are generated.
If you want to create any differentation between the manual projects, I suggest you change
the name of the kernel's source folder (e.g. base_1, base_2 or whatevs).

NOTE: Also note that each project is located in a folder within $CDG_ROOTD in the following
pattern: $CDG_ROOTD/<experiment>/<kernel>/<code>/rbwUnrollX_explVecY, where <experiment>,
<kernel> are self-explanatory, <code> is the design point code (e.g. f200_l0.1.0.0_...),
and X and Y are the design knobs for the read-before-write unroll transformation (rbwUnroll)
and explicit vectorisation (explVec). X = 1 or Y = 1 means enabled, 0 otherwise. This means
that when Lina parameters are varied, it will not be differentiable from the code repository
side. If you want to have 2 unique manual transformations that use the same <code>, for
example one using repeated points with Lina and another not, please use different
<experiment>s to isolate.
```

The CodeGen repositories used by Cirith are located at `etc/lina/linatools/codegenrepo`.

> **NOTE:** cirith maintains different codegen repositories for each Lina setup (e.g.
`b0.v0.k1.p0`, `b0.v1.k1.p1`, ...). The difference between each repository is more related to the
memory analyses performed by Lina (i.e. the `memanalysis.rpt` files might differ). However, the
manual transformation might be the same among all setups, therefore before manipulating the kernel,
check if there isn't already an implemented kernel with the same code (e.g. `f200_0.1.0.0_...`),
same manual transforms enabled (e.g. `rbwUnroll1_explVec0`) from the same kernel and experiment.
More specifically, check for these folders:
`etc/lina/linatools/codegenrepo/*/<experiment>/<kernel>/<code>/<rbwUnrollX_explVecY>/`. If such
similar project is found, its kernel files can probably be reused. Simply copy/paste it from one 
repo (setup) to another.

#### parboil

This is the folder to put all the files related to Parboil benchmark. See
[this](https://github.com/comododragon/cirith-fpga/blob/main/etc/parboil/README) for details on how
to properly populate this folder.

There are two folders already placed here. The `cirithcustomdatasets`, as name says, contains custom
datasets from Parboil benchmark, but adjusted to be used with the cirith, more specifically the zcu104
board. These datasets were adjusted so that the respective kernels can fit on this platform.

The folder `etc/parboil/cirithtools` contains scripts to perform the cirith commands. This is the heart
of the integration between frontend (cirith) and backend (Parboil benchmarks explored on lina). The files
here are responsible for moving files, to create experiments from source files, run exploration, decide
best point, synthesise, and so on.

### experiments

This is the folder where the exploration experiments are generated. Basically, files from `sources`
are used to generate experiments, coordinated by the configuration files and scripts from `etc`.

Kernel explorations are placed in `experiments/fpga` by the kernel/application name. Within this
folder, multiple experiments can be placed, based on the **sources** for each kernel. For example,
experiment `default` can be generated based on source `zcu104`, or `default_bsl` can be generated
from `zcu104_bsl`. Within each experiment folder, there are also multiple folders, one for each
**setup** (e.g. `b0.v0.k1.p0`, see [Exploration Variants](#exploration-variants)). Finally inside
each setup folder, is a complete exploration project. In summary, the folder hierarchy follows:

```
experiments/fpga/<KERNEL>/<EXPERIMENT>/<SETUP>/...
```

The existing projects can be used as headstart. Let us use `histo` as example.

All setups have a similar structure, based on the folder structure expected by the `run.py` scripts.
See [here](https://github.com/comododragon/lina?tab=readme-ov-file#perform-an-exploration) and
[here](https://github.com/comododragon/linaii?tab=readme-ov-file#the-script) for more information.

#### workspace

The exploration is performed inside this folder. Within `workspace/experiment1/histo`, one can find
a folder for each design point to be explored, plus a `base` folder. The `base` folder is where lina
puts all files that are shared during the exploration. The outputs for each design point are placed
in the design point folders, for example `workspace/experiment1/histo/f100.0_l0.1.0.0_ahist.block.4`.

#### sources

The `sources` folder here is assembled based on files present on the root `sources` folder, like the
JSON files.

#### outcomes

Decided pareto-optimal points are placed inside this folder. After `decide` command is called, an
`opt` symlink will be created, pointing to the best point (as estimated by lina). Inside `opt` there
will be a synthesisable project, that can be tested on board.

#### knob0, knob1, ...

When the `expand` command is called, the synthesisable project present on `outcomes/experiment1/histo/opt`
is passed through CodeGen for [post-decision code transformation](#post-decision-code-transformation).
The resultant project is placed at a `knobX` folder, where X is the knob ID that was used during
expansion. This is also a synthesisable project that can be tested on board.

#### csvs

All estimated metrics using lina are collected and saved on a CSV file inside this folder.

#### codegen.py, paretogen.py, run.py, utils, ...

The other files present on this folder are symlinks to scripts that are used during exploration,
for example CodeGen, ParetoGen, etc.


## Acknowledgments

The project author would like to thank São Paulo Research Foundation (FAPESP), who funded the research where this project was inserted (Grants 2016/18937-7 and 2018/22289-6).

The opinions, hypotheses, conclusions or recommendations contained in this material are the sole responsibility of the author(s) and do not necessarily reflect FAPESP opinion.

