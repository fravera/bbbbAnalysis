import subprocess
import sys
import argparse
import datetime
import glob
import re

eosPath = "root://cmseos.fnal.gov"
baseEosPath = "/eos/uscms/store/user/fravera/bbbb_ntuples"
##############################
##### CMD line options
parser = argparse.ArgumentParser(description='Command line parser of plotting options')

parser.add_argument('--tag',          dest='tag',         help='tag name'   ,      default=None)
parser.add_argument('--sample',       dest='sample',      help='sample name',      default=None)
parser.add_argument('--output',       dest='output',      help='sample name',      default=None)

args = parser.parse_args()

if not args.tag or not args.sample or not args.output:
    print "Please set tag, sample and output names"

outputFile = open(args.output, "w")

signalOutputPath = baseEosPath + "/" + args.tag + "/" + args.sample + "/output/"
out = subprocess.Popen(['eos', eosPath, 'ls', signalOutputPath], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
stdoutSecond,stderr = out.communicate()
for signalFileName in stdoutSecond.split():
    outputFile.writelines(eosPath + "/" + signalOutputPath + signalFileName + "\n")