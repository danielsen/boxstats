#
# Description: Helper functions for statitics submissions
source ./config.ini

host="${CARBON_HOST:-$carbon_host}"
port="${CARBON_PORT:-$carbon_port}"

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
  stamp=$3

  printf "$namespace.$metric $value $stamp" >&3
  #printf "$namespace.$metric $value $stamp\n"
}

