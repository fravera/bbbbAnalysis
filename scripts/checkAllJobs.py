import os
import sys
import subprocess
import argparse
import glob

##############################
##### CMD line options
parser = argparse.ArgumentParser(description='Command line parser of plotting options')

parser.add_argument('--dir',          dest='folderList',      help='process folder', nargs='+',        required=True)
# parser.add_argument('--dir',          dest='folder',      help='process folder',         default=None)
parser.add_argument('--resubCmd',     dest='resubCmd',    help='print resubmit cmd',  action='store_true',   default=False)
parser.add_argument('--issueCmd',     dest='issueCmd',    help='issue resubmit cmd',  action='store_true',   default=False)

args = parser.parse_args()
# folder = args.folder
# if '*' in folder:
#     out = subprocess.Popen(['ls', '-1',  '-d', folder], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
#     stdout,stderr = out.communicate()
#     folderList = []
#     for folderName in stdout.split():
#         folderList.append(folderName)
# else:
#     folderList = [folder]

folderList = []
for pattern in args.folderList:
    folderList.extend(glob.glob(pattern))

# folderList = args.folderList


totalNumberOfJobs = 0
totalNumberOfCompletedJobs = 0

for currentFolder in folderList:
    out = subprocess.Popen(['ls', '-1', currentFolder], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
    stdout,stderr = out.communicate()
    print "Checking jobs for", currentFolder
    for jobName in stdout.split():
        totalNumberOfJobs +=1
        fullFolderName = currentFolder + "/" + jobName
        # print "Checking folder", fullFolderName
        taskCommand = ['python', 'scripts/getTaskStatus.py', '--dir', fullFolderName]
        if args.resubCmd: taskCommand.append("--resubCmd")
        if args.issueCmd: taskCommand.append("--issueCmd")
        statusOut = subprocess.Popen(taskCommand, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)
        statusStdout,statusStderr = statusOut.communicate()
        if ("** Failed         :  0 (0.0%)" not in statusStdout) or ("** failed but copied   :  0 (0.0%)" not in statusStdout) :
            print "Failed jobs for ", fullFolderName
        for line in statusStdout.splitlines():
            if ("** Success        :" in line) and ("(100.0%)" in line) : totalNumberOfCompletedJobs += 1
            elif ("** Success        :" in line) and ("(100.0%)" not in line): print "Jobs still running for ", fullFolderName

print "Completed jobs = ", totalNumberOfCompletedJobs, " over ", totalNumberOfJobs



