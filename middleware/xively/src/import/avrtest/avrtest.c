/*
 ****************************************************************************
 *
 * avrtest - A simple simulator for the Atmel AVR family of microcontrollers
 *           designed to test the compiler
 * Copyright (C) 2001, 2002, 2003   Theodore A. Roth, Klaus Rudolph
 * Copyright (C) 2007 Paulo Marques
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 ****************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef uint8_t byte;
typedef uint16_t word;
typedef uint32_t dword;

// ---------------------------------------------------------------------------
//     configuration values (in bytes).

#define MAX_RAM_SIZE     (64 * 1024)
#define MAX_FLASH_SIZE  (256 * 1024)  // Must be at least 128KB

// PC is used as array index into decoded_flash[].
// If PC has bits outside the following mask, something went really wrong,
// e.g. in pop_PC.

#define PC_VALID_MASK (MAX_FLASH_SIZE/2 - 1)

// ---------------------------------------------------------------------------
//     register and port definitions

#ifdef ISA_XMEGA
#define IOBASE  0
#else
#define IOBASE  0x20
#endif

#define SREG    (0x3F + IOBASE)
#define SPH     (0x3E + IOBASE)
#define SPL     (0x3D + IOBASE)
#define EIND    (0x3C + IOBASE)
#define RAMPZ   (0x3B + IOBASE)


// ports used for application <-> simulator interactions
#define IN_AVRTEST
#include "avrtest.h"
#include "flag-tables.c"
#include "sreg.h"

#define REGX    26
#define REGY    28
#define REGZ    30

#define INLINE inline __attribute__((always_inline))

struct arch_desc
{
  // Name of the architecture.
  const char *name;
  // True if PC is 3 bytes, false if only 2 bytes.
  unsigned char pc_3bytes;
  // True if the architecture has EIND related insns (EICALL/EIJMP).
  unsigned char has_eind;
};

// List of supported archs with their features.
const struct arch_desc arch_descs[] =
  {
#ifdef ISA_XMEGA
    { 
      .name = "avrxmega6",
      .pc_3bytes = 1,
      .has_eind = 1
    },
#else
    { 
      .name = "avr51",
      .pc_3bytes = 0,
      .has_eind = 0
    },
    {
      .name = "avr6",
      .pc_3bytes = 1,
      .has_eind = 1
    },
#endif
    { NULL, 0, 0}
  };

struct arch_desc arch;


enum decoder_operand_masks
  {
    /** 2 bit register id  ( R24, R26, R28, R30 ) */
    mask_Rd_2     = 0x0030,
    /** 3 bit register id  ( R16 - R23 ) */
    mask_Rd_3     = 0x0070,
    /** 4 bit register id  ( R16 - R31 ) */
    mask_Rd_4     = 0x00f0,
    /** 5 bit register id  ( R00 - R31 ) */
    mask_Rd_5     = 0x01f0,

    /** 3 bit register id  ( R16 - R23 ) */
    mask_Rr_3     = 0x0007,
    /** 4 bit register id  ( R16 - R31 ) */
    mask_Rr_4     = 0x000f,
    /** 5 bit register id  ( R00 - R31 ) */
    mask_Rr_5     = 0x020f,

    /** for 8 bit constant */
    mask_K_8      = 0x0F0F,
    /** for 6 bit constant */
    mask_K_6      = 0x00CF,

    /** for 7 bit relative address */
    mask_k_7      = 0x03F8,
    /** for 12 bit relative address */
    mask_k_12     = 0x0FFF,
    /** for 22 bit absolute address */
    mask_k_22     = 0x01F1,

    /** register bit select */
    mask_reg_bit  = 0x0007,
    /** status register bit select */
    mask_sreg_bit = 0x0070,
    /** address displacement (q) */
    mask_q_displ  = 0x2C07,

    /** 5 bit register id  ( R00 - R31 ) */
    mask_A_5      = 0x00F8,
    /** 6 bit IO port id */
    mask_A_6      = 0x060F
  };
// ----------------------------------------------------------------------------
//     some helpful types and constants

#define EXIT_STATUS_EXIT    0
#define EXIT_STATUS_ABORTED 1
#define EXIT_STATUS_TIMEOUT 2

typedef struct
{
  byte data_index;
  byte oper1;
  word oper2;
} decoded_op;

#define OP_FUNC_TYPE void __attribute__((fastcall))

typedef OP_FUNC_TYPE (*opcode_func)(int,int);

typedef struct
{
  opcode_func func;
  int size;
  int cycles;
  const char *hreadable;
} opcode_data;

// ---------------------------------------------------------------------------
//     auxiliary lookup tables

enum
  {
    avr_op_index_dummy = 0,

    avr_op_index_ADC = 1,   avr_op_index_ADD,   avr_op_index_ADIW,
    avr_op_index_SBIW,
    avr_op_index_AND,   avr_op_index_ANDI,  avr_op_index_ASR,
    avr_op_index_BCLR,  avr_op_index_BLD,   avr_op_index_BRBC,
    avr_op_index_BRBS,  avr_op_index_BSET,  avr_op_index_BST,
    avr_op_index_CALL,  avr_op_index_CBI,   avr_op_index_COM,
    avr_op_index_CP,    avr_op_index_CPC,   avr_op_index_CPI,
    avr_op_index_CPSE,  avr_op_index_DEC,   avr_op_index_EICALL,
    avr_op_index_EIJMP, avr_op_index_ELPM,  avr_op_index_ELPM_Z,
    avr_op_index_ELPM_Z_incr,avr_op_index_EOR,  avr_op_index_ESPM,
    avr_op_index_FMUL,  avr_op_index_FMULS, avr_op_index_FMULSU,
    avr_op_index_ICALL, avr_op_index_IJMP,  avr_op_index_ILLEGAL,
    avr_op_index_IN,    avr_op_index_INC,   avr_op_index_JMP,
    avr_op_index_LDD_Y, avr_op_index_LDD_Z, avr_op_index_LDI,
    avr_op_index_LDS,   avr_op_index_LD_X,  avr_op_index_LD_X_decr,
    avr_op_index_LD_X_incr, avr_op_index_LD_Y_decr, avr_op_index_LD_Y_incr,
    avr_op_index_LD_Z_decr, avr_op_index_LD_Z_incr, avr_op_index_LPM,
    avr_op_index_LPM_Z, avr_op_index_LPM_Z_incr,avr_op_index_LSR,
    avr_op_index_MOV,   avr_op_index_MOVW,  avr_op_index_MUL,
    avr_op_index_MULS,  avr_op_index_MULSU, avr_op_index_NEG,
    avr_op_index_NOP,   avr_op_index_OR,    avr_op_index_ORI,
    avr_op_index_OUT,   avr_op_index_POP,   avr_op_index_PUSH,
    avr_op_index_RCALL, avr_op_index_RET,   avr_op_index_RETI,
    avr_op_index_RJMP,  avr_op_index_ROR,   avr_op_index_SBC,
    avr_op_index_SBCI,  avr_op_index_SBI,   avr_op_index_SBIC,
    avr_op_index_SBIS,  avr_op_index_SBRC,  avr_op_index_SBRS,
    avr_op_index_SLEEP, avr_op_index_SPM,   avr_op_index_STD_Y,
    avr_op_index_STD_Z, avr_op_index_STS,   avr_op_index_ST_X,
    avr_op_index_ST_X_decr, avr_op_index_ST_X_incr, avr_op_index_ST_Y_decr,
    avr_op_index_ST_Y_incr, avr_op_index_ST_Z_decr, avr_op_index_ST_Z_incr,
    avr_op_index_SUB,   avr_op_index_SUBI,  avr_op_index_SWAP,
    avr_op_index_WDR,
#ifdef ISA_XMEGA
    avr_op_index_XCH,
    avr_op_index_LAS, avr_op_index_LAC, avr_op_index_LAT,
#endif // ISA_XMEGA
  };

extern const opcode_data opcode_func_array[];

// ---------------------------------------------------------------------------
// vars that hold core definitions

// configured flash size
static unsigned int flash_addr_mask;

// maximum number of executed instructions (used as a timeout)
static dword max_instr_count;

// Initialize sram from .data segment.
static int flag_initialize_sram;

// From the command line, can be disabled by -no-stdin
static int flag_have_stdin = 1;

// From the command line, can be disabled by -no-stdout
static int flag_have_stdout = 1;

// filename of the file being executed
static const char *program_name;
static unsigned int program_size;

// ----------------------------------------------------------------------------
// vars that hold simulator state. These are kept as global vars for simplicity

// cycle counter
static dword program_cycles;

static unsigned cpu_PC;

#ifdef ISA_XMEGA
static byte cpu_reg[0x20];
#else
#define cpu_reg cpu_data
#endif /* XMEGA */

// cpu_data is used to store registers (non-xmega), ioport values
// and actual SRAM
static byte cpu_data[MAX_RAM_SIZE];

// flash
static byte cpu_flash[MAX_FLASH_SIZE];
static decoded_op decoded_flash[MAX_FLASH_SIZE/2];

// ----------------------------------------------------------------------------
//     log functions

#ifndef LOG_DUMP

// empty placeholders to keep the rest of the code clean

#define log_add_instr(...)    (void) 0
#define log_add_data_mov(...) (void) 0
#define log_add_reg_mov(...)  (void) 0
#define log_dump_line(...)    (void) 0

#else /* LOG_DUMP */

static char log_data[256];
char *cur_log_pos = log_data;

static void
log_add_data (const char *data)
{
  int len = strlen (data);
  memcpy (cur_log_pos, data, len);
  cur_log_pos += len;
}

static void
log_add_instr (const char *instr)
{
  char buf[32];
  if (arch.pc_3bytes)
    sprintf (buf, "%06x: %-5s ", cpu_PC * 2, instr);
  else
    sprintf (buf, "%04x: %-5s ", cpu_PC * 2, instr);
  log_add_data (buf);
}

static void
log_add_data_mov (const char *format, int addr, int value)
{
  char buf[32], adname[16];

  switch (addr)
    {
    case SREG: strcpy (adname, "SREG"); break;
    case SPH:  strcpy (adname, "SPH"); break;
    case SPL:  strcpy (adname, "SPL"); break;
    default:
      if (addr < 256)
        sprintf (adname, "%02x", addr);
      else
        sprintf (adname, "%04x", addr);
    }

  sprintf (buf, format, adname, value);
  log_add_data (buf);
}

static void
log_add_reg_mov (const char *format, int regno, int value)
{
  char buf[32];

  sprintf (buf, format, regno, value);
  log_add_data (buf);
}

static void
log_dump_line (void)
{
  *cur_log_pos = '\0';
  puts (log_data);
  cur_log_pos = log_data;
}

#endif /*  LOG_DUMP */


static const char *exit_status_text[] =
  {
    [EXIT_STATUS_EXIT]    = "EXIT",
    [EXIT_STATUS_ABORTED] = "ABORTED",
    [EXIT_STATUS_TIMEOUT] = "TIMEOUT"
  };

static void __attribute__((noreturn,noinline))
leave (int status, const char *reason)
{
  // make sure we print the last log line before leaving
  log_dump_line();

  printf ("\n"
          " exit status: %s\n"
          "      reason: %s\n"
          "     program: %s\n"
          "exit address: %06x\n"
          "total cycles: %u\n\n",
          exit_status_text[status], reason,
          program_name ? program_name : "-not set-",
          cpu_PC * 2, program_cycles);
  exit (0);
}

// ----------------------------------------------------------------------------
//     ioport / ram / flash, read / write entry points

// lowest level memory accessors

static INLINE int
data_read_byte_raw (int address)
{
  // add code here to handle special events
  if (address == STDIN_PORT && flag_have_stdin)
    return getchar();

  return cpu_data[address];
}

static INLINE void
data_write_byte_raw (int address, int value)
{
  // add code here to handle special events
  switch (address)
    {
    case STDOUT_PORT:
      if (!flag_have_stdout)
        break;
      putchar (0xff & value);
      return;
    case EXIT_PORT:
      leave ((0xff & value) ? EXIT_STATUS_ABORTED : EXIT_STATUS_EXIT,
             "exit function called");
      break;
    case ABORT_PORT:
      leave (EXIT_STATUS_ABORTED, "abort function called");
      break;
    }

  // default action, just store the value
  cpu_data[address] = value;
}

static INLINE int
flash_read_byte (int address)
{
  address &= flash_addr_mask;
  // add code here to handle special events
  return cpu_flash[address];
}


// mid-level memory accessors

// read a byte from memory / ioport / register

static INLINE int
data_read_byte (int address)
{
  int ret = data_read_byte_raw (address);
  log_add_data_mov ("(%s)->%02x ", address, ret);
  return ret;
}

// write a byte to memory / ioport / register

static INLINE void
data_write_byte (int address, int value)
{
  log_add_data_mov ("(%s)<-%02x ", address, value);
  data_write_byte_raw (address, value);
}

// get_reg / put_reg are just placeholders for read/write calls where we can
// be sure that the adress is < 32

static INLINE byte
get_reg (int regno)
{
  log_add_reg_mov ("(R%d)->%02x ", regno, cpu_reg[regno]);
  return cpu_reg[regno];
}

static INLINE void
put_reg (int regno, byte value)
{
  log_add_reg_mov ("(R%d)<-%02x ", regno, value);
  cpu_reg[regno] = value;
}

static INLINE int
get_word_reg (int regno)
{
  int ret = cpu_reg[regno] | (cpu_reg[regno + 1] << 8);
  log_add_reg_mov ("(R%d)->%04x ", regno, ret);
  return ret;
}

static INLINE void
put_word_reg (int regno, int value)
{
  log_add_reg_mov ("(R%d)<-%04x ", regno, value & 0xFFFF);
  cpu_reg[regno] = value;
  cpu_reg[regno + 1] = value >> 8;
}

// read a word from memory / ioport / register

static INLINE int
data_read_word (int address)
{
  int ret = (data_read_byte_raw (address)
             | (data_read_byte_raw (address + 1) << 8));
  log_add_data_mov ("(%s)->%04x ", address, ret);
  return ret;
}

// write a word to memory / ioport / register

static INLINE void
data_write_word (int address, int value)
{
  value &= 0xffff;
  log_add_data_mov ("(%s)<-%04x ", address, value);
  data_write_byte_raw (address, value & 0xFF);
  data_write_byte_raw (address + 1, value >> 8);
}

// ----------------------------------------------------------------------------
//     flag manipulation functions

static INLINE void
update_flags (int flags, int new_values)
{
  int sreg = data_read_byte (SREG);
  sreg = (sreg & ~flags) | new_values;
  data_write_byte (SREG, sreg);
}

static INLINE int
get_carry (void)
{
  return (data_read_byte (SREG) & FLAG_C) != 0;
}

// fast flag update tables to avoid conditional branches on frequently
// used operations

#define FUT_ADD_SUB_INDEX(v1,v2,res)            \
  ((((v1) & 0x08) << 9)                         \
   | (((v2) & 0x08) << 8)                       \
   | (((v1) & 0x80) << 3)                       \
   | (((v2) & 0x80) << 2)                       \
   | ((res) & 0x1FF))

#define FUT_ADDSUB16_INDEX(v1, res)                             \
  (((((v1) >> 8) & 0x80) << 3) | (((res) >> 8) & 0x1FF))

// ----------------------------------------------------------------------------
//     helper functions

static INLINE void
add_program_cycles (dword cycles)
{
  program_cycles += cycles;
}


static INLINE void
push_byte (int value)
{
  int sp = data_read_word (SPL);
  // temporary hack to disallow growing the stack over the reserved
  // register area
  if (sp < 0x40 + IOBASE)
    leave (EXIT_STATUS_ABORTED, "stack pointer overflow");
  data_write_byte (sp, value);
  data_write_word (SPL, sp - 1);
}

static int
pop_byte(void)
{
  int sp = data_read_word (SPL);
  sp++;
  data_write_word (SPL, sp);
  return data_read_byte (sp);
}

static INLINE void
push_PC (void)
{
  int sp = data_read_word (SPL);
  // temporary hack to disallow growing the stack over the reserved
  // register area
  if (sp < 0x40 + IOBASE)
    leave (EXIT_STATUS_ABORTED, "stack pointer overflow");
  data_write_byte (sp, cpu_PC);
  sp--;
  data_write_byte (sp, cpu_PC >> 8);
  sp--;
  if (arch.pc_3bytes)
    {
      data_write_byte (sp, cpu_PC >> 16);
      sp--;
    }
  data_write_word (SPL, sp);
}

static INLINE void
pop_PC (void)
{
  unsigned pc = 0;
  int sp = data_read_word (SPL);
  if (arch.pc_3bytes)
    {
      sp++;
      pc = data_read_byte (sp) << 16;
      if (pc >= MAX_FLASH_SIZE / 2)
        leave (EXIT_STATUS_ABORTED, "program counter out of bounds");
    }
  sp++;
  pc |= data_read_byte (sp) << 8;
  sp++;
  pc |= data_read_byte (sp);
  data_write_word (SPL, sp);
  cpu_PC = pc;
}


// perform the addition and set the appropriate flags
static INLINE void
do_addition_8 (int rd, int rr, int carry)
{
  int value1, value2, sreg, result;

  value1 = get_reg (rd);
  value2 = get_reg (rr);

  result = value1 + value2 + carry;

  sreg = flag_update_table_add8[FUT_ADD_SUB_INDEX (value1, value2, result)];
  update_flags (FLAG_H | FLAG_S | FLAG_V | FLAG_N | FLAG_Z | FLAG_C, sreg);

  put_reg (rd, result & 0xFF);
}

// perform the subtraction and set the appropriate flags
static INLINE int
do_subtraction_8 (int value1, int value2, int carry, int use_carry)
{
  int sreg, result = value1 - value2 - carry;
  unsigned flag;

  sreg = flag_update_table_sub8[FUT_ADD_SUB_INDEX (value1, value2, result)];
  flag = use_carry && ((data_read_byte (SREG) & FLAG_Z) == 0);
  sreg &= ~(flag << FLAG_Z_BIT);
  update_flags (FLAG_H | FLAG_S | FLAG_V | FLAG_N | FLAG_Z | FLAG_C, sreg);

  return result & 0xFF;
}

static INLINE void
store_logical_result (int rd, int result)
{
  put_reg (rd, result);
  update_flags (FLAG_S | FLAG_V | FLAG_N | FLAG_Z,
                flag_update_table_logical[result]);
}

/* 10q0 qq0d dddd 1qqq | LDD */

static INLINE void
load_indirect (int rd, int ind_register, int adjust, int displacement_bits)
{
  //TODO: use RAMPx registers to address more than 64kb of RAM
  int ind_value = get_word_reg (ind_register);

  if (adjust < 0)
    ind_value += adjust;
  put_reg (rd, data_read_byte (ind_value + displacement_bits));
  if (adjust > 0)
    ind_value += adjust;
  if (adjust)
    put_word_reg (ind_register, ind_value);
}

static INLINE void 
store_indirect (int rd, int ind_register, int adjust, int displacement_bits)
{
  //TODO: use RAMPx registers to address more than 64kb of RAM
  int ind_value = get_word_reg (ind_register);

  if (adjust < 0)
    ind_value += adjust;
  data_write_byte (ind_value + displacement_bits, get_reg (rd));
  if (adjust > 0)
    ind_value += adjust;
  if (adjust)
    put_word_reg (ind_register, ind_value);
}

static INLINE void
load_program_memory (int rd, int use_RAMPZ, int incr)
{
  int address = get_word_reg (REGZ);
  if (use_RAMPZ)
    address |= data_read_byte (RAMPZ) << 16;
  put_reg (rd, flash_read_byte (address));
  if (incr)
    {
      address++;
      put_word_reg (REGZ, address & 0xFFFF);
      if (use_RAMPZ)
        data_write_byte (RAMPZ, address >> 16);
    }
}

static INLINE void
skip_instruction_on_condition (int condition)
{
  int size;
  if (condition)
    {
      size = opcode_func_array[decoded_flash[cpu_PC].data_index].size;
      cpu_PC = (cpu_PC + size) & PC_VALID_MASK;
      add_program_cycles (size);
    }
}

static INLINE void
branch_on_sreg_condition (int rd, int rr, int flag_value)
{
  if (((data_read_byte (SREG) & rr) != 0) == flag_value)
    {
      int delta = rd;
      // if (delta & 0x40) delta |= ~0x7F;
      unsigned neg = (delta & 0x40) << 1;
      delta |= -neg;
      cpu_PC = (cpu_PC + delta) & PC_VALID_MASK;
      add_program_cycles (1);
    }
}

static INLINE void
rotate_right (int rd, int value, int top_bit)
{
  value |= (!!top_bit) << 8;
  put_reg (rd, value >> 1);

  update_flags (FLAG_S | FLAG_V | FLAG_N | FLAG_Z | FLAG_C,
                flag_update_table_ror8[value]);
}

static INLINE void
do_multiply (int rd, int rr, int signed1, int signed2, int shift)
{
  int v1, v2, result, sreg;

  v1 = signed1 ? ((signed char) get_reg (rd)) : get_reg (rd);
  v2 = signed2 ? ((signed char) get_reg (rr)) : get_reg (rr);
  result = (v1 * v2) & 0xFFFF;

  sreg = (result & 0x8000) >> (15 - FLAG_C_BIT);
  result <<= shift;
  sreg |= FLAG_Z & - (result == 0);
  update_flags (FLAG_Z | FLAG_C, sreg);
  put_word_reg (0, result);
}

// handle illegal instructions
static OP_FUNC_TYPE avr_op_ILLEGAL (int rd, int rr)
{
  char buf[128];
  sprintf (buf, "illegal opcode %04x", rr);
  leave (EXIT_STATUS_ABORTED, buf);
}

// ----------------------------------------------------------------------------
//     opcode execution functions

/* opcodes with no operands */
/* 1001 0101 0001 1001 | EICALL */
static OP_FUNC_TYPE avr_op_EICALL (int rd, int rr)
{
  if (arch.has_eind)
    {
      push_PC();
      cpu_PC = get_word_reg (REGZ) | (data_read_byte (EIND) << 16);
      if (cpu_PC >= MAX_FLASH_SIZE / 2)
        leave (EXIT_STATUS_ABORTED, "program counter out of bounds");
    }
  else
    {
      avr_op_ILLEGAL (0, 0x9519);
    }
}

/* 1001 0100 0001 1001 | EIJMP */
static OP_FUNC_TYPE avr_op_EIJMP (int rd, int rr)
{
  if (arch.has_eind)
    {
      cpu_PC = get_word_reg (REGZ) | (data_read_byte (EIND) << 16);
      if (cpu_PC >= MAX_FLASH_SIZE / 2)
        leave (EXIT_STATUS_ABORTED, "program counter out of bounds");
    }
  else
    {
      avr_op_ILLEGAL (0, 0x9419);
    }
}

/* 1001 0101 1101 1000 | ELPM */
static OP_FUNC_TYPE avr_op_ELPM (int rd, int rr)
{
  load_program_memory (0, 1, 0);
}

/* 1001 0101 1111 1000 | ESPM */
static OP_FUNC_TYPE avr_op_ESPM (int rd, int rr)
{
  avr_op_ILLEGAL (0, 0x95F8);
  //TODO
}

/* 1001 0101 0000 1001 | ICALL */
static OP_FUNC_TYPE avr_op_ICALL (int rd, int rr)
{
  push_PC();
  cpu_PC = get_word_reg (REGZ);
  add_program_cycles (arch.pc_3bytes);
}

/* 1001 0100 0000 1001 | IJMP */
static OP_FUNC_TYPE avr_op_IJMP (int rd, int rr)
{
  cpu_PC = get_word_reg (REGZ);
}

/* 1001 0101 1100 1000 | LPM */
static OP_FUNC_TYPE avr_op_LPM (int rd, int rr)
{
  load_program_memory (0, 0, 0);
}

/* 0000 0000 0000 0000 | NOP */
static OP_FUNC_TYPE avr_op_NOP (int rd, int rr)
{
}

/* 1001 0101 0000 1000 | RET */
static OP_FUNC_TYPE avr_op_RET (int rd, int rr)
{
  pop_PC();
  add_program_cycles (arch.pc_3bytes);
}

/* 1001 0101 0001 1000 | RETI */
static OP_FUNC_TYPE avr_op_RETI (int rd, int rr)
{
  avr_op_RET (rd, rr);
  update_flags (FLAG_I, FLAG_I);
}

/* 1001 0101 1000 1000 | SLEEP */
static OP_FUNC_TYPE avr_op_SLEEP (int rd, int rr)
{
  // we don't have anything to wake us up, so just pretend we wake
  // up immediately
}

/* 1001 0101 1110 1000 | SPM */
static OP_FUNC_TYPE avr_op_SPM (int rd, int rr)
{
  avr_op_ILLEGAL (0,0x95E8);
  //TODO
}

/* 1001 0101 1010 1000 | WDR */
static OP_FUNC_TYPE avr_op_WDR (int rd, int rr)
{
  // we don't have a watchdog, so do nothing
}


/* opcodes with two 5-bit register (Rd and Rr) operands */
/* 0001 11rd dddd rrrr | ADC or ROL */
static OP_FUNC_TYPE avr_op_ADC (int rd, int rr)
{
  do_addition_8 (rd, rr, get_carry());
}

/* 0000 11rd dddd rrrr | ADD or LSL */
static OP_FUNC_TYPE avr_op_ADD (int rd, int rr)
{
  do_addition_8 (rd, rr, 0);
}

/* 0010 00rd dddd rrrr | AND or TST */
static OP_FUNC_TYPE avr_op_AND (int rd, int rr)
{
  int result = get_reg (rd) & get_reg (rr);
  store_logical_result (rd, result);
}

/* 0001 01rd dddd rrrr | CP */
static OP_FUNC_TYPE avr_op_CP (int rd, int rr)
{
  do_subtraction_8 (get_reg (rd), get_reg (rr), 0, 0);
}

/* 0000 01rd dddd rrrr | CPC */
static OP_FUNC_TYPE avr_op_CPC (int rd, int rr)
{
  do_subtraction_8 (get_reg (rd), get_reg (rr), get_carry(), 1);
}

/* 0001 00rd dddd rrrr | CPSE */
static OP_FUNC_TYPE avr_op_CPSE (int rd, int rr)
{
  skip_instruction_on_condition (get_reg (rd) == get_reg (rr));
}

/* 0010 01rd dddd rrrr | EOR or CLR */
static OP_FUNC_TYPE avr_op_EOR (int rd, int rr)
{
  int result = get_reg (rd) ^ get_reg (rr);
  store_logical_result (rd, result);
}

/* 0010 11rd dddd rrrr | MOV */
static OP_FUNC_TYPE avr_op_MOV (int rd, int rr)
{
  put_reg (rd, get_reg (rr));
}

/* 1001 11rd dddd rrrr | MUL */
static OP_FUNC_TYPE avr_op_MUL (int rd, int rr)
{
  do_multiply (rd, rr, 0, 0, 0);
}

/* 0010 10rd dddd rrrr | OR */
static OP_FUNC_TYPE avr_op_OR (int rd, int rr)
{
  int result = get_reg (rd) | get_reg (rr);
  store_logical_result (rd, result);
}

/* 0000 10rd dddd rrrr | SBC */
static OP_FUNC_TYPE avr_op_SBC (int rd, int rr)
{
  put_reg (rd, do_subtraction_8 (get_reg (rd), get_reg (rr), get_carry(), 1));
}

/* 0001 10rd dddd rrrr | SUB */
static OP_FUNC_TYPE avr_op_SUB (int rd, int rr)
{
  put_reg (rd, do_subtraction_8 (get_reg (rd), get_reg (rr), 0, 0));
}


/* opcode with a single register (Rd) as operand */
/* 1001 010d dddd 0101 | ASR */
static OP_FUNC_TYPE avr_op_ASR (int rd, int rr)
{
  int value = get_reg (rd);
  rotate_right (rd, value, value & 0x80);
}

/* 1001 010d dddd 0000 | COM */
static OP_FUNC_TYPE avr_op_COM (int rd, int rr)
{
  int result = 0xFF & ~get_reg (rd);
  put_reg (rd, result);
  update_flags (FLAG_S | FLAG_V | FLAG_N | FLAG_Z | FLAG_C,
                flag_update_table_logical[result] | FLAG_C);
}

/* 1001 010d dddd 1010 | DEC */
static OP_FUNC_TYPE avr_op_DEC (int rd, int rr)
{
  int result = (get_reg (rd) - 1) & 0xFF;
  put_reg (rd, result);
  update_flags (FLAG_S | FLAG_V | FLAG_N | FLAG_Z,
                flag_update_table_dec[result]);
}

/* 1001 000d dddd 0110 | ELPM */
static OP_FUNC_TYPE avr_op_ELPM_Z (int rd, int rr)
{
  load_program_memory (rd, 1, 0);
}

/* 1001 000d dddd 0111 | ELPM */
static OP_FUNC_TYPE avr_op_ELPM_Z_incr (int rd, int rr)
{
  load_program_memory (rd, 1, 1);
}

/* 1001 010d dddd 0011 | INC */
static OP_FUNC_TYPE avr_op_INC (int rd, int rr)
{
  int result = (get_reg (rd) + 1) & 0xFF;
  put_reg (rd, result);
  update_flags (FLAG_S | FLAG_V | FLAG_N | FLAG_Z,
                flag_update_table_inc[result]);
}

/* 1001 000d dddd 0000 | LDS */
static OP_FUNC_TYPE avr_op_LDS (int rd, int rr)
{
  //TODO:RAMPD
  put_reg (rd, data_read_byte (rr));
}

/* 1001 000d dddd 1100 | LD */
static OP_FUNC_TYPE avr_op_LD_X (int rd, int rr)
{
  load_indirect (rd, REGX, 0, 0);
}

/* 1001 000d dddd 1110 | LD */
static OP_FUNC_TYPE avr_op_LD_X_decr (int rd, int rr)
{
  load_indirect (rd, REGX, -1, 0);
}

/* 1001 000d dddd 1101 | LD */
static OP_FUNC_TYPE avr_op_LD_X_incr (int rd, int rr)
{
  load_indirect (rd, REGX, 1, 0);
}

/* 1001 000d dddd 1010 | LD */
static OP_FUNC_TYPE avr_op_LD_Y_decr (int rd, int rr)
{
  load_indirect (rd, REGY, -1, 0);
}

/* 1001 000d dddd 1001 | LD */
static OP_FUNC_TYPE avr_op_LD_Y_incr (int rd, int rr)
{
  load_indirect (rd, REGY, 1, 0);
}

/* 1001 000d dddd 0010 | LD */
static OP_FUNC_TYPE avr_op_LD_Z_decr (int rd, int rr)
{
  load_indirect (rd, REGZ, -1, 0);
}

/* 1001 000d dddd 0010 | LD */
static OP_FUNC_TYPE avr_op_LD_Z_incr (int rd, int rr)
{
  load_indirect (rd, REGZ, 1, 0);
}

/* 1001 000d dddd 0100 | LPM Z */
static OP_FUNC_TYPE avr_op_LPM_Z (int rd, int rr)
{
  load_program_memory (rd, 0, 0);
}

/* 1001 000d dddd 0101 | LPM Z+ */
static OP_FUNC_TYPE avr_op_LPM_Z_incr (int rd, int rr)
{
  load_program_memory (rd, 0, 1);
}

/* 1001 010d dddd 0110 | LSR */
static OP_FUNC_TYPE avr_op_LSR (int rd, int rr)
{
  rotate_right (rd, get_reg (rd), 0);
}

/* 1001 010d dddd 0001 | NEG */
static OP_FUNC_TYPE avr_op_NEG (int rd, int rr)
{
  put_reg (rd, do_subtraction_8 (0, get_reg (rd), 0, 0));
}

/* 1001 000d dddd 1111 | POP */
static OP_FUNC_TYPE avr_op_POP (int rd, int rr)
{
  put_reg (rd, pop_byte());
}

#ifdef ISA_XMEGA

static void
xmega_atomic (int regno, int op)
{
  int mask = get_reg (regno);
  int address = get_word_reg (REGZ);
  int val = data_read_byte (address);

  put_reg (regno, val);

  switch (op)
    {
    case avr_op_index_XCH: val = mask; break;
    case avr_op_index_LAS: val |=  mask; break;
    case avr_op_index_LAC: val &= ~mask; break;
    case avr_op_index_LAT: val ^=  mask; break;
    }

  data_write_byte (address, val);
}

/* 1001 001d dddd 0100 | XCH */
static OP_FUNC_TYPE avr_op_XCH (int rd, int rr)
{
  xmega_atomic (rd, avr_op_index_XCH);
}

/* 1001 001d dddd 0101 | LAS */
static OP_FUNC_TYPE avr_op_LAS (int rd, int rr)
{
  xmega_atomic (rd, avr_op_index_LAS);
}

/* 1001 001d dddd 0110 | LAC */
static OP_FUNC_TYPE avr_op_LAC (int rd, int rr)
{
  xmega_atomic (rd, avr_op_index_LAC);
}

/* 1001 001d dddd 0111 | LAT */
static OP_FUNC_TYPE avr_op_LAT (int rd, int rr)
{
  xmega_atomic (rd, avr_op_index_LAT);
}

#endif // ISA_XMEGA

/* 1001 001d dddd 1111 | PUSH */
static OP_FUNC_TYPE avr_op_PUSH (int rd, int rr)
{
  push_byte (get_reg (rd));
}

/* 1001 010d dddd 0111 | ROR */
static OP_FUNC_TYPE avr_op_ROR (int rd, int rr)
{
  rotate_right (rd, get_reg (rd), get_carry());
}

/* 1001 001d dddd 0000 | STS */
static OP_FUNC_TYPE avr_op_STS (int rd, int rr)
{
  //TODO:RAMPD
  data_write_byte (rr, get_reg (rd));
}

/* 1001 001d dddd 1100 | ST */
static OP_FUNC_TYPE avr_op_ST_X (int rd, int rr)
{
  store_indirect (rd, REGX, 0, 0);
}

/* 1001 001d dddd 1110 | ST */
static OP_FUNC_TYPE avr_op_ST_X_decr (int rd, int rr)
{
  store_indirect (rd, REGX, -1, 0);
}

/* 1001 001d dddd 1101 | ST */
static OP_FUNC_TYPE avr_op_ST_X_incr (int rd, int rr)
{
  store_indirect (rd, REGX, 1, 0);
}

/* 1001 001d dddd 1010 | ST */
static OP_FUNC_TYPE avr_op_ST_Y_decr (int rd, int rr)
{
  store_indirect (rd, REGY, -1, 0);
}

/* 1001 001d dddd 1001 | ST */
static OP_FUNC_TYPE avr_op_ST_Y_incr (int rd, int rr)
{
  store_indirect (rd, REGY, 1, 0);
}

/* 1001 001d dddd 0010 | ST */
static OP_FUNC_TYPE avr_op_ST_Z_decr (int rd, int rr)
{
  store_indirect (rd, REGZ, -1, 0);
}

/* 1001 001d dddd 0001 | ST */
static OP_FUNC_TYPE avr_op_ST_Z_incr (int rd, int rr)
{
  store_indirect (rd, REGZ, 1, 0);
}

/* 1001 010d dddd 0010 | SWAP */
static OP_FUNC_TYPE avr_op_SWAP (int rd, int rr)
{
  int value = get_reg (rd);
  put_reg (rd, ((value << 4) & 0xF0) | ((value >> 4) & 0x0F));
}

/* opcodes with a register (Rd) and a constant data (K) as operands */
/* 0111 KKKK dddd KKKK | CBR or ANDI */
static OP_FUNC_TYPE avr_op_ANDI (int rd, int rr)
{
  int result = get_reg (rd) & rr;
  store_logical_result (rd, result);
}

/* 0011 KKKK dddd KKKK | CPI */
static OP_FUNC_TYPE avr_op_CPI (int rd, int rr)
{
  do_subtraction_8 (get_reg (rd), rr, 0, 0);
}

/* 1110 KKKK dddd KKKK | LDI or SER */
static OP_FUNC_TYPE avr_op_LDI (int rd, int rr)
{
  put_reg (rd, rr);
}

/* 0110 KKKK dddd KKKK | SBR or ORI */
static OP_FUNC_TYPE avr_op_ORI (int rd, int rr)
{
  int result = get_reg (rd) | rr;
  store_logical_result (rd, result);
}

/* 0100 KKKK dddd KKKK | SBCI */
/* 0101 KKKK dddd KKKK | SUBI */
static INLINE OP_FUNC_TYPE avr_op_SBCI_SUBI (int rd, int rr, int carry,
                                             int use_carry)
{
  put_reg (rd, do_subtraction_8 (get_reg (rd), rr, carry, use_carry));
}

static OP_FUNC_TYPE avr_op_SBCI (int rd, int rr)
{
  avr_op_SBCI_SUBI (rd, rr, get_carry(), 1);
}

static OP_FUNC_TYPE avr_op_SUBI (int rd, int rr)
{
  avr_op_SBCI_SUBI (rd, rr, 0, 0);
}


/* opcodes with a register (Rd) and a register bit number (b) as operands */
/* 1111 100d dddd 0bbb | BLD */
static OP_FUNC_TYPE avr_op_BLD (int rd, int rr)
{
  int value = get_reg (rd) | rr;
  unsigned flag = (data_read_byte (SREG) >> FLAG_T_BIT) & 1;
  value &= ~rr | -flag;
  put_reg (rd, value);
}

/* 1111 101d dddd 0bbb | BST */
static OP_FUNC_TYPE avr_op_BST (int rd, int rr)
{
  int bit = get_reg (rd) & rr;
  update_flags (FLAG_T, bit ? FLAG_T : 0);
}

/* 1111 110d dddd 0bbb | SBRC */
static OP_FUNC_TYPE avr_op_SBRC (int rd, int rr)
{
  skip_instruction_on_condition (!(get_reg (rd) & rr));
}

/* 1111 111d dddd 0bbb | SBRS */
static OP_FUNC_TYPE avr_op_SBRS (int rd, int rr)
{
  skip_instruction_on_condition (get_reg (rd) & rr);
}

/* opcodes with a relative 7-bit address (k) and a register bit number (b)
   as operands */
/* 1111 01kk kkkk kbbb | BRBC */
static OP_FUNC_TYPE avr_op_BRBC (int rd, int rr)
{
  branch_on_sreg_condition (rd, rr, 0);
}

/* 1111 00kk kkkk kbbb | BRBS */
static OP_FUNC_TYPE avr_op_BRBS (int rd, int rr)
{
  branch_on_sreg_condition (rd, rr, 1);
}


/* opcodes with a 6-bit address displacement (q) and a register (Rd)
   as operands */
/* 10q0 qq0d dddd 1qqq | LDD */
static OP_FUNC_TYPE avr_op_LDD_Y (int rd, int rr)
{
  load_indirect (rd, REGY, 0, rr);
}

/* 10q0 qq0d dddd 0qqq | LDD */
static OP_FUNC_TYPE avr_op_LDD_Z (int rd, int rr)
{
  load_indirect (rd, REGZ, 0, rr);
}

/* 10q0 qq1d dddd 1qqq | STD */
static OP_FUNC_TYPE avr_op_STD_Y (int rd, int rr)
{
  store_indirect (rd, REGY, 0, rr);
}

/* 10q0 qq1d dddd 0qqq | STD */
static OP_FUNC_TYPE avr_op_STD_Z (int rd, int rr)
{
  store_indirect (rd, REGZ, 0, rr);
}


/* opcodes with a absolute 22-bit address (k) operand */
/* 1001 010k kkkk 110k | JMP */
static OP_FUNC_TYPE avr_op_JMP (int rd, int rr)
{
  cpu_PC = rr | (rd << 16);
}

/* 1001 010k kkkk 111k | CALL */
static OP_FUNC_TYPE avr_op_CALL (int rd, int rr)
{
  push_PC();
  cpu_PC = rr | (rd << 16);
  add_program_cycles (arch.pc_3bytes);
}


/* opcode with a sreg bit select (s) operand */
/* BCLR takes place of CL{C,Z,N,V,S,H,T,I} */
/* BSET takes place of SE{C,Z,N,V,S,H,T,I} */
/* 1001 0100 1sss 1000 | BCLR */
static OP_FUNC_TYPE avr_op_BCLR (int rd, int rr)
{
  update_flags (rd, 0);
}

/* 1001 0100 0sss 1000 | BSET */
static OP_FUNC_TYPE avr_op_BSET (int rd, int rr)
{
  update_flags (rd, rd);
}


/* opcodes with a 6-bit constant (K) and a register (Rd) as operands */
/* 1001 0110 KKdd KKKK | ADIW */
static OP_FUNC_TYPE avr_op_ADIW (int rd, int rr)
{
  int svalue, evalue, sreg;
  unsigned flag;

  svalue = get_word_reg (rd);
  evalue = svalue + rr;
  put_word_reg (rd, evalue);

  sreg = flag_update_table_add8[FUT_ADDSUB16_INDEX (svalue, evalue)];
  sreg &= ~FLAG_H;
  flag = (evalue & 0xFFFF) != 0;
  sreg &= ~(flag << FLAG_Z_BIT);

  update_flags (FLAG_S | FLAG_V | FLAG_N | FLAG_Z | FLAG_C, sreg);
}

/* 1001 0111 KKdd KKKK | SBIW */
static OP_FUNC_TYPE avr_op_SBIW (int rd, int rr)
{
  int svalue, evalue, sreg;
  unsigned flag;

  svalue = get_word_reg (rd);
  evalue = svalue - rr;
  put_word_reg (rd, evalue);

  sreg = flag_update_table_sub8[FUT_ADDSUB16_INDEX (svalue, evalue)];
  sreg &= ~FLAG_H;
  flag = (evalue & 0xFFFF) != 0;
  sreg &= ~(flag << FLAG_Z_BIT);

  update_flags (FLAG_S | FLAG_V | FLAG_N | FLAG_Z | FLAG_C, sreg);
}


/* opcodes with a 5-bit IO Addr (A) and register bit number (b) as operands */
/* 1001 1000 AAAA Abbb | CBI */
static OP_FUNC_TYPE avr_op_CBI (int rd, int rr)
{
  data_write_byte (rd, data_read_byte (rd) & ~(rr));
}

/* 1001 1010 AAAA Abbb | SBI */
static OP_FUNC_TYPE avr_op_SBI (int rd, int rr)
{
  data_write_byte (rd, data_read_byte (rd) | rr);
}

/* 1001 1001 AAAA Abbb | SBIC */
static OP_FUNC_TYPE avr_op_SBIC (int rd, int rr)
{
  skip_instruction_on_condition (!(data_read_byte (rd) & rr));
}

/* 1001 1011 AAAA Abbb | SBIS */
static OP_FUNC_TYPE avr_op_SBIS (int rd, int rr)
{
  skip_instruction_on_condition (data_read_byte (rd) & rr);
}


/* opcodes with a 6-bit IO Addr (A) and register (Rd) as operands */
/* 1011 0AAd dddd AAAA | IN */
static OP_FUNC_TYPE avr_op_IN (int rd, int rr)
{
  put_reg (rd, data_read_byte (rr));
}

/* 1011 1AAd dddd AAAA | OUT */
static OP_FUNC_TYPE avr_op_OUT (int rd, int rr)
{
  data_write_byte (rr, get_reg (rd));
}


/* opcodes with a relative 12-bit address (k) operand */
/* 1100 kkkk kkkk kkkk | RJMP */
static OP_FUNC_TYPE avr_op_RJMP (int rd, int rr)
{
  int delta = rr;
  //if (delta & 0x800) delta |= ~0xFFF;
  unsigned neg = (delta & 0x800) << 1;
  delta |= -neg;

  // special case: endless loop usually means that the program has ended
  if (delta == -1)
    leave (EXIT_STATUS_EXIT, "infinite loop detected (normal exit)");
  cpu_PC = (cpu_PC + delta) & PC_VALID_MASK;
}

/* 1101 kkkk kkkk kkkk | RCALL */
static OP_FUNC_TYPE avr_op_RCALL (int rd, int rr)
{
  int delta = rr;
  // if (delta & 0x800) delta |= ~0xFFF;
  unsigned neg = (delta & 0x800) << 1;
  delta |= -neg;

  push_PC();
  cpu_PC = (cpu_PC + delta) & PC_VALID_MASK;
  add_program_cycles (arch.pc_3bytes);
}


/* opcodes with two 4-bit register (Rd and Rr) operands */
/* 0000 0001 dddd rrrr | MOVW */
static OP_FUNC_TYPE avr_op_MOVW (int rd, int rr)
{
#ifdef LOG_DUMP
  put_word_reg (rd, get_word_reg (rr));
#else
  put_reg (rd,     get_reg (rr));
  put_reg (rd + 1, get_reg (rr + 1));
#endif
}

/* 0000 0010 dddd rrrr | MULS */
static OP_FUNC_TYPE avr_op_MULS (int rd, int rr)
{
  do_multiply (rd, rr, 1, 1, 0);
}

/* opcodes with two 3-bit register (Rd and Rr) operands */
/* 0000 0011 0ddd 0rrr | MULSU */
static OP_FUNC_TYPE avr_op_MULSU (int rd, int rr)
{
  do_multiply (rd, rr, 1, 0, 0);
}

/* 0000 0011 0ddd 1rrr | FMUL */
static OP_FUNC_TYPE avr_op_FMUL (int rd, int rr)
{
  do_multiply (rd, rr, 0, 0, 1);
}

/* 0000 0011 1ddd 0rrr | FMULS */
static OP_FUNC_TYPE avr_op_FMULS (int rd, int rr)
{
  do_multiply (rd, rr, 1, 1, 1);
}

/* 0000 0011 1ddd 1rrr | FMULSU */
static OP_FUNC_TYPE avr_op_FMULSU (int rd, int rr)
{
  do_multiply (rd, rr, 1, 0, 1);
}


// ----------------------------------------------------------------------------
//     ELF loader.

typedef uint16_t Elf32_Half;
typedef uint32_t Elf32_Word;
typedef uint32_t Elf32_Addr;
typedef uint32_t Elf32_Off;

#define EI_NIDENT 16
typedef struct
{
  unsigned char e_ident[EI_NIDENT]; /* Magic number and other info */
  Elf32_Half    e_type;                 /* Object file type */
  Elf32_Half    e_machine;              /* Architecture */
  Elf32_Word    e_version;              /* Object file version */
  Elf32_Addr    e_entry;                /* Entry point virtual address */
  Elf32_Off     e_phoff;                /* Program header table file offset */
  Elf32_Off     e_shoff;                /* Section header table file offset */
  Elf32_Word    e_flags;                /* Processor-specific flags */
  Elf32_Half    e_ehsize;               /* ELF header size in bytes */
  Elf32_Half    e_phentsize;            /* Program header table entry size */
  Elf32_Half    e_phnum;                /* Program header table entry count */
  Elf32_Half    e_shentsize;            /* Section header table entry size */
  Elf32_Half    e_shnum;                /* Section header table entry count */
  Elf32_Half    e_shstrndx;             /* Section header string table index */
} Elf32_Ehdr;

typedef struct
{
  Elf32_Word    p_type;                 /* Segment type */
  Elf32_Off     p_offset;               /* Segment file offset */
  Elf32_Addr    p_vaddr;                /* Segment virtual address */
  Elf32_Addr    p_paddr;                /* Segment physical address */
  Elf32_Word    p_filesz;               /* Segment size in file */
  Elf32_Word    p_memsz;                /* Segment size in memory */
  Elf32_Word    p_flags;                /* Segment flags */
  Elf32_Word    p_align;                /* Segment alignment */
} Elf32_Phdr;

#define EI_CLASS 4
#define ELFCLASS32 1

#define EI_DATA 5
#define ELFDATA2LSB 1

#define EI_VERSION 5
#define EV_CURRENT 1

#define ET_EXEC 2
#define EM_AVR 0x53

#define PT_LOAD 1

#define DATA_VADDR 0x800000

static Elf32_Half
get_elf32_half (Elf32_Half *v)
{
  unsigned char *p = (unsigned char*) v;
  return p[0] | (p[1] << 8);
}

static Elf32_Word
get_elf32_word (Elf32_Word *v)
{
  unsigned char *p = (unsigned char*) v;
  return p[0] | (p[1] << 8) | (p[2] << 16) | (p[3] << 24);
}

static void
load_elf (FILE *f)
{
  Elf32_Ehdr ehdr;
  Elf32_Phdr phdr[16];
  int nbr_phdr;
  int i;
  size_t res;
    
  rewind (f);
  if (fread (&ehdr, sizeof (ehdr), 1, f) != 1)
    leave (EXIT_STATUS_ABORTED, "can't read ELF header");
  if (ehdr.e_ident[EI_CLASS] != ELFCLASS32
      || ehdr.e_ident[EI_DATA] != ELFDATA2LSB
      || ehdr.e_ident[EI_VERSION] != EV_CURRENT)
    leave (EXIT_STATUS_ABORTED, "bad ELF header");
  if (get_elf32_half (&ehdr.e_type) != ET_EXEC
      || get_elf32_half (&ehdr.e_machine) != EM_AVR
      || get_elf32_word (&ehdr.e_version) != EV_CURRENT
      || get_elf32_half (&ehdr.e_phentsize) != sizeof (Elf32_Phdr))
    leave (EXIT_STATUS_ABORTED, "ELF file is not an AVR executable");

  nbr_phdr = get_elf32_half (&ehdr.e_phnum);
  if ((unsigned)nbr_phdr > sizeof (phdr)/sizeof (*phdr))
    leave (EXIT_STATUS_ABORTED, "ELF file contains too many PHDR");

  if (fseek (f, get_elf32_word (&ehdr.e_phoff), SEEK_SET) != 0)
    leave (EXIT_STATUS_ABORTED, "ELF file truncated");
  res = fread (phdr, sizeof (Elf32_Phdr), nbr_phdr, f);
  if (res != (size_t)nbr_phdr)
    leave (EXIT_STATUS_ABORTED, "can't read PHDRs of ELF file");
  for (i = 0; i < nbr_phdr; i++)
    {
      Elf32_Addr addr = get_elf32_word (&phdr[i].p_paddr);
      Elf32_Addr vaddr = get_elf32_word (&phdr[i].p_vaddr);
      Elf32_Word filesz = get_elf32_word (&phdr[i].p_filesz);
      Elf32_Word memsz= get_elf32_word (&phdr[i].p_memsz);
        
      if (get_elf32_word (&phdr[i].p_type) != PT_LOAD)
        continue;
      if (filesz == 0)
        continue;
      /* if (verbose)
         printf ("Load 0x%06x - 0x%06x\n",
         (unsigned)addr, (unsigned)(addr + memsz)); */
      if (addr + memsz > MAX_FLASH_SIZE)
        leave (EXIT_STATUS_ABORTED,
               "program too big to fit in flash");
      if (fseek (f, get_elf32_word (&phdr[i].p_offset), SEEK_SET) != 0)
        leave (EXIT_STATUS_ABORTED, "ELF file truncated");
      if (fread (cpu_flash + addr, filesz, 1, f) != 1)
        leave (EXIT_STATUS_ABORTED, "ELF file truncated");
      /* Also copy in SRAM.  */
      if (flag_initialize_sram && vaddr >= DATA_VADDR)
        memcpy (cpu_data + vaddr - DATA_VADDR,
                cpu_flash + addr, filesz);
      /* No need to clear memory.  */
      if ((unsigned) (addr + memsz) > program_size)
        program_size = addr + memsz;
    }
}

// ----------------------------------------------------------------------------
//     parse command line arguments

static void
load_to_flash (const char *filename)
{
  FILE *fp;
  char buf[EI_NIDENT];
  size_t len;

  fp = fopen (filename, "rb");
  if (!fp)
    leave (EXIT_STATUS_ABORTED, "can't open program file");

  len = fread (buf, 1, sizeof (buf), fp);
  if (len == sizeof (buf)
      && buf[0] == 0x7f
      && buf[1] == 'E'
      && buf[2] == 'L'
      && buf[3] == 'F')
    {
      load_elf (fp);
    }
  else
    {
      rewind (fp);
      program_size = fread (cpu_flash, 1, MAX_FLASH_SIZE, fp);
    }
  fclose (fp);
}


static void
usage (void)
{
  const struct arch_desc *d;

  printf ("usage: avrtest [-d] [-m MAXCOUNT] [-mmcu=ARCH] program\n");
  printf ("Options:\n"
          "  -d           Initialize SRAM from .data (for ELF program)\n"
          "  -m MAXCOUNT  Execute at most MAXCOUNT instructions\n"
          "  -no-stdin    Disable stdin, i.e. reading from STDIN_PORT\n"
          "               will not wait for user input.\n"
          "  -no-stdout   Disable stdout, i.e. writing to STDOUT_PORT\n"
          "               will not print to stdout.\n"
          "  -mmcu=ARCH   Select instruction set for ARCH\n"
          "    ARCH is one of:");
  for (d = arch_descs; d->name; d++)
    printf ("%c%s", (d == arch_descs) ? ' ' : ',', d->name);
  printf ("\n");
  leave (EXIT_STATUS_ABORTED, "command line error");
}

static void
parse_args (int argc, char *argv[])
{
  int i;

  //max_instr_count = 1000000000;
  max_instr_count = 0;

  // parse command line arguments
  for (i = 1; i < argc; i++)
    {
      //  Use naive but very portable method to decode arguments.
      if (strcmp (argv[i], "-d") == 0)
        {
          flag_initialize_sram = 1;
        }
      else if (strcmp (argv[i], "-no-stdin") == 0)
        {
          flag_have_stdin = 0;
        }
      else if (strcmp (argv[i], "-no-stdout") == 0)
        {
          flag_have_stdout = 0;
        }
      else if (strcmp (argv[i], "-m") == 0)
        {
          i++;
          if (i >= argc)
            usage();
          max_instr_count = strtoul (argv[i], NULL, 0);
        }
      else if (strncmp (argv[i], "-mmcu=", 6) == 0)
        {
          const struct arch_desc *d;
          for (d = arch_descs; d->name; d++)
            if (strcmp (argv[i] + 6, d->name) == 0)
              {
                arch = *d;
                break;
              }
          if (d->name == NULL)
            usage();
        }
      else
        {
          if (program_name != NULL)
            usage();
          program_name = argv[i];
      }
    }

  // setup default values
  flash_addr_mask = (arch.pc_3bytes ? MAX_FLASH_SIZE : (1 << 17)) - 1;

  if (program_name == NULL)
    usage();
  load_to_flash (program_name);

  if (program_size & (~flash_addr_mask))
    {
      fprintf (stderr, "program is too large (size: %u, max: %u)\n",
               program_size, flash_addr_mask + 1);
      leave (EXIT_STATUS_ABORTED, "program too large");
    }
}


// ----------------------------------------------------------------------------
// flash pre-decoding functions
// * = may need extra cycles

const opcode_data opcode_func_array[] = {
  // dummy entry to guarantee that "zero" is an invalid function
  [avr_op_index_dummy] = {NULL, 0, 0, NULL},

  [avr_op_index_ADC] =       { avr_op_ADC,         1, 1, "ADC"     },
  [avr_op_index_ADD] =       { avr_op_ADD,         1, 1, "ADD"     },
  [avr_op_index_ADIW] =      { avr_op_ADIW,        1, 2, "ADIW"    },
  [avr_op_index_SBIW] =      { avr_op_SBIW,        1, 2, "SBIW"    },
  [avr_op_index_AND] =       { avr_op_AND,         1, 1, "AND"     },
  [avr_op_index_ANDI] =      { avr_op_ANDI,        1, 1, "ANDI"    },
  [avr_op_index_ASR] =       { avr_op_ASR,         1, 1, "ASR"     },
  [avr_op_index_BCLR] =      { avr_op_BCLR,        1, 1, "BCLR"    },
  [avr_op_index_BLD] =       { avr_op_BLD,         1, 1, "BLD"     },
  [avr_op_index_BRBC] =      { avr_op_BRBC,        1, 1, "BRBC"    }, // *
  [avr_op_index_BRBS] =      { avr_op_BRBS,        1, 1, "BRBS"    }, // *
  [avr_op_index_BSET] =      { avr_op_BSET,        1, 1, "BSET"    },
  [avr_op_index_BST] =       { avr_op_BST,         1, 1, "BST"     },
  [avr_op_index_CALL] =      { avr_op_CALL,        2, 4, "CALL"    }, // *
  [avr_op_index_CBI] =       { avr_op_CBI,         1, 2, "CBI"     },
  [avr_op_index_COM] =       { avr_op_COM,         1, 1, "COM"     },
  [avr_op_index_CP] =        { avr_op_CP,          1, 1, "CP"      },
  [avr_op_index_CPC] =       { avr_op_CPC,         1, 1, "CPC"     },
  [avr_op_index_CPI] =       { avr_op_CPI,         1, 1, "CPI"     },
  [avr_op_index_CPSE] =      { avr_op_CPSE,        1, 1, "CPSE"    }, // *
  [avr_op_index_DEC] =       { avr_op_DEC,         1, 1, "DEC"     },
  [avr_op_index_EICALL] =    { avr_op_EICALL,      1, 4, "EICALL"  },
  [avr_op_index_EIJMP] =     { avr_op_EIJMP,       1, 2, "EIJMP"   },
  [avr_op_index_ELPM] =      { avr_op_ELPM,        1, 3, "ELPM"    },
  [avr_op_index_ELPM_Z] =    { avr_op_ELPM_Z,      1, 3, "ELPM Z"  },
  [avr_op_index_ELPM_Z_incr]={ avr_op_ELPM_Z_incr, 1, 3, "ELPM Z+" },
  [avr_op_index_EOR] =       { avr_op_EOR,         1, 1, "EOR"     },
  [avr_op_index_ESPM] =      { avr_op_ESPM,        1, 1, "ESPM"    },
  [avr_op_index_FMUL] =      { avr_op_FMUL,        1, 2, "FMUL"    },
  [avr_op_index_FMULS] =     { avr_op_FMULS,       1, 2, "FMULS"   },
  [avr_op_index_FMULSU] =    { avr_op_FMULSU,      1, 2, "FMULSU"  },
  [avr_op_index_ICALL] =     { avr_op_ICALL,       1, 3, "ICALL"   }, // *
  [avr_op_index_IJMP] =      { avr_op_IJMP,        1, 2, "IJMP"    },
  [avr_op_index_ILLEGAL] =   { avr_op_ILLEGAL,     1, 1, "ILLEGAL" },
  [avr_op_index_IN] =        { avr_op_IN,          1, 1, "IN"      },
  [avr_op_index_INC] =       { avr_op_INC,         1, 1, "INC"     },
  [avr_op_index_JMP] =       { avr_op_JMP,         2, 3, "JMP"     },
  [avr_op_index_LDD_Y] =     { avr_op_LDD_Y,       1, 2, "LDD r,Y+q" },
  [avr_op_index_LDD_Z] =     { avr_op_LDD_Z,       1, 2, "LDD r,Z+q" },
  [avr_op_index_LDI] =       { avr_op_LDI,         1, 1, "LDI"     },
  [avr_op_index_LDS] =       { avr_op_LDS,         2, 2, "LDS"     },
  [avr_op_index_LD_X] =      { avr_op_LD_X,        1, 2, "LD r,X"  },
  [avr_op_index_LD_X_decr] = { avr_op_LD_X_decr,   1, 2, "LD r,-X" },
  [avr_op_index_LD_X_incr] = { avr_op_LD_X_incr,   1, 2, "LD r,X+" },
  [avr_op_index_LD_Y_decr] = { avr_op_LD_Y_decr,   1, 2, "LD r,-Y" },
  [avr_op_index_LD_Y_incr] = { avr_op_LD_Y_incr,   1, 2, "LD r,Y+" },
  [avr_op_index_LD_Z_decr] = { avr_op_LD_Z_decr,   1, 2, "LD r,-Z" },
  [avr_op_index_LD_Z_incr] = { avr_op_LD_Z_incr,   1, 2, "LD r,Z+" },
  [avr_op_index_LPM] =       { avr_op_LPM,         1, 3, "LPM"     },
  [avr_op_index_LPM_Z] =     { avr_op_LPM_Z,       1, 3, "LPM Z"   },
  [avr_op_index_LPM_Z_incr]= { avr_op_LPM_Z_incr,  1, 3, "LPM Z+"  },
  [avr_op_index_LSR] =       { avr_op_LSR,         1, 1, "LSR"     },
  [avr_op_index_MOV] =       { avr_op_MOV,         1, 1, "MOV"     },
  [avr_op_index_MOVW] =      { avr_op_MOVW,        1, 1, "MOVW"    },
  [avr_op_index_MUL] =       { avr_op_MUL,         1, 2, "MUL"     },
  [avr_op_index_MULS] =      { avr_op_MULS,        1, 2, "MULS"    },
  [avr_op_index_MULSU] =     { avr_op_MULSU,       1, 2, "MULSU"   },
  [avr_op_index_NEG] =       { avr_op_NEG,         1, 1, "NEG"     },
  [avr_op_index_NOP] =       { avr_op_NOP,         1, 1, "NOP"     },
  [avr_op_index_OR] =        { avr_op_OR,          1, 1, "OR"      },
  [avr_op_index_ORI] =       { avr_op_ORI,         1, 1, "ORI"     },
  [avr_op_index_OUT] =       { avr_op_OUT,         1, 1, "OUT"     },
  [avr_op_index_POP] =       { avr_op_POP,         1, 2, "POP"     },
#ifdef ISA_XMEGA
  [avr_op_index_XCH] =       { avr_op_XCH,         1, 1, "XCH"     },
  [avr_op_index_LAS] =       { avr_op_LAS,         1, 1, "LAS"     },
  [avr_op_index_LAC] =       { avr_op_LAC,         1, 1, "LAC"     },
  [avr_op_index_LAT] =       { avr_op_LAT,         1, 1, "LAT"     },
#endif // ISA_XMEGA
  [avr_op_index_PUSH] =      { avr_op_PUSH,        1, 2, "PUSH"    },
  [avr_op_index_RCALL] =     { avr_op_RCALL,       1, 3, "RCALL"   }, // *
  [avr_op_index_RET] =       { avr_op_RET,         1, 4, "RET"     }, // *
  [avr_op_index_RETI] =      { avr_op_RETI,        1, 4, "RETI"    }, // *
  [avr_op_index_RJMP] =      { avr_op_RJMP,        1, 2, "RJMP"    },
  [avr_op_index_ROR] =       { avr_op_ROR,         1, 1, "ROR"     },
  [avr_op_index_SBC] =       { avr_op_SBC,         1, 1, "SBC"     },
  [avr_op_index_SBCI] =      { avr_op_SBCI,        1, 1, "SBCI"    },
  [avr_op_index_SBI] =       { avr_op_SBI,         1, 2, "SBI"     },
  [avr_op_index_SBIC] =      { avr_op_SBIC,        1, 1, "SBIC"    }, // *
  [avr_op_index_SBIS] =      { avr_op_SBIS,        1, 1, "SBIS"    }, // *
  [avr_op_index_SBRC] =      { avr_op_SBRC,        1, 1, "SBRC"    }, // *
  [avr_op_index_SBRS] =      { avr_op_SBRS,        1, 1, "SBRS"    }, // *
  [avr_op_index_SLEEP] =     { avr_op_SLEEP,       1, 1, "SLEEP"   },
  [avr_op_index_SPM] =       { avr_op_SPM,         1, 1, "SPM"     },
  [avr_op_index_STD_Y] =     { avr_op_STD_Y,       1, 2, "STD Y+q,r" },
  [avr_op_index_STD_Z] =     { avr_op_STD_Z,       1, 2, "STD Z+q,r" },
  [avr_op_index_STS] =       { avr_op_STS,         2, 2, "STS"     },
  [avr_op_index_ST_X] =      { avr_op_ST_X,        1, 2, "ST X,r"  },
  [avr_op_index_ST_X_decr] = { avr_op_ST_X_decr,   1, 2, "ST -X,r" },
  [avr_op_index_ST_X_incr] = { avr_op_ST_X_incr,   1, 2, "ST X+,r" },
  [avr_op_index_ST_Y_decr] = { avr_op_ST_Y_decr,   1, 2, "ST -Y,r" },
  [avr_op_index_ST_Y_incr] = { avr_op_ST_Y_incr,   1, 2, "ST Y+,r" },
  [avr_op_index_ST_Z_decr] = { avr_op_ST_Z_decr,   1, 2, "ST -Z,r" },
  [avr_op_index_ST_Z_incr] = { avr_op_ST_Z_incr,   1, 2, "ST Z+,r" },
  [avr_op_index_SUB] =       { avr_op_SUB,         1, 1, "SUB"     },
  [avr_op_index_SUBI] =      { avr_op_SUBI,        1, 1, "SUBI"    },
  [avr_op_index_SWAP] =      { avr_op_SWAP,        1, 1, "SWAP"    },
  [avr_op_index_WDR] =       { avr_op_WDR,         1, 1, "WDR"     },
};

// Opcodes with no arguments except NOP       1001 010~ ~~~~ ~~~~ 
static const byte avr_op_16_index[1 + 0x1ff] = {
  [0x9519 ^ 0x9400] = avr_op_index_EICALL, /* 1001 0101 0001 1001 | EICALL */
  [0x9419 ^ 0x9400] = avr_op_index_EIJMP,  /* 1001 0100 0001 1001 | EIJMP */
  [0x95D8 ^ 0x9400] = avr_op_index_ELPM,   /* 1001 0101 1101 1000 | ELPM */
  [0x95F8 ^ 0x9400] = avr_op_index_ESPM,   /* 1001 0101 1111 1000 | ESPM */
  [0x9509 ^ 0x9400] = avr_op_index_ICALL,  /* 1001 0101 0000 1001 | ICALL */
  [0x9409 ^ 0x9400] = avr_op_index_IJMP,   /* 1001 0100 0000 1001 | IJMP */
  [0x95C8 ^ 0x9400] = avr_op_index_LPM,    /* 1001 0101 1100 1000 | LPM */
  [0x9508 ^ 0x9400] = avr_op_index_RET,    /* 1001 0101 0000 1000 | RET */
  [0x9518 ^ 0x9400] = avr_op_index_RETI,   /* 1001 0101 0001 1000 | RETI */
  [0x9588 ^ 0x9400] = avr_op_index_SLEEP,  /* 1001 0101 1000 1000 | SLEEP */
  [0x95E8 ^ 0x9400] = avr_op_index_SPM,    /* 1001 0101 1110 1000 | SPM */
  [0x95A8 ^ 0x9400] = avr_op_index_WDR,    /* 1001 0101 1010 1000 | WDR */
};

// Opcodes unique with upper 6 bits           ~~~~ ~~rd dddd rrrr
static const byte avr_op_6_index[1 << 6] = {
  [0x1C00 >> 10] = avr_op_index_ADC,       /* 0001 11rd dddd rrrr | ADC, ROL */
  [0x0C00 >> 10] = avr_op_index_ADD,       /* 0000 11rd dddd rrrr | ADD, LSL */
  [0x2000 >> 10] = avr_op_index_AND,       /* 0010 00rd dddd rrrr | AND, TST */
  [0x1400 >> 10] = avr_op_index_CP,        /* 0001 01rd dddd rrrr | CP */
  [0x0400 >> 10] = avr_op_index_CPC,       /* 0000 01rd dddd rrrr | CPC */
  [0x1000 >> 10] = avr_op_index_CPSE,      /* 0001 00rd dddd rrrr | CPSE */
  [0x2400 >> 10] = avr_op_index_EOR,       /* 0010 01rd dddd rrrr | EOR, CLR */
  [0x2C00 >> 10] = avr_op_index_MOV,       /* 0010 11rd dddd rrrr | MOV */
  [0x9C00 >> 10] = avr_op_index_MUL,       /* 1001 11rd dddd rrrr | MUL */
  [0x2800 >> 10] = avr_op_index_OR,        /* 0010 10rd dddd rrrr | OR */
  [0x0800 >> 10] = avr_op_index_SBC,       /* 0000 10rd dddd rrrr | SBC */
  [0x1800 >> 10] = avr_op_index_SUB,       /* 0001 10rd dddd rrrr | SUB */
};

// Unique with upper 7, lower 4 bits              1001 0~~d dddd ~~~~
static const byte avr_op_74_index[1 + 0x7ff] = {
  [0x9405 ^ 0x9000] = avr_op_index_ASR,        /* 1001 010d dddd 0101 | ASR */
  [0x9400 ^ 0x9000] = avr_op_index_COM,        /* 1001 010d dddd 0000 | COM */
  [0x940A ^ 0x9000] = avr_op_index_DEC,        /* 1001 010d dddd 1010 | DEC */
  [0x9006 ^ 0x9000] = avr_op_index_ELPM_Z,     /* 1001 000d dddd 0110 | ELPM */
  [0x9007 ^ 0x9000] = avr_op_index_ELPM_Z_incr,/* 1001 000d dddd 0111 | ELPM */
  [0x9403 ^ 0x9000] = avr_op_index_INC,        /* 1001 010d dddd 0011 | INC */
  [0x9000 ^ 0x9000] = avr_op_index_LDS,        /* 1001 000d dddd 0000 | LDS */
  [0x900C ^ 0x9000] = avr_op_index_LD_X,       /* 1001 000d dddd 1100 | LD */
  [0x900E ^ 0x9000] = avr_op_index_LD_X_decr,  /* 1001 000d dddd 1110 | LD */
  [0x900D ^ 0x9000] = avr_op_index_LD_X_incr,  /* 1001 000d dddd 1101 | LD */
  [0x900A ^ 0x9000] = avr_op_index_LD_Y_decr,  /* 1001 000d dddd 1010 | LD */
  [0x9009 ^ 0x9000] = avr_op_index_LD_Y_incr,  /* 1001 000d dddd 1001 | LD */
  [0x9002 ^ 0x9000] = avr_op_index_LD_Z_decr,  /* 1001 000d dddd 0010 | LD */
  [0x9001 ^ 0x9000] = avr_op_index_LD_Z_incr,  /* 1001 000d dddd 0001 | LD */
  [0x9004 ^ 0x9000] = avr_op_index_LPM_Z,      /* 1001 000d dddd 0100 | LPM */
  [0x9005 ^ 0x9000] = avr_op_index_LPM_Z_incr, /* 1001 000d dddd 0101 | LPM */
  [0x9406 ^ 0x9000] = avr_op_index_LSR,        /* 1001 010d dddd 0110 | LSR */
  [0x9401 ^ 0x9000] = avr_op_index_NEG,        /* 1001 010d dddd 0001 | NEG */
  [0x900F ^ 0x9000] = avr_op_index_POP,        /* 1001 000d dddd 1111 | POP */
#ifdef ISA_XMEGA
  [0x9204 ^ 0x9000] = avr_op_index_XCH,        /* 1001 001d dddd 0100 | XCH */
  [0x9205 ^ 0x9000] = avr_op_index_LAS,        /* 1001 001d dddd 0101 | LAS */
  [0x9206 ^ 0x9000] = avr_op_index_LAC,        /* 1001 001d dddd 0110 | LAC */
  [0x9207 ^ 0x9000] = avr_op_index_LAT,        /* 1001 001d dddd 0111 | LAT */
#endif // ISA_XMEGA
  [0x920F ^ 0x9000] = avr_op_index_PUSH,       /* 1001 001d dddd 1111 | PUSH */
  [0x9407 ^ 0x9000] = avr_op_index_ROR,        /* 1001 010d dddd 0111 | ROR */
  [0x9200 ^ 0x9000] = avr_op_index_STS,        /* 1001 001d dddd 0000 | STS */
  [0x920C ^ 0x9000] = avr_op_index_ST_X,       /* 1001 001d dddd 1100 | ST */
  [0x920E ^ 0x9000] = avr_op_index_ST_X_decr,  /* 1001 001d dddd 1110 | ST */
  [0x920D ^ 0x9000] = avr_op_index_ST_X_incr,  /* 1001 001d dddd 1101 | ST */
  [0x920A ^ 0x9000] = avr_op_index_ST_Y_decr,  /* 1001 001d dddd 1010 | ST */
  [0x9209 ^ 0x9000] = avr_op_index_ST_Y_incr,  /* 1001 001d dddd 1001 | ST */
  [0x9202 ^ 0x9000] = avr_op_index_ST_Z_decr,  /* 1001 001d dddd 0010 | ST */
  [0x9201 ^ 0x9000] = avr_op_index_ST_Z_incr,  /* 1001 001d dddd 0001 | ST */
  [0x9402 ^ 0x9000] = avr_op_index_SWAP,       /* 1001 010d dddd 0010 | SWAP */
};


static int
decode_opcode (decoded_op *op, unsigned opcode1, unsigned opcode2)
{
  unsigned decode;
  byte index;

  // opcodes with no operands

  if (0x0000 == opcode1)
    return avr_op_index_NOP;                     /* 0000 0000 0000 0000 | NOP */

  // All other opcodes with no operands are:        1001 010x xxxx xxxx
  if ((opcode1 ^ 0x9400) <= 0x1ff
      && ((index = avr_op_16_index[opcode1 ^ 0x9400])))
    return index;

  // opcodes with two 5-bit register operands       xxxx xxrd dddd rrrr

  if ((index = avr_op_6_index[opcode1 >> 10]))
    {
      op->oper2 = (opcode1 & 0x0F) | ((opcode1 >> 5) & 0x0010);
      op->oper1 = (opcode1 >> 4) & 0x1F;
      return index;
    }

  // opcodes with a single register operand         1001 0xxd dddd xxxx

  decode = opcode1 & ~(mask_Rd_5);
  if ((decode ^ 0x9000) <= 0x7ff
      && ((index = avr_op_74_index[decode ^ 0x9000])))
    {
      // oper2 only used with LDS, STS
      op->oper2 = opcode2;
      op->oper1 = (opcode1 >> 4) & 0x1F;
      return index;
    }

  /* opcodes with a register (Rd) and a constant data (K) as operands */
  op->oper1 = ((opcode1 >> 4) & 0xF) | 0x10;
  op->oper2 = (opcode1 & 0x0F) | ((opcode1 >> 4) & 0x00F0);

  decode = opcode1 & ~(mask_Rd_4 | mask_K_8);
  switch (decode) {
  case 0x7000: return avr_op_index_ANDI; /* 0111 KKKK dddd KKKK | CBR or ANDI */
  case 0x3000: return avr_op_index_CPI;  /* 0011 KKKK dddd KKKK | CPI */
  case 0xE000: return avr_op_index_LDI;  /* 1110 KKKK dddd KKKK | LDI or SER */
  case 0x6000: return avr_op_index_ORI;  /* 0110 KKKK dddd KKKK | SBR or ORI */
  case 0x4000: return avr_op_index_SBCI; /* 0100 KKKK dddd KKKK | SBCI */
  case 0x5000: return avr_op_index_SUBI; /* 0101 KKKK dddd KKKK | SUBI */
  }

  /* opcodes with a register (Rd) and a register bit number (b) as operands */
  op->oper1 = (opcode1 >> 4) & 0x1F;
  op->oper2 = 1 << (opcode1 & 0x0007);
  decode = opcode1 & ~(mask_Rd_5 | mask_reg_bit);
  switch (decode) {
  case 0xF800: return avr_op_index_BLD;   /* 1111 100d dddd 0bbb | BLD */
  case 0xFA00: return avr_op_index_BST;   /* 1111 101d dddd 0bbb | BST */
  case 0xFC00: return avr_op_index_SBRC;  /* 1111 110d dddd 0bbb | SBRC */
  case 0xFE00: return avr_op_index_SBRS;  /* 1111 111d dddd 0bbb | SBRS */
  }

  /* opcodes with a relative 7-bit address (k) and a register bit number (b)
     as operands */
  op->oper1 = (opcode1 >> 3) & 0x7F;
  decode = opcode1 & ~(mask_k_7 | mask_reg_bit);
  switch (decode) {
  case 0xF400: return avr_op_index_BRBC;   /* 1111 01kk kkkk kbbb | BRBC */
  case 0xF000: return avr_op_index_BRBS;   /* 1111 00kk kkkk kbbb | BRBS */
  }

  /* opcodes with a 6-bit address displacement (q) and a register (Rd)
     as operands */

  if ((opcode1 & 0xd000) == 0x8000)
    {
      op->oper1 = (opcode1 >> 4) & 0x1F;
      op->oper2 = ((opcode1 & 0x7)
                   | ((opcode1 >> 7) & 0x18)
                   | ((opcode1 >> 8) & 0x20));
      decode = opcode1 & ~(mask_Rd_5 | mask_q_displ);
      switch (decode) {
      case 0x8008: return avr_op_index_LDD_Y;   /* 10q0 qq0d dddd 1qqq | LDD */
      case 0x8000: return avr_op_index_LDD_Z;   /* 10q0 qq0d dddd 0qqq | LDD */
      case 0x8208: return avr_op_index_STD_Y;   /* 10q0 qq1d dddd 1qqq | STD */
      case 0x8200: return avr_op_index_STD_Z;   /* 10q0 qq1d dddd 0qqq | STD */
      }
    }

  /* opcodes with a absolute 22-bit address (k) operand */
  op->oper1 = (opcode1 & 1) | ((opcode1 >> 3) & 0x3E);
  op->oper2 = opcode2;
  decode = opcode1 & ~(mask_k_22);
  switch (decode) {
  case 0x940E: return avr_op_index_CALL;        /* 1001 010k kkkk 111k | CALL */
  case 0x940C: return avr_op_index_JMP;         /* 1001 010k kkkk 110k | JMP */
  }

  /* opcode1 with a sreg bit select (s) operand */
  op->oper1 = 1 << ((opcode1 >> 4) & 0x07);
  decode = opcode1 & ~(mask_sreg_bit);
  switch (decode) {
    /* BCLR takes care of CL{C,Z,N,V,S,H,T,I} */
    /* BSET takes care of SE{C,Z,N,V,S,H,T,I} */
  case 0x9488: return avr_op_index_BCLR;        /* 1001 0100 1sss 1000 | BCLR */
  case 0x9408: return avr_op_index_BSET;        /* 1001 0100 0sss 1000 | BSET */
  }

  /* opcodes with a 6-bit constant (K) and a register (Rd) as operands */
  op->oper1 = ((opcode1 >> 3) & 0x06) + 24;
  op->oper2 = (opcode1 & 0xF) | ((opcode1 >> 2) & 0x30);
  decode = opcode1 & ~(mask_K_6 | mask_Rd_2);
  switch (decode) {
  case 0x9600: return avr_op_index_ADIW;        /* 1001 0110 KKdd KKKK | ADIW */
  case 0x9700: return avr_op_index_SBIW;        /* 1001 0111 KKdd KKKK | SBIW */
  }

  /* opcodes with a 5-bit IO Addr (A) and register bit number (b) as operands */
  op->oper1 = ((opcode1 >> 3) & 0x1F) + IOBASE;
  op->oper2 = 1 << (opcode1 & 0x0007);
  decode = opcode1 & ~(mask_A_5 | mask_reg_bit);
  switch (decode) {
  case 0x9800: return avr_op_index_CBI;         /* 1001 1000 AAAA Abbb | CBI */
  case 0x9A00: return avr_op_index_SBI;         /* 1001 1010 AAAA Abbb | SBI */
  case 0x9900: return avr_op_index_SBIC;        /* 1001 1001 AAAA Abbb | SBIC */
  case 0x9B00: return avr_op_index_SBIS;        /* 1001 1011 AAAA Abbb | SBIS */
  }

  /* opcodes with a 6-bit IO Addr (A) and register (Rd) as operands */
  op->oper1 = ((opcode1 >> 4) & 0x1F);
  op->oper2 = ((opcode1 & 0x0F) | ((opcode1 >> 5) & 0x30)) + IOBASE;
  decode = opcode1 & ~(mask_A_6 | mask_Rd_5);
  switch (decode) {
  case 0xB000: return avr_op_index_IN;          /* 1011 0AAd dddd AAAA | IN */
  case 0xB800: return avr_op_index_OUT;         /* 1011 1AAd dddd AAAA | OUT */
  }

  /* opcodes with a relative 12-bit address (k) operand */
  op->oper2 = opcode1 & 0xFFF;
  decode = opcode1 & ~(mask_k_12);
  switch (decode) {
  case 0xD000: return avr_op_index_RCALL;   /* 1101 kkkk kkkk kkkk | RCALL */
  case 0xC000: return avr_op_index_RJMP;    /* 1100 kkkk kkkk kkkk | RJMP */
  }

  /* opcodes with two 4-bit register (Rd and Rr) operands */
  decode = opcode1 & ~(mask_Rd_4 | mask_Rr_4);
  switch (decode) {
  case 0x0100:
    op->oper1 = ((opcode1 >> 4) & 0x0F) << 1;
    op->oper2 = (opcode1 & 0x0F) << 1;
    return avr_op_index_MOVW;        /* 0000 0001 dddd rrrr | MOVW */
  case 0x0200:
    op->oper1 = ((opcode1 >> 4) & 0x0F) | 0x10;
    op->oper2 = (opcode1 & 0x0F) | 0x10;
    return avr_op_index_MULS;        /* 0000 0010 dddd rrrr | MULS */
  }

  /* opcodes with two 3-bit register (Rd and Rr) operands */
  op->oper1 = ((opcode1 >> 4) & 0x07) | 0x10;
  op->oper2 = (opcode1 & 0x07) | 0x10;
  decode = opcode1 & ~(mask_Rd_3 | mask_Rr_3);
  switch (decode) {
  case 0x0300: return avr_op_index_MULSU;   /* 0000 0011 0ddd 0rrr | MULSU */
  case 0x0308: return avr_op_index_FMUL;    /* 0000 0011 0ddd 1rrr | FMUL */
  case 0x0380: return avr_op_index_FMULS;   /* 0000 0011 1ddd 0rrr | FMULS */
  case 0x0388: return avr_op_index_FMULSU;  /* 0000 0011 1ddd 1rrr | FMULSU */
  }

  op->oper2 = opcode1;
  return avr_op_index_ILLEGAL;
}

static void
decode_flash (void)
{
  word opcode1, opcode2;
  unsigned int i = 0;

  opcode1 = cpu_flash[i] | (cpu_flash[i + 1] << 8);
  
  for (i = 0; i < program_size; i += 2)
    {
      opcode2 = cpu_flash[i + 2] | (cpu_flash[i + 3] << 8);
      decoded_flash[i / 2].data_index
        = decode_opcode (&decoded_flash[i / 2], opcode1, opcode2);
      opcode1 = opcode2;
    }
}

// ----------------------------------------------------------------------------
//     main execution loop

static INLINE void
do_step (void)
{
  decoded_op dop;
  const opcode_data *data;
    
  // fetch decoded instruction
  dop = decoded_flash[cpu_PC];
  if (!dop.data_index)
    leave (EXIT_STATUS_ABORTED, "program counter out of program space");

  // execute instruction
  data = &opcode_func_array[dop.data_index];
  log_add_instr (data->hreadable);
  cpu_PC += data->size;
  add_program_cycles (data->cycles);
  data->func (dop.oper1, dop.oper2);
  log_dump_line();
}

static void
execute (void)
{
  dword count;

  program_cycles = 0;
  cpu_PC = 0;

  if (!max_instr_count)
    {
      for (;;)
        do_step();
    }
  else
    {
      for (count = 0; count < max_instr_count; count++)
        {
          do_step();
        }
      leave (EXIT_STATUS_TIMEOUT, "instruction count limit reached");
    }
}


// main: as simple as it gets
int
main (int argc, char *argv[])
{
  arch = arch_descs[0];
  parse_args (argc, argv);

  decode_flash();
  execute();

  return EXIT_SUCCESS;
}
