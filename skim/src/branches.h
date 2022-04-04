#define branches_h

// branches to be processed
std::vector<std::string> inputBranch = {
  "geant_list_size",
  "pdg",
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
  "geant_list_size",
  "ntracks_pandoraTrack",
  "genlist_endPointS",
  "recoTrack_bestplane",
  "recoTrack_bestplane_hits",
  "recoTrack_bestplane_trkidtruth",
  "recoTrack_bestplane_trkpidpdg",
  "recoTrack_bestplane_resrg",
  "recoTrack_bestplane_dedx"
};

// helper
// appending files
std::vector<std::string> makeList(std::string textfile)
{
  std::ifstream file(textfile);
  std::string str;
  std::vector<std::string> vfout;
  while (std::getline(file, str)) { vfout.push_back(str); }
  return vfout;
}
