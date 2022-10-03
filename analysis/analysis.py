import argparse
import sys
import glob
import ROOT

parser = argparse.ArgumentParser(description='Cosmic muon dE/dx versus residual length analysis.',
                                 formatter_class=argparse.ArgumentDefaultsHelpFormatter)

parser.add_argument('infiles', metavar='anatree_rootfiles', type=glob.glob, nargs='+',
                    help='AnalysisTree ROOT file globs')
parser.add_argument('--good-track-hits', dest='goodTrackHits', type=int, default=5,
                    help='At least how many hits needed for good tracks')
parser.add_argument('-o', dest='outFileName', default='out_dune_cosmicmuon_analysis_Jan2022.root',
                    help='Output histogram file name')

args = parser.parse_args()

kMaxTrackHits = 2000

rootfilelist = []
for globlist in args.infiles:
  rootfilelist += globlist

# DEBUG: for debugging args
#print('Input file list:')
#for s in rootfilelist:
#  print(s)
#
#print('# of hits for good track: ', args.goodTrackHits)
#print('Output histogram file name: ', args.outFileName)
#sys.exit(77)
# end DEBUG

print('Number of input files:', len(rootfilelist))
print('Now building TChain.')

chain = ROOT.TChain("anatree")

for rootfn in rootfilelist:
  chain.AddFile(rootfn)

print('TChain built.')
print('Events in TChain:', chain.GetEntries())
print('Initializing histograms.')

#nx = 200
#xmin = 0.0
#xmax = 200.0
#ny = 200
#ymin = 0.0
#ymax = 50.0

# zoom1
nx = 200
xmin = 0.0
xmax = 40.0
ny = 200
ymin = 0.0
ymax = 15.0

colorcode = {13: 1, 211: 2, 321: 3, 2212: 4}
dataplane = { 0 : 'U-plane', 1 : 'V-plane', 2 : 'WZ-plane' } # X = W = Z

hDeDxPlot_Right = {}
hDeDxPlot_Wrong = {}

for i in colorcode:
  hDeDxPlot_Right[i] = ROOT.TH2F("hDeDxPlot_Right_PDG" + str(i),
                                 "dE/dx vs residual track length for correctly matched PDG (PDG " + str(i) + ")",
                                 nx, xmin, xmax, ny, ymin, ymax)
  hDeDxPlot_Wrong[i] = ROOT.TH2F("hDeDxPlot_Wrong_PDG" + str(i),
                                 "dE/dx vs residual track length for wrongly matched PDG (PDG " + str(i) + ")",
                                 nx, xmin, xmax, ny, ymin, ymax)

  

bestPlane = ROOT.TH1F( "bestPlane" , "best plane used for track" , 3 , -0.5 , 2.5  )
#pdgId = ROOT.TH1F( "pdgId" , "PdgId used in the study" , 20 , -2200 , 2200  )

print('Histograms initialized. Now looping through the chain.')

for k, ev in enumerate(chain):
  # print progress per 100 events
  if k % 100 == 0:
    print('Now processing entry', k)
  # convert everything into python lists for speed
  ###### GEANT MC information
  list_pdg = list(ev.pdg)                         # GEANT MC Particle
  list_intpcactive = list(ev.inTPCActive)         # did particle pass through TPC physical boundaries
  list_EndPointx = list(ev.EndPointx)             # ending 'X' position of the particle in cm
  list_EndPointy = list(ev.EndPointy)             #
  list_EndPointz = list(ev.EndPointz)             #
  list_EndPointx_tpcAV = list(ev.EndPointx_tpcAV) # 'X' position of particle's last step inside the TPC boundaries
  list_EndPointy_tpcAV = list(ev.EndPointy_tpcAV) #
  list_EndPointz_tpcAV = list(ev.EndPointz_tpcAV) #
  list_TrackId = list(ev.TrackId)                 # Track id assigned by GEANT4
  ###### PlaneData : [ntracks][3]
  list_trkidtruth = list(ev.trkidtruth_pandoraTrack) # true geant trackid assigned to the reconstructed track in a given plane
  #list_trkorigin = list(ev.trkorigin_pandoraTrack) # mc origin type: 0=unknown, 1=beam neutrino, 2=cosmic, 3=supernova neutrino, 4=??
  list_trkpdgtruth = list(ev.trkpdgtruth_pandoraTrack) # true pdg code associated to the particle with GEANT trackID @trkidtruth@
  list_trkpidpdg = list(ev.trkpidpdg_pandoraTrack) # true pdg code associated to the particle with GEANT trackID
  list_ntrkhits = list(ev.ntrkhits_pandoraTrack) # number of hits associated to the track in that plane #
  ### PID information : [ntracks][3]
  list_trkpidpdg = list(ev.trkpidpdg_pandoraTrack) # pdg code assigned by the @Chi2PIDAlg@ PID algorithm
  list_trkpidchi = list(ev.trkpidchi_pandoraTrack) # Minimum reduced Chi2 assigned by the @Chi2PIDAlg@ PID algorithm
  list_trkpidchipr = list(ev.trkpidchipr_pandoraTrack) # Chi-square for proton assigned by the @Chi2PIDAlg@ PID algorithm
  list_trkpidchika = list(ev.trkpidchika_pandoraTrack) # Chi-square for kaon assigned by the @Chi2PIDAlg@ PID algorithm
  list_trkpidchipi = list(ev.trkpidchipi_pandoraTrack) # Chi-square for pion assigned by the @Chi2PIDAlg@ PID algorithm
  list_trkpidchimu = list(ev.trkpidchimu_pandoraTrack) # Chi-square for muon assigned by the @Chi2PIDAlg@ PID algorithm
  list_trkpidpida = list(ev.trkpidpida_pandoraTrack) # PIDA value assigned to the track by Bruce's @PIDA@ algorithm
  list_trkke = list(ev.trkke_pandoraTrack) # kinetic energy of the track in each plane from calorimetry in MeV

  # Track Data : [ntracks] (ONLY)
  list_trkpidbestplane = list(ev.trkpidbestplane_pandoraTrack) # Currently, this is defined as the plane with most hits --> one array
  list_ntrk = list(ev.ntracks_pandoraTrack) # number of reconstructed tracks
  list_trkId = list(ev.trkId_pandoraTrack) # Geant track ID of the track
  list_trkcosmic = list(ev.trkcosmicscore_tagger_pandoraTrack) # assigned cosmic score by Geometry based tagger, 1 yes, 0 no

  # Hit Data : [ntracks][kMaxTrackHits]
  list_trkdedx = list(ev.trkdedx_pandoraTrack) # dedx of the track hit in a given plane (in MeV/cm)
  list_trkdqdx = list(ev.trkdqdx_pandoraTrack) # dqdx of the track hit in a given plane (in ADC/cm)
  list_trkresrg = list(ev.trkresrg_pandoraTrack) # range from end of track (in cm)

  # loop on the pdg
  for iMC, mcpdg in enumerate(list_pdg):
    # only look for MC muons, pions, kaons and protons
    if abs(mcpdg) not in colorcode: continue

    # only want true particle which ends at TPC
    dx = list_EndPointx_tpcAV[iMC] - list_EndPointx[iMC]
    dy = list_EndPointy_tpcAV[iMC] - list_EndPointy[iMC]
    dz = list_EndPointz_tpcAV[iMC] - list_EndPointz[iMC]
    if (dx+dy+dz > 1e-10): continue

    # did the gen particle end up in tpc?
    if list_intpcactive[iMC] !=1: continue

    # loop over number of track data
    for itrk in list_ntrk:
      
      # loop over plane data
      for ipln in range(0,3):
        # Check the assigned TRACK ID on both mc information and reconstructed track.
        if list_TrackId[iMC] != list_trkidtruth[itrk][ipln]: continue

        if list_ntrkhits[itrk][ipln] < args.goodTrackHits: continue

        isMatch = (abs(mcpdg) == abs(trkpdgtruth[itrk][ipln]))
        
        # now get the dE/dx vs residual L points and plot on respective PDG histos
        for j in range(kMaxTrackHits):
          x = list_trkresrg[itrk][kMaxTrackHits*i+j]
          y = list_trkdedx[itrk][kMaxTrackHits*i + j]
          if x > 0.0 or y > 0.0:
            if isMatch:
              hDeDxPlot_Right[abs(mcpdg)].Fill(x, y)
            else:
              hDeDxPlot_Wrong[abs(mcpdg)].Fill(x, y)

print('Done looping TChain. Now writing output file.')

outfile = ROOT.TFile(args.outFileName, "RECREATE")
for i in colorcode:
  hDeDxPlot_Right[i].Write()
  hDeDxPlot_Wrong[i].Write()
bestPlane.Write()
outfile.Close()

print('All done!')

