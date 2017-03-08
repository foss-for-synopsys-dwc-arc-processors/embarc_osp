#ifndef CONTAINER2_H
#define CONTAINER2_H

#include "container1.h"

typedef struct {
	uint8_t secret[SECRET_LEN+1];
	uint8_t initialized;
} SECRET_CONTEXT2;

extern void trusted_ops(void);

#endif /* CONTAINER2_H */