#### make all the eff plots for the AN

# for year in 2016 2017 2018; do
# for year in 2016; do
for year in 2017 2018; do
    ## ggF signal
    python eff_vs_klambda.py $year
    for p in mHHcateg GGFvsVBF; do python frac_categ_GGF_vs_klambda.py $p $year; done

    ## VBF signal
    for c in kl CV C2V; do python eff_VBF_vs_coupling.py $c $year; done
    for c in kl CV C2V; do for p in VBFcateg GGFvsVBF GGFvsVBFonVBFevts; do python frac_categ_VBF_vs_coupling.py $c $p $year; done ; done
done