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
	exit 0
fi

if [ -n "$RESULT2" ]; then
	echo $RESULT
	echo "The moktest is not trusted by kernel!"
else
	echo "The moktest is already trusted by kernel!"
	exit 0
fi

echo
echo "--------------------"
echo "Check MOK list"
echo "--------------------"

RESULT=$(mokutil --test-key signing_key.x509 2>&1)
RESULT2=$(echo $RESULT | grep "is not enrolled")

if [ -n "$RESULT2" ]; then
	echo $RESULT
	echo "The signing_key.x509 certificate is not in MOK list!"
fi

echo
echo "--------------------"
echo "Enroll MOK"
echo "--------------------"

echo "Run mokutil to import certificate"
mokutil --root-pw --import signing_key.x509
RESULT=$(mokutil --list-new 2>&1) 
RESULT2=$(echo $RESULT | grep 'key 1')

if [ -n "$RESULT2" ]; then
	echo "The certificate is in import list now!"
fi

echo
echo "========================================"
echo "Please run 'reboot' command to reboot system for enroll MOK from shim UI."
echo "After enroll MOK by shim with root password then boot to system, please run plugfest-test.sh again to continue testing."
