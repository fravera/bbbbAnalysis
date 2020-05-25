import os
import sys
import argparse
import datetime
import glob
import re

def getLog (proto, ID, silenceWarning=False):
    logs = glob.glob(proto.format(ID, '*'))
    if len(logs) == 0:
        if not silenceWarning: print ">>> No log found for job", ID
        return None
    if len(logs) > 1:
        # print ">>> Too many logs found for job", ID, ' : ', logs, 'returning last'
        if not silenceWarning: print ">>> Too many logs found for job", ID, ' (resubmitted?) , returning last'
    return logs[-1]

# def getExitCode(fname):
#     f = open(fname)
#     code = -888
#     fileCopied = False;
#     for line in f:
#         if code == -888:
#             if "*** Break *** segmentation violation" in line:
#                 code = -666;
#             if '... job finished with status' in line:
#                 code = int(re.search('... job finished with status (\d+)', line).group(1))
#         if "... copy done with status 0" in line:
#             fileCopied = True;

#     if fileCopied and not (code == 0 or code == -888):
#         code = code - 10000
#     if code==0 and not fileCopied:
#         code=-777
#     return code

def getExitCode(fname, text='job ended with status'):
    f = open(fname)
    code = -888
    for line in f:
        # if not '... cmsRun finished with status' in line:
        if not '... %s' % text in line:
            continue
        # code = int(re.search('... cmsRun finished with status (\d+)', line).group(1))
        code = int(re.search('... %s (\d+)' % text, line).group(1))
    return code


##############################
##### CMD line options
parser = argparse.ArgumentParser(description='Command line parser of plotting options')

parser.add_argument('--dir',          dest='folder',      help='process folder',         default=None)
parser.add_argument('--resubCmd',     dest='resubCmd',    help='print resubmit cmd',  action='store_true',   default=False)
parser.add_argument('--issueCmd',     dest='issueCmd',    help='issue resubmit cmd',  action='store_true',   default=False)
parser.add_argument('--long',         dest='short',       help='want long summary',   action='store_false',  default=True)

args = parser.parse_args()

if not args.folder:
    print "Please set job+log folder name"

##############################
#remove last last folder / if present
folder = args.folder
if folder[-1] == '/':
    folder=folder[:-1]

job_proto = folder + '/job_{0}.sh'
log_proto = folder + '/job_{0}.sh_{1}.stdout'

jobs_sh = glob.glob(job_proto.format('*'))
jobs_ID = [int(re.search(job_proto.format('(\d+)'), x).group(1)) for x in jobs_sh]
# print jobs_ID
# print "** Found", len(jobs_ID), 'jobs'

if len(jobs_ID) == 0:
    print "** No jobs found"
    sys.exit()

logs_txt = {ID: getLog(log_proto, ID) for ID in jobs_ID if getLog(log_proto, ID, silenceWarning=True)}
# print logs_txt

#############################

exitCodes_job  = []
exitCodes_run  = []
exitCodes_copy = []
## code -999 means no log yet (unfinished?)
## code -888 means no CMSSW string found (unfinished? / crash?)
for ID in jobs_ID:
    if not ID in logs_txt:
        # exitCodes.append(-999)
        exitCodes_job.append(-999)
        exitCodes_run.append(-999)
        exitCodes_copy.append(-999)
    else:
        # exitCodes.append(getExitCode(logs_txt[ID]))
        exitCodes_job.append(getExitCode(logs_txt[ID],  'job finished with status'))
        exitCodes_run.append(getExitCode(logs_txt[ID],  'execution finished with status'))
        exitCodes_copy.append(getExitCode(logs_txt[ID], 'copy done with status'))

###########################
missing         = []
unfinished      = []
failed          = []
success         = []
notCopied       = []
failedButCopied = []


# for idx, ID in enumerate(jobs_ID):
#     code = exitCodes[idx]
#     if code <=-10000:
#         failedButCopied.append(ID)
#         code = code + 10000
#     if   code == -999: 
#         missing.append((ID, code))
#     elif code == -888: 
#         unfinished.append((ID, code))
#     elif code == 0:    
#         success.append((ID, code))
#     elif code == -777:
#         notCopied.append((ID, code))
#     else:              
#         failed.append((ID, code))

for idx, ID in enumerate(jobs_ID):
    code_run  = exitCodes_run[idx]
    code_copy = exitCodes_copy[idx]
    code_job  = exitCodes_job[idx]

    code = code_job ## FIXME; propagate more codes?

    if code_job == -999:
        missing.append((ID, code))
    elif code_job == -888:
        unfinished.append((ID, code))
    elif code_run == 0 and code_job == 0 and code_copy == 0:
        success.append((ID, code))
    else:
        failed.append((ID, code))

    ## special cases
    if code_copy == 0 and code_run != 0:
        failedButCopied.append((ID, code))
    if code_copy != 0:
        notCopied.append((ID, code))

    # if code <=-10000:
    #     failedButCopied.append(ID)
    #     code = code + 10000
    # if   code == -999: 
    #     missing.append((ID, code))
    # elif code == -888: 
    #     unfinished.append((ID, code))
    # elif code == 0:    
    #     success.append((ID, code))
    # elif code == -777:
    #     notCopied.append((ID, code))
    # else:              
    #     failed.append((ID, code))


# print exitCodes
print "\n***********************************************************"
print "** TOTAL JOB FOUND: ", len(jobs_ID)
print "** Success        : ", len(success) , "(%.1f%%)" % (100.*len(success)/len(jobs_ID))
print "** Failed         : ", len(failed) , "(%.1f%%)" % (100.*len(failed)/len(jobs_ID))
print "** Unfinished     : ", len(unfinished) , "(%.1f%%)" % (100.*len(unfinished)/len(jobs_ID))
print "** Missing logs   : ", len(missing) , "(%.1f%%)" % (100.*len(missing)/len(jobs_ID))
print "**************************** "
print "** not copied          : ", len(notCopied) , "(%.1f%%)" % (100.*len(notCopied)/len(jobs_ID))
print "** failed but copied   : ", len(failedButCopied) , "(%.1f%%)" % (100.*len(failedButCopied)/len(jobs_ID))
print "***********************************************************"

if not args.short:
   print '\n'
   print '** Failed jobs ID + CODE, logfile'
   for val in failed:
       print '-', val[0], '+', val[1], ' , ', logs_txt[val[0]]
   print '** Unfinished jobs ID + CODE, logfile'
   for val in unfinished:
       print '-', val[0], '+', val[1], ' , ', logs_txt[val[0]]
   print '** Missing log jobs ID'
   for val in missing:
       print '-', val[0]

#######################
if args.resubCmd or args.issueCmd:
    print "\n** Resubmit commands\n"
    # print "cd %s" % args.folder
    resubCmds = []
    failed = failed
    # failed = failed + notCopied
    for val in failed:
        if val[0] in failedButCopied:
            print "file from job ", val[0], " need to be canceled"
        # jobscript = job_proto.format(val[0]).replace(args.folder + '/', '')
        jobscript = args.folder + '/job_' + str(val[0]) + '.sh'
        command   = "scripts/t3submit %s" % jobscript
        resubCmds.append(command)
    for cmd in resubCmds:
        print cmd

if args.issueCmd:
    print "\n** Issuing resub\n"
    for cmd in resubCmds:
        print cmd
        os.system(cmd)


