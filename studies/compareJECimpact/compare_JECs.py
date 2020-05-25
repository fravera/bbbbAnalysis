import ROOT
import sys

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptStat(0)

def make_histogram(tIn, expr, cut, hname, nbins, xmin, xmax):
    hformat = 'h (%i, %f, %f)' % (nbins, xmin, xmax)
    tIn.Draw(expr + ' >> ' + hformat, cut)
    myh = ROOT.gPad.GetPrimitive("h");
    out_h = myh.Clone(hname)
    out_h.SetDirectory(0)
    return out_h


var = "HH_b1_pt"
cut = ""

xmin = 0
xmax = 400

# var = "H1_m"
# cut = ""

# xmin = 0
# xmax = 500


logy = False

if len(sys.argv) > 1:
    var = sys.argv[1]

if len(sys.argv) > 3:
    xmin = float(sys.argv[2])
    xmax = float(sys.argv[3])


print "... var : ", var , "[", xmin, "," , xmax, "]"
print "... cut : ", cut

# ########### JECs

# syst_list = [
#     'AbsoluteMPFBias',
#     'AbsoluteScale',
#     'AbsoluteStat',
#     'FlavorQCD',
#     'Fragmentation',
#     'PileUpDataMC',
#     'PileUpPtBB',
#     'PileUpPtEC1',
#     'PileUpPtEC2',
#     'PileUpPtHF',
#     'PileUpPtRef',
#     'RelativeFSR',
#     'RelativeJEREC1',
#     'RelativeJEREC2',
#     'RelativeJERHF',
#     'RelativePtBB',
#     'RelativePtEC1',
#     'RelativePtEC2',
#     'RelativePtHF',
#     'RelativeBal',
#     'RelativeSample',
#     'RelativeStatEC',
#     'RelativeStatFSR',
#     'RelativeStatHF',
#     'SinglePionECAL',
#     'SinglePionHCAL',
#     'TimePtEta',
# ]
# dirlist = ['up', 'down']

# fname_proto = 'root://cmseos.fnal.gov//store/user/lcadamur/bbbb_ntuples/testJEC2018/SKIM_GluGluToHHTo4B_node_SM_TuneCP5_PSWeights_13TeV-madgraph-pythia8_{syst}_{dir}/output/bbbbNtuple_0.root'
# fname_ref   = 'root://cmseos.fnal.gov//store/user/lcadamur/bbbb_ntuples/testJEC2018/SKIM_GluGluToHHTo4B_node_SM_TuneCP5_PSWeights_13TeV-madgraph-pythia8/output/bbbbNtuple_0.root'
# ofolder     = 'plots_2018'

################ JERs
syst_list = [ 'jer', 'bjer']
dirlist   = ['up', 'down']
fname_proto = 'root://cmseos.fnal.gov//store/user/lcadamur/bbbb_ntuples/testJER2/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_{syst}_{dir}/output/bbbbNtuple_0.root'
fname_ref   = 'root://cmseos.fnal.gov//store/user/lcadamur/bbbb_ntuples/testJEC2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph/output/bbbbNtuple_0.root'
ofolder     = 'plots'


histos_up = []
histos_down = []

for syst in syst_list:
    for dirr in dirlist:
        print '... doing', syst, dirr
        file = ROOT.TFile.Open(fname_proto.format(syst=syst, dir=dirr))
        tree = file.Get('bbbbTree')
        h = make_histogram(tree, var, cut, 'h_{syst}_{dir}'.format(syst=syst, dir=dirr), 100, xmin, xmax)

        if dirr == 'up':
            h.SetLineColor(ROOT.kGreen+1)
            histos_up.append(h)
        else:
            h.SetLineColor(ROOT.kBlue)
            histos_down.append(h)

## now make the nominal one
file = ROOT.TFile.Open(fname_ref)
tree = file.Get('bbbbTree')
ref  = make_histogram(tree, var, cut, 'h_{syst}_{dir}'.format(syst=syst, dir=dirr), 100, xmin, xmax)

c1 = ROOT.TCanvas('c1', 'c1', 600, 600)
c1.SetLogy(logy)

ref.SetLineColor(ROOT.kRed)
ref.SetLineWidth(3)

ref.Draw()
for idx in range(len(syst_list)):
    histos_up[idx].Draw('same')
    histos_down[idx].Draw('same')

c1.Print('%s/JECcompare_%s.pdf' % (ofolder, var), 'pdf')

cloneref = ref.Clone('cloneref')

## now print the impact of each JEC
for i, syst in enumerate(syst_list):
    print '... plotting', syst
    h_v1 = histos_up[i] ## up
    h_v2 = histos_down[i] ## down
    cloneref.SetTitle(syst)
    cloneref.Draw()
    h_v1.Draw('same')
    h_v2.Draw('same')
    c1.Print('%s/comp_%s_%s.pdf' % (ofolder, var, syst), 'pdf')
