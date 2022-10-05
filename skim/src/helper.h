#define helper_h

//
std::vector<std::string> makeList(std::string textfile)
{
  std::ifstream file(textfile);
  std::string str;
  std::vector<std::string> vfout;
  //while (std::getline(file, str)) { vfout.push_back(str); }
  while (std::getline(file, str)) { if(str.find('#')==std::string::npos) vfout.push_back(str); }
  return vfout;
}

//
std::vector<std::string> joinVector( std::vector<std::string> &v1 , std::vector<std::string> &v2){
  std::vector<std::string> vout(v1.size() + v2.size());
  std::vector<std::string>::iterator it;
  
  it = set_union(v1.begin(), v1.end(), v2.begin(), v2.end(), vout.begin());
  return vout;
}

//
void postproc::turnOnBranches(std::vector<std::string> branchIn, TTree *tree_)
{
  tree_->SetBranchStatus("*",0);  // disable all branches
  for ( auto& branch : branchIn )
    tree_->SetBranchStatus( branch.c_str() , 1 );  // activate branchname
}

//
template <typename T>
bool contains(std::vector<T> vec, const T & elem)
{
  bool result = false;
  if( find(vec.begin(), vec.end(), elem) != vec.end() )
    {
      result = true;
    }
  return result;
}
