#!/bin/sh
#
# Description: Functions to report on machine memory
source ./config.ini
source ./functions.sh

eventTime=$1
metricPrefix="$fqdn.load_average"

rawLoad=`cat /proc/loadavg`

one=`echo $rawLoad | cut -d' ' -f1`
five=`echo $rawLoad | cut -d' ' -f2`
fifteen=`echo $rawLoad | cut -d' ' -f3`

send_stat $metricPrefix.short $one "g"
send_stat $metricPrefix.medium $five "g"
send_stat $metricPrefix.long $fifteen "g"

exit 0
