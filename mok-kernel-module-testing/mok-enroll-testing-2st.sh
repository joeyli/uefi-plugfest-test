#!/bin/bash

echo
echo "--------------------"
echo "Attempt load moktest"
echo "--------------------"

RESULT=$(modprobe moktest --allow-unsupported 2>&1)
RESULT2=$(echo $RESULT | grep "Required key not available")
RESULT3=$(echo $RESULT | grep "Invalid module format")
RESULT4=$(echo $RESULT | grep "Module moktest not found")

if [ -n "$RESULT4" ] || [ -n "$RESULT3" ]; then
        echo $RESULT
        echo "Need recompiler moktest module for testing"
fi

if [ -n "$RESULT2" ]; then
	echo $RESULT
	echo "The moktest is not trusted by kernel!"
fi

if [ -z "$RESULT4" ] && [ -z "$RESULT3" ] && [ -z "$RESULT2" ]; then
	echo "The moktest is trusted by kernel!"
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
else
	echo "The uefi-plugfest.der certificate is enrolled success!"
fi
