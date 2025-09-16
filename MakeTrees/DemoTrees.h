#include "sbnana/CAFAna/Core/SpectrumLoader.h"
#include "sbnana/CAFAna/Core/Spectrum.h"
#include "sbnana/CAFAna/Core/Tree.h"
#include "sbnana/CAFAna/Core/Binning.h"

#include "sbnanaobj/StandardRecord/Proxy/SRProxy.h"

#include "sbnana/CAFAna/Systs/UniverseOracle.h"
#include "sbnana/CAFAna/Systs/SBNWeightSysts.h"

#include "TFile.h"
#include "TTree.h"
#include "TVector3.h"

using namespace ana;

// a lot of these are meant to be as in our CAFAna code from xsec analysis, though changing the signal region to not explicitly disallow other particles. Just 1muNp

// GENERAL:
////////////

bool isInAV (double x, double y, double z)
{
  if ( std::isnan(x) || std::isnan(y) || std::isnan(z) ) return false;

  return (( ( x < -61.94 && x > -358.49 ) ||
	    ( x >  61.94 && x <  358.49 )) &&
	  ( ( y > -181.86 && y < 134.96 ) &&
	    ( z > -894.95 && z < 894.95 ) ));
}
  
bool isInFV (double x, double y, double z)
{
  if ( std::isnan(x) || std::isnan(y) || std::isnan(z) ) return false;

  return (( ( x < -61.94 - 25 && x > -358.49 + 25 ) ||
	    ( x >  61.94 + 25 && x <  358.49 - 25 )) &&
	  ( ( y > -181.86 + 25 && y < 134.96 - 25 ) &&
	    ( z > -894.95 + 30 && z < 894.95 - 50 ) ));
}

bool isContainedVol (double x, double y, double z)
{
  if ( std::isnan(x) || std::isnan(y) || std::isnan(z) ) return false;

  return (( ( x < -61.94 - 10. && x > -358.49 + 10. ) ||
	    ( x >  61.94 + 10. && x <  358.49 - 10. )) &&
	  ( ( y > -181.86 + 10. && y < 134.96 - 10. ) &&
	    ( z > -894.95 + 10. && z < 894.95 - 10. ) ));
}

bool IsValidTrkIdx( const caf::SRSliceProxy* slice, const unsigned int idxTrk ) {
  return slice->reco.npfp > idxTrk;
}

bool IsTracklikeTrack( const caf::SRSliceProxy* slice, const unsigned int idxTrk ) {
  return (!std::isnan(slice->reco.pfp.at(idxTrk).trackScore) && slice->reco.pfp.at(idxTrk).trackScore > 0.45);
}

bool IsShowerlike( const caf::SRSliceProxy* slice, const unsigned int idxShw ) {
  return (!std::isnan(slice->reco.pfp.at(idxShw).trackScore) && slice->reco.pfp.at(idxShw).trackScore > 0. && slice->reco.pfp.at(idxShw).trackScore <= 0.45 );
}

bool IsPrimaryPFP( const caf::SRSliceProxy* slice, const unsigned int idxTrk ) {
  return slice->reco.pfp.at(idxTrk).parent_is_primary;
}

/////////////////////////////////////////////////////////////////////////////////////////////////

// TRUTH INFO FOR COVARIANCE MATRIX:
////////////

double CalcTKI_deltaPT(const TVector3 vec_p_mu, const TVector3 vec_p_pro, const TVector3 vec_p_nu){

  TVector3 unit_vec_p_nu = vec_p_nu.Unit();

  // Get transverse momenta w.r.t. the neutrino direction
  TVector3 pt_mu = vec_p_mu - (vec_p_mu.Dot(unit_vec_p_nu))*unit_vec_p_nu ;
  TVector3 pt_pro = vec_p_pro - (vec_p_pro.Dot(unit_vec_p_nu))*unit_vec_p_nu;

  TVector3 vec_deltaPT = pt_mu+pt_pro;

  return vec_deltaPT.Mag();

}

const TruthVar kTruth_MuonIndex([](const caf::SRTrueInteractionProxy *nu) -> int {
    double max_E(-999);
    int truth_idx(-1);
    for(std::size_t i(0); i < nu->prim.size(); ++i){
      // primary
      if( nu->prim.at(i).start_process!=0 ) continue;
      // muon
      if( abs(nu->prim.at(i).pdg)!=13 ) continue;
      // non-nan genE
      if(isnan(nu->prim.at(i).genE)) continue;

      double this_E = nu->prim.at(i).genE;
      // if larger E, update
      if(this_E>max_E){
        max_E = this_E;
        truth_idx = i;
      }
    }
    return truth_idx;
  });

const TruthVar kTruth_ProtonIndex([](const caf::SRTrueInteractionProxy *nu) -> int {
    double max_E(-999);
    int truth_idx(-1);
    for(std::size_t i(0); i < nu->prim.size(); ++i){
      // primary
      if( nu->prim.at(i).start_process!=0 ) continue;
      // proton
      if( abs(nu->prim.at(i).pdg)!=2212 ) continue;
      // non-nan genE
      if(isnan(nu->prim.at(i).genE)) continue;

      double this_E = nu->prim.at(i).genE;
      // if larger E, update
      if(this_E>max_E){
	max_E = this_E;
        truth_idx = i;
      }
    }
    return truth_idx;
  });

const TruthVar kTruth_deltaPT([](const caf::SRTrueInteractionProxy *nu) -> double {
    double ret(-5.f);

    int truth_mu_idx = kTruth_MuonIndex(nu);
    int truth_pro_idx = kTruth_ProtonIndex(nu);
    if(truth_mu_idx>=0 && truth_pro_idx>=0){

      const auto& p_mu = nu->prim.at(truth_mu_idx).genp;
      const auto& p_pro = nu->prim.at(truth_pro_idx).genp;
      const auto& p_nu = nu->momentum;

      TVector3 vec_p_mu(p_mu.x, p_mu.y, p_mu.z);
      TVector3 vec_p_pro(p_pro.x, p_pro.y, p_pro.z);
      TVector3 vec_p_n(p_nu.x, p_nu.y, p_nu.z);

      ret =  CalcTKI_deltaPT(vec_p_mu, vec_p_pro, vec_p_n);

    }

    return ret;
});

const SpillMultiVar kTrueDeltaPT_CCQE_Covar([](const caf::SRSpillProxy* sr) {
    std::vector<double> DeltaPTVec;

    for ( auto const& nu : sr->mc.nu ) {
      if ( abs(nu.pdg) != 14 ||
	   !nu.iscc ||
	   std::isnan(nu.position.x) || std::isnan(nu.position.y) || std::isnan(nu.position.z) ||
	   !isInFV(nu.position.x, nu.position.y, nu.position.z) )
        continue;

      if ( nu.genie_mode != 0 ) continue;

      DeltaPTVec.push_back(kTruth_deltaPT(&nu));
    }

    return DeltaPTVec;
});

const SpillMultiVar kFSINWeight_CCQE_Covar([](const caf::SRSpillProxy* sr) {
    std::vector<double> weights;

    const UniverseOracle& uo = UniverseOracle::Instance();
    const unsigned int psetIdx = uo.ParameterSetIndex("GENIEReWeight_ICARUS_v2_multisim_FSI_N_VariationResponse");

    for ( auto const& nu : sr->mc.nu ) {
      if ( abs(nu.pdg) != 14 ||
           !nu.iscc ||
           std::isnan(nu.position.x) || std::isnan(nu.position.y) || std::isnan(nu.position.z) ||
           !isInFV(nu.position.x, nu.position.y, nu.position.z) )
        continue;

      if ( nu.genie_mode != 0 ) continue;

      const caf::Proxy<std::vector<caf::SRMultiverse>>& wgts = nu.wgt;
      if ( wgts.empty() ) {
	for ( unsigned int idx=0; idx < 100; ++idx ) weights.push_back(1.);
      }
      else {
        if ( wgts[psetIdx].univ.size() != 100 ) std::cout << "IMPOSING 100 UNIVERSES WHEN THERE ARE ACTUALLY " << wgts[psetIdx].univ.size() << std::endl;
        for ( unsigned int idx=0; idx < 100; ++idx ) weights.push_back( wgts[psetIdx].univ[idx] );
      }
    }

    return weights;
});

// TRUE:
////////////

// Signal definition -- modified to not require 0pi & others
bool Is1muNp(const caf::Proxy<caf::SRTrueInteraction>& true_int, bool ApplyPhaseSpcaeCut){
  if ( true_int.index < 0 ) return false;

  if ( abs(true_int.pdg) != 14 ||
         !true_int.iscc ||
       std::isnan(true_int.position.x) || std::isnan(true_int.position.y) || std::isnan(true_int.position.z) ||
       !isInFV(true_int.position.x, true_int.position.y, true_int.position.z) )
    return false; // not signal

  unsigned int nMu(0), nP(0), nPi(0);
  unsigned int genieNPhotons(0), genieNMesons(0), genieNBaryonsAndPi0(0);
  double maxMomentumP = 0.;
  bool passProtonPCut = false;
  for ( auto const& prim : true_int.prim ) {
    if ( prim.start_process != 0 ) continue;

    double momentum = sqrt( (prim.genp.x*prim.genp.x) + (prim.genp.y*prim.genp.y) + (prim.genp.z*prim.genp.z) );

    bool PassMuonPCut = (momentum > 0.226);
    if ( abs(prim.pdg) == 13 ) {
      if (ApplyPhaseSpcaeCut ? PassMuonPCut : true) nMu+=1;
    }

    if ( abs(prim.pdg) == 2212 ) {
      nP+=1;
      if ( momentum > maxMomentumP ) {
	maxMomentumP = momentum;
	passProtonPCut = (momentum > 0.4 && momentum < 1.);
      }
    }

    if ( abs(prim.pdg) == 111 || abs(prim.pdg) == 211 ) nPi+=1;
    // CHECK A SIMILAR DEFINITION AS MINERVA FOR EXTRA REJECTION OF UNWANTED THINGS IN SIGNAL DEFN.
    if ( abs(prim.pdg) == 22 && prim.startE > 0.01 ) genieNPhotons+=1;
    else if ( abs(prim.pdg) == 211 || abs(prim.pdg) == 321 || abs(prim.pdg) == 323 ||
                prim.pdg == 111 || prim.pdg == 130 || prim.pdg == 310 || prim.pdg == 311 ||
	      prim.pdg == 313 || abs(prim.pdg) == 221 || abs(prim.pdg) == 331 ) genieNMesons+=1;
    else if ( prim.pdg == 3112 || prim.pdg == 3122 || prim.pdg == 3212 || prim.pdg == 3222 ||
                prim.pdg == 4112 || prim.pdg == 4122 || prim.pdg == 4212 || prim.pdg == 4222 ||
	      prim.pdg == 411 || prim.pdg == 421 || prim.pdg == 111 ) genieNBaryonsAndPi0+=1;
  }

  if ( nMu!=1 || nP==0 /*|| nPi > 0 || genieNPhotons > 0 || genieNMesons > 0 || genieNBaryonsAndPi0 > 0*/ ) {
    return false;
  }
  else if ( ApplyPhaseSpcaeCut ) return passProtonPCut; 

  return true;
}

const TruthVar kTruth_SignalType([](const caf::SRTrueInteractionProxy *nu) -> int {
    if ( Is1muNp(*nu, true) ) return 1;
    else if ( Is1muNp(*nu, false) ) return 2;
    else return 0; // not signal
});

const TruthCut kTruthCutIsSignal = kTruth_SignalType == 1;
const TruthCut kTruthCutIsSignalLike = kTruth_SignalType > 0;

// Cosine of angle between mu and leading p, true
const TruthVar kTruth_CosThMuonProton([](const caf::SRTrueInteractionProxy *nu) -> double {
    double ret(-5.f);

    int truth_mu_idx = kTruth_MuonIndex(nu);
    int truth_pro_idx = kTruth_ProtonIndex(nu);
    if(truth_mu_idx>=0 && truth_pro_idx>=0){

      const auto& p_mu = nu->prim.at(truth_mu_idx).genp;
      const auto& p_pro = nu->prim.at(truth_pro_idx).genp;

      TVector3 vec_p_mu(p_mu.x, p_mu.y, p_mu.z);
      TVector3 vec_p_pro(p_pro.x, p_pro.y, p_pro.z);

      ret = vec_p_mu.Unit().Dot( vec_p_pro.Unit() );

    }

    return ret;
  });

const Var kNuMITrueCosThMuP([](const caf::SRSliceProxy* slc) -> float {
    float costh(-5.f);
    if ( slc->truth.index >= 0 ) costh = kTruth_CosThMuonProton(&slc->truth);

    return costh;
  });


/////////////////////////////////////////////////////////////////////////////////////////////////


// RECO:
////////////

// Truth match type
// CutType; 1=Signal, 2=OOPS, 3=OtherCC, 4=NuNC, 5=NotNu
const Cut kNuMI_1muNpStudy_Signal_WithoutPhaseSpaceCut([](const caf::SRSliceProxy* slc) {
    return Is1muNp(slc->truth, false);
  });

const Cut kNuMI_1muNpStudy_Signal_WithPhaseSpaceCut([](const caf::SRSliceProxy* slc) {
    return Is1muNp(slc->truth, true);
  });

const Cut kNuMI_1muNpStudy_Signal_FailPhaseSpaceCut = kNuMI_1muNpStudy_Signal_WithoutPhaseSpaceCut && // signal,
                                                      !kNuMI_1muNpStudy_Signal_WithPhaseSpaceCut; // but fail phase cut

const Cut kNuMI_1muNpStudy_OtherNuCC([](const caf::SRSliceProxy* slc) {
    if ( slc->truth.index < 0 ) return false; // not Nu
    if ( !slc->truth.iscc ) return false; // not CC
    if ( kNuMI_1muNpStudy_Signal_WithoutPhaseSpaceCut(slc) ) return false; // covered by signal without phase space cut
    return true;
  });

const Cut kNuMI_IsSliceNuNC([](const caf::SRSliceProxy* slc) {
    if ( slc->truth.index < 0 ) return false;

    if ( slc->truth.iscc )
      return false; // IS CC

    return true;
  });

const Cut kNuMI_IsSlcNotNu([](const caf::SRSliceProxy* slc) {
    return ( slc->truth.index < 0 );
  });

const Var kNuMISliceSignalType([](const caf::SRSliceProxy* slc) -> int {
    if ( kNuMI_1muNpStudy_Signal_WithPhaseSpaceCut(slc) ) return 1; // Signal (with phase space cut)
    else if ( kNuMI_1muNpStudy_Signal_FailPhaseSpaceCut(slc) ) return 2; // Signal but out of phase space cut (OOPS)
    else if ( kNuMI_1muNpStudy_OtherNuCC(slc) ) return 3; // CC but not (signal without phase space cut)
    else if ( kNuMI_IsSliceNuNC(slc) ) return 4; // NC
    else if ( kNuMI_IsSlcNotNu(slc) ) return 5; // Not nu-slice Cosmic
    else return 0;
  });

// Is the slice vertex in the fiducial volume?
const Cut kNuMIVertexInFV([](const caf::SRSliceProxy* slc) {
    if ( std::isnan(slc->vertex.x) || std::isnan(slc->vertex.y) || std::isnan(slc->vertex.z) ) return false;
    return isInFV(slc->vertex.x,slc->vertex.y,slc->vertex.z);
  });

// Is the slice considered a clear cosmic
const Cut kNuMINotClearCosmic([](const caf::SRSliceProxy* slc) {
    return !slc->is_clear_cosmic;
  });

// Do we have a muon candidate?
const Var kNuMIMuonCandidateIdx([](const caf::SRSliceProxy* slc) -> int {
    float Longest(0);
    int PTrackInd(-1);

    unsigned int idxTrk = 0;
    while ( IsValidTrkIdx(slc, idxTrk) ) {
      if ( !IsTracklikeTrack(slc, idxTrk) ) { 
	idxTrk+=1;
	continue;
      }
      auto const& trk = slc->reco.pfp.at(idxTrk).trk;
      unsigned int thisIdx = idxTrk;
      idxTrk+=1;

      if ( std::isnan(trk.start.x) || std::isnan(trk.len) || trk.len <= 0. ) continue;
      if ( std::isnan(slc->vertex.x) || std::isnan(slc->vertex.y) || std::isnan(slc->vertex.z) ) return -1;
      const float Atslc = std::hypot(slc->vertex.x - trk.start.x,
				     slc->vertex.y - trk.start.y,
				     slc->vertex.z - trk.start.z);
      const bool isPrimCandidate = (Atslc < 10. && IsPrimaryPFP(slc,thisIdx));

      if ( !isPrimCandidate || trk.calo[2].nhit < 5 ) continue;
      const bool Contained = isContainedVol(trk.end.x,trk.end.y,trk.end.z);
      const float Chi2Proton = trk.chi2pid[2].chi2_proton;
      const float Chi2Muon = trk.chi2pid[2].chi2_muon;
      if ( (!Contained && trk.len > 50.) || (Contained && trk.len > 50. && Chi2Proton > 60. && Chi2Muon>0. && Chi2Muon < 30.) ) {
	if ( trk.len <= Longest ) continue;
	Longest = trk.len;
	PTrackInd = thisIdx;
      }
    }

    return PTrackInd;
  });

const Cut kNuMIHasMuonCandidate([](const caf::SRSliceProxy* slc) {
    return ( kNuMIMuonCandidateIdx(slc) >= 0 );
  });

// Do we have a proton candidate
const Var kNuMIProtonCandidateIdx([](const caf::SRSliceProxy* slc) -> int {
    int primaryInd = kNuMIMuonCandidateIdx(slc);

    float Longest(0);
    int PTrackInd(-1);

    unsigned int idxTrk = 0;
    while ( IsValidTrkIdx(slc, idxTrk) ) {
      int thisIdxInt = idxTrk;
      if ( thisIdxInt == primaryInd ) {
        idxTrk+=1;
        continue; // skip the particle which is the muon candidate!
      }
      if ( !IsTracklikeTrack(slc, idxTrk) ) { 
        idxTrk+=1;
        continue;
      }
      auto const& trk = slc->reco.pfp.at(idxTrk).trk; //GetTrack( slc, idxTrk );
      unsigned int thisIdx = idxTrk;
      idxTrk+=1;

      if ( std::isnan(trk.start.x) || std::isnan(trk.len) || trk.len <= 0. ) continue;
      if ( std::isnan(slc->vertex.x) || std::isnan(slc->vertex.y) || std::isnan(slc->vertex.z) ) return -1;
      const float Atslc = std::hypot(slc->vertex.x - trk.start.x,
                                     slc->vertex.y - trk.start.y,
                                     slc->vertex.z - trk.start.z);
      const bool isPrimCandidate = (Atslc < 10. && IsPrimaryPFP(slc,thisIdx));

      if ( !isPrimCandidate || trk.calo[2].nhit < 5 ) continue;
      const bool Contained = isContainedVol(trk.end.x,trk.end.y,trk.end.z);
      const float Chi2Proton = trk.chi2pid[2].chi2_proton;
      const float Chi2Muon = trk.chi2pid[2].chi2_muon;

      if ( Contained && Chi2Proton>0. && Chi2Proton < 90. && Chi2Muon > 30. ) {
        if ( trk.len <= Longest ) continue;
        Longest = trk.len;
        PTrackInd = thisIdx;
      }
    }

    return PTrackInd;
  });

const Cut kNuMIHasProtonCandidate([](const caf::SRSliceProxy* slc) {
    return ( kNuMIProtonCandidateIdx(slc) >= 0 );
  });

const Cut kNuMIProtonCandidateRecoPTreshold([](const caf::SRSliceProxy* slc) {
    float p(-5.f);

    if ( kNuMIProtonCandidateIdx(slc) >= 0 )
      {
	auto const& trk = slc->reco.pfp.at(kNuMIProtonCandidateIdx(slc)).trk;
	p = trk.rangeP.p_proton;
      }

    return (p > 0.4 && p < 1.0);
  });

const Cut kSelectionCut = kNuMIVertexInFV && kNuMINotClearCosmic && kNuMIHasMuonCandidate &&
                          kNuMIHasProtonCandidate && kNuMIProtonCandidateRecoPTreshold;

// Cosine of angle between mu and leading p, reco                                                                                                                                 
const Var kNuMIRecoCosThMuP([](const caf::SRSliceProxy* slc) -> float {
    float costh(-5.f);

    if ( kNuMIMuonCandidateIdx(slc) >= 0 && kNuMIProtonCandidateIdx(slc) >= 0 ) {
      auto const& mutrk = slc->reco.pfp.at(kNuMIMuonCandidateIdx(slc)).trk;
      auto const& ptrk = slc->reco.pfp.at(kNuMIProtonCandidateIdx(slc)).trk;

      TVector3 muDir(mutrk.dir.x, mutrk.dir.y, mutrk.dir.z);
      muDir = muDir.Unit();
      TVector3 pDir(ptrk.dir.x, ptrk.dir.y, ptrk.dir.z);
      pDir = pDir.Unit();

      costh = TMath::Cos( muDir.Angle(pDir) );
    }

    return costh;
  });

