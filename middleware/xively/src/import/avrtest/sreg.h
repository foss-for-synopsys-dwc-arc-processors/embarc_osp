#ifndef SREG_H
#define SREG_H

#define FLAG_I_BIT  7
#define FLAG_T_BIT  6
#define FLAG_H_BIT  5
#define FLAG_S_BIT  4
#define FLAG_V_BIT  3
#define FLAG_N_BIT  2
#define FLAG_Z_BIT  1
#define FLAG_C_BIT  0

#define FLAG_I  (1 << FLAG_I_BIT)
#define FLAG_T  (1 << FLAG_T_BIT)
#define FLAG_H  (1 << FLAG_H_BIT)
#define FLAG_S  (1 << FLAG_S_BIT)
#define FLAG_V  (1 << FLAG_V_BIT)
#define FLAG_N  (1 << FLAG_N_BIT)
#define FLAG_Z  (1 << FLAG_Z_BIT)
#define FLAG_C  (1 << FLAG_C_BIT)

#endif /* SREG_H */
