import os
import sys
import argparse
import getpass
import subprocess

signalVariationList = ["", "_Total_down", "_Total_up", "_bjer_down", "_bjer_up", "_jer_down", "_jer_up"]
eosProtoPath = "/store/user/{0}/bbbb_ntuples/{1}{2}/"
eosPath      = 'root://cmseos.fnal.gov'
fileListProtoName = "plotterListFiles/{0}Resonant_NMSSM_XYH_bbbb/{1}/FileList_{2}.txt"

parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--year',  dest = 'year',  help = 'folder containing the skims',  required = True )
parser.add_argument('--tag' ,  dest = 'tag' ,  help = 'folder containing the skims',  required = True )

args = parser.parse_args()

username = getpass.getuser()
print "... Welcome", username

for signalVariation in signalVariationList:
	eosBaseFolder = eosProtoPath.format(username,args.tag,signalVariation)
	out = subprocess.Popen(['eos', eosPath, 'ls', eosBaseFolder], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
	stdoutFirst,stderr = out.communicate()
	for signalFolderName in stdoutFirst.split():
		if signalFolderName[:5] != "SKIM_": continue
		datasetName =  signalFolderName[5:]
		eosDatasetFolder = eosBaseFolder + signalFolderName + "/output/"
		out = subprocess.Popen(['eos', eosPath, 'ls', eosDatasetFolder], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
		stdoutSecond,stderr = out.communicate()
		signalSubFolder = ""
		if "NMSSM_XYH_bbbb_MX_" in datasetName: signalSubFolder = "Signal" + signalVariation
		outputFileName = fileListProtoName.format(args.year, signalSubFolder, datasetName)
		outputFile = open(outputFileName, "w")
		# print outputFileName
		for skimFileName in stdoutSecond.split():
			outputFile.write(eosPath + "/" + eosDatasetFolder + skimFileName + "\n")
			# print eosPath + "/" + eosDatasetFolder + skimFileName
