###################################
# Author : L. Cadamuro (UF)
# Date   : 11/03/2020
# Brief  : code that implements the HH model in combine
# structure of the code :
# xxHHSample  -> defines the interface to the user, that will pass the xs and the coupling setups
# xxHHFormula -> implements the matrix component representation, that calculates the symbolic scalings
# HHModel     -> implements the interfaces to combine
###################################


from sympy import *
from numpy import matrix
from numpy import linalg
from sympy import Matrix


class VBFHHSample:
    def __init__(self, val_CV, val_C2V, val_kl, val_xs, label):
        self.val_CV  = val_CV
        self.val_C2V = val_C2V
        self.val_kl  = val_kl
        self.val_xs  = val_xs
        self.label   = label

####################

class GGFHHSample:
    def __init__(self, val_kl, val_kt, val_xs, label):
        self.val_kl  = val_kl
        self.val_kt  = val_kt
        self.val_xs  = val_xs
        self.label   = label

####################

class VBFHHFormula:
    def __init__(self, sample_list):
        self.sample_list = sample_list
        self.build_matrix()
        self.calculatecoeffients()        

    def build_matrix(self):    
        """ create the matrix M in this object """

        if len(self.sample_list) != 6:
            print "[ERROR] : expecting 6 samples in input"
            raise RuntimeError("malformed vbf input sample list")
        M_tofill = [
            [None,None,None,None,None,None],
            [None,None,None,None,None,None],
            [None,None,None,None,None,None],
            [None,None,None,None,None,None],
            [None,None,None,None,None,None],
            [None,None,None,None,None,None]
        ]

        for isample, sample in enumerate(self.sample_list):
            # print isample, " CV, C2V, kl = ", sample.val_CV, sample.val_C2V, sample.val_kl
            
            ## implement the 6 scalings
            M_tofill[isample][0] = sample.val_CV**2 * sample.val_kl**2
            M_tofill[isample][1] = sample.val_CV**4
            M_tofill[isample][2] = sample.val_C2V**2
            M_tofill[isample][3] = sample.val_CV**3 * sample.val_kl
            M_tofill[isample][4] = sample.val_CV    * sample.val_C2V    * sample.val_kl
            M_tofill[isample][5] = sample.val_CV**2 * sample.val_C2V
        
        # print M_tofill
        self.M = Matrix(M_tofill)

    def calculatecoeffients(self):
        """ create the function sigma and the six coefficients in this object """

        try: self.M
        except AttributeError: self.build_matrix()
        
        ##############################################    
        CV, C2V, kl, a, b, c, iab, iac, ibc, s1, s2, s3, s4, s5, s6 = symbols('CV C2V kl a b c iab iac ibc s1 s2 s3 s4 s5 s6')    
        ### the vector of couplings
        c = Matrix([
            [CV**2 * kl**2] ,
            [CV**4]         ,
            [C2V**2]        ,
            [CV**3 * kl]    ,
            [CV * C2V * kl] ,
            [CV**2 * C2V]   
        ])
        ### the vector of components
        v = Matrix([
            [a]   ,
            [b]   ,
            [c]   ,
            [iab] ,
            [iac] ,
            [ibc]  
        ])    
        ### the vector of samples (i.e. cross sections)
        s = Matrix([
            [s1] ,
            [s2] ,
            [s3] ,
            [s4] ,
            [s5] ,
            [s6] 
        ])    
        ####    
        Minv   = self.M.inv()
        self.coeffs = c.transpose() * Minv # coeffs * s is the sigma, accessing per component gives each sample scaling
        self.sigma  = self.coeffs*s

####################

class GGFHHFormula:
    def __init__(self, sample_list):
        self.sample_list = sample_list
        self.build_matrix()
        self.calculatecoeffients()        

    def build_matrix(self):    
        """ create the matrix M in this object """

        if len(self.sample_list) != 3:
            print "[ERROR] : expecting 3 samples in input"
            raise RuntimeError("malformed ggf input sample list")
        M_tofill = [
            [None,None,None],
            [None,None,None],
            [None,None,None],
        ]

        for isample, sample in enumerate(self.sample_list):
            
            ## implement the 3 scalings - box, triangle, interf
            M_tofill[isample][0] = sample.val_kt**4
            M_tofill[isample][1] = sample.val_kt**2 * sample.val_kl**2
            M_tofill[isample][2] = sample.val_kt**3 * sample.val_kl

        # print M_tofill
        self.M = Matrix(M_tofill)

    def calculatecoeffients(self):
        """ create the function sigma and the six coefficients in this object """

        try: self.M
        except AttributeError: self.build_matrix()

        ##############################################    
        kl, kt, box, tri, interf, s1, s2, s3 = symbols('kl kt box tri interf s1 s2 s3')
        
        ### the vector of couplings
        c = Matrix([
            [kt**4]         ,
            [kt**2 * kl**2] ,
            [kt**3 * kl]    ,
        ])
        ### the vector of components
        v = Matrix([
            [box]   ,
            [tri]   ,
            [interf],
        ])    
        ### the vector of samples (i.e. cross sections)
        s = Matrix([
            [s1] ,
            [s2] ,
            [s3]
        ])    
        ####    
        Minv   = self.M.inv()
        self.coeffs = c.transpose() * Minv # coeffs * s is the sigma, accessing per component gives each sample scaling
        self.sigma  = self.coeffs*s

########################################################
# definition of the model inputs

## NOTE : the scaling functions are not sensitive to global scalings of the xs of each sample
## so by convention use val_xs of the samples *without* the decay BR

## NOTE 2 : the xs *must* correspond to the generator one

# VBF : val_CV, val_C2V, val_kl
VBF_sample_list = [
    VBFHHSample(1,1,1,   val_xs = 0.00054/(0.3364), label = 'qqHH_CV_1_C2V_1_kl_1'  ),
    VBFHHSample(1,2,1,   val_xs = 0.00472/(0.3364), label = 'qqHH_CV_1_C2V_2_kl_1'  ),
    VBFHHSample(1,1,2,   val_xs = 0.00044/(0.3364), label = 'qqHH_CV_1_C2V_1_kl_2'  ),
    VBFHHSample(1,1,0,   val_xs = 0.00145/(0.3364), label = 'qqHH_CV_1_C2V_1_kl_0'  ),
    VBFHHSample(0.5,1,1, val_xs = 0.00353/(0.3364), label = 'qqHH_CV_0p5_C2V_1_kl_1'),
    VBFHHSample(1.5,1,1, val_xs = 0.02149/(0.3364), label = 'qqHH_CV_1p5_C2V_1_kl_1'),
]

# VBF : val_kl, val_kt
GGF_sample_list = [
    GGFHHSample(1,1,   val_xs = 0.02675, label = 'ggHH_kl_1_kt_1'  ),
    GGFHHSample(0,1,   val_xs = 0.06007, label = 'ggHH_kl_0_kt_1'  ),
    GGFHHSample(5,1,   val_xs = 0.07903, label = 'ggHH_kl_5_kt_1'  ),
    # GGFHHSample(2.45,1,   val_xs = 0.01133, label = 'ggHH_kl_2p45_kt_1'  ),
]
