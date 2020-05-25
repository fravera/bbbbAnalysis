def custom_crab(config):
  print '>> Customising the crab config'
  config.Site.storageSite = 'T3_US_FNALLPC'
  config.JobType.allowUndistributedCMSSW = True