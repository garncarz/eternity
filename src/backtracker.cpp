/**
 * @file backtracker.cpp
 * hledani reseni pomoci back trackingu;
 * temp ukladani plochy se stavem nejblize cili
 */

#include <stdlib.h>
#include <stdio.h>

#include "backtracker.h"
#include "area.h"
#include "cards.h"

using namespace Area;
using namespace Cards;


int Backtracker::max_phase = -1;
int Backtracker::max_step = 0;
bool Backtracker::bcktrk_steps_lines = false;
bool Backtracker::bcktrk_steps_inclined_lines = false;
bool Backtracker::bcktrk_steps_spiral = true;
bool Backtracker::bcktrk_order_same = true;
bool Backtracker::bcktrk_order_random = false;
bool Backtracker::impatient = false;
unsigned long long Backtracker::impatient_limit;
unsigned long long Backtracker::impatient_count;
Backtracker::position Backtracker::steps[MAX_X * MAX_Y];


void Backtracker::finished()
{
  area_write();
  area_save("area_backtrack_result.area");
  
  printf("OK\n");
  exit(0);
}


void Backtracker::prepare_steps()
{
  int i = 0;  // cislo kroku

  // levy horni roh
  steps[i].x = 1;
  steps[i++].y = 1;

  // pravy horni roh
  steps[i].x = area_max_x;
  steps[i++].y = 1;

  // pravy spodni roh
  steps[i].x = area_max_x;
  steps[i++].y = area_max_y;

  // levy spodni roh
  steps[i].x = 1;
  steps[i++].y = area_max_y;

  // horni strana
  for (int j = 0; j < area_max_x - 2; j++)
  {
    steps[i].x = j + 2;
    steps[i++].y = 1;
  }

  // prava strana
  for (int j = 0; j < area_max_y - 2; j++)
  {
    steps[i].x = area_max_x;
    steps[i++].y = j + 2;
  }

  // spodni strana
  for (int j = 0; j < area_max_x - 2; j++)
  {
    steps[i].x = area_max_x - j - 1;
    steps[i++].y = area_max_y;
  }

  // leva strana
  for (int j = 0; j < area_max_y - 2; j++)
  {
    steps[i].x = 1;
    steps[i++].y = area_max_y - j - 1;
  }

  if (bcktrk_steps_lines)  // vnitrek - radkove
  {
    for (int j = 0; i < area_max_x * area_max_y; j++)
    {
      steps[i].x = j % (area_max_x - 2) + 2;
      steps[i].y = j / (area_max_x - 2) + 2;
      //if (!fixed(steps[i].x, steps[i].y))
        i++;
    }
  }

  else if (bcktrk_steps_inclined_lines)  // vnitrek - sikme
  {
    // nejdrive zaciname nize a nize od leve strany
    // a jdeme vpravo nahoru
    for (int start_y = 2; start_y < area_max_y; start_y++)
      for (int x = 2, y = start_y; x < area_max_x && y > 1; x++, y--)
      {
        steps[i].x = x;
        steps[i++].y = y;
      }
    // a pote zaciname od spodni strany stale vice vpravo
    // a jdeme vpravo nahoru
    for (int start_x = 3; start_x < area_max_x; start_x++)
      for (int x = start_x, y = area_max_y - 1; x < area_max_x && y > 1;
          x++, y--)
      {
        steps[i].x = x;
        steps[i++].y = y;
      }
  }

  else if (bcktrk_steps_spiral)  // vnitrek - spirala
  {
    // kraje spiraly
    int min_x = 2, min_y = 2, max_x = area_max_x - 1, max_y = area_max_y - 1;
    // aktualni posun ve spirale
    int diff_x = 1, diff_y = 0;
    // aktualni pozice
    int x = min_x, y = min_y;
    // dokud jsme ve spirale (jdeme do stredu)
    while ((min_x <= max_x) && (min_y <= max_y))
    {
      steps[i].x = x; // ulozeni
      steps[i++].y = y;

      x += diff_x;  // posun
      y += diff_y;
      // narazili jsme v danem smeru na kraj?
      // pokud ano, je nutne zmenit smer a upravit hranice
      if (diff_x == 1 && x >= max_x)
        { diff_x = 0; diff_y = 1; min_y++; }
      else if (diff_y == 1 && y >= max_y)
        { diff_x = -1; diff_y = 0; max_x--; }
      else if (diff_x == -1 && x <= min_x)
        { diff_x = 0; diff_y = -1; max_y--; }
      else if (diff_y == -1 && y <= min_y)
        { diff_x = 1; diff_y = 0; min_x++; }
    }
    steps[i].x = x;  // stred spiraly, nedojdeme k nemu v cyklu
    steps[i++].y = y;
  }

  // vypsani vytvoreneho poradi
  printf("\nporadi:\n");
  for (int y = 1; y <= area_max_y; y++)
  {
    for (int x = 1; x <= area_max_x; x++)
    {
      bool was = false;
      for (int step = 0; step < area_max_x * area_max_y; step++)
        if (steps[step].x == x && steps[step].y == y && (was = true))
          printf("%3d ", step);
      if (!was)
        printf("--- ");
    }
    printf("\n");
  }
}


void Backtracker::go_backtrack(int phase, int step)
{
  // otestujeme, zda-li jsme se nedostali tam, kde se doposud nikdo nedostal;
  // jestlize ano, ulozime stav plochy do souboru area_backtrack_temp.area
  if (phase > max_phase)
  {
    max_phase = phase;
    max_step = step;
    //printf("max %i-%i\n", max_phase, max_step);
    area_save("area_backtrack_temp.area");
  }
  else if (phase == max_phase && step > max_step)
  {
    max_step = step;
    //printf("max %i-%i\n", max_phase, max_step);
    area_save("area_backtrack_temp.area");
  }

  // pokud provadime netrpelivy backtracking a provedli jsme jiz vice iteraci,
  // nez je limit, nebudeme se dale zanorovat => konec daneho backtrackingu
  if (impatient)
  {
    if (++impatient_count >= impatient_limit)
      return;
  }

  int x, y;  // souradnice na plose

  if (phase == 0)  // prace se zafixovanymi kartickami
  {
    if (step >= MAX_FIXED)  // jiz jsme zrotovali vsechny zafixovane karticky
    {
      go_backtrack(phase + 1, 0);
      return;
    }
    x = area_fixed_x[step];
    y = area_fixed_y[step];
    go_backtrack(phase, step + 1);  // bez rotace
    for (int rot = 1; rot < 4; rot++)  // vsechny moznosti rotace
    {
      int tmp[4];
      for (int side = 0; side < 4; side++)
        tmp[side] = area[y][x][side];
      for (int side = 0; side < 4; side++)
        area[y][x][side] = tmp[(side + rot) % 4];
      go_backtrack(phase, step + 1);  // s rotaci
    }
    return;
  }

  else if (phase == 1)  // normalni backtracking - jakekoliv karticky
  {
    if (step >= area_max_x * area_max_y)  // neni uz co => konec
    {
      finished();
      return;
    }

    x = steps[step].x;
    y = steps[step].y;

    // se zafixovanou kartickou nemuzeme nic delat
    if (fixed(x, y))
    {
      go_backtrack(phase, step + 1);
      return;
    }

    int pattern[4];
    set_pattern(x, y, pattern);  // cemu musi karticka vyhovovat?
  
    t_symbol_card* symbol_card;
    symbol_card = get_first_symbol_card(pattern);

    while (symbol_card != NULL)  // dokud jsou vyhovujici karticky
    {
      if ((*symbol_card->count) > 0)  // smime pouzit pouze volne karticky
      {
        card_copy(symbol_card->rotated, area[y][x]);  // umistime na plochu
        (*symbol_card->count)--;  // o jednu volnou karticku daneho typu mene

        go_backtrack(phase, step + 1);  // pokracujeme v hledani reseni

        card_copy(empty_card, area[y][x]);  // policko na plose se uvolnilo
        (*symbol_card->count)++;  // karticka take
      }

      // pokud jsme si vytvorili svuj vlastni seznam, tak jej jiz v budoucnu
      // nebudeme potrebovat, proto budeme kazdy dale nepotrebny prvek
      // ihned mazat
      if (bcktrk_order_random)
      {
        t_symbol_card* old_card = symbol_card;
        symbol_card = symbol_card->next;  // dalsi vyhovujici karticka
        delete old_card;  // smazani onoho v budoucnu nepotrebneho prvku
      }
      else  // nevytvareli jsme si novy seznam, pouze se presuneme dale
        symbol_card = symbol_card->next;
    }
  }

}


void Backtracker::backtracker()
{
  printf("backtracker... ");
  fflush(stdout);
  area_load("area.area"); // nacte plochu
  // naplni symbol cards - mozne karticky pro vsechny vzory
  create_symbol_cards();
  area_clean(); // vyprazdni plochu a vytvori sede pres-okraje
  prepare_steps();  // pripravi poradi policek pro projiti
  go_backtrack(0, 0); // backtracking
  printf("konec\n");
}


void Backtracker::impatient_backtracker(unsigned long long limit)
{
  printf("netrpelivy backtracker... ");
  fflush(stdout);
  area_load("area.area"); // nacte plochu
  // naplni symbol cards - mozne karticky pro vsechny vzory
  create_symbol_cards();
  area_clean(); // vyprazdni plochu a vytvori sede pres-okraje
  prepare_steps();  // pripravi poradi policek pro projiti

  // prirazeni netrpeliveho backtrackingu
  impatient = true;
  impatient_limit = limit;

  // ukonceno, pouze pokud zavolany backtracking stihne nalezt a ulozit reseni
  while (true)
  {
    printf(".");
    shake_all_symbol_cards();
    impatient_count = 0;
    go_backtrack(0, 0);
  }
}
