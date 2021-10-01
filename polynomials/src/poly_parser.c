/** @file
 Implementacja parsera wielomianów

 @author Julia Karmowska
 @date 2021
*/

#include <ctype.h>
#include <limits.h>
#include <errno.h>
#include "poly_parser.h"
#include "memory.h"

/**
 * Zwiększa rozmiar tablicy.
 * @param[in] n rozmiar
 * @return @f$ 1 + 2 * n @f$
 */
static size_t increase_size(size_t n)
{
    if (n == SIZE_MAX) //SIZE_MAX może zostać osiągnięty, bo n = 2^m - 1
        exit(1); //jeśli zostanie osiągnięty, to nie możemy już zwiększyć rozmiaru
    return 1 + 2 * n;
}

/**
 * Sprawdza, czy na początku tablicy znaków jaest poprawna liczba typu long.
 * @param[in] line : tablica znaków
 * @param[in] end : pierwszy znak za ostatnim wczytanym
 * @param[in] value : wczytana liczba
 * @return Czy na początku tablicy znaków jaest poprawna liczba typu long?
 */
static bool isLong(char *line, char **end, poly_coeff_t *value)
{
    if (line[0] == '\n' || line[0] == '\0')
        return false;
    long number;
    errno = 0;
    number = strtol(line, end, 10);
    if (errno != 0)
        return false;
    *value = number;
    return true;
}

/**
 * Sprawdza, czy na początku tablicy znaków jaest poprawna liczba typu unsigned long.
 * @param[in] line : tablica znaków
 * @param[in] end : pierwszy znak za ostatnim wczytanym
 * @param[in] value : wczytana liczba
 * @return Czy na początku tablicy znaków jaest poprawna liczba typu unsigned long?
 */
static bool isUnsignedLong(char *line, char **end, unsigned long *value)
{
    if (line[0] == '\n' || line[0] == '\0')
        return false;
    unsigned long long number;
    errno = 0;
    number = strtoull(line, end, 10);
    if (errno != 0)
        return false;

    *value = number;
    return true;
}

/**
 * Sprawdza, czy na początku tablicy znaków jest poprawny współczynnik wielomianu
 * (typ long, niepoprzedzony zankiem plus), po którym stoi przecinek. Jeśli tak, zapamiętuje wartość współczynnika.
 * @param[in] line : wiersz, tablica znaków
 * @param[in] coeff : wczytany współczynnik
 * @param[in] end : pierwszy znak, który nie został wczytany
 * @return Czy na początku @p line jest poprawny współczynnik wielomianu?
 */
static bool isCoeffPoly(char *line, poly_coeff_t *coeff, char **end)
{
    if (line[0] == '+')
        return false;

    if (isLong(line, end, coeff))
    {
        if (**end != ',')
            return false;
        return true;
    }
    return false;
}

/**
 * Sprawdza, czy na początku tablicy znaków jest poprawny wykładnik
 * (nieujemna liczba z zakresu int, niepoprzedzona zankiem plus), po którym stoi znak @f$)@f$.
 * Jeśli tak, zapamiętuje wartość wykładnika.
 * @param[in] line : wiersz, tablica znaków
 * @param[in] exp : wczytany wykładnik
 * @param[in] end : pierwszy znak, który nie został wczytany
 * @return Czy na początku @p line jest poprawny współczynnik wielomianu?
 */
static bool isExpPoly(char *line, poly_exp_t *exp, char **end)
{
    if (line[0] == '+')
        return false;
    unsigned long number;

    if (isUnsignedLong(line, end, &number))
    {
        if (**end != ')')
            return false;
        if (number > INT_MAX)
            return false;
        *exp = (int) number;
        return true;
    }
    return false;
}

/**
 * Jeśli to koniecczne, zwiększa rozmiar tablicy jednomianów.
 * @param[in] monos tablica jednomianów
 * @param[in] size rozmiar tablicy
 * @param[in] length liczba jednoianów w tablicy
 */
static void MaybeResizeMonos(Mono **monos, size_t *size, size_t length)
{
    if (length >= *size)
    {
        *size = increase_size(*size);
        *monos = realloc(*monos, (*size) * sizeof(Mono));
        CHECK_PTR(*monos);
    }

}

/**
 * Wczytuje jednomian postaci (coeff, exp), zapisuje go w tablicy jednomianów na najmniejszym wolnym indeksie.
 * Ustawia wskaźnik @p end na pierwsze miejsce w wierszu po wczytanym jednomianie.
 * @param[in] beg : początek wiersza, tablica znaków
 * @param[in] end : znak za ostatnim wczytanym
 * @param[in] monos : tablica jednomianów
 * @param[in] size : rozmiar tablicy
 * @param[in] used : liczba jednoianów w tablicy
 * @return  : Czy wczytano poprawny jednomian?
 */
static bool GetMono(char *beg, char **end, Mono **monos, size_t *size, size_t *used)
{//przed był '('
    assert(isdigit(beg[0]) || beg[0] == '-');
    poly_coeff_t coeff = 0;
    poly_exp_t exp = 0;
    if (isCoeffPoly(beg, &coeff, end))
    { //teraz **end = ','
        beg = *end + 1; //pierwszy znak po przecinku
        if (isExpPoly(beg, &exp, end))
        {   // **end = ')', jednomian jest poprawny
            Poly poly = PolyFromCoeff(coeff);
            MaybeResizeMonos(monos, size, *used);
            if (!PolyIsZero(&poly))
            {
                (*monos)[*used] = MonoFromPoly(&poly, exp);
                (*used)++;
            }
            (*end)++;
            return true;
        }
    }
    return false;
}

/**
 * Dodaje jednomian do tablicy
 * @param[in] monos : tablica jednomianów
 * @param[in] size : rozmiar tablicy
 * @param[in] count : liczba jednoianów w tablicy
 * @param[in] poly : wielomian
 * @param[in] exp : wykładnik
 */
static void AddMono(Mono **monos, size_t *size, size_t *count, Poly poly, poly_exp_t exp)
{
    MaybeResizeMonos(monos, size, *count);
    if (PolyIsZero(&poly))
        (*monos)[*count] = MonoFromPoly(&poly, 0);
    else (*monos)[*count] = MonoFromPoly(&poly, exp);
    (*count)++;
}

/**
 * Wczytuje sumę jednomianów do tablicy jednomianów.
 * @param[in] line : wiersz
 * @param[in] end : wskaźnik na ostatni wczytany znak
 * @param[in] monos : tablica jednomianów
 * @param[in] count : liczba wczytanych jednomianów
 * @param[in] size : rozmiar tablicy jednomianów
 * @param[in] limit : znak, którym powinna kończyć się wczytywana suma jednomianów
 * @return Czy udało się wczytać sumę jednomianów?
 */
static bool GetMonos(char *line, char **end, Mono **monos, size_t *count, size_t *size, char limit);

/**
 * Ustawia wskaźnik na dwa znaki po znaku '+', tak aby wskazywał na początek nowego jednomianu, po znaku '('.
 * Jeśli po znaku '+' jest niewłaściwa sekwencja, zwraca fałsz.
 * @param[in] beg : początek tablicy znaków
 * @return : Czy udało się wczytać znak '+' i początek nowego jednomianu jest poprawny?
 */
static bool SkipPlusChar(char **beg)
{
    assert(**beg == '+');
    if ((*beg)[1] != '(')
        return false;
    else
    {
        *beg = *beg + 2;
        if (!isdigit(**beg) && **beg != '(' &&
            **beg != '-') //sprawdzamy to tutaj, bo jeśli byłoby '\n', to dalej błąd nie zostałby wykryty
            return false;
    }
    return true;
}

/**
 * Usuwa z pamięci wielomian i tablicę jednomianów.
 * @param[in] monos : tablica jednomianów
 * @param[in] size :rozmiar tablicy
 */
static void DestroyMonos(Mono **monos, size_t size)
{
    for (size_t i = 0; i < size; i++)
    {
        MonoDestroy(&((*monos)[i]));
    }
    free(*monos);
}

/**
 * Przesuwa wskaźniki o jeden znak do przodu
 * @param[in] beg pierwszy znak do wczytania
 * @param[in] end ostatni wczytany znak
 */
static void SkipOneChar(char **beg, char **end)
{
    (*end)++;
    *beg = *end;

}

/**
 * Wczytauje wykładnik jednomianu, jeśli jest on poprawny
 * @param[in] end ostatni wczytany znak
 * @param[in] beg pierwszy znak do wczytania
 * @param[in] exp wykładnik
 * @return Czy udało się wczytać wykładnik?
 */
static bool GetExp(char **end, char **beg, poly_exp_t *exp)
{
    SkipOneChar(beg, end);
    return isExpPoly(*beg, exp, end);
}


/**
 * Wczytuje wnętrze nawiasu, czyli jednomian (element przed przecinkiem w nawiasie jest złożonym wielomianem),
 * ustawia wskaźnik @p end i @p beg na pierwszy znak po nawiasie zamykającym.
 * @param[in] beg : początek tablicy znaków
 * @param[in] end : ostatni wczytany znak
 * @param[in] monos : tablica jednomianów
 * @param[in] count : liczba jednomianów w tablicy
 * @param[in] size : rozmiar tablicy
 * @return : Czy zawartość nawiasu została poprawnie wczytana?
 */
static bool getBrackets(char **end, char **beg, Mono **monos, size_t *count, size_t *size)
{
    if (!isdigit((*beg)[1]) && (*beg)[1] != '(' && (*beg)[1] != '-')
        return false;
    size_t newCount = 0;
    size_t newSize = INITIAL_ARRAY_SIZE;
    Mono *newMonos = calloc(INITIAL_ARRAY_SIZE, sizeof(Mono));
    CHECK_PTR(newMonos);
    if (!GetMonos(*beg, end, &newMonos, &newCount, &newSize, ','))
    {
        DestroyMonos(&newMonos, newSize);
        return false;
    } else
    {
        poly_exp_t exp = 0;
        if (GetExp(end, beg, &exp))
        {
            Poly newPoly = PolyAddMonos(newCount, newMonos);
            free(newMonos);
            AddMono(monos, size, count, newPoly, exp);
            SkipOneChar(beg, end);
        } else
        {
            DestroyMonos(&newMonos, newSize);
            return false;
        }
    }
    return true;
}

/**
 * Wczytuje jednomian i ustawia wskaźnik beg na pierwszy znak za nawiasem zamykającym jednomian.
 * @param[in] beg : pierwszy znak jeszcze nie wczytany
 * @param[in] end : ostatni wczytany znak
 * @param[in] monos : tablica jednomianów
 * @param[in] count : liczba jednomianów
 * @param[in] size : rozmiar tablicy
 * @return Czy udało się wczytać jednomian?
 */
static bool SkipMono(char **beg, char **end, Mono **monos, size_t *count, size_t *size)
{
    if (!GetMono(*beg, end, monos, size, count)) //teraz end jest za )
        return false;

    *beg = *end;
    return true;
}

static bool GetMonos(char *line, char **end, Mono **monos, size_t *count, size_t *size, char limit)
{
    assert(line[0] == '(');
    char *beg = line + 1;//następny znak po '('
    if (*beg == limit || *beg == '\0')
        return false;
    while (*beg != limit && *beg != '\0')
    {
        if (isdigit(*beg) || *beg == '-')
        {
            if (!SkipMono(&beg, end, monos, count, size))
                return false;
        } else if (*beg == '+')
        {
            if (!SkipPlusChar(&beg))
                return false;
        } else if (*beg == '(')
        {
            if (!getBrackets(end, &beg, monos, count, size))
                return false;
        } else return false;
    }
    return true;
}

/**
 * Wczytuje liczbę. Liczba musi być jedynym wyrazem w wierszu.
 * @param[in] line : wiersz
 * @param[in] value : wczytana wartość
 * @return Czy wczytano poprawną wartość?
 */
static bool GetSingleNumber(char *line, poly_coeff_t *value)
{
    if (line[0] == '+')
        return false;
    char *end;
    if (isLong(line, &end, value))
    {
        if (*end != '\0' && *end != '\n')
            return false;
        return true;
    }
    return false;
}

/**
 * Wczytuje wielomian będący liczbą, jeśli to możliwe.
 * @param[in] line : wiersz
 * @param[in] poly  : wielomian
 * @return typ wiersza
 */
static LineType GetPolyCoeff(char *line, Poly *poly)
{
    poly_coeff_t value = 0;
    if (!GetSingleNumber(line, &value))
        return WRONG_POLY;

    *poly = PolyFromCoeff(value);
    return POLY;
}

/**
 * Określa typ wiersza zawierającego wielomian (poprawny lub nie).
 * @param[in] line : wiersz
 * @param[in] length : długość wiersza
 * @param[in] poly : wczytany wielomian
 * @return rodzaj wiersza
 */
static LineType GetPolyHelp(char *line, Poly *poly, long length)
{
    size_t count = 0;
    if (line[0] == '(' && length > 5)
    {
        char *end = line;
        Mono *monos = calloc(INITIAL_ARRAY_SIZE, sizeof(Mono));
        CHECK_PTR(monos);
        size_t size = INITIAL_ARRAY_SIZE;
        if (GetMonos(line, &end, &monos, &count, &size, '\n'))
        {
            *poly = PolyAddMonos(count, monos);
            free(monos);
            return POLY;
        } else
        {
            DestroyMonos(&monos, size);
            return WRONG_POLY;
        }
    }
    return GetPolyCoeff(line, poly); //jest tylko jedna liczba - stała
}


LineType GetPoly(char *line, long length, Poly *poly)
{
    for (int i = 0; i < length; i++)
    {
        if (!isdigit(line[i]) && line[i] != '(' && line[i] != ',' && line[i] != ')' && line[i] != '-' &&
            line[i] != '+' && line[i] != '\n')
            return WRONG_POLY;
    }
    return GetPolyHelp(line, poly, length);
}