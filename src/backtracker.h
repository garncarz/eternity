/**
 * @file backtracker.h
 * hledani reseni pomoci back trackingu;
 * temp ukladani plochy se stavem nejblize cili
 */

#ifndef BACKTRACKER_H
#define BACKTRACKER_H

#include "define.h"


/**
 * namespace obalujici veci starajici se o backtracking reseni problemu
 */ 
namespace Backtracker
{
  /**
   * maximalni faze, kam jsme zatim dospeli
   */   
  extern int max_phase;

  /**
   * maximalni krok v maximalni fazi, kde jsme doposud dospeli
   */   
  extern int max_step;
  
  /**
   * radkovy postup pri backtrackingu
   */
  extern bool bcktrk_steps_lines;
  
  /**
   * postup sikmymi radky pri backtrackingu
   */
  extern bool bcktrk_steps_inclined_lines;
  
  /**
   * spiralovity postup pri backtrackingu
   */
  extern bool bcktrk_steps_spiral;
  
  /**
   * pri pokladani karticek budeme vzdy vybirat ze stejne setrideneho seznamu 
   */
  extern bool bcktrk_order_same;
  
  /**
   * pri pokladani karticek budeme vzdy vybirat z nahodne setrideneho seznamu 
   */
  extern bool bcktrk_order_random;

  /**
   * provadime netrpelivy backtracking?
   */   
  extern bool impatient;

  /**
   * limit poctu jakehokoliv dosazeni karticky pri netrpelivem backtrackingu
   */   
  extern unsigned long long impatient_limit;

  /**
   * aktualni pocet jakehokoliv dosazeni karticky pri netrpelivem backtrackingu
   */   
  extern unsigned long long impatient_count;

  /**
   * pozice policka na plose
   */   
  struct position
  {
    /**
     * x-ova souradnice
     */
    int x;
    
    /**
     * y-ova souradnice
     */
    int y;
  };

  /**
   * v jakem poradi vezmeme policka na plose utokem
   */   
  extern position steps[MAX_X * MAX_Y];


  /**
   * vyhra!
   */   
  void finished();

  /**
   * pripravi poradi policek na plose pro zkouseni
   */   
  void prepare_steps();

  /**
   * backtracking reseni problemu
   * @param phase faze reseni (0 - reseni zafixovanych karticek,
   *    1 - reseni zbytku)
   * @param step krok v dane fazi
   */   
  void go_backtrack(int phase, int step);

  /**
   * spusti backtracking
   */   
  void backtracker();

  /**
   * spousti netrpelivy backtracking - pocet jeho pokusu urcen limitem;
   * pro kazdy novy backtracking jsou karticky nahodne zamichany
   * @param limit limit pokusu kazdeho nove rozbehnuteho backtrackingu   
   */   
  void impatient_backtracker(unsigned long long limit);
}

#endif // BACKTRACKER_H
