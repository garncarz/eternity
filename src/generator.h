/**
 * @file generator.h
 * generuje nahodnou hraci plochu
 */

#ifndef GENERATOR_H
#define GENERATOR_H


/**
 * namespace obalujici funkci pro generovani nahodne plochy
 */
namespace Generator
{
  /**
   * vygeneruje nahodnou hraci plochu: jak nezamichanou (area_orig.area),
   * tak zamichanou (area.area);
   * mozny problem: zastoupeni symbolu nemusi byt rovnomerne
   */
  void generator();
}

#endif // GENERATOR_H
