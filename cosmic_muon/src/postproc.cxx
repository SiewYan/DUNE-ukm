#include "header.h"
#include "functions.h"

// pdg[geant_list_size]/I
// EndPointz[geant_list_size]/F
// EndPointz_tpcAV[geant_list_size]/F
// TrackId[geant_list_size]/I
// trkpidbestplane_pandoraTrack[ntracks_pandoraTrack]/S

// ntrkhits_pandoraTrack[ntracks_pandoraTrack][3]/S 
// trkidtruth_pandoraTrack[ntracks_pandoraTrack][3]/I
// trkpidpdg_pandoraTrack[ntracks_pandoraTrack][3]/I
// trkresrg_pandoraTrack[ntracks_pandoraTrack][3][2000]/F
// trkdedx_pandoraTrack[ntracks_pandoraTrack][3][2000]/F

std::vector<std::string> planeData = {
  "ntrkhits_pandoraTrack",
  "trkidtruth_pandoraTrack",
  "trkpidpdg_pandoraTrack",
  "trkresrg_pandoraTrack",
  "trkdedx_pandoraTrack"
};

// https://bitbucket.org/imranyusuff_HEP/dune_cosmicmuon_analysis_jan2022/src/master/dune_cosmicmuon_analysis_Jan2022.py

int main(int argc, char **argv) {

  const auto poolSize = ROOT::GetThreadPoolSize();

  if (poolSize == 0){
    std::cout << "Error, number of poolSize detected to be zero" << std::endl;
    std::cout << "IsImplicitMTEnabled() is : " << ROOT::IsImplicitMTEnabled() << std::endl;
    return -1;
  }
  
  ROOT::EnableImplicitMT(poolSize);

  if(argc != 4) {
    std::cout << "Use executable with following arguments: ./analysis name input output" << std::endl;
    return -1;
  }

  const std::string name   = argv[1];
  const std::string input  = argv[2];
  const std::string output = argv[3];

  std::cout << ">>> Process input       : " << input << std::endl;
  std::cout << ">>> Process output      : " << output << std::endl;
  std::cout << ">>> Number of core used : " << poolSize << std::endl;

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

  // skimming  
  // look at particle of interest
  auto df1  = df.Filter( "( abs(pdg) == 13 || abs(pdg) == 211 || abs(pdg) == 321 || abs(pdg) == 2212 )" , "select only muon, kaon, pion and proton" );

  // select true particle ends at TPC
  auto df2 = df1.Define( "dx" , "EndPointx_tpcAV - EndPointx" )
    .Define( "dy" , "EndPointy_tpcAV - EndPointy" )
    .Define( "dz" , "EndPointz_tpcAV - EndPointz" )
    .Define( "ds" , "dx+dy+dz" )
    .Filter( "ds<1e-10" , "ensure particle stop within tpc" )
    ;

  // flatten all planes
  auto df3 = ApplyDefines( df2 , planeData );
  
  // The plane number 0 corresponds to U, 1 corresponds to V and 2 corresponds to W (or Z)
  // bit1 : U plane is matched ;
  // bit2 : V plane is matched ;
  // bit3 : W or Z  plane is matched
  // all match --> 111 (in binary) => 1+2+4 = 7
  // (bit1) + 2*(bit2) + 4*(bit3)  
  //df = df.Define( "pandora_bestplanebit" , "trkpidbestplane_pandoraTrack[0] + 2*(trkpidbestplane_pandoraTrack[1]) + 4*(trkpidbestplane_pandoraTrack[2])" );
  //df = Matching(df);

  auto dfout = df3;
  dfout.Snapshot( "tpc" , output , dfout.GetDefinedColumnNames() );
  
  ROOT::RDF::SaveGraph( dfout ,"graph_tpc.dot");
  
  auto report = dfout.Report();
  report->Print();
  
  time.Stop();
  time.Print();

  // 
}
