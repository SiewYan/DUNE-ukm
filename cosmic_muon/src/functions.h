#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "header.h"

std::vector<std::string> outbranch = {};

//template <typename T = std::string>
//  auto saveBranch(T branch ){
//  outbranch.push_back(branch);
//}
  
// recursive function
auto ApplyDefines(RNode df , const std::vector<std::string> &names , unsigned int i = 0)
{
  
  if (i == names.size())
    return df;
  
  return ApplyDefines( 
		      df
		      .Define( names[i]+"_Uplane"  , names[i]+"[0]" ) 
		      .Define( names[i]+"_Vplane"  , names[i]+"[1]" )
		      .Define( names[i]+"_WZplane" , names[i]+"[2]" )
		      , names , i + 1 );
}

/**
   template <typename T>
auto Matching(T v){

  using namespace ROOT::VecOps;
  
  auto doMatching = [](
		       RVec<int>& recotrk_ID,
		       RVec<int>& recotrk_pdgId,
		       RVec<RVec<int>>& pandoratrk_ID,
		       RVec<int>& pandoratrk_pdgId
		       ){
    
    auto Idxs = Combinations( recotrk_ID , pandoratrk_ID );
    const auto rsize = Idxs[0].size();
    const auto osize = pandoratrk_ID.size();
    
    std::tuple<RVec<int>,RVec<std::pair<int,int>>,RVec<int>> out;

    RVec<int> pandtrk_matchID_idx, pandtrk_matchpdgId_idx;
    RVec<int> pandtrk_bestplane;
    
    for (size_t i = 0 ; i < rsize ; i++ ){
      
      const auto ireco = Idxs[0][i];
      const auto itruth = Idxs[1][i];
      
      // track ID
      auto reco_trkID = recotrk_ID[ireco];
      auto pandora_trkID = pandoratrk_ID[itruth];
      
      // track matched pdgId
      auto reco_pdgId = abs(recotrk_pdgId[ireco]);
      auto pandora_pdgId = abs(pandoratrk_pdgId[itruth]);
      
      if ( reco_trkID == pandora_trkID ) matchID_idx.push_back(itruth);
      if ( reco_pdgId == pandora_pdgId ) matchpdgId_idx.push_back(itruth);
      
    } //
    out = {  , pandtrk_pdgId };
  } //
  
  return dfin
    .Define( "doMatching" , "postproc" , { "TrackId" , "trkidtruth_pandoraTrack" , "pdg" , "trkpidpdg_pandoraTrack" } )
    .Define( "trkID_isMatch" , "std::get<0>(postproc)" )
    .Define( "trkpdgId_isMatch" , "std::get<1>(postproc).first" )
    .Define( "trkpdgId_Match" , "std::get<1>(postproc).second" )
    .Define( "trkPlanebit_isMatch" , "std::get<2>(postproc)" )
    ;
}
**/

#endif
