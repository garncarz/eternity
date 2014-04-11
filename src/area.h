/**
 * @file area.h
 * sprava hraci plochy
 */

#ifndef AREA_H
#define AREA_H

#include "define.h"


/**
 * namespace obalujici veci starajici se o hraci plochu
 */
namespace Area
{
  /**
   * typ pro plochu[radky][sloupce][strany]
   */
  typedef int area_t[MAX_Y + 2][MAX_X + 2][4];
  
  /**
   * plocha
   */
  extern area_t area;
  
  /**
   * realna sirka plochy
   */
  extern int area_max_x;
  
  /**
   * realna vyska plochy
   */
  extern int area_max_y;
  
  /**
   * realny pocet symbolu
   */
  extern int area_max_symbols;
  
  // pokud je MAX_FIXED rovno nule (nebo-li nechceme zafixovane karticky),
  // musime vytvorit dana pole s velikosti alespon 1,
  // ackoliv pouzivany nebudou
  #if MAX_FIXED > 0
  /**
   * velikost pole se zafixovanymi kartickami - ochrana pred nulovou velikosti
   */
  #define VIRTUAL_MAX_FIXED MAX_FIXED
  #else
  /**
   * velikost pole se zafixovanymi kartickami - ochrana pred nulovou velikosti
   */
  #define VIRTUAL_MAX_FIXED 1
  #endif
  
  /**
   * x souradnice zafixovanych karticek
   */
  extern int area_fixed_x[VIRTUAL_MAX_FIXED];
  
  /**
   * y souradnice zafixovanych karticek
   */
  extern int area_fixed_y[VIRTUAL_MAX_FIXED];
  
  /**
   * zafixovane karticky
   */   
  extern int area_fixed_card[VIRTUAL_MAX_FIXED][4];
  
  /**
   * prazdna karta = neobsazene policko na plose
   */
  const int empty_card[] = {UNDEFINED, UNDEFINED, UNDEFINED, UNDEFINED};
  
  /**
   * nacte plochu ze souboru do promenne plochy
   * @param filename soubor pro nacteni plochy
   * @param to_area cilova plocha, defaultne defaultni plocha area
   */
  void area_load(const char *filename, area_t to_area = area);
  
  /**
   * ulozi plochu do souboru
   * @param filename cilovy soubor
   */
  void area_save(const char *filename);
  
  /**
   * vypise plochu na std. vystup
   */
  void area_write();
  
  /**
   * vyprazdni plochu
   */
  void area_clean();
  
  /**
   * zkopiruje plochu
   * @param from_area puvodni plocha
   * @param to_area cilova plocha
   */
  void area_copy(area_t from_area, area_t to_area);
  
  /**
   * je karticka na dane pozici zafixovana?
   * @param x x-ova souradnice karticky
   * @param y y-ova souradnice karticky
   * @return fixace karticky
   */
  bool fixed(int x, int y);
  
  /**
   * vrati nahodny symbol
   * @return nahodny symbol
   */
  int rand_symbol();
  
  /**
   * vytvori nahodnou plochu
   * @param new_fixed vytvaret nove fixovane karticky?
   */
  void area_create(bool new_fixed = true);
  
  /**
   * promicha plochu
   */
  void area_randomize();
}

#endif // AREA_H
