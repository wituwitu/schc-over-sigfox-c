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
 *  returns: the size of the buffer.
 */
size_t repeat_char(char* dest, char c, int length) ;

/*
 * Function:  strrev
 * --------------------
 * Reverses a string (char*) in place. Requires that the argument
 * be initialized as a mutable char array (char[], not char*)
 *
 *  start: pointer to the start of the string to reverse.
 */
void strrev(char *start);

void zfill(char* dest, const char* src, unsigned int length);