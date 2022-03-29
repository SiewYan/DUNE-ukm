#define functions_h

std::vector<std::string> makeList(std::string textfile)
{
  std::ifstream file(textfile);
  std::string str;
  std::vector<std::string> vfout;
  while (std::getline(file, str)) { vfout.push_back(str); }
  return vfout;
}
