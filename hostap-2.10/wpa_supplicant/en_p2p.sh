#!/bin/bash

sudo rfkill unblock all

sudo ifconfig wlan0 down

sudo ifconfig ap0 down

sudo ifconfig p2p0 up

sudo killall -9 wpa_supplicant

sudo ./wpa_supplicant -ip2p0 -Dnl80211 -c ./p2p_supplicant.conf -ddd | while read line;do echo `date +%F\ %H:%M:%S.%3N` $line;done

<<!
sudo netstat -anlp | grep -w LISTEN

sudo killall -9 udhcpd
sudo udhcpd -fS ./udhcpd.conf

# setup oper channel and enter the monitoring mode.
# the phone will display this device.
sudo wpa_cli -ip2p0 p2p_set listen_channel 1
sudo wpa_cli -ip2p0 p2p_listen
sudo wpa_cli -ip2p0 list_network


# force go
sudo wpa_cli -ip2p0 p2p_group_remove p2p-p2p0-0
sudo wpa_cli -ip2p0 p2p_group_add persistent freq=5180 vht

sudo ifconfig p2p-p2p0-0 192.168.2.1
sudo dnsmasq --interface=p2p-p2p0-0 --no-daemon --no-resolv --leasefile-ro --no-poll --dhcp-range=192.168.2.2,192.168.2.254,12h --port 54

# agree to gc joining the group
sudo wpa_cli -ip2p-p2p0-0 wps_pbc

# pin
sudo wpa_cli -ip2p-p2p0-0 wps_pin any 12345678

sudo wpa_cli -ip2p-p2p0-0 status
sudo wpa_cli -ip2p-p2p0-0 all_sta / list_st


# gc role
# the board connects to the phone actively in gc mode.
sudo wpa_cli -ip2p0 p2p_find 3
sudo wpa_cli -ip2p0 p2p_stop_find
sudo wpa_cli -ip2p0 p2p_peers
sudo wpa_cli -ip2p0 p2p_set disallow_freq 2000-5740,5750-8000
sudo wpa_cli -ip2p0 p2p_connect 3e:c9:5f:2b:b6:99 pbc go_intent=1

sudo udhcpc -ip2p-p2p0-0 -bq

# pin
sudo wpa_cli -ip2p0 p2p_connect 3e:c9:5f:2b:b6:99 12345678 join auto
!

