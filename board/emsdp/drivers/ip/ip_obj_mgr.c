
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

DEV_GPIO_PTR gpio_get_dev(int32_t gpio_id)
{

	switch(gpio_id){
#if (USE_DW_GPIO_PORT_A)
		case DW_GPIO_PORT_A:
#endif
#if (USE_DW_GPIO_PORT_B)
		case DW_GPIO_PORT_B:
#endif
#if (USE_DW_GPIO_PORT_C)
		case DW_GPIO_PORT_C:
#endif
#if (USE_DW_GPIO_PORT_D)
		case DW_GPIO_PORT_D:
#endif
			return dw_gpio_get_dev(gpio_id);
			break;

#if (USE_DFSS_GPIO_0)
		case DFSS_GPIO_0_ID:
#endif
#if (USE_DFSS_GPIO_1)
		case DFSS_GPIO_1_ID:
#endif
#if (USE_DFSS_GPIO_2)
		case DFSS_GPIO_2_ID:
#endif
#if (USE_DFSS_GPIO_3)
		case DFSS_GPIO_3_ID:
#endif
#if (USE_DFSS_GPIO_8B0)
		case DFSS_GPIO_8B0_ID:
#endif
#if (USE_DFSS_GPIO_8B1)
		case DFSS_GPIO_8B1_ID:
#endif
#if (USE_DFSS_GPIO_8B2)
		case DFSS_GPIO_8B2_ID:
#endif
#if (USE_DFSS_GPIO_8B3)
		case DFSS_GPIO_8B3_ID:
#endif
#if (USE_DFSS_GPIO_4B0)
		case DFSS_GPIO_4B0_ID:
#endif
#if (USE_DFSS_GPIO_4B1)
		case DFSS_GPIO_4B1_ID:
#endif
#if (USE_DFSS_GPIO_4B2)
		case DFSS_GPIO_4B2_ID:
#endif
#if (USE_DFSS_GPIO_4B3)
		case DFSS_GPIO_4B3_ID:
#endif
			return dfss_gpio_get_dev(gpio_id);
			break;
		default:
			break;
	}
	return NULL;
}