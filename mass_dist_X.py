# The following code is built to loop over ROOT files constructed from different selection criteria (various Delta R values). The purpose is to display the mass distribution for the different values of Delta R. 

print("[INFO] Importing libraries...")
from ROOT import TFile, TH1F
import numpy as np
import os
import matplotlib.pyplot as plt
plt.rcParams.update({'font.size': 8})
from matplotlib import rc
from matplotlib.ticker import MultipleLocator

# Below, the code will loop through the ROOT files in the directory. The files will not necessarily be in order of increasing DeltaR, so I will "sort" them myself.
ratios = ['0.10', '0.20', '0.05', '0.15', '0.25', '0.30', '0.35', '0.40']
ratios_sorted = list(ratios)
ratios_sorted.sort()
d = {}
for i,ratio in enumerate(ratios_sorted):
    d[ratios_sorted[i]] = i

# This function checks that all four b quarks have been given an assignment of 0 or 1 (instead of the initialized -1 for the matchedflag branches). Essentially, it checks that all 4 b quarks have been flagged (not necessarily matched).
def checkFlags(l1,l2,l3,l4):
    mask = []
    for i in range(len(l1)):
        if (l1[i] >=0 and l2[i] >= 0 and l3[i] >=0 and l4[i] >= 0):
                mask.append(True)
        else:
                mask.append(False)
    return np.asarray(mask)

fig, axs = plt.subplots(2, 4)#, tight_layout=True)
fig.tight_layout(pad=4.0)

print("[INFO] Looping through files in directory...")
count = 0
num_bins = 15
for filename in os.listdir(os.getcwd()): # Loop through files in directory
    if filename.endswith(".root"): # Catch files with .root extension
        print("[INFO] Opening {}".format(filename))
        f = TFile(filename)
        t = f.Get("bbbbTree")
        H1b1, H1b2, H2b1, H2b2 = [], [], [], []
        H1m, H2m, HHm = [], [], []
        mask = []
        print("[INFO] Looping over events in file...")
        for evt in t:
            H1m.append(evt.H1_m)
            H2m.append(evt.H2_m)
            HHm.append(evt.HH_m)
            H1b1.append(evt.gen_H1_b1_matchedflag)
            H1b2.append(evt.gen_H1_b2_matchedflag)
            H2b1.append(evt.gen_H2_b1_matchedflag)
            H2b2.append(evt.gen_H2_b2_matchedflag)

        mask = checkFlags(H1b1, H1b2, H2b1, H2b2)
            
        H1m, H2m, HHm = np.asarray(H1m), np.asarray(H2m), np.asarray(HHm)
        H1m, H2m, HHm = H1m[mask], H2m[mask], HHm[mask]

        # Create text label to be displayed on histograms
        text = r'$\Delta$ R ='
        text += ratios[count]
        ind = d[ratios[count]]


        data = HHm
        # num_bins = int(np.floor((np.max(data) - np.min(data))/15)) # Dynamic binning
        binwidth = 35
        y_text = 400
        # num_bins = np.arange(min(data), max(data) + binwidth, binwidth)
        # axs[ind/4][ind%4].text(H1_txt_coords[0],H1_txt_coords[1],text)
        # axs[ind/4][ind%4].set_ylim(ymin=1, ymax=1000)
        # axs[ind/4][ind%4].set_xlim(xmin=0, xmax=400)

        x_text = np.max(data) - ( np.max(data) - np.min(data) )/2 # x-coord of text placement

        # The following code block ensures that the plots are displayed in order of increasing Delta R values, even if the files are not in order in the directory.
        axs[ind/4][ind%4].xaxis.set_major_locator(MultipleLocator(300))
        if ind/4 == 0 and ind%4 == 0:
            axs[ind/4][ind%4].xaxis.set_major_locator(MultipleLocator(100))
            y_text = 60
        if ind/4 == 1 and (ind%4 == 1 or ind%4 == 2):
            axs[ind/4][ind%4].xaxis.set_major_locator(MultipleLocator(400))
        if ind/4 == 1 and ind%4 == 3:
            axs[ind/4][ind%4].xaxis.set_major_locator(MultipleLocator(500))
        axs[ind/4][ind%4].text(x_text,y_text,text)
        axs[ind/4][ind%4].set_yscale("log")
        axs[ind/4][ind%4].hist(data, label=text, bins=30)

        count += 1

plt.tight_layout()
filename = 'HHm_distribution.png'
plt.savefig(filename)
print 'Figure saved as ' + filename



