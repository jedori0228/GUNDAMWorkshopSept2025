. /cvmfs/larsoft.opensciencegrid.org/spack-v0.22.0-fermi/setup-env.sh

## hljl7gy root@6.28.12%gcc@12.2.0 arch=linux-almalinux9-x86_64_v3
spack load /hljl7gy

spack load gcc@12.2.0

## b2vqwgc ifdhc@2.8.0%gcc@12.2.0 arch=linux-almalinux9-x86_64_v3
spack load /b2vqwgc

spack load xrootd@5.6.9%gcc@12.2.0

gibuu_tag=r2025_02
export PATH=${PATH}:`pwd`/gibuu-${gibuu_tag}-src/release/testRun/
