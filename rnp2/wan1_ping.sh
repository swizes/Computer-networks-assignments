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

rm -f 3_3_1.dat
# test multiple connections that end directly
for (( i=50;$i<=1500;i+=100 ))
do
    echo $i
    ping 192.168.18.133 -s $i -c 5 >>3_3_1.dat
done
awk '/time=/{print $1, substr($7,6)}' 3_3_1.dat >3_3_2.dat

gnuplot -p -e "f(x)=a+b*x;
fit f(x) \"3_3_2.dat\" via a,b;
plot \"3_3_2.dat\", f(x);"

exit $?
