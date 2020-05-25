import effutils.EffUtilsNew as eut
import effutils.HHscalings  as hhs
import ROOT
import numpy as np
import sys
import sympy

ROOT.gROOT.SetBatch(True)

year = 2018
if len(sys.argv) > 1:
    year = int(sys.argv[1])

print '... doing year :' , year

oname     = 'eff_%i_vs_klambda.pdf' % year
title     = "ggF signal, %i simulation" % year
folder_in = '/uscms/home/guerrero/nobackup/Run2/HH2020/Spring/CMSSW_10_2_13/src/bbbbAnalysis/MyDataMCHistos/DataMCHistos%i' % year
fin       = '%s/outPlotter.root' % folder_in
file_in   = ROOT.TFile.Open(fin)
cfg_in    = folder_in + '/' + eut.findInFolder(folder_in, 'plotter_*.cfg')
print "[INFO] : input file is  ", fin
print "[INFO] : config file is ", cfg_in

# logy = False
# ymax = 1.1
# ymin = 0

logy = True
ymax = 5
ymin = 1.e-3

## these samples are added to the plots if required
## empty list for none
single_pts_list = [
    {
        'kl'       : 1,
        'rootfile' : file_in,
        'name'     : 'ggHH_kl_1_kt_1',
        'cfg'      : cfg_in,
    },
    {
        'kl'       : 0,
        'rootfile' : file_in,
        'name'     : 'ggHH_kl_0_kt_1',
        'cfg'      : cfg_in,
    },
    {
        'kl'       : 2.45,
        'rootfile' : file_in,
        'name'     : 'ggHH_kl_2p45_kt_1',
        'cfg'      : cfg_in,
    },
    {
        'kl'       : 5,
        'rootfile' : file_in,
        'name'     : 'ggHH_kl_5_kt_1',
        'cfg'      : cfg_in,
    },
]

# year = 2018

# if year == 2018:
#     file_in = ROOT.TFile.Open('/uscms/home/guerrero/nobackup/Run2/HH2019/Fall2019/CMSSW_10_2_5/src/bbbbAnalysis/MyHistos/Histos2018/outPlotter.root')
#     lumi    = 59740 # pb


input_samples = [
    eut.effReader(file_in, 'ggHH_kl_0_kt_1'),
    eut.effReader(file_in, 'ggHH_kl_1_kt_1'),
    eut.effReader(file_in, 'ggHH_kl_2p45_kt_1'),
    # eut.effReader(file_in, 'ggHH_kl_5_kt_1'),
]

input_samples_pts = [eut.effReader(x['rootfile'], x['name']) for x in single_pts_list]

## scale by lumi and xs:
for e in input_samples:
    e.correctForXS(cfg_in)

for i in range(len(input_samples_pts)):
    input_samples_pts[i].correctForXS(single_pts_list[i]['cfg'])

## NOTE : the xs is actually taken from the files, not from val_xs
GGF_sample_list = [
    hhs.GGFHHSample(0,1,    val_xs = 0.06007, label = 'ggHH_kl_0_kt_1'  ),
    hhs.GGFHHSample(1,1,    val_xs = 0.02675, label = 'ggHH_kl_1_kt_1'  ),
    hhs.GGFHHSample(2.45,1, val_xs = 0.01133, label = 'ggHH_kl_2p45_kt_1'  ),
    # hhs.GGFHHSample(5,1,    val_xs = 0.07903, label = 'ggHH_kl_5_kt_1'  ),
]


scales = hhs.GGFHHFormula(GGF_sample_list)

## now match the input samples to the sample list - use the name
### function has a 1-to-1 correspondence to *input_samples*
functions = []
for i in range(len(input_samples)):
    n1 = input_samples[i].process
    for j in range(len(GGF_sample_list)):
        n2 = GGF_sample_list[j].label
        if (n1 == n2):
            functions.append(scales.coeffs[j])
            break

if len(functions) != len(input_samples):
    raise RuntimeError("cannot match inputs to scalings")

functions_lam = [sympy.utilities.lambdify((sympy.Symbol('kl'), sympy.Symbol('kt')), fdesc) for fdesc in functions]


# for f in functions:
#     print f.evalf(subs = {'kl' : 1, 'kt' : 1})

#########################################################################

# define the plot content

to_plot = [
    'trigger',
    'presel',
    '4b',
    'ggf',
    'sr',
]

### maps the things to plot to the names they have
entries = {
    'trigger' : 'Ntrg_w',
    'presel'  : 'Nsel_w',
    '4b'      : 'Btag4_Histogram',
    'ggf'     : 'Btag4_GGFcateg_Histogram',
    'sr'      : 'Btag4_GGFcateg_SR_110_Histogram',
}

colors = {
    'trigger' : ROOT.kBlack,
    'presel'  : ROOT.kGreen+1,
    '4b'      : ROOT.kRed,
    'ggf'     : ROOT.kBlue+1,
    'sr'      : ROOT.kOrange+1
}

legends = {
    'trigger' : 'Trigger',
    'presel'  : 'Preselection (4 jet, 3 b)',
    '4b'      : '4 b-tagged jets',
    'ggf'     : 'ggF category',
    'sr'      : 'Signal region',
}

klambdas = np.linspace(-20, 20, 101)
# klambdas = [-2, -1, 0, 1.0, 2, 3, 4]

#########################################################################
# make the plot

graphs = {}
for tp in to_plot:
    graphs[tp] = ROOT.TGraph()

for ikl, kl in enumerate(klambdas):
    if ikl % 10 == 0 : print '... doing ', kl
    # scales = [functions[isample].evalf(subs = {'kl' : kl, 'kt' : 1}) for isample in range(len(input_samples))]
    scales = [functions_lam[isample](kl, 1) for isample in range(len(input_samples))]
    for tp in to_plot:
        ntot  = 0
        npass = 0
        for isample, sample in enumerate(input_samples):
            nthis_tot     = sample.counts[sample.tot_w_name] * scales[isample]
            nthis_pass    = sample.counts[entries[tp]]       * scales[isample]
            ntot  = ntot + nthis_tot
            npass = npass + nthis_pass

            # print sample.process
            # print 1.*nthis_pass/nthis_tot
            # print 1.*sample.counts[entries[tp]]/sample.counts[sample.tot_w_name]
            # print nthis_tot,  sample.counts[sample.tot_w_name], scales[isample]
            # print nthis_pass, sample.counts[entries[tp]], scales[isample]
            # print sample.get_eff(entries[tp])

        g = graphs[tp]
        g.SetPoint(g.GetN(), kl, 1.*npass/ntot)

####### the check points

single_points_grs = {}
for tp in to_plot:
    single_points_grs[tp] = ROOT.TGraph()

for ipt, pt in enumerate(single_pts_list):
    kl = pt['kl']
    for tp in to_plot:
        g = single_points_grs[tp]
        eff = input_samples_pts[ipt].get_eff(entries[tp])
        g.SetPoint(g.GetN(), kl, eff)

        if kl == 1: # printout
            print "... kl = 1 : Step : {:<20} - {:<40} : eff : {:.2f}%".format(tp, legends[tp], 100.*eff) 


########################################################################
# set the styles

for tp in to_plot:
    graphs[tp].SetLineColor(colors[tp])
    graphs[tp].SetMarkerColor(colors[tp])
    graphs[tp].SetMarkerStyle(8)
    graphs[tp].SetMarkerSize(0.4)

    if tp in single_points_grs : 
        single_points_grs[tp].SetLineColor(colors[tp])
        single_points_grs[tp].SetMarkerColor(colors[tp])
        # single_points_grs[tp].SetMarkerStyle(4) 
        # single_points_grs[tp].SetMarkerSize(0.8)
        single_points_grs[tp].SetMarkerStyle(5)
        single_points_grs[tp].SetMarkerSize(1.5)



mg = ROOT.TMultiGraph()
for tp in to_plot:
    mg.Add(graphs[tp], 'lp')
    if tp in single_points_grs : mg.Add(single_points_grs[tp], 'p')

c1 = ROOT.TCanvas('c1', 'c1', 600, 600)
c1.SetFrameLineWidth(3)
c1.SetLeftMargin(0.13)
c1.SetBottomMargin(0.13)
c1.SetLogy(logy)
mg.Draw('APL')

# mg.SetMinimum(2e-3)
mg.SetMaximum(ymax)
mg.SetMinimum(ymin)
mg.SetTitle('%s;#kappa_{#lambda};#varepsilon' % title)
mg.GetXaxis().SetTitleSize(0.055)
mg.GetYaxis().SetTitleSize(0.055)
mg.GetXaxis().SetLabelSize(0.045)
mg.GetYaxis().SetLabelSize(0.045)
mg.GetXaxis().SetTitleOffset(1.08)
mg.GetYaxis().SetTitleOffset(1.08)
mg.Draw('APL')

leg = ROOT.TLegend(0.5, 0.7, 0.88, 0.88)
for tp in to_plot:
    leg.AddEntry(graphs[tp], legends[tp], "pl")
leg.SetFillStyle(0)
leg.SetBorderSize(0)
leg.Draw()
c1.Print(oname, 'pdf')


