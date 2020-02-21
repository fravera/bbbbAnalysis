import subprocess

folder = '/store/user/fravera/bbbb_ntuples/NMSSM_XYH_bbbb_basicSelection_Plus_v15/SKIM_BTagCSV_Data/output/'
eosPath = 'root://cmseos.fnal.gov'
out = subprocess.Popen(['eos', eosPath, 'ls', folder], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
# out = subprocess.Popen(['xrdfsls', '/store/user/fravera/'], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
stdout,stderr = out.communicate()

listOfFiles = []
for fileName in stdout.split():
    if ".root" in fileName:
        listOfFiles.append(eosPath + "/" + folder + "/" + fileName)

for file in listOfFiles:
    print file
