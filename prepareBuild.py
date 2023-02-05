Import("env")
import os
#import git
import time
import subprocess
import shutil
import requests
import json

#BUILDNR = int(round(time.time()))
BUILDNR = time.strftime("%Y%m%d_%H%M%S", time.localtime())
VERSION_FILE = "version.h"
CONFIG_FILE = "config.h"
BUILD_DIR = "build/"
BUILD_SRC = env['PIOENV']
BOARD = env['BOARD']
API_URL = ""
FILENAME = ""

cwd = os.path.abspath(os.getcwd())
#r = git.repo.Repo(cwd)
#GITVERSION = r.git.describe();


print ("-------------------------------------------------------------")

if not os.path.exists(BUILD_DIR):
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



def writeversion(source, target, env):
    print ("----------------------- writeversion Start -----------------------")
       
    binPath = os.path.join('.pio/build',BUILD_SRC,'firmware.bin')
    destPath = os.path.join('build', BOARD+"_"+str(BUILDNR)+'.bin')

    global FILENAME 
    FILENAME = BOARD+"_"+str(BUILDNR)+'.bin'
    #print ("Copying File: ")+binPath+(" to ")+destPath
    # os.rename(binPath, destPath)
    shutil.copyfile(binPath, destPath)
    
    print ("----------------------- writeversion End -----------------------")

def before_build(source, target, env):
    # print ("Build in docker: "+str(in_docker())
    writeversion(source, target, env)

def after_build(source, target, env):
    print ("----------------------- archiveVersion Start -----------------------")
    #archiveVersion(source, target, env)
    print ("----------------------- archiveVersion End -----------------------")
    # do some actions

def prepareVersionAndConfig(source, target, env):
    print ("----------------------- prepareVersionAndConfig Start -----------------------")

    FILE = os.path.join(os.path.join(cwd, "src"), VERSION_FILE)
    #os.remove(FILE)

    if os.path.exists(FILE):
        f = open(FILE, "r+")
    else:
        f = open(FILE, "w")


    #f.write('const int FW_VERSION = '+str(BUILDNR)+';\n#ifndef VERSION_H\n#define VERSION_H\n#define _VER_ ( "'+GITVERSION+'" )\n#endif //VERSION_H')
    f.write('const int FW_VERSION = '+str(BUILDNR)+';\n#ifndef VERSION_H\n#define VERSION_H\n#endif //VERSION_H')
    f.close()

    #FILE = os.path.join(os.path.join(cwd, "src"), CONFIG_FILE)
    #os.remove(FILE)

    #if os.path.exists(FILE):
    #    f = open(FILE, "r+")
    #else:
    #    f = open(FILE, "w")
    
    #f.write('#ifndef CONFIG_H\n#define CONFIG_H\n#define _BOARDNAME_ ( "'+BOARD+'" )\n#define _BOARDID_ ("'+str(getBoardID(BOARD))+'")\n#endif //VERSION_H')
    #f.close()
    print ("----------------------- prepareVersionAndConfig End -----------------------")


env.AddPreAction("buildprog", before_build)
env.AddPostAction("buildprog", after_build)
env.AddPreAction("$BUILD_DIR/src/main.cpp.o", prepareVersionAndConfig)