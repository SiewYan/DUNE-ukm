#include "encoder.h"

// file:///tmp/mozilla_siewyan0/AnalysisTree%20variables%20-%20uBooNE%20code%20-%20Fermilab%20Redmine.html

using namespace encoder;

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

  // look at muon
  df  = df.Filter( "abs(pdg) == 13" );

  // only want true particle which ends at TPC
  df = df.Define( "dx" , "EndPointx_tpcAV - EndPointx" )
    .Define( "dy" , "EndPointy_tpcAV - EndPointy" )
    .Define( "dz" , "EndPointz_tpcAV - EndPointz" )
    .Define( "ds" , "dx+dy+dz" )
    .Filter( "ds < 1e-10" , "ensure length is within 1e-10" )
    ;
  
  // get the indices of reco track where G4 TrackID match the MC's
  // HEREEEE
  df = getMatchedBit(df);

  // flatten information according to plane
  df = df.Define( "matchMCIdx" , "matchedBit.first.second" );

  // is this the best plane
  // trkpidbestplane_pandoraTrack[ntracks_pandoraTrack]
  df = df.Filter( "matchedBit.second >= 7" , "match all planes" );

  // loop over the number of wire planes (which is always 3: 0 is U plane, 1 is V plane, and 2 is W (or Z) plane
  // ntrkhits_pandoraTrack[ntracks_pandoraTrack][3]
  df = df
    .Define( "ntrkhits_Uplane" , "ntrkhits_pandoraTrack[matchMCIdx][0]" )
    .Define( "ntrkhits_Vplane" , "ntrkhits_pandoraTrack[matchMCIdx][1]" )
    .Define( "ntrkhits_Wplane" , "ntrkhits_pandoraTrack[matchMCIdx][2]" )
    .Filter( "ntrkhits_Uplnae > 5" )
    .Filter( "ntrkhits_Vplnae > 5" )
    .Filter( "ntrkhits_Wplnae > 5" )
    ;
  
  // is this a good track (>= goodTrackHits hits) ?
  df = df.Filter( "ntrkhits_pandoraTrack > 5" ); // ntrkhits_pandoraTrack[ntracks_pandoraTrack][3]

  // pdg of MC particle tracked the IDed reco track PDG?
  df = df.Filter( "abs(pdg) == trkpidpdg_pandoraTrack[0]" , "mc is tracking the reco in all plane" ); // trkpidpdg_pandoraTrack[ntracks_pandoraTrack][3]

  // 
}
