#!/usr/bin/env python3

# This scripts runs GCC as well as IFCC on each test-case provided and compares the results.
#
# input: the test-cases are specified either as individual
#         command-line arguments, or as part of a directory tree
#
# output: 
#
# The script is divided in three distinct steps:
# - in the ARGPARSE step, we understand the command-line arguments
# - in the PREPARE step, we copy all our test-cases into a single directory tree
# - in the TEST step, we actually run GCC and IFCC on each test-case
#
#
def printProgressBar(act, total, nbOk):
    nbFail = act - nbOk
    nbCase = int(30)
    taux = act/total
    nbARemplir = int(taux * nbCase)
    barStr = "Progress : ["
    for i in range(nbCase):
        if i < nbARemplir:
            barStr = barStr + '#' # '█'
        else:
            barStr = barStr + '-'
    barStr = barStr + f"] {taux*100:3.0f}%  OK : {nbOk:2}  Failed : {nbFail:2}"
    print(barStr, "\033[0K\r", end = "\r")

import argparse
import glob
import os
import shutil
import sys
import subprocess

def command(string, logfile=None):
    """execute `string` as a shell command, optionnaly logging stdout+stderr to a file. return exit status.)"""
    if args.verbose:
        print("ifcc-test.py: "+string)
    try:
        output=subprocess.check_output(string,stderr=subprocess.STDOUT,shell=True)
        ret= 0
    except subprocess.CalledProcessError as e:
        ret=e.returncode
        output = e.output

    if logfile:
        f=open(logfile,'w')
        print(output.decode(sys.stdout.encoding)+'\n'+'exit status: '+str(ret),file=f)
        f.close()

    return ret

def dumpfile(name):
    print(open(name).read(),end='')

######################################################################################
## ARGPARSE step: make sense of our command-line arguments

argparser   = argparse.ArgumentParser(
description = "Compile multiple programs with both GCC and IFCC, run them, and compare the results.",
epilog      = ""
)

argparser.add_argument('input',metavar='PATH',nargs='+',help='For each path given:'
                       +' if it\'s a file, use this file;'
                       +' if it\'s a directory, use all *.c files in this subtree')

argparser.add_argument('-d','--debug',action="count",default=0,
                       help='Increase quantity of debugging messages (only useful to debug the test script itself)')
argparser.add_argument('-v','--verbose',action="count",default=0,
                       help='Increase verbosity level. You can use this option multiple times.')
argparser.add_argument('--ifcc-wrapper',metavar='PATH',
                       help='Invoke your compiler through the shell script at PATH. (default: `ifcc-wrapper.sh`)')
argparser.add_argument('--gcc-wrapper',metavar='PATH',
                       help='Invoke your compiler through the shell script at PATH. (default: `ifcc-wrapper.sh`)')
argparser.add_argument('--as-wrapper',metavar='PATH',
                       help='Invoke your compiler through the shell script at PATH. (default: `ifcc-wrapper.sh`)')
argparser.add_argument('--exec-wrapper',metavar='PATH',
                       help='Invoke your compiler through the shell script at PATH. (default: `ifcc-wrapper.sh`)')

args=argparser.parse_args()

if args.debug >=2:
    print('debug: command-line arguments '+str(args))

orig_cwd=os.getcwd()
if "ifcc-test-output" in orig_cwd:
    print('error: cannot run from within the output directory')
    exit(1)
    
if os.path.isdir('ifcc-test-output'):
    # cleanup previous output directory
    command('rm -rf ifcc-test-output')
os.mkdir('ifcc-test-output')
    
## Then we process the inputs arguments i.e. filenames or subtrees
inputfilenames=[]
for path in args.input:
    path=os.path.normpath(path) # collapse redundant slashes etc.
    if os.path.isfile(path):
        if path[-2:] == '.c':
            inputfilenames.append(path)
        else:
            print("error: incorrect filename suffix (should be '.c'): "+path)
            exit(1)
    elif os.path.isdir(path):
        for dirpath,dirnames,filenames in os.walk(path):
            inputfilenames+=[dirpath+'/'+name for name in filenames if name[-2:]=='.c']
    else:
        print("error: cannot read input path `"+path+"'")
        sys.exit(1)

## debug: after treewalk
if args.debug:
    print("debug: list of files after tree walk:"," ".join(inputfilenames))

## sanity check
if len(inputfilenames) == 0:
    print("error: found no test-case in: "+" ".join(args.input))
    sys.exit(1)

## Here we check that we can actually read the files. Our goal is to
## fail as early as possible when the CLI arguments are wrong.
for inputfilename in inputfilenames:
    try:
        f=open(inputfilename,"r")
        f.close()
    except Exception as e:
        print("error: "+e.args[1]+": "+inputfilename)
        sys.exit(1)

## Last but not least: we now locate the "wrapper script" that we will
## use to invoke ifcc
gcc_wrapper=os.path.realpath(os.getcwd()+"/"+ args.gcc_wrapper)
ifcc_wrapper=os.path.realpath(os.getcwd()+"/"+ args.ifcc_wrapper)
as_wrapper=os.path.realpath(os.getcwd()+"/"+ args.as_wrapper)
exec_wrapper=os.path.realpath(os.getcwd()+"/"+ args.exec_wrapper)

if not os.path.isfile(gcc_wrapper):
    print("error: cannot find "+os.path.basename(gcc_wrapper)+" in directory: "+os.path.dirname(gcc_wrapper))
    exit(1)
if not os.path.isfile(ifcc_wrapper):
    print("error: cannot find "+os.path.basename(ifcc_wrapper)+" in directory: "+os.path.dirname(ifcc_wrapper))
    exit(1)
if not os.path.isfile(as_wrapper):
    print("error: cannot find "+os.path.basename(as_wrapper)+" in directory: "+os.path.dirname(as_wrapper))
    exit(1)
if not os.path.isfile(exec_wrapper):
    print("error: cannot find "+os.path.basename(exec_wrapper)+" in directory: "+os.path.dirname(exec_wrapper))
    exit(1)

if args.debug:
    print("debug: wrapper path: "+ifcc_wrapper)
        
######################################################################################
## PREPARE step: copy all test-cases under ifcc-test-output

jobs=[]

for inputfilename in inputfilenames:
    if args.debug>=2:
        print("debug: PREPARING "+inputfilename)

    if 'ifcc-test-output' in os.path.realpath(inputfilename):
        print('error: input filename is within output directory: '+inputfilename)
        exit(1)
    
    ## each test-case gets copied and processed in its own subdirectory:
    ## ../somedir/subdir/file.c becomes ./ifcc-test-output/somedir-subdir-file/input.c
    lastDir = inputfilename.split('/')
    lastDir = lastDir[-2] + '--' + lastDir[-1]
    subdir='ifcc-test-output/'+lastDir.strip("./")[:-2].replace('/','-')
    os.mkdir(subdir)
    shutil.copyfile(inputfilename, subdir+'/input.c')
    jobs.append(subdir)

## eliminate duplicate paths from the 'jobs' list
unique_jobs=[]
for j in jobs:
    for d in unique_jobs:
        if os.path.samefile(j,d):
            break # and skip the 'else' branch
    else:
        unique_jobs.append(j)
jobs=sorted(unique_jobs)
# debug: after deduplication
if args.debug:
    print("debug: list of test-cases after deduplication:"," ".join(jobs))


######################################################################################
## TEST step: actually compile all test-cases with both compilers

nbOk = 0
nbTest = 0
errorText = "\n\n"
for jobname in jobs:
    printProgressBar(nbTest, len(jobs), nbOk)
    nbTest = nbTest + 1
    os.chdir(orig_cwd)
    os.chdir(jobname)
    jobname = jobname[17:]
    
    ## Reference compiler = GCC
    gccstatus=command(gcc_wrapper + " asm-gcc.s input.c", "gcc-compile.txt")
    if gccstatus == 0:
        # test-case is a valid program. we should run it
        gccstatus=command(as_wrapper + " exe-gcc asm-gcc.s", "gcc-link.txt")
    if gccstatus == 0: # then both compile and link stage went well
        exegccstatus=command(exec_wrapper + " ./exe-gcc", "gcc-execute.txt")
        if args.verbose >=2:
            dumpfile("gcc-execute.txt")
            
    ## IFCC compiler
    ifccstatus=command(ifcc_wrapper+" asm-ifcc.s input.c", "ifcc-compile.txt")
    
    if gccstatus != 0 and ifccstatus != 0:
        ## ifcc correctly rejects invalid program -> test-case ok
        #print('TEST-CASE: '+jobname)
        #print("TEST OK")
        nbOk = nbOk + 1
        continue
    elif gccstatus != 0 and ifccstatus == 0:
        ## ifcc wrongly accepts invalid program -> error
        errorText = errorText + '[' + jobname + "] : your compiler accepts an invalid program\n"
        continue
    elif gccstatus == 0 and ifccstatus != 0:
        ## ifcc wrongly rejects valid program -> error
        errorText = errorText + '[' + jobname  + "] : your compiler rejects a valid program\n"
        if args.verbose:
            dumpfile("ifcc-compile.txt")
        continue
    else:
        ## ifcc accepts to compile valid program -> let's link it
        ldstatus=command(as_wrapper + " exe-ifcc asm-ifcc.s", "ifcc-link.txt")
        if ldstatus:
            errorText = errorText + '[' + jobname + "] : your compiler produces incorrect assembly\n"
            if args.verbose:
                dumpfile("ifcc-link.txt")
            continue

    ## both compilers  did produce an  executable, so now we  run both
    ## these executables and compare the results.
        
    command(exec_wrapper + " ./exe-ifcc","ifcc-execute.txt")
    if open("gcc-execute.txt").read() != open("ifcc-execute.txt").read() :
        errorText = errorText + '[' + jobname + "] : different results at execution\n"
        if args.verbose:
            print("GCC:")
            dumpfile("gcc-execute.txt")
            print("you:")
            dumpfile("ifcc-execute.txt")
        continue

    ## last but not least
    #print('TEST-CASE: '+jobname)
    #print("TEST OK")
    nbOk = nbOk + 1

printProgressBar(nbTest, len(jobs), nbOk)
if (nbTest != nbOk):
    print(errorText)
else:
    print("\n")
