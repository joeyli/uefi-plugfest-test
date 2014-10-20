#!/bin/bash

osc signkey --sslcert > uefi-plugfest.pem
openssl x509 -inform PEM -in uefi-plugfest.pem -outform DER -out uefi-plugfest.der
openssl x509 -in uefi-plugfest.der -inform DER -noout -text
