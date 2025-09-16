// For GUNDAM Workshop Part 2:
//   16 September 2025
// Prepared by Bruce Howard, building from work done by BH, Jaesung Kim, and numerous others

// Defining vars and headers and such
#include "DemoTrees.h"

// C++
#include <vector>
#include <string>
#include <utility>

using namespace ana;

std::string cafs="/pnfs/icarus/archive/sam_managed_users/jskim/data/2023A_NuMI_Reproc_ReNuSyst_BNBTrackSplitProb/*/*/*/*0.root";

void DemoTrees()
{
  SpectrumLoader loader(cafs);

  ///////////////////////////////////////////////////////////////////////////
  // A reco tree
  ///////////////////////////////////////////////////////////////////////////

  std::vector<std::string> listOfBranches = { "kNuMISliceSignalType/i", "kRecoCosThMuP", "kTrueCosThMuP" };
  std::vector<Var> listOfVars = { kNuMISliceSignalType, kNuMIRecoCosThMuP, kNuMITrueCosThMuP };

  Tree selectedTree ( "Selection", listOfBranches, loader, listOfVars, kNoSpillCut, kSelectionCut, kNoShift, true, true );

  ///////////////////////////////////////////////////////////////////////////
  // A systematic tree: simple Zexp CCQE
  ///////////////////////////////////////////////////////////////////////////

  std::vector<std::string> genieSigmaKnobs = {
    "GENIEReWeight_ICARUS_v2_multisigma_ZExpA1CCQE",
    "GENIEReWeight_ICARUS_v2_multisigma_ZExpA2CCQE",
    "GENIEReWeight_ICARUS_v2_multisigma_ZExpA3CCQE",
    "GENIEReWeight_ICARUS_v2_multisigma_ZExpA4CCQE"
  };

  std::vector<std::string> systNames;
  std::vector<const ISyst*> systsSigmaKnobs;
  std::vector<std::pair<int,int>> sigmasList;
  for ( auto const& knob : genieSigmaKnobs ) systNames.push_back(knob);
  for ( auto const& knob : genieSigmaKnobs ) systsSigmaKnobs.push_back( new SBNWeightSyst(knob) );
  for ( auto const& knob : genieSigmaKnobs ) sigmasList.push_back( std::make_pair<int,int>(-3,3) );

  NSigmasTree selectedTreeSyst ( "Systematics", systNames, loader, systsSigmaKnobs, sigmasList, kNoSpillCut, kSelectionCut, kNoShift, true, true );

  ///////////////////////////////////////////////////////////////////////////
  // A truth tree: just the true signal and the true CosThMuP
  ///////////////////////////////////////////////////////////////////////////

  std::vector<std::string> listOfTruthBranches = { "SignalType/i", "TrueCosThMuP" };
  std::vector<TruthVar> listOfTruthVars = { kTruth_SignalType, kTruth_CosThMuonProton };

  Tree truthTree ( "Truth", listOfTruthBranches, loader, listOfTruthVars, kNoSpillCut, kTruthCutIsSignal, kSelectionCut, kNoShift, true, true );

  ///////////////////////////////////////////////////////////////////////////
  // A covariance matrix:
  //   Just for fun. Let's look at True DeltaPT for FSI p weights. True CCQE
  ///////////////////////////////////////////////////////////////////////////

  // Vars:
  //   axis = kTrueDeltaPT_CCQE_Covar
  //   weight = kFSINWeight_CCQE_Covar

  // Binning
  std::vector< std::vector< std::pair< std::string, std::pair<double,double> > > > covar_bin;
  covar_bin.push_back( { std::make_pair("kTrueDeltaPT_CCQE_Covar", std::make_pair(0.00, 0.05) ) } );
  covar_bin.push_back( { std::make_pair("kTrueDeltaPT_CCQE_Covar", std::make_pair(0.05, 0.10) ) } );
  covar_bin.push_back( { std::make_pair("kTrueDeltaPT_CCQE_Covar", std::make_pair(0.10, 0.15) ) } );
  covar_bin.push_back( { std::make_pair("kTrueDeltaPT_CCQE_Covar", std::make_pair(0.15, 0.20) ) } );
  covar_bin.push_back( { std::make_pair("kTrueDeltaPT_CCQE_Covar", std::make_pair(0.20, 0.25) ) } );
  covar_bin.push_back( { std::make_pair("kTrueDeltaPT_CCQE_Covar", std::make_pair(0.25, 0.35) ) } );
  covar_bin.push_back( { std::make_pair("kTrueDeltaPT_CCQE_Covar", std::make_pair(0.35, 0.55) ) } );
  covar_bin.push_back( { std::make_pair("kTrueDeltaPT_CCQE_Covar", std::make_pair(0.55, 0.85) ) } );
  covar_bin.push_back( { std::make_pair("kTrueDeltaPT_CCQE_Covar", std::make_pair(0.85, 2.00) ) } );

  Tree truthTreeCCQE ( "TruthCCQE", {"kTrueDeltaPT_CCQE"}, loader, {kTrueDeltaPT_CCQE_Covar}, kNoSpillCut, true );
  CovarianceMatrixTree covarTreeCCQE ( "CovarCCQE", {"kTrueDeltaPT_CCQE_Covar", "kFSINWeight_CCQE_Covar"}, "kFSINWeight_CCQE_Covar",
				       covar_bin, loader, {kTrueDeltaPT_CCQE_Covar, kFSINWeight_CCQE_Covar}, 100, kNoSpillCut );

  ///////////////////////////////////////////////////////////////////////////
  // Now that we're ready to go, let's go
  ///////////////////////////////////////////////////////////////////////////
  loader.Go();

  ///////////////////////////////////////////////////////////////////////////
  // Save the tree:
  ///////////////////////////////////////////////////////////////////////////
  TFile *f = new TFile("demo.root","recreate");

  selectedTreeSyst.MergeTree(selectedTree);
  selectedTreeSyst.SaveTo( f->mkdir("selectedTree") );
  selectedTreeSyst.SaveToTClonesArrays( f->mkdir("selectedTree_TClonesArrays") );

  truthTree.SaveTo( f->mkdir("truthTree") );
  truthTreeCCQE.SaveTo( f->mkdir("truthTreeCCQE") );

  covarTreeCCQE.PrintBinning();
  covarTreeCCQE.SaveToDebug( f->mkdir("covarTreeCCQE"), true );
}
