from  ConfigParser import *
import ast

class ConfigurationReader:
    def __init__(self, configFileName): 
        ###########Read Config file
        print "[INFO] Reading skim configuration file . . ."
        cfgparser = ConfigParser()
        cfgparser.read('%s'%configFileName)
        ##########Get skim variables
        print "[INFO] Getting configuration parameters . . ."
        self.backgroundWeightName         = ast.literal_eval(cfgparser.get("configuration","backgroundWeightName"))
        print "    -The backgroundWeightName:"
        print "      *",self.backgroundWeightName[0]
        self.modelArguments               = ast.literal_eval(cfgparser.get("configuration","modelArguments"))
        print "    -The random seed:"
        print "      *",self.modelArguments 
        self.minpt                        = ast.literal_eval(cfgparser.get("configuration","minbjetpt"))
        print "    -The min b-jet pt:"
        print "      *",self.minpt 
        self.minRegressedPt               = ast.literal_eval(cfgparser.get("configuration","minbjetregressedpt"))
        print "    -The min b-jet regressed pt"
        print "      *",self.minRegressedPt 
        self.minEta                       = ast.literal_eval(cfgparser.get("configuration","minbjeteta"))
        print "    -The min b-jet eta"
        print "      *",self.minEta 
        self.maxEta                       = ast.literal_eval(cfgparser.get("configuration","maxbjeteta"))
        print "    -The max b-jet eta"
        print "      *",self.maxEta 
        self.preSelection                 = ast.literal_eval(cfgparser.get("configuration","preSelection"))
        print "    -The preSelection:"
        print "      *",self.preSelection
        self.controlRegionSelection       = ast.literal_eval(cfgparser.get("configuration","controlRegionSelection"))
        print "    -The controlRegionSelection:"
        print "      *",self.controlRegionSelection
        self.skimFolder                   = ast.literal_eval(cfgparser.get("configuration","skimFolder"))
        print "    -The skimFolder:"
        print "      *",self.skimFolder
        self.variables                    = ast.literal_eval(cfgparser.get("configuration","variables"))
        print "    -The list of variables:"
        for x in range(len(self.variables)):
            print "      *",self.variables[x]
        self.trainingVariables            = ast.literal_eval(cfgparser.get("configuration","trainingVariables"))
        print "    -The list of training variables:"
        for x in range(len(self.trainingVariables)):
            print "      *",self.trainingVariables[x]
        self.bTagSelection                = ast.literal_eval(cfgparser.get("configuration","bTagSelection"))
        print "    -The bTagSelection:"
        print "      *",self.bTagSelection
        self.antiBTagSelection            = ast.literal_eval(cfgparser.get("configuration","antiBTagSelection"))
        print "    -The antiBTagSelection:"
        print "      *",self.antiBTagSelection
        self.threadNumber       = ast.literal_eval(cfgparser.get("configuration","threadNumber"))
        print "    -The thread number for applying weights:"
        print "      *",self.threadNumber

