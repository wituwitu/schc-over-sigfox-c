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
int bin_to_int(const char *bin);

/*
 * Function:  char_to_bin
 * --------------------
 * Obtains the binary string representation of an integer between -127 and 126
 * (signed char).
 *
 *  dest: buffer where to store the result.
 *  src: integer to process.
 */
void char_to_bin(char dest[9], signed char src);

/*
 * Function:  bytes_to_bin
 * --------------------
 * Obtains the binary string representation of an array of chars.
 *
 *  dest: buffer where to store the result.
 *  bytes: array of chars to process.
 *  src_size: length of the source excluding the null terminator.
 */
void bytes_to_bin(char *dest, const char *src, unsigned int src_size);

/*
 * Function:  bin_to_bytes
 * --------------------
 * Obtains the char array representation of a binary string.
 *
 *  dest: buffer where to store the result.
 *  bits: binary string to process.
 *  src_size: length of the source excluding the null terminator.
 */
void bin_to_bytes(char *dest, char *src, unsigned int src_size);