/**
 * @file eternity.cpp
 * obsluha reseni problemu
 */

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <signal.h>

#include "eternity.h"
#include "area.h"

// namespacy:
#include "generator.h"
#include "backtracker.h"
#include "annealing.h"
#include "pseudognt.h"
#include "genetics.h"
#include "sat.h"
#include "unsat.h"

using namespace Area;


time_t Eternity::rawtime_start, Eternity::rawtime_end;


void Eternity::help()
{
  std::cout << 
    "Pouziti:\n" <<
    "  gen X Y S - vygeneruje nahodnou plochu s rozmery X > 1 krat Y > 1\n" <<
    "    a poctem nesedych symbolu S > 0\n" <<
    "  bck [steps] [cards_order] [im limit] -" <<
        " spusti backtracking reseni problemu\n" <<
    "    steps - jak postupovat po herni plose:\n" <<
    "      - radkove, / sikme, @ spiralovite (defaultni)\n" <<
    "    cards_order - z jakeho poradi karticek vybirat pro kazdou pozici:\n" <<
    "      123 - vzdy stejneho (defaultni), rnd - nahodneho\n" <<
    "    im - pokud je zadano, zacne se provadet netrpelivy backtracking\n" <<
    "      limit - max. pocet dosazeni karticky pri kazdem jednotlivem" <<
        " backtrackingu\n" <<
    "  anneal - resi problem pomoci simulovaneho zihani\n" <<
    "  pseudognt - spusti reseni problemu pomoci" <<
        " pseudo-genetickeho algoritmu\n" <<
    "    (jen mutuje malo kvalitni karticky)\n" <<
    "  gnt [decode] [origin cross mut] - spusti reseni problemu pomoci GA\n" <<
    "    (reprodukce, krizeni, mutace; retezce populace jsou kodovanymi" <<
        " plochami)\n" <<
    "    decode - zpusob dekodovani retezcu:\n" <<
    "      sym - dekodovat jako symboly na plose, defaultni\n" <<
    "      pos - dekodovat jako pozice karticek\n" <<
    "    origin - vaha kladena na spravne symboly na kartickach" <<
        " v procentech\n" <<
    "      (zbytek jde do shody s okolnimi kartickami), defaultne 75\n" <<
    "    cross - pravdepodobnost krizeni v promilich, defaultne 400\n" <<
    "    mut - pravdepodobnost mutace v promilich, defaultne 3\n" <<
    "  sat [not] - zapise problem do souboru sat.txt v Simplify" <<
        " SAT formatu\n" <<
    "    not - negovat vyslednou podminku (pri hledani protiprikladu)\n" <<
    "  unsat - vytvori plochu ze SAT reseni\n";
}


void Eternity::error()
{
  help();
  exit(1);
}


void Eternity::end()
{
  // konec mereni casu
  time(&rawtime_end);
  struct tm* timeinfo = localtime(&rawtime_end);
  printf("\nkonec %s\n", asctime(timeinfo));

  // trvani
  double spent_time = difftime(rawtime_end, rawtime_start);
  int hours = (int)spent_time / 3600;
  spent_time -= hours * 3600;
  int minutes = (int)spent_time / 60;
  spent_time -= minutes * 60;
  int seconds = (int)spent_time;
  printf("trvani %02i:%02i:%02i\n", hours, minutes, seconds);
}


void Eternity::terminate(int)
{
  exit(1);
}


int Eternity::main(int argn, char **arg)
{
  if (argn < 2) { // prilis maly pocet argumentu - pouze vypiseme napovedu
    help();
    return 0;
  }

  // zacatek mereni casu
  time(&rawtime_start);
  struct tm* timeinfo = localtime(&rawtime_start);
  printf("start %s\n", asctime(timeinfo));

  // registrace "uklidove" funkce - pro konec mereni casu
  atexit(end);

  // registrace osetreni signalu Ctrl+C
  signal(SIGINT, terminate);

  // inicializace generatoru pseudonahodnych cisel
  srand((int)time(NULL));

  if (strcmp(arg[1], "gen") == 0) // mame generovat pole?
  {
    using namespace Generator;
    
    if (argn != 5) // chybny pocet argumentu
      error();

    area_max_x = atoi(arg[2]); // velikost pole - x
    area_max_y = atoi(arg[3]); // velikost pole - y
    area_max_symbols = atoi(arg[4]); // pocet nesedych symbolu
    if (area_max_x < 2 || area_max_y < 2 || area_max_symbols < 1)
      error(); // chybne nastavene pole

    generator(); // vse v poradku, vygenerujeme pole
  }

  else if(strcmp(arg[1], "bck") == 0) // mame resit problem backtrackingem?
  {
    using namespace Backtracker;

    if (argn >= 3) // mame zadano, jak postupovat po herni plose?
    {
      bcktrk_steps_lines = false; // odstranime defaultni hodnotu
      if (strcmp(arg[2], "-") == 0) // mame postupovat radkove
        bcktrk_steps_lines = true;
      else if (strcmp(arg[2], "/") == 0) // mame postupovat sikme
        bcktrk_steps_inclined_lines = true;
      else if (strcmp(arg[2], "@") == 0) // mame postupovat spiralovite
        bcktrk_steps_spiral = true;
      else // neznamy argument
        error();
    }

    // mame zadano, z jakeho poradi karticek pro kazdou pozici vybirat?
    if (argn >= 4)
    {
      bcktrk_order_same = false; // odstranime defaultni hodnotu
      if (strcmp(arg[3], "123") == 0) // mame vybirat vzdy ze stejneho poradi
        bcktrk_order_same = true;
      // mame vybirat vzdy z nahodneho poradi
      else if (strcmp(arg[3], "rnd") == 0)
        bcktrk_order_random = true;
      else // neznamy argument
        error();
    }

    if (argn >= 5) // je zadan netrpelivy backtracking?
    {
      if (strcmp(arg[4], "im") == 0) // opravdu je zadan?
      {
        if (argn < 6)
          error(); // chybi argument

        // limit - pro kazdy jednotlivy backtracking
        unsigned long long limit = atol(arg[5]);
        if (limit < 1) // ma rozumnou hodnotu?
          error();

        impatient_backtracker(limit); // spusteni netrpliveho backtrackingu
      }
      else // neznamy argument
        error();
    }
    else // klasicky backtracking
      backtracker();
  }


  else if (strcmp(arg[1], "anneal") == 0) // mame resit problem sim. zihanim?
  {
    using namespace Annealing;
    anneal(); // spusti reseni simulovanym zihanim
  }


  // mame resit problem pseudo-genetickym algoritmem?
  else if (strcmp(arg[1], "pseudognt") == 0)
  {
    using namespace PseudoGnt;

    if (argn != 2) // jsou spravne argumenty? (zadne dalsi argumenty)
      error();

    pseudognt(); // spusti reseni pseudo-genetickym algoritmem
  }

  // mame resit problem genetickym algoritmem?
  else if (strcmp(arg[1], "gnt") == 0)
  {
    using namespace Genetics;
    
    if (argn >= 3) // mame zadano, jak dekodovat retezce?
    {
      if (strcmp(arg[2], "sym") == 0) // jako symboly
        area_decode = &area_decode_symbols;
      else if (strcmp(arg[2], "pos") == 0) // jako pozice
        area_decode = &area_decode_positions;
      else // chybne zadane parametry
        error();
    }

    if (argn == 6) // byly zadany konstanty?
    {
      // vaha shody kombinace symbolu na kartickach se zadanou plochou
      origin_importance = (double)atoi(arg[2]) / 100;
      crossover_likehood = atoi(arg[3]); // pravdepodobnost krizeni
      mutation_likehood = atoi(arg[4]); // pravdepodobnost mutovani
    }
    else if (argn < 4); // bez parametru konstant
    else // chybne zadane parametry
      error();

    genetics(); // spusti reseni genetickym algoritmem
  }

  else if (strcmp(arg[1], "sat") == 0) // mame zapsat problem jako SAT?
  {
    using namespace Sat;

    if (argn == 2) // ne negovat vyslednou podminku
      sat(false);
    // negovat vyslednou podminku
    else if (argn == 3 && strcmp(arg[2], "not") == 0)
      sat(true);
    else
      error(); // chybne zadane parametry
  }

  else if (strcmp(arg[1], "unsat") == 0) // mame vytvorit plochu ze SAT reseni?
  {
    using namespace Unsat;
    unsat();
  }

  else // chybny argument/argumenty
    error();

  return 0;
}


/**
 * volana hlavni funkce programu - pouze preda rizeni Eternity::main(...)
 * @param argn pocet argumentu
 * @param arg argumenty programu
 * @return navratova hodnota behu programu
 */
int main(int argn, char **arg)
{
  return Eternity::main(argn, arg);
}
