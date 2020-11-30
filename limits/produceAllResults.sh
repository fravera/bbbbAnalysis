TAG="fullSubmission_v17"

g++  -std=c++17 -I `root-config --incdir`  -o PlotLimitsFromCondor PlotLimitsFromCondor.cc `root-config --libs` -O3
g++  -std=c++17 -I `root-config --incdir`  -o Plot2DLimitMap       Plot2DLimitMap.C        `root-config --libs` -O3

# ./PlotLimitsFromCondor $TAG impacts
python CompareHHAnalysisAll.py --input Limits_$TAG.root --syst
python CompareHHAnalysisAll.py --input Limits_$TAG.root --syst
./Plot2DLimitMap Limits_$TAG.root

python MeasureSystematicEffect.py --input Limits_$TAG.root --impacts

python OverlapPlots.py  --input Limits_$TAG.root --systematics
python OverlapTheory.py --input Limits_$TAG.root --systematics
python PlotHHLimit.py --input Limits_$TAG.root --systematics
