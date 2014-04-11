/**
 * @file define.h
 * soubor s obecnymi definicemi
 */

#ifndef DEFINE_H
#define DEFINE_H


// strany karticky:

/**
 * severni strana karticky
 */
#define NORTH 0

/**
 * vychodni strana karticky
 */
#define EAST 1

/**
 * jizni strana karticky
 */
#define SOUTH 2

/**
 * zapadni strana karticky
 */
#define WEST 3


// rozmery plochy a pocet symbolu (krome sedych):

/**
 * maximalni x rozmer plochy
 */
#define MAX_X 16

/**
 * maximalni y rozmer plochy
 */
#define MAX_Y 16

/**
 * maximalni pocet symbolu
 */
#define SYMBOLS 22


/**
 * hodnota pro libovolny symbol
 */
#define UNDEFINED (SYMBOLS + 1)

/**
 * maximalni pocet symbolu - vcetne sedych a nedefinovanych
 */
#define MAX_SYMBOLS (SYMBOLS + 2)

/**
 * pocet zafixovanych karticek
 */
#define MAX_FIXED 0


#endif // DEFINE_H
