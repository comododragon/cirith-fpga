# cirith-ungol (FPGA)
A framework to explore, synthesise, and profile FPGA projects using the Lina estimator.

## Note

**Documentation for this repository is still being assembled.**

The `cirith` tool has a very descriptive help message that can help meanwhile. Below is the help
message:

```
Usage: cirith fpga COMMAND [OPTION]... KERNEL

Where COMMAND may be:
   generate: generate a new experiment from the kernel source
       optional OPTIONs: -h, -s, -e, -f, -F, -m
       default subprocesses environment: lina ($CIR_LI_* variables)
   explore : perform design space exploration on an experiment
       optional OPTIONs: -h, -e, -m
       default subprocesses environment: lina ($CIR_LI_* variables)
   decide  : given an explored design space, decide the best point(s)
       optional OPTIONs: -h, -e, -m
       default subprocesses environment: vivado ($CIR_VI_* variables)
   synth   : synthesise the points selected with "decide"
       optional OPTIONs: -h, -e, -m, -k
       default subprocesses environment: vivado ($CIR_VI_* variables)
   expand  : expand a decided design point: apply some code transformations
       optional OPTIONs: -h, -e, -m, -k
       default subprocesses environment: vivado ($CIR_VI_* variables)
   verify  : execute synthesised points and verify their results
       optional OPTIONs: -h, -e, -m, -k
       default subprocesses environment: vivado ($CIR_VI_* variables)
   profile : execute a synthesised point, profile power and/or execution time
       (default is both)
       optional OPTIONs: -h, -e, -m, -k, -w, -x, -c, -r, -p, -i
       default subprocesses environment: vivado ($CIR_VI_* variables)
   collect : collect results from an experiment, save to the experiment database (path
       is $PWD/results/fpga/SRC/EXP*)
       optional OPTIONs: -h, -e, -f, -F, -d, -b, -g
       default subprocesses environment: none
   stat    : show information about an experiment
       optional OPTIONs: -h, -e
       default subprocesses environment: none

Where OPTION may be:
    -h     , --help             : this message
    -s SRC , --source=SRC       : use SRC as the source for the experiment (default is
                                  "base", folder path is $PWD/sources/fpga/KERNEL/SRC)
    -e EXP , --experiment=EXP   : use EXP as the destination experiment (default is
                                  "default", folder path is
                                  $PWD/experiments/fpga/KERNEL/EXP)
    -f     , --force            : run COMMAND for this KERNEL EXP, even if it ran before
                                  already (will ask for confirmation though)
    -F     , --force-really     : run COMMAND for this KERNEL EXP, even if it ran
                                  before already (NO CONFIRMATION ASKED!!!!)
    -m MASK, --mask=MASK        : use MASK to select which variants of an experiment to
                                  perform the COMMAND. See the section "Exploration
                                  variants" below for more information and defaults. The
                                  default value is only used for the "generate" command.
                                  All other commands will iterate over all generated
                                  variants in the experiment if no "-m|--mask" is set
    -k KNOB, --knob=KNOB        : perform a post-exploration (actually post-decision)
                                  code transformation using knob KNOB. See the section
                                  "Post-decision code transformation" below for more
                                  information (default is 0)
    -w     , --power            : profile only power
    -x     , --exectime         : profile only execution time
    -c     , --cooldown=TIME    : wait TIME seconds before actually starting the profile
                                  (default is 0 seconds)
    -r RATE, --samplrate=RATE   : sampling rate (in ms) for power sampling (default is
                                  100 ms)
    -p     , --preserve         : preserve the power log file (it will be saved with the
                                  profiling timestamp)
    -i     , --skip-export      : if you use a bootable sdcard or any other medium/procedure
                                  that requires manual intervention when moving a
                                  project from this PC to the accelerator, you can skip this
                                  "exportation" step using this option. This is useful,
                                  for example, when several profiling runs should be
                                  performed using a same project (i.e. no need to transfer
                                  every time). This requires also support on the backend
                                  (i.e. your sources/scripts must implement this skip). This
                                  flag is automatically disabled if multiple variants are
                                  considered in a cirith execution, either by the "-m|--mask"
                                  argument or by iterating all variants in the experiment
    -d     , --delete           : delete result files $PWD/results/fpga/SRC/EXP* if they
                                  exist
    -b     , --backup           : if file $PWD/results/fpga/SRC/EXP.csv exists, it is
                                  compressed with all associated *.pmon files to a file
                                  named $PWD/results/fpga/SRC/EXP_backup_$TIMESTAMP.tar.gz,
                                  where $TIMESTAMP is the date timestamp of the first measure
                                  in this csv file. If support to tar and/or zlib is not
                                  installed, a folder with the same name will be generated
                                  instead. The new "collect" command is then executed on
                                  a clean CSV file
    -g     , --always-gen-backup: always generate a backup/file folder from the collected
                                  results. New backup file/folder will have a numerical
                                  index before the extension (e.g. "*-2.tar.gz")

Environment variables: cirith uses custom environment variables to set up the paths
    to the tools like CUDA, Vivado, etc. These variables have as prefix CIR_XX_*, where
    XX is the prefix (midfix?) for a specific tool. Before running any command, cirith
    sets up different environments for each tool. Each CIR_XX_* variable replaces the
    respective variable without the suffix. For example, $CIR_XX_FOO will replace $FOO
    when environment for tool XX is selected. Supported tools for FPGA are:
        none                    : use the standard environment
        vivado                  : use the environment set by $CIR_VI_* variables
        lina                    : use the environment set by $CIR_LI_* variables

    The default environment for each command is defined within each command's description
    description. You can force another environment by using the "environment=" option in
    your configuration file. For example:
        [generate]
        environment=cuda

    HINT: One can use the "setenv.sh" script to set these variables. Check the script's
          content for further info!

Exploration variants: to provide more flexibility in the explorations, cirith allows for each
    experiment to have different variants self-contained. Each variant usually consists of a
    small different setup in your design space exploration, for example enabling or disabling
    certain heuristics. One could argue that these could be considered optimisation knobs
    suitable for the design space exploration itself, however usually these setups that compose
    each variant are not intended to be explored for every kernel. With a robust DSE, only one
    variant might be enough. The remaining are useful for specific optimisations or analyses.
    This variant scheme is specific to each tool and must be implemented in cirith, or can also
    be disabled by providing support to just a single variant. Current supported variant schemes are:
        NONE: to be implemented (sorry...)
        LINA: bX.vY.kZ.pW, where:
                  bX: enable/disable on-chip buffering of all arrays. b0 is disabled, b1 is enabled
                  vY: enable/disable automatic vectorisation analysis. v0 is disabled, v1 is enabled
                  kZ: enable/disable DDR banking. k0 is disabled, k1 is enabled
                  pW: select Lina DDR scheduling policy. p0 is conservative, p1 is permissive
              For example to define a specific variant during cirith execution, use
              "-m|--mask=b0.v0.k1.p1". You can also select more than one. For example to select
              both banking policies, use "-m|--mask=b0.v0.k01.p1" (this means that both k0 and k1
              will be considered). Projects in cirith should be able to differentiate between
              variants, usually using different folders inside the experiment folder.
              Default value if no "-m" argument is supplied is: b0.v01.k1.p01

   NOTE: You can use mask to select more than one variant to be manipulated on a single cirith
         run. However, these variants must exist (i.e. the set of variants you pass through "-m"
         when running any command other than "generate" and "collect" must be a sub-set of the
         set you used during the "generate" option). The "collect" command always iterates over
         all generated variants

Post-decision code transformation: after executing "decide" for one or more variants of an
    experiment, you can proceed to directly synthesising the selected point. However, there might
    be some code manipulations that can improve final performance (e.g. by relaxing constraints of
    the HLS optimiser). You can activate these manipulations by using knobs. After running "decide",
    you can expand one or more variants using an integer knob ID. This knob ID is passed to the
    backend which your backend logic can use to implement different code manipulations. Each knob
    setup should be self-contained within a variant, at least with the files that communicates with
    cirith. In summary, an FPGA experiment is composed of one or more variants, and each variant is
    composed of one or more setups via knobs. You can only select a single knob at a time using
    the "-k" option. If no code manipulation is desired, use the default knob only. In fact if the
    "synth" command is executed right after "decide", the "expand" command will be automatically
    executed with the default knob value of 0. This is similar to running "decide", then "expand"
    with "-k|--knob=0" and then "synth" with "-k|--knob=0".

```

you can access this message by running:
```
./cirith fpga --help
```

The kernels available for exploration are: `histo`, `lbm_small`, `mri_q` and `sgemm`.

## Preparation

Ensure you have Xilinx SDSoC 2018.2 installed. Also ensure you have proper libraries for Zynq
UltraScale+ platform.

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
