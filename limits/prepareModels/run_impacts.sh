WSPACE=datacard2018_comb_GGFcateg1GGFcateg2.root
FOLDER=physicsmodel2018
# WSPACE=datacard2018_GGFcateg2.root
EXPECT_SIGNAL=10

HERE=`pwd`
echo "... working in", $FOLDER
cd ${FOLDER}

combineTool.py -M Impacts -d ${WSPACE}  --doInitialFit --robustFit 1 --freezeParameters r_gghh,r_qqhh,CV,C2V,kl,kt --setParameterRanges r=-20,20 --redefineSignalPOIs r -m 125 --expectSignal ${EXPECT_SIGNAL} -t -1
combineTool.py -M Impacts -d ${WSPACE}  --doFits       --robustFit 1 --freezeParameters r_gghh,r_qqhh,CV,C2V,kl,kt --setParameterRanges r=-20,20 --redefineSignalPOIs r -m 125 --expectSignal ${EXPECT_SIGNAL} -t -1 --parallel 4
combineTool.py -M Impacts -d ${WSPACE}  -o impacts_sig_inj${EXPECT_SIGNAL}.json              --freezeParameters r_gghh,r_qqhh,CV,C2V,kl,kt --setParameterRanges r=-20,20 --redefineSignalPOIs r -m 125
plotImpacts.py -i impacts_sig_inj${EXPECT_SIGNAL}.json -o impacts_sig_inj${EXPECT_SIGNAL}

cd $HERE