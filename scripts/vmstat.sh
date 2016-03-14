#!/bin/sh
#
# Description: Functions to report on machine memory
source ./config.ini
source ./functions.sh

eventTime=$1
metricPrefix="$fqdn.vmstat"

metrics=("runningProcs" "blockedProcs" "swpdMem" "freeMem" "buffMem" "cacheMem"
  "swapIn" "swapOut" "blocksIn" "blocksOut" "interrupts" "contextSwitches"
  "userCPU" "systemCPU" "idleCPU" "waitingCPU")

vmstatData=`vmstat 1 2 | tail -1`
rawArray=($vmstatData)

j=0
for metric in ${metrics[@]}
do
  send_stat $metricPrefix.$metric ${rawArray[j]} $eventTime
  j=`expr $j + 1`
done

exit 0
