#!/bin/bash

# pripravi potrebne veci pro Eternity projekt:
#   - stahne a nainstaluje externi SAT solvery
#   - overi pritomnost dalsich potrebnych externich programu
#   - overi nastaveni cest k projektu
# bunzip2 a chmod vyzaduji sudo

# specifikace cest a nazvu souboru ke stahovani a rozbalovani
Simplify_zip="http://freshmeat.net/redir/simplifyprover/72120/url_zip/Simplify-1.5.5-13-06-07-binary.zip"
zChaff_zip="http://www.ee.princeton.edu/~chaff/zchaff/zchaff.2007.3.12.zip"

if test ""$1 == "cygwin"
then
  echo Cygwin...
  Simplify_program="Simplify/Simplify-1.5.4.exe"
  Spear_bz2="http://www.domagoj-babic.com/uploads/Spear/Download/Spear_i586_32_Cygwin.bz2"
  Spear_program="Spear_i586_32_Cygwin"
  MiniSat_program="http://minisat.se/downloads/MiniSat_v1.14_cygwin"
elif test ""$1 == "linux"
then
  echo Linux...
  Simplify_program="Simplify/Simplify-1.5.4.linux"
  Spear_bz2="http://www.domagoj-babic.com/uploads/Spear/Download/Spear_i586_32_Linux.bz2"
  Spear_program="Spear_i586_32_Linux"
  MiniSat_program="http://minisat.se/downloads/MiniSat_v1.14_linux"
else
  echo "Pouziti: install.sh cygwin|linux [nosat]"
  exit 1
fi


if test ""$2 != "nosat"
then
  test -d download || mkdir download
  test -d programs || mkdir programs
  test -d tmp || mkdir tmp
  
  # obecny vzor: pokud neni nainstalovan dany program...
  if test ! -e programs/simplify
  then
    # ziskani jmena archivu
    Simplify_zip_filename=${Simplify_zip##*/}
    # stahnuti archivu do download adresare
    test -e download/$Simplify_zip_filename || wget $Simplify_zip -P download
    # ziskani jmena programu
    Simplify_program_filename=${Simplify_program##*/}
    # rozbaleni programu
    unzip -j download/$Simplify_zip_filename $Simplify_program -d programs
    # presun programu do adresare programs a prejmenovani na jednoduche jmeno
    mv programs/$Simplify_program_filename programs/simplify
    # uprava prav, muze se hodit, ale zrejme neni nutna
    chmod u=rwx,og=rx programs/simplify
  fi
  
  if test ! -e programs/spear
  then
    Spear_bz2_filename=${Spear_bz2##*/}
    test -e download/$Spear_bz2_filename || wget $Spear_bz2 -P download
    Spear_program_filename=${Spear_program##*/}
    bunzip2 -k -d download/$Spear_bz2_filename
    mv download/$Spear_program_filename programs/spear
    chmod u=rwx,og=rx programs/spear
  fi
  
  if test ! -e programs/minisat
  then
    # MiniSat je rovnou spustitelnym programem
    MiniSat_program_filename=${MiniSat_program##*/}
    wget $MiniSat_program -P programs
    mv programs/$MiniSat_program_filename programs/minisat
    chmod u=rwx,og=rx programs/minisat
  fi
  
  if test ! -e programs/zchaff
  then
    zChaff_zip_filename=${zChaff_zip##*/}
    test -e download/$zChaff_zip_filename || wget $zChaff_zip -P download
    unzip download/$zChaff_zip_filename -d tmp
    # zChaff je treba zkompilovat
    cd tmp/zchaff
    make
    cd ../..
    mv tmp/zchaff/zchaff programs/zchaff
    chmod u=rwx,og=rx programs/zchaff
  fi

fi  # sat


# kontrola existence vyuzivanych externich programu
external=""
for program in make g++ python javac jar java doxygen pdflatex wget unzip bunzip2 tar gzip; do
  # je dany program volatelny?
  if [[ ! `which $program 2> /dev/null` ]]; then
    # ne => je pridan do seznamu k nainstalovani
    external+="$program\n"
  fi
done
# v pripade neprazdneho seznamu je seznam vypsan
if [[ $external != "" ]]; then
  echo Chybejici externi programy:
  echo -e $external
fi


# podobny postup jako u ext. programu je uplatnen u kontroly volatelnych cest
manually=""
# obsahuje dana systemova promenna danou hodnotu?
if [[ ! $ETERNITY_BIN =~ $PWD/bin ]]; then
  manually+="export ETERNITY_BIN=$PWD/bin\n"
fi
if [[ ! $ETERNITY_PROGRAMS =~ $PWD/programs ]]; then
  manually+="export ETERNITY_PROGRAMS=$PWD/programs\n"
fi
if [[ ! $PATH =~ $PWD/bin ]] || [[ ! $PATH =~ $PWD/programs ]]; then
  manually+="export PATH=\$PATH:\$ETERNITY_BIN:\$ETERNITY_PROGRAMS\n"
fi
if [[ $manually != "" ]]; then
  echo Nutno provest rucne:
  echo -e $manually
fi
