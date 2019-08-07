
##HADD DATA AND SM MC SAMPLES
selections=("case2")

for selection in ${selections[@]}
do
     mkdir 2016$selection
     rm 2016$selection/*.root
     hadd 2016$selection/SKIM_Data.root                                                    `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016$selection/SKIM_Data_BTagCSV/output/ | grep '\.root'`
     hadd 2016$selection/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.root                `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016$selection/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph/output/ | grep '\.root'`
     hadd 2016$selection/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph.root                   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016$selection/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph/output/ | grep '\.root'`
     hadd 2016$selection/SKIM_QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016$selection/SKIM_QCD_HT200to300_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2016$selection/SKIM_QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016$selection/SKIM_QCD_HT300to500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2016$selection/SKIM_QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016$selection/SKIM_QCD_HT500to700_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2016$selection/SKIM_QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016$selection/SKIM_QCD_HT700to1000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2016$selection/SKIM_QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016$selection/SKIM_QCD_HT1000to1500_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2016$selection/SKIM_QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016$selection/SKIM_QCD_HT1500to2000_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2016$selection/SKIM_QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016$selection/SKIM_QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2016$selection/SKIM_QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8.root  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016$selection/SKIM_QCD_HT2000toInf_TuneCUETP8M1_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2016$selection/SKIM_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8.root                  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016$selection/SKIM_TT_TuneCUETP8M2T4_13TeV-powheg-pythia8/output/ | grep '\.root'`
     mkdir 2017$selection
     rm 2017$selection/*.root
     hadd 2017$selection/SKIM_Data.root                                                    `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017$selection/SKIM_Data_BTagCSV/output/ | grep '\.root'`
     hadd 2017$selection/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph.root                `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017$selection/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_1_13TeV-madgraph/output/ | grep '\.root'`
     hadd 2017$selection/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg.root      `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017$selection/SKIM_GluGluToHHTo4B_node_SM_13TeV-madgraph_correctedcfg/output/ | grep '\.root'`
     hadd 2017$selection/SKIM_QCD_HT200to300_TuneCP5_13TeV-madgraph-pythia8.root           `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017$selection/SKIM_QCD_HT200to300_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
     hadd 2017$selection/SKIM_QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8.root           `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017$selection/SKIM_QCD_HT300to500_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
     hadd 2017$selection/SKIM_QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8.root           `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017$selection/SKIM_QCD_HT500to700_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
     hadd 2017$selection/SKIM_QCD_HT700to1000_TuneCP5_13TeV-madgraph-pythia8.root          `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017$selection/SKIM_QCD_HT700to1000_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
     hadd 2017$selection/SKIM_QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8.root         `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017$selection/SKIM_QCD_HT1000to1500_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
     hadd 2017$selection/SKIM_QCD_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8.root         `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017$selection/SKIM_QCD_HT1500to2000_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
     hadd 2017$selection/SKIM_QCD_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8.root          `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017$selection/SKIM_QCD_HT2000toInf_TuneCP5_13TeV-madgraph-pythia8/output/ | grep '\.root'`
     hadd 2017$selection/SKIM_TTToHadronic_TuneCP5_13TeV-powheg-pythia8.root               `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017$selection/SKIM_TTToHadronic_TuneCP5_13TeV-powheg-pythia8/output/ | grep '\.root'`
     hadd 2017$selection/SKIM_TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8.root           `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017$selection/SKIM_TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/output/ | grep '\.root'`
     hadd 2017$selection/SKIM_TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8.root                  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017$selection/SKIM_TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/output/ | grep '\.root'`
     mkdir 2018$selection
     rm 2018$selection/*.root
     hadd 2018$selection/SKIM_Data.root                                               `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2018$selection/SKIM_Data_BTagCSV/output/ | grep '\.root'`
     hadd 2018$selection/SKIM_QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2018$selection/SKIM_QCD_HT200to300_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2018$selection/SKIM_QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2018$selection/SKIM_QCD_HT300to500_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2018$selection/SKIM_QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2018$selection/SKIM_QCD_HT500to700_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2018$selection/SKIM_QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8.root  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2018$selection/SKIM_QCD_HT700to1000_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2018$selection/SKIM_QCD_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8.root `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2018$selection/SKIM_QCD_HT1000to1500_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2018$selection/SKIM_QCD_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8.root `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2018$selection/SKIM_QCD_HT1500to2000_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2018$selection/SKIM_QCD_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8.root  `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2018$selection/SKIM_QCD_HT2000toInf_TuneCP5_13TeV-madgraphMLM-pythia8/output/ | grep '\.root'`
     hadd 2018$selection/SKIM_TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8.root             `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2018$selection/SKIM_TTTo2L2Nu_TuneCP5_13TeV-powheg-pythia8/output/ | grep '\.root'`
     hadd 2018$selection/SKIM_TTToHadronic_TuneCP5_13TeV-powheg-pythia8.root          `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2018$selection/SKIM_TTToHadronic_TuneCP5_13TeV-powheg-pythia8/output/ | grep '\.root'`
     hadd 2018$selection/SKIM_TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8.root      `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2018$selection/SKIM_TTToSemiLeptonic_TuneCP5_13TeV-powheg-pythia8/output/ | grep '\.root'`
done 


##HADD: VBF-HH Coupling variations
hadd 2016case2/SKIM_VBFHHTo4B_CV_0_5_C2V_1_C3_1_13TeV-madgraph.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016case2/SKIM_VBFHHTo4B_CV_0_5_C2V_1_C3_1_13TeV-madgraph//output/ | grep '\.root'`
hadd 2016case2/SKIM_VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016case2/SKIM_VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph//output/ | grep '\.root'`
hadd 2016case2/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph.root     `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016case2/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph//output/ | grep '\.root'`
hadd 2016case2/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph.root     `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016case2/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph//output/ | grep '\.root'`
hadd 2016case2/SKIM_VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph.root     `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016case2/SKIM_VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph/output/ | grep '\.root'`
hadd 2017case2/SKIM_VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph.root     `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017case2/SKIM_VBFHHTo4B_CV_1_C2V_2_C3_1_13TeV-madgraph/output/ | grep '\.root'`
hadd 2017case2/SKIM_VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017case2/SKIM_VBFHHTo4B_CV_1_5_C2V_1_C3_1_13TeV-madgraph//output/ | grep '\.root'`
hadd 2017case2/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph.root     `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017case2/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_0_13TeV-madgraph//output/ | grep '\.root'`
hadd 2017case2/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph.root     `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017case2/SKIM_VBFHHTo4B_CV_1_C2V_1_C3_2_13TeV-madgraph//output/ | grep '\.root'`


##HADD: klambda variations
for kl in `seq -f %g -10 1 10`; do
     klname=${kl/./d}
     if [[ $klname == *-* ]]; then
         klname=${klname/-/m_};
     else
         klname=p_${klname}
     fi

     hadd 2016case2/SKIM_GGHH4B_rew_kl_${klname}.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2016case2/SKIM_GGHH4B_rew_kl_${klname}/output/ | grep '\.root'`
done

for kl in `seq -f %g -10 1 10`; do
     klname=${kl/./d}
     if [[ $klname == *-* ]]; then
         klname=${klname/-/m_};
     else
         klname=p_${klname}
     fi

     hadd 2017case2/SKIM_GGHH4B_rew_kl_${klname}.root   `xrdfsls -u /eos/uscms/store/user/guerrero/bbbb_ntuples/2017case2/SKIM_GGHH4B_rew_kl_${klname}/output/ | grep '\.root'`
done