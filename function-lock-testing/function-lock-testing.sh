#!/bin/bash

check_prereqs()
{
        local msg="skip all tests:"

        if [ $UID != 0 ]; then
                echo $msg must be run as root >&2
                exit 0
        fi
}

run_test()
{
        local test="$1"

        echo "--------------------"
        echo "running $test"
        echo "--------------------"

        if [ "$(type -t $test)" = 'function' ]; then
                ( $test )
        else
                ( ./$test )
        fi

        if [ $? -ne 0 ]; then
                echo "  [FAIL]"
                rc=1
        else
                echo "  [PASS]"
        fi
}

test_setpci()
{
	RESULT=$(/sbin/setpci -s 00:00.0 0x50.B=0x40 2>&1)
	echo $RESULT
	RESULT2=$(echo $RESULT | grep 'Operation not permitted')

	if [ -n "$RESULT2" ]; then
		exit 0
	else
		echo "setpci didn't lock down"
		exit 1
	fi
}

test_s4()
{
	RESULT=$(/bin/cat /sys/power/state | grep disk)	
	echo $(/bin/cat /sys/power/state)

	if [ -n "$RESULT" ]; then
		echo "S4 didn't lock down"
		exit 1	
	fi
}

test_kdump()
{
	if [ ! -f /sbin/rckdump ]; then
		echo "doesn't have rckdump"
		exit 0
	fi

	RESULT=$(/sbin/rckdump start)
	echo $RESULT
	RESULT1=$(echo $RESULT | grep 'Operation not permitted')

	if [ -n "$RESULT1" ]; then
		exit 0
	else
		echo "kexec didn't lock down"
		exit 1
	fi
}

test_ioport()
{
	./keyboard-set-leds
	RESULT=$(dmesg | grep 'traps: keyboard-set-le')

	if [ -n "$RESULT" ]; then
		echo "I/O port locked down"
		exit 0
	else
		echo "I/O port didn't lock down"
		exit 1
	fi
}

check_prereqs

rc=0

run_test test_setpci
run_test test_s4
run_test test_kdump
run_test test_ioport

exit $rc
