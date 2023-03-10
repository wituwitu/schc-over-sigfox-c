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
 * Function:  int_to_bin
 * --------------------
 * Obtains the binary string representation of an integer.
 *
 *  dest: buffer where to store the result.
 *  src: integer to process.
 *  dest_size: size of the buffer.
 */
void int_to_bin(char dest[9], signed int src, int dest_size);

/*
 * Function:  bytes_to_bin
 * --------------------
 * Obtains the binary string representation of an array of chars.
 * Returns the bit size of the result.
 *
 *  dest: buffer where to store the result.
 *  bytes: array of chars to process.
 *  src_size: length of the source excluding the null terminator.
 */
int bytes_to_bin(char *dest, const char *src, int src_size);

/*
 * Function:  bin_to_bytes
 * --------------------
 * Obtains the char array representation of a binary string.
 * Returns the byte size of the result.
 *
 *  dest: buffer where to store the result.
 *  bits: binary string to process.
 *  src_size: length of the source excluding the null terminator.
 */
int bin_to_bytes(char *dest, char *src, int src_size);