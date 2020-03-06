###########First prepare just the signal
python MCBackgroundModel.py --config config/outputskim_2016.cfg --sample 0 --casename FullNtuples
python MCBackgroundModel.py --config config/outputskim_2017.cfg --sample 0 --casename FullNtuples
python MCBackgroundModel.py --config config/outputskim_2018.cfg --sample 0 --casename FullNtuples
###########Second prepare just the datadriven background
python MCBackgroundModel.py --config config/outputskim_2016.cfg --sample 2 --casename FullNtuples
python MCBackgroundModel.py --config config/outputskim_2017.cfg --sample 2 --casename FullNtuples
python MCBackgroundModel.py --config config/outputskim_2018.cfg --sample 2 --casename FullNtuples
