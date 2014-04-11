#!/bin/bash

# spusti zobrazovac plochy, nepovinne parametry:
#   1. soubor plochy
#   2. soubor obrazku k vytvoreni podle dane plochy

# ziskani spravne cesty k JAR souboru a souborum parametru
if [[ `which cygpath 2> /dev/null` ]]; then
  # nutne prevest Cygwin Linux cesty na Windows cesty
  jar=`cygpath -wp $ETERNITY_BIN/areaDisplayer.jar`
  if [[ ""$1 != "" ]]; then
    prvni=`cygpath -wp $1`
  fi
  if [[ ""$2 != "" ]]; then
    druhy=`cygpath -wp $2`
  fi
else
  jar="$ETERNITY_BIN/areaDisplayer.jar"
  prvni=$1
  druhy=$2
fi

# spusteni zobrazovace s pripadnymi parametry
java -jar $jar $prvni $druhy
