#!/bin/bash
#----------------------------- User configuration -----------------------------

# Common settings
#------------------------

# How you're loading your module. Used to determine how to find the target module. (AsRtld/Module/Kip)
export LOAD_KIND="Module"
# Program you're targetting. Used to determine where to deploy your files.
export PROGRAM_ID="0100801011c3e000"
# Optional path to copy the final ELF to, for convenience.
export ELF_EXTRACT=""
# Python command to use. Must be Python 3.4+.
export PYTHON="python3"
# Make arguments.
export MAKE_ARGS="-j8 V=1"
# JSON to use to make .npdm
export NPDM_JSON="qlaunch.json"
# Additional C/C++ flags to use.
export C_FLAGS=""
export CXX_FLAGS=""

# AsRtld settings
#------------------------

# Path to the SD card. Used to mount and deploy files on SD, likely with hekate UMS.
export MOUNT_PATH="/mnt/k"

# Module settings
#------------------------

# Settings for deploying over FTP. Used by the deploy-ftp.py script.
export FTP_IP="192.168.0.235"
export FTP_PORT="5000"
export FTP_USERNAME="anonymous"
export FTP_PASSWORD=""

# Settings for deploying to Ryu. Used by the deploy-ryu.sh script.
export RYU_PATH="/mnt/c/Users/shado/AppData/Roaming/Ryujinx"

#-------------------------- End of user configuration --------------------------

# Setup common variables.
export MISC_PATH=$(pwd)/misc
export MK_PATH=$MISC_PATH/mk
export SCRIPTS_PATH=$MISC_PATH/scripts
export SPECS_PATH=$MISC_PATH/specs
export NPDM_JSON_PATH=$MISC_PATH/npdm-json

# Setup variable for .json
export NPDM_JSON=$NPDM_JSON_PATH/$NPDM_JSON

# Import target env variables.
source $SCRIPTS_PATH/target-common.sh

# Import target env for load kind.
if [ $LOAD_KIND == "Module" ]; then
    export LOAD_KIND_ENUM=2
    source $SCRIPTS_PATH/target-module.sh
elif [ $LOAD_KIND == "AsRtld" ]; then
    export LOAD_KIND_ENUM=1
    source $SCRIPTS_PATH/target-rtld.sh
else 
    echo "Invalid LOAD_KIND!"
    exit 1
fi

# Perform user action.
if [ "$1" == "build" ]; then
    make $MAKE_ARGS
    source $SCRIPTS_PATH/post-build.sh
elif [ "$1" == "clean" ]; then
    make $MAKE_ARGS clean
elif [ "$1" == "deploy-sd" ]; then
    source $SCRIPTS_PATH/deploy-sd.sh
elif [ "$1" == "deploy-ftp" ]; then
    $PYTHON $SCRIPTS_PATH/deploy-ftp.py
elif [ "$1" == "deploy-ryu" ]; then
     source $SCRIPTS_PATH/deploy-ryu.sh
elif [ "$1" == "make-npdm-json" ]; then
    $PYTHON $SCRIPTS_PATH/make-npdm-json.py
else
    echo "Invalid arg. (build/clean/deploy-sd/deploy-ftp/deploy-ryu)"
fi