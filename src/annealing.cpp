/**
 * @file annealing.cpp
 * reseni problemu simulovanym zihanim
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "annealing.h"
#include "area.h"
#include "cards.h"

using namespace Area;
using namespace Cards;

/**
 * budeme ukladat hodnoty teploty do souboru?
 * ano => necht plati define
 * ne => necht plati undef
 */
// #define TEMPERATURE_SAVE 1
#undef TEMPERATURE_SAVE

#ifdef TEMPERATURE_SAVE
/**
 * soubor pro ukladani hodnot teploty
 */
FILE *temperature_file;
#endif

double Annealing::count_energy()
{
  double energy = 0;
  
  // pro kazdou karticku
  for (int y = 1; y <= area_max_y; y++)
    for (int x = 1; x <= area_max_x; x++)
    {
      // spocitame energii karticky primou umerou podle neshodnosti
      // symbolu s okolim a sedych symbolu uprostred plochy (nepatri tam)
      double card_energy = 0;
      card_energy += (area[y][x][NORTH] != area[y - 1][x][SOUTH]) ? 1 : 0;
      card_energy += (y > 1 && area[y][x][NORTH] == 0) ? 1 : 0;
      card_energy += (area[y][x][EAST] != area[y][x + 1][WEST]) ? 1 : 0;
      card_energy += (x < area_max_x && area[y][x][EAST] == 0) ? 1 : 0;
      card_energy += (area[y][x][SOUTH] != area[y + 1][x][NORTH]) ? 1 : 0;
      card_energy += (y < area_max_y && area[y][x][SOUTH] == 0) ? 1 : 0;
      card_energy += (area[y][x][WEST] != area[y][x - 1][EAST]) ? 1 : 0;
      card_energy += (x > 1 && area[y][x][WEST] == 0) ? 1 : 0;
      
      // pripocitani do celkove energie
      energy += card_energy;
    }

  // prace se zafixovanymi kartickami - v pripade nedodrzeni jejich pozic
  // dojde k penalizaci
  for (int fixed_nr = 0; fixed_nr < MAX_FIXED; fixed_nr++)
    if (!cards_equal(area_fixed_card[fixed_nr], area[area_fixed_y[fixed_nr]]
        [area_fixed_x[fixed_nr]]))
      energy *= 2;

  return energy;
}


void Annealing::neighbour_area()
{
  // prohodi dve nahodne sousedici karticky nebo jednu nahodnou nahodne zrotuje
  
  if (rand() % 3 == 1) // rotovat nahodnou karticku?
  {
    int x = rand() % area_max_x + 1;
    int y = rand() % area_max_y + 1;
    int rot = rand() % 4;
    
    // zrotovani karticky
    int card_temp[4];
    card_copy(area[y][x], card_temp);
    card_copy(card_temp, area[y][x], rot);
  }
  else { // vymenit dve nahodne sousedici karticky
    int x1 = rand() % area_max_x + 1;
    int y1 = rand() % area_max_y + 1;

    int x2 = x1;
    int y2 = y1;
    if (rand() % 2 == 1) // vymenit v ramci jednoho radku
    {
      if (x1 == 1) // muzeme pouze vpravo?
        x2 = 2;
      else if (x1 == area_max_x) // muzeme pouze vlevo?
        x2 = x1 - 1;
      else // vyberem nahodne z vlevo a vpravo
        x2 = x1 + ((rand() % 2 == 1) ? 1 : -1);
    }
    else { // vymenit v ramci jednoho sloupce
      if (y1 == 1) // muzeme pouze dolu?
        y2 = 2;
      else if (y1 == area_max_y) // muzeme pouze nahoru?
        y2 = y1 - 1;
      else // vyberem nahodne z dolu a nahoru
        y2 = y1 + ((rand() % 2 == 1) ? 1 : -1);
    }
    
    // prohozeni karticek
    int card_temp[4];
    card_copy(area[y1][x1], card_temp);
    card_copy(area[y2][x2], area[y1][x1]);
    card_copy(card_temp, area[y2][x2]);
  }
}


double Annealing::probability(double old_energy, double new_energy,
  double temperature)
{
  // podle
  // http://en.wikipedia.org/wiki/Simulated_annealing#Acceptance_probabilities
  
  if (new_energy < old_energy)
    return 1;
  else
    return exp((old_energy - new_energy) / temperature);
}


double Annealing::temperature(unsigned long long step)
{
#ifdef TEMPERATURE_SAVE
  // zapsani hodnoty teploty do souboru;
  // POZOR na dodrzeni shody hodnoty s navratovou hodnotou
  fprintf(temperature_file, "%f\n", 10000000 / (double)(step % 10000000 + 1));
#endif

  return 10000000 / (double)(step % 10000000 + 1);
}


double Annealing::double_rand()
{
  return (rand() % 100001) / 100000.0;
}


void Annealing::anneal()
{
  printf("simulated annealing...\n");
  
#ifdef TEMPERATURE_SAVE
  // otevreni souboru pro ukladani hodnot teploty
  if ((temperature_file = fopen("anneal_temperature.txt", "w")) == NULL)
  {
    printf("nepodarilo se vytvorit anneal_temperature.txt!\n");
    exit(1);
  }
#endif
  
  area_load("area.area");   // nacte plochu
  
  double energy = count_energy(); // energie plochy - "oceli"
  area_t best_area; // zatim nejlepsi plocha ("stav oceli")
  area_copy(area, best_area);
  double best_energy; // energie zatim nejlepsi plochy
  best_energy = energy;
  
  area_t old_area; // plocha minuleho kroku
  area_copy(area, old_area);
  double old_energy; // energie minuleho kroku
  old_energy = energy;
  
  unsigned long long step = 0; // krok

  // dokud je nenulova energie (mensi byt nemuze)
  while (energy > 0.0)
  {
    neighbour_area(); // sousedni stav "oceli"
    energy = count_energy(); // jeho energie
    
    // dostali jsme se do zatim nejlepsiho stavu?
    if (energy < best_energy)
    {
      // zkopirujeme jej, vypiseme a ulozime
      area_copy(area, best_area);
      best_energy = energy;
      
      printf("%f\n", energy);
      area_write();
      area_save("area_anneal_temp.area");
    }
    
    // je nahoda priklonena k tomu, ze prejdeme do noveho stavu oceli?
    if (probability(old_energy, energy, temperature(step))
      > double_rand())
    {
      // ano, zkopirujeme aktualni stav do stavu minuleho
      area_copy(area, old_area);
      old_energy = energy;
    }
    else {
      // ne, zkopirujeme stav plochy zpatky
      area_copy(old_area, area);
    }
    
    // dalsi krok s pripadnym vypsanim
    step++;
    /* if (step % 1000000 == 0)
      printf("step %lld\n\n", step);*/
  }
  
  // finalni vypsani a ulozeni plochy - vysledku
  area_copy(best_area, area);
  area_write();
  area_save("area_anneal_result.area");
  
#ifdef TEMPERATURE_SAVE
  // uzavreni souboru s hodnotami teploty
  fclose(temperature_file);
#endif
  
  printf("OK\n");
}
