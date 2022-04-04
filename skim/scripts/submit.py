import os, sys, glob
from optparse import OptionParser

cwd = os.getcwd()
usage = "usage: %prog [options]"
parser = OptionParser(usage)
parser.add_option("-d","--dataset", action="store", type="string", dest="dataset", default="all")
parser.add_option("-t","--test", action="store_true", dest="test", default=False)

(options, args) = parser.parse_args()

dataset = options.dataset
test = options.test

def getTOPDIR(dir_):
    dirtop=[]
    for itoken in dir_.split('/'):
        dirtop.append(itoken)
        if itoken == 'skim': break;
    return "/".join(dirtop)
pass

def makescript( sample_ , cmd_ , jobout__ ):
    outscript = jobout__+"/"+sample_+".sh"
    with open( outscript , 'a') as script :
        script.write( '#!/bin/bash\n' )
        script.write( 'export HOME=%s\n' %getTOPDIR(jobout__) )
        script.write( 'echo $HOME\n' )
        #script.write( 'ulimit -c 0\n' )
        script.write( 'cd $HOME\n' )
        #script.write( 'make\n')
        script.write( 'pwd\n' )
        script.write( cmd_ )
        script.close()
    os.system( 'chmod +x %s' %outscript )

    outscriptSub = outscript.replace( '.sh' , '.sub' )
    with open( outscriptSub , 'a' ) as script :
        script.write( 'universe = vanilla\n' )
        script.write( 'Executable = %s\n' %outscript )
        #script.write( 'transfer_input_files = /homeui/%s/x509up_u%s\n' %( os.environ["USER"] ,  os.getuid() ) )
        #script.write( 'use_x509userproxy = True\n' )
        #script.write( 'x509userproxy = /homeui/%s/x509up_u%s\n' %( os.environ["USER"] , os.getuid() ) )
        script.write( 'output    = %s\n' %outscript.replace('.sh' , '.out') )
        script.write( 'error     = %s\n' %outscript.replace('.sh' , '.err') )
        script.write( 'log       = %s\n' %outscript.replace('.sh','.log') )
        #script.write( 'transfer_output_remaps  = \"%s.root=%s.root\"\n' %( sample_name__ , outscript.split('/')[-1].replace('.sh','') ) )
        script.write( 'should_transfer_files = YES\n' )
        script.write( 'when_to_transfer_output = ON_EXIT\n' )
        script.write( 'queue\n')
        script.close()
    if not test: os.system( 'condor_submit %s' %(outscriptSub) )

pass

def execute( iproc_ , jobout_ , output_ ):
    sample = iproc_.split('/')[-1].split('.txt')[0]
    cmd="%s/bin/postproc -o %s/%s.root -f %s" %( getTOPDIR(jobout_) , output_ , sample , iproc_  )
    print(cmd)
    makescript( sample , cmd , jobout_ )
    
pass

if __name__ == "__main__" :

    TOP_DIR= getTOPDIR(os.getcwd())
    SOURCE_DIR="/home/hoh/dune_ukm/skim/data"
    PROD_DIR="/data/hoh"
    PROD_FOLD="prod_v1"
    prod_out=PROD_DIR+"/"+PROD_FOLD
    job_dir=TOP_DIR+"/"+"jobs"
    
    if not os.path.exists(prod_out): os.system( "mkdir -p %s" %(prod_out) )
    if not os.path.exists(job_dir): 
        os.system( "mkdir -p %s" %(job_dir) )
    else:
        os.system( "rm %s/*" %(job_dir) )
    
    filelists = glob.glob("%s/*.txt" %SOURCE_DIR) if dataset == "all" else glob.glob("%s/%s*.txt" %(SOURCE_DIR,dataset) )

    if not test: os.system("cd %s; make" %TOP_DIR )
    for iproc in filelists:
        #print(iproc)
        execute( iproc , job_dir , prod_out )

