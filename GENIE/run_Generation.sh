#!/bin/bash

#------------------------------------
GeneratorName=GENIE

#------------------------------------
HornCur=fhc

#------------------------------------
# Flux file
# 1) NuMI
# - We are using bin-width divided histogram (i.e., each bin content is flux/GeV)
FluxType=ICARUS
FluxFilePath=${myWD}/Flux/ICARUS/NuMI/flux_BinWidthDevided_250616.root
FluxHistName_numu=corrected_flux_fhc_numu_BinWidthDivided
FluxHistName_numubar=corrected_flux_fhc_numubar_BinWidthDivided
# 2) BNB
#FluxType=ICARUS_BNB
#FluxFilePath=${myWD}/Flux/ICARUS/BNB/bnb_icarus_numu_histogram.root
#FluxHistName_numu=hNuE_FV_ZFRONT_FULL_ERANGE

#------------------------------------
# - GENIE Tune
#   - We will use ${GENIE_XSEC_TUNE}
if [ -z ${GENIE_XSEC_TUNE} ]; then
  echo "GENIE_XSEC_TUNE is not set; run something like"
  echo "export GENIE_XSEC_TUNE=AR23_20i_00_000"
  return 1
fi
GeneratorConfigName=${GENIE_XSEC_TUNE}

#------------------------------------
# - GENIE xsec splines
#   - We will use ${GENIEXSECFILE}
#   - If you are using UPS, you may get this by "setup genie_xsec v3_04_00 -qAR2320i00000:e1000:k250"
if [ -z ${GENIEXSECFILE} ]; then
  echo "GENIEXSECFILE is not set; point GENIEXSECFILE to your pre-calculated cross section splines"
  return 1
fi

#------------------------------------
# - GENIE event list
#   - Here I only generate CC
#   - You can find more in https://github.com/GENIE-MC/Generator/blob/master/config/EventGeneratorListAssembler.xml
GENIEEventList=CC 

#------------------------------------
GeneratorJobName=GUNDAMWorkshopSept2025

#------------------------------------
# - Target; Argon 100%
TargetName=Ar
Target=1000180400[1.0]

#------------------------------------
# - Number of events to generate
NEvents=1000000

#------------------------------------
# - JobID (I use this as random seed)
JobID=0

#------------------------------------
# - Random seed (as explained above, I use JobID)
RandomSeed=${JobID}

#------------------------------------
OutputDestBase=${OutputBaseDir}/${GeneratorName}/${GeneratorConfigName}/${FluxType}/${GeneratorJobName}/Target_${TargetName}

# numu
Run_numu=true
if [[ "$Run_numu" == true ]]; then
  NuName=numu
  NuPDG=14
  FluxHistName=${FluxHistName_numu}
  OutputDest=${OutputDestBase}/${HornCur}_Nu${NuPDG}
  mkdir -p ${OutputDest}
  GenOutputFile=${OutputDest}/output_${GeneratorName}_${JobID}.root
  NuisOutputFile=${OutputDest}/output_${GeneratorName}_${JobID}.nuisflat.root
# 1) run gevgen
  gevgen -f ${FluxFilePath},${FluxHistName} -r 1 -n ${NEvents} -t ${Target} -e 0,20 -p ${NuPDG} --tune ${GENIE_XSEC_TUNE} --cross-sections ${GENIEXSECFILE} --seed ${RandomSeed} -o ${GenOutputFile} --event-generator-list ${GENIEEventList}
# 2) PrepareGENIE; Add additional info into the gevgen output for further NUISANCE routines
  PrepareGENIE -i ${GenOutputFile} -f ${FluxFilePath},${FluxHistName} -t ${Target}
# 3) Make NUISANCE Tree, nuisflat
  nuisflat -c ${myWD}/NUISANCE_cards/card_nothing.card -i GENIE:${GenOutputFile} -o ${NuisOutputFile} -f GenericFlux
fi

# numubar
Run_numubar=false
if [[ "$Run_numubar" == true ]]; then
  NuName=numubar
  NuPDG=-14
  FluxHistName=${FluxHistName_numubar}
  OutputDest=${OutputDestBase}/${HornCur}_Nu${NuPDG}
  mkdir -p ${OutputDest}
  GenOutputFile=${OutputDest}/output_${GeneratorName}_${JobID}.root
  NuisOutputFile=${OutputDest}/output_${GeneratorName}_${JobID}.nuisflat.root
# 1) run gevgen
  gevgen -f ${FluxFilePath},${FluxHistName} -r 1 -n ${NEvents} -t ${Target} -e 0,20 -p ${NuPDG} --tune ${GENIE_XSEC_TUNE} --cross-sections ${GENIEXSECFILE} --seed ${RandomSeed} -o ${GenOutputFile} --event-generator-list ${GENIEEventList}
# 2) PrepareGENIE; Add additional info into the gevgen output for further NUISANCE routines
  PrepareGENIE -i ${GenOutputFile} -f ${FluxFilePath},${FluxHistName} -t ${Target}
# 3) Make NUISANCE Tree, nuisflat
  nuisflat -c ${myWD}/NUISANCE_cards/card_nothing.card -i GENIE:${GenOutputFile} -o ${NuisOutputFile} -f GenericFlux 
fi
