#ifndef CONTAINER1_H
#define CONTAINER1_H

#define SECRET_LEN	64
#define PWD_LEN		6

#define GET_SECRET	1
#define SET_PWD		2
#define SET_SECRET	3

typedef struct {
	uint8_t * secret;
	uint8_t * pwd;
	uint8_t initialized;
	uint8_t fails;
} SECRET_CONTEXT;


#define INTNO_TIMER1	17
#define INTNO_SWI 	18

/* the following extern variables are used to simulate memory leakage */
extern uint8_t container1_pwd[];
extern uint8_t container1_secret[];
extern SECRET_CONTEXT container1_context;


extern int32_t init_secret(void);
extern int32_t operate_secret(char* pwd, uint32_t cmd, char * data);


#endif