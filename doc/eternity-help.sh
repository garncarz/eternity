#!/bin/bash

# vytvorime ../tmp/eternity.txt s help vypisem hlavniho programu
# (pokud jiz neexistuje)

if [[ ! -f ../tmp/eternity.txt ]]; then
  echo "eternity => ../tmp/eternity.txt"
  eternity >> ../tmp/eternity.txt
fi
