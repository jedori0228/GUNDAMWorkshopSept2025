archive_name=r2025_02
YearTag=2025

src_dirname=gibuu-${archive_name}-src

cd ${src_dirname}
cd release
make buildRootTuple_POS
make withROOT=1
