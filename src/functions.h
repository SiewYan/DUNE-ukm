#define functions_h

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
  "pdg",
  "TrackId",
  "trkpidbestplane_pandoraTrack",
  "ntrkhits_pandoraTrack",
  "trkidtruth_pandoraTrack",
  "trkpidpdg_pandoraTrack",
  "trkresrg_pandoraTrack",
  "trkdedx_pandoraTrack" ,
  "ds_tpcAV"
};


std::vector<std::string> makeList(std::string textfile)
{
  std::ifstream file(textfile);
  std::string str;
  std::vector<std::string> vfout;
  while (std::getline(file, str)) { vfout.push_back(str); }
  return vfout;
}
