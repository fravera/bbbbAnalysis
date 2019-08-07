rm -r MyHistos
#Define folders and histograms
folders=(GGFcateg_SR_110_Histogram VBFcateg_SR_110_Histogram) #GGFcateg_CR_110_Histogram GGFcateg_CR_210_Histogram GGFcateg_SR_210_Histogram VBFcateg_CR_110_Histogram VBFcateg_CR_210_Histogram VBFcateg_SR_110_Histogram VBFcateg_SR_210_Histogram )
histograms=(nJet HH_m BDT1 BDT2 BDT3) # H1_pt H2_pt H1_eta H2_eta H1_bb_deltaR H2_bb_deltaR JJ_m j1j2_deltaEta)
#histograms=(HH_m BDT1 BDT2 costh_HH_b1_cm H1_bb_deltaPhi H1_eta H1_m_H2_m H1_pt_H2_pt h1j1_deltaR H1rand_m_H2rand_m H2_bb_deltaR H2_m h2j1_deltaR HH_b1_deepCSV HH_b1_pt HH_b2_eta HH_b3_deepCSV HH_b3_pt HH_b4_eta j1etaj2eta JJ_eta JJ_j1_eta JJ_j1_qgl JJ_j2_eta JJ_j2_qgl JJ_m_j1j2_deltaEta VBFEvent BDT1_BDT2 BDT3 costh_HH_b2_cm H1_bb_deltaR H1_m H1_pt h1h2_deltaEta h1j2_deltaR H2_bb_deltaPhi H2_eta H2_pt h2j2_deltaR HH_b1_eta HH_b2_deepCSV HH_b2_pt HH_b3_eta HH_b4_deepCSV HH_b4_pt j1j2_deltaEta JJ_j1_deepCSV JJ_j1_pt JJ_j2_deepCSV JJ_j2_pt JJ_m nJet)
models=(MODEL) #MODEL_99 MODEL_999 MODEL_9999 MODEL_GGF_3000_99 MODEL_GGF_3000_999 MODEL_GGF_3000_9999) 
#models=(MODEL MODEL_GGF_3000 MODEL_GGF_6000  MODEL_GGF_9000 MODEL_VBF_8000 MODEL_VBF_10000 MODEL_VBF_12000) 
#Start with 2016
cp -r Histos2016skim++ Histos2016
cd Histos2016
#Change the folder tag from Btag3 to Btag4
for folder in ${folders[@]}
do
       for model in ${models[@]}
       do
            rootrm -r outPlotter.root:${model}/Btag4_${folder}
            rootmv outPlotter.root:${model}/Btag3_${folder}/ outPlotter.root:${model}/Btag4_${folder}
            for histogram in ${histograms[@]}
            do
                rootmv outPlotter.root:${model}/Btag4_${folder}/${model}_Btag3_${folder}_${histogram}  outPlotter.root:${model}/Btag4_${folder}/${model}_Btag4_${folder}_${histogram} 
            done 
       	    echo "Done with the model $model and folder $folder in 2016"
       done
done  
cd ..

#Now start with 2017
cp -r Histos2017skim++ Histos2017
cd Histos2017
rootrm -r outPlotter.root:MODEL/Btag4*
#Change the folder tag from Btag3 to Btag4
for folder in ${folders[@]}
do
     rootmv outPlotter.root:MODEL/Btag3_$folder/ outPlotter.root:MODEL/Btag4_$folder/
     for histogram in ${histograms[@]}
     do
         rootmv outPlotter.root:MODEL/Btag4_$folder/MODEL_Btag3_${folder}_$histogram  outPlotter.root:MODEL/Btag4_$folder/MODEL_Btag4_${folder}_$histogram 
     done 
	   echo "Done with the folder $folder in 2017"
done 
cd ..

#Now start with 2018
cp -r Histos2018skim++ Histos2018
cd Histos2018
rootrm -r outPlotter.root:MODEL/Btag4*
#Change the folder tag from Btag3 to Btag4
for folder in ${folders[@]}
do
     rootmv outPlotter.root:MODEL/Btag3_$folder/ outPlotter.root:MODEL/Btag4_$folder/
     for histogram in ${histograms[@]}
     do
         rootmv outPlotter.root:MODEL/Btag4_$folder/MODEL_Btag3_${folder}_$histogram  outPlotter.root:MODEL/Btag4_$folder/MODEL_Btag4_${folder}_$histogram 
     done 
	   echo "Done with the folder $folder in 2018"
done 
cd ..

mkdir MyHistosNominal
mv Histos20* MyHistosNominal