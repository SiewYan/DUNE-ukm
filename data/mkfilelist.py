#!/bin/python

import glob, os
from collections import OrderedDict

fileDict = {
    'kaon'   : '/data/iyusuff/cosmicgen_kaon_20210424/data/*/*/*_freco_ana.root',
    'proton' : '/data/iyusuff/cosmicgen_proton_20210502/data/*/*/*_freco_ana.root'
}

# number of file per job
fileperjob = 5

for dataset, dirs in fileDict.items():

    print( 'dataset : ', dataset, ' ; dirs : ', dirs )
    
    ncount=0; nf=0
    for ifile in glob.iglob(dirs):
        if ncount == 0 : f = open("%s_part%s.txt" %(dataset,nf) , "a" )
        ncount+=1
        #f.write( ifile+'\n' if ncount != fileperjob else ifile )
        f.write( ifile+'\n' )

        if ncount == fileperjob :
            print("write to %s_part%s.txt" %(dataset,nf) )
            f.close()
            ncount=0 ; nf+=1

    #check the last file is closed properly
    if f.closed == False:
        #print("line of content of file %s_part%s.txt : %s" %(dataset,nf,len(f.readlines())))
        print("file %s is not close properly, f.close() now" %(f.name) )
        f.close()
