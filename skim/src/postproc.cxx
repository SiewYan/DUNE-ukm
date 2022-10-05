#define postproc_cxx
#include "postproc.h"
#include "helper.h"
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

//void postproc::turnOnBranches(std::vector<std::string> branchIn, TTree *tree_)
//{
//  tree_->SetBranchStatus("*",0);  // disable all branches
//  for ( auto& branch : branchIn )
//    tree_->SetBranchStatus( branch.c_str() , 1 );  // activate branchname
//}

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
  std::cout<< "nevent : " << nentries <<std::endl;

  std::vector<int> pdgid{ 13, 211, 321, 2212};

  std::vector<std::string> inbranches = makeList("data/inbranch.dat");
  std::vector<std::string> outbranches = makeList("data/outbranch.dat");
  turnOnBranches(inbranches,fChain);

  TFile *f = TFile::Open( fout , "RECREATE" );
  TTree *newtree = new TTree("newtree","postprocessing tree");

  // new branch
  int ngenpart;
  std::vector<Float_t> genpart_ds;
  std::vector<Int_t> genpart_pdg;
  std::vector<Int_t> genpart_status;
  std::vector<Int_t> genpart_trackId;
  // 0 : U plane ; 1 : V plane ; 2 : W plane
  //
  std::vector<Float_t> track_Uplane_cpdg_resrg;
  std::vector<Float_t> track_Uplane_cpdg_dedx;

  std::vector<Float_t> track_Uplane_wpdg_resrg;
  std::vector<Float_t> track_Uplane_wpdg_dedx;

  std::vector<Float_t> track_Vplane_cpdg_resrg;
  std::vector<Float_t> track_Vplane_cpdg_dedx;

  std::vector<Float_t> track_Vplane_wpdg_resrg;
  std::vector<Float_t> track_Vplane_wpdg_dedx;

  std::vector<Float_t> track_Wplane_cpdg_resrg;
  std::vector<Float_t> track_Wplane_cpdg_dedx;

  std::vector<Float_t> track_Wplane_wpdg_resrg;
  std::vector<Float_t> track_Wplane_wpdg_dedx;

  // add new branch
  newtree->Branch( "ngenpart" , &ngenpart );
  newtree->Branch( "genpart_ds" , &genpart_pdg );
  newtree->Branch( "genpart_pdg" , &genpart_pdg );
  newtree->Branch( "genpart_status" , &genpart_status );
  newtree->Branch( "genpart_trackId" , &genpart_trackId );

  newtree->Branch( "track_Uplane_cpdg_resrg" , &track_Uplane_cpdg_resrg );
  newtree->Branch( "track_Uplane_cpdg_dedx" , &track_Uplane_cpdg_dedx );
  newtree->Branch( "track_Uplane_wpdg_resrg" , &track_Uplane_wpdg_resrg );
  newtree->Branch( "track_Uplane_wpdg_dedx" , &track_Uplane_wpdg_dedx );

  newtree->Branch( "track_Vplane_cpdg_resrg" , &track_Vplane_cpdg_resrg );
  newtree->Branch( "track_Vplane_cpdg_dedx" , &track_Vplane_cpdg_dedx );
  newtree->Branch( "track_Vplane_wpdg_resrg" , &track_Vplane_wpdg_resrg );
  newtree->Branch( "track_Vplane_wpdg_dedx" , &track_Vplane_wpdg_dedx );

  newtree->Branch( "track_Wplane_cpdg_resrg" , &track_Wplane_cpdg_resrg );
  newtree->Branch( "track_Wplane_cpdg_dedx" , &track_Wplane_cpdg_dedx );
  newtree->Branch( "track_Wplane_wpdg_resrg" , &track_Wplane_wpdg_resrg );
  newtree->Branch( "track_Wplane_wpdg_dedx" , &track_Wplane_wpdg_dedx );

  // EVENT LOOP
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    if(jentry % 20 == 0) { std::cout << "Number of event processed : " << jentry <<std::endl; }
    //std::cout << "Number of event processed : " << jentry <<std::endl;
    // if (Cut(ientry) < 0) continue;

    ngenpart=0;
    genpart_ds.clear();
    genpart_pdg.clear();
    genpart_status.clear();
    genpart_trackId.clear();

    // Proton 2212
    track_Uplane_proton_cpdg_resrg.clear();
    track_Uplane_proton_cpdg_dedx.clear();

    track_Uplane_proton_wpdg_resrg.clear();
    track_Uplane_proton_wpdg_dedx.clear();

    track_Vplane_proton_cpdg_resrg.clear();
    track_Vplane_proton_cpdg_dedx.clear();

    track_Vplane_proton_wpdg_resrg.clear();
    track_Vplane_proton_wpdg_dedx.clear();

    track_Wplane_proton_cpdg_resrg.clear();
    track_Wplane_proton_cpdg_dedx.clear();

    track_Wplane_proton_wpdg_resrg.clear();
    track_Wplane_proton_wpdg_dedx.clear();

    // Muon 13
    track_Uplane_muon_cpdg_resrg.clear();
    track_Uplane_muon_cpdg_dedx.clear();

    track_Uplane_muon_wpdg_resrg.clear();
    track_Uplane_muon_wpdg_dedx.clear();

    track_Vplane_muon_cpdg_resrg.clear();
    track_Vplane_muon_cpdg_dedx.clear();

    track_Vplane_muon_wpdg_resrg.clear();
    track_Vplane_muon_wpdg_dedx.clear();

    track_Wplane_muon_cpdg_resrg.clear();
    track_Wplane_muon_cpdg_dedx.clear();

    track_Wplane_muon_wpdg_resrg.clear();
    track_Wplane_muon_wpdg_dedx.clear();

    // Kaon 321
    track_Uplane_kaon_cpdg_resrg.clear();
    track_Uplane_kaon_cpdg_dedx.clear();

    track_Uplane_kaon_wpdg_resrg.clear();
    track_Uplane_kaon_wpdg_dedx.clear();

    track_Vplane_kaon_cpdg_resrg.clear();
    track_Vplane_kaon_cpdg_dedx.clear();

    track_Vplane_kaon_wpdg_resrg.clear();
    track_Vplane_kaon_wpdg_dedx.clear();

    track_Wplane_kaon_cpdg_resrg.clear();
    track_Wplane_kaon_cpdg_dedx.clear();

    track_Wplane_kaon_wpdg_resrg.clear();
    track_Wplane_kaon_wpdg_dedx.clear();

    // Pion 211
    track_Uplane_pion_cpdg_resrg.clear();
    track_Uplane_pion_cpdg_dedx.clear();

    track_Uplane_pion_wpdg_resrg.clear();
    track_Uplane_pion_wpdg_dedx.clear();

    track_Vplane_pion_cpdg_resrg.clear();
    track_Vplane_pion_cpdg_dedx.clear();

    track_Vplane_pion_wpdg_resrg.clear();
    track_Vplane_pion_wpdg_dedx.clear();

    track_Wplane_pion_cpdg_resrg.clear();
    track_Wplane_pion_cpdg_dedx.clear();

    track_Wplane_pion_wpdg_resrg.clear();
    track_Wplane_pion_wpdg_dedx.clear();

    // COLLECTION 1 : MC INFORMATION
    for (Int_t imc = 0 ; imc < geant_list_size ; imc++){

      // looking only on muon, kaon, pion, proton
      if (!contains( pdgid , abs(pdg[imc]) )) continue;

      if ( inTPCActive[imc] != 1 ) continue;

      Float_t ds = abs(EndPointx_tpcAV[imc] - EndPointx[imc]) + abs(EndPointy_tpcAV[imc] - EndPointy[imc]) + abs(EndPointz_tpcAV[imc] - EndPointz[imc]);

      if (ds > 1e-10 ) continue;

      // Save the passing genlist
      ngenpart+=1;
      genpart_ds.push_back(ds);
      genpart_pdg.push_back(pdg[imc]);
      genpart_status.push_back(status[imc]);
      genpart_trackId.push_back(TrackId[imc]);

      // COLLECTION 2 : TRACK DATA
      for (Int_t itrk = 0 ; itrk < ntracks_pandoraTrack; itrk++){

	// there are total of three planes
	// COLLECTION 3 : PLANE DATA
	for (Int_t ipln = 0 ; ipln < 3; ipln++){

	  // GEANT track ID matched
	  if (TrackId[imc] != trkidtruth_pandoraTrack[itrk][ipln]) continue;

	  // at least good hits
	  if (ntrkhits_pandoraTrack[itrk][ipln]<5) continue;

	  // evaluate PdgId matches or not
	  Int_t isMatch = (abs(pdg[imc]) == abs(trkpdgtruth_pandoraTrack[itrk][ipln])) ? 1 : 0;

	  // get the dE/dx vs residual L points, and plots on respective PDG histos
	  // COLLECTION 4 : number of hits of the particular track on the particular plane
	  for (Int_t ihit=0 ; ihit<ntrkhits_pandoraTrack[itrk][ipln]; ihit++){
	    //
	    if (ipln==0){
	      // proton
	      if (abs(pdg[imc]) == 2212 ){

		if (isMatch){
		  track_Uplane_proton_cpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
		  track_Uplane_proton_cpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
		}
		else
		  {
		    track_Uplane_proton_wpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
		    track_Uplane_proton_wpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
		  }

	      }
	      // kaon
	      else if (abs(pdg[imc]) == 321){

		if (isMatch){
                  track_Uplane_kaon_cpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                  track_Uplane_kaon_cpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                }
                else
                  {
                    track_Uplane_kaon_wpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                    track_Uplane_kaon_wpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                  }

	      }
	      // pion
	      else if (abs(pdg[imc]) == 211){

		if (isMatch){
                  track_Uplane_pion_cpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                  track_Uplane_pion_cpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                }
                else
                  {
                    track_Uplane_pion_wpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                    track_Uplane_pion_wpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                  }
	      }
	      // muon
	      else if (abs(pdg[imc]) == 13){
		if (isMatch){
                  track_Uplane_muon_cpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                  track_Uplane_muon_cpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                }
                else
                  {
                    track_Uplane_muon_wpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                    track_Uplane_muon_wpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                  }
	      }
        //
        else if (ipln==1){
         // proton
         if (abs(pdg[imc]) == 2212 ){

     if (isMatch){
       track_Uplane_proton_cpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
       track_Uplane_proton_cpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
     }
     else
       {
         track_Uplane_proton_wpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
         track_Uplane_proton_wpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
       }

         }
         // kaon
         else if (abs(pdg[imc]) == 321){

     if (isMatch){
                    track_Uplane_kaon_cpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                    track_Uplane_kaon_cpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                  }
                  else
                    {
                      track_Uplane_kaon_wpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                      track_Uplane_kaon_wpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                    }

         }
         // pion
         else if (abs(pdg[imc]) == 211){

     if (isMatch){
                    track_Uplane_pion_cpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                    track_Uplane_pion_cpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                  }
                  else
                    {
                      track_Uplane_pion_wpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                      track_Uplane_pion_wpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                    }
         }
         // muon
         else if (abs(pdg[imc]) == 13){
     if (isMatch){
                    track_Uplane_muon_cpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                    track_Uplane_muon_cpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                  }
                  else
                    {
                      track_Uplane_muon_wpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                      track_Uplane_muon_wpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                    }
         }


         // third plane
         else if (ipln==2){
           // proton
           if (abs(pdg[imc]) == 2212 ){

       if (isMatch){
         track_Uplane_proton_cpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
         track_Uplane_proton_cpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
       }
       else
         {
           track_Uplane_proton_wpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
           track_Uplane_proton_wpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
         }

           }
           // kaon
           else if (abs(pdg[imc]) == 321){

       if (isMatch){
                     track_Uplane_kaon_cpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                     track_Uplane_kaon_cpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                   }
                   else
                     {
                       track_Uplane_kaon_wpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                       track_Uplane_kaon_wpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                     }

           }
           // pion
           else if (abs(pdg[imc]) == 211){

       if (isMatch){
                     track_Uplane_pion_cpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                     track_Uplane_pion_cpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                   }
                   else
                     {
                       track_Uplane_pion_wpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                       track_Uplane_pion_wpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                     }
           }
           // muon
           else if (abs(pdg[imc]) == 13){
       if (isMatch){
                     track_Uplane_muon_cpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                     track_Uplane_muon_cpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                   }
                   else
                     {
                       track_Uplane_muon_wpdg_resrg.push_back(trkresrg_pandoraTrack[itrk][ipln][ihit]);
                       track_Uplane_muon_wpdg_dedx.push_back(trkdedx_pandoraTrack[itrk][ipln][ihit]);
                     }
           }
	  }// number of hits from track on the plane
	} // 3 plane loop
      } // number of tracks
    } // genlist loop
    newtree->Fill();
  } // jentry

  //newtree->Fill();
  //newtree->CopyEntries(fChain);

  turnOnBranches(outbranches,newtree);

  //TTree* tout = newtree->CloneTree(0);
  //tout->CopyEntries(newtree);
  //tout->Write();
  newtree->Write();
  f->Close();
}

void postproc::Skim()
{
  if (fChain == 0) return;

  std::vector<std::string> inbranches = makeList("data/inbranch.dat");
  std::vector<std::string> outbranches = makeList("data/outbranch.dat");

  turnOnBranches(inbranches,fChain);
  //Long64_t nentries = fChain->GetEntriesFast();

  TFile *f = TFile::Open( fout , "RECREATE" );
  TTree *newtree = fChain->CloneTree(0);

  Long64_t nentries = fChain->GetEntries();
  std::cout<< "nevent : " << nentries <<std::endl;

  // EVENT LOOP
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentries;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    if(jentry % 20 == 0) { std::cout << "Number of event processed : " << jentry <<std::endl; }

    newtree->Fill();

  }
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
  TCLAP::SwitchArg             onlyskim ( "s" , "onlyskim"  , "perform dropping branches only" , cmd , false);
  TCLAP::ValueArg<std::string> inFile   ( "f" , "filelist" , "Dataset to be ran"   , true  , "dummy"         , "string" , cmd );
  TCLAP::ValueArg<std::string> outFile  ( "o" , "output"  , "Name used for output" , true  , "dummy"         , "string" , cmd );

  cmd.parse( argc, argv );

  std::cout<< "onlyskim  : "<<onlyskim.getValue()<<std::endl;
  std::cout<< "inFile    : "<<inFile.getValue()<<std::endl;
  std::cout<< "outFile   : "<<outFile.getValue()<<std::endl;

  std::vector<std::string> files = makeList(inFile.getValue());
  postproc* t = new postproc( files , outFile.getValue() );

  if (!onlyskim.getValue()){
    t->Loop();
  }
  else{
    std::cout<<" --> Skim ONLY"<<std::endl;
    t->Skim();
  }
  return 0;
}
