/**
 * @file sat.cpp
 * priprava SAT zapisu problemu ve formatu Simplify
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "sat.h"
#include "area.h"

using namespace Area;


FILE *Sat::fp;


void Sat::card_position(int card_nr, int x, int y)
{
  fprintf(fp, "card_position_%d__%d_%d", card_nr, x, y);
}


void Sat::one_position_parent(int x, int y, int left, int right)
{
  fprintf(fp, "one_position_%d_%d__%d_%d", x, y, left, right);
}


void Sat::card_rotation(int card_nr, int rot)
{
  fprintf(fp, "card_rotation_%d_%d", card_nr, rot);
}


void Sat::area_symbol(int x, int y, int side, int symbol)
{
  // pro redukci poctu danych promennych budeme uvazovat jen SOUTH
  // a  EAST strany (stejne maji mit sousedi stejne symboly)
  if (side == NORTH)
  {
    y--;
    side = SOUTH;
  }
  else if (side == WEST)
  {
    x--;
    side = EAST;
  }

  fprintf(fp, "area_symbol_%d_%d__%d_%d", x, y, side, symbol);
}


void Sat::one_symbol_parent(int x, int y, int side, int left, int right)
{
  fprintf(fp, "one_symbol_%d_%d__%d_%d_%d", x, y, side, left, right);
}


void Sat::max_one_card_on_position(int x, int y, int card_left, int card_right)
{
  // zbyva jedna karticka
  if (card_right == card_left)
  {
    fprintf(fp, "  (OR (AND (NOT ");
    card_position(card_left, x, y);
    fprintf(fp, ") (NOT ");
    one_position_parent(x, y, card_left, card_right);
    fprintf(fp, ")) (AND ");
    card_position(card_left, x, y);
    fprintf(fp, " ");
    one_position_parent(x, y, card_left, card_right);
    fprintf(fp, "))\n");
  }
  
  // zbyvaji dve karticky
  else if (card_left == card_right - 1)
  {
    fprintf(fp, "  (OR (NOT ");
    card_position(card_left, x, y);
    fprintf(fp, ") (NOT ");
    card_position(card_right, x, y);
    fprintf(fp, "))\n");
    
    fprintf(fp, "  (OR (AND (NOT (OR ");
    card_position(card_left, x, y);
    fprintf(fp, " ");
    card_position(card_right, x, y);
    fprintf(fp, ")) (NOT ");
    one_position_parent(x, y, card_left, card_right);
    fprintf(fp, ")) (AND (OR ");
    card_position(card_left, x, y);
    fprintf(fp, " ");
    card_position(card_right, x, y);
    fprintf(fp, ") ");
    one_position_parent(x, y, card_left, card_right);
    fprintf(fp, "))\n");
  }
  
  // zbyva vice karticek (treba ctyri)
  else {
    // stred karticek pro puleni intervalu
    int center = (card_left + card_right) / 2;
    
    fprintf(fp, "  (OR (NOT ");
    one_position_parent(x, y, card_left, center);
    fprintf(fp, ") (NOT ");
    one_position_parent(x, y, center + 1, card_right);
    fprintf(fp, "))\n");
    
    fprintf(fp, "  (OR (AND (NOT (OR ");
    one_position_parent(x, y, card_left, center);
    fprintf(fp, " ");
    one_position_parent(x, y, center + 1, card_right);
    fprintf(fp, ")) (NOT ");
    one_position_parent(x, y, card_left, card_right);
    fprintf(fp, ")) (AND (OR ");
    one_position_parent(x, y, card_left, center);
    fprintf(fp, " ");
    one_position_parent(x, y, center + 1, card_right);
    fprintf(fp, ") ");
    one_position_parent(x, y, card_left, card_right);
    fprintf(fp, "))\n");
    
    max_one_card_on_position(x, y, card_left, center);
    max_one_card_on_position(x, y, center + 1, card_right);
  }
}


void Sat::cards_on_area()
{
  // pro kazdou karticku
  for (int card_nr = 0; card_nr < area_max_y * area_max_x; card_nr++)
  {
    int x, y;
    // pokud je karticka zafixovana, musi byt na urcene pozici
    if (fixed(x = card_nr % area_max_x + 1, y = card_nr / area_max_x + 1))
    {
      card_position(card_nr, x, y);
      fprintf(fp, "\n");
    }
    else { // jinak...
      // karticka je na nejake pozici na plose
      fprintf(fp, "  (OR ");
      for (y = 1; y <= area_max_y; y++)
        for (x = 1; x <= area_max_x; x++)
        {
          card_position(card_nr, x, y);
          fprintf(fp, " ");
        }
      fprintf(fp, ")\n");
    }
  }
  
  // pro kazdou pozici
  for (int y = 1; y <= area_max_y; y++)
    for (int x = 1; x <= area_max_x; x++)
    {
      // na dane pozici muze byt maximalne jedna karticka
      // + predchozi podminka => bude tam prave jedna karticka
      fprintf(fp, "  ");
      one_position_parent(x, y, 0, area_max_y * area_max_x - 1);
      fprintf(fp, "\n");
      max_one_card_on_position(x, y, 0, area_max_y * area_max_x - 1);
    }
}


void Sat::rotated_cards_area_content()
{
  // kazda karticka musi byt rotovana:
  for (int card_nr = 0; card_nr < area_max_y * area_max_x; card_nr++)
  {
    fprintf(fp, "  (OR ");
    for (int rot = 0; rot < 4; rot++)
    {
      card_rotation(card_nr, rot);
      fprintf(fp, " ");
    }
    fprintf(fp, ")\n");

    // kazda karticka muze byt rotovana pouze jednim zpusobem:
    // pro kazdou dvojici rotaci karticky muze byt platna max. jedna
    for (int rot1 = 0; rot1 < 4; rot1++)
      for (int rot2 = rot1 + 1; rot2 < 4; rot2++)
      {
        fprintf(fp, "  (OR (NOT ");
        card_rotation(card_nr, rot1);
        fprintf(fp, ") (NOT ");
        card_rotation(card_nr, rot2);
        fprintf(fp, "))\n");
      }
  }
  
  // pro kazdou pozici na plose
  for (int y = 1; y <= area_max_y; y++)
    for (int x = 1; x <= area_max_x; x++)
    {
      // pro kazdou karticku
      for (int card_nr = 0; card_nr < area_max_y * area_max_x; card_nr++)
      {
        // prepocet na souradnici v poli area
        int card_y = card_nr / area_max_x + 1;
        int card_x = card_nr % area_max_x + 1;
        
        // pokud je dana karticka fixovana, nemusime o ni uvazovat na jinych
        // pozicich
        if (fixed(card_x, card_y) && (card_x != x || card_y != y))
          continue;

        // pro kazdou rotaci karticky
        for (int rot = 0; rot < 4; rot++)
        {
          // zapis podminky: karticka na dane pozici && karticka rotovana
          // dany pocet krat => urcite symboly na urcite pozici na plose
          // (zapis bez implikace)
          fprintf(fp, "  (OR (NOT (AND ");
          card_position(card_nr, x, y);
          fprintf(fp, " ");
          card_rotation(card_nr, rot);
          fprintf(fp, ")) (AND ");
          for (int side = 0; side < 4; side++)
          {
            area_symbol(x, y, side,
              area[card_y][card_x][(side + rot) % 4]);
            fprintf(fp, " ");
          }
          fprintf(fp, "))\n");
        }
      }
    }
}


void Sat::gray_border(int x, int y, int side)
{
  // sedy symbol na danem miste
  fprintf(fp, "  ");
  area_symbol(x, y, side, 0);
  fprintf(fp, "\n");

  // na danem miste nesmi byt jiny symbol nez sedy
  for (int symbol = 1; symbol <= area_max_symbols; symbol++)
  {
    fprintf(fp, "  (NOT ");
    area_symbol(x, y, side, symbol);
    fprintf(fp, ")\n");
  }
}


void Sat::max_one_symbol_on_side(int x, int y, int side, int symbol_left,
  int symbol_right)
{
  // zbyva jeden symbol
  if (symbol_right == symbol_left)
  {
    fprintf(fp, "  (OR (AND (NOT ");
    area_symbol(x, y, side, symbol_left);
    fprintf(fp, ") (NOT ");
    one_symbol_parent(x, y, side, symbol_left, symbol_right);
    fprintf(fp, ")) (AND ");
    area_symbol(x, y, side, symbol_left);
    fprintf(fp, "  ");
    one_symbol_parent(x, y, side, symbol_left, symbol_right);
    fprintf(fp, "))\n");
  }
  
  // zbyvaji dva symboly
  else if (symbol_left == symbol_right - 1)
  {
    fprintf(fp, "  (OR (NOT ");
    area_symbol(x, y, side, symbol_left);
    fprintf(fp, ") (NOT ");
    area_symbol(x, y, side, symbol_right);
    fprintf(fp, "))\n");
    
    fprintf(fp, "  (OR (AND (NOT (OR ");
    area_symbol(x, y, side, symbol_left);
    fprintf(fp, " ");
    area_symbol(x, y, side, symbol_right);
    fprintf(fp, ")) (NOT ");
    one_symbol_parent(x, y, side, symbol_left, symbol_right);
    fprintf(fp, ")) (AND (OR ");
    area_symbol(x, y, side, symbol_left);
    fprintf(fp, " ");
    area_symbol(x, y, side, symbol_right);
    fprintf(fp, ") ");
    one_symbol_parent(x, y, side, symbol_left, symbol_right);
    fprintf(fp, "))\n");
  }
  
  // zbyva vice symbolu
  else {
    // stred symbolu pro puleni intervalu
    int center = (symbol_left + symbol_right) / 2;
 
    fprintf(fp, "  (OR (NOT ");
    one_symbol_parent(x, y, side, symbol_left, center);
    fprintf(fp, ") (NOT ");
    one_symbol_parent(x, y, side, center + 1, symbol_right);
    fprintf(fp, "))\n");
    
    fprintf(fp, "  (OR (AND (NOT (OR ");
    one_symbol_parent(x, y, side, symbol_left, center);
    fprintf(fp, " ");
    one_symbol_parent(x, y, side, center + 1, symbol_right);
    fprintf(fp, ")) (NOT ");
    one_symbol_parent(x, y, side, symbol_left, symbol_right);
    fprintf(fp, ")) (AND (OR ");
    one_symbol_parent(x, y, side, symbol_left, center);
    fprintf(fp, "  ");
    one_symbol_parent(x, y, side, center + 1, symbol_right);
    fprintf(fp, ") ");
    one_symbol_parent(x, y, side, symbol_left, symbol_right);
    fprintf(fp, "))\n");
    
    max_one_symbol_on_side(x, y, side, symbol_left, center);
    max_one_symbol_on_side(x, y, side, center + 1, symbol_right);
  }
}


void Sat::neighbour_symbol(int x, int y, int side)
{
  // na danem miste nesmi byt sedy symbol
  fprintf(fp, "  (NOT ");
  area_symbol(x, y, side, 0);
  fprintf(fp, ")\n");

  // prave jeden symbol je pravdivy
  fprintf(fp, "  ");
  one_symbol_parent(x, y, side, 1, area_max_symbols);
  fprintf(fp, "\n");
  max_one_symbol_on_side(x, y, side, 1, area_max_symbols);
}


void Sat::right_symbols()
{
  // sede okraje
  for (int x = 1; x <= area_max_x; x++)
  {
    gray_border(x, 1, NORTH);
    gray_border(x, area_max_y, SOUTH);
  }
  for (int y = 1; y <= area_max_y; y++)
  {
    gray_border(1, y, WEST);
    gray_border(area_max_x, y, EAST);
  }

  // okrajovy radek dole - horizontalne
  for (int x = 1; x < area_max_x; x++)
    neighbour_symbol(x, area_max_y, EAST);
  // okrajovy sloupec vpravo - vertikalne
  for (int y = 1; y < area_max_y; y++)
    neighbour_symbol(area_max_x, y, SOUTH);

  // vse ostatni - nesedy soused vpravo i dole
  for (int y = 1; y < area_max_y; y++)
    for (int x = 1; x < area_max_x; x++)
      // pro strany, ktere jsou uvazovany (pri redukci stran kvuli
      // shodnosti N-S a E-W): E a S
      for (int side = EAST; side <= SOUTH; side++)
        neighbour_symbol(x, y, side);
}


void Sat::sat(bool neg)
{
  printf("SAT... ");
  fflush(stdout);

  area_load("area.area"); // nacteni zadani

  // otevreni souboru pro zapsani SAT podminky
  if ((fp = fopen("sat.txt", "w")) == NULL)
  {
    printf("nepodarilo se otevrit pro zapis sat.txt!\n");
    exit(1);
  }

  // zacatek SAT podminky
  if (neg)
    fprintf(fp, "(NOT (AND\n");
  else
    fprintf(fp, "(AND\n");

  // vytvori podminku pro fakt, ze kazda karticka je prave jednou na plose
  // a na kazdem policku je prave jedna karticka
  cards_on_area();
  // vytvori podminku pro symboly na plose podle pozic a rotaci karticek
  rotated_cards_area_content();
  // vytvori podminku pro prave jeden symbol na kazdem miste
  right_symbols();

  // uzavreni podminky a souboru
  if (neg)
    fprintf(fp, "))\n");
  else
    fprintf(fp, ")\n");
  fclose(fp);
  
  printf("OK\n");
}
