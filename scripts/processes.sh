#!/bin/sh
#
# Description: Wrapper for process cpu tracking 
source ./config.ini
source ./functions.sh

eventTime=$1
metricPrefix="$fqdn"

for process in ${processes[@]}
do 
  cpupct=`./scripts/process-cpu $process 3`
  send_stat $metricPrefix.$process.cpu $cpupct "g"
done

exit 0
