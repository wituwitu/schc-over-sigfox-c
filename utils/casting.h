//
// Created by witu on 18-01-23.
//

#ifndef SCHC_OVER_SIGFOX_C_CASTING_H
#define SCHC_OVER_SIGFOX_C_CASTING_H

#endif //SCHC_OVER_SIGFOX_C_CASTING_H

/*
 * Function:  bin_to_int
 * --------------------
 * Transforms a binary string into its integer representation.
 *
 *  bin: Binary string (composed of chars '0' and '1') to process.
 */
int bin_to_int(char* bin);

/*
 * Function:  int_to_bin
 * --------------------
 * Obtains the binary string representation of an integer between -127 and 126
 * (signed char).
 *
 *  dest: buffer where to store the result.
 *  n: integer to process.
 *  length: size of the buffer excluding the null terminator.
 */
void int_to_bin(char* dest, signed char n, unsigned int length);

/*
 * Function:  bytes_to_bin
 * --------------------
 * Obtains the binary string representation of an array of chars.
 *
 *  dest: buffer where to store the result.
 *  bytes: array of chars to process.
 *  length: length of the buffer excluding the null terminator.
 */
void bytes_to_bin(char* dest, char* bytes, unsigned int length);