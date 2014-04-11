/**
 * @file pseudognt.h
 * hledani reseni pomoci algoritmu inspirovaneho genetickymi algoritmy;
 * algoritmus neco o problemu vi; nepracuje se s retezci, ale rovnou s hraci
 * plochou - nekvalitni karticky mutuji (zadna reprodukce a krizeni)
 */

#ifndef PSEUDOGNT_H
#define PSEUDOGNT_H

#include "define.h"
#include "cards.h"


/**
 * namespace obalujici veci starajici se o reseni problemu
 * prvni implementaci pseudo-genetickeho algoritmu
 */ 
namespace PseudoGnt
{
  /**
   * pocty karticek zadane plochy pro kazdou moznou kombinaci vzoru
   */   
  extern Cards::cards_count_t orig_cards_count;

  /**
   * pocty karticek aktualni plochy pro kazdou moznou kombinaci vzoru
   */   
  extern Cards::cards_count_t cards_count;

  /**
   * kvalita jednotlivych karticek na plose (od 0 do 1)
   */   
  extern double card_quality[MAX_Y][MAX_X];

  /**
   * celkova kvalita aktualni plochy/populace
   */   
  extern double quality;

  /**
   * delka opakovani priblizne stejne kvality populace
   */   
  extern int quality_const_length;

  /**
   * vraci realne nahodne cislo
   * @return nahodne cislo   
   */   
  double double_rand();

  /**
   * vrati kvalitu aktualni populace
   * @return kvalita aktualni populace   
   */   
  double population_quality();

  /**
   * zmutuje karticku na danych souradnicich;
   * dava pozor na sede okraje;
   * dava pozor na shodu barev s okolim - pokud ma sousedici karticka vysokou
   * kvalitu, okopiruje jeji stranu; jinak (nebo pokud zvitezi nahoda)
   * zmeni i danou stranu sousedici karticky
   * @param x x-ova souradnice karticky
   * @param y y-ova souradnice karticky      
   */   
  void rand_card_for(int x, int y);

  /**
   * zmeni karticku na souradnicich x, y na nahodnou
   * (vcetne casti sousedicich  karticek)
   * @param x x-ova souradnice karticky
   * @param y y-ova souradnice karticky      
   */   
  void force_rand_card_for(int x, int y);

  /**
   * nahodne zmeni cast karticek plochy
   */   
  void rand_some_cards();

  /**
   * pokusi se vylepsit populaci
   */   
  void better_population();

  /**
   * vypis plochy spolecne s kvalitami (plochy & jednotlivych karticek)
   */   
  void area_pseudognt_write();

  /**
   * spusti reseni pseudo-genetickym algoritmem
   */      
  void pseudognt();
}

#endif // PSEUDOGNT
