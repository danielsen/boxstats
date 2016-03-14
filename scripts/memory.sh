#!/bin/sh
#
# Description: Functions to report on machine memory
source ./config.ini
source ./functions.sh

eventTime=$1
metricPrefix="$fqdn.memory"

getMetric(){
  grep -E "^$1" /proc/meminfo | grep -oE "[[:digit:]]{0,10}"
}

memTotal=$(getMetric "MemTotal")
memFree=$(getMetric "MemFree")
memBuffers=$(getMetric "Buffers")
memCached=$(getMetric "Cached")
memSwapTotal=$(getMetric "SwapTotal")
memSwapFree=$(getMetric "SwapFree")

memSwapUsed=`expr $memSwapTotal - $memSwapFree`
memUsed=`expr $memTotal - $memFree`

metrics=(
  "total:$memTotal" 
  "free:$memFree"
  "buffers:$memBuffers"
  "cached:$memCached"
  "swapTotal:$memSwapTotal"
  "swapFree:$memSwapFree"
  "swapUsed:$memSwapUsed"
  "used:$memUsed"
)

for metric in ${metrics[@]} 
do
  key=`echo $metric | cut -d':' -f1`
  value=`echo $metric | cut -d':' -f2`
  bytes=`expr $value \* 1024`
  send_stat $metricPrefix.$key $bytes "g"
done

exit 0
