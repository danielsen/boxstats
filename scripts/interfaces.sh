#!/bin/sh
#
# Description: Functions to report on machine memory
source ./config.ini
source ./functions.sh

eventTime=$1
metricPrefix="$fqdn.interfaces"

metrics=("rxBytes" "rxPackets" "rxErrors" "rxDrops" "rxFifo" "rxFrame"
 "rxCompressed" "rxMulticast" "txBytes" "txPackets" "txErrors" "txDrops"
 "txFifo" "txColls" "txCarrier" "txCompressed")

while read line
do
  echo $line | grep -qs ":"
  if [[ $? -gt 0 ]]
  then
    continue
  fi

  echo $line | grep -vqs "lo:"
  if [[ $? -gt 0 ]]
  then
    continue
  fi

  interface=`echo $line| cut -d':' -f1`
  interfaceData=`echo $line| cut -d':' -f2`
  rawArray=($interfaceData)

  j=0
  for metric in ${metrics[@]}
  do
    send_stat $metricPrefix.$interface.$metric ${rawArray[j]} $eventTime
    j=`expr $j + 1`
  done
done < /proc/net/dev 

exit 0
