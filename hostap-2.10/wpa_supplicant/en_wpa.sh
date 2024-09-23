#!/bin/bash

sudo rfkill unblock all

sudo ifconfig ap0 down

sudo ifconfig p2p0 down

sudo ifconfig wlan0 up

sudo killall -9 wpa_supplicant

sudo ./wpa_supplicant -iwlan0 -Dnl80211 -c ./wpa_supplicant.conf.mt76 -dd | while read line;do echo `date +%F\ %H:%M:%S.%3N` $line;done

sudo udhcpc -i wlan0 -b -q

sudo wpa_cli -i wlan0 status

<<!
sudo dhclient -r wlan0

sudo dhclient wlan0
!

