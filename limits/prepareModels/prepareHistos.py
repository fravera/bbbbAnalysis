import os
import glob
import ast
import argparse
import sys
import copy
import ROOT
import numpy as np
from array import array
import math
from  ConfigParser import *
from StringIO import StringIO

addForUnrolling = "_Rebinned_Unrolled"

def combine_histos_quadrature (histo_list, h_nominal, name):
	""" for every bin, compute the pos and neg separations, and then sum them in quadrature separately """
	
	h_up = h_nominal.Clone(name + 'Up')
	h_dn = h_nominal.Clone(name + 'Down')

	for ibin in range(1, h_nominal.GetNbinsX()+1):
		
		deltas_up = [0] # 0 delta protects against one-sided variations
		deltas_dn = [0] # 0 delta protects against one-sided variations
		y_nominal = h_nominal.GetBinContent(ibin)
		
		for h in histo_list:
			y_this = h.GetBinContent(ibin)
			delta  = y_this - y_nominal
			
			if delta >= 0:
				deltas_up.append(delta)
			else:
				deltas_dn.append(delta)

		err_up = sum([math.sqrt(d*d) for d in deltas_up])
		err_dn = sum([math.sqrt(d*d) for d in deltas_dn])

		h_up.SetBinContent(ibin, y_nominal + err_up)
		h_dn.SetBinContent(ibin, y_nominal - err_dn)

	return (h_up, h_dn)

def combine_histos_maxmin (histo_list, h_nominal, name):
	""" for every bin, compute the pos and neg separations, and then sum them in quadrature separately """
	
	h_up = h_nominal.Clone(name + 'Up')
	h_dn = h_nominal.Clone(name + 'Down')

	for ibin in range(1, h_nominal.GetNbinsX()+1):
		
		deltas_up = [0] # 0 delta protects against one-sided variations
		deltas_dn = [0] # 0 delta protects against one-sided variations
		y_nominal = h_nominal.GetBinContent(ibin)
		
		for h in histo_list:
			y_this = h.GetBinContent(ibin)
			delta  = y_this - y_nominal
			
			if delta >= 0:
				deltas_up.append(delta)
			else:
				deltas_dn.append(delta)

		err_up = max([abs(d) for d in deltas_up])
		err_dn = max([abs(d) for d in deltas_dn])

		h_up.SetBinContent(ibin, y_nominal + err_up)
		h_dn.SetBinContent(ibin, y_nominal - err_dn)

	return (h_up, h_dn)


def get_h_nominal_name(process, categ, obs):
	s = "{process}/{categ}/{process}_{categ}_{obs}".format(process=process, categ=categ, obs=obs) + addForUnrolling
	return s

def RunPreparation(dataset,directory,processes,categandobs,folder, process_rename, syst_list, syst_to_comb, signalName):
	#Get root file
	# file = ROOT.TFile.Open("../../MyHistos/%s/outPlotter.root"%directory)
	file = ROOT.TFile.Open("%s/outPlotter.root" % directory)
	
	os.system('rm -rf %s'%folder)
	os.system('mkdir -p %s'%folder)
	#Open output file
	for k in range(0,len(categandobs)):
		hfile = ROOT.TFile('%s/outPlotter_%s_%s.root'%(folder,dataset,signalName), 'RECREATE')
		for process in processes:
			#Get histograms
			# h=ROOT.TH1F()
			#print "%s/Bfolder4_%s_SR_110_Histogram/%s_Bfolder4_%s_SR_110_Histogram_%s"%(process,categandobs[k][0],process,categandobs[k][0],categandobs[k][1])
			
			h = file.Get(get_h_nominal_name(process, categandobs[k][0], categandobs[k][1])) 
			print get_h_nominal_name(process, categandobs[k][0], categandobs[k][1])
			
			newname = process if process not in process_rename else process_rename[process]
			h.SetName("%s" % newname)

			#Write data and bkg:
			h.Write()

		### now systematics
		for process in processes:
			newname = process if process not in process_rename else process_rename[process]
			
			h_nominal = file.Get(get_h_nominal_name(process, categandobs[k][0], categandobs[k][1]))

			# parsing the input systematics
			for sys_desc in syst_list:
				sys_name    = sys_desc[0]
				sys_dirs    = sys_desc[1]
				sys_read    = sys_desc[2]
				sys_outname = sys_desc[3] if len(sys_desc) > 3 else sys_name
				
				variable  = categandobs[k][1]
				selection = categandobs[k][0]

				if sys_read == 'wsyst':
					h_proto = '{sample}/{selection}/{sample}_{selection}_{variable}_{syst}{direction}' + addForUnrolling
				elif sys_read == 'wsyst_nodir':
					h_proto = '{sample}/{selection}/{sample}_{selection}_{variable}{syst}' + addForUnrolling
				elif sys_read == 'altsample':
					h_proto = '{sample}_{syst}{direction}/{selection}/{sample}_{syst}{direction}_{selection}_{variable}' + addForUnrolling
				else:
					h_proto = sys_read

				for sd in sys_dirs:
					hname = h_proto.format(sample=process, selection=selection, variable=variable, syst=sys_name, direction=sd)
					h = file.Get(hname)
					if not h:
						# print "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
						# print "Histogram ", hname, " does not exist"
						# print "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!"
						# raise RuntimeError("Histogram for systematics with name : %s : is not in the input file" % hname)
						continue ## some cases (e.g. the data) won't have it

					outname_proto = '{sample}_{syst}{Dir}'
					if sd == 'up' or sd == '_up' or sd == 'Up' or sd == "_Up":
						Dir = 'Up'
					elif sd == 'down' or sd == '_down' or sd == 'Down' or sd == "_Dn":
						Dir = 'Down'
					elif sd is None:
						Dir = ''
					else:
						raise RuntimeError("cannot understand the direction -%s- of -%s-, please implement its handling" % (sd, sys_name))

					outname = outname_proto.format(sample=newname, syst=sys_outname, Dir=Dir)
					h.SetName(outname)
					h.Write()

			# making the combination of input systematics
			for comb_desc in syst_to_comb:
				
				inputs = comb_desc['inputs']

				if comb_desc['read_as'] == 'wsyst':
					h_proto = '{sample}/{selection}/{sample}_{selection}_{variable}_{syst}' + addForUnrolling
				elif comb_desc['read_as'] == 'altsample':
					h_proto = '{sample}_{syst}/{selection}/{sample}_{syst}_{selection}_{variable}' + addForUnrolling
				else:
					h_proto = comb_desc['read_as']

				outname_proto = '{sample}_{syst}{Dir}'

				histos_to_comb    = []
				histos_not_found  = []
				for inpt in inputs:
					h = file.Get(h_proto.format(sample=process, selection=selection, variable=variable, syst=inpt))
					if not h:
						histos_not_found.append(inpt)
						continue
					histos_to_comb.append(h)

				if len(histos_not_found) != 0 and len(histos_to_comb) != 0: # I found some histos, but not all
					print '[ERROR] : for systematic combined group ', comb_desc['name'] , 'only a subset of histograms was found'
					print '......... not found : ', histos_not_found
					print '......... check the inputs'
					raise RuntimeError("Systematic group to be combined is malformed")

				if len(histos_to_comb) == 0:
					continue # nothing to do for this process (e.g. these are data)

				## compute the variations according to the method
				if comb_desc['combine'] == 'quadrature':
					h_up, h_down = combine_histos_quadrature (histos_to_comb, h_nominal, comb_desc['name'])
					
				elif comb_desc['combine'] == 'maxmin':
					h_up, h_down = combine_histos_maxmin (histos_to_comb, h_nominal, comb_desc['name'])
				else:
					raise RuntimeError('the combination type -%s- is not implemented' % comb_desc['combine'])

				h_up   .SetName(outname_proto.format(sample=newname, syst=comb_desc['name'], Dir='Up'))
				h_down .SetName(outname_proto.format(sample=newname, syst=comb_desc['name'], Dir='Down'))
				
				# print " --------- ", process,categandobs[k][0],process,categandobs[k][0],categandobs[k][1]
				# print h_nominal.GetName(), h_nominal.Integral()
				# print h_up.GetName(), h_up.Integral()
				# print h_down.GetName(), h_down.Integral()

				h_up   .Write()
				h_down .Write()

		hfile.Close()		
#############COMMAND CODE IS BELOW ######################

###########OPTIONS
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config', dest='cfgfile',  help='Name of config file with MC information',   required = True)
parser.add_argument('--signal', dest='signal',   help='Name of the signal',                        required = True)
parser.add_argument('--folder',    dest='folder',          help='override config folder' , default="None")
parser.add_argument('--directory',    dest='directory',          help='override config directory' , default="None")
args = parser.parse_args()
configfilename = args.cfgfile

###########Read Config file
print "[INFO] Reading configuration file . . ."
with open(configfilename) as templateConfiguration:
	signalConfiguration = templateConfiguration.read()
signalConfiguration = signalConfiguration.replace("${signalTemplate}",args.signal)
cfgparser = ConfigParser()
cfgparser.readfp(StringIO(signalConfiguration))

##########Get skim variables
print "[INFO] Getting configuration parameters . . ."
dataset     = ast.literal_eval(cfgparser.get("configuration","dataset"))
print "    -The dataset:"
print "      *",dataset
directory   = ast.literal_eval(cfgparser.get("configuration","directory"))
if args.directory != "None": directory = args.directory
print "    -The location of the input histograms:"
print "      *",directory
processes   = ast.literal_eval(cfgparser.get("configuration","processes"))
print "    -The list of proceses:"
for x in range(len(processes)):
  print "      *",processes[x]
categandobs  = ast.literal_eval(cfgparser.get("configuration","categandobs"))
print "    -The list of categories and observables:"
for x in range(len(categandobs)):
  print "      *The category",categandobs[x][0]," has observable ",categandobs[x][1]
folder         = ast.literal_eval(cfgparser.get("configuration","folder"))
if args.folder != "None": folder = args.folder

print "    -The name of the folder to save the results:"
print "      *",folder
process_rename = ast.literal_eval(cfgparser.get("configuration", "replacements"))
print "    -There are : ", len(process_rename), "processes that will be renamed"
for key, value in process_rename.items():
	print "      * {:<30} --> {:<30}".format(key, value)


syst_list = ast.literal_eval(cfgparser.get("systematics", "syst_list"))
print "    -Systematics : will read", len(syst_list), "systematics"
for el in syst_list:
	extra = ' --> {}'.format(el[3]) if len(el) > 3 else ''
	print "      * [{:>9}] {:<25} {:<}".format(el[2], el[0], extra)

syst_to_comb = ast.literal_eval(cfgparser.get("systematics", "to_combine"))
print "    -Systematics : ", len(syst_to_comb), "new histograms will be defined as combination"
for el in syst_to_comb:
	print "      * {:>10}, built as -{}- from".format(el['name'], el['combine'])
	for source in el['inputs']:
		print "        {:10} - {:<}".format('', source)

##########Make microskims
print "[INFO] Making the histograms for physics model . . ."
RunPreparation(
	dataset         = dataset,
	directory       = directory,
	processes       = processes,
	categandobs     = categandobs,
	folder             = folder,
	process_rename  = process_rename,
	syst_list       = syst_list,
	syst_to_comb    = syst_to_comb,
	signalName      = args.signal
)
