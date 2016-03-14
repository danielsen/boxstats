#!/bin/sh
#
# Description : Submit machine stats to graphite 
#

config_file=$1

if [[ -z $config_file ]]
then
  source ./config.ini
else
  source $config_file
fi

cd $runtime_path 
source ./functions.sh

eventTime=`date +'%s'`
printf "Compiling stats for $eventTime\n"

connect
for script in `ls ./$script_path/*.sh`
do 
  $script $eventTime

  if [[ $? -gt 0 ]]
  then
    printf "Error running $script\n"
  fi
done
disconnect

exit 0
