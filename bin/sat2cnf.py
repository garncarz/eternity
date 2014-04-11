#!/usr/bin/python

# sat2cnf.py
# prevede Simplify SAT zapis do CNF zapisu
# argumenty:
#   - input_file - soubor se Simplify SAT zapisem
#   - output_file - soubor s CNF zapisem, ktery bude vytvoren

import os # pro remove
import sys # pro argumenty a exit


# vraci promennou variable prelozenou do ciselneho formatu
def translate(variable):
  # hash mapa prekladu
  global translation
  # nacteni jiz existujiciho prekladu
  translated = translation.get(variable, None)
  # neexistujici pripad:
  if translated == None:
    # nove cislo...
    global number
    # ...je prekladem
    translated = number
    # pridani do hash mapy
    translation[variable] = translated
    # pridani prekladu do prekladaciho souboru
    global trans_file
    trans_file.write(str(translated) + "\n" + str(variable) + "\n")
    # dalsi cislo
    number = number + 1
  # return variable # bez prekladu
  return translated # s prekladem


# vraci nove ciselne jmeno pro mezi-promennou
def newName():
  # nove cislo
  global number
  # pridani prekladu do prekladaciho souboru
  global trans_file
  trans_file.write(str(number) + "\ntmp" + str(number) + "\n")
  # dalsi cislo
  number = number + 1
  # return "tmp" + str(number - 1) # bez prekladu
  return number - 1 # s prekladem


# tisk CNF klauzule string
def cnf_print(string):
  global output
  # pokud jsou radky delsi nez 80 znaku, rozsekame je:
  while len(string) > 80:
    border = string.rfind(" ", 0, 80)
    if border > 0:
      ln = string[:border]
    else:
      ln = string
    output.write(ln + "\n")
    string = string[len(ln):]
  output.write(string + " 0\n")
  global line
  # pocet radku/klauzuli
  line = line + 1


# trida pro operator (mezi-promennou) ci promennou vyroku
class Node:
  def __init__(self):
    self.children = []
  
  def setOperator(self, operator):
    self.operator = operator

  def getOperator(self):
    return self.operator
  
  # jmeno promenne ci mezi-promenne
  def setName(self, name):
    self.name = name
  
  # jmeno promenne ci mezi-promenne
  def getName(self):
    # do jmena zahrneme i pripadnou negaci
    if self.operator == "negvar":
      return "-" + str(self.name)
    else:
      return str(self.name)

  # negovane jmeno promenne ci mezi-promenne
  def getNegName(self):
    # pripadnou negaci promenne negujeme
    if self.operator == "negvar":
      return str(self.name)
    else:
      return "-" + str(self.name)
  
  def addChild(self, child):
    self.children.append(child)
  
  def getChildren(self):
    return self.children
  
  # vypisuje hierarchii na std. vystup (pro kontrolu)
  def write(self, depth):
    # odsazeni podle hloubky zanoreni
    s = ""
    for i in range(depth):
      s = s + " "
    # je uzel jiz listem?
    if self.operator.startswith("var"):
      print s + str(self.name)
      return
    # pokud ne, budeme se zanorovat dale
    print s + self.operator
    for child in self.children:
      child.write(depth + 1)

  # vytvari hierarchicky CNF zapis
  def cnf_write(self):
    # konjunkce
    if self.operator == "&":
      s = ""
      for child in self.children:
        # potomek nebo negace rodice
        cnf_print(child.getName() + " " + self.getNegName())
        # pridani negace potomka
        s = s + child.getNegName() + " "
      # negace potomku nebo rodic
      cnf_print(s + self.getName())
      # rekurzivni CNF zapis
      for child in self.children:
        child.cnf_write()
    # disjunkce
    elif self.operator == "|":
      s = ""
      for child in self.children:
        # negace potomka nebo rodic
        cnf_print(child.getNegName() + " " + self.getName())
        # pridani potomka
        s = s + child.getName() + " "
      # potomci nebo negace rodice
      cnf_print(s + self.getNegName())
      # rekurzivni CNF zapis
      for child in self.children:
        child.cnf_write()


# trida pro obsah souboru se SAT zapisem
class Content:
  # otevre a nacte zacatek souboru
  def __init__(self, input_filename):
    self.input = open(input_filename, "r")
    self.string = ""
    self.shorten(0)

  def startswith(self, char):
    return self.string.startswith(char)
  
  def find(self, char):
    return self.string.find(char)
  
  # vrati zacatek aktualniho obsahu
  def preview(self):
    return self.string[:20]
  
  def get(self):
    return self.string
  
  # oseka (zepredu) obsah a v pripade potreby (kratke delky) jej doplni
  # ze souboru
  def shorten(self, offset):
    self.string = self.string[offset:].strip()
    while len(self.string) < 200:
      line = self.input.readline()
      if line == "": # konec souboru
        break
      self.string = self.string + line.lower()
      self.string = self.string.replace("\n", "").replace("\r", "")
  
  # uzavre soubor
  def close(self):
    self.input.close()


# nalezne potomky danemu uzlu;
# neg - pocita s moznym propadem negace
def findChildren(node, neg):
  global content
  # osekani mezer ze stran obsahu
  content.shorten(0)
  
  # jiz neni dalsi dite
  if content.startswith(")"):
    return 0
  
  # novy operator
  elif content.startswith("("):
    content.shorten(1) # osekani
    
    # budeme vytvaret ve strukture formule novou mezipromennou?
    # (ano pri AND nebo OR)
    newVar = False
    
    # zpracovani typu operatoru
    if content.startswith("and"):
      # zarizeni se podle negace
      if neg == False:
        node.setOperator("&")
      else:
        node.setOperator("|")
      content.shorten(3)
      newVar = True
    elif content.startswith("or"):
      # zarizeni se podle negace
      if neg == False:
        node.setOperator("|")
      else:
        node.setOperator("&")
      content.shorten(2)
      newVar = True
    elif content.startswith("not"):
      content.shorten(3)
      # zmena negace
      if neg == False:
        findChildren(node, True)
      else:
        findChildren(node, False)
    else:
      raise Exception("chybny operator: " + content.preview())

    # mame opravdu vytvorit novou mezipromennou?
    if newVar == True:
      # operatory AND a OR jsou transformovany v novou mezi-promennou
      node.setName(newName())

      # rekurzivni hledani potomku daneho uzlu
      while 1:
        child = Node()
        if findChildren(child, neg) == 1:
          node.addChild(child)
        else:
          break
    
    # osekani uzaviraci zavorky    
    end = content.find(")")
    if end == -1:
      raise Exception("chybne zavorkovani: " + content.preview())
    content.shorten(end + 1)
  
  # promenna
  else:
    # nalezeni konce promenne
    space = content.find(" ")
    end = content.find(")")
    if (end < space) or (space == -1):
      space = end
    if space == -1:
      raise Exception("chybne zavorkovani: " + content.preview())
    
    # ulozeni udaju do uzlu a osekani obsahu
    if neg == False:  # je promenna negovana, ci ne?
      node.setOperator("var")
    else:
      node.setOperator("negvar")
    variable = content.get()[:space]
    node.setName(translate(variable))
    content.shorten(space)
  
  # hledani potomku ok  
  return 1


# nacteni argumentu
try:
  input_filename = sys.argv[1]
  output_filename = sys.argv[2]
except:
  print "pouziti: sat2cnf.py <input_file> <output_file>" 
  sys.exit(1)

print "sat2cnf... findChildren(root)"

# pocet radku CNF zapisu
line = 0

# pocet promennych a mezi-promennych CNF zapisu
number = 1

# preklady promennych
translation = {}

# soubor pro ukladani prekladu
trans_file = open("cnf_translation.txt", "w")

# obsah souboru se SAT zapisem
content = Content(input_filename)

# korenovy uzel a jeho nalezeni  
root = Node()
if findChildren(root, False) == 0:
  raise Exception("chybne definovany root")

content.close()


# CNF zapis:
print "sat2cnf... cnf_write"

# zapis klauzuli:
output = open(output_filename + ".temp", "w")
root.cnf_write()
cnf_print(str(root.getName())) # korenovy uzel ma platit
output.close()

# CNF zapis vcetne hlavicky:
output = open(output_filename, "w")
output.write("p cnf " + str(number - 1) + " " + str(line) + "\n")
input = open(output_filename + ".temp", "r")
for line in input:
  output.write(line)
input.close()
output.close()
os.remove(output_filename + ".temp")

trans_file.close()

print "sat2cnf OK"
