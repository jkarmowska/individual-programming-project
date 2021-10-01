#define _GNU_SOURCE

#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <stdint.h>
#include "line_process.h"
#include "word_type.h"

//ASCII codes of characters
#define FIRST_WHITESPACE 9
#define LAST_WHITESPACE 13
#define LAST_LETTER 126
#define SPACE 32
#define ENDLINE 10

#define GETLINE_ERROR -1
#define END_OF_FILE -1

static size_t increase_size(size_t n)
{
    if (n == SIZE_MAX) //SIZE_MAX can be reached as n = 2^m - 1
        exit(1); //if it is reached, then n can no longer be increased thus exit(1)

    return 1 + 2 * n;
}

void allocate_one_line(Set *new_line)
{
    new_line->non_numbers = calloc(INITIAL_ARRAY_SIZE, sizeof(char *));
    if (!new_line->non_numbers)
        exit(1);

    new_line->numbers = calloc(INITIAL_ARRAY_SIZE, sizeof(long double));
    if (!new_line->numbers)
        exit(1);

    new_line->size_of_non_numbers_arr = INITIAL_ARRAY_SIZE;
    new_line->size_of_numbers_arr = INITIAL_ARRAY_SIZE;
    new_line->how_many_non_numbers = 0;
    new_line->how_many_numbers = 0;
}


int read_line_into_string(char **string)
{
    size_t size = 0;
    errno = 0;
    int result;

    result = getline(string, &size, stdin); //getline returns length of read line or -1

    if (result == GETLINE_ERROR && errno == ENOMEM) //problem with memory occurred
        exit(1);

    else if (result == GETLINE_ERROR) //input ended
    {
        return END_OF_FILE;
    }

    return result;
}

static bool comment(const char *string)
{
    if (string[0] == '#')
        return true;
    else
        return false;
}


static bool wrong_letter_found(const char *string, int length)
{
    for (int i = 0; i < length; i++)
    {   //if letter is out of range return true

        if (string[i] < FIRST_WHITESPACE || (string[i] > LAST_WHITESPACE && string[i] < SPACE) ||
            (string[i] > LAST_LETTER))
            return true;
    }
    return false;
}

static bool only_white_signs(const char *string, int length)
{
    for (int i = 0; i < length; i++)
    {   //if letter is not a whitespace range return false
        if (!isspace(string[i]))
            return false;
    }
    return true;
}

//returns a pointer to first character in line that is not a whitespace
static char *skip_white_spaces(char *line)
{
    int i = 0;
    while (isspace(line[i]) && line[i] != ENDLINE)
        i++;
    return line + i;
}

static void reallocate_numbers_arr(long double **numbers, size_t *size)
{
    *size = increase_size(*size);

    *numbers = realloc(*numbers, *size * sizeof(long double));
    if (!*numbers)
        exit(1);
}

static void add_number_into_set(Set *set, long double number)
{
    if (set->how_many_numbers >= set->size_of_numbers_arr) //reallocating if necessary
        reallocate_numbers_arr(&(set->numbers), &(set->size_of_numbers_arr));

    set->numbers[set->how_many_numbers] = number; //adding new number and increasing count
    (set->how_many_numbers)++;
}


static void reallocate_non_numbers_arr(char ***non_numbers, size_t *size)
{
    *size = increase_size(*size);

    *non_numbers = realloc(*non_numbers, *size * sizeof(char *));
    if (!*non_numbers)
        exit(1);
}

static void add_non_number_into_set(Set *set, char *word)
{
    if (set->how_many_non_numbers >= set->size_of_non_numbers_arr) //sizing up the array of non_numbers if necessary
        reallocate_non_numbers_arr(&(set->non_numbers), &(set->size_of_non_numbers_arr));

    set->non_numbers[set->how_many_non_numbers] = word; //adding the new word and increasing the count
    (set->how_many_non_numbers)++;
}

static void get_word_into_set(Set *set, char *line, char **end)
{
    long double number;
    if (is_a_number(line, end, &number))//checking if the first word in line is a number,
        // if yes then adding it into numbers array
        add_number_into_set(set, number);
    else
    {   //the first word in line is non_number so we extract it and add to non_numbers array
        char *word;
        extract_word(line, end, &word);
        add_non_number_into_set(set, word);
    }
    //end points to first whitespace sign after a word or \0
}

static void process_the_line(Set *set, char *string)
{
    allocate_one_line(set);
    char *beginning = string;//new variable to keep the string pointer unmodified so we can free it later
    char *end;
    beginning = skip_white_spaces(beginning);
    //now beginning points to first character of a word

    while (*beginning != '\n' && *beginning != '\0')//
    {
        get_word_into_set(set, beginning, &end);
        //now end points to first character after a word - that is a whitespace or '\0'
        beginning = skip_white_spaces(end);
        //beginning points to a first character that is not a whitespace or to endline or '\0'
    }
}

//modifies new_line and returns type of line (COMMENT, ERROR_LETTER, GOOD_LINE, END_OF_FILE)
Type turn_line_into_set(Set *new_line)
{
    char *string = NULL;
    int length = read_line_into_string(&string);
    int result;
    if (length == END_OF_FILE)//if there was no new line, return END_OF_FILE to signal end of the program
        result = EndOfFile;
    else if (comment(string))
        result = Comment;
    else if (wrong_letter_found(string, length))
        result = ErrorLetter;
    else if (only_white_signs(string, length))
        result = EmptyLine;
    else //the line is correct, so we turn it into a set of numbers and non numbers
    {
        process_the_line(new_line, string);
        result = GoodLine;
    }
    free(string);
    return result;
}


