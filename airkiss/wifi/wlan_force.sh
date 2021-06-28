#!/bin/bash
killall wpa_supplicant
WIFI_DEV="wlan0"
#ifconfig wlan0 up
wpa_supplicant -i${WIFI_DEV} -Dnl80211 -c /etc/wpa_supplicant_force.conf &
sleep 2
/etc/init.d/dhcpcd restart
#udhcpc -b -i ${WIFI_DEV}
#echo "nameserver 8.8.8.8" > /etc/resolv.conf
echo 4 >/proc/sys/kernel/printk
#sleep 1
exit 0

