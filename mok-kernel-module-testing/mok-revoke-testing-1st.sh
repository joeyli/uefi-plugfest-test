#!/bin/bash

echo
echo "--------------------"
echo "Revoke MOK"
echo "--------------------"

echo "Removing moktest-kmp and certificate"
rpm -e moktest
rpm -e moktest-kmp-default
RESULT=$(mokutil --list-delete 2>&1)
RESULT2=$(echo $RESULT | grep 'key 1')

if [ -n "$RESULT2" ]; then
	echo "The uefi-plugfest.der certificate is in delete queue!"
fi

echo
echo "========================================"
echo "Please run 'reboot' command to reboot system for delete MOK from shim UI."
