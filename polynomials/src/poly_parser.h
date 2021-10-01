/** @file
 Interfejs parsera wielomianu

 @author Julia Karmowska
 @date 2021
*/

#ifndef POLYNOMIALS_POLY_PARSER_H
#define POLYNOMIALS_POLY_PARSER_H

#include "poly.h"
#include "parser_structs.h"

/**
 * Wczytuje wielomian z wiersza. Jeśli w wierszu jest poprawny wielomian, zapamiętuje go.
 * Jeśli nie, zwraca WRONG_POLY.
 * @param[in] line : wiersz
 * @param[in] length : długość wiersza
 * @param[in] poly : wczytany wielomian
 * @return rodzaj wiersza
 */
extern LineType GetPoly(char *line, long length, Poly *poly);

#endif //POLYNOMIALS_POLY_PARSER_H
