#!/bin/bash

echo
echo "--------------------"
echo "Attempt load moktest"
echo "--------------------"

RESULT=$(insmod moktest.ko 2>&1)
RESULT2=$(echo $RESULT | grep "Required key not available")
RESULT3=$(echo $RESULT | grep "Invalid module format")

if [ -n "$RESULT3" ]; then
        echo $RESULT
        echo "Need recompiler moktest module for testing"
fi

if [ -n "$RESULT2" ]; then
	echo $RESULT
	echo "The moktest is not trusted by kernel!"
	exit 0
else
	echo "The moktest is already trusted by kernel!"
fi

echo
echo "--------------------"
echo "Check MOK list"
echo "--------------------"

RESULT=$(mokutil --test-key cert/uefi-plugfest.der 2>&1)
RESULT2=$(echo $RESULT | grep "is not enrolled")

if [ ! -n "$RESULT2" ]; then
	echo $RESULT
	echo "The uefi-plugfest.der certificate is enrolled success!"
fi

echo
echo "--------------------"
echo "Revoke MOK"
echo "--------------------"

echo "Run mokutil to revoke certificate"
mokutil --root-pw --delete cert/uefi-plugfest.der > /dev/null
RESULT=$(mokutil --root-pw --delete cert/uefi-plugfest.der 2>&1)
RESULT2=$(echo $RESULT | grep "Skip uefi-plugfest.der")

if [ -n "$RESULT2" ]; then
	echo "The uefi-plugfest.der certificate is in delete queue!"
fi

echo
echo "========================================"
echo "Please run 'reboot' command to reboot system for delete MOK from shim UI."
