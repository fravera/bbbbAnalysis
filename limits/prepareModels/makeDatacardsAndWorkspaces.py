import ROOT
import argparse
import collections
import fnmatch
from  ConfigParser import *
import ast
import os
from subprocess import Popen
from StringIO import StringIO
import subprocess
import time

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

def writelnarr(f, txtarr, align='{:>25}', firstAlign = '{:<35}', secondAlign = '{:<6}', addEmptyAtIdx = None):
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

def readShapeFromAltHistos(root_inputfile, nominalname, systname, systformat='{nominalname}_{syst}'):
    
    name_nom = nominalname
    name_dn  = systformat.format(nominalname=nominalname, syst=systname+'Down')
    name_up  = systformat.format(nominalname=nominalname, syst=systname+'Up')


    h_nom = root_inputfile.Get(name_nom)
    h_dn  = root_inputfile.Get(name_dn)
    h_up  = root_inputfile.Get(name_up)

    if not h_nom : raise RuntimeError("histogram %s not found" % name_nom)
    if not h_dn  : raise RuntimeError("histogram %s not found" % name_dn)
    if not h_up  : raise RuntimeError("histogram %s not found" % name_up)

    y_nom = 1. * h_nom.Integral()
    y_dn  = 1. * h_dn.Integral()
    y_up  = 1. * h_up.Integral()

    if y_nom <= 0:
        return (0, 0)

    var_dn = y_dn / y_nom
    var_up = y_up / y_nom

    return (var_dn, var_up)


########################
parser = argparse.ArgumentParser(description='Command line parser of skim options')
parser.add_argument('--config',    dest='cfgfile',        help='Name of config file with MC information',   required = True)
parser.add_argument('--signal',    dest='signal',         help='Name of the signal',   required = True)
parser.add_argument('--card-only', dest='doworkspace',    help='Just make the txt cards, no workspace',     action='store_false', default=True)
parser.add_argument('--no-comb',   dest='docombination',  help='Do not make the combined cards',            action='store_false', default=True)
parser.add_argument('--no-bbb',    dest='dobbb',          help='Do not add the bin-by-bin uncs',            action='store_false', default=True)
parser.add_argument('--addScaleSignal',    dest='addScaleSignal',          help='add scale signal',         action='store_true' , default=False)
parser.add_argument('--folder',    dest='folder',          help='override config folder' , default="None")

args = parser.parse_args()
configfilename = args.cfgfile

###########Read Config file
print "[INFO] Reading configuration file . . ."
with open(configfilename) as templateConfiguration:
	signalConfiguration = templateConfiguration.read()
signalConfiguration = signalConfiguration.replace("${signalTemplate}",args.signal)
cfgparser = ConfigParser()
cfgparser.optionxform = lambda option: option # preserve lower-upper case
cfgparser.readfp(StringIO(signalConfiguration))

categandobs = ast.literal_eval(cfgparser.get("configuration","categandobs"))
##########Get variables for datacard preparation
folder       = ast.literal_eval(cfgparser.get("configuration","folder"))
datas        = ast.literal_eval(cfgparser.get("configuration","datas"))
sigs         = ast.literal_eval(cfgparser.get("configuration","sigs"))
bkgs         = ast.literal_eval(cfgparser.get("configuration","bkgs"))
dsetname     = ast.literal_eval(cfgparser.get("configuration","dataset"))
card_name    = "datacard%s"%dsetname
categories   = [categandobs[k][0] for k in range(0,len(categandobs)) ]
combinations = ast.literal_eval(cfgparser.get("configuration","combinations"))
datacards    = []

if args.folder != "None": folder = args.folder

## consider the renamings
def apply_rename(renames, values):
    for i in range(len(values)):
        if values[i] in renames:
            values[i] = renames[values[i]]

process_rename = ast.literal_eval(cfgparser.get("configuration", "replacements"))
apply_rename(process_rename, datas)
apply_rename(process_rename, sigs)
apply_rename(process_rename, bkgs)

## parse systematics from cfg
##
## SYSTEMATICS - list the name and the process affected by how much
## syst types that have 'fromInput:lnN' are read from the matching name and converted to lnN
## syst types that have 'fromInput:shape' are read from the matching name and set as shape unc.
## processes are matched with wildcards
systs = collections.OrderedDict()
for line in cfgparser.items('datacard_systematics'):
    systs[line[0]] = ast.literal_eval(line[1])

# ############################################################################################

groups = collections.OrderedDict()
for line in cfgparser.items('datacard_groups'):
    groups[line[0]] = ast.literal_eval(line[1])

############################################################################################

syst_lnN_prune_value   = 0.01 # None to skip

print "[INFO] systs from histos ignored if error is less than", syst_lnN_prune_value

##################################################################################
# filter systematics

# frominput_shape_syst = [x for x in systs.keys() if 'fromInput:shape' in systs[x][0]]
# frominput_lnN_syst   = [x for x in systs.keys() if 'fromInput:lnN'   in systs[x][0]]

# print frominput_shape_syst
# print frominput_lnN_syst

#Get into the main folder
mainDirectory = os.getcwd()
os.chdir(folder)
#Make datacards, one per category
print "[INFO] Making datacards: "
for categ in categories:
    #######################
    fname = "%s_%s.txt"%(card_name,categ)
    datacards.append("%s_%s.txt"%(card_name,categ))
    print " - %s/%s "%(folder,fname)
    fcard = open(fname, 'w')
    inputfile = "outPlotter_%s_%s.root"%(dsetname,args.signal)

    root_inputfile = ROOT.TFile.Open(inputfile) # for parsing variations after

    ########################
    writeln    (fcard, 'imax *  number of channels')
    writeln    (fcard, 'jmax *  number of backgrounds')
    writeln    (fcard, 'kmax *  number of nuisance parameters (sources of systematical uncertainties)')
    writeln    (fcard, '----------------------------------------------------------------------------------------------------------------------------------')
    ## shapes
    # for data in datas: writeln    (fcard, 'shapes %s bin1 %s %s'%(data,inputfile,data) )
    # for bkg  in bkgs : writeln    (fcard, 'shapes %s bin1 %s %s'%(bkg,inputfile,bkg) )
    # for sig  in sigs : writeln    (fcard, 'shapes %s bin1 %s %s'%(sig,inputfile,sig) )
    writeln    (fcard, 'shapes * %s %s $PROCESS $PROCESS_$SYSTEMATIC'%(categ, inputfile) )


    # if args.QCDsyst: writeln    (fcard, 'shapes QCD bin1 {} {} {}'.format(fIn_name, hname, hname + "_$SYSTEMATIC"))


    writeln    (fcard, '----------------------------------------------------------------------------------------------------------------------------------')
    ## observation
    writelnarr (fcard, ('bin', categ))
    writelnarr (fcard, ('observation', '-1'))
    writeln    (fcard, '----------------------------------------------------------------------------------------------------------------------------------')
    writeln    (fcard, '# list the expected events for signal and all backgrounds in that bin')
    writeln    (fcard, '# the second process line must have a positive number for backgrounds, and 0 or neg for signal')
    nsig = len(sigs)
    nbkg = len(bkgs)
    ntot = nsig + nbkg
    writelnarr (fcard, ['bin'] + ([categ,]*ntot) , addEmptyAtIdx=1)
    writelnarr (fcard, ['process'] + sigs + bkgs ,  addEmptyAtIdx=1)
    writelnarr (fcard, ['process'] + range(-len(sigs)+1, 1) + range(1, len(bkgs)+1), addEmptyAtIdx=1)
    writelnarr (fcard, ['rate']
                    + ["-1" for hname in sigs]
                    + ["-1" for hname in bkgs], addEmptyAtIdx=1)
    writeln    (fcard, '----------------------------------------------------------------------------------------------------------------------------------')
    for syst_source in systs.keys():
        processes = []
        sytype = systs[syst_source][0]
        elemts = systs[syst_source][1:]
        
        ln_from_histo    = False
        # shape_from_histo = False

        if not affects_any(syst_source, systs, sigs + bkgs):
            continue    

        if sytype == 'fromInput:lnN':
            sytype        = 'lnN'
            ln_from_histo = True
        if sytype == 'fromInput:shape':
            sytype           = 'shape'
            # shape_from_histo = True

        line_tokens = [syst_source, sytype]

        for proc in sigs + bkgs: ## loop through the datacard columns
            iaffect = []
            for idx, el in enumerate(elemts):
                if len(el) > 2 and not fnmatch.fnmatch(categ, el[2]):
                        continue # does not apply to this category
                if fnmatch.fnmatch(proc, el[0]):
                    iaffect.append(idx)
            ## multiple assignments?
            if len(iaffect) == 0:
                line_tokens.append('-')
            else:
                if len(iaffect) > 1:
                    print ">> [WARNING] << process", proc, "matches twice the syst source", syst_source, ', using the 1st occurrence'
                the_idx   = iaffect[0]
                this_syst = elemts[the_idx][1]

                if ln_from_histo:
                    down, up  = readShapeFromAltHistos(root_inputfile, proc, syst_source)
                    this_syst = '%.3f/%.3f' % (down, up)
                    maxdelta  = max([abs(1. - up), abs(1. - down)])
                    if syst_lnN_prune_value and maxdelta < syst_lnN_prune_value:
                        # print "... [INFO] : systematics", syst_source, 'from histo is less than %.3f' % syst_lnN_prune_value, 'will be skipped'
                        this_syst = '-'
                line_tokens.append(this_syst)

        # check that the systematics applies to something - if not, remove it
        if len([x for x in line_tokens[2:] if x.strip() != '-']) == 0:
            print "... [INFO] : systematics   {:<30} has no effects and will be removed".format(line_tokens[0])
            continue
        writelnarr (fcard, line_tokens)

    for idx, (grname, grelems) in enumerate(groups.items()):
        if idx == 0: writeln (fcard, '')
        writeln (fcard, '{} group = {}'.format(grname, ' '.join(grelems)))

    if args.dobbb:
        writeln (fcard, '')
        writeln (fcard, '* autoMCStats 10')

    if args.addScaleSignal:
        writeln    (fcard, 'myscale rateParam * signalSelfBias 1')
        writeln    (fcard, 'nuisance edit freeze myscale')

#Make combined datacards
if args.docombination:
    print "[INFO] Making combined datacards: "
    for comb in combinations:
        combstr="python ../../../../HiggsAnalysis/CombinedLimit/scripts/combineCards.py "
        outcomb=""
        for k in  range(0,len(comb)):
            # print k, card_name, comb[k]
            # combstr+="card%i=%s_%s.txt "%(k,card_name,comb[k])
            combstr+="%s=%s_%s.txt "%(comb[k],card_name,comb[k])
            outcomb+="%s"%comb[k]
        combstr+="> %s_comb_%s.txt"%(card_name,outcomb)
        os.system('%s &'%combstr)
        datacards.append("%s_comb_%s.txt"%(card_name,outcomb)) 
        print " - %s/%s_comb_%s.txt"%(folder,card_name,outcomb)

if args.doworkspace:
    ##Make workspace
    print "[INFO] Making workspace for all datacards"
    if datas[0] != 'data_obs':
        for card in datacards:
            # p = subprocess.Popen(('text2workspace.py %s -D %s &'%(card, datas[0])).split())
            # p.wait()
            os.system('text2workspace.py %s -D %s'%(card, datas[0]))
    else:
        for card in datacards:
            # p = subprocess.Popen((str('text2workspace.py %s > tmp.txt &'%card)).split())
            # p.wait()
            os.system(str('text2workspace.py %s &'%card))
            # subprocess.check_call(str('text2workspace.py %s > tmp.txt &'%card), shell=True)
# import subprocess as subp
# time.sleep(10)

# checkIfRunningTaskCommand = "ps aux"
# stillRunning = True
# print 'text2workspace.py %s'%card
# while stillRunning:
#     print "StillRunning"
#     time.sleep(1)
#     checkIfRunningTask = subprocess.Popen(checkIfRunningTaskCommand.split(), stdout=subprocess.PIPE)
#     streamdata = checkIfRunningTask.communicate()[0]
#     stillRunning = False
#     # print streamdata
#     for card in datacards:
#         for line in streamdata.splitlines():
#         #     print line
#             if 'text2workspace.py' in line and "fravera" in line : stillRunning = True

os.chdir(mainDirectory)
