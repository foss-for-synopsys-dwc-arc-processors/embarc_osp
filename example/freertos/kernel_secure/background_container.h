#ifndef BACKGROUND_CONTAINER_H
#define BACKGROUND_CONTAINER_H


#ifdef INTNO_I2C0
#define INTNO_SWI		INTNO_I2C0
#else
#define INTNO_SWI		20
#endif

extern void default_interrupt_handler(void *p_exinf);

#endif