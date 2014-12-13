#!/bin/bash
# shell script for task 2 rnp
# Jannik Iacobi, Cumhur Güner
# 26.11.14

echo "Windowsize Transfer Rate"
# test multiple connections that end directly
for (( i=512;$i<=600000;i*=2 )); 
do
	netperf -H 192.168.17.17 -l 1 -- -S $i | awk '/16384 /{printf "%9d  %6.2f\n", $1/2, $5}'
done
exit $?
