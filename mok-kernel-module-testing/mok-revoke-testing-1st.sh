#!/bin/bash

echo
echo "--------------------"
echo "Revoke MOK"
echo "--------------------"

echo "Run mokutil to revoke certificate"
mokutil --root-pw --delete cert/uefi-plugfest.der > /dev/null
RESULT=$(mokutil --root-pw --delete cert/uefi-plugfest.der 2>&1)
RESULT2=$(echo $RESULT | grep "uefi-plugfest.der")

if [ -n "$RESULT2" ]; then
	echo "The uefi-plugfest.der certificate is in delete queue!"
fi

echo
echo "========================================"
echo "Please run 'reboot' command to reboot system for delete MOK from shim UI."
