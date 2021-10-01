/** @file
 Interfejs parsera wiersza danych wejściowych

 @author Julia Karmowska
 @date 2021
*/

#ifndef POLYNOMIALS_PARSER_H
#define POLYNOMIALS_PARSER_H

#include "poly.h"
#include "parser_structs.h"



/**
 * Wczytuje wiersz i wyznacza jego typ. Jeśli wiersz jest typu POLY, to przypisuje wczytany
 * wielomian zmiennej @f$poly@f$. Jeśli wiersz jest typu AT lub DEG_BY, przypisuje odpowiednio
 * zmiennym @f$at_val@f$ lub @f$deg_by_var@f$ wczytaną wartość argumentu.
 * @result struktura zawierająca typ wiersza i parametr
 */
extern ParseResult ParseLine();

#endif //POLYNOMIALS_PARSER_H
