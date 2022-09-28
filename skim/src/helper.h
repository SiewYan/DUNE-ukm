#define helper_h

/***
// branches to be processed
std::vector<std::string> inputBranch = {
  "geant_list_size",
  "pdg",
  "status",
  "EndPointx",
  "EndPointy",
  "EndPointz",
  "EndPointx_tpcAV",
  "EndPointy_tpcAV",
  "EndPointz_tpcAV",
  "TrackId",
  "trkpidbestplane_pandoraTrack",
  "ntrkhits_pandoraTrack",
  "trkidtruth_pandoraTrack",
  "trkpidpdg_pandoraTrack",
  "trkresrg_pandoraTrack",
  "trkdedx_pandoraTrack"
};

// branches to be saved
std::vector<std::string> outputBranch = {
  "run",
  "subrun",
  "event",
  "evttime",
  "beamtime",
  "taulife",
  "isdata",
  "geant_list_size",
  "genlist_pdg",
  "genlist_status",
  "genlist_trackId",
  "ntracks_pandoraTrack",
  "genlist_endPointS",
  "recoTrack_bestplane",
  "recoTrack_bestplane_hits",
  "recoTrack_bestplane_trkidtruth",
  "recoTrack_bestplane_trkpidpdg",
  "recoTrack_bestplane_resrg",
  "recoTrack_bestplane_dedx"
};
***/

std::vector<std::string> makeList(std::string textfile)
{
  std::ifstream file(textfile);
  std::string str;
  std::vector<std::string> vfout;
  //while (std::getline(file, str)) { vfout.push_back(str); }
  while (std::getline(file, str)) { if(str.find('#')==std::string::npos) vfout.push_back(str); }
  return vfout;
}

std::vector<std::string> joinVector( std::vector<std::string> &v1 , std::vector<std::string> &v2){
  std::vector<std::string> vout(v1.size() + v2.size());
  std::vector<std::string>::iterator it;
  
  it = set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), vout.begin());
  return vout;
}

std::vector<std::string> inbranches = makeList("data/inbranch.dat");
std::vector<std::string> variables = makeList("data/outbranch.dat");
std::vector<std::string> outbranches = joinVector(inbranches,variables);

void postproc::turnOnBranches(std::vector<std::string> branchIn, TTree *tree_)
{
  tree_->SetBranchStatus("*",0);  // disable all branches
  for ( auto& branch : branchIn )
    tree_->SetBranchStatus( branch.c_str() , 1 );  // activate branchname
}

