/*
 * misc.c
 *
 *  Created on: Oct 31, 2021
 *      Author: muaddib
 */
#include "misc.h"

bool is_power_of_two(uint32_t n)
{
    return n && (!(n & (n - 1)));
}

// Returns position of the only set bit in 'n'
int32_t find_bit_position(uint32_t n)
{
    if (!is_power_of_two(n))
        return -1;

    uint32_t i = 1;
    uint32_t pos = 0;

    // Iterate through bits of n till we find a set bit
    // i&n will be non-zero only when 'i' and 'n' have a set bit
    // at same position
    while (!(i & n)) {
        // Unset current bit and set the next bit in 'i'
        i = i << 1;

        // increment position
        ++pos;
    }

    return pos;
}
