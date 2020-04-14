from ROOT import TFile

filename = 'test_NMSSM_XYH_bbbb_MC.root'
f = TFile(filename)
t = f.Get('bbbbTree')
Ntot = float(t.GetEntries())

count_H_match, count_Jet_match = 0, 0
for evt in t:
    if (evt.gen_H1_b1_matchedflag > -1 and evt.gen_H1_b2_matchedflag > -1 and evt.gen_H2_b1_matchedflag > -1 and evt.gen_H2_b2_matchedflag > -1):
        count_H_match += 1 

    if (evt.recoJetMatchedToGenJet1 > -1 and evt.recoJetMatchedToGenJet2 > -1 and     evt.recoJetMatchedToGenJet3 > -1 and evt.recoJetMatchedToGenJet4 > -1):
        count_Jet_match += 1

H_eff = float(count_H_match) / Ntot
jet_eff = float(count_Jet_match) / Ntot

print("Efficiency for jets assigned to a particular H is {}.".format(H_eff))
print("Efficiency for jets without assignment is {}.".format(jet_eff))