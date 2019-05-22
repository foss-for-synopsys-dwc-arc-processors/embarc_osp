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


	GROUP : {
		.mli_rom ALIGN(8): {
			_f_mli_rom = .;
			"*libmli*.a*"(TYPE text)
			"*libmli*.a*"(.text*)
			"*libmli*.a*"(TYPE lit)
			_e_mli_rom = .;
		}
	} > REGION_MLI_ROM AT > REGION_ROM

	_load_addr_mli_rom = LOADADDR(.mli_rom);

	GROUP : {
		.mli_data ALIGN(8): {
			_f_mli_data = .;
			"*libmli*.a*"(TYPE data)
			_e_mli_data = .;
		}
	} > REGION_MLI_DATA AT > REGION_ROM

	_load_addr_mli_data = LOADADDR(.mli_data);

	GROUP : {
		.Zdata ALIGN(8): {
			_f_Zdata = .;
			*(.Zdata*)
			_e_Zdata = .;
		}
	} > REGION_MLI_ZDATA AT > REGION_ROM

	_load_addr_Zdata = LOADADDR(.Zdata);

	GROUP : {
		.mli_model_p2 ALIGN(8): {
			_f_mli_model_p2 = .;
			*(.mli_model_p2 .mli_model_p2.*)
			_e_mli_model_p2 = .;
		}
	} > REGION_MLI_MODEL_P2 AT > REGION_ROM

	_load_addr_mli_model_p2 = LOADADDR(.mli_model_p2);

	GROUP : {
		.mli_model ALIGN(8): {
			_f_mli_model = .;
			*(.mli_model .mli_model.*)
			_e_mli_model = .;
		}
	} > REGION_MLI_MODEL AT > REGION_ROM

	_load_addr_mli_model = LOADADDR(.mli_model);

	GROUP (NOLOAD): {
		.mli_bss ALIGN(8): {
			_f_mli_bss = .;
			"*libmli*.a*"(TYPE bss)
			_e_mli_bss = .;
		}
	} > REGION_MLI_BSS

	GROUP (NOLOAD) : {
		.Xdata ALIGN(8): {
			_f_Xdata = .;
			*(.Xdata*)
			_e_Xdata = .;
		}
	} > REGION_MLI_XDATA

	GROUP (NOLOAD) : {
		.Ydata ALIGN(8): {
			_f_Ydata = .;
			*(.Ydata*)
			_e_Ydata = .;
		}
	} > REGION_MLI_YDATA