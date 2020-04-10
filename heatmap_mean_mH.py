import pandas as pd
import seaborn as sns
import matplotlib.pyplot as plt
from matplotlib import cm
from matplotlib.colors import ListedColormap
import numpy as np
from GenHisto import Histogram

# Process Higgs mass mean with or without cuts applied.
with_cuts = True
mass_title = 'Higgs Mass Mean'
mass_filename = 'higgs_mass_mean'
sigma_title = 'Higgs Mass Sigma'
sigma_filename = 'higgs_mass_sigma'
csv_filename = 'higgs_mass_fits'
if not with_cuts:
    mass_title += ' Without Cuts'
    mass_filename += '_no_cuts'
    sigma_title += ' Without Cuts'
    sigma_filename += '_no_cuts'
    csv_filename += '_no_cuts'
    print("[INFO] Creating heatmap of Higgs mass WITHOUT cuts.")
else: print("[INFO] Creating heatmap of Higgs mass WITH cuts.")

mass_filename += '.pdf'
sigma_filename += '.pdf'
csv_filename += '.csv'

# Import DataFrame of mass values.
df = pd.read_csv(csv_filename)

# This block of code causes bins with zero value to appear as white.
col_map = cm.get_cmap('rainbow', 256)
newcolors = col_map(np.linspace(0, 1, 256))
white = np.array([1, 1, 1, 1])    # White background (Red, Green, Blue, Alpha).
newcolors[0, :] = white    # Only change bins with 0 entries.
newcmp = ListedColormap(newcolors)

########################## MASS PLOT #####################################################

# This sorts the data into a pivot table where the mH values are the elements and the mY and mX values are the rows and columns, respectively.
df_pivot_m = df.pivot(index='m_Y',columns='m_X',values='m_H')

# Attempt to remove "0.0" labels from Seaborn's annot option.
df_temp = df["m_H"].tolist()
for i,obj in enumerate(df_temp):
    if obj == 0.0:
        df_temp[i] = ''
    else: df_temp[i] = round(obj,1) # Round mH values to first decimal place.

df_title_m = np.transpose(np.asarray([df_temp[i:i+19] for i in range(0,len(df_temp),19)]))

print('[INFO] Creating Higgs mean mass distribution heatmap.')
# Build a heatmap of the mean mH using Seaborn 
hm = sns.heatmap(df_pivot_m,cmap=newcmp,annot=df_title_m,annot_kws={"size": 7},fmt='', vmin=110, vmax=135)
hm.figure.tight_layout()
hm.invert_yaxis()
hm.set_title(mass_title, fontdict={'fontsize':16})
hm.figure.savefig(mass_filename)

# Clear figure or else the colorbar gets duplicated.
plt.clf()

########################## SIGMA PLOT ####################################################

print('[INFO] Creating Higgs sigma mass distribution heatmap.')
df_pivot_s = df.pivot(index='m_Y',columns='m_X',values='sigma')

# Attempt to remove "0.0" labels from Seaborn's annot option.
df_temp = df["sigma"].tolist()
for i,obj in enumerate(df_temp):
    if obj == 0.0:
        df_temp[i] = ''
    else: df_temp[i] = round(obj,1) # Round mH values to first decimal place.

df_title = np.transpose(np.asarray([df_temp[i:i+19] for i in range(0,len(df_temp),19)]))

hs = sns.heatmap(df_pivot_s,cmap=newcmp,annot=df_title,annot_kws={"size": 7},fmt='')
# hs.figure.tight_layout()
hs.invert_yaxis()
hs.set_title(sigma_title, fontdict={'fontsize':16})
hs.figure.savefig(sigma_filename)






plt.clf()






test_histo = Histogram(filesave='testfile.pdf', xdata=df_pivot_m, isDataFrame=True, label=df_title_m, cmap=newcmp)
test_histo.Make3DPlot()