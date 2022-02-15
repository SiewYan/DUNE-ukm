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
auto getmatchedIdx( T &df ){

  using namespace ROOT::VecOps;
  
  auto matchIdx(
		RVec<int>& recotrkID,
		RVec<RVec<int>>& truthtrkID
		){
    // save truth Idx
    RVec<int> matchIdx;
    auto Idxs = Combinations( recotrkID , truthtrkID );
    const auto rsize = Idxs[0].size();
    
    for (size_t i = 0 ; i < rsize ; i++ ){
      const auto ireco = Idxs[0];
      const auto itruth = Idxs[1];

      //
    }

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
  

}
