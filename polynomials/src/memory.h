/** @file
 Makra do alokacji.

 @author Julia Karmowska
 @date 2021
*/

#ifndef POLYNOMIALS_MEMORY_H
#define POLYNOMIALS_MEMORY_H
#include <stdint.h>
#include <stdlib.h>

/**
 * To jest stała reprezentująca początkowy rozmiar tablicy
 */
#define INITIAL_ARRAY_SIZE 15

/**
 * Sprawdza, czy alokacja pamięci zakończyła się sukcesem.
 * @param[in] p : wskaźnik na zaalokowaną pamięć
 */
#define CHECK_PTR(p)  \
  do {                \
    if (p == NULL) {  \
      exit(1);        \
    }                 \
  } while (0)


#endif //POLYNOMIALS_MEMORY_H
