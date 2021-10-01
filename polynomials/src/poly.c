/** @file
 Implementacja operacji na wielomianach rzadkich wielu zmiennych

 @author Julia Karmowska
 @date 2021
*/

#include <stdlib.h>
#include <stdio.h>
#include "poly.h"

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

/**
 * To jest stała reprezentująca stopień wielomianu zerowego
 */
#define DEG_OF_ZERO_POLY -1

/**
 * To jest stała reprezentująca dodatkową komórkę tablicy
 */
#define ONE_ARR_CELL_FOR_COEFF 1


void PolyDestroy(Poly *p)
{
    if (p == NULL)
        return;
    if (p->arr != NULL)
    {
        for (size_t i = 0; i < p->size; i++)
        {
            MonoDestroy(&p->arr[i]);
        }
        free(p->arr);
    }
}

Poly PolyClone(const Poly *p)
{
    if (p->arr == NULL)
        return PolyFromCoeff(p->coeff);

    Poly result;
    result.size = p->size;
    result.arr = calloc(result.size, sizeof(Mono));
    CHECK_PTR(result.arr);
    for (size_t i = 0; i < result.size; i++)
    {
        result.arr[i] = MonoClone(&p->arr[i]);
    }
    return result;
}

/**
 * Tworzy nowy wielomian z tablicą jednomianów o danym rozmiarze.
 * @param[in] size : rozmiar tablicy jednomianów
 * @return wielomian
 */
static Poly PolyNewFromSize(size_t size)
{
    Poly result;
    result.size = 0;
    result.arr = calloc(size, sizeof(Mono));
    CHECK_PTR(result.arr);
    return result;
}

/**
 * Wyznacza rozmiar tablicy jednomianów wielomianu będącego sumą dwóch wielomianów.
 * @param[in] p : wielomian
 * @param[in] q : wielomian
 * @return rozmiar tablicy jednomianów
 */
static size_t SizeOfAddArray(const Poly *p, const Poly *q)
{
    if (p->arr == NULL && q->arr == NULL)
        return 0;
    else if (p->arr != NULL && q->arr != NULL)
        return p->size + q->size;
    else if (p->arr != NULL)
        return p->size + ONE_ARR_CELL_FOR_COEFF;
    else
        return q->size + ONE_ARR_CELL_FOR_COEFF;
}

/**
 * Kopiuje jednomian z tablicy arr wielomianu p do tablicy arr wielomianu result
 * pod danymi indeksami oraz zwiększa indeksy.
 * @param[in] p : wielomian
 * @param[in] result : wielomian
 * @param[in] p_index : indeks w tablicy jednomianów wielomianu @p p
 * @param[in] res_index : indeks w tablicy jednomianów wielomianu @p result
 */
static void MonoCloneAndChangeIndex(const Poly *p, Poly *result, size_t *p_index, size_t *res_index)
{
    result->arr[*res_index] = MonoClone(&p->arr[*p_index]);
    (*p_index)++;
    (*res_index)++;
}

/**
 * Dodaje dwa wielomiany, dopóki nie dotrze do końca tablicy jednomianów jednego z nich.
 * Do wynikowego wielomianu wpisuje jednomiany w kolejności rosnącej według wykładników.
 * Jeśli wykładniki w obu wielomianach są równe, to jako współczynnik przy tym wykładniku w wielomianie @f$result@f$
 * wpisuje sumę współczynników z wielomianów @f$p@f$ i @f$q@f$.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @param[in] result : wielomian wynikowy @f$result@f$
 * @param[in] p_index : indeks w tablicy wielomianu @f$p@f$
 * @param[in] q_index : indeks w tablicy wielomianu @f$q@f$
 * @param[in] res_index : indeks w tablicy wielomianu @f$q@f$
 * */
static void
AddTwoPolyTillOneEnds(const Poly *p, const Poly *q, Poly *result, size_t *p_index, size_t *q_index, size_t *res_index)
{
    assert(p->arr != NULL && q->arr != NULL);
    if (p->arr[*p_index].exp == q->arr[*q_index].exp)
    {
        result->arr[*res_index].exp = p->arr[*p_index].exp;
        result->arr[*res_index].p = PolyAdd(&p->arr[*p_index].p, &q->arr[*q_index].p);

        //jeśli w wyniku dodawania otrzymaliśmy wielomian zerowy, to pomijamy go
        if (!PolyIsZero(&result->arr[*res_index].p))
            (*res_index)++;
        (*p_index)++;
        (*q_index)++;
    } else if (p->arr[*p_index].exp < q->arr[*q_index].exp)
        MonoCloneAndChangeIndex(p, result, p_index, res_index);
    else
        MonoCloneAndChangeIndex(q, result, q_index, res_index);
}

/**
 * Dodaje dwa niestałe wielomiany, wielomian @f$p@f$ ma większy ostatni wykładnik w tablicy jednomianów.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @param[in] result : wielomian wynikowy @f$result@f$
 */
static void PolyAddTwoNonConstFirstLarger(const Poly *p, const Poly *q, Poly *result)
{
    size_t p_index = 0;
    size_t q_index = 0;
    size_t res_index = 0;
    while (p_index < p->size && q_index < q->size)
    {
        AddTwoPolyTillOneEnds(p, q, result, &p_index, &q_index, &res_index);
    }
    while (p_index < p->size)
    {
        result->arr[res_index] = MonoClone(&p->arr[p_index]);
        p_index++;
        res_index++;
    }
    if (res_index == 0) //musimy zwolnić tablicę jednomianów i stworzyć wielomian zerowy
    {
        PolyDestroy(result);
        *result = PolyZero();
    } else
        result->size = res_index;
}

/**
 * Dodaje dwa niestałe wielomiany.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] q : wielomian @f$q@f$
 * @param[in] result : wielomian wynikowy @f$result@f$
 */
static void PolyAddTwoNonConst(const Poly *p, const Poly *q, Poly *result)
{
    //sprawdzamy który wielomian ma większy ostatni wykładnik
    if (p->arr[p->size - 1].exp >= q->arr[q->size - 1].exp)
        PolyAddTwoNonConstFirstLarger(p, q, result);
    else
        PolyAddTwoNonConstFirstLarger(q, p, result);
}

/**
 * Kopiuje tablicę jednomianów z jednego wielomianu do drugiego, od podanego indeksu do końca.
 * @param[in] p : wielomian @f$p@f$
 * @param[in] result : wielomian wynikowy @f$result@f$
 * @param[in] p_index : indeks w tablicy wielomianu @f$p@f$
 * @param[in] res_index : indeks w tablicy wielomianu @f$result@f$
 */
static void ClonePartOfMonos(const Poly *p, Poly *result, size_t *res_index, size_t p_index)
{
    while (p_index < p->size)
    {
        result->arr[*res_index] = MonoClone(&p->arr[p_index]);
        p_index++;
        (*res_index)++;
    }
}

/**
 * Zmienia wielomian w inny wielomian.
 * Niszczy dotychczasowy wielomian i klonuje do niego nowy.
 * @param[in] p : wielomian do skolonowania@f$p@f$
 * @param[in] result : wielomian wynikowy @f$result@f$
 */
static void PolyChangeInto(Poly *result, const Poly *p)
{
    PolyDestroy(result);
    *result = PolyClone(p);
}

/**
 * Dodaje wielomian stały i niestały.
 * @param[in] p : wielomian stały @f$p@f$
 * @param[in] q : wielomian niestały @f$q@f$
 * @param[in] result : wielomian wynikowy @f$result@f$
 */
static void PolyAddConstAndNonConst(const Poly *p, const Poly *q, Poly *result)
{
    if (PolyIsZero(p))
    {
        PolyChangeInto(result, q);
        return;
    }
    size_t res_index = 0;
    result->arr[0].exp = 0; //pierwszy wykładnik jest zawsze równy 0, bo dodajemy wielomian stały (może zostać potem pominięty)
    if (q->arr[0].exp != 0) //kopiujemy wielomian p z wykładnikiem 0 i potem kopiujemy całą tablicę q.arr
    {
        result->arr[0].p = PolyClone(p);
        res_index++;
        ClonePartOfMonos(q, result, &res_index, 0);
    } else
    {   //q->arr[0].exp = 0, więc pod zerowym indeksem dodajemy p +q.arr[0].p
        result->arr[0].p = PolyAdd(p, &q->arr[0].p); //pod indeksem zerowym stoi suma p+q.arr[0]
        if (!PolyIsZero(&result->arr[0].p)) //jeśli wynikiem był zerowy wielomian, to pomijamy go
            res_index++;
        ClonePartOfMonos(q, result, &res_index, 1);
    }
    result->size = res_index;
}

/**
 * Redukuje wielomian do postaci wielomianu stałego, jeśli to możliwe
 * @param[in] p : wielomian @f$p@f$
 */
static void MaybeReduceToCoeff(Poly *p)
{
    if (p->arr != NULL && p->size == 1 && p->arr[0].exp == 0 && PolyIsCoeff(&p->arr[0].p))
    {
        poly_coeff_t coeff = p->arr[0].p.coeff;
        PolyDestroy(p);
        *p = PolyFromCoeff(coeff);
    }
}

Poly PolyAdd(const Poly *p, const Poly *q)
{
    size_t size_of_new_poly;
    size_of_new_poly = SizeOfAddArray(p, q);
    if (size_of_new_poly == 0) //p i q są wielomianami stałymi
        return PolyFromCoeff(p->coeff + q->coeff);

    Poly result = PolyNewFromSize(size_of_new_poly);
    if (p->arr != NULL && q->arr != NULL) //oba nie są stałe
        PolyAddTwoNonConst(p, q, &result);
    else if (p->arr != NULL) //tylko q jest stały
        PolyAddConstAndNonConst(q, p, &result);
    else //tylko p jest stały
        PolyAddConstAndNonConst(p, q, &result);

    MaybeReduceToCoeff(&result);

    return result;
}

/**
 * Porównuje jednomiany według wykładników.
 */
static int CmpMonos(const void *a, const void *b)
{
    const Mono *p1 = (Mono *) a;
    const Mono *p2 = (Mono *) b;
    if (p1->exp < p2->exp)
        return -1;
    else if (p1->exp > p2->exp)
        return 1;
    else return 0;
}

/**
 * Sortuje tablicę jednomianów rosnąco według wykładników.
 * @param[in] monos : tablica jednomianów
 * @param[in] size : rozmiar tablicy @f$monos@f$
 */
static void SortMonosArr(Mono *monos, size_t size)
{
    qsort(monos, size, sizeof(Mono), CmpMonos);
}


/**
 * Przepisuje jednomian do innego indeksu w tablicy, a pod stary indeks wstawia wielomiaan zerowy.
 * @param[in] monos : tablica jednomianów
 * @param[in] current_index :  indeks, z którego kopiujemy i który wypełniamy zerem
 * @param[in] last_used_index : indeks, do którego kopiujemy wielomian
 */
static void PushMonoToEmpty(Mono *monos, size_t current_index, size_t last_used_index)
{
    assert(PolyIsZero(&monos[last_used_index].p));
    monos[last_used_index].p = monos[current_index].p;
    monos[last_used_index].exp = monos[current_index].exp;
    monos[current_index].p = PolyZero();
}

/**
 * Dodaje jednomian do innego jednomianu w tablicy jednomianów.
 * Pod indeks, z którego zabiera jednoman, wpisuje jako współczynnik wielomian zerowy.
 * @param[in] monos : tablica jednomianów
 * @param[in] current_index :  indeks, z którego uzyskujemy jednomian do sumy i który wypełniamy zerem
 * @param[in] last_used_index : indeks, do którego dodajemy jednomian
 */
static void AddMonoToMono(Mono *monos, size_t current_index, size_t *last_used_index)
{
    Poly temp = PolyClone(&monos[*last_used_index].p); //musimy skopiować, żeby potem móc zwolnić pamięć
    PolyDestroy(&monos[*last_used_index].p);
    monos[*last_used_index].p = PolyAdd(&temp, &monos[current_index].p);

    PolyDestroy(&monos[current_index].p);
    monos[current_index].p = PolyZero();
    PolyDestroy(&temp);

    if (PolyIsZero(&monos[*last_used_index].p))
    {
        if (*last_used_index > 0)
            (*last_used_index)--;
    }
}

/**
 * Kopiuje tablicę jednomianów (płytka kopia).
 * @param[in] monos : tablica jednomianów
 * @param[in] size : rozmiar tablicy @f$monos@f$
 * @return tablica z kopią jednomianów
 */
static Mono *CopyMonosArr(const Mono monos[], size_t size)
{
    Mono *res = calloc(size, sizeof(Mono));
    CHECK_PTR(res);
    for (size_t i = 0; i < size; i++)
        res[i] = monos[i];

    return res;
}

/**
 * Klonuje tablicę jednomianów (głęboka kopia).
 * @param[in] monos : tablica jednomianów
 * @param[in] size : rozmiar tablicy @f$monos@f$
 * @return tablica z kopią jednomianów
 */
static Mono *CloneMonosArr(const Mono *monos, size_t size)
{
    Mono *res = calloc(size, sizeof(Mono));
    CHECK_PTR(res);
    for (size_t i = 0; i < size; i++)
    {
        res[i].p = PolyClone(&(monos[i].p));
        res[i].exp = monos[i].exp;
    }
    return res;
}

/**
 * Redukuje tablicę jednomianów, tak, aby w pierszej części tablicy długości @f$used@f$
 * każdy wykładnik występował maksymalnie raz.
 * @param[in] monos : tablica jednomianów
 * @param[in] size :  rozmiar tablicy @f$monos@f$
 * @param[in] used : użyte miejsce w tablicy
 */
static void ReduceMonosArr(Mono *monos, size_t size, size_t *used)
{
    size_t i = 1; //indeks, który aktualnie analizujemy
    size_t last = 0; //ostatni modyfikowany indeks
    while (i < size)
    {
        if (!PolyIsZero(&monos[i].p)) //jeśli jest zerowy, to go pomijamy
        {
            //dodajemy jednomian z indeksu i do jednomianu o tym samym wykładniku pod indeksem last
            if (monos[i].exp == monos[last].exp)
            {
                //last może się zmniejszyć, gdy powstanie wielomian zerowy, pod indeksem i wstawiamy wielomian zerowy
                AddMonoToMono(monos, i, &last);
            } else
            {
                last++;
                if (last != i) //przepisujemy jednomian z indeksu i do indeksu last, w miejsce i wstawiamy zerowy
                    PushMonoToEmpty(monos, i, last);
            }
        }
        i++;
    }
    *used = last + 1;
}

/**
 * Tworzy wielomian z posortowanej tablicy jednomianów lub jeśli
 * w tablicy pozostał tylko jeden jednomian, z wykładnikiem 0 i stałym współczynnikiem,
 * tworzy wielomian stały i zwalnia pamięć tablicy.
 * @param[in] monos : tablica jednomianów
 * @param[in] count : liczba jednomianów
 * @return wielomian wynikowy
 */
static Poly CreatePolyFromArr(Mono monos[], size_t count)
{
    size_t used = count;
    if (count > 1)
        ReduceMonosArr(monos, count, &used); //redukowanie tablicy
    Poly result;
    if (used == 1 && (PolyIsZero(&monos[0].p) || (monos[0].exp == 0 && PolyIsCoeff(&monos[0].p))))
        //wynik będzie wielomianem stałym
    {
        result = monos[0].p;
        free(monos);
    } else //wielomian nie jest stały - kopiujemy zredukowaną tablicę
    {
        result.size = used;
        result.arr = monos;
    }
    return result;
}

Poly PolyAddMonos(size_t count, const Mono monos[])
{
    if (count == 0) //pusta tablica - wielomian zerowy
        return PolyZero();
    Mono *monos_cp = CopyMonosArr(monos, count);
    SortMonosArr(monos_cp, count);
    return CreatePolyFromArr(monos_cp, count);
}

/**
 * Sumuje listę jednomianów i tworzy z nich wielomian. Przejmuje na własność
 * pamięć wskazywaną przez @p monos i jej zawartość. Może dowolnie modyfikować
 * zawartość tej pamięci. Zakładamy, że pamięć wskazywana przez @p monos
 * została zaalokowana na stercie. Jeśli @p count lub @p monos jest równe zeru
 * (NULL), tworzy wielomian tożsamościowo równy zeru.
 * @param[in] count : liczba jednomianów
 * @param[in] monos : tablica jednomianów
 * @return wielomian będący sumą jednomianów
 */
Poly PolyOwnMonos(size_t count, Mono *monos)
{
    if (count == 0) //pusta tablica - wielomian zerowy
        return PolyZero();
    SortMonosArr(monos, count);
    return CreatePolyFromArr(monos, count);
}


Poly PolyCloneMonos(size_t count, const Mono monos[])
{
    if (count == 0) //pusta tablica - wielomian zerowy
        return PolyZero();
    Mono *monos_cp = CloneMonosArr(monos, count);
    SortMonosArr(monos_cp, count);
    return CreatePolyFromArr(monos_cp, count);
}

bool PolyIsEq(const Poly *p, const Poly *q)
{
    if (p->arr == NULL && q->arr == NULL)
        return p->coeff == q->coeff;
    if (p->arr == NULL || q->arr == NULL)
        return false;
    //oba są niestałe, więc muszą mieć tę samą liczbę jednomianów w tablicy
    if (p->size != q->size)
        return false;

    for (size_t i = 0; i < p->size; i++)
    {
        if (p->arr[i].exp != q->arr[i].exp) //wykładniki muszą być równe
            return false;
        if (!PolyIsEq(&p->arr[i].p, &q->arr[i].p)) //współczynniki muszą być takie same
            return false;
    }
    return true;
}

static Poly PolyMulByCoeff(Poly *p, poly_coeff_t coeff);

/**
 * Wypełnia tablicę jednomianów wynikiem mnożenia jednomianów przez stały współczynnik
 * @param[in] p : wielomian  @f$p@f$
 * @param[in] result : wielomian wynikowy @f$result@f$
 * @param[in] coeff : współczynnik
 * @param[in] count : licznik jednomianów w tablicy @f$arr@f$ wielomianu  @f$result@f$
 */
static void PolyMulByCoeffHelp(Poly *p, Poly *result, poly_coeff_t coeff, size_t *count)
{
    assert(*count == 0);
    for (size_t i = 0; i < p->size; i++)
    {
        Poly temp = PolyMulByCoeff(&p->arr[i].p, coeff);
        if (!PolyIsZero(&temp)) //mnożenie dwóch niezerowych może dać w wyniku zero (overflow)
        {
            result->arr[*count].p = temp;
            result->arr[*count].exp = p->arr[i].exp;
            (*count)++;
        }
    }
}

/**
 * Mnoży wielomian przez stały współczynnik.
 * @param[in] p : wielomian  @f$p@f$
 * @param[in] coeff : współczynnik
 * @return @f$p * coeff@f$
 */
Poly PolyMulByCoeff(Poly *p, poly_coeff_t coeff)
{
    if (PolyIsCoeff(p))
        return PolyFromCoeff(coeff * (p->coeff));
    Poly res = PolyNewFromSize(p->size);
    size_t count = 0;
    PolyMulByCoeffHelp(p, &res, coeff, &count);
    if (count == 0)
    {
        PolyDestroy(&res);
        return PolyZero();
    }
    res.size = count;
    if (count == 1 && res.arr[0].exp == 0 && PolyIsCoeff(&res.arr[0].p)) //można zredukować do wielomianu stałego
    {
        poly_coeff_t result = res.arr[0].p.coeff;
        PolyDestroy(&res);
        return PolyFromCoeff(result);
    }
    return res;
}

Poly PolyNeg(const Poly *p)
{
    return PolyMulByCoeff((Poly *) p, -1);
}

Poly PolySub(const Poly *p, const Poly *q)
{
    Poly neg_q = PolyNeg(q);
    Poly res = PolyAdd(p, &neg_q);
    PolyDestroy(&neg_q);
    return res;
}

/**
 * Mnoży dwa jednomiany
 * @param[in] p : jednomian  @f$p@f$
 * @param[in] q : jednomian  @f$q@f$
 * @return @f$p * q@f$
 */
static Mono MonoMul(Mono *p, Mono *q)
{
    Mono res;
    res.exp = p->exp + q->exp;
    res.p = PolyMul(&p->p, &q->p);
    return res;
}

/**
 * Tworzy tablicę jednomianów, która jest wynikiem mnożenia dwóch tablic - każdy indeks z każdym.
 * @param[in] p : wielomian  @f$p@f$
 * @param[in] q : wielomian  @f$q@f$
 * @param[in] size : rozmiar wynikowej tablicy
 * @return tablica jednomianów
 */
static Mono *FillMonoMulArray(const Poly *p, const Poly *q, size_t size)
{
    Mono *monos = calloc(size, sizeof(Mono));
    CHECK_PTR(monos);
    size_t monos_index = 0;
    for (size_t i = 0; i < p->size; i++)
    {
        for (size_t j = 0; j < q->size; j++)
        {
            monos[monos_index] = MonoMul(&p->arr[i], &q->arr[j]);
            monos_index++;
        }
    }
    return monos;
}

Poly PolyMul(const Poly *p, const Poly *q)
{
    if (p->arr != NULL && q->arr != NULL)
    {
        size_t size_of_monos = p->size * q->size;
        Mono *monos = FillMonoMulArray(p, q, size_of_monos);
        Poly res = PolyAddMonos(size_of_monos, monos);
        free(monos);
        return res;
    } else if (p->arr == NULL && q->arr == NULL)
    {
        return PolyFromCoeff(p->coeff * q->coeff);
    } else if (q->arr != NULL)
    {
        return PolyMulByCoeff((Poly *) q, p->coeff);
    } else
        return PolyMulByCoeff((Poly *) p, q->coeff);
}

/**
 * Wykonuje szybkie potęgowanie.
 * @param[in] x : podstawa
 * @param[in] n : wykładnik
 * @return @f$x^n@f$
 */
static poly_coeff_t QuickPow(poly_coeff_t x, poly_exp_t n)
{
    poly_coeff_t res = 1;
    while (n > 0)
    {
        if (n % 2 == 1)
            res *= x;

        x *= x;
        n /= 2;
    }
    return res;
}


Poly PolyAt(const Poly *p, poly_coeff_t x)
{
    if (p->arr == NULL)
        return PolyClone(p);

    Poly result = PolyZero();
    for (size_t i = 0; i < p->size; i++)
    {
        //mnożenie wielomianów z tablicy przez x podniesiony do wykładnika
        Poly mul_result = PolyMulByCoeff(&p->arr[i].p, QuickPow(x, p->arr[i].exp));
        Poly temp = result; //trzymamy, żeby potem zwolnić pamięć
        result = PolyAdd(&result, &mul_result);
        PolyDestroy(&temp);
        PolyDestroy(&mul_result);
    }
    return result;
}

poly_exp_t PolyDeg(const Poly *p)
{
    if (PolyIsZero(p))
        return DEG_OF_ZERO_POLY;
    if (p->arr == NULL)
        return 0;
    poly_exp_t result = 0;
    for (size_t i = 0; i < p->size; i++) //szukamy największego występującego wykładnika
    {
        poly_exp_t temp = p->arr[i].exp + PolyDeg(&p->arr[i].p);
        if (temp > result)
            result = temp;
    }
    return result;
}

/**
 * Zwraca stopień wielomianu ze względu na jego główną (najpłytszą) zmienną $x_0$.
 * @param[in] p : wielomian
 * @return stopień wielomianu @p p z względu na zmienną o indeksie 0
 */
static poly_exp_t PolyDegByZero(const Poly *p)
{
    assert(!PolyIsZero(p));
    if (p->arr == NULL)
        return 0;
    else
        return p->arr[p->size - 1].exp;
}

poly_exp_t PolyDegBy(const Poly *p, size_t var_idx)
{
    if (PolyIsZero(p))
        return DEG_OF_ZERO_POLY;

    if (var_idx == 0)
        return PolyDegByZero(p);

    if (p->arr == NULL)
        return 0;

    poly_exp_t max_deg = DEG_OF_ZERO_POLY;
    for (size_t i = 0; i < p->size; i++)
    {
        poly_exp_t temp = PolyDegBy(&p->arr[i].p, var_idx - 1);
        if (temp > max_deg)
            max_deg = temp;
    }
    return max_deg;
}

/**
 * Wypisuje jednomian w formacie (p,exp)
 * @param[in] mono : jednomian do wypisania
 */
static void MonoPrint(Mono *mono)
{
    printf("(");
    PolyPrint(&mono->p);
    printf(",%d)", mono->exp);
}

void PolyPrint(Poly *p)
{
    if (PolyIsCoeff(p))
    {
        printf("%ld", p->coeff);
    } else
    {
        for (size_t i = 0; i < p->size; i++)
        {
            MonoPrint(&(p->arr[i]));
            if (i < p->size - 1)
                printf("+");
        }
    }
}

/**
 * Wykonuje szybkie potęgowanie wielomianu.
 * @param[in] p : podstawa - wielomian
 * @param[in] exp : wykładnik
 * @return @f$p^exp@f$
 */
static Poly PolyQuickPow(const Poly *p, poly_exp_t exp)
{
    Poly res = PolyFromCoeff(1);
    Poly q = PolyClone(p);
    while (exp > 0)
    {
        if (exp % 2 == 1)
        {
            Poly temp = PolyMul(&res, &q);
            PolyDestroy(&res);
            res = temp;
        }
        Poly temp = PolyMul(&q, &q);
        PolyDestroy(&q);
        q = temp;
        exp /= 2;
    }
    PolyDestroy(&q);
    return res;
}

/**
 * Funkcja rekurencyjna, wykonuje operację składania wielomianów. jeśli @p p jest wielomianem stałym, zwraca jego kopię.
 * W przeciwnym razie tworzy wielomian wyjściowy i dla wszystkich elementów w tablicy jednomianów:
 * wywołuje się dla wielomianu tego jednomianu, przekazując stopień zagłębienia o jeden większy,
 * potęguje q[depth] do potęgi tego jednomianu (lub jeśli k jest mniejsze od stopnia, to wynik potęgowania jest zerem),
 * mnoży wynik rekurencji i potęgowania i wynik mnożenia dodaje do wielomianu wyjściowego.
 *
 * @param[in] p : wielomian, do którego podstawiamy wieomiany z tablicy q
 * @param[in] k : liczba wielomianóœ w tablicy q
 * @param[in] q : tablica wielomianów
 * @param[in] depth : stopień zagłębienia w wielomianie p
 * @return złożenie wielomianów
 */
static Poly PolyComposeHelp(const Poly *p, size_t k, const Poly q[], size_t depth)
{
    if (PolyIsCoeff(p))
        return PolyClone(p);

    Poly res = PolyZero();
    for (size_t i = 0; i < p->size; i++)
    {
        Poly deep_result = PolyComposeHelp(&(p->arr[i].p), k, q, depth + 1);
        Poly power_result;
        if (k == 0 || depth >= k)
        {
            Poly temp = PolyZero();
            //nie można zrobić power_result=PolyZero, bo jeśli exp=0, to wtedy powinien być wielomian stały 1
            power_result = PolyQuickPow(&temp, p->arr[i].exp);
        } else
            power_result = PolyQuickPow(&(q[depth]), p->arr[i].exp);
        Poly current_index_res = PolyMul(&deep_result, &power_result);
        Poly add_result = PolyAdd(&res, &current_index_res);
        PolyDestroy(&power_result);
        PolyDestroy(&res);
        PolyDestroy(&deep_result);
        PolyDestroy(&current_index_res);
        res = add_result;
    }
    return res;
}

Poly PolyCompose(const Poly *p, size_t k, const Poly q[])
{
    return PolyComposeHelp(p, k, q, 0);
}

