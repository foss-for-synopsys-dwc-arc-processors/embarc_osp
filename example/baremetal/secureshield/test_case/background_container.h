#ifndef BACKGROUND_CONTAINER_H
#define BACKGROUND_CONTAINER_H

#define INTNO_SWI0 18
typedef struct {
	uint8_t secret[128];
	uint8_t initialized;
} TEST_CONTEXT;


extern void default_interrupt_handler(void *p_exinf);
extern void soft_interrupt0(void *p_exinf);

#endif