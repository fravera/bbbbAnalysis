OUTDIR="root://cmseos.fnal.gov//store/user/{0}/bbbb_ntuples_CMSDAS_trigger/"


TAG="ntuples_20Jan2020_MuonCut30GeV_v23_matched"
python scripts/submitSkimOnTier3.py --executable bin/skim_trigger_for_das.exe --outputDir ${OUTDIR} --tag=$TAG --input inputFiles/CMSDAS/Samples2016/SingleMuon_Data_forTrigger.txt                  --cfg config/skim_CMSDAS_2016Trigger.cfg --njobs 200 --match --is-data
python scripts/submitSkimOnTier3.py --executable bin/skim_trigger_for_das.exe --outputDir ${OUTDIR} --tag=$TAG --input inputFiles/CMSDAS/Samples2016/MC_TT_TuneCUETP8M2T4_13TeV_forTrigger.txt       --cfg config/skim_CMSDAS_2016Trigger.cfg --njobs 100 --match --puWeight weights/2016/MC_TT_TuneCUETP8M2T4_13TeV_forTrigger_PUweights.root       --no-tar --no-xrdcp-tar
python scripts/submitSkimOnTier3.py --executable bin/skim_trigger_for_das.exe --outputDir ${OUTDIR} --tag=$TAG --input inputFiles/CMSDAS/Samples2016/MC_WJetsToLNu_TuneCUETP8M1_13TeV_forTrigger.txt --cfg config/skim_CMSDAS_2016Trigger.cfg --njobs 50  --match --puWeight weights/2016/MC_WJetsToLNu_TuneCUETP8M1_13TeV_forTrigger_PUweights.root --no-tar --no-xrdcp-tar
python scripts/submitSkimOnTier3.py --executable bin/skim_trigger_for_das.exe --outputDir ${OUTDIR} --tag=$TAG --input inputFiles/CMSDAS/Samples2016/NMSSM_XYHbbbb_privateProduction_forTrigger.txt  --cfg config/skim_CMSDAS_2016Trigger.cfg --njobs 20  --match --puWeight weights/2016/NMSSM_XYHbbbb_privateProduction_forTrigger_PUweights.root  --no-tar --no-xrdcp-tar --is-signal



TAG="ntuples_20Jan2020_MuonCut30GeV_v23_unMatched"
python scripts/submitSkimOnTier3.py --executable bin/skim_trigger_for_das.exe --outputDir ${OUTDIR} --tag=$TAG --input inputFiles/CMSDAS/Samples2016/SingleMuon_Data_forTrigger.txt                  --cfg config/skim_CMSDAS_2016Trigger.cfg --njobs 200         --is-data
python scripts/submitSkimOnTier3.py --executable bin/skim_trigger_for_das.exe --outputDir ${OUTDIR} --tag=$TAG --input inputFiles/CMSDAS/Samples2016/MC_TT_TuneCUETP8M2T4_13TeV_forTrigger.txt       --cfg config/skim_CMSDAS_2016Trigger.cfg --njobs 100         --puWeight weights/2016/MC_TT_TuneCUETP8M2T4_13TeV_forTrigger_PUweights.root       --no-tar --no-xrdcp-tar
python scripts/submitSkimOnTier3.py --executable bin/skim_trigger_for_das.exe --outputDir ${OUTDIR} --tag=$TAG --input inputFiles/CMSDAS/Samples2016/MC_WJetsToLNu_TuneCUETP8M1_13TeV_forTrigger.txt --cfg config/skim_CMSDAS_2016Trigger.cfg --njobs 50          --puWeight weights/2016/MC_WJetsToLNu_TuneCUETP8M1_13TeV_forTrigger_PUweights.root --no-tar --no-xrdcp-tar
python scripts/submitSkimOnTier3.py --executable bin/skim_trigger_for_das.exe --outputDir ${OUTDIR} --tag=$TAG --input inputFiles/CMSDAS/Samples2016/NMSSM_XYHbbbb_privateProduction_forTrigger.txt  --cfg config/skim_CMSDAS_2016Trigger.cfg --njobs 20          --puWeight weights/2016/NMSSM_XYHbbbb_privateProduction_forTrigger_PUweights.root  --no-tar --no-xrdcp-tar --is-signal


