/*
 * debug.h
 *
 *  Created on: Oct 17, 2021
 *      Author: muaddib
 */

#ifndef INC_SOFTWARE_DEBUG_H_
#define INC_SOFTWARE_DEBUG_H_

#include "global_inc.h"

int _write(int file, char *ptr, int len);

#define DEBUG_PRINT(...) do {printf(__VA_ARGS__); printf("\n");} while (0)



#endif /* INC_SOFTWARE_DEBUG_H_ */
