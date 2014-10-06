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
else
	echo "The uefi-plugfest.der certificate is enrolled success!"
fi
