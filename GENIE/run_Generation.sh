# Flux file
FluxFilePath=${myWD}/Flux/ICARUS/flux_BinWidthDevided_250616.root

# Flux histograms
# - We are using bin-width divided histogram (i.e., each bin content is flux/GeV)
FluxHistName_numu=corrected_flux_fhc_numu_BinWidthDivided
FluxHistName_numubar=corrected_flux_fhc_numubar_BinWidthDivided

# - Number of events to generate
NEvents=1000

# - Target; Argon 100%
Target=1000180400[1.0] 

# - GENIE Tune
#   - We will use ${GENIE_XSEC_TUNE}
if [ -z ${GENIE_XSEC_TUNE} ]; then
  echo "GENIE_XSEC_TUNE is not set; run something like"
  echo "export GENIE_XSEC_TUNE=AR23_20i_00_000"
  return 1
fi

# - GENIE xsec splines
#   - We will use ${GENIEXSECFILE}
#   - If you are using UPS, you may get this by "setup genie_xsec v3_04_00 -qAR2320i00000:e1000:k250"
if [ -z ${GENIEXSECFILE} ]; then
  echo "GENIEXSECFILE is not set; point GENIEXSECFILE to your pre-calculated cross section splines"
  return 1
fi

# - GENIE event list
#   - Here I only generate CC
#   - You can find more in https://github.com/GENIE-MC/Generator/blob/master/config/EventGeneratorListAssembler.xml
GENIEEventList=CC 

# - JobID (I use this as random seed)
JobID=0

# - Random seed (as explained above, I use JobID)
RandomSeed=${JobID}

# numu
# 1) run gevgen
gevgen -f ${FluxFilePath},${FluxHistName_numu} \
-r 1 \
-n ${NEvents} \
-t ${Target} \
-e 0,20 \
-p 14 \
--tune ${GENIE_XSEC_TUNE} \
--cross-sections ${GENIEXSECFILE} \
--seed ${RandomSeed} \
-o output_GENIE_numu_${GENIEEventList}_${JobID}.root \
--event-generator-list ${GENIEEventList}
# 2) PrepareGENIE; Add additional info into the gevgen output for further NUISANCE routines
PrepareGENIE -i output_GENIE_numu_${GENIEEventList}_${JobID}.root \
-f ${FluxFilePath},${FluxHistName_numu} \
-t ${Target}
# 3) Make NUISANCE Tree, nuisflat
nuisflat -c ${myWD}/NUISANCE_cards/card_nothing.card  \
-i GENIE:output_GENIE_numu_${GENIEEventList}_${JobID}.root \
-o output_GENIE_numu_${GENIEEventList}_${JobID}.nuisflat.root \
-f GenericFlux

# numubar
# 1) run gevgen
gevgen -f ${FluxFilePath},${FluxHistName_numubar} \
-r 1 \
-n ${NEvents} \
-t ${Target} \
-e 0,20 \
-p -14 \
--tune ${GENIE_XSEC_TUNE} \
--cross-sections ${GENIEXSECFILE} \
--seed ${RandomSeed} \
-o output_GENIE_numubar_${GENIEEventList}_${JobID}.root \
--event-generator-list ${GENIEEventList}
# 2) PrepareGENIE; Add additional info into the gevgen output for further NUISANCE routines
PrepareGENIE -i output_GENIE_numubar_${GENIEEventList}_${JobID}.root \
-f ${FluxFilePath},${FluxHistName_numubar} \
-t ${Target}
# 3) Make NUISANCE Tree, nuisflat
nuisflat -c ${myWD}/NUISANCE_cards/card_nothing.card  \
-i GENIE:output_GENIE_numubar_${GENIEEventList}_${JobID}.root \
-o output_GENIE_numubar_${GENIEEventList}_${JobID}.nuisflat.root \
-f GenericFlux

