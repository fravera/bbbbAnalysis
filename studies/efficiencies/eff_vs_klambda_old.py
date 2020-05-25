import effutils.EffUtils as eut
import collections
import ROOT
ROOT.gROOT.SetBatch(True)

######
# definition of the plot content

### maps the things to plot to the names they have, and mark wheter the info is from skims or from plots
entries              = collections.OrderedDict()
entries['all']       = ('Ntot_w', 'skim') ## ** must ** be always included as 'all' for overall normalisation
entries['trigger']   = ('Ntrg_w', 'skim')
entries['presel' ]   = ('Nsel_w', 'skim')
entries['final3b'  ] = ('Btag3_VBFcateg_Inclusive_Histogram', 'histo')
entries['final4b'  ] = ('Btag4_VBFcateg_Inclusive_Histogram', 'histo')

# subset of "entries" to be actually plotted, in order
to_plot = [
    'trigger',
    'presel',
    'final3b',
    'final4b'
]

colors = {
    'trigger' : ROOT.kBlack,
    'presel'  : ROOT.kGreen+1,
    'final3b' : ROOT.kBlue,
    'final4b' : ROOT.kRed,
}

legs = {
    'trigger' : 'Trigger',
    'presel'  : 'Preselection',
    'final3b' : '3 b-tagged jets',
    'final4b' : '4 b-tagged jets',
}


## path prependend to all the files found in the file lists
## needed in case files names are stored in the list in a relative way (e.g., the lines in the lists below are relative to Daniel's working folder)
## if nothing must be prepended, use file_abs_path = None
file_abs_path = '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis'

### define a dictionary for klambda point, histo name, normalisation xs, and skim file list
klambdas_desc = collections.OrderedDict()
klambdas_desc[-20] = ('GGHH4B_rew_kl_m_20',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_20.txt')
klambdas_desc[-19] = ('GGHH4B_rew_kl_m_19',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_19.txt')
klambdas_desc[-18] = ('GGHH4B_rew_kl_m_18',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_18.txt')
klambdas_desc[-17] = ('GGHH4B_rew_kl_m_17',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_17.txt')
klambdas_desc[-16] = ('GGHH4B_rew_kl_m_16',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_16.txt')
klambdas_desc[-15] = ('GGHH4B_rew_kl_m_15',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_15.txt')
klambdas_desc[-14] = ('GGHH4B_rew_kl_m_14',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_14.txt')
klambdas_desc[-13] = ('GGHH4B_rew_kl_m_13',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_13.txt')
klambdas_desc[-12] = ('GGHH4B_rew_kl_m_12',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_12.txt')
klambdas_desc[-11] = ('GGHH4B_rew_kl_m_11',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_11.txt')
klambdas_desc[-10] = ('GGHH4B_rew_kl_m_10',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_10.txt')
klambdas_desc[-9]  = ('GGHH4B_rew_kl_m_9',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_9.txt')
klambdas_desc[-8]  = ('GGHH4B_rew_kl_m_8',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_8.txt')
klambdas_desc[-7]  = ('GGHH4B_rew_kl_m_7',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_7.txt')
klambdas_desc[-6]  = ('GGHH4B_rew_kl_m_6',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_6.txt')
klambdas_desc[-5]  = ('GGHH4B_rew_kl_m_5',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_5.txt')
klambdas_desc[-4]  = ('GGHH4B_rew_kl_m_4',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_4.txt')
klambdas_desc[-3]  = ('GGHH4B_rew_kl_m_3',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_3.txt')
klambdas_desc[-2]  = ('GGHH4B_rew_kl_m_2',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_2.txt')
klambdas_desc[-1]  = ('GGHH4B_rew_kl_m_1',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_m_1.txt')
klambdas_desc[-0]  = ('GGHH4B_rew_kl_p_0',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_0.txt')
klambdas_desc[1]   = ('GGHH4B_rew_kl_p_1',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_1.txt')
klambdas_desc[2]   = ('GGHH4B_rew_kl_p_2',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_2.txt')
klambdas_desc[3]   = ('GGHH4B_rew_kl_p_3',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_3.txt')
klambdas_desc[4]   = ('GGHH4B_rew_kl_p_4',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_4.txt')
klambdas_desc[5]   = ('GGHH4B_rew_kl_p_5',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_5.txt')
klambdas_desc[6]   = ('GGHH4B_rew_kl_p_6',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_6.txt')
klambdas_desc[7]   = ('GGHH4B_rew_kl_p_7',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_7.txt')
klambdas_desc[8]   = ('GGHH4B_rew_kl_p_8',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_8.txt')
klambdas_desc[9]   = ('GGHH4B_rew_kl_p_9',   '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_9.txt')
klambdas_desc[10]  = ('GGHH4B_rew_kl_p_10',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_10.txt')
klambdas_desc[11]  = ('GGHH4B_rew_kl_p_11',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_11.txt')
klambdas_desc[12]  = ('GGHH4B_rew_kl_p_12',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_12.txt')
klambdas_desc[13]  = ('GGHH4B_rew_kl_p_13',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_13.txt')
klambdas_desc[14]  = ('GGHH4B_rew_kl_p_14',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_14.txt')
klambdas_desc[15]  = ('GGHH4B_rew_kl_p_15',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_15.txt')
klambdas_desc[16]  = ('GGHH4B_rew_kl_p_16',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_16.txt')
klambdas_desc[17]  = ('GGHH4B_rew_kl_p_17',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_17.txt')
klambdas_desc[18]  = ('GGHH4B_rew_kl_p_18',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_18.txt')
klambdas_desc[19]  = ('GGHH4B_rew_kl_p_19',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_19.txt')
klambdas_desc[20]  = ('GGHH4B_rew_kl_p_20',  '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/plotterListFiles/2016NonResonantAnalysis/SKIM_GGHH4B_rew_kl_p_20.txt')

outPlotter  = '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/NominalHistos/Histos2016/outPlotter.root'
plotterCfg  = '/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/NominalHistos/Histos2016/plotter_2016NonResonantDiHiggs4B_skim++.cfg'



#############################################
### define the things to extract for the efficiencies
skim_denominator = entries['all'][0]
skim_vals_to_get = [x[0] for x in entries.values() if x[1] == 'skim']
sels_to_get      = [x[0] for x in entries.values() if x[1] == 'histo']

###################################################################################

# the container of all results
results_skim   = collections.OrderedDict()
cross_sections = collections.OrderedDict()

# skims
print "... checking skims"
for klambda, desc in klambdas_desc.items():
    # print klambda, desc
    filelist = desc[1]
    effs_skim = eut.get_eff_skims(
        filelist=filelist,
        vals_to_get = skim_vals_to_get,
        prepend_path = file_abs_path
    )
    results_skim[klambda] = effs_skim
# print results_skim

# xs
print "... extracting xs"
for klambda, desc in klambdas_desc.items():
    filelist = desc[1]
    xs = eut.extract_xs(
        filelist=filelist,
        prepend_path = file_abs_path
    )
    cross_sections[klambda] = xs
# print cross_sections

## histograms
print "... checking histograms"
sigs_to_get  = [x[0] for x in klambdas_desc.values()]
xs_to_unfold = {klambdas_desc[klambda][0] : cross_sections[klambda] for klambda in klambdas_desc.keys()}

## NB: all the histograms are normalised to : sum(w)^selected * lumi *  xs / sum(w)^total = eff * lumi * xs
## so I have to unscale the histos by the xs and the lumi to get the efficiency
## lumi is deduced from the cfg, for the xs I have to pass the map of the values previously obtained
histos_effs = eut.get_yields_plots (outPlotter, plotterCfg, sigs_to_get, sels_to_get, unfold_lumi = True, unfold_xs_map = xs_to_unfold)
# print histos_effs

###################################################################################
#### build the plots

graphs = collections.OrderedDict()

for tp in to_plot:
    ## FIXME: could become a TGraphAsymmErrors here
    g = ROOT.TGraph()
    for klambda in klambdas_desc.keys():
        x = klambda
        desc = entries[tp]
        this_name = desc[0]
        this_type = desc[1]
        if this_type == 'skim':
            y = results_skim[klambda][this_name]/results_skim[klambda][skim_denominator]
        elif this_type == 'histo':
            y = histos_effs [klambdas_desc[klambda][0]] [desc[0]] ## sample / selection
        else:
            raise RuntimeError('eff type must be skim or histo, but you set %s' % this_type)
        g.SetPoint(g.GetN(), x, y)
    
    g.SetLineColor(colors[tp])
    g.SetMarkerColor(colors[tp])
    g.SetMarkerStyle(8)
    g.SetMarkerSize(0.8)
    graphs[tp] = g

mg = ROOT.TMultiGraph()
for g in graphs.values():
    mg.Add(g, 'lp')

c1 = ROOT.TCanvas('c1', 'c1', 600, 600)
c1.SetFrameLineWidth(3)
c1.SetLeftMargin(0.13)
c1.SetBottomMargin(0.13)
c1.SetLogy()
mg.Draw('APL')

mg.SetMinimum(2e-3)
mg.SetMaximum(5)
mg.SetTitle(';#kappa_{#lambda};#varepsilon')
mg.GetXaxis().SetTitleSize(0.055)
mg.GetYaxis().SetTitleSize(0.055)
mg.GetXaxis().SetLabelSize(0.045)
mg.GetYaxis().SetLabelSize(0.045)
mg.GetXaxis().SetTitleOffset(1.08)
mg.GetYaxis().SetTitleOffset(1.08)
mg.Draw('APL')

leg = ROOT.TLegend(0.5, 0.7, 0.88, 0.88)
for name, gr in graphs.items():
    leg.AddEntry(gr, legs[name], "pl")
leg.SetFillStyle(0)
leg.SetBorderSize(0)
leg.Draw()
c1.Print('eff_vs_klambda.pdf', 'pdf')

