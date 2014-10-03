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
else
	echo "The moktest is alrady trusted by kernel!"
fi

echo
echo "--------------------"
echo "Check MOK list"
echo "--------------------"

RESULT=$(mokutil --test-key signing_key.x509 2>&1)
RESULT2=$(echo $RESULT | grep "is not enrolled")

echo $RESULT
if [ -n "$RESULT2" ]; then
	echo "The signing_key.x509 certificate is not in MOK list!"
else
	echo "The signing_key.x509 certificate is enrolled success!"
fi
