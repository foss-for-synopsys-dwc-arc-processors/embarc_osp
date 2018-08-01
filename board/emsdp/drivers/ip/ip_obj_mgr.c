
#include "embARC_toolchain.h"
#include "emsdp/emsdp.h"


DEV_SPI_PTR spi_get_dev(int32_t spi_id)
{
	switch(spi_id){
#if (USE_DW_SPI_0)
		case DW_SPI_0_ID:
#endif
#if (USE_DW_SPI_1)
		case DW_SPI_1_ID:
#endif
#if (USE_DW_SPI_2)
		case DW_SPI_2_ID:
#endif
#if (USE_DW_SPI_3)
		case DW_SPI_3_ID:
#endif
			return dw_spi_get_dev(spi_id);
			break;

#if (USE_DFSS_SPI_0)
		case DFSS_SPI_0_ID:
#endif
#if (USE_DFSS_SPI_1)
		case DFSS_SPI_1_ID:
#endif
#if (USE_DFSS_SPI_2)
		case DFSS_SPI_2_ID:
#endif
#if (USE_DFSS_SPI_3)
		case DFSS_SPI_3_ID:
#endif
			return dfss_spi_get_dev(spi_id);
			break;
		default:
			break;
	}
	return NULL;
}
