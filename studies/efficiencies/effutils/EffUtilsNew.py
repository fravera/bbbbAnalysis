import collections
import glob
import ROOT 
import fnmatch
import os

### these are from the plotter, path is set in __init__.py
import ConfigReader as cfgr

class effReader:
    def __init__(self, root_fin, process):
        self.counts     = {} # collections.OrderedDict()
        self.tot_w_name = 'Ntot_w' ## my denominator
        self.process    = process
        hname = '{process}/{process}'.format(process=process)
        # print hname
        print "[INFO] Opening histogram for process:", process
        h = root_fin.Get(hname)
        for i in range(1, h.GetNbinsX()+1):
            l = h.GetXaxis().GetBinLabel(i)
            c = h.GetBinContent(i)
            self.counts[l] = c

        if self.counts[self.tot_w_name] == 0:
            print "[WARNING] : sample",  self.process, "has 0 total processed events"

    # def correctForLumiXS(self, cfg_file, corr_list = None, tot_w = 'Ntot_w'):
    def correctForXS(self, cfg_file, corr_list = None, tot_w = 'Ntot_w'):
        if not corr_list:
            corr_list = ['Ntot_w', "Ntrg_w", "Nsel_w"]
        cfg  = cfgr.ConfigReader(cfg_file)
        # lumi = float(cfg.readOption ("general::lumi")) ## in pb
        cfg_samples_name = os.path.basename(cfg.readOption("configs::sampleCfg"))
        cfg_samples_dir  = os.path.join(os.path.dirname(cfg_file), '')
        cfg_lfn          = cfg_samples_dir + cfg_samples_name
        cfg_samples      = cfgr.ConfigReader(cfg_lfn)
        filelistname     = cfg_samples.readOption("samples::%s" % self.process)

        if not os.path.isabs(filelistname):

            ## check if list is an absolute path, otherwise check recursively one level above, until exhaustion
            ntest = 0
            startdir = cfg_samples_dir
            while ntest < 5:
                fullname = os.path.join(startdir, filelistname)
                # print ntest, fullname
                if os.path.isfile(fullname):
                    filelistname = fullname        
                    break
                startdir = os.path.join(startdir, '..')
                ntest += 1

        def parseInputFileList (fileName) :
            filelist = []
            with open (fileName) as fIn:
                for line in fIn:
                    line = (line.split("#")[0]).strip()
                    if line:
                        filelist.append(line)
            return filelist

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

        xs = extract_xs(filelistname)

        print "[INFO] For sample", self.process
        # print "....... lumi : ", lumi
        print "....... xs   : ", xs
        # print "....... lumi * xs applied to : ", corr_list
        print "....... xs applied to : ", corr_list

        denom = self.counts[tot_w]
        for key in self.counts.keys():
            if key in corr_list:
                self.counts[key] = self.counts[key] * xs
                ## convert to number of events : n/ntot * lumi * sigma
                # print self.counts[key], 'BEFORE', lumi * xs
                # self.counts[key] = self.counts[key]/denom * lumi * xs
                # print self.counts[key], 'AFTER'

        # for key, val in self.counts.items():
        #     print key, val
        # print '....................'

    def get_eff(self, selection):
        
        denom = self.counts[self.tot_w_name]
        num   = self.counts[selection]
        if denom == 0:
            return 0
        return 1.*num/denom


def findInFolder (folder, pattern):
    ll = []
    for ff in os.listdir(folder):
        if fnmatch.fnmatch(ff, pattern): ll.append(ff)
    if len (ll) == 0:
        print "*** WARNING: No valid config " , pattern , "found in " , folder
        return None
    if len (ll) > 1:
        print "*** WARNING: Too many configs found in " , folder , ", using first one : " , ll
    return ll[0]

