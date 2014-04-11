#!/usr/bin/python

# cnf2sat.py
# prevede CNF zapis vysledku do Simplify SAT zapisu
# argumenty:
#   - input_file - soubor s vysledkem v CNF formatu
#   - output_file - soubor se SAT zapisem, ktery bude vytvoren

import sys # pro argumenty a exit


# nacteni argumentu
try:
  input_filename = sys.argv[1]
  output_filename = sys.argv[2]
except:
  print "pouziti: cnf2sat.py <input_file> <output_file>"
  sys.exit(1)

print "cnf2sat..."

# preklady promennych (a mezi-promennych)
translation = {}
trans_file = open("cnf_translation.txt", "r")
lines = trans_file.readlines()
trans_file.close()
# na lichem radku je ulozeno CNF cislo promenne a na sudem SAT nazev
for i in range(len(lines) / 2):
  key = lines[i * 2][:-1]
  value = lines[i * 2 + 1]
  translation[key] = value


input = open(input_filename, "r")
output = open(output_filename, "w")

# nalezeni reseni v inputu
for line in input:
  if not line.startswith("v "):
    continue
  line = line[2:].strip() # orezani
  result = line.split() # rozsekani na promenne
  # pro kazdou promennou
  for var in result:
    # nepravdive hodnoty a ukoncovaci nula nas nezajimaji
    if int(var) <= 0:
      continue
    # mezi-promenne nas take nezajimaji
    if translation[var].startswith("tmp"):
      continue
    # zapis prekladu do outputu
    output.write(translation[var])
  
  
input.close()
output.close()

print "cnf2sat OK"
