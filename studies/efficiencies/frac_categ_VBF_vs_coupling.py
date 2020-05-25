## to make all plots :
## for c in CV C2V kl ; do for p in VBFcateg GGFvsVBF GGFvsVBFonVBFevts ; do python frac_categ_VBF_vs_coupling.py $c $p ; done; done

import effutils.EffUtilsNew as eut
import effutils.HHscalings  as hhs
import ROOT
import numpy as np
import sys
import sympy

ROOT.gROOT.SetBatch(True)

# logy = True
# ymax = 5
# ymin = 1.e-3

couplingname = 'C2V' # the coupling to make eff vs : CV, C2V, kl - note : others are fixed to 1
plottype     = 'VBFcateg'
year         = 2018 ## used for the oname

if len(sys.argv) > 1:
    couplingname = sys.argv[1]

if len(sys.argv) > 2:
    plottype = sys.argv[2]

if len(sys.argv) > 3:
    year = int(sys.argv[3])


print '... doing coupling :', couplingname
print '... doing plot     :', plottype
print '... doing year     :', year


title     = 'VBF signal, %i simulation' % year
folder_in = '/uscms/home/guerrero/nobackup/Run2/HH2020/Spring/CMSSW_10_2_13/src/bbbbAnalysis/MyDataMCHistos/DataMCHistos%i' % year
fin       = '%s/outPlotter.root' % folder_in
file_in   = ROOT.TFile.Open(fin)
cfg_in    = folder_in + '/' + eut.findInFolder(folder_in, 'plotter_*.cfg')
print "[INFO] : input file is  ", fin
print "[INFO] : config file is ", cfg_in


if plottype == 'VBFcateg':
    #######################
    ## for VBF SM vs BSM categ
    to_plot = [
        'VBF_SMcat',
        'VBF_BSMcat',
    ]
    oname = 'frac_categ_%i_VBF_vs_%s.pdf' % (year, couplingname)
    logy = False
    ymax = 1.1
    ymin = 0


if plottype == 'GGFvsVBF':
    ######################
    ## for VBF vs GGF categ
    to_plot = [
        'GGFcateg',
        'VBFcateg',
    ]
    oname = 'frac_categ_%i_GGFvsVBF_VFBsignal_vs_%s.pdf' % (year, couplingname)
    logy = False
    ymax = 1.1
    ymin = 0


if plottype == 'GGFvsVBFonVBFevts':
    #######################
    ## for VBF vs GGF categ on VBFevents only
    to_plot = [
        'GGFcategForVBFEvts',
        'VBFcategForVBFEvts',
    ]
    oname = 'frac_categ_%i_GGFvsVBF_ForVBFEvts_VFBsignal_vs_%s.pdf' % (year, couplingname)
    logy = False
    ymax = 1.1
    ymin = 0


## these samples are added to the plots if required
## empty list for none
single_pts_list = [
    {
        'kl'       : 1,
        'CV'       : 1,
        'C2V'       : 1,
        'rootfile' : file_in,
        'name'     : 'qqHH_CV_1_C2V_1_kl_1',
        'cfg'      : cfg_in,
    },
    {
        'kl'       : 1,
        'CV'       : 1,
        'C2V'      : 2,
        'rootfile' : file_in,
        'name'     : 'qqHH_CV_1_C2V_2_kl_1',
        'cfg'      : cfg_in,
    },
    {
        'kl'       : 0,
        'CV'       : 1,
        'C2V'      : 1,
        'rootfile' : file_in,
        'name'     : 'qqHH_CV_1_C2V_1_kl_0',
        'cfg'      : cfg_in,
    },
    {
        'kl'       : 2,
        'CV'       : 1,
        'C2V'      : 1,
        'rootfile' : file_in,
        'name'     : 'qqHH_CV_1_C2V_1_kl_2',
        'cfg'      : cfg_in,
    },
    {
        'kl'       : 1,
        'CV'       : 0.5,
        'C2V'      : 1,
        'rootfile' : file_in,
        'name'     : 'qqHH_CV_0p5_C2V_1_kl_1',
        'cfg'      : cfg_in,
    },
    {
        'kl'       : 1,
        'CV'       : 1.5,
        'C2V'      : 1,
        'rootfile' : file_in,
        'name'     : 'qqHH_CV_1p5_C2V_1_kl_1',
        'cfg'      : cfg_in,
    },
]

input_samples = [
    eut.effReader(file_in, 'qqHH_CV_1_C2V_1_kl_1'),
    eut.effReader(file_in, 'qqHH_CV_1_C2V_2_kl_1'),
    eut.effReader(file_in, 'qqHH_CV_1_C2V_1_kl_0'),
    eut.effReader(file_in, 'qqHH_CV_1_C2V_1_kl_2'),
    eut.effReader(file_in, 'qqHH_CV_0p5_C2V_1_kl_1'),
    eut.effReader(file_in, 'qqHH_CV_1p5_C2V_1_kl_1'),
]

input_samples_pts = [eut.effReader(x['rootfile'], x['name']) for x in single_pts_list]

## scale by lumi and xs:
for e in input_samples:
    e.correctForXS(cfg_in)

for i in range(len(input_samples_pts)):
    input_samples_pts[i].correctForXS(single_pts_list[i]['cfg'])

## NOTE : the xs is actually taken from the files, not from val_xs
# VBF : val_CV, val_C2V, val_kl
VBF_sample_list = [
    hhs.VBFHHSample(1,1,1,   val_xs = 0.00054/(0.3364), label = 'qqHH_CV_1_C2V_1_kl_1'  ),
    hhs.VBFHHSample(1,2,1,   val_xs = 0.00472/(0.3364), label = 'qqHH_CV_1_C2V_2_kl_1'  ),
    hhs.VBFHHSample(1,1,2,   val_xs = 0.00044/(0.3364), label = 'qqHH_CV_1_C2V_1_kl_2'  ),
    hhs.VBFHHSample(1,1,0,   val_xs = 0.00145/(0.3364), label = 'qqHH_CV_1_C2V_1_kl_0'  ),
    hhs.VBFHHSample(0.5,1,1, val_xs = 0.00353/(0.3364), label = 'qqHH_CV_0p5_C2V_1_kl_1'),
    hhs.VBFHHSample(1.5,1,1, val_xs = 0.02149/(0.3364), label = 'qqHH_CV_1p5_C2V_1_kl_1'),
]

scales = hhs.VBFHHFormula(VBF_sample_list)

## now match the input samples to the sample list - use the name
### function has a 1-to-1 correspondence to *input_samples*
functions = []
for i in range(len(input_samples)):
    n1 = input_samples[i].process
    for j in range(len(VBF_sample_list)):
        n2 = VBF_sample_list[j].label
        if (n1 == n2):
            functions.append(scales.coeffs[j])
            break

if len(functions) != len(input_samples):
    raise RuntimeError("cannot match inputs to scalings")

functions_lam = [sympy.utilities.lambdify((sympy.Symbol('CV'), sympy.Symbol('C2V'), sympy.Symbol('kl')), fdesc) for fdesc in functions]

#########################################################################

# define the plot content

### maps the things to plot to the names they have
entries = {
    'VBF_SMcat'     : 'Btag4_VBFcateg1_SR_110_Histogram',
    'VBF_BSMcat'    : 'Btag4_VBFcateg2_SR_110_Histogram',
    'GGFcateg'      : 'Btag4_GGFcateg_Histogram',
    'VBFcateg'      : 'Btag4_VBFcateg_Histogram',
    'GGFcategForVBFEvts' : 'Btag4_VBFpreNoGGFKillercut_Histogram',
    'VBFcategForVBFEvts' : 'Btag4_VBFpreGGFKillercut_Histogram',
}

colors = {
    'VBF_SMcat'     : ROOT.kBlue+1,
    'VBF_BSMcat'      : ROOT.kRed,
    'GGFcateg'        : ROOT.kBlue+1,
    'VBFcateg'        : ROOT.kRed,
    'GGFcategForVBFEvts' : ROOT.kBlue+1,
    'VBFcategForVBFEvts' : ROOT.kRed,
}

legends = {
    'VBF_SMcat'  : 'SM VBF category (cat. 1)',
    'VBF_BSMcat' : 'BSM VBF category (cat. 2)',
    'GGFcateg'     : 'ggF category',
    'VBFcateg'     : 'VBF category',
    'GGFcategForVBFEvts'     : '4b + jj, ggF-killer reject',
    'VBFcategForVBFEvts'     : '4b + jj, ggF-killer accept',
}

### change here to vary the coupling plotted

if couplingname == 'CV':
    xtitle       = 'C_{V}'
    npts = 101
    kls  = np.ones(npts)
    CVs  = np.linspace(-5, 5, npts)
    C2Vs = np.ones(npts)
    icoupling_of_interest = 0 ## index in the ZIP function below

    # npts = 10
    # kls  = np.ones(npts)
    # CVs  = np.linspace(-1.2, -0.3, npts)
    # C2Vs = np.ones(npts)
    # icoupling_of_interest = 0 ## index in the ZIP function below


if couplingname == 'C2V':
    xtitle       = 'C_{2V}'
    npts = 101
    kls  = np.ones(npts)
    CVs  = np.ones(npts)
    C2Vs = np.linspace(-5, 5, npts)
    icoupling_of_interest = 1 ## index in the ZIP function below


if couplingname == 'kl':
    xtitle       = '#kappa_{#lambda}'
    couplingname = 'kl'
    npts = 101
    kls  = np.linspace(-20, 20, npts)
    CVs  = np.ones(npts)
    C2Vs = np.ones(npts)
    icoupling_of_interest = 2 ## index in the ZIP function below

couplings = zip(CVs, C2Vs, kls)
print "CV, C2V, kl"
print '   ->', couplings[0]
print '   ->', couplings[1]
print '   -> [...]'
print '   ->', couplings[-2]
print '   ->', couplings[-1]

#########################################################################
# make the plot

graphs = {}
for tp in to_plot:
    graphs[tp] = ROOT.TGraph()

for ipt, pt in enumerate(couplings):
    if ipt % 10 == 0 : print '... doing ', pt

    # print "======= DEBUG, couplings[ipt] = ", couplings[ipt]


    # scales = [functions[isample].evalf(subs = {'CV' : pt[0], 'C2V' : pt[1], 'kl' : pt[2]}) for isample in range(len(input_samples))]
    scales = [functions_lam[isample](pt[0], pt[1], pt[2]) for isample in range(len(input_samples))]

    values = {}

    for tp in to_plot:
        n_cat  = 0
        for isample, sample in enumerate(input_samples):
            nthis_tot     = sample.counts[entries[tp]] * scales[isample]
            n_cat  = n_cat + nthis_tot
            # print isample, sample.counts[entries[tp]], '*', scales[isample], '=', nthis_tot
        values[tp] = n_cat
        # print tp, '----->', values[tp]

    # print values
    # print sum(values.values())
    # print "......"

    tot_evts  = sum(values.values())
    for tp in to_plot:
    
        g = graphs[tp]
        g.SetPoint(g.GetN(), pt[icoupling_of_interest], 1.*values[tp]/tot_evts)

# ####### the check points

single_points_grs = {}
for tp in to_plot:
    single_points_grs[tp] = ROOT.TGraph()

for ipt, pt in enumerate(single_pts_list):
    kl  = pt['kl']
    CV  = pt['CV'] 
    C2V = pt['C2V']

    ## check which couplings were fixed and which were varied
    kl_fixed = False
    CV_fixed = False
    C2V_fixed = False

    if len(set(kls))  == 1: kl_fixed = True
    if len(set(CVs))  == 1: CV_fixed = True
    if len(set(C2Vs)) == 1: C2V_fixed = True

    kl_ref  = -1 if not kl_fixed else kls[0]
    CV_ref  = -1 if not CV_fixed else CVs[0]
    C2V_ref = -1 if not C2V_fixed else C2Vs[0]

    ## reject points that do not belong to this scal
    if kl_fixed and kl != kl_ref:
        continue
    if CV_fixed and CV != CV_ref:
        continue
    if C2V_fixed and C2V != C2V_ref:
        continue

    values = {}
    for tp in to_plot:
        values[tp] = input_samples_pts[ipt].counts[entries[tp]]

    for tp in to_plot:
        g = single_points_grs[tp]
        frac = values[tp] / sum(values.values())
        
        xcoupling = None
        if not kl_fixed:
            xcoupling = kl
        if not CV_fixed:
            xcoupling = CV
        if not C2V_fixed:
            xcoupling = C2V

        g.SetPoint(g.GetN(), xcoupling, frac)

        if kl == 1 and CV == 1 and C2V == 1: # printout
            print "... SM : Step : {:<20} - {:<40} : fraction : {:.2f}%".format(tp, legends[tp], 100.*frac) 


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
mg.SetTitle('%s;%s;Fraction' % (title, xtitle))
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


