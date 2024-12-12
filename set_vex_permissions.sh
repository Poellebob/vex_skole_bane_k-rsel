#!/bin/bash

# Find the VEX device
VEX_DEVICE=$(lsusb | grep "VEX Robotics V5 Brain" | awk '{print $2"/"$4}' | sed 's/://')

if [ -z "$VEX_DEVICE" ]; then
    echo "VEX device not found."
    exit 1
fi

# Get the device path
DEVICE_PATH=$(readlink -f /dev/bus/usb/$VEX_DEVICE)

if [ -z "$DEVICE_PATH" ]; then
    echo "Device path not found."
    exit 1
fi

# Change permissions
sudo chmod 666 $DEVICE_PATH

echo "Permissions set to 666 for $DEVICE_PATH"