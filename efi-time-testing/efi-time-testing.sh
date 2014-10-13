#!/bin/bash

efivarfs_mount=/sys/firmware/efi/efivars
test_guid=210be57c-9849-4fc7-a635-e6382d1aec27

check_prereqs()
{
	local msg="skip all tests:"

	if [ $UID != 0 ]; then
		echo $msg must be run as root >&2
		exit 0
	fi

	# Install rtc-efi if not there
	RTC_EFI=$(modinfo rtc-efi || echo "")
	if [ -z "$RTC_EFI" ]; then
		echo "Install rtc-efi-kmp RPM"
		rpm -Uvh rpm/rtc-efi-kmp*.rpm
	fi

	modprobe rtc-efi-platform --allow-unsupported
	modprobe rtc-efi --allow-unsupported

	RTC_EFI=$(lsmod | grep 'rtc_efi')
	if [ -n "$RTC_EFI" ]; then
		echo "modporbed rtc-efi driver for testing"
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
	echo ""
}

test_rtc_efi_tz()
{
	RTC_EFI=$(lsmod | grep 'rtc_efi')
	if [ -n "$RTC_EFI" ]; then
		./rtc-tz-test
	else
		echo "Skipped RTC-EFI testing because rtc_efi didn't load"
		exit 1
	fi
	echo ""
}

check_prereqs

rc=0

run_test test_rtc_efi_tz

exit $rc
