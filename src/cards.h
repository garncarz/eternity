/**
 * @file cards.h
 * prace s kartickou/kartickami
 */

#ifndef CARDS_H
#define CARDS_H

#include "define.h"
#include "area.h"

using namespace Area;


/**
 * namespace obalujici veci starajici se o praci s kartickou/kartickami
 */
namespace Cards
{
  /**
   * typ pro evidenci karticky s urcitym vzorem
   */ 
  struct t_symbol_card
  {
    /**
     * pocet karticek s danym urcitym vzorem
     */
    int* count;
    
    /**
     * symboly na jednotlivych stranach, zrotovano tak, jak to ma byt
     */   
    int rotated[4];
    
    /**
     * dalsi karticka vyhovujici stejnemu vzoru
     */
    t_symbol_card* next;
  };
  
  /**
   * typ pro pocty karticek pro kazdou moznou kombinaci vzoru,
   * pocitame i se sedymi vzory
   */
  typedef int cards_count_t[SYMBOLS + 1][SYMBOLS + 1][SYMBOLS + 1][SYMBOLS + 1];
  
  /**
   * 4-rozmerne pole s ukazateli na prvni karticku vyhovujici danemu vzoru;\n
   * rozmery: [sever][vychod][jih][zapad];\n
   * symboly: jednotlive symboly/barvy, sedy, nedefinovany (muze byt cokoliv);\n
   * kazda karticka tedy bude obsazena vicekrat, nejvice karticek bude\n
   * v [UNDEFINED][UNDEFINED][UNDEFINED][UNDEFINED] - vsechny karticky
   */ 
  extern t_symbol_card* symbol_cards[MAX_SYMBOLS][MAX_SYMBOLS]
    [MAX_SYMBOLS][MAX_SYMBOLS];
  
  /**
   * vsechny moznosti, jak "zakryt" strany karticky
   * => pro ziskani vsech moznych undefined stavu
   */
  const int symbol_mask[16][4] =
  {
    {0, 0, 0, 0},
    {0, 0, 0, 1},
    {0, 0, 1, 0},
    {0, 0, 1, 1},
    {0, 1, 0, 0},
    {0, 1, 0, 1},
    {0, 1, 1, 0},
    {0, 1, 1, 1},
    {1, 0, 0, 0},
    {1, 0, 0, 1},
    {1, 0, 1, 0},
    {1, 0, 1, 1},
    {1, 1, 0, 0},
    {1, 1, 0, 1},
    {1, 1, 1, 0},
    {1, 1, 1, 1}
  };
  
  
  /**
   * naplni symbol_cards - odkazy na vsechny karticky podle
   * vsech moznych kombinaci nutnych symbolu
   */
  void create_symbol_cards();
  
  /**
   * vraci zamichany seznam symbol cards, kde vstupem je prvni symbol card
   * originalniho seznamu; nema vliv na originalni seznam
   * @param _sc prvni symbol card originalniho seznamu
   * @return zamichany seznam symbol cards      
   */
  t_symbol_card* shaked_symbol_cards(t_symbol_card* _sc);
  
  /**
   * nahodne zamicha vsechny symbol cards
   */
  void shake_all_symbol_cards();
  
  /**
   * vraci ukazatel na prvni symbol card, ktera vyhovuje danemu vzoru
   * @param pattern vzor symbolu na vsech stranach karticky
   * @return prvni symbol card vyhovujici danemu vzoru
   */
  t_symbol_card* get_first_symbol_card(int pattern[]);
  
  /**
   * spocita karticky pro kazdou moznou kombinaci vzoru,
   * pocitame i se sedymi vzory; pocty ulozi do count
   * @param count zde se ulozi vysledek
   */
  void count_cards(cards_count_t count);
  
  /**
   * spocita karticky pro kazdou moznou kombinaci vzoru,
   * pocitame i se sedymi vzory; pocty ulozi do count;\n
   * SAFE varianta - kontroluje platnost symbolu
   * @param count zde se ulozi vysledek
   */   
  void count_cards_safe(cards_count_t count);
  
  /**
   * zkopiruje karticku
   * @param from originalni karticka
   * @param to cilova karticka
   * @param rot rotace, defaultne nulova
   */
  void card_copy(const int from[], int to[], int rot = 0);
  
  /**
   * overi shodnost dvou karticek, i pres rotaci
   * @param card1 prvni karticka
   * @param card2 druha karticka
   * @return jsou karticky shodne?
   */
  bool cards_equal(const int card1[], const int card2[]);
  
  /**
   * nastavi vzor pro danou pozici podle sousedu
   * @param x x-ova souradnice na plose
   * @param y y-ova souradnice na plose
   * @param pattern zde se ulozi vysledek
   */   
  void set_pattern(int x, int y, int pattern[]);
  
  /**
   * vrati pocet sedych policek na karticce
   * @param card kontrolovana karticka
   * @return pocet sedych policek na karticce
   */
  int gray_count(const int card[]);
  
  /**
   * zkontroluje karticky - alespon seda policka;
   * v pripade poruseni pravidel ukonci program
   * @param check_area plocha ke zkontrolovani
   */
  void check_cards(Area::area_t check_area = area);
}

#endif // CARDS_H
