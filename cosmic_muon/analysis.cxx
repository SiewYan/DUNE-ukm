#include "ROOT/RDataFrame.hxx"
#include "ROOT/RDFHelpers.hxx"
#include "ROOT/RVec.hxx"

#include "Math/Vector4D.h"
#include "TStopwatch.h"

#include <iostream>
#include <sstream>
#include <fstream>
#include <stdlib.h>
#include <string>
#include <vector>
#include <cmath>
//#include <map>

#include "utility" // std::pair
#include <algorithm> // for std::find
#include <iterator> // for std::begin, std::end

#include "TRandom3.h"
#include "TLorentzVector.h"

template < typename T>
auto getMatchedBit( T &dfin ){

  using namespace ROOT::VecOps;
  
  auto matchedBit(
		RVec<int>& recotrkID,
		RVec<RVec<int>>& truthtrkID
		){
    
    auto Idxs = Combinations( recotrkID , truthtrkID );
    const auto rsize = Idxs[0].size();

    // bit1 : 1st matched ; bit2 : 2nd matched ; bit3 : 3rd matched
    // if all match --> 111 (in binary) => 1+2+4 = 7
    // (bit1) + 2*(bit2) + 4*(bit3)
    RVec<int> matchIdxbit;
    for (size_t i = 0 ; i < rsize ; i++ ){
      
      const auto ireco = Idxs[0][i];
      const auto itruth = Idxs[1][i];
      const auto vecplane = truthtrkID[itruth];
      
      // matching to truth info according to the plane
      int bit1 = ( recotrkID[ireco] == vecplane[0] ) ? 1 : 0;
      int bit2 = ( recotrkID[ireco] == vecplane[1] ) ? 1 : 0;
      int bit3 = ( recotrkID[ireco] == vecplane[2] ) ? 1 : 0;

      matchedBit.push_back( bit1 + 2*(bit2) + 4*(bit3) );
      
    }
    
    return dfin.Define( "matchedBit" , matchedBit , { "TrackId" , "trkidtruth_pandoraTrack" } );
  }


};

int main(int argc, char **argv) {

  const auto poolSize = ROOT::GetThreadPoolSize();
  ROOT::EnableImplicitMT(poolSize);

  if(argc != 4) {
    std::cout << "Use executable with following arguments: ./analysis name input output year" << std::endl;
    return -1;
  }

  const std::string name   = argv[1];
  const std::string input  = argv[2];
  const std::string output = argv[3];

  std::cout << ">>> Process input: " << input << std::endl;
  std::cout << ">>> Process output: " << output << std::endl;

  // Initialize time
  TStopwatch time;
  time.Start();

  // filelist
  std::vector<std::string> infiles;
  std::ifstream file(input);
  std::string str;
  while (std::getline(file, str)) { infiles.push_back(str); }
  
  //
  ROOT::RDataFrame df( "analysistree/anatree", infiles);

  // only want true particle which ends at TPC
  df = df.Define( "dx" , "EndPointx_tpcAV - EndPointx" )
    .Define( "dy" , "EndPointy_tpcAV - EndPointy" )
    .Define( "dz" , "EndPointz_tpcAV - EndPointz" )
    .Define( "ds" , "dx+dy+dz" )
    .Filter( "ds < 1e-10" , "ensure length is within 1e-10" )
    ;
  
  // get the indices of reco track where G4 TrackID match the MC's
  df = getMatchedBit(df);

  // is this the best plane
  df = df.Filter( "matchedBit >= 7" , "match all plane" );

  // is this a good track (>= goodTrackHits hits) ?
  df = df.Filter( "ntrkhits_pandoraTrack > 5" );

  // pdg of MC particle tracked the IDed reco track PDG?
  df = df.Filter( "abs(pdg) == trkpidpdg_pandoraTrack" ); // length of 3!! 

  // 
}
