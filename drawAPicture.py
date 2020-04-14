from ROOT import TFile, TCanvas, TH1F, gROOT, kTRUE
gROOT.SetBatch(kTRUE) # Do not display canvas

def drawPortrait(filename, eosbool):
    """
    The idea for this program is to take in a ROOT file and draw all of the interesting branches.
    """

    f = TFile(filename,"read")
    filename = filename.split('.')[0] + '.pdf'
    print(filename)

    t = f.Get("bbbbTree")

    # c1 = TCanvas("c","nJets")

    c1 = TCanvas("c1","pT Distribution")
    c1.Divide(2,2)
    c1.Print(filename+"[")

    count = 0
    h1 = []
    for i in range(4):
        print("[INFO] Moving into gPad {}".format(i+1))
        c1.cd(i+1)
        title = "H{}_b{}_ptRegressed >> h{}".format(i/2+1,i%2+1,i)
        print("[INFO] Creating histogram titled {}".format(title).split('>>')[0])
        h1.append(TH1F("h{}".format(i),title.split('>>')[0],100,0,850))
        t.Draw(title)
        h1[i].Draw()
        c1.Update()
        count+=1
    c1.Print(filename)

    c1 = TCanvas("c2","Angular Distribution")
    c1.Divide(2,2)

    for j in range(2):
        print("[INFO] Moving into gPad {}".format(j+1))
        c1.cd(j+1)
        title = "H{}_bb_deltaPhi >> h{}".format(j+1,count)
        print("[INFO] Creating histogram titled {}".format(title).split('>>')[0])
        h1.append(TH1F("h{}".format(count),title.split('>>')[0],100,0,7))
        t.Draw(title)
        h1[count].Draw()
        c1.Update()
        count+=1

    for i in range(2):
        print("[INFO] Moving into gPad {}".format(i+1))
        c1.cd(i+3)
        title = "H{}_bb_deltaEta >> h{}".format(j+1,count)
        print("[INFO] Creating histogram titled {}".format(title).split('>>')[0])
        h1.append(TH1F("h{}".format(count),title.split('>>')[0],100,0,7))
        t.Draw(title)
        h1[count].Draw()
        c1.Update()
        count+=1

    c1.Print(filename)

    c1 = TCanvas("c3","nJet")
    h = TH1F("h","nJet",11,0,18)
    t.Draw("nJet >> h")
    h.Draw()
    c1.Update()
    
    c1.Print(filename)
    c1.Print(filename+"]")

drawPortrait('test_NMSSM_XYH_bbbb_MC_selectedJets_20GeVpT.root',False)