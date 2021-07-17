#!/bin/bash

e=100
while [[ $e -lt 10000000 ]]; do
    export ENE=$e
    echo $ENE
    envsubst '${ENE}' < macros/gamma_scan_template.mac > macros/gamma_scan.mac
    ./pixelSim macros/gamma_scan.mac "gamma_scan/${ENE}.root"
    e=`bc <<< "($e * 1.2 + 0.5)/1"`
done
