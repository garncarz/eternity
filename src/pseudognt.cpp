/**
 * @file pseudognt.cpp
 * hledani reseni pomoci algoritmu inspirovaneho genetickymi algoritmy;
 * algoritmus neco o problemu vi; nepracuje se s retezci, ale rovnou s hraci
 * plochou - nekvalitni karticky mutuji (zadna reprodukce a krizeni)
 */

#include <stdlib.h>
#include <stdio.h>
#include <math.h>

#include "pseudognt.h"
#include "area.h"
#include "cards.h"

using namespace Area;
using namespace Cards;

/**
 * budeme ukladat hodnoty kvality do souboru?
 * ano => necht plati define
 * ne => necht plati undef
 */
// #define PSEUDOGNT_QUALITY_SAVE 1
#undef PSEUDOGNT_QUALITY_SAVE

#ifdef PSEUDOGNT_QUALITY_SAVE
/**
 * soubor pro ukladani hodnot kvality
 */
FILE *pseudognt_quality_file;
#endif

cards_count_t PseudoGnt::orig_cards_count;
cards_count_t PseudoGnt::cards_count;
double PseudoGnt::card_quality[MAX_Y][MAX_X];
double PseudoGnt::quality;
int PseudoGnt::quality_const_length = 0;


double PseudoGnt::double_rand()
{
  return (rand() % 10001) / 10000.0;
}


double PseudoGnt::population_quality()
{
  double quality = 0;

  // maximalni hodnota bodu pro jednu karticku
  double one_point = (double)1 / area_max_x / area_max_y;

  // pro kazdou karticku
  for (int y = 1; y <= area_max_y; y++)
    for (int x = 1; x <= area_max_x; x++)
    {
      // originalni pocet karticek s danym vzorem
      int orig_count = orig_cards_count[area[y][x][NORTH]]
        [area[y][x][EAST]][area[y][x][SOUTH]][area[y][x][WEST]];

      // aktualni pocet karticek s danym vzorem
      int count = cards_count[area[y][x][NORTH]][area[y][x][EAST]]
        [area[y][x][SOUTH]][area[y][x][WEST]];

      // kvalita karticky podle poctu:
      // je-li dany typ karticky v originale obsazen vicekrat, je kvalita
      // karticky maximalni; neni-li dany typ karticky obsazen vubec,
      // je kvalita karticky nulova; jinak je kvalita karticky rovna
      // podilu originalniho poctu a aktualniho poctu
      double origin_quality = (orig_count >= count) ? 1 :
        (orig_count == 0) ? 0 : (double)orig_count / count;

      // kvalita podle vyhovovani okoli
      // v nynejsi verzi programu k nicemu, karticka vzdy okoli vyhovuje
      double neighbour_quality = 0;
      neighbour_quality += (area[y][x][NORTH] == area[y - 1][x][SOUTH]) ? 1 : 0;
      neighbour_quality += (area[y][x][EAST] == area[y][x + 1][WEST]) ? 1 : 0;
      neighbour_quality += (area[y][x][SOUTH] == area[y + 1][x][NORTH]) ? 1 : 0;
      neighbour_quality += (area[y][x][WEST] == area[y][x - 1][EAST]) ? 1 : 0;
      neighbour_quality /= 4;

      // celkova kvalita karticky
      // card_quality[y][x] = origin_quality * 0.75 + neighbour_quality * 0.25;
      card_quality[y][x] = origin_quality;

      // celkova kvalita populace
      quality += card_quality[y][x] * one_point;
    }

  // prace se zafixovanymi kartickami - v pripade nedodrzeni jejich pozic
  // dojde k penalizaci
  for (int fixed_nr = 0; fixed_nr < MAX_FIXED; fixed_nr++)
    if (!cards_equal(area_fixed_card[fixed_nr], area[area_fixed_y[fixed_nr]]
        [area_fixed_x[fixed_nr]]))
    {
      card_quality[area_fixed_y[fixed_nr]][area_fixed_x[fixed_nr]] = 0;
      quality /= 2;
    }

  return quality;
}


void PseudoGnt::rand_card_for(int x, int y)
{
  area[y][x][NORTH] = (y == 1) ? 0 :
    (double_rand() >= card_quality[y - 1][x]) ?
    area[y - 1][x][SOUTH] = rand_symbol() : area[y - 1][x][SOUTH];
  area[y][x][EAST] = (x == area_max_x) ? 0 :
    (double_rand() >= card_quality[y][x + 1]) ?
    area[y][x + 1][WEST] = rand_symbol() : area[y][x + 1][WEST];
  area[y][x][SOUTH] = (y == area_max_y) ? 0 :
    (double_rand() >= card_quality[y + 1][x]) ?
    area[y + 1][x][NORTH] = rand_symbol() : area[y + 1][x][NORTH];
  area[y][x][WEST] = (x == 1) ? 0 :
    (double_rand() >= card_quality[y][x - 1]) ?
    area[y][x - 1][EAST] = rand_symbol() : area[y][x - 1][EAST];
}


void PseudoGnt::force_rand_card_for(int x, int y)
{
  area[y][x][NORTH] = (y == 1) ? 0 : 
    area[y - 1][x][SOUTH] = rand_symbol();
  area[y][x][EAST] = (x == area_max_x) ? 0 : 
    area[y][x + 1][WEST] = rand_symbol();
  area[y][x][SOUTH] = (y == area_max_y) ? 0 : 
    area[y + 1][x][NORTH] = rand_symbol();
  area[y][x][WEST] = (x == 1) ? 0 :
    area[y][x - 1][EAST] = rand_symbol();
}


void PseudoGnt::rand_some_cards()
{
  int nr = area_max_x * area_max_y / 5;
  if (nr < 1)
    nr = 1;
  for (int i = 0; i < nr; i++)
    force_rand_card_for(rand() % area_max_x + 1, rand() % area_max_y + 1);
}


void PseudoGnt::better_population()
{
  // je kvalita populace plus minus stejna jiz delsi dobu?
  if (quality_const_length > 20)
  {
    rand_some_cards();  // pokusime se vytvorit vetsi ruznorodost
    quality_const_length = 0;
  }
  else
    // pro kazdou karticku
    for (int y = 1; y <= area_max_y; y++)
      for (int x = 1; x <= area_max_x; x++)
        // jestlize je nahoda silnejsi nez kvalita karticky,
        // zmen danou karticku
        if (double_rand() >= card_quality[y][x])
          rand_card_for(x, y);
}


void PseudoGnt::area_pseudognt_write()
{
  count_cards(cards_count);  // nutne pro population_quality
  printf("*********\n%f\n", population_quality());
  area_write();
  // krome plochy vypiseme i kvality vsech karticek
  for (int y = 1; y <= area_max_y; y++)
  {
    for (int x = 1; x <= area_max_x; x++)
      printf("%f, ", card_quality[y][x]);
    printf("\n");
  }
  printf("\n");
}


void PseudoGnt::pseudognt()
{
  printf("pseudognt... ");
  fflush(stdout);

#ifdef PSEUDOGNT_QUALITY_SAVE
  // otevreni souboru pro ukladani hodnot kvality
  if ((pseudognt_quality_file = fopen("quality_pseudognt.txt", "w")) == NULL)
  {
    printf("nepodarilo se vytvorit quality_pseudognt.txt!\n");
    exit(1);
  }
#endif

  area_load("area.area"); // nacte plochu
  // spocita karticky pro kazdou moznou kombinaci vzoru
  count_cards(orig_cards_count);
  area_write();

  area_create(false);  // vytvori novou, zcela nahodnou plochu se zachovanim
      // puvodnich fixovanych karticek

  // zatim nejlepsi kvalita populace
  double best_quality = -1;

  // jiz pro novou plochu:
  // spocita pocty karticek pro vsechny kombinace vzoru
  count_cards(cards_count);
  // urci aktualni kvalitu
  quality = population_quality();
  // posledni kvalita - pro urcovani konstantnosti
  double last_quality = quality;
  // po kolika krocich je kvalita plus minus konstantni
  quality_const_length = 0;

#ifdef PSEUDOGNT_QUALITY_SAVE
  // prida hodnotu kvality do log souboru
  fprintf(pseudognt_quality_file, "%f\n", quality);
#endif

  // dokud neni kvalita rovna 1.0
  while (quality < 1.0)
  {
    better_population();  // pokus se vylepsit populaci
    // spocita pocty karticek pro vsechny kombinace vzoru
    count_cards(cards_count);
    quality = population_quality();  // urci aktualni kvalitu

#ifdef PSEUDOGNT_QUALITY_SAVE
  // zapsani hodnoty kvality do souboru
  fprintf(pseudognt_quality_file, "%f\n", quality);
#endif

    // je kvalita plus minus stejna jako minule?
    if (fabs(quality - last_quality) < 0.002)
      quality_const_length++;
    else {  // nova posledni kvalita - jsme nekde jinde
      quality_const_length = 0;
      last_quality = quality;
    }

    if (quality > best_quality) // dosahli jsme zatim nejlepsiho vysledku?
    {
      best_quality = quality;
      area_pseudognt_write();
      area_save("area_pseudognt_temp.area");
    }
  }

  area_save("area_pseudognt_result.area");  // ulozeni vysledku

#ifdef PSEUDOGNT_QUALITY_SAVE
  // uzavreni souboru s hodnotami kvality
  fclose(pseudognt_quality_file);
#endif

  printf("OK\n");
}
