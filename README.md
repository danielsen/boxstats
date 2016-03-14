### Overview
`boxstats.sh` is a simple, configurable set of tools for monitoring a hosts
health an recording that information in any time series database via statsd.

### Installation and Use
Installation is manually managed at this time. The simplest way is to clone the
respoitory into a directory where you would like the program to live and add
a cronjob of for execution.

    $ mkdir /opt/boxtstats
    $ cd /opt/boxstats
    $ git clone git://github.com/danielsen/boxstats
    $ make

    (edit config.ini and crontab)

