/**
 * @file unsat.cpp
 * vytvori vyslednou plochu podle SAT reseni
 */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "unsat.h"
#include "area.h"

using namespace Area;


void Unsat::unsat()
{
  printf("ziskani plochy ze SAT reseni... ");
  fflush(stdout);
  
  // pro vytvoreni stejne velke plochy jako ma zadani
  area_load("area.area");
  area_clean();

  // nacteni SAT reseni:
  FILE *fp;
  if ((fp = fopen("sat_result.txt", "r")) == NULL)
  {
    printf("nepodarilo se otevrit sat_result.txt!\n");
    exit(1);
  }

  char *string = new char[100];
  while (!feof(fp))
  {
    fscanf(fp, "%s", string);

    // nepravdy nas nezajimaji
    if (strstr(string, "NOT") != NULL)
    {
      // dalsi retezec je negovana promenna, ta nas nezajima
      fscanf(fp, "%s", string);
      continue;
    }

    // nacteni udaju o obsahu plochy
    int x, y, side, symbol;
    if (sscanf(string, "area_symbol_%d_%d__%d_%d", &x, &y, &side, &symbol))
    {
      area[y][x][side] = symbol;
      
      // doplneni daneho symbolu i pro sousedni parovaci pozici (musi byt)
      if (side == SOUTH)
        area[y + 1][x][NORTH] = symbol;
      else if (side == EAST)
        area[y][x + 1][WEST] = symbol;
    }
  }
  delete []string;

  // ulozeni vysledne plochy
  area_save("area_sat_result.area");

  printf("OK\n");
}
