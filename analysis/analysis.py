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
  '''
  list_pdg = list(ev.pdg)
  list_EndPointx = list(ev.EndPointx)
  list_EndPointy = list(ev.EndPointy)
  list_EndPointz = list(ev.EndPointz)
  list_EndPointx_tpcAV = list(ev.EndPointx_tpcAV)
  list_EndPointy_tpcAV = list(ev.EndPointy_tpcAV)
  list_EndPointz_tpcAV = list(ev.EndPointz_tpcAV)
  list_TrackId = list(ev.TrackId)
  list_trkpidbestplane = list(ev.trkpidbestplane_pandoraTrack)
  list_ntrkhits = list(ev.ntrkhits_pandoraTrack)
  list_trkidtruth = list(ev.trkidtruth_pandoraTrack)
  list_trkpidpdg = list(ev.trkpidpdg_pandoraTrack)
  list_trkresrg = list(ev.trkresrg_pandoraTrack)
  list_trkdedx = list(ev.trkdedx_pandoraTrack)
  '''
  list_pdg = list(ev.genlist_pdg)
  list_endPointS = list(ev.genlist_endPointS)
  list_TrackId = list(ev.genlist_trackId)
  list_trkpidbestplane = list(ev.recoTrack_bestplane)
  list_trkidtruth = list(ev.recoTrack_bestplane_trkpidpdg)
  list_ntrkhits = list(ev.recoTrack_bestplane_hits)
  list_trkpidpdg = list(ev.recoTrack_bestplane_trkpidpdg)
  list_trkresrg = list(ev.recoTrack_bestplane_resrg)
  list_trkdedx = list(ev.recoTrack_bestplane_dedx)
  for iMC, mcpdg in enumerate(list_pdg):
    # only look for MC muons, pions, kaons and protons
    if abs(mcpdg) not in colorcode: continue
    # only want true particle which ends at TPC
    if ( list_endPointS[iMC] > 1e-10): continue
    # get the Geant4 Track ID for this MC
    trkid = list_TrackId[iMC]
    # get the indices of reco track where G4 TrackID match the MC's
    midx = [i for i, x in enumerate(list_trkidtruth) if x == trkid]
    if midx != []:
      for i in midx:
        # is this the best plane? 
        # All the track info is selected with bestplane: recoTrack_bestplane
        #if i % 3 != list_trkpidbestplane[i//3]:
        #  continue
        bestPlane.Fill(list_trkpidbestplane[i])
        # is this a good track (>= goodTrackHits hits) ?
        if list_ntrkhits[i] < args.goodTrackHits:
          continue
        # pdg of MC particle tracked the IDed reco track PDG?
        isMatch = (abs(mcpdg) == abs(list_trkpidpdg[i]))
        # now get the dE/dx vs residual L points and plot on respective PDG histos
        for j in range(kMaxTrackHits):
          x = list_trkresrg[kMaxTrackHits*i + j]
          y = list_trkdedx[kMaxTrackHits*i + j]
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

