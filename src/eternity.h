/**
 * @file eternity.h
 * obsluha reseni problemu
 */

/** @mainpage
 * @image html http://www.thetechnodrome.com/images/albums/Krang_and_Shredder_Together/shredandkrang1.jpg
 */

#ifndef ETERNITY_H
#define ETERNITY_H

#include <time.h>


/**
 * namespace obalujici obsluhu reseni problemu
 */
namespace Eternity
{
  /**
   * zacatek mereni casu
   */
  extern time_t rawtime_start;
  
  /**
   * konec mereni casu
   */
  extern time_t rawtime_end;

  /**
   * napoveda pro pouziti
   */
  void help();
  
  /**
   * chyba => vypise napovedu a ukonci program
   */
  void error();
  
  /**
   * funkce spustena pri jakemkoliv ukonceni programu
   */
  void end();
  
  /**
   * ukoncuje program pri Ctrl+C (pro vypsani doby behu)   
   */
  void terminate(int);
  
  /**
   * hlavni funkce programu
   * @param argn pocet argumentu
   * @param arg argumenty programu
   * @return navratova hodnota behu programu
   */
  int main(int argn, char **arg);
}

#endif // ETERNITY_H
