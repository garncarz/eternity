#!/usr/bin/env python2

# minisat_correct.py
# upravi CNF vysledek z programu MiniSat do spravneho formatu
# ("v " pred vyslednymi promennymi)
# argumenty:
#   - filename - soubor s MiniSat CNF vysledkem

import os # pro remove a rename
import sys # pro argumenty a exit

# nacteni argumentu
try:
  filename = sys.argv[1]
except:
  print "pouziti: minisat_correct.py <filename>"
  sys.exit(1)

print "minisat_correct"

input = open(filename, "r")
output = open(filename + ".temp", "w")
line = input.readline()
output.write(line)
line = input.readline()
output.write("v " + line)
input.close()
output.close()

# presun na puvodni soubor
os.remove(filename)
os.rename(filename + ".temp", filename)
