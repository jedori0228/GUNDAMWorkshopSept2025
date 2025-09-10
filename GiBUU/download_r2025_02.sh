archive_name=r2025_02
YearTag=2025

src_dirname=gibuu-${archive_name}-src
mkdir -p ${src_dirname}
cd ${src_dirname}

curl -J -L -O "https://gibuu.hepforge.org/downloads?f=archive/${archive_name}/release${YearTag}.tar.gz" && tar -xzvf release${YearTag}.tar.gz
curl -J -L -O "https://gibuu.hepforge.org/downloads?f=archive/${archive_name}/buuinput${YearTag}.tar.gz" && tar -xzvf buuinput${YearTag}.tar.gz
curl -J -L -O "https://gibuu.hepforge.org/downloads?f=archive/${archive_name}/libraries${YearTag}_RootTuple.tar.gz" && tar -xzvf libraries${YearTag}_RootTuple.tar.gz

cd ../
