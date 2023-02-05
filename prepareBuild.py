Import("env")
import os
import time
import subprocess
import shutil
import requests
import json


BUILD_TIME = int(round(time.time()))
VERSION_FILE = "version.h"
CONFIG_FILE = "config.h"
BUILD_DIR = "build/"
BUILD_SRC = env['PIOENV']
BOARD = env['BOARD']
SERVER_URL = "http://217.160.249.173/"
CONFIG_URL = ""
FILENAME = ""

cwd = os.path.abspath(os.getcwd())
print ("-------------------------------------------------------------")

if not os.path.exists(BUILD_DIR):
    os.mkdir(BUILD_DIR)
if not os.path.exists(BUILD_DIR + BUILD_SRC):
        os.mkdir(BUILD_DIR + BUILD_SRC)
if not os.path.exists(BUILD_DIR + 'config'):
        os.mkdir(BUILD_DIR + 'config')

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
    destPath = os.path.join('build', BUILD_SRC, "firmware_"+str(BUILD_TIME)+'.bin')
    confFile = os.path.join('build','config','config.json')
    
    shutil.copyfile(binPath, destPath)
    
    if os.path.exists(confFile):
        f = open(confFile, "a")
    else:
        f = open(confFile, "w")
    
    confData = {
        "type": BUILD_SRC,
        "version": "2.5.1",
        "url": SERVER_URL + BUILD_SRC + "/firmware_" + str(BUILD_TIME)+".bin"
        }
    with f as outfile:
        json.dump(confData, outfile)
    
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
    f.write('const int FW_VERSION = '+str(BUILD_TIME)+';\n#ifndef VERSION_H\n#define VERSION_H\n#endif //VERSION_H')
    f.close()

    FILE = os.path.join(os.path.join(cwd, "src"), CONFIG_FILE)
    #os.remove(FILE)

    if os.path.exists(FILE):
        f = open(FILE, "r+")
    else:
        f = open(FILE, "w")
    
    f.write('#ifndef CONFIG_H\n#define CONFIG_H\n#define _BOARDNAME_ ( "'+BUILD_SRC+'" )\n#endif //VERSION_H')
    f.close()
    print ("----------------------- prepareVersionAndConfig End -----------------------")


env.AddPreAction("buildprog", before_build)
env.AddPostAction("buildprog", after_build)
env.AddPreAction("$BUILD_DIR/src/main.cpp.o", prepareVersionAndConfig)