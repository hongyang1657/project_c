#!/bin/bash

DATE=$(date)

echo "${DATE}"
/usr/bin/startapp &>/data/found/debuglog/${DATE}.log 2>&1
