#!/bin/bash

#------------------------------------
GeneratorName=NuWro

#------------------------------------
HornCur=fhc

#------------------------------------
# Flux file
# 1) NuMI
# - Flux NOT divided by bin-width (https://nuwro.github.io/user-guide/getting-started/parameters/)
FluxType=ICARUS_NuMI
FluxFilePath=${myWD}/Flux/ICARUS/NuMI/flux_BinWidthDevided_250616.root
FluxHistName_numu=corrected_flux_fhc_numu
FluxHistName_numubar=corrected_flux_fhc_numubar
# 2) BNB
#FluxType=ICARUS_BNB
#FluxFilePath=${myWD}/Flux/ICARUS/BNB/bnb_icarus_numu_histogram.root
#FluxHistName_numu=hNuE_FV_ZFRONT_FULL_ERANGE

#------------------------------------
GeneratorConfigName=21.09.2

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
# 1) run generation
  nuwro -i NuWroCard_CC_Ar_NuMIFlux_numu.txt -o ${GenOutputFile} -p number_of_events=${NEvents} -p number_of_test_events=${NEvents} -p random_seed=${RandomSeed} -p 'beam_direction=0 0 1' -p beam_particle=${NuPDG} -p beam_inputroot=${FluxFilePath} -p beam_inputroot_flux=${FluxHistName}
# 2) PrepareNuWroEvents; Add additional info into the gevgen output for further NUISANCE routines
  PrepareNuWroEvents -F ${FluxFilePath},${FluxHistName},${NuPDG} ${GenOutputFile}
# 3) Make NUISANCE Tree, nuisflat
  nuisflat -c ${myWD}/NUISANCE_cards/card_nothing.card -i NuWro:${GenOutputFile} -o ${NuisOutputFile} -f GenericFlux
fi

# numu
Run_numubar=false
if [[ "$Run_numubar" == true ]]; then
  NuName=numubar
  NuPDG=-14
  FluxHistName=${FluxHistName_numubar}
  OutputDest=${OutputDestBase}/${HornCur}_Nu${NuPDG}
  mkdir -p ${OutputDest}
  GenOutputFile=${OutputDest}/output_${GeneratorName}_${JobID}.root
  NuisOutputFile=${OutputDest}/output_${GeneratorName}_${JobID}.nuisflat.root
# 1) run generation
  nuwro -i NuWroCard_CC_Ar_NuMIFlux_numu.txt -o ${GenOutputFile} -p number_of_events=${NEvents} -p number_of_test_events=${NEvents} -p random_seed=${RandomSeed} -p 'beam_direction=0 0 1' -p beam_particle=${NuPDG} -p beam_inputroot=${FluxFilePath} -p beam_inputroot_flux=${FluxHistName}
# 2) PrepareNuWroEvents; Add additional info into the gevgen output for further NUISANCE routines
  PrepareNuWroEvents -F ${FluxFilePath},${FluxHistName},${NuPDG} ${GenOutputFile}
# 3) Make NUISANCE Tree, nuisflat
  nuisflat -c ${myWD}/NUISANCE_cards/card_nothing.card -i NuWro:${GenOutputFile} -o ${NuisOutputFile} -f GenericFlux
fi
