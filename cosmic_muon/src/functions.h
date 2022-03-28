#ifndef FUNCTIONS_H
#define FUNCTIONS_H

#include "header.h"

std::vector<std::string> outbranch = {};

/// count occurences of substrings
/// credit https://stackoverflow.com/questions/22489073/counting-the-number-of-occurrences-of-a-string-within-a-string
inline int count_substring(const std::string &str, const std::string &sub) {
  int nSub = 0;
  std::string::size_type iSub = 0;
  while ((iSub = str.find(sub, iSub)) != std::string::npos) {
    ++nSub;
    iSub += sub.length();
  }
    
  return nSub;
}
  
inline bool contain(const std::string &str, const std::string &sub) {
  return count_substring(str, sub) > 0;
}
  
// recursive function
auto ApplyDefines(RNode df , const std::vector<std::string> &names , unsigned int i = 0)
{

  using namespace ROOT::VecOps;
  
  if (i == names.size())
    return df;
  
  return ApplyDefines( 
		      df
		      .Define( names[i]+"_Uplane"  , [](const RVec<int>& c) { return c[0]; } , { names[i] } )
		      //.Define( names[i]+"_Vplane"  , [](const RVec<int[3]>& v) { return v[1]; } , {names[i]} )
		      //.Define( names[i]+"_WZplane" , [](const RVec<int[3]>& v) { return v[2]; } , {names[i]} )
		      , names , i + 1 );
}

// ntrkhits_pandoraTrack[ntracks_pandoraTrack][3]/S
// trkidtruth_pandoraTrack[ntracks_pandoraTrack][3]/I
// trkpidpdg_pandoraTrack[ntracks_pandoraTrack][3]/I
// trkresrg_pandoraTrack[ntracks_pandoraTrack][3][2000]/F
// trkdedx_pandoraTrack[ntracks_pandoraTrack][3][2000]/F


template <typename T>
auto flatten(T &dfin){
  
  using namespace ROOT::VecOps;
  
  auto flattening = [](
		       RVec<int> br1
		       ){
    RVec<int> out1,out2,out3;
    auto tsize = br1.size();
    for ( auto i = 0 ; i < tsize ; i++ ){
      for ( auto j = 0 ; j < 3 ; j++ ){
	if (j == 0) out1.push_back(br1[i+3*j]);
	if (j == 1) out2.push_back(br1[i+3*j]);
	if (j == 2) out3.push_back(br1[i+3*j]);
      }
    } //
    std::tuple<RVec<int>,RVec<int>,RVec<int>> out = { out1 , out2 , out3 };
  }; //
  
  dfin = dfin
    .Define( "flattening" , flattening , { "ntrkhits_pandoraTrack" } )
    .Define( "ntrkhits_pandoraTrack_UPlane" , "std::get<0>(flattening)" )
    .Define( "ntrkhits_pandoraTrack_VPlane" , "std::get<1>(flattening)" )
    .Define( "ntrkhits_pandoraTrack_WZPlane" , "std::get<2>(flattening)" )
    ;
  
  return dfin;
}

#endif
