#!/bin/bash
# shell script for task 2 rnp
# Jannik Iacobi, Cumhur Güner
# 26.11.14
usage()
{
	echo "usage: $0 <hostname> <hostport>"
}

#if [ $# <= 2 ]; then
#    usage
#    exit 1
#fi

rm -f 1.dat
# test multiple connections that end directly
for (( i=50;$i<=1500;i+=100 ))
do
    echo $i
    ping 192.168.17.13 -s $i -c 5 >>1.dat
done
awk '/time=/{print $1, substr($7,6)}' 1.dat >2.dat

gnuplot -p -e "f(x)=a+b*x;
fit f(x) \"2.dat\" via a,b;
plot \"2.dat\", f(x);"

exit $?
