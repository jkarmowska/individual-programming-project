#include <stdlib.h>
#include <ctype.h>
#include <errno.h>
#include <string.h>
#include <math.h>
#include <stdint.h>
#include "word_type.h"
#include "structs_and_const.h"


#define HEX_PREFIX_LENGTH 2 //length of "0X" prefix
#define SIGNED_HEX_PREFIX 3 //length of "+0x" or "-0x" prefix
#define FIRST_LETTER_INDEX 0
#define SECOND_LETTER_INDEX 1
#define THIRD_LETTER_INDEX 2
#define HEX_ZERO_LENGTH 2 //length of "0x"
#define DISTANCE_TO_SMALL_LETTERS 32 //distance between 'A' and 'a' in ASCII code

#define OCT_BASE 8
#define DEC_BASE 10
#define HEX_BASE 16

#define FIRST_CAP_LETTER 65 //ASCII code of 'A'
#define LAST_CAP_LETTER 90 //ASCII code of 'Z'

static size_t increase_size(size_t n)
{
    if (n == SIZE_MAX) //SIZE_MAX can be reached as n = 2^m - 1
        exit(1); //if it is reached, then n can no longer be increased thus exit(1)

    return 1 + 2 * n;
}

static bool is_convertible_to_unsigned_long_long(char *beginning, char **end, int base, long double *result)
{
    unsigned long long number;

    if (*beginning == '-')//we only want to extract unsigned values
        return false;

    errno = 0;
    number = strtoull(beginning, end, base);

    if (!isspace(**end) && **end != '\0')//there was a letter out of range
        return false;
    if (errno != 0 && number == 0)//another error occurred
        return false;

    //no errors occurred
    *result = (long double) number;
    return true;
}

static bool begins_with_0x(const char *word)
{
    return word[FIRST_LETTER_INDEX] == '0' && (word[SECOND_LETTER_INDEX] == 'x' || word[SECOND_LETTER_INDEX] == 'X');
}

static bool begins_with_signed_0x(char *word)
{
    if (strlen(word) < SIGNED_HEX_PREFIX)
        return false;

    return (word[FIRST_LETTER_INDEX] == '+' || word[FIRST_LETTER_INDEX] == '-')
           && word[SECOND_LETTER_INDEX] == '0' && (word[THIRD_LETTER_INDEX] == 'x' || word[THIRD_LETTER_INDEX] == 'X');
}

static bool begins_with_0(char *word)
{
    if (strlen(word) < 1)
        return false;
    return word[0] == '0';
}

static bool is_hex_unsigned_long_long(char *beginning, char **end, long double *number)
{
    if (strlen(beginning) < HEX_PREFIX_LENGTH)
        return false;
    if (!begins_with_0x(beginning))
        return false;
    if (strlen(beginning) == HEX_ZERO_LENGTH || isspace(beginning[HEX_ZERO_LENGTH]))
    {   //if a number in beginning is "0x" or "0X" it should be treated as having value 0
        *number = 0;
        *end = &beginning[HEX_ZERO_LENGTH];
        return true;
    }
    return is_convertible_to_unsigned_long_long(beginning, end, HEX_BASE, number);
}

static bool is_oct_unsigned_long_long(char *beginning, char **end, long double *number)
{
    if (!begins_with_0(beginning))
        return false;
    return is_convertible_to_unsigned_long_long(beginning, end, OCT_BASE, number);
}

static bool is_dec_unsigned_long_long(char *beginning, char **end, long double *number)
{
    return is_convertible_to_unsigned_long_long(beginning, end, DEC_BASE, number);
}

static bool is_unsigned_long_long(char *beginning, char **end, long double *number)
{
    return is_hex_unsigned_long_long(beginning, end, number) || is_oct_unsigned_long_long(beginning, end, number) ||
           is_dec_unsigned_long_long(beginning, end, number);
}

static bool is_long_long(char *beginning, char **end, long double *result)
{
    long long number;
    errno = 0;

    number = strtoll(beginning, end, DEC_BASE);

    if (!isspace(**end) && **end != '\0')//there was a letter out of range
        return false;
    if (errno != 0 && number == 0)//some other error occurred
        return false;

    //no errors occurred
    *result = (long double) number;
    return true;
}

static bool is_long_double(char *beginning, char **end, long double *result)
{
    long double number;
    errno = 0;

    number = strtold(beginning, end);

    if (!isspace(**end) && **end != '\0') //there was a letter out of range
        return false;
    if (errno != 0 && number == 0) //some other error occurred
        return false;

    if (isnan(number)) //NaN can be parsed to long double but is should be treated as a non_number
        return false;

    //no errors occurred
    *result = number;
    return true;
}

bool is_a_number(char *beginning, char **end, long double *number)
{
    if (is_unsigned_long_long(beginning, end, number) || is_long_long(beginning, end, number))
        return true;
    if (begins_with_0x(beginning) ||
        begins_with_signed_0x(beginning))//begins with 0x but cannot be parsed into unsigned long long
        //so it it not a number, as floating point numbers must be in decimal base
        return false;

    if (is_long_double(beginning, end, number))
        return true;

    return false;
}


static void reallocate_string(char **word, size_t *size)
{
    *size = increase_size(*size);

    *word = realloc(*word, (*size) * sizeof(char));
    if (!*word)
        exit(1);
}

static void add_letter_to_word(char letter_to_add, char **word, size_t *size_of_word, size_t *how_many_letters_in_word)
{
    if (letter_to_add >= FIRST_CAP_LETTER && letter_to_add <= LAST_CAP_LETTER)
        letter_to_add += DISTANCE_TO_SMALL_LETTERS; //changing capital letters to small letters

    if (*how_many_letters_in_word + 1 >=
        *size_of_word) //comparison with +1 because we add one index for '\0' to end string
    {
        reallocate_string(word, size_of_word);
    }

    //adding new letter and \0, increasing count of the letters in word
    (*word)[*how_many_letters_in_word] = letter_to_add;
    (*word)[*how_many_letters_in_word + 1] = '\0';
    (*how_many_letters_in_word)++;
}

static void initialize_string(char **string, size_t *size_of_string, size_t *how_many_letters)
{
    *size_of_string = INITIAL_ARRAY_SIZE;
    *how_many_letters = 0;

    *string = calloc(INITIAL_ARRAY_SIZE, sizeof(char));
    if (!*string)
        exit(1);
}

void extract_word(char *beginning, char **end, char **result)
{
    int i = 0;
    char *extracted_word;
    size_t size_of_word;
    size_t how_many_letters;
    initialize_string(&extracted_word, &size_of_word, &how_many_letters);

    while (!isspace(beginning[i]) && beginning[i] != '\0')
    {
        //adding letter from beginning[i] to the end of extracted word
        add_letter_to_word(beginning[i], &extracted_word, &size_of_word, &how_many_letters);
        i++;
    }
    //beginning[i] is now pointing to first whitespace or '\0' after extracted word
    *end = beginning + i;
    *result = extracted_word;
}