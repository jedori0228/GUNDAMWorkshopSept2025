# Build gibbu locally using Alma9+spack
```
# load packages from spack
source setup.sh

# download the pacakge
source download_r2025_02.sh

# Edit one line from "gibuu-r2025_02-src/release/Makefile"
# L66~L67
# #export VERSION='SVN revision $(shell svnversion -n .)'
# export VERSION='$(shell cat version.txt)'

# build
source build_r2025_02.sh


```

- Edit FileNameFlux
    - Should be a text file (.dat), each line is "energy fluxvalue"
- Create a directory
    - GiBBU produces lots of output files, so you may want to run GiBBU in a separate directory, not here!
- Generation:
```
# numu
mkdir numu
cd numu
GiBUU.x < ../GiBUU2025_ICARUS_numu.job
cd -
# numubar
mkdir numubar
cd numubar
GiBUU.x < ../GiBUU2025_ICARUS_numubar.job
cd -
```
- To run post-processing (PrepareGiBUU and nuisflat), run run_PostGeneration.py
