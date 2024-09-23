#!/bin/bash

sudo rfkill unblock all

sudo ifconfig p2p0 up

sudo killall -9 wpa_supplicant

sudo ./wpa_supplicant -ip2p0 -Dnl80211 -c ./p2p_supplicant.conf -ddd #-B

<<!
sudo netstat -anlp | grep -w LISTEN

sudo ifconfig p2p0 192.168.60.1
sudo dnsmasq --interface=p2p0 --no-daemon --no-resolv --leasefile-ro --no-poll --dhcp-range=192.168.60.2,192.168.60.254,12h --port 54

sudo wpa_cli -ip2p0 status


# setup oper channel and enter the monitoring mode.
# the phone will display this device.

sudo wpa_cli -ip2p0 p2p_set listen_channel 6
sudo wpa_cli -ip2p0 p2p_listen

sudo wpa_cli -ip2p0 list_network


# the board connects to the phone actively in GO mode.

sudo wpa_cli -ip2p0 p2p_find 10
sudo wpa_cli -ip2p0 p2p_stop_find
sudo wpa_cli -ip2p0 peers
sudo wpa_cli -ip2p0 p2p_connect 5e:05:a2:1a:d4:e1 pbc go_intent=15

sudo killall -9 udhcpd
sudo udhcpd -fS ./udhcpd.conf
!
