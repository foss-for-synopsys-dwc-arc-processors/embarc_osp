/*======================================================================*/
/* Proxy kernel linker script                                           */
/*======================================================================*/
/* This is the linker script used when building the proxy kernel. */

/*----------------------------------------------------------------------*/
/* Setup                                                                */
/*----------------------------------------------------------------------*/

/* The OUTPUT_ARCH command specifies the machine architecture where the
   argument is one of the names used in the BFD library. More
   specifically one of the entires in bfd/cpu-mips.c */

OUTPUT_ARCH( "riscv" )
ENTRY(_start)

/*----------------------------------------------------------------------*/
/* Sections                                                             */
/*----------------------------------------------------------------------*/

MEMORY {
  ICCM0 (rwx) : ORIGIN = 0x0, LENGTH = 512K
  DCCM (rwx) : ORIGIN = 0x200000, LENGTH = 512K
  MEM (rwx) : ORIGIN = 0x10000000, LENGTH = 3072M
}

STACK_SIZE = 1024;

CL_SIZE = 32;

SECTIONS
{

  /* text: test code section */
  .text.init : { *(.text.init) } > MEM

  /* Do we need this? . = ALIGN(0x1000); */
  .tohost : { *(.tohost) } > MEM

   .preinit_array     :
  {
    PROVIDE_HIDDEN (__preinit_array_start = .);
    KEEP (*(.preinit_array*))
    PROVIDE_HIDDEN (__preinit_array_end = .);
  } >MEM
  .init_array :
  {
    PROVIDE_HIDDEN (__init_array_start = .);
    KEEP (*(SORT(.init_array.*)))
    KEEP (*(.init_array*))
    PROVIDE_HIDDEN (__init_array_end = .);
  } >ICCM0

  /* Do we need this? . = ALIGN(0x1000); */
  .text : { *(.text) } > MEM

  /* data segment */
  .data : {
   __DATA_BEGIN__ = .;
   *(.data)
   } > MEM

  .sdata : {
    __SDATA_BEGIN__ = .;
    *(.srodata.cst16) *(.srodata.cst8) *(.srodata.cst4) *(.srodata.cst2) *(.srodata*)
    *(.sdata .sdata.* .gnu.linkonce.s.*)
  } > MEM
  _edata = .; PROVIDE(edata = .);

   .rodata ALIGN(CL_SIZE) : {
   *(.rodata) *(.rodata.*) *(.gnu.linkonce.r.*)
   . = ALIGN(CL_SIZE);
   LONG(0x13);
   . = ALIGN(CL_SIZE);
  } > MEM

  /* bss segment */
  __bss_start = .;
  .sbss : {
    *(.sbss .sbss.* .gnu.linkonce.sb.*)
    *(.scommon)
  } > MEM

  .bss : { *(.bss) } > MEM
  __BSS_END__ = .;
    __global_pointer$ = MIN(__SDATA_BEGIN__ + 0x800,
                            MAX(__DATA_BEGIN__ + 0x800, __BSS_END__ - 0x800));

  /* thread-local data segment */
  .tdata :
  {
    _tdata_begin = .;
    *(.tdata)
    _tdata_end = .;
  } > MEM
  .tbss :
  {
    *(.tbss)
    _tbss_end = .;
  } > MEM

  /* End of uninitalized data segement */
  _end = .; PROVIDE (end = .);

  .stack ORIGIN(MEM) + LENGTH(MEM) - STACK_SIZE : {
    PROVIDE(__STACK_START__ = .);
    . += STACK_SIZE;
    PROVIDE(__C_STACK_TOP__ = .);
    PROVIDE(__STACK_END__ = .);
    PROVIDE(__stack_pointer$ = .);
  } > MEM
}
