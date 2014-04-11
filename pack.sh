#!/bin/bash

# vycisti projekt a zabali jej do ../eternity-YYYY-mm-dd.tar.gz

make clear

cd ..
date=`date +-%Y-%m-%d`
tar -c -f eternity$date.tar eternity
gzip eternity$date.tar -9
