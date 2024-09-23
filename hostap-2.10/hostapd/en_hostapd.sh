#!/bin/bash

sudo rfkill unblock all

sudo ifconfig wlan0 down

sudo ifconfig p2p0 down

sudo ifconfig ap0 up

sudo ifconfig ap0 192.168.40.1

sudo killall hostapd

sudo dnsmasq --interface=ap0 --no-resolv --leasefile-ro --no-poll --dhcp-range=192.168.40.2,192.168.40.254,12h --port 54 #--no-daemon

<<!
sudo killall -9 udhcpd
sudo udhcpd -fS ./udhcpd.conf &
!

sudo ./hostapd -ddd ./hostapd.conf.mt76 | while read line;do echo `date +%F\ %H:%M:%S.%3N` $line;done
