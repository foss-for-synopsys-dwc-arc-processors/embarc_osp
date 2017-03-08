#ifndef AVRTEST_H
#define AVRTEST_H

#ifndef IN_AVRTEST
#ifdef __AVR_SFR_OFFSET__
#define IOBASE __AVR_SFR_OFFSET__
#else
#define IOBASE 0x20
#endif
#endif

#define STDIN_PORT_ADDR  (0x32 + IOBASE)
#define STDOUT_PORT_ADDR (0x32 + IOBASE)
#define EXIT_PORT_ADDR   (0x2F + IOBASE)
#define ABORT_PORT_ADDR  (0x29 + IOBASE)


#ifdef IN_AVRTEST

/* This defines are for avrtest itself.  */

#define STDIN_PORT  STDIN_PORT_ADDR
#define STDOUT_PORT STDOUT_PORT_ADDR 
#define EXIT_PORT   EXIT_PORT_ADDR 
#define ABORT_PORT  ABORT_PORT_ADDR 

#else

/* This defines can be used in the AVR application.  */

#define STDIN_PORT  (*((volatile unsigned char*) STDIN_PORT_ADDR))
#define STDOUT_PORT (*((volatile unsigned char*) STDOUT_PORT_ADDR))
#define EXIT_PORT   (*((volatile unsigned char*) EXIT_PORT_ADDR))
#define ABORT_PORT  (*((volatile unsigned char*) ABORT_PORT_ADDR))

#endif /* IN_AVRTEST */

#endif /* AVRTEST_H */



