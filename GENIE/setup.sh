. /cvmfs/larsoft.opensciencegrid.org/spack-v0.22.0-fermi/setup-env.sh

# gpts2zp genie@3.04.02%gcc@12.2.0 arch=linux-almalinux9-x86_64_v2
spack load /gpts2zp

## hljl7gy root@6.28.12%gcc@12.2.0 arch=linux-almalinux9-x86_64_v3
spack load /hljl7gy

export GENIE_XSEC_TUNE=AR23_20i_00_000
export GENIEXSECFILE=/cvmfs/larsoft.opensciencegrid.org/products/genie_xsec/v3_04_00/NULL/AR2320i00000-k250-e1000/data/gxspl-FNALsmall.xml
