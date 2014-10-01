#!/bin/bash

echo
echo "--------------------"
echo "Attempt load moktest"
echo "--------------------"

RESULT=$(insmod moktest.ko 2>&1)
RESULT2=$(echo $RESULT | grep "Required key not available")

if [ -n "$RESULT2" ]; then
	echo $RESULT
	echo "The moktest is not trusted by kernel!"
	exit 0
else
	echo "The moktest is alrady trusted by kernel!"
fi

echo
echo "--------------------"
echo "Check MOK list"
echo "--------------------"

RESULT=$(mokutil --test-key signing_key.x509 2>&1)
RESULT2=$(echo $RESULT | grep "is not enrolled")

if [ ! -n "$RESULT2" ]; then
	echo $RESULT
	echo "The signing_key.x509 certificate is enrolled success!"
fi

echo
echo "--------------------"
echo "Revoke MOK"
echo "--------------------"

echo "Run mokutil to revoke certificate"
mokutil --root-pw --delete signing_key.x509 > /dev/null
RESULT=$(mokutil --root-pw --delete signing_key.x509 2>&1)
RESULT2=$(echo $RESULT | grep "Skip signing_key.x509")

if [ -n "$RESULT2" ]; then
	echo "The signing_key.x509 certificate is in delete queue!"
fi

echo
echo "========================================"
echo "Please run 'reboot' command to reboot system for delete MOK from shim UI."
