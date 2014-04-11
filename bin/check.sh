#!/bin/bash

# zobrazime original plochy
display.sh area_orig.area &

# a kazde reseni plochy
for file in *
do
  if [[ -f $file && $file =~ "result.area" ]]; then
    display.sh $file &
  fi
done
