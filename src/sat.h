/**
 * @file sat.h
 * priprava SAT zapisu problemu ve formatu Simplify
 */

#ifndef SAT_H
#define SAT_H


/**
 * namespace obalujici veci starajici se o zapis problemu v SAT formatu
 */ 
namespace Sat
{
  /**
   * soubor pro zapis SAT podminky
   */
  extern FILE *fp;


  /**
   * vytiskne retezec pro karticku na pozici
   * @param card_nr cislo karticky
   * @param x x-ova souradnice karticky
   * @param y y-ova souradnice karticky
   */
  void card_position(int card_nr, int x, int y);
  
  /**
   * vytiskne retezec pro pomocnou promennou pro podminky ohledne
   * maximalne jedne karticky na pozici v urcitem intervalu
   * @param x x-ova souradnice karticky
   * @param y y-ova souradnice karticky
   * @param left leva hranice intervalu
   * @param right prava hranice intervalu
   */
  void one_position_parent(int x, int y, int left, int right);

  /**
   * vytiskne retezec pro rotaci karticky
   * @param card_nr cislo karticky
   * @param rot rotace karticky
   */
  void card_rotation(int card_nr, int rot);

  /**
   * vytiskne retezec pro symbol na strane na pozici (v plose)
   * @param x x-ova souradnice na plose
   * @param y y-ova souradnice na plose
   * @param side strana na danych souradnicich
   * @param symbol symbol na dane strane
   */
  void area_symbol(int x, int y, int side, int symbol);
  
  /**
   * vytiskne retezec pro pomocnou promennou pro podminky ohledne
   * maximalne jednoho symbolu na strane na pozici v urcitem intervalu
   * @param x x-ova souradnice na plose
   * @param y y-ova souradnice na plose
   * @param side strana na danych souradnicich
   * @param left leva hranice intervalu
   * @param right prava hranice intervalu
   */
  void one_symbol_parent(int x, int y, int side, int left, int right);
  
  /**
   * vytiskne pravidla pro fakt, za na dane pozici muze byt jen karticka
   * budto z prvni nebo druhe poloviny rozmezi karticek;
   * a pote rekurzivne na jednotlive poloviny (=>LOG N)
   * @param x x-ova souradnice na plose
   * @param y y-ova souradnice na plose
   * @param card_left leva hranice rozmezi
   * @param card_right prava hranice rozmezi
   */
  void max_one_card_on_position(int x, int y, int card_left, int card_right);

  /**
   * vytiskne podminku pro fakt, ze kazda karticka je prave jednou na plose
   * a na kazde pozici je prave jedna karticka
   */
  void cards_on_area();

  /**
   * vytiskne podminku pro symboly na plose podle pozic a rotaci karticek
   */
  void rotated_cards_area_content();

  /**
   * vytiskne podminku pro sedou stranu side na karticce dane pozici
   * @param x x-ova souradnice plochy
   * @param y y-ova souradnice plochy
   * @param side strana na danych souradnicich
   */
  void gray_border(int x, int y, int side);
  
  /**
   * vytiskne pravidla pro fakt, za na dane strane dane pozice muze byt jen
   * symbol budto z prvni nebo druhe poloviny rozmezi symbolu;
   * a pote rekurzivne na jednotlive poloviny (=>LOG N)
   * @param x x-ova souradnice na plose
   * @param y y-ova souradnice na plose
   * @param side strana na danych souradnicich
   * @param symbol_left leva hranice rozmezi
   * @param symbol_right prava hranice rozmezi
   */
  void max_one_symbol_on_side(int x, int y, int side, int symbol_left,
    int symbol_right);

  /**
   * vytiskne pravidla pro prave jeden symbol na dane pozici
   * (sousedici pozice => nesedy symbol)
   * @param x x-ova souradnice na plose
   * @param y y-ova souradnice na plose
   * @param side strana na danych souradnicich
   */
  void neighbour_symbol(int x, int y, int side);

  /**
   * vytiskne podminku pro prave jeden symbol na kazdem miste
   */
  void right_symbols();

  /**
   * spusti zapis problemu v Simplify SAT formatu
   * @param neg mame celou podminku negovat? (pro hledani protiprikladu)
   */
  void sat(bool neg);
}

#endif // SAT_H
