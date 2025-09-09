import glob
import re
import os

#------------------------------------
myWD = os.environ["myWD"]
OutputBaseDir = os.environ["OutputBaseDir"]

#------------------------------------
GeneratorName = 'GiBUU'

#------------------------------------
HornCur = 'fhc'

#------------------------------------
GeneratorConfigName = 'r2025_02'

#------------------------------------
GeneratorJobName = 'GUNDAMWorkshopSept2025'

#------------------------------------
# - Target; Argon 100%
TargetName = 'Ar'

#------------------------------------
NuiscardPath = f'{myWD}/NUISANCE_cards/card_nothing.card'

#------------------------------------
# Flux file
# - GiBBU wants uniformly-binned flux
# - Also, bin-width divided
FluxType = 'ICARUS'
FluxFileDir = f'{myWD}/Flux/ICARUS/flux_BinWidthDevided_250616.root_dat'

#------------------------------------
OutputDestBase = f'{OutputBaseDir}/{GeneratorName}/{GeneratorConfigName}/{FluxType}/{GeneratorJobName}/Target_{TargetName}'

NuTypes = [
  'numu',
  'numubar',
]

for NuType in NuTypes:

  NuPDG = 14 if NuType=='numu' else -14

  ThisOutputDir = f'{OutputDestBase}/{HornCur}_Nu{NuPDG}'
  os.system(f'mkdir -p {ThisOutputDir}')
  
  DirName = NuType
  GiBUUFiles = glob.glob(f'./{DirName}/EventOutput.Pert.0*.root')
  pat = re.compile(r"\.(\d{8})\.root$")
  JobIDs = []
  for GiBUUFile in GiBUUFiles:
    #./numu/EventOutput.Pert.00000009.root
    m = pat.search(GiBUUFile)
    this_jobid = int(m.group(1))
    JobIDs.append(this_jobid)
  for i in range(len(GiBUUFiles)):
    if (i+1) not in JobIDs:
      print(i+1)
      break


  # prep
  for GiBUUFile in GiBUUFiles:
    #./numu/EventOutput.Pert.00000009.root
    m = pat.search(GiBUUFile)
    this_jobid = int(m.group(1))
    this_prep_output = f'{ThisOutputDir}/output_GiBUU_{this_jobid}.root'

    cmd = f'PrepareGiBUU -i {GiBUUFile} -f {FluxFileDir}/corrected_flux_{HornCur}_{NuType}_BinWidthDivided.dat -o {this_prep_output}'
    print(cmd)

    this_nuis_output = f'{ThisOutputDir}/output_GiBUU_{this_jobid}.nuisflat.root'
    cmd = f'nuisflat -c {NuiscardPath} -i GiBUU:{this_prep_output} -o {this_nuis_output} -f GenericFlux'

    print(cmd)
    
