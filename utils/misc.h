//
// Created by witu on 23-01-23.
//

#ifndef SCHC_OVER_SIGFOX_C_MISC_H
#define SCHC_OVER_SIGFOX_C_MISC_H

#endif //SCHC_OVER_SIGFOX_C_MISC_H

/*
 * Function:  repeat_char
 * --------------------
 * Repeats a single chatacter until the desired length into a buffer.
 *
 *  dest: buffer where to store the resulting char array.
 *  c: the character to be repeated.
 *  length: the number of times to repeat the character.
 *
 *  returns: the byte_size of the buffer.
 */
size_t repeat_char(char *dest, char c, int length) ;

/*
 * Function:  strrev
 * --------------------
 * Reverses a mutable string (char[]) in place.
 *  start: pointer to the start of the string to reverse.
 */
void strrev(char *start);

/*
 * Function:  zfill
 * --------------------
 * Adds preceding zeros to a mutable string (char[]).
 *
 *  dest: buffer where to store the new string.
 *  src: pointer to the start of the string to process.
 *  length: desired length of the result string.
 */
void zfill(char dest[], char *src, unsigned int length);

/*
 * Function:  replace_char
 * --------------------
 * Replaces a single character inside a mutable string (char[]).
 *
 *  src: string to modify.
 *  idx: index of the character to be replaced.
 *  c: character to replace.
 */
void replace_char(char src[], int idx, char c);

/*
 * Function:  is_monochar
 * --------------------
 * Checks if a string contains only occurences of the specified character.
 *
 *  s: string to check.
 *  c: character to look for.
 */
int is_monochar(char s[], char c);

/*
 * Function:  generate_packet
 * --------------------
 * Fills a char[] buffer of the desired length with placeholder data.
 *
 *  dest: buffer where to store the information.
 *  length: desired length of the information.
 */
void generate_packet(char dest[], int length);

int round_to_next_multiple(signed int n, int factor);