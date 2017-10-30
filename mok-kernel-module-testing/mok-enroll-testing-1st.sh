#!/bin/bash

echo
echo "--------------------"
echo "Attempt load moktest"
echo "--------------------"

# Install moktest RPM if not there
MOKTEST=$(rpm -qa | grep moktest)
if [ -z $MOKTEST ]; then
	KERNEL_VER=`uname -r | cut -d'.' -f 1-2`
        for i in rpm/moktest-*$KERNEL_VER*.rpm; do
		RPMS="$RPMS $i"
		echo $RPMS
        done
	rpm -i $RPMS
fi

RESULT=$(modprobe moktest --allow-unsupported 2>&1)
INVALID_MODULE=$(echo $RESULT | grep "Invalid module format")
INVALID_MODULE2=$(echo $RESULT | grep "Module moktest not found")

if [ -n "$INVALID_MODULE" ] || [ -n "$INVALID_MODULE2" ]; then
	echo $RESULT
	echo "Need recompiler moktest module for testing"
fi

SECUREBOOT=$(hexdump -C /sys/firmware/efi/efivars/SecureBoot-* | grep '01')
if [ -n "$SECUREBOOT" ]; then
        echo "(Secure Boot Enabled)"
	NON_TRUSTED=$(echo $RESULT | grep "Required key not available")
else
        echo "(Secure Boot Disabled)"
	NON_TRUSTED=$(dmesg | grep "moktest: module verification failed")
fi

if [ -n "$NON_TRUSTED" ]; then
	echo $NON_TRUSTED
	echo "The moktest is not trusted by kernel!"
fi

if [ -z "$INVALID_MODULE" ] && [ -z "$INVALID_MODULE2" ] && [ -z "$NON_TRUSTED" ]; then
	echo "The moktest is already trusted by kernel!"
fi

echo
echo "--------------------"
echo "Check MOK list"
echo "--------------------"

RESULT=$(mokutil --test-key /etc/uefi/certs/uefi-plugfest.der 2>&1)
RESULT2=$(echo $RESULT | grep "is not enrolled")

echo $RESULT
if [ -n "$RESULT2" ]; then
	echo "The uefi-plugfest.der certificate is not in MOK list!"
fi

echo
echo "--------------------"
echo "Enroll MOK"
echo "--------------------"

echo "Run mokutil to check new certificate list"
RESULT=$(mokutil --list-new 2>&1) 
RESULT2=$(echo $RESULT | grep 'key 1')

if [ -n "$RESULT2" ]; then
	echo "The certificate is in import list now!"
fi

echo
echo "========================================"
echo "Please run 'reboot' command to reboot system for enroll MOK from shim UI."
echo "After enroll MOK by shim with root password then boot to system, please run \"plugfest-test.sh --stage2\" to continue testing."
