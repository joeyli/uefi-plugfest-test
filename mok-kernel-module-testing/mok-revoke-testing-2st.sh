#!/bin/bash

echo
echo "--------------------"
echo "Check MOK list"
echo "--------------------"

RESULT=$(mokutil --test-key cert/uefi-plugfest.der 2>&1)
RESULT2=$(echo $RESULT | grep "is not enrolled")

echo $RESULT
if [ -n "$RESULT2" ]; then
	echo "The uefi-plugfest.der certificate is not in MOK list! Revoke success!"
else
	echo "The uefi-plugfest.der certificate is enrolled success!"
fi
