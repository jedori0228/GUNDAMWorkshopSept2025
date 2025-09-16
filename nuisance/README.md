# Build nuisance
## cmake will mostly automatically detect generator that are built
```
mkdir nuisance
cd nuisance
git clone git@github.com:jedori0228/nuisance.git nuisance-src -b work_TempNEUT6
mkdir build
cd build
cmake ../nuisance-src
make install -j4
# run below in every new shell
source Linux/setup.sh
```
