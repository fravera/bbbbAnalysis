
class ReweightModelAndTransferFactor:
  def __init__(self, reweightMethod, transferFactor, normalization):
    self.reweightMethod = reweightMethod
    self.transferFactor = transferFactor
    self.normalization  = normalization
