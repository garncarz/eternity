/**
 * @file annealing.h
 * reseni problemu simulovanym zihanim
 */

#ifndef ANNEALING_H
#define ANNEALING_H

/**
 * namespace obalujici veci starajici se o reseni problemu
 * simulovanym zihanim
 */
namespace Annealing
{
  /**
   * vrati energii aktualni oceli
   * @return aktualni energie oceli
   */
  double count_energy();
  
  /**
   * upravi plochu area do sousedni plochy (velmi podobne plochy) - ziha ocel
   */
  void neighbour_area();
  
  /**
   * vraci pravdepodobnost pro prechod plochy do noveho stavu
   * @param old_energy energie stareho stavu plochy
   * @param new_energy energie noveho stavu plochy
   * @param temperature aktualni teplota oceli
   * @return pravdepodobnost pro prechod plochy do noveho stavu
   */
  double probability(double old_energy, double new_energy, double temperature);
  
  /**
   * vraci aktualni teplotu oceli
   * @param step krok zihani
   * @return aktualni teplota oceli
   */
  double temperature(unsigned long long step);
  
  /**
   * vraci nahodne cislo v realnem intervalu <0, 1>
   * @return nahodne cislo v realnem intervalu <0, 1>
   */
  double double_rand();
  
  /**
   * spusti reseni problemu simulovanym zihanim
   */
  void anneal();
}

#endif // ANNEALING_H
