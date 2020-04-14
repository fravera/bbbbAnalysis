import numpy as np
import sys, fileinput

mx = np.load("mX_values.npz")
my = np.load("mY_values.npz")

mx = np.append(mx['arr_0'],2000)
my = np.append(my['arr_0'],1600)
my = np.append(my,1800)

directory = 'plotterListFiles/2016Resonant_NMSSM_XYH_bbbb/'

def replaceLines(f, arg1, arg2):
    for line in f:
        line = line.replace(arg1, arg2)
        sys.stdout.write(line)

for mX in mx:
    for mY in my:
        filename = 'FileList_NMSSM_XYH_bbbb_MX_{}_MY_{}_Fast.txt'.format(str(mX),str(mY)) 
        file_path = directory + filename 
        print("[INFO] Opening file {}".format(filename))
        try: 
            f = fileinput.input(file_path, inplace=True)
            # replaceLines(f,"fravera","srosenzw")
            replaceLines(f,"fullSim_v1","fastSim_v1")
        except: continue
        
