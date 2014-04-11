/**
 * @file area.cpp
 * sprava hraci plochy
 */

#include <stdlib.h>
#include <stdio.h>

#include "area.h"
#include "cards.h"

using namespace Cards;


area_t Area::area;
int Area::area_max_x;
int Area::area_max_y;
int Area::area_max_symbols;
int Area::area_fixed_x[VIRTUAL_MAX_FIXED];
int Area::area_fixed_y[VIRTUAL_MAX_FIXED];
int Area::area_fixed_card[VIRTUAL_MAX_FIXED][4];


void Area::area_load(const char *filename, area_t to_area)
{
  FILE *fp;
  
  if ((fp = fopen(filename, "r")) == NULL)
  {
    printf("nepodarilo se otevrit %s!\n", filename);
    exit(1);
  }
  
  // rozmery plochy a pocet symbolu
  fscanf(fp, "%ix%ix%i\n", &area_max_x, &area_max_y, &area_max_symbols);

  // pozice zafixovanych karticek
  for (int fixed = 0; fixed < MAX_FIXED; fixed++)
    fscanf(fp, "%i,%i ", &area_fixed_x[fixed], &area_fixed_y[fixed]);
  fscanf(fp, "\n");

  area_clean(); // pro pomocna seda presahova policka

  // jednotliva policka
  for (int y = 1; y <= area_max_y; y++)
  {
    for (int x = 1; x <= area_max_x; x++)
    {
      fscanf(fp, "[%i,%i,%i,%i] ",
          &to_area[y][x][NORTH],
          &to_area[y][x][EAST],
          &to_area[y][x][SOUTH],
          &to_area[y][x][WEST]);
    }
    fscanf(fp, "\n");
  }
  
  fclose(fp);

  // okopirujeme si zafixovanou karticku
  for (int fixed = 0; fixed < MAX_FIXED; fixed++)
    card_copy(to_area[area_fixed_y[fixed]][area_fixed_x[fixed]],
      area_fixed_card[fixed]);

  check_cards(to_area); // zkontrolujeme karticky
}


void Area::area_save(const char *filename)
{
  FILE *fp;
  
  if ((fp = fopen(filename, "w")) == NULL)
  {
    printf("nepodarilo se vytvorit %s!\n", filename);
    exit(1);
  }
  
  // rozmery plochy a pocet symbolu
  fprintf(fp, "%ix%ix%i\n", area_max_x, area_max_y, area_max_symbols);
  
  // pozice zafixovanych karticek
  for (int fixed = 0; fixed < MAX_FIXED; fixed++)
    fprintf(fp, "%i,%i ", area_fixed_x[fixed], area_fixed_y[fixed]);
  fprintf(fp, "\n");

  // ukladame tak, jak plocha vypada:
  // v radcich (pozicovane s y) sloupce (pozicovane s x)
  for (int y = 1; y <= area_max_y; y++)
  {
    for (int x = 1; x <= area_max_x; x++)
    {
      fprintf(fp, "[%i,%i,%i,%i] ",
          area[y][x][NORTH],
          area[y][x][EAST],
          area[y][x][SOUTH],
          area[y][x][WEST]);
    }
    fprintf(fp, "\n");
  }
  
  fclose(fp);
}


void Area::area_write()
{
  printf("\n");
  
  for (int y = 1; y <= area_max_y; y++)
  {
    for (int x = 1; x <= area_max_x; x++)
    {
      char cs, ce;
      if (fixed(x, y))
      {
        cs = '{'; // specialni znaky pro fixovane karticky
        ce = '}';
      }
      else {
        cs = '['; // znaky pro normalni karticky
        ce = ']';
      }
      printf("%c%i,%i,%i,%i%c ",
          cs,
          area[y][x][NORTH],
          area[y][x][EAST],
          area[y][x][SOUTH],
          area[y][x][WEST],
          ce);
    }
    printf("\n");
  }
  
  printf("\n");
}


void Area::area_clean()
{
  // presah plochy - pomocna nulova policka
  int zero_card[] = {0, 0, 0, 0};
  for (int x = 1; x <= area_max_x; x++)
  {
    card_copy(zero_card, area[0][x]);
    card_copy(zero_card, area[area_max_x + 1][x]);
  }
  for (int y = 1; y <= area_max_y; y++)
  {
    card_copy(zero_card, area[y][0]);
    card_copy(zero_card, area[y][area_max_y + 1]);
  }
  
  // plocha
  for (int y = 1; y <= area_max_y; y++)
    for (int x = 1; x <= area_max_x; x++)
      card_copy(empty_card, area[y][x]);

  // vratime zafixovane karticky
  for (int fixed = 0; fixed < MAX_FIXED; fixed++)
    card_copy(area_fixed_card[fixed],
      area[area_fixed_y[fixed]][area_fixed_x[fixed]]);
}


void Area::area_copy(area_t from_area, area_t to_area)
{
  for (int y = 1; y <= area_max_y; y++)
    for (int x = 1; x <= area_max_x; x++)
      for (int side = 0; side < 4; side++)
        to_area[y][x][side] = from_area[y][x][side];
}


bool Area::fixed(int x, int y)
{
  for (int fixed = 0; fixed < MAX_FIXED; fixed++)
    if (x == area_fixed_x[fixed] && y == area_fixed_y[fixed])
      return true;
  return false;
}


int Area::rand_symbol()
{
  return rand() % area_max_symbols + 1;
}


void Area::area_create(bool new_fixed)
{
  // sede okraje
  for (int x = 1; x <= area_max_x; x++)
    area[1][x][NORTH] = area[area_max_y][x][SOUTH] = 0;
  for (int y = 1; y <= area_max_y; y++)
    area[y][1][WEST] = area[y][area_max_x][EAST] = 0;

  // okrajovy radek dole - horizontalne
  for (int x = 1; x < area_max_x; x++)
    area[area_max_y][x][EAST] = area[area_max_y][x + 1][WEST] = rand_symbol();
  // okrajovy sloupec vpravo - vertikalne
  for (int y = 1; y < area_max_y; y++)
    area[y][area_max_x][SOUTH] = area[y + 1][area_max_x][NORTH] = rand_symbol();

  // vse ostatni - nesedy soused vpravo i dole
  for (int y = 1; y < area_max_y; y++)
  {
    for (int x = 1; x < area_max_x; x++)
    {
      area[y][x][EAST] = area[y][x + 1][WEST] = rand_symbol();
      area[y][x][SOUTH] = area[y + 1][x][NORTH] = rand_symbol();
    }
  }

  // mame vytvaret nove zafixovane karticky?
  if (new_fixed)
  {
    // pokud je nastaveno vice zafixovanych karticek nez tech neokrajovych
    if (MAX_FIXED > (area_max_x - 2) * (area_max_y - 2))
    {
      printf("zafixovanych karticek nesmi byt vice nez tech neokrajovych!\n");
      exit(1);
    }
  
    // vygenerujeme nahodne pozice zafixovanych karticek (nekde uvnitr)
    for (int f = 0; f < MAX_FIXED; f++)
      area_fixed_x[f] = area_fixed_y[f] = -1; // priprava pro neopakovani pozic
    for (int f = 0; f < MAX_FIXED; f++)
    {
      int x, y; // nove pozice
      do {
        x = rand() % (area_max_x - 2) + 2;
        y = rand() % (area_max_y - 2) + 2;
      } while (fixed(x, y)); // nechceme opakovat zafixovane pozice
      area_fixed_x[f] = x;
      area_fixed_y[f] = y;
    }
  }

}


void Area::area_randomize()
{
  // prohazovani karticek zopakujeme tolikrat,
  // kolik je karticek - na kazdou karticku
  // tedy muze vyjit dvakrat
  for (int i = 0; i < area_max_x * area_max_y; i++)
  {
    // ziskame nahodne souradnice dvou karticek
    int x1 = rand() % area_max_x + 1;
    int y1 = rand() % area_max_y + 1;
    int x2 = rand() % area_max_x + 1;
    int y2 = rand() % area_max_y + 1;

    // zafixovanou karticku nemuzeme prohodit
    if (fixed(x1, y1) || fixed(x2, y2))
      continue;

    // a prohodime je
    for (int side = 0; side < 4; side++)
    {
      int tmp = area[y1][x1][side];
      area[y1][x1][side] = area[y2][x2][side];
      area[y2][x2][side] = tmp;
    }
  }

  // kazdou karticku nahodne otoci
  for (int y = 1; y <= area_max_y; y++)
    for (int x = 1; x <= area_max_x; x++)
    {
      int rot = rand() % 4;
      if (rot != 0)
      {
        int tmp[4];
        for (int side = 0; side < 4; side++)
          tmp[side] = area[y][x][side];
        for (int side = 0; side < 4; side++)
          area[y][x][side] = tmp[(side + rot) % 4];
      }
    }
}
