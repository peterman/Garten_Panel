Import("env")
import os
import git
import time
import subprocess
import shutil
import requests
import json

BUILDNR = int(round(time.time()))
VERSION_FILE = "version.h"
CONFIG_FILE = "config.h"
BUILD_DIR = "build/"
GITVERSION = ""
BOARD = env['BOARD']
API_URL = ""
FILENAME = ""

cwd = os.path.abspath(os.getcwd())
r = git.repo.Repo(cwd)
GITVERSION = r.git.describe();


def in_docker():
    """ Returns: True if running in a docker container, else False """
    try:
        with open('/proc/1/cgroup', 'rt') as ifh:
            return 'docker' in ifh.read()
    except:
        return False

print ("----------------------- BUILD STARTED -----------------------")
print ("Info:")
print ("-------------------------------------------------------------")
print ("Current build targets: \t"), map(str, BUILD_TARGETS)
print ("Board: \t")+env['BOARD']
print ("Version: \t")+GITVERSION
print ("In Docker: \t")+str(in_docker())


if in_docker():
    print ("CI-Process, generate build version")
    print ("Docker container: No deletion of build files")
else:
    print ("No CI-Process, generate dev version")
    GITVERSION = GITVERSION + "_dev"
print ("-------------------------------------------------------------")

if not os.path.exists(BUILD_DIR):
    os.mkdir(BUILD_DIR)
else:
    if not in_docker():
        shutil.rmtree(BUILD_DIR)
        os.mkdir(BUILD_DIR)

def getBoardID(name):
    print ("[getBoardID] Getting Board-ID")
    payload = {'columns': 'ID', 'filter': 'InternalName,eq,'+name}
    r = requests.get(API_URL + "/Boards/?transform=1" , params=payload)
    r.raise_for_status()

    ret = r.json()
    BoardID = ret['Boards'][0]['ID']
    print ("[getBoardID] Board-ID: ")+str(BoardID)
    return BoardID

def archiveVersion(source, target, env):
    if in_docker():
        print ("[archiveVersion] Save Version")
        BoardID = getBoardID(BOARD)

        if BoardID != 0:
        
            payload = {'Version': GITVERSION, 'Build': BUILDNR, 'Path':FILENAME, 'Board_ID':BoardID, 'DevVersion':'false'}

            r = requests.post(API_URL + "/Boards_Firmware",  data=payload)
            # print ("Return: "+r.text
            r.raise_for_status()
            print ("[archiveVersion] Version-ID: ")+str(r.text)
        else:
            print ("[archiveVersion] Keine Board-ID ermittelt")
    else:
        print ("No CI process, don't archive version to database")


def writeversion(source, target, env):
    print ("----------------------- writeversion Start -----------------------")
       
    binPath = os.path.join('.pioenvs',BOARD,'firmware.bin')
    destPath = os.path.join('build', BOARD+"_"+str(BUILDNR)+'.bin')

    global FILENAME 
    FILENAME = BOARD+"_"+str(BUILDNR)+'.bin'
    print ("Copying File: ")+binPath+' to '+destPath
    # os.rename(binPath, destPath)
    shutil.copyfile(binPath, destPath)
    
    print ("----------------------- writeversion End -----------------------")

def before_build(source, target, env):
    # print ("Build in docker: "+str(in_docker())
    writeversion(source, target, env)

def after_build(source, target, env):
    print ("----------------------- archiveVersion Start -----------------------")
    archiveVersion(source, target, env)
    print ("----------------------- archiveVersion End -----------------------")
    # do some actions

def prepareVersionAndConfig(source, target, env):
    print ("----------------------- prepareVersionAndConfig Start -----------------------")

    FILE = os.path.join(os.path.join(cwd, "src"), VERSION_FILE)
    os.remove(FILE)

    if os.path.exists(FILE):
        f = file(FILE, "r+")
    else:
        f = file(FILE, "w")


    f.write('const int FW_VERSION = '+str(BUILDNR)+';\n#ifndef VERSION_H\n#define VERSION_H\n#define _VER_ ( "'+GITVERSION+'" )\n#endif //VERSION_H')
    f.close()

    FILE = os.path.join(os.path.join(cwd, "src"), CONFIG_FILE)
    os.remove(FILE)

    if os.path.exists(FILE):
        f = file(FILE, "r+")
    else:
        f = file(FILE, "w")
    
    f.write('#ifndef CONFIG_H\n#define CONFIG_H\n#define _BOARDNAME_ ( "'+BOARD+'" )\n#define _BOARDID_ ("'+str(getBoardID(BOARD))+'")\n#endif //VERSION_H')
    f.close()
    print ("----------------------- prepareVersionAndConfig End -----------------------")


env.AddPreAction("buildprog", before_build)
env.AddPostAction("buildprog", after_build)
env.AddPreAction("$BUILD_DIR/src/main.o", prepareVersionAndConfig)