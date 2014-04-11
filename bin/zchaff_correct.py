#!/usr/bin/python

# zchaff_correct.py
# upravi CNF vysledek z programu Z-Chaff do spravneho formatu
# ("v " pred vyslednymi promennymi)
# argumenty:
#   - filename - soubor s Z-Chaff CNF vysledkem

import os # pro remove a rename
import sys # pro argumenty a exit

# nacteni argumentu
try:
  filename = sys.argv[1]
except:
  print "pouziti: zchaff_correct.py <filename>" 
  sys.exit(1)

print "zchaff_correct"

input = open(filename, "r")
output = open(filename + ".temp", "w")
for i in range(5):
  input.readline()
line = input.readline()
line = line[:line.find("Random")]
output.write("v " + line)
input.close()
output.close()

# presun na puvodni soubor
os.remove(filename)
os.rename(filename + ".temp", filename)
