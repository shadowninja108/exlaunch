#!/bin/bash
set -e

# Mount drive.
sudo mount ${MOUNT_PATH}
# Copy files to drive.
cp ${OUT}/* ${MOUNT_PATH}${SD_OUT}
# Unmount drive.
sudo umount ${MOUNT_PATH}