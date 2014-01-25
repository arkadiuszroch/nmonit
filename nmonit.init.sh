#!/bin/bash

## Fill in name of program here.
PROG="nmonit"
PROG_PATH="/usr/lib/nmonit"
PROG_ARGS="-d" 
PID_PATH="/var/run/"
NMONIT_PID_FILE="/usr/lib/nmonit/.nmonit.pid"

start() {
    if [ -e "$PID_PATH/$PROG.pid" ]; then
        ## Program is running, exit with error.
        echo "Error! $PROG is currently running!" 1>&2
        exit 1
    else
        ## Change from /dev/null to something like /var/log/$PROG if you want to save output.
        $PROG_PATH/$PROG $PROG_ARGS 2>&1 >/dev/null &
        pid=`ps ax | grep -i 'nmonit -d$'| awk '{print $1}' | head -n 1`
        echo "$PROG started ($pid)"
        echo $pid > "$PID_PATH/$PROG.pid"
    fi
}

stop() {
    if [ -e "$PID_PATH/$PROG.pid" ]; then
        ## Program is running, so stop it
        pid=`cat $PID_PATH/$PROG.pid`
        kill $pid

        rm "$PID_PATH/$PROG.pid"
        rm "$NMONIT_PID_FILE"
        
        echo "$PROG stopped ($pid)"
    else
        ## Program is not running, exit with error.
        echo "Error! $PROG not started!" 1>&2
        exit 1
    fi
}

## Check to see if we are running as root first.
## Found at http://www.cyberciti.biz/tips/shell-root-user-check-script.html
if [ "$(id -u)" != "0" ]; then
    echo "This script must be run as root" 1>&2
    exit 1
fi

case "$1" in
    start)
        start
        exit 0
    ;;
    stop)
        stop
        exit 0
    ;;
    reload|restart|force-reload)
        stop
        start
        exit 0
    ;;
    **)
        echo "Usage: $0 {start|stop|reload}" 1>&2
        exit 1
    ;;
esac