# The following code is built to loop over ROOT files constructed from different selection criteria (various Delta R values). The purpose is to display the mass distribution for the different values of Delta R. 

print("[INFO] Importing libraries....")
from ROOT import TFile, TH1F
import numpy as np
import os
import matplotlib.pyplot as plt
plt.rcParams.update({'font.size': 8})
from matplotlib import rc
from matplotlib.ticker import MultipleLocator

# Below, the code will loop through the files in the directory. The files will not necessarily be in order of increasing DeltaR, so I will "sort" them myself.
ratios = ['0.10', '0.20', '0.05', '0.15', '0.25', '0.30', '0.35', '0.40']
ratios_sorted = list(ratios)
ratios_sorted.sort()
d = {}
for i,ratio in enumerate(ratios_sorted):
    d[ratios_sorted[i]] = i

r10, r20, r05, r15, r25, r30, r35, r40 = [], [], [], [], [], [], [], []

# fig, axs = plt.subplots(2, 4)#, tight_layout=True)
fig, ax = plt.subplots()
fig.tight_layout(pad=4.0)

count = 0
num_bins = 15
for filename in os.listdir(os.getcwd()): # Loop over files in directory
    if filename.endswith(".root"): # Catch files with .root extension
        print("[INFO] Opening ".format(filename))
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

        H1b1, H1b2, H2b1, H2b2 = np.asarray(H1b1), np.asarray(H1b2), np.asarray(H2b1), np.asarray(H2b2)

        H1b1valid = np.asarray([i >= 0 for i in H1b1])
        H1b2valid = np.asarray([i >= 0 for i in H1b2])
        H2b1valid = np.asarray([i >= 0 for i in H2b1])
        H2b2valid = np.asarray([i >= 0 for i in H2b2])

        # Create masks to keep only events for which b jets have been flagged (with 0 or 1 - default value is -1).
        e11 = float(len(H1b1[H1b1valid]))/float(len(H1b1))
        e12 = float(len(H1b2[H1b2valid]))/float(len(H1b2))
        e21 = float(len(H2b1[H2b1valid]))/float(len(H2b1))
        e22 = float(len(H2b2[H2b2valid]))/float(len(H2b2))

        if count == 0:
            r10.append([e11,e12,e21,e22])
        elif count == 1:
            r20.append([e11,e12,e21,e22])
        elif count == 2:
            r05.append([e11,e12,e21,e22])
        elif count == 3:
            r15.append([e11,e12,e21,e22])
        elif count == 4:
            r25.append([e11,e12,e21,e22])
        elif count == 5:
            r30.append([e11,e12,e21,e22])
        elif count == 6:
            r35.append([e11,e12,e21,e22])
        else: r40.append([e11,e12,e21,e22])

        count+=1

### Plot efficiency.
x1 = np.repeat(0.10,4)
x2 = np.repeat(0.20,4)
x3 = np.repeat(0.05,4)
x4 = np.repeat(0.15,4)
x5 = np.repeat(0.25,4)
x6 = np.repeat(0.30,4)
x7 = np.repeat(0.35,4)
x8 = np.repeat(0.40,4)
plt.scatter(x1, r10, color='green', label=r'$\Delta$R = 0.10')
plt.scatter(x2, r20, color='blue', label=r'$\Delta$R = 0.20')
plt.scatter(x3, r05, color='red', label=r'$\Delta$R = 0.05')
plt.scatter(x4, r15, color='orange', label=r'$\Delta$R = 0.15')
plt.scatter(x5, r25, color='purple', label=r'$\Delta$R = 0.25')
plt.scatter(x6, r30, color='pink', label=r'$\Delta$R = 0.30')
plt.scatter(x7, r35, color='darkgreen', label=r'$\Delta$R = 0.35')
plt.scatter(x8, r40, color='darkblue', label=r'$\Delta$R = 0.40')
plt.legend(loc='lower right')
plt.xlabel(r'$\Delta$R threshold')
plt.ylabel('Efficiency') 
plt.savefig('efficiency_colors.pdf')


