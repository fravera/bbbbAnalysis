#! /usr/bin/env python

import seaborn as sb
import matplotlib.pyplot as plt
# from matplotlib import cm
# from matplotlib.colors import ListedColormap
import numpy as np

class Histogram():

    def __init__(self, filesave, xdata, ydata=None, zdata=None, isDataFrame=False, label=False, labelsize=7, comap=None, vmin=None, vmax=None, fmt=None, whiteBkgd=False):
        """
        This class takes in up to three distinct datasets. If the dataset is input as a Pandas DataFrame, it only requires one input, which we take to be xdata. The data will be handled differently depending on whether or not it is a dataframe so the isDataFrame boolean will be u  sed to dictate how to handle the data. The label input becomes the Seaborn annot input, which may be True, False, or a set of user-defined labels. 
        """

        self.filesave = filesave # Save the file using this name
        self.xdata = xdata 
        self.df_bool = isDataFrame # Is xdata a Pandas DataFrame?
        self.colmap = comap # Desired colormap
        self.label = label # Annotation label. (Set to True for numbers, False for empty, or customize with a user-generated label)
        self.labelsize = labelsize # Font size of label
        self.vmin = vmin # Colorbar min
        self.vmax = vmax # Colorbar max
        self.fmt = fmt # Format of label (Default is '.2g')
        # self.whitebkfg_bool = whiteBkgd

        self.MakeHeatmap()

    def MakeHeatmap(self):
        # if self.whitebkfg_bool:
        #     print("[HISTOGRAM] Changing background to white.")
        #     self.colormap = self.ChangeBkgdToWhite()

        if self.df_bool:
            # If the data is a Pandas DataFrame, use this block to make a heatmap.
            plt.clf()
            hm = sb.heatmap(self.xdata,cmap=self.colmap,annot=self.label,annot_kws={"size": self.labelsize}, vmin=self.vmin, vmax=self.vmax, fmt=self.fmt)
            hm.invert_yaxis()
            # plt.show()
            # hs.set_title(sigma_title, fontdict={'fontsize':16})
            hm.figure.savefig(self.filesave)
            print("[HISTOGRAM] File saved as {}".format(self.filesave))

    # def ChangeBkgdToWhite(self, n=256):
    #     print('[HISTOGRAM] ChangeBkgdToWhite has been called.')
    #     tmp_colors = cm.get_cmap(self.colormap, n)
    #     newcolors = tmp_colors(np.linspace(0, 1, n))
    #     white = np.array([1, 1, 1, 1])    # White background (Red, Green, Blue, Alpha).
    #     newcolors[0, :] = white    # Only change bins with 0 entries.
    #     newcmp = ListedColormap(newcolors)
    #     return newcmp
