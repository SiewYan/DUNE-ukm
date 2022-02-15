import argparse
import sys
import glob
import ROOT

ROOT.ROOT.EnableImplicitMT(4)

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
  # glob string
  if "*" in globlist:
    files = glob.glob(globlist)
    for thefile in files: rootfilelist += thefile
  else:
    rootfilelist += globlist

# lambda function
ROOT.gInterpreter.Declare("""
using namespace ROOT::VecOps;
using RNode = ROOT::RDF::RNode;

using VecI_t = const ROOT::RVec<int>&;
using VecF_t = const ROOT::RVec<float>&;
ROOT::RVec<int> getMatchIdx( VecI_t recoID , VecI_t truthID ){

RVec<int> out;

//
auto idxs = Combinations( recoID , truthID );
for ( size_t i = 0; i < idxs[0].size(); i++ ) {
const auto i1 = idxs[0][i];
const auto i2 = idxs[1][i];

// save the truthID idx if truth's is matched to reco's
if ( recoID[i1] == truthID[i2] ) out.push_back(i2);

}
return out;
}

""")

print('Number of input files:', len(rootfilelist))
print("Defining RDataFrame")
df = ROOT.ROOT.RDataFrame( "analysistree/anatree" , rootfilelist )

#only look for MC muons, pions, kaons and protons
df = df 

# only want true particle which ends at TPC
df = df.Define( 'dx' , 'EndPointx_tpcAV - EndPointx' )\
       .Define( 'dy' , 'EndPointx_tpcAV - EndPointx' )\
       .Define( 'dz' , 'EndPointx_tpcAV - EndPointx' )\
       .Define( 's' , 'dx+dy+dz' )

df = df.Filter( 's > 1e-10' , 'ensure length is within 1e-10' )

print('All done!')

