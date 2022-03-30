#define postproc_cxx
#include "postproc.h"
#include "functions.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

// User Memeber functions
Int_t postproc::Cut(Long64_t entry)
{
  // This function may be called from Loop.
  // returns  1 if entry is accepted.
  // returns -1 otherwise.

  return 1;
}

void postproc::turnOnBranches(std::vector<std::string> branchIn, TTree *tree_)
{
  tree_->SetBranchStatus("*",0);  // disable all branches
  for ( auto& branch : branchIn )
    tree_->SetBranchStatus( branch.c_str() , 1 );  // activate branchname
}

void postproc::Loop()
{
//   In a ROOT session, you can do:
//      root> .L postProc.C
//      root> postProc t
//      root> t.GetEntry(12); // Fill t data members with entry number 12
//      root> t.Show();       // Show values of entry 12
//      root> t.Show(16);     // Read and show values of entry 16
//      root> t.Loop();       // Loop on all entries
//

//     This is the loop skeleton where:
//    jentry is the global entry number in the chain
//    ientry is the entry number in the current Tree
//  Note that the argument to GetEntry must be:
//    jentry for TChain::GetEntry
//    ientry for TTree::GetEntry and TBranch::GetEntry
//
//       To read only selected branches, Insert statements like:
// METHOD1:
//    fChain->SetBranchStatus("*",0);  // disable all branches
//    fChain->SetBranchStatus("branchname",1);  // activate branchname
// METHOD2: replace line
//    fChain->GetEntry(jentry);       //read all branches
//by  b_branchname->GetEntry(ientry); //read only this branch

  if (fChain == 0) return;
  
  //Long64_t nentries = fChain->GetEntriesFast();
  Long64_t nentries = fChain->GetEntries();
  std::cout<< "nentries : " << nentries <<std::endl;

  turnOnBranches(inputBranch,fChain);
  
  TFile *f = TFile::Open( fout , "recreate" );
  TTree *newtree = fChain->CloneTree(0);
  //newtree->CopyEntries(fChain);

  // new branch
  const Int_t MAXSIZE = 19000; //geant_list_size;
  Float_t ds_tpcAV[MAXSIZE] = {-9999.};
  
  // add new branch
  //newtree->Branch( "ntpcAV" , &ntpcAV , "ntpcAV/I" );
  newtree->Branch( "ds_tpcAV" , &ds_tpcAV , Form("ds_tpcAV[%i]/F",MAXSIZE)); //"ds_tpcAV[MAXSIZE]/F" );
  //newtree->Branch( "ds_tpcAV" , &ds_tpcAV );

  // EVENT LOOP
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    //if(jentry % 100 == 0) { std::cout << "Number of event processed : " << jentry <<std::endl; }
    std::cout << "Number of event processed : " << jentry <<std::endl;
    // if (Cut(ientry) < 0) continue;
    
    // true particle which ends at TPC
    //ntpcAV = static_cast<int>(pdg.size());
    Int_t ntpc = sizeof(pdg);
    for (Int_t imc = 0 ; imc < ntpc ; imc ++ ){
      Float_t dx = EndPointx_tpcAV[imc] - EndPointx[imc];
      Float_t dy = EndPointy_tpcAV[imc] - EndPointy[imc];
      Float_t dz = EndPointz_tpcAV[imc] - EndPointz[imc];
      ds_tpcAV[imc] = dx+dy+dz;
    }
    
    newtree->Fill();
  } // end of loop

  //newtree->Fill();
  //newtree->CopyEntries(fChain);
  turnOnBranches(outputBranch,newtree);
  tout = newtree->CloneTree(0);
  tout->Write();
  f->Close();
}

int main(int argc, char **argv) {

  // start time
  TStopwatch time;
  time.Start();

  //std::cout<<"argc : " << argc << std::endl;

  TCLAP::CmdLine cmd( "postproc" , ' ' , "0.1" );  
  TCLAP::ValueArg<std::string> inFile ( "f" , "filelist" , "Dataset to be ran"   , true , "dummy"         , "string" , cmd );
  TCLAP::ValueArg<std::string> outFile( "o" , "output"  , "Name used for output" , true , "dummy"         , "string" , cmd );

  cmd.parse( argc, argv );

  std::cout<< "inFile  : "<<inFile.getValue()<<std::endl;
  std::cout<< "outFile : "<<outFile.getValue()<<std::endl;

  std::vector<std::string> files = makeList(inFile.getValue());
  postproc* t = new postproc( files , outFile.getValue() );
  t->Loop();
  return 0;
}
