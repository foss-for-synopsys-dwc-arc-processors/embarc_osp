#ifndef REGION_MLI_ROM
#define REGION_MLI_ROM		REGION_ICCM
#endif

#ifndef REGION_MLI_DATA
#define REGION_MLI_DATA		REGION_DCCM
#endif

#ifndef REGION_MLI_BSS
#define REGION_MLI_BSS		REGION_DCCM
#endif

#ifndef REGION_MLI_ZDATA
#define REGION_MLI_ZDATA	REGION_DCCM
#endif

#ifndef REGION_MLI_MODEL_P2
#define REGION_MLI_MODEL_P2	REGION_DCCM
#endif

#ifndef REGION_MLI_MODEL
#define REGION_MLI_MODEL	REGION_XCCM
#endif

#ifndef REGION_MLI_XDATA
#define REGION_MLI_XDATA	REGION_XCCM
#endif

#ifndef REGION_MLI_YDATA
#define REGION_MLI_YDATA	REGION_YCCM
#endif

	/DISCARD/ : {
		/*.arcextmap contains debug information not used by gnu */
		*libmli*.a:*.o(.arcextmap*)
	}
	.mli_rom : ALIGN(8) {
		_f_mli_rom = .;
  		*libmli*.a:*.o(.mli_lib*)
		*libmli*.a:*.o(.rodata .rodata.*)
		_e_mli_rom = .;
	} > REGION_MLI_ROM AT > REGION_ROM

	_load_addr_mli_rom = LOADADDR(.mli_rom);

	.mli_data : ALIGN(8) {
		_f_mli_data = .;
		*libmli*.a:*.o(.rodata_in_data .rodata_in_data.*)
		*libmli*.a:*.o(.data .data.*)
		_e_mli_data = .;
	} > REGION_MLI_DATA AT > REGION_ROM

	_load_addr_mli_data = LOADADDR(.mli_data);

	.Zdata : ALIGN(8) {
		_f_Zdata = .;
		*(.Zdata*)
		_e_Zdata = .;
	} > REGION_MLI_ZDATA AT > REGION_ROM

	_load_addr_Zdata = LOADADDR(.Zdata);

	.mli_model_p2 : ALIGN(8) {
		_f_mli_model_p2 = .;
		*(.mli_model_p2 .mli_model_p2.*)
		_e_mli_model_p2 = .;
	} > REGION_MLI_MODEL_P2 AT > REGION_ROM

	_load_addr_mli_model_p2 = LOADADDR(.mli_model_p2);

	.mli_model : ALIGN(8) {
		_f_mli_model = .;
		*(.mli_model .mli_model.*)
		_e_mli_model = .;
	} > REGION_MLI_MODEL AT > REGION_ROM

	_load_addr_mli_model = LOADADDR(.mli_model);

	.mli_bss (NOLOAD) : ALIGN(8) {
		_f_mli_bss = .;
		*libmli*.a:*.o(.bss .bss.*)
		_e_mli_bss = .;
	} > REGION_MLI_BSS

	.Xdata (NOLOAD) : ALIGN(8) {
		_f_Xdata = .;
		*(.Xdata*)
		_e_Xdata = .;
	} > REGION_MLI_XDATA

	.Ydata (NOLOAD) : ALIGN(8) {
		_f_Ydata = .;
		*(.Ydata*)
		_e_Ydata = .;
	} > REGION_MLI_YDATA