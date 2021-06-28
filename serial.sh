#!/bin/bash

((num=0xff))
while [ 1 ]
do
	echo $num > /dev/ttyUSB0
	sleep 1
done
