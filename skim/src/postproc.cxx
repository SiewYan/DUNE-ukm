#define postproc_cxx
#include "postproc.h"
#include "branches.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>

// User Memeber functions
//Int_t postproc::Cut(Long64_t entry)
//{
  // This function may be called from Loop.
  // returns  1 if entry is accepted.
  // returns -1 otherwise.

//  return 1;
//}

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
  
  TFile *f = TFile::Open( fout , "RECREATE" );
  TTree *newtree = fChain->CloneTree(0);

  // new branch
  std::vector<Float_t> genlist_endPointS;
  std::vector<Int_t> recoTrack_bestplane;
  
  std::vector<Int_t> recoTrack_bestplane_hits;
  std::vector<Int_t> recoTrack_bestplane_trkidtruth;
  std::vector<Int_t> recoTrack_bestplane_trkpidpdg;
  std::vector<Float_t> recoTrack_bestplane_resrg;
  std::vector<Float_t> recoTrack_bestplane_dedx;
  
  // add new branch
  newtree->Branch( "genlist_endPointS" , &genlist_endPointS );
  newtree->Branch( "recoTrack_bestplane" , &recoTrack_bestplane );
  newtree->Branch( "recoTrack_bestplane_hits" , &recoTrack_bestplane_hits );
  newtree->Branch( "recoTrack_bestplane_trkidtruth" , &recoTrack_bestplane_trkidtruth );
  newtree->Branch( "recoTrack_bestplane_trkpidpdg" , &recoTrack_bestplane_trkpidpdg );
  newtree->Branch( "recoTrack_bestplane_resrg" , &recoTrack_bestplane_resrg );
  newtree->Branch( "recoTrack_bestplane_dedx" , &recoTrack_bestplane_dedx );

  // EVENT LOOP
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    if(jentry % 20 == 0) { std::cout << "Number of event processed : " << jentry <<std::endl; }
    //std::cout << "Number of event processed : " << jentry <<std::endl;
    // if (Cut(ientry) < 0) continue;

    genlist_endPointS.clear(); 
    recoTrack_bestplane.clear(); 
    recoTrack_bestplane_hits.clear(); 
    recoTrack_bestplane_trkidtruth.clear();
    recoTrack_bestplane_trkpidpdg.clear();
    recoTrack_bestplane_resrg.clear(); 
    recoTrack_bestplane_dedx.clear();
    
    for (Int_t imc = 0 ; imc < geant_list_size ; imc++){
      genlist_endPointS.push_back(
				  (EndPointx_tpcAV[imc] - EndPointx[imc]) + 
				  (EndPointy_tpcAV[imc] - EndPointy[imc]) +
				  (EndPointz_tpcAV[imc] - EndPointz[imc])
				  );
    }
    
    // trkpidbestplane_pandoraTrack[ntracks_pandoraTrack]/S
    // ntrkhits_pandoraTrack[ntracks_pandoraTrack][3]/S
    // trkidtruth_pandoraTrack[ntracks_pandoraTrack][3]/I
    // trkpidpdg_pandoraTrack[ntracks_pandoraTrack][3]/I
    // trkresrg_pandoraTrack[ntracks_pandoraTrack][3][2000]/F
    // trkdedx_pandoraTrack[ntracks_pandoraTrack][3][2000]/F
    
    for (Int_t itrk = 0 ; itrk < ntracks_pandoraTrack; itrk++){
      
      Int_t ibestplane = trkpidbestplane_pandoraTrack[itrk];
      recoTrack_bestplane.push_back(ibestplane);
      recoTrack_bestplane_hits.push_back(ntrkhits_pandoraTrack[itrk][ibestplane]);
      recoTrack_bestplane_trkidtruth.push_back(trkidtruth_pandoraTrack[itrk][ibestplane]);
      recoTrack_bestplane_trkpidpdg.push_back(trkpidpdg_pandoraTrack[itrk][ibestplane]);

      for (Int_t j = 0 ; j < MAXSIZE ; j++){

	recoTrack_bestplane_resrg.push_back(trkresrg_pandoraTrack[itrk][ibestplane][j]);
	recoTrack_bestplane_dedx.push_back(trkdedx_pandoraTrack[itrk][ibestplane][j]);
      }
      
    } // ntracks_pandoraTrack
    
    newtree->Fill();
  } // jentry

  //newtree->Fill();
  //newtree->CopyEntries(fChain);
  turnOnBranches(outputBranch,newtree);
  TTree* tout = newtree->CloneTree(0);
  tout->CopyEntries(newtree);
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
