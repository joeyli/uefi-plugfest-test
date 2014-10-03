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

RESULT=$(mokutil --test-key signing_key.x509 2>&1)
RESULT2=$(echo $RESULT | grep "is not enrolled")

echo $RESULT
if [ -n "$RESULT2" ]; then
	echo "The signing_key.x509 certificate is not in MOK list! Revoke success!"
else
	echo "The signing_key.x509 certificate is enrolled success!"
fi
