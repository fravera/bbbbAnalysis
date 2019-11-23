mkdir BKGSTUDIES2016
rm BKGSTUDIES2016/*.root
hadd BKGSTUDIES2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_8000.root    `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/BKGSTUDIES2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_8000/output/ | grep 'bbbbNtuple_0.root'`
hadd BKGSTUDIES2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_10000.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/BKGSTUDIES2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_10000/output/ | grep 'bbbbNtuple_0.root'`
hadd BKGSTUDIES2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_12000.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/BKGSTUDIES2016/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_12000/output/ | grep 'bbbbNtuple_0.root'`
hadd BKGSTUDIES2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_3000.root        `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/BKGSTUDIES2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_3000/output/ | grep 'bbbbNtuple_0.root'`
hadd BKGSTUDIES2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_6000.root        `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/BKGSTUDIES2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_6000/output/ | grep 'bbbbNtuple_0.root'`
hadd BKGSTUDIES2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_9000.root        `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/BKGSTUDIES2016/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_9000/output/ | grep 'bbbbNtuple_0.root'`
mkdir BKGSTUDIES2017
rm BKGSTUDIES2017/*.root
hadd BKGSTUDIES2017/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_8000.root     `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/BKGSTUDIES2017/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_8000/output/ | grep 'bbbbNtuple_0.root'`
hadd BKGSTUDIES2017/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_10000.root    `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/BKGSTUDIES2017/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_10000/output/ | grep 'bbbbNtuple_0.root'`
hadd BKGSTUDIES2017/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_12000.root    `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/BKGSTUDIES2017/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph_12000/output/ | grep 'bbbbNtuple_0.root'`
hadd BKGSTUDIES2017/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_3000.root        `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/BKGSTUDIES2017/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_3000/output/ | grep 'bbbbNtuple_0.root'`
hadd BKGSTUDIES2017/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_6000.root        `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/BKGSTUDIES2017/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_6000/output/ | grep 'bbbbNtuple_0.root'`
hadd BKGSTUDIES2017/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_9000.root        `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/BKGSTUDIES2017/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_9000/output/ | grep 'bbbbNtuple_0.root'`