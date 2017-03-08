#ifndef _FYMODEM_H_
#define _FYMODEM_H_

/**
 * Free YModem implementation.
 *
 * Fredrik Hederstierna 2014
 *
 * This file is in the public domain.
 * You can do whatever you want with it.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 */

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdarg.h>

// receive file over ymodem
extern int32_t fymodem_receive(void);

// send file over ymodem
extern int32_t fymodem_send(const char* filename);

#endif //_FYMODEM_H_
