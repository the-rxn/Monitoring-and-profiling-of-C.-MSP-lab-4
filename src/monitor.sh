#!/bin/bash 
echo -e "\033[32m  running memUsage and cpuUsage monitoring script\033[30m"

echo -e "\e[49m \e[39m ∞ "
# echo -e '\033[2K'

while sleep 1; do ./memUsage.sh; ./cpuUsage.sh ; done;
echo "killed monitoring script"
