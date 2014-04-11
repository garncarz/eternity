/**
 * @file generator.cpp
 * generuje nahodnou hraci plochu
 */

#include <stdio.h>

#include "generator.h"
#include "area.h"

using namespace Area;


void Generator::generator()
{
  printf("generuji nahodnou plochu... ");
  fflush(stdout);
  area_create();
  area_save("area_orig.area");
  area_randomize();
  area_save("area.area");
  printf("OK\n");
}
