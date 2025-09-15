## rwdwxv6 pythia6@6.4.28%gcc@12.2.0 arch=linux-almalinux9-x86_64_v2
spack load /rwdwxv6
export PYTHIA6_LIB_DIR=$(spack location -i /rwdwxv6)/lib
export PYTHIA6=${PYTHIA6_LIB_DIR}
# For nuwro build
export LIBRARY_PATH=${PYTHIA6}:${LIBRARY_PATH}
