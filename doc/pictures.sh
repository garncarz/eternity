#!/bin/bash

# pro kazdy .area soubor v adresari
for file in *.area
do
  # pokud pro nej neexistuje obrazek v adresari ../tmp/
  if [[ ! -f "../tmp/"${file%area}"png" ]]; then
    echo $file "=>" "../tmp/"${file%area}"png"
    # tak spustime jeho prevod na obrazek
    display.sh $file "../tmp/"${file%area}"png"
  fi
done
