#!/bin/bash
set -e

# Verify the path is set.
if [ -z "${RYU_PATH}" ]; then
    echo "RYU_PATH appears to not be set! Check your exlaunch.sh?"
fi

# Setup the path to the game's mods folder.
export MODS_PATH=${RYU_PATH}/mods/contents/${PROGRAM_ID}/mods

# Ensure directory exists.
mkdir -p ${MODS_PATH}/exefs;

# Copy over files.
cp ${OUT}/* ${MODS_PATH}/exefs