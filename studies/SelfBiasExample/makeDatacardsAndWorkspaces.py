import ROOT
import argparse
import collections
import fnmatch
from  ConfigParser import *
import ast
import os
from subprocess import Popen

def get_histoname(proc, selection, var):
    # QCD_baseline_SR_BDTG_nomasscut
    hname = '{name}_{sel}_{var}'.format(name=proc, sel=selection, var=var)
    # hname = '{}_{}_{}'.format(proc, selection, var)
    return hname

def get_histo (fIn, proc, selection, var):
    # print "call get histo on fIn={}. proc={}. selection={}, var={}".format()
    hname = get_histoname(proc, selection, var)
    # if not fIn.GetListOfKeys().Contains(hname):
    h = fIn.Get(hname)
    if not h:
        raise RuntimeError("histo not found : %s" % hname)
    return h

def writeln (f, txt):
    f.write(txt + '\n')

def writelnarr(f, txtarr, align='{:>12}', firstAlign = '{:<12}', secondAlign = '{:<6}', addEmptyAtIdx = None):
    if addEmptyAtIdx:
        txtarr = txtarr[0:addEmptyAtIdx] + [''] + txtarr[addEmptyAtIdx:]
    str_proto = ''
    for idx, txt in enumerate(txtarr):
        str_proto += '%s ' % (firstAlign if idx == 0 else secondAlign if idx == 1 else align)
    str_proto = str_proto[:-1] ## remove trailing space
    the_str = str_proto.format(*txtarr)
    writeln(f, the_str)

def affects_any(syst, systlist, procs):
    sytype = systs[syst][0]
    elemts = systs[syst][1:]
    for proc in procs: ## loop through the datacard columns
        iaffect = []
        for idx, el in enumerate(elemts):
            if fnmatch.fnmatch(proc, el[0]):
                return True
    return False

########################
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config', dest='cfgfile',  help='Name of config file with MC information',   required = True)
args = parser.parse_args()
configfilename = args.cfgfile
print "[INFO] Reading configuration file "
cfgparser = ConfigParser()
cfgparser.read('%s'%configfilename)

folder       = ast.literal_eval(cfgparser.get("configuration","folder"))
sigandbkgs   = ast.literal_eval(cfgparser.get("configuration","sigandbkgs"))
data         = ast.literal_eval(cfgparser.get("configuration","datas"))
categandobs  = ast.literal_eval(cfgparser.get("configuration","categandobs"))
dsetname     = ast.literal_eval(cfgparser.get("configuration","dataset"))
card_name    = "datacard%s"%dsetname

categories   = [categandobs[k][0] for k in range(0,len(categandobs)) ]
datacards    = []

## SYSTEMATICS - list the name and the process affected by how much
## processes are matched with wildcards
systs = collections.OrderedDict()
systs['lumi'    ] = ['lnN' , (sigandbkgs[0][0], '1.025')]
systs['xS'      ] = ['lnN' , (sigandbkgs[0][0], '1.050')]
systs['xB'      ] = ['lnN' , ('MODEL*', '1.050')]
#Get into the main folder
os.chdir("%s/%s"%(os.getcwd(),folder))
#Make datacards, one per category
print "[INFO] Making datacards: "
for group in sigandbkgs:
      for categ in categories:
         #######################
         sigs         = [group[0]]
         bkgs         = [group[1],group[2]]
         fname = "%s_%s_%s.txt"%(card_name,categ,sigs[0])
         datacards.append("%s_%s_%s.txt"%(card_name,categ,sigs[0]))
         print " - %s/%s "%(folder,fname)
         fcard = open(fname, 'w')
         inputfile = "outPlotter_%s.root"%(categ)
         ########################
         writeln    (fcard, 'imax *  number of channels')
         writeln    (fcard, 'jmax *  number of backgrounds')
         writeln    (fcard, 'kmax *  number of nuisance parameters (sources of systematical uncertainties)')
         writeln    (fcard, '----------------------------------------------------------------------------------------------------------------------------------')
         ## shapes
         writeln    (fcard, 'shapes %s bin1 %s %s'%(data[0],inputfile,data[0]) )
         for sig  in sigs:  writeln    (fcard, 'shapes %s bin1 %s %s'%(sig,inputfile,sig) )
         for bkg  in bkgs:  writeln    (fcard, 'shapes %s bin1 %s %s'%(bkg,inputfile,bkg) )
         writeln    (fcard, '----------------------------------------------------------------------------------------------------------------------------------')
         ## observation
         writelnarr (fcard, ('bin', 'bin1'))
         writelnarr (fcard, ('observation', '-1'))
         writeln    (fcard, '----------------------------------------------------------------------------------------------------------------------------------')
         writeln    (fcard, '# list the expected events for signal and all backgrounds in that bin')
         writeln    (fcard, '# the second process line must have a positive number for backgrounds, and 0 or neg for signal')
         nsig = len(sigs)
         nbkg = len(bkgs)
         ntot = nsig + nbkg
         writelnarr (fcard, ['bin'] + (['bin1',]*ntot) , addEmptyAtIdx=1)
         writelnarr (fcard, ['process'] + sigs + bkgs ,  addEmptyAtIdx=1)
         writelnarr (fcard, ['process'] + range(-len(sigs)+1, 1) + range(1, len(bkgs)+1), addEmptyAtIdx=1)
         writelnarr (fcard, ['rate']
                         + ["-1" for sig in sigs]
                         + ["-1" for sig in bkgs], addEmptyAtIdx=1)
         writeln    (fcard, '----------------------------------------------------------------------------------------------------------------------------------')
         #####systematics
         for syst_source in systs.keys():
             processes = []
             sytype = systs[syst_source][0]
             elemts = systs[syst_source][1:]
             if not affects_any(syst_source, systs, sigs + bkgs):
                 continue         
             line_tokens = [syst_source, sytype]         
             for proc in sigs + bkgs: ## loop through the datacard columns
                 iaffect = []
                 for idx, el in enumerate(elemts):
                     if fnmatch.fnmatch(proc, el[0]):
                         iaffect.append(idx)
                 ## multiple assignments?
                 if len(iaffect) == 0:
                     line_tokens.append('-')
                 else:
                     if len(iaffect) > 1:
                         print ">> [WARNING] << process", proc, "matches twice the syst source", syst_source, ', using the 1st occurrence'
                     the_idx = iaffect[0]
                     line_tokens.append(elemts[the_idx][1])
             writelnarr (fcard, line_tokens)
         writeln    (fcard, 'myscale rateParam * %s 1'%bkgs[1])
         writeln    (fcard, 'nuisance edit freeze myscale')
##Make workspace
print "[INFO] Making workspace for all datacards"
for card in datacards: os.system('text2workspace.py %s -D DATA &'%card)