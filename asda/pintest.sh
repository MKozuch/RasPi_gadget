#!/usr/bin/env bash

if [ ! -d /sys/class/gpio/gpio$1 ]
then
  echo $1 > /sys/class/gpio/export
fi

echo in > /sys/class/gpio/gpio$1/direction
echo high > /sys/class/gpio/gpio$1/direction

watch -n1 cat /sys/class/gpio/gpio$1/value

echo $1 > /sys/class/gpio/unexport
