#!/bin/sh
#
# Description: Wrapper for docker process cpu tracking 
source ./config.ini
source ./docker.ini
source ./functions.sh

eventTime=$1
metricPrefix="$fqdn"

for image in ${images[@]}
do 
  cpupct=0
  containers=$(docker ps -q --filter "image=$image")
  for container in ${containers[@]}
  do
    cpid=`docker inspect $container | grep -oE "\"Pid\": [0-9]{0,6}" | \
      cut -d' ' -f2` 
    ccpu=`./scripts/process-cpu -p $cpid -i $interval`
    cpupct=`echo "$cpupct + $ccpu" | bc`
  done
  send_stat $metricPrefix.$image $cpupct "g"
done

exit 0
