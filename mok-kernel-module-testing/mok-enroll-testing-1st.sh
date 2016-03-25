#!/bin/bash

echo
echo "--------------------"
echo "Attempt load moktest"
echo "--------------------"

# Install acpica RPM if not there
MOKTEST=$(modinfo moktest)
if ! [ -n "$MOKTEST" ]; then
        rpm -i rpm/moktest-kmp*.rpm
        echo "Install moktest-kmp RPM"
        echo ""
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

if [ -z "$INVALID_MODULE" ] && [ -z "$INVALID_MODULE2" ] && [-z "$NON_TRUSTED"]; then
	echo "The moktest is already trusted by kernel!"
fi

echo
echo "--------------------"
echo "Check MOK list"
echo "--------------------"

RESULT=$(mokutil --test-key cert/uefi-plugfest.der 2>&1)
RESULT2=$(echo $RESULT | grep "is not enrolled")

echo $RESULT
if [ -n "$RESULT2" ]; then
	echo "The uefi-plugfest.der certificate is not in MOK list!"
fi

echo
echo "--------------------"
echo "Enroll MOK"
echo "--------------------"

echo "Run mokutil to import certificate"
mokutil --root-pw --import cert/uefi-plugfest.der
RESULT=$(mokutil --list-new 2>&1) 
RESULT2=$(echo $RESULT | grep 'key 1')

if [ -n "$RESULT2" ]; then
	echo "The certificate is in import list now!"
fi

echo
echo "========================================"
echo "Please run 'reboot' command to reboot system for enroll MOK from shim UI."
echo "After enroll MOK by shim with root password then boot to system, please run \"plugfest-test.sh --stage2\" to continue testing."
