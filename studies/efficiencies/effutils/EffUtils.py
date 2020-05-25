import collections
import glob
import ROOT 

### these are from the plotter, path is set in __init__.py
import ConfigReader as cfgr
import ToolsNew as Tools

def parseInputFileList (fileName) :
    filelist = []
    with open (fileName) as fIn:
        for line in fIn:
            line = (line.split("#")[0]).strip()
            if line:
                filelist.append(line)
    return filelist

def findBinByLabel(hIn, label):
    """ find the bin number corresponding to label, return -1 if not found """

    ibinfound = -1
    for ibin in range(1, hIn.GetNbinsX()+1):
        thisl = hIn.GetXaxis().GetBinLabel(ibin)
        if thisl == label:
            ibinfound = ibin
            break
    return ibinfound


def get_eff_skims (filelist, eff_histo_name = "eff_histo", vals_to_get = None, prepend_path = None):
    
    """ open all the histograms from a specific filelist and get the counters for various selection levels """
    
    files = parseInputFileList(filelist)
    if prepend_path:
        # print '.... get_eff_skims : prepending path', prepend_path, "to all files found"
        if prepend_path[-1] != '/':
            prepend_path += '/'
        files = [prepend_path + f for f in files]

    ## use default if not specified by used
    if not vals_to_get:
        vals_to_get = ['Ntot_w', 'Ntrg_w', 'Nsel_w']
    
    sums = {x : 0 for x in vals_to_get}

    for fname in files:
        fIn  = ROOT.TFile.Open(fname)
        hIn  = fIn.Get(eff_histo_name)

        # loop on all values to find
        for ival in range(len(vals_to_get)):
            label = vals_to_get[ival]
            ibin = findBinByLabel(hIn, label) # in principle all files shoudl have the same structure, but research for all just in case
            if ibin < 0:
                raise RuntimeError("Could not find the skim eff value named %s in the input histogram %s of files %s" % (label, eff_histo_name, filelist))
            content = hIn.GetBinContent(ibin)
            sums[label] += content
        fIn.Close()

    ## return as a dictionary
    result = collections.OrderedDict()
    for val in vals_to_get:
        result[val] = sums[val]

    return result


def get_yields_plots (outPlotterName, cfgName, samplelist, sellist, var = None, unfold_lumi = True, unfold_xs_map = None):
    
    """ extract the normalisations from the inputs.
    if var = None, one will be automatically deduced by the code
    if unfold_lumi, the total number of events is divided by lumi (to get the original normalisation to relate to the skims """    

    cfg = cfgr.ConfigReader (cfgName)
    opl = ROOT.TFile.Open(outPlotterName)

    lumi = float(cfg.readOption ("general::lumi")) ## in pb

    varlist = cfg.readListOption ("general::variables")
    # a bit random, but first try some "good" variables, then pick up the first one
    var = \
        'HH_mass'  if 'HH_mass'  in varlist else \
        'HH_b1_pt' if 'HH_b1_pt' in varlist else \
        varlist[0]

    ## return the result as dictionaries : result[sample][selection] -> yield
    result = collections.OrderedDict()
    for sample in samplelist:
        result[sample] = collections.OrderedDict()
        for sel in sellist:
            result[sample][sel] = 0.0

    ## retrieve the histograms and get their yield
    for sel in sellist:
        histos   = Tools.retrieveHistos (opl, samplelist, var, sel)
        for sample in samplelist:
            h = histos[sample]
            integral = h.Integral(-1,-1)
            if unfold_lumi   : integral = integral / lumi
            if unfold_xs_map : integral = integral / unfold_xs_map[sample]
            result[sample][sel] = integral

    opl.Close()
    return result

def extract_xs (filelist, tree_name = "bbbbTree", xs_branch_name = "XS", prepend_path = None):

    files = parseInputFileList(filelist)
    if prepend_path:
        # print '.... extract_xs : prepending path', prepend_path, "to all files found"
        if prepend_path[-1] != '/':
            prepend_path += '/'
        files = [prepend_path + f for f in files]

    ## xs is always the same : use 1st entry of 1st tree 
    ## FIXME: in the unlikley case that this one has 0 entries, a check should be made here to skip to the next file
    fIn  = ROOT.TFile.Open(files[0])
    tree = fIn.Get(tree_name)
    tree.GetEntry(0)
    xs   = getattr(tree, xs_branch_name)
    fIn.Close()
    return xs
