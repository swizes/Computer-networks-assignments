#!/bin/bash

name=$(/usr/bin/awk 'BEGIN {RS=" ";FS="="}$1=="name"{print $2}' /proc/cmdline )

echo "Init script on router $name started."

hostname $name

echo "1" > /proc/sys/net/ipv4/ip_forward

echo "#no nameserver configured" >/etc/resolv.conf

echo  '127.0.0.1 localhost
172.16.11.1  r1_0
172.16.11.2  r2_0
172.16.12.1  r2_1
172.16.15.1  r2_2
172.16.12.2  r3_0
172.16.12.3  r4_0
172.16.103.2  r4_1
172.16.103.1  r5_0
172.16.106.1  r5_1
172.16.xx.xx  r5_2
172.16.106.2  r6_0
172.16.15.2   r6_1
172.16.14.1   r6_2
172.16.14.2  r7_0
' >/etc/hosts

echo 'iptables -P FORWARD DROP
iptables -A FORWARD -m state --state ESTABLISHED,RELATED  -j ACCEPT
iptables -A FORWARD -m state --state NEW -p tcp --dport 21  -j ACCEPT'>/root/.bash_history


ifconfig lo 127.0.0.1/24 up


case $name in
  r1)
    ifconfig eth0 172.16.11.1/24 up
    #ROUTING TABELLE ANLEGEN
    route add -net 172.16.12.0/24 gw 172.16.11.2
    route add -net 172.16.103.0/24 gw 172.16.11.2
    route add -net 172.16.15.0/24 gw 172.16.11.2
    route add -net 172.16.14.0/24 gw 172.16.11.2
    route add -net 172.16.106.0/24 gw 172.16.11.2
  ;;
  r2)
    ifconfig eth0 172.16.11.2/24 up
    ifconfig eth1 172.16.12.1/24 up
    ifconfig eth2 172.16.15.1/24 up
    #---------- ROUTING TABELLE ANLEGEN ---------- 
    route add -net 172.16.103.0/24 gw 172.16.12.3
    route add -net 172.16.14.0/24 gw 172.16.15.2
    route add -net 172.16.106.0/24 gw 172.16.15.2
    #---------- POLICY SETZEN---------- 
    iptables -P FORWARD DROP
    iptables -P INPUT   DROP
    iptables -P OUTPUT  DROP
    #---------- PING ERLAUBEN---------- 
    #Ping von Netzwerk 1 an Router 2
    iptables -A INPUT -s 172.16.11.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A OUTPUT -d 172.16.11.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 2 an Router 2
    iptables -A INPUT -s 172.16.12.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A OUTPUT -d 172.16.12.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 4 an Router 2
    iptables -A INPUT -s 172.16.14.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A OUTPUT -d 172.16.14.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 1 an Netzwerk 2
    iptables -A FORWARD -s 172.16.11.0/24 -d 172.16.12.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.11.0/24 -s 172.16.12.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 1 an Netzwerk 5
    iptables -A FORWARD -s 172.16.11.0/24 -d 172.16.15.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.11.0/24 -s 172.16.15.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 1 an Netzwerk 3
    iptables -A FORWARD -s 172.16.11.0/24 -d 172.16.103.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.11.0/24 -s 172.16.103.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 1 an Netzwerk 4
#    iptables -A FORWARD -s 172.16.11.0/24 -d 172.16.14.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
#    iptables -A FORWARD -d 172.16.11.0/24 -s 172.16.14.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 2 an Netzwerk 1
    iptables -A FORWARD -s 172.16.12.0/24 -d 172.16.11.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.12.0/24 -s 172.16.11.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 2 an Netzwerk 5
    iptables -A FORWARD -s 172.16.12.0/24 -d 172.16.15.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.12.0/24 -s 172.16.15.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 2 an Netzwerk 4
    iptables -A FORWARD -s 172.16.12.0/24 -d 172.16.14.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.12.0/24 -s 172.16.14.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 4 an Netzwerk 3
    iptables -A FORWARD -s 172.16.14.0/24 -d 172.16.103.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.14.0/24 -s 172.16.103.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 4 an Netzwerk 1
#    iptables -A FORWARD -s 172.16.14.0/24 -d 172.16.11.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
#    iptables -A FORWARD -d 172.16.14.0/24 -s 172.16.11.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 4 an Netzwerk 2
    iptables -A FORWARD -s 172.16.14.0/24 -d 172.16.12.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.14.0/24 -s 172.16.12.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #---------- SSH---------- 
    #Von Netzwerk 1 an Rechner 3
    iptables -A FORWARD	-p tcp --dport 22 -d 172.16.12.2 -j ACCEPT
    iptables -A FORWARD -p tcp --sport 22 -s 172.16.12.2 -j ACCEPT
    #---------- Stateful----------
    #Von Netzwerk 1 zu Rechner 3
    iptables -A FORWARD -m state --state ESTABLISHED,RELATED -j ACCEPT
    iptables -A FORWARD -m state --state NEW -p tcp --syn --dport 80 -d 172.16.12.2 -j ACCEPT
    iptables -A FORWARD -m state --state NEW -p tcp --syn --dport 21 -d 172.16.12.2 -j ACCEPT
  ;;
  r3)
    ifconfig eth0 172.16.12.2/24 up
    #ROUTING TABELLE ANLEGEN
    route add -net 172.16.11.0/24 gw 172.16.12.1
    route add -net 172.16.103.0/24 gw 172.16.12.3
    route add -net 172.16.106.0/24 gw 172.16.12.1
    route add -net 172.16.15.0/24 gw 172.16.12.1
    route add -net 172.16.14.0/24 gw 172.16.12.1
  ;;
  r4)
    ifconfig eth0 172.16.12.3/24 up
    ifconfig eth1 172.16.103.2/24 up
    #ROUTIN TABELLE ANLEGEN
    route add -net 172.16.11.0/24 gw 172.16.12.1
    route add -net 172.16.15.0/24 gw 172.16.12.1
    route add -net 172.16.14.0/24 gw 172.16.103.1
    route add -net 172.16.106.0/24 gw 172.16.103.1
    #POLICY SETZEN
    iptables -P FORWARD DROP
    iptables -P INPUT   DROP
    iptables -P OUTPUT  DROP
    #Ping von Netzwerk 1 an Router 4
    iptables -A INPUT -s 172.16.11.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A OUTPUT -d 172.16.11.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 2 an Router 4
    iptables -A INPUT -s 172.16.12.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A OUTPUT -d 172.16.12.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 4 an Router 4
    iptables -A INPUT -s 172.16.14.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A OUTPUT -d 172.16.14.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping an Internet 5
    iptables -A FORWARD -s 172.16.11.0/24 -d 172.16.103.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.11.0/24 -s 172.16.103.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 2 an Netzwerk 3
    iptables -A FORWARD -s 172.16.12.0/24 -d 172.16.103.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.12.0/24 -s 172.16.103.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 4 an Netzwerk 2
    iptables -A FORWARD -s 172.16.14.0/24 -d 172.16.12.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.14.0/24 -s 172.16.12.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #---------- SSH---------- 
    #Von Netzwerk 4 an Rechner 3
    iptables -A FORWARD	-p tcp --dport 22 -d 172.16.12.2 -s ! 172.16.103.1 -j ACCEPT
    iptables -A FORWARD -p tcp --sport 22 -s 172.16.12.2 -s ! 172.16.103.1 -j ACCEPT
    #---------- Stateful----------
    #Von Netzwerk 1 zu Rechner 3
    iptables -A FORWARD -m state --state ESTABLISHED,RELATED -j ACCEPT
    iptables -A FORWARD -m state --state NEW -p tcp --syn --dport 80 -d 172.16.12.2 -j ACCEPT
    iptables -A FORWARD -m state --state NEW -p tcp --syn --dport 21 -d 172.16.12.2 -j ACCEPT
  ;;
  r5)
    ifconfig eth0 172.16.103.1/24 up
    ifconfig eth1 172.16.106.1/24 up
    #ROUTIN TABELLE ANLEGEN
    route add -net 172.16.12.0/24 gw 172.16.103.2
    route add -net 172.16.11.0/24 gw 172.16.103.2
    route add -net 172.16.15.0/24 gw 172.16.106.2
    route add -net 172.16.14.0/24 gw 172.16.106.2 
  ;;
  r6)
    ifconfig eth0 172.16.106.2/24 up
    ifconfig eth1 172.16.15.2/24 up
    ifconfig eth2 172.16.14.1/24 up
    #ROUTIN TABELLE ANLEGEN
    route add -net 172.16.11.0/24 gw 172.16.15.1
    route add -net 172.16.12.0/24 gw 172.16.15.1
    #POLICY SETZEN
    iptables -P FORWARD DROP
    iptables -P INPUT   DROP
    iptables -P OUTPUT  DROP
    #Ping von Netzwerk 1 an Router 6
    iptables -A INPUT -s 172.16.11.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A OUTPUT -d 172.16.11.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 2 an Router 6
    iptables -A INPUT -s 172.16.12.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A OUTPUT -d 172.16.12.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 4 an Router 6
    iptables -A INPUT -s 172.16.14.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A OUTPUT -d 172.16.14.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Netzwerk 1 darf an Netzwerk 4 PING
#    iptables -A FORWARD -s 172.16.11.0/24 -d 172.16.14.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
#    iptables -A FORWARD -d 172.16.11.0/24 -s 172.16.14.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 2 an Netzwerk 4
    iptables -A FORWARD -s 172.16.12.0/24 -d 172.16.14.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.12.0/24 -s 172.16.14.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 4 an Netzwerk 3
    iptables -A FORWARD -s 172.16.14.0/24 -d 172.16.103.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.14.0/24 -s 172.16.103.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 4 an Netzwerk 2
    iptables -A FORWARD -s 172.16.14.0/24 -d 172.16.12.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.14.0/24 -s 172.16.12.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 4 an Netzwerk 1
#    iptables -A FORWARD -s 172.16.14.0/24 -d 172.16.11.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
#    iptables -A FORWARD -d 172.16.14.0/24 -s 172.16.11.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #Ping von Netzwerk 4 an Netzwerk 5
    iptables -A FORWARD -s 172.16.14.0/24 -d 172.16.15.0/24 -p icmp --icmp-type ECHO-REQUEST -j ACCEPT
    iptables -A FORWARD -d 172.16.14.0/24 -s 172.16.15.0/24 -p icmp --icmp-type ECHO-REPLY -j ACCEPT
    #---------- SSH---------- 
    #Von Netzwerk 4 an Rechner 3
    iptables -A FORWARD	-p tcp --dport 22 -d 172.16.12.2 -j ACCEPT
    iptables -A FORWARD -p tcp --sport 22 -s 172.16.12.2 -j ACCEPT
    #---------- Stateful----------
    #Von Netzwerk 1 zu Rechner 3
    iptables -A FORWARD -m state --state ESTABLISHED,RELATED -j ACCEPT
    iptables -A FORWARD -m state --state NEW -p tcp --syn --dport 80 -d 172.16.12.2 -j ACCEPT
    iptables -A FORWARD -m state --state NEW -p tcp --syn --dport 21 -d 172.16.12.2 -j ACCEPT
    #---------- NAT----------
    iptables -t nat -A POSTROUTING -o eth0 -j MASQUERADE 
  ;;
  r7)
    ifconfig eth0 172.16.14.2/24 up
    #ROUTIN TABELLE ANLEGEN
    route add -net 172.16.15.0/24 gw 172.16.14.1
    route add -net 172.16.103.0/24 gw 172.16.14.1
    route add -net 172.16.11.0/24 gw 172.16.14.1
    route add -net 172.16.12.0/24 gw 172.16.14.1
    route add -net 172.16.106.0/24 gw 172.16.14.1
  ;;
esac

