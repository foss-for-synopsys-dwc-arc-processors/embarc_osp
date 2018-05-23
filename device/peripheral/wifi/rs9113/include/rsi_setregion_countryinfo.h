/**
 * @file     setregion_countryinfo.h
 * @version  3.6
 * @date     2013-May-16
 *
 *
 * @section License
 * This program should be used on your own responsibility.
 * Redpine Signals assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief HEADER: API specific Defines
 *
 * @section Description
 * This file contains the function prototypes of the APIs defined related to set region supported API
 * and defines used in API's.
 * Copyright(C) Redpine Signals 2013
 * All rights reserved by Redpine Signals.
 */

#ifndef _SETREGION_COUNTRYINFO_H_
#define _SETREGION_COUNTRYINFO_H_

#include <rsi_driver.h>

#define       REGION_US          1
#define       REGION_EU          2
#define       REGION_JP          3
#define       REGION_INDIA       4
#define       REGION_SINGAPORE   5
#define       REGION_KOREA       6


void extract_setregionap_country_info(rsi_req_set_region_ap_t *);
#endif
