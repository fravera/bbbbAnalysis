import os
import sys
import argparse
import getpass
import subprocess

parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--tag'       ,  dest = 'tag'       ,  help = 'production tag'           ,  required = True        )

args = parser.parse_args()

username = getpass.getuser()
print "... Welcome", username

folderName = "DataPlots_" + args.tag
outputFileName = "DataPlots_" + args.tag + "/outPlotter.root"

if os.path.isdir(folderName):
    print "... working folder", folderName, " already exists, exit"
    sys.exit()

cmd='mkdir ' + folderName
if os.system(cmd) != 0:
    print "... Not able to execute command \"", cmd, "\", exit"
    sys.exit()

haddCmd = "hadd -f " + outputFileName + " `xrdfs root://cmseos.fnal.gov ls -u /store/user/" + username + "/bbbb_histograms/" + args.tag + " | grep .root`"

if os.system(haddCmd) != 0:
    print "... Not able to execute command \"", haddCmd, "\", exit"
    sys.exit()

