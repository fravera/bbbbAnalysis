import ROOT
ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

ROOT.gStyle.SetHatchesSpacing(0.5)

import sys
import math

def make_syst_histo(syst_list, proto, nominal, fIn, name, method = 'quadrature'):
    histos = []
    for s in syst_list:
        # print name, s
        h = fIn.Get(proto.format(syst=s))
        histos.append(h)

    hout = nominal.Clone(name)
    for ibin in range(1, hout.GetNbinsX()+1):
        y = hout.GetBinContent(ibin)
        deltas = []
        for ih, h in enumerate(histos):
            yh = h.GetBinContent(ibin)
            d  = yh - y
            # if ibin == 1: print '>>>', name, ibin, syst_list[ih], y, yh, d
            deltas.append(d)
            # print "...", h.GetName(), y, yh, d
        
        if method =='quadrature':
            sqs_up = [d*d for d in deltas if d >= 0]
            sqs_dn = [d*d for d in deltas if d < 0]
            err_up = math.sqrt(sum(sqs_up))
            err_dn = math.sqrt(sum(sqs_dn))
            err = max(err_up, err_dn)

        elif method =='maxerr':
            err  = max([abs(d) for d in deltas])
        
        else:
            raise RuntimeError('method %s not implemented' % method)

        # print y, err
        # if ibin == 1: print "...", name, ibin, err
        hout.SetBinError(ibin, err)

    return hout

def make_syst_ratio_band(denom, hsyst):
    hband = denom.Clone(hsyst.GetName() + '_ratio')
    hband.Divide(denom)
    for ibin in range(1, hband.GetNbinsX()+1):
        y = denom.GetBinContent(ibin)
        e = denom.GetBinError(ibin)
        erel = abs(e/y) if y > 0 else 0
        hband.SetBinError(ibin, erel) ## in a ratio, the error is the relative error w.r.t the bin content
    return hband


fIn = ROOT.TFile('ggHHjets_2/outPlotter.root')

# var   = 'j1j2_deltaEta'
# xtitle = '#Delta#eta(j_{1}, j_{2})'

var   = 'JJ_m'
xtitle = 'm_{jj} [GeV]'

if len(sys.argv) > 2:
    var    = sys.argv[1]
    xtitle = sys.argv[2]

print '... var    : ', var
print '... xtitle : ', xtitle

title = ';%s;Events' % xtitle

sel   = 'Btag4_SR_110_VBF_Histogram' # Btag4_SR_110_Histogram, Btag4_SR_110_VBF_Histogram
if len(sys.argv) > 3:
   sel = sys.argv[3]

print '... sel    : ', sel 

add_GEN_err   = True

LHEscale_syst_list = [
    'LHEScaleWeight_var0',
    'LHEScaleWeight_var1',
    'LHEScaleWeight_var2',
    'LHEScaleWeight_var3',
    'LHEScaleWeight_var4',
    'LHEScaleWeight_var5',
    'LHEScaleWeight_var6',
    'LHEScaleWeight_var7',
    'LHEScaleWeight_var8'
]

PSweights_syst_list = [
    'PSWeight_var0',
    'PSWeight_var1',
    'PSWeight_var2',
    'PSWeight_var3'
]

h_LO   = fIn.Get('{sample}/{selection}/{sample}_{selection}_{variable}'.format(sample = 'ggHH_LO',   selection = sel, variable = var))
h_LOjj = fIn.Get('{sample}/{selection}/{sample}_{selection}_{variable}'.format(sample = 'ggHHjj_LO', selection = sel, variable = var))
h_NLO  = fIn.Get('{sample}/{selection}/{sample}_{selection}_{variable}'.format(sample = 'ggHH_NLO',  selection = sel, variable = var))

h_LO.SetLineColor(ROOT.kBlue)
h_LOjj.SetLineColor(ROOT.kRed)
h_NLO.SetLineColor(ROOT.kGreen+1)

h_LO.SetMarkerColor(ROOT.kBlue)
h_LOjj.SetMarkerColor(ROOT.kRed)
h_NLO.SetMarkerColor(ROOT.kGreen+1)

h_LO.SetMarkerStyle(8)
h_LOjj.SetMarkerStyle(8)
h_NLO.SetMarkerStyle(8)

h_LO.SetMarkerSize(0.6)
h_LOjj.SetMarkerSize(0.6)
h_NLO.SetMarkerSize(0.6)

if add_GEN_err:
    nominal = h_NLO
    proto   = '{sample}/{selection}/{sample}_{selection}_{variable}'.format(sample = 'ggHH_NLO',   selection = sel, variable = var) + '_{syst}'
    hsyst_LHEscales_NLO = make_syst_histo(LHEscale_syst_list, proto, nominal, fIn, 'hsyst_LHEscales_NLO')
    hsyst_LHEscales_NLO.SetFillStyle(3353)
    hsyst_LHEscales_NLO.SetFillColor(ROOT.kRed+1)
    hsyst_LHEscales_NLO.SetLineWidth(0)

    hsyst_PSweights_NLO = make_syst_histo(PSweights_syst_list, proto, nominal, fIn, 'hsyst_PSweights_NLO')
    hsyst_PSweights_NLO.SetFillStyle(3335)
    hsyst_PSweights_NLO.SetFillColor(ROOT.kBlue+2)
    hsyst_PSweights_NLO.SetLineWidth(0)
    
    hsyst_sum = make_syst_histo(LHEscale_syst_list + PSweights_syst_list, proto, nominal, fIn, 'hsyst_sum')
    hsyst_sum.SetFillStyle(3002)
    hsyst_sum.SetFillColor(ROOT.kBlack)
    hsyst_sum.SetLineWidth(0)


else:
    hsyst_LHEscales_NLO = None
    hsyst_PSweights_NLO = None
    hsyst_sum = None

mmaxs = [
    h_LO.GetMaximum(),
    h_LOjj.GetMaximum(),
    h_NLO.GetMaximum(),
]

mmax = max(mmaxs)

logy       = True
ratiopanel = True

if len(sys.argv) > 4:
    logy = False if sys.argv[4] == '0' else True

print "... logy?  : ", logy

########################
labelfont = 43
labelsize = 20

titlefont = 43
titlesize = 25

ytoffset  = 1.4

########################

h_LO.SetMaximum(10 * mmax if logy else 1.15*mmax)
if not logy: h_LO.SetMinimum(0)
h_LO.SetTitle(title)
h_LO.GetXaxis().SetTitleFont(titlefont)
h_LO.GetXaxis().SetTitleSize(0 if ratiopanel else titlesize)
h_LO.GetXaxis().SetLabelFont(labelfont)
h_LO.GetXaxis().SetLabelSize(0 if ratiopanel else labelsize)

h_LO.GetYaxis().SetTitleFont(titlefont)
h_LO.GetYaxis().SetTitleSize(titlesize)
h_LO.GetYaxis().SetLabelFont(labelfont)
h_LO.GetYaxis().SetLabelSize(labelsize)

h_LO.GetYaxis().SetTitleOffset(ytoffset)

c1 = ROOT.TCanvas('c1', 'c1', 600, 600)
c1.SetFrameBorderSize(3)

if ratiopanel:
    pad1 = ROOT.TPad('pad1', 'Mainpad',  0.0, 0.3, 1.0, 1.0)
    pad2 = ROOT.TPad('pad2', 'Ratiopad', 0.0, 0.0, 1.0, 0.3)

    pad1.SetBottomMargin(0.01)
    pad1.SetLeftMargin(0.13)

    pad2.SetTopMargin(0.02)
    pad2.SetBottomMargin(0.35)
    pad2.SetLeftMargin(0.13)

else:
    pad1 = ROOT.TPad('pad1', 'Mainpad',  0.0, 0.0, 1.0, 1.0)
    pad2 = None

if pad1: pad1.Draw()
if pad2: pad2.Draw()

pad1.cd()
pad1.SetLogy(logy)

h_LO.Draw()
h_LOjj.Draw('same')
h_NLO.Draw('same')
if hsyst_LHEscales_NLO: hsyst_LHEscales_NLO.Draw("E2 same")
if hsyst_PSweights_NLO: hsyst_PSweights_NLO.Draw("E2 same")
if hsyst_sum:           hsyst_sum.Draw("E2 same")

leg = ROOT.TLegend(0.1, 0.92, 0.95, 0.98)
leg.SetNColumns(3)
leg.AddEntry(h_LO,   'LO',      'lep')
leg.AddEntry(h_LOjj, 'LO + jj', 'lep')
leg.AddEntry(h_NLO,  'NLO',     'lep')
leg.SetBorderSize(0)
leg.SetFillStyle(0)
leg.Draw()

if hsyst_LHEscales_NLO or hsyst_PSweights_NLO or hsyst_sum:
    leg_var = ROOT.TLegend(0.63, 0.6, 0.88, 0.75)
    leg_var.SetBorderSize(0)
    leg_var.SetFillStyle(0)
    if hsyst_LHEscales_NLO : leg_var.AddEntry(hsyst_LHEscales_NLO, 'LHE scale', 'f')
    if hsyst_PSweights_NLO : leg_var.AddEntry(hsyst_PSweights_NLO, 'PS weights', 'f')
    if hsyst_sum           : leg_var.AddEntry(hsyst_sum, 'Sum', 'f')
    leg_var.Draw()

## a label with category
label = None
if sel == 'Btag4_SR_110_VBF_Histogram':
    label = 'bbbb + jj region'
if sel == 'Btag4_SR_110_Histogram':
    label = 'bbbb region'
tlabel = ROOT.TLatex(0.85, 0.85, label)
tlabel.SetNDC(True)
tlabel.SetTextFont(43)
tlabel.SetTextSize(20)
tlabel.SetTextAlign(33)
tlabel.Draw()


if ratiopanel:
    pad2.cd()
    histos = [h_LO, h_LOjj, h_NLO]
    denom  = h_NLO
    frame  = denom.Clone('frame')
    frame.Reset()
    frame.SetTitle(';%s;Ratio to NLO' % xtitle)

    frame.GetXaxis().SetTitleFont(titlefont)
    frame.GetXaxis().SetTitleSize(titlesize)
    frame.GetXaxis().SetLabelFont(labelfont)
    frame.GetXaxis().SetLabelSize(labelsize)

    frame.GetYaxis().SetTitleFont(titlefont)
    frame.GetYaxis().SetTitleSize(titlesize)
    frame.GetYaxis().SetLabelFont(labelfont)
    frame.GetYaxis().SetLabelSize(labelsize)

    frame.GetXaxis().SetTitleOffset(3)
    frame.GetYaxis().SetTitleOffset(ytoffset)
    
    frame.SetMinimum(0.01)
    frame.SetMaximum(2.99)
    frame.Draw('hist')
    hnews = []
    for h in histos:
        if h is denom:
            continue
        # print h.GetName()
        hnew = h.Clone('ratio_%s' % h.GetName())
        hnew.Divide(denom)
        hnews.append(hnew)
        hnew.Draw('same')

    ### the ratio:
    if hsyst_LHEscales_NLO:
        hsyst_LHEscales_NLO_ratio = make_syst_ratio_band (hsyst_LHEscales_NLO, denom)
        hsyst_LHEscales_NLO_ratio.Draw('E2 same')

    if hsyst_PSweights_NLO:
        hsyst_PSweights_NLO_ratio = make_syst_ratio_band (hsyst_PSweights_NLO, denom)
        hsyst_PSweights_NLO_ratio.Draw('E2 same')

    if hsyst_sum:
        hsyst_sum_ratio = make_syst_ratio_band (hsyst_sum, denom)
        hsyst_sum_ratio.Draw('E2 same')


    l = ROOT.TLine(frame.GetBinLowEdge(1), 1, frame.GetBinLowEdge(frame.GetNbinsX()+1), 1)
    l.SetLineStyle(8)
    l.Draw()

c1.Print('ggHHjjplots/comp_{var}_{sel}.pdf'.format(var=var, sel=sel), 'pdf')