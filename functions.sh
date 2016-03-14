#
# Description: Helper functions for statitics submissions
source ./config.ini

host="${CARBON_HOST:-$statsd_host}"
port="${CARBON_PORT:-$statsd_port}"

connect() {
  exec 3<> /dev/udp/$host/$port
}

disconnect() {
  exec 3<&-
  exec 3>&-
}

send_stat() {
  metric=$1
  value=$2
  mtype=$3

  printf "$namespace.$metric:$value|$mtype" >&3
}

