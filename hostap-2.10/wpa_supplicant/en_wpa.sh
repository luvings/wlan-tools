#!/bin/bash

sudo rfkill unblock all

sudo ifconfig wlan0 up

sudo killall -9 wpa_supplicant

sudo ./wpa_supplicant -iwlan0 -Dnl80211 -c ./wpa_supplicant.conf.mt76 -ddd #-B

sudo udhcpc -b -i wlan0 -q

sudo wpa_cli -i wlan0 status

<<!
sudo dhclient -r wlan0

sudo dhclient wlan0
!
