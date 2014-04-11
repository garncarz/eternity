/**
 * @file cards.cpp
 * prace s kartickou/kartickami
 */

#include <stdlib.h>
#include <stdio.h>

#include "cards.h"
#include "backtracker.h"


Cards::t_symbol_card* Cards::symbol_cards[MAX_SYMBOLS][MAX_SYMBOLS]
  [MAX_SYMBOLS][MAX_SYMBOLS];


void Cards::create_symbol_cards()
{
  // vynulovani
  for (int north = 0; north < MAX_SYMBOLS; north++)
    for (int east = 0; east < MAX_SYMBOLS; east++)
      for (int south = 0; south < MAX_SYMBOLS; south++)
        for (int west = 0; west < MAX_SYMBOLS; west++)
          symbol_cards[north][east][south][west] = NULL;

  // pro kazdou karticku na hraci plose
  for (int y = 1; y <= area_max_y; y++)
  {
    for (int x = 1; x <= area_max_x; x++)
    {
      if (fixed(x, y))
        continue; // se zafixovanymi kartickami pracovat nebudeme

      // pokud jiz dana karticka existuje a narazili jsme na dalsi,
      // pouze zvysime pocet daneho typu karticky
      if (symbol_cards[area[y][x][NORTH]][area[y][x][EAST]]
        [area[y][x][SOUTH]][area[y][x][WEST]] != NULL)
      {
        (*symbol_cards[area[y][x][NORTH]][area[y][x][EAST]]
          [area[y][x][SOUTH]][area[y][x][WEST]]->count)++;
      }
      else {  // jinak ji musime teprve vytvorit
        // pocet dane karticky - budou se na nej odvolavat
        // vsechny rotace a maskovani
        int* card_count = new int;
        *card_count = 1;

        // pro kazdy typ zakryti stran karticky symbolem UNDEFINED
        for (int mask_nr = 0; mask_nr < 16; mask_nr++)
        {
          // indexovaci strany - kam do symbol_cards budeme zrovnat ukladat
          int index_side[4];
          for (int side = 0; side < 4; side++)
            index_side[side] = (symbol_mask[mask_nr][side] == 1)
              ? area[y][x][side] : UNDEFINED;

          // pro kazdou rotaci
          for (int rot = 0; rot < 4; rot++)
          {
            // nova symbol_card se spravne zrotovanymi stranami
            // a ukazatelem na pocet daneho typu karticky
            t_symbol_card* symbol_card = new t_symbol_card;
            card_copy(area[y][x], symbol_card->rotated, rot);
            symbol_card->count = card_count;

            // pridame na spravnou pozici do symbol_cards na zacatek
            symbol_card->next = symbol_cards
              [index_side[(0 + rot) % 4]]
              [index_side[(1 + rot) % 4]]
              [index_side[(2 + rot) % 4]]
              [index_side[(3 + rot) % 4]];
            symbol_cards
              [index_side[(0 + rot) % 4]]
              [index_side[(1 + rot) % 4]]
              [index_side[(2 + rot) % 4]]
              [index_side[(3 + rot) % 4]] = symbol_card;
          }  // for rot
        }  // for maskr_nr
      }  // else
    }  // for x
  }  // for y

}


Cards::t_symbol_card* Cards::shaked_symbol_cards(Cards::t_symbol_card* _sc)
{
  // abychom neovlivnili puvodni seznam
  t_symbol_card* sc = _sc;

  // inicializace lokalniho pole pro symbol cards
  // *4 => kazda karticka v poli muze byt teoreticky 4* - vzdy jinak natocena
  t_symbol_card* sc_ar[MAX_X * MAX_Y * 4];
  for (int i = 0; i < MAX_X * MAX_Y * 4; i++)
    sc_ar[i] = NULL;

  // vytvoreni kopii puvodnich symbol cards do lokalniho pole
  // a spocitani jejich poctu
  int size;
  for (size = 0; sc != NULL; size++, sc = sc->next)
  {
    sc_ar[size] = new t_symbol_card;
    sc_ar[size]->count = sc->count;
    card_copy(sc->rotated, sc_ar[size]->rotated);
  }
  
  // pro nulovou velikost jiz nic nemusime delat
  if (size == 0)
    return NULL;
  
  // ziskani prvni nahodne karticky
  int pos = rand() % size;
  t_symbol_card* fsc = sc_ar[pos];  // prvni karticka
  sc_ar[pos] = NULL;  // jiz je tedy pouzita
  t_symbol_card* lsc = fsc;  // posledni karticka - nyni zaroven i prvni

  // dokud jsme nezkopirovali do noveho seznamu nejakou karticku...
  for (int remaining = size - 1; remaining > 0; remaining--)
  {
    // nahodna karticka
    do {
      pos = rand() % size;
    } while (sc_ar[pos] == NULL);

    // jeji pridani na konec seznamu
    lsc->next = sc_ar[pos];
    lsc = sc_ar[pos];

    // karticka jiz tedy neni k dispozici pro pridani do seznamu
    sc_ar[pos] = NULL;
  }

  // posledni karticka nema nasledovnika
  lsc->next = NULL;

  // vracime prvni karticku noveho seznamu
  return fsc;
}


void Cards::shake_all_symbol_cards()
{
  // pro vsechny moznosti
  for (int north = 0; north < MAX_SYMBOLS; north++)
    for (int east = 0; east < MAX_SYMBOLS; east++)
      for (int south = 0; south < MAX_SYMBOLS; south++)
        for (int west = 0; west < MAX_SYMBOLS; west++)
        {
          // vytvorime novy, zamichany seznam
          // a ulozime jej do symbol_cards
          t_symbol_card* sc = symbol_cards[north][east][south][west];
          t_symbol_card* shaked_sc = shaked_symbol_cards(sc);
          symbol_cards[north][east][south][west] = shaked_sc;

          // pote stary seznam smazeme
          while (sc != NULL)
          {
            t_symbol_card* sc_next = sc->next;
            delete sc;
            sc = sc_next;
          }
        }
}


Cards::t_symbol_card* Cards::get_first_symbol_card(int pattern[])
{
  using namespace Backtracker;  // pro zpusob razeni karticek
  if (bcktrk_order_same)
    // nedochazi k zadnym zmenam
    return symbol_cards[pattern[0]][pattern[1]][pattern[2]][pattern[3]];
  else if (bcktrk_order_random)
    // nahodne karticky promichame
    // (vytvorime novy seznam, bez vedlejsich efektu)
    return shaked_symbol_cards(
      symbol_cards[pattern[0]][pattern[1]][pattern[2]][pattern[3]]);
  return NULL;
}


void Cards::count_cards(cards_count_t count)
{
  // vynulovani poctu karticek
  for (int north = 0; north < SYMBOLS + 1; north++)
    for (int east = 0; east < SYMBOLS + 1; east++)
      for (int south = 0; south < SYMBOLS + 1; south++)
        for (int west = 0; west < SYMBOLS + 1; west++)
          count[north][east][south][west] = 0;

  // spocitani karticek - kazdou i zrotuje pro vsechny moznosti
  for (int y = 1; y <= area_max_y; y++)
    for (int x = 1; x <= area_max_x; x++)
      for (int rot = 0; rot < 4; rot++)
        count[area[y][x][(0 + rot) % 4]][area[y][x][(1 + rot) % 4]]
          [area[y][x][(2 + rot) % 4]][area[y][x][(3 + rot) % 4]]++;
}


void Cards::count_cards_safe(cards_count_t count)
{
  // vynulovani poctu karticek
  for (int north = 0; north < SYMBOLS + 1; north++)
    for (int east = 0; east < SYMBOLS + 1; east++)
      for (int south = 0; south < SYMBOLS + 1; south++)
        for (int west = 0; west < SYMBOLS + 1; west++)
          count[north][east][south][west] = 0;

  // spocitani karticek - kazdou i zrotuje pro vsechny moznosti
  for (int y = 1; y <= area_max_y; y++)
    for (int x = 1; x <= area_max_x; x++)
      if (area[y][x][0] < SYMBOLS + 1
          && area[y][x][1] < SYMBOLS + 1
          && area[y][x][2] < SYMBOLS + 1
          && area[y][x][3] < SYMBOLS + 1) 
        for (int rot = 0; rot < 4; rot++)
          count[area[y][x][(0 + rot) % 4]][area[y][x][(1 + rot) % 4]]
            [area[y][x][(2 + rot) % 4]][area[y][x][(3 + rot) % 4]]++;
}


void Cards::card_copy(const int from[], int to[], int rot)
{
  for (int side = 0; side < 4; side++)
    to[side] = from[(side + rot) % 4];
}


bool Cards::cards_equal(const int card1[], const int card2[])
{
  // nejdrive se pokusime nalezt spravnou synchronizaci rotaci karticek
  for (int side = 0; side < 4; side++)
    if (card1[side] == card2[0])
      // pro ni pote overujeme shodnost ostatnich stran
      for (int side2 = 0; side2 < 5; side2++)
      {
        if (side2 == 5)
          // vsechny strany shodne, karticky si jsou rovny
          return true;
        if (card1[(side + side2) % 4] != card2[side2])
          // karticky si na teto pozici nejsou rovny
          break;
      }
  // nebyla nalezena zadna rotace pro ekvivalenci karticek
  return false;
}


void Cards::set_pattern(int x, int y, int pattern[])
{
  pattern[NORTH] = area[y - 1][x][SOUTH];
  pattern[EAST] = area[y][x + 1][WEST];
  pattern[SOUTH] = area[y + 1][x][NORTH];
  pattern[WEST] = area[y][x - 1][EAST];
}


int Cards::gray_count(const int card[])
{
  int count = 0;
  for (int side = 0; side < 4; side++)
    if (card[side] == 0)
      count++;
  return count;
}


void Cards::check_cards(area_t check_area)
{
  int counts[3] = {0, 0, 0};
  
  for (int y = 1; y <= area_max_y; y++)
    for (int x = 1; x <= area_max_x; x++)
      counts[gray_count(check_area[y][x])]++;
  
  if (counts[2] != 4 || counts[1] != 2 * (area_max_x + area_max_y - 4) ||
    counts[0] != (area_max_x - 2) * (area_max_y - 2))
  {
    printf("chyba - seda policka nejsou dobre!\n");
    exit(1);
  }
}
