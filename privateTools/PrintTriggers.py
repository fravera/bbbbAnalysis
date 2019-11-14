# import ROOT in batch mode
import sys
oldargv = sys.argv[:]
sys.argv = [ '-b-' ]
import ROOT
ROOT.gROOT.SetBatch(True)
sys.argv = oldargv

# load FWLite C++ libraries
ROOT.gSystem.Load("libFWCoreFWLite.so");
ROOT.gSystem.Load("libDataFormatsFWLite.so");
ROOT.AutoLibraryLoader.enable()

# load FWlite python libraries
from DataFormats.FWLite import Handle, Events

triggerBits, triggerBitLabel = Handle("edm::TriggerResults"), ("TriggerResults","","HLT")
triggerObjects, triggerObjectLabel  = Handle("std::vector<pat::TriggerObjectStandAlone>"), "slimmedPatTrigger"
# triggerObjects, triggerObjectLabel  = Handle("std::vector<pat::TriggerObjectStandAlone>"), "selectedPatTrigger"
# triggerObjects, triggerObjectLabel  = Handle("std::vector<pat::TriggerObjectStandAlone>"), "selectedPatTrigger"
triggerPrescales, triggerPrescaleLabel  = Handle("pat::PackedTriggerPrescales"), "patTrigger"

# open file (you can use 'edmFileUtil -d /store/whatever.root' to get the physical file name)
# events = Events("root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/GluGluToBulkGravitonToHHTo4B_M-300_narrow_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/60000/0C9C47A4-ADC1-E611-8104-001E675A6AB3.root")
# events = Events("root://cms-xrd-global.cern.ch//store/data/Run2016E/BTagCSV/MINIAOD/17Jul2018-v1/20000/BCECD4A8-CF92-E811-8491-F452141014E0.root")
# events = Events("root://cms-xrd-global.cern.ch//store/mc/RunIISummer16MiniAODv2/GluGluToBulkGravitonToHHTo4B_M-750_narrow_13TeV-madgraph/MINIAODSIM/PUMoriond17_80X_mcRun2_asymptotic_2016_TrancheIV_v6-v1/120000/F44EF4C4-80CC-E611-B027-002590D9D8C0.root")
events = Events("root://cms-xrd-global.cern.ch//store/data/Run2016B/BTagCSV/MINIAOD/17Jul2018_ver2-v1/80000/00F7ADBB-3C8C-E811-B755-008CFAF71768.root ")
# events = Events("root://cmseos.fnal.gov//store/data/Run2016B/SingleMuon/MINIAOD/17Jul2018_ver2-v1/90000/FEADEB19-1D92-E811-BAFA-0025905C54D8.root")
# events = Events("root://cms-xrd-global.cern.ch//store/data/Run2016B/SingleMuon/MINIAOD/17Jul2018_ver2-v1/90000/FEADEB19-1D92-E811-BAFA-0025905C54D8.root")
numberOfEventsTriggered = 0
numberOfProcessedEvent = 0
for iev,event in enumerate(events):
    if numberOfProcessedEvent>=10 : break
    event.getByLabel(triggerBitLabel, triggerBits)
    event.getByLabel(triggerObjectLabel, triggerObjects)
    event.getByLabel(triggerPrescaleLabel, triggerPrescales)

    #if event.eventAuxiliary().event() != 1020767395:
    #    continue

    #print "\nEvent %d: run %6d, lumi %4d, event %12d" % (iev,event.eventAuxiliary().run(), event.eventAuxiliary().luminosityBlock(),event.eventAuxiliary().event())
    # print "\n === TRIGGER PATHS ==="
    names = event.object().triggerNames(triggerBits.product())
    triggerName = "HLT_DoubleJet90_Double30_TripleBTagCSV_p087_v1"
    # triggerName = "HLT_IsoMu20_v3"
    
    # found = False
    # for i in xrange(triggerBits.product().size()):
    #     # print "Trigger ", names.triggerName(i), ", prescale ", triggerPrescales.product().getPrescaleForIndex(i), ": ", ("PASS" if triggerBits.product().accept(i) else "fail (or not run)") 
    #     if names.triggerName(i) == triggerName:
    #         found = triggerBits.product().accept(i)

    # if not found:
    #     # print "impossible"
    #     continue

    skip = False

    for i in xrange(triggerBits.product().size()):
        if names.triggerName(i) == "HLT_DoubleJet90_Double30_TripleBTagCSV_p087_v1" or names.triggerName(i) == "HLT_QuadJet45_TripleBTagCSV_p087_v1" :
            skip = triggerBits.product().accept(i)

    if skip:
        continue

    numberOfProcessedEvent = numberOfProcessedEvent +1

    numberOfObjectsDict = {
        "hltBTagCaloCSVp087Triple" : 0 ,
        "hltDoubleCentralJet90" : 0 ,
        "hltDoublePFCentralJetLooseID90" : 0 ,
        "hltL1sTripleJetVBFIorHTTIorDoubleJetCIorSingleJet" : 0 ,
        "hltQuadCentralJet30" : 0 ,
        "hltQuadPFCentralJetLooseID30" : 0
    }


    numberOfObjectsDictMin = {
        "hltBTagCaloCSVp087Triple" : 3 ,
        "hltDoubleCentralJet90" : 2 ,
        "hltDoublePFCentralJetLooseID90" : 2 ,
        "hltL1sTripleJetVBFIorHTTIorDoubleJetCIorSingleJet" : 1 ,
        "hltQuadCentralJet30" : 4 ,
        "hltQuadPFCentralJetLooseID30" : 4
    }



    # numberOfObjectsDict = {
    #     "hltBTagCaloCSVp087Triple" : 0 ,
    #     "hltDoubleCentralJet90" : 0 ,
    #     "hltDoublePFCentralJetLooseID90" : 0 ,
    #     "hltL1sTripleJetVBFIorHTTIorDoubleJetCIorSingleJet" : 0 ,
    #     "hltQuadCentralJet30" : 0 ,
    #     "hltQuadPFCentralJetLooseID30" : 0
    # }

    #umberOfObjectsDict = {
    #    "hltBTagCaloCSVp087Triple" : 0 ,
    #    "hltL1sQuadJetC50IorQuadJetC60IorHTT280IorHTT300IorHTT320IorTripleJet846848VBFIorTripleJet887256VBFIorTripleJet927664VBF" : 0 ,
    #    "hltQuadCentralJet45" : 0 ,
    #    "hltQuadPFCentralJetLooseID45" : 0
    #}

    #numberOfObjectsDictMin = {
    #    "hltBTagCaloCSVp087Triple" : 3 ,
    #    "hltL1sQuadJetC50IorQuadJetC60IorHTT280IorHTT300IorHTT320IorTripleJet846848VBFIorTripleJet887256VBFIorTripleJet927664VBF" : 1 ,
    #    "hltQuadCentralJet45" : 4 ,
    #    "hltQuadPFCentralJetLooseID45" : 4
    #}

    # numberOfObjectsDict = {
    #     "hltL1fL1sMu22L1Filtered0" : 0,
    #     "hltL1sSingleMu22" : 0 ,
    #     "hltL2fL1sMu22L1f0L2Filtered10Q" : 0 ,
    #     "hltL3crIsoL1sMu22L1f0L2f10QL3f24QL3trkIsoFiltered0p09" : 0 ,
    #     "hltL3fL1sMu22L1f0L2f10QL3Filtered24Q" : 0
    # }

    # numberOfObjectsDictMin = {
    #     "hltL1fL1sMu22L1Filtered0" : 1,
    #     "hltL1sSingleMu22" : 1 ,
    #     "hltL2fL1sMu22L1f0L2Filtered10Q" : 1 ,
    #     "hltL3crIsoL1sMu22L1f0L2f10QL3f24QL3trkIsoFiltered0p09" : 1 ,
    #     "hltL3fL1sMu22L1f0L2f10QL3Filtered24Q" : 1
    # }


    # numberOfObjectsDict = {
    #     "hltL1fL1sMu22L1Filtered0" : 0,
    #     "hltL1sSingleMu22" : 0 ,
    #     "hltL2fL1sMu22L1f0L2Filtered10Q" : 0 ,
    #     "hltL3crIsoL1sMu22L1f0L2f10QL3f24QL3trkIsoFiltered0p09" : 0 ,
    #     "hltL3fL1sMu22L1f0L2f10QL3Filtered24Q" : 0
    # }

    # numberOfObjectsDictMin = {
    #     "hltL1fL1sMu22L1Filtered0" : 1,
    #     "hltL1sSingleMu22" : 1 ,
    #     "hltL2fL1sMu22L1f0L2Filtered10Q" : 1 ,
    #     "hltL3crIsoL1sMu22L1f0L2f10QL3f24QL3trkIsoFiltered0p09" : 1 ,
    #     "hltL3fL1sMu22L1f0L2f10QL3Filtered24Q" : 1
    # }

    # numberOfObjectsDictMin = {
    #     "hltL1fL1sMu18L1Filtered0" : 0,
    #     "hltL1sSingleMu18" : 0,
    #     "hltL2fL1sMu18L1f0L2Filtered10Q" : 0,
    #     "hltL3crIsoL1sMu18L1f0L2f10QL3f20QL3trkIsoFiltered0p09" : 0,
    #     "hltL3fL1sMu18L1f0L2f10QL3Filtered20Q" : 0
    # }

    # numberOfObjectsDict = {
    #     "hltL1fL1sMu18L1Filtered0" : 1,
    #     "hltL1sSingleMu18" : 1,
    #     "hltL2fL1sMu18L1f0L2Filtered10Q" : 1,
    #     "hltL3crIsoL1sMu18L1f0L2f10QL3f20QL3trkIsoFiltered0p09" : 1,
    #     "hltL3fL1sMu18L1f0L2f10QL3Filtered20Q" : 1
    # }

    print "trigger!!!!!"

    for i in xrange(triggerBits.product().size()):
        # print "Trigger ", names.triggerName(i), ", prescale ", triggerPrescales.product().getPrescaleForIndex(i), ": ", ("PASS" if triggerBits.product().accept(i) else "fail (or not run)") 
        if triggerBits.product().accept(i):
            print names.triggerName(i)

    numberOfEventsTriggered += 1

    for j,to in enumerate(triggerObjects.product()):
        # to.unpackPathNames(names);
        to.unpackFilterLabels(event.object(),triggerBits.product());

        # pathslast = set(to.pathNames())
        # if triggerName not in pathslast: continue
        # print "         filters: ", ", ".join([str(f) for f in to.filterLabels()])

        objectUsed = True
        for filter in to.filterLabels():
            if filter in numberOfObjectsDict:
                numberOfObjectsDict[filter] += 1
                objectUsed = True
                #if filter == "hltL1sTripleJetVBFIorHTTIorDoubleJetCIorSingleJet":
                #    objectUsed = False
        
        if objectUsed:
            doPrintFilters = False
            for id in to.filterIds():
               if id == 0 or id == 85 or id == 86 or id == -99:
                   doPrintFilters = True
            
            if doPrintFilters:
                print "Trigger object pt %6.2f eta %+5.3f phi %+5.3f  " % (to.pt(),to.eta(),to.phi())
                print "         collection: ", to.collection()
                print "         type ids: ", ", ".join([str(f) for f in to.filterIds()])
                print "         filters: ",
                for f in to.filterLabels():
                    # if f in numberOfObjectsDict:
                        print f, ", ",   
                print 
            
        # # pathslast = set(to.pathNames())
        # print "         paths:   ", ", ".join([("%s*" if f in pathslast else "%s")%f for f in to.pathNames()]) 
    
    # for value in numberOfObjectsDict :
    #     if numberOfObjectsDict[value] < numberOfObjectsDictMin[value] :
    #         print "\nPorca merda"


