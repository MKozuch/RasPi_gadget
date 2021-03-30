#!/usr/bin/env bash

gpio -g mode 14 up
gpio -g mode 15 up

function write_report {
    echo -ne $1 > /dev/hidg0
}


# x1e 1
# x27 0
# x66 powr

while :
do
  p14=`gpio -g read 14`
  p15=`gpio -g read 15`

if [ $p14 -eq 0 ]; then
  write_report "\0\0\x1e\0\0\0\0\0"
  write_report "\0\0\0\0\0\0\0\0"
  #echo -e 0x4
  echo p14
fi

if [ $p15 -eq 0 ]; then
  write_report "\0\0\x27\0\0\0\0\0"
  write_report "\0\0\0\0\0\0\0\0"
  #echo -e 0x5
fi

  sleep 0.01
done


