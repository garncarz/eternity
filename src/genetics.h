/**
 * @file genetics.h
 * hledani reseni pomoci genetickeho algoritmu;
 * populace je tvorena retezci, kde kazdy koduje urcity obsah hraci plochy;
 * retezce se reprodukuji, krizi a mutuji
 */

#ifndef GENETICS_H
#define GENETICS_H

#include "define.h"
#include "cards.h"

/**
 * maximalni velikost retezce (= maximalni pocet jeho bitu);
 * (20 bitu je maximalni delka pro jednu karticku - vyber z 32 symbolu (5 bitu)
 * pro 4 strany)
 */
#define MAX_STRING_LENGTH (MAX_X * MAX_Y * 20)

/**
 * maximalni velikost populace (pocet retezcu)
 */
// #define MAX_POPULATION_SIZE 30
#define MAX_POPULATION_SIZE 150


/**
 * namespace obalujici veci starajici se o reseni problemu
 * implementaci genetickeho algoritmu
 */
namespace Genetics
{
  /**
   * typ pro retezec
   */
  typedef bool t_string[MAX_STRING_LENGTH];

  /**
   * typ pro populaci
   */
  typedef t_string t_population[MAX_POPULATION_SIZE];

  /**
   * pocet bitu pro zakodovani pozice
   */
  extern int position_bits;
  
  /**
   * pocet bitu pro zakodovani rotace
   */
  extern int rotation_bits;
  
  /**
   * pocet bitu pro zakodovani symbolu
   */
  extern int symbol_bits;

  /**
   * velikost retezce (= pocet jeho bitu)
   */
  extern int string_length;

  /**
   * velikost populace
   */
  extern int population_size;

  /**
   * zatim nejlepsi kvalita retezce
   */
  extern double best_quality;
  
  /**
   * puvodni plocha - pro kopirovani a porovnavani
   */
  extern area_t orig_area;

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
   * dulezitost spravnych symbolu na kartickach (odpovidaji zadane plose)
   */
  extern double origin_importance;

  /**
   * pravdepodobnost krizeni
   */
  extern int crossover_likehood;

  /**
   * pravdepodobnost mutace
   */
  extern int mutation_likehood;


  /**
   * zkopiruje retezec string1 do retezce string2
   * @param string1 puvodni retezec
   * @param string2 cilovy retezec
   */
  void copy_string(t_string string1, t_string string2);

  /**
   * dekoduje dany retezec na aktualni hraci plochu podle symbolu na plose
   * @param string retezec obsahujici stav plochy
   */
  void area_decode_symbols(t_string string);
  
  /**
   * dekoduje dany retezec na aktualni hraci plochu podle pozic karticek
   * @param string retezec obsahujici stav plochy
   */
  void area_decode_positions(t_string string);

  /**
   * dekoduje dany retezec na aktualni hraci plochu
   * @param string retezec obsahujici stav plochy
   */
  extern void (*area_decode) (t_string string);

  /**
   * ohodnoti aktualni hraci plochu a kvalitu vrati
   * @return aktualni kvalita hraci plochy
   */
  double evaluate();

  /**
   * nalezne nejlepsi retezec populace
   * @param population populace
   * @param string_pos pozice nejlepsiho retezce (vracena hodnota)
   * @return kvalita nejlepsiho retezce
   */
  double find_best_string(t_population population, int *string_pos);

  /**
   * vyhleda pulenim intervalu hodnotu na rulete
   * @param value hledana hodnota
   * @param roulette ruleta
   * @return pozice hodnoty v rulete
   */
  int bin_search(double value, double roulette[]);

  /**
   * vytvori nahodne populaci
   * @param population nova populace
   */   
  void uprise(t_population population);

  /**
   * reprodukuje ruletovym system starou generaci populace old_gen
   * do nove generace populace new_gen
   * @param old_gen stara generace
   * @param new_gen nova generace
   */
  void reproduce(t_population old_gen, t_population new_gen);

  /**
   * zkrizi dva retezce - podle nahodne velkych "polovin"
   * @param first prvni retezec
   * @param second druhy retezec
   */
  void crossover_strings_halfs(t_string first, t_string second);
  
  /**
   * zkrizi dva retezce - kazdy bit nahodne
   * @param first prvni retezec
   * @param second druhy retezec
   */
  void crossover_strings_bits(t_string first, t_string second);
  
  /**
   * zkrizi dva retezce
   * @param first prvni retezec
   * @param second druhy retezec
   */
  extern void (*crossover_strings) (t_string first, t_string second);

  /**
   * zkrizi retezce stare generace populace old_gen
   * do nove generace populace new_gen
   * @param old_gen stara generace
   * @param new_gen nova generace
   */
  void crossover(t_population old_gen, t_population new_gen);

  /**
   * zmutuje danou populaci
   * @param population populace
   */
  void mutate(t_population population);

  /**
   * vypise retezec (vcetne cisla generace, kvality retezce a retezce
   * dekodovaneho jako hraci plocha)
   * @param generation_count poradi generace
   * @param string retezec znazornujici plochu
   * @param quality kvalita daneho retezce
   */
  void write_string(long generation_count, t_string string, double quality);

  /**
   * spusti reseni genetickym algoritmem
   */
  void genetics();
}

#endif // GENETICS_H
