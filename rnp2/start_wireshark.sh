#!/bin/bash
xhost +local:root
echo "starting wireshark"
sudo /usr/bin/wireshark


exit $?
