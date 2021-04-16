/* ------------------------------------------
 * Copyright (c) 2017, Synopsys, Inc. All rights reserved.

 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:

 * 1) Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.

 * 2) Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.

 * 3) Neither the name of the Synopsys, Inc., nor the names of its contributors may
 * be used to endorse or promote products derived from this software without
 * specific prior written permission.

 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
--------------------------------------------- */

/**
 * \file
 * \ingroup ARC_HAL_MISC_UDMA
 * \brief Configurations of uDMA controller
 */

/**
 * \addtogroup ARC_HAL_MISC_UDMA
 * @{
 */
#ifndef _ARC_UDMA_CONFIG_H_
#define _ARC_UDMA_CONFIG_H_

#include "arc_feature_config.h"
#include "arc_exception.h"

/*------ Available uDMA driver configurations */
#ifndef DMA_MULTI_IRQ
/** Multiple interrupts for DMA */
#define DMA_MULTI_IRQ			0
#endif

#ifndef CORE_DMAC_CHANNELS
/** Total channel number of DMA */
#define	CORE_DMAC_CHANNELS		2
#endif

#ifndef CORE_DMAC_REGISTERS
/** Total register based channel number of DMA */
#define	CORE_DMAC_REGISTERS		0
#endif

#ifndef DMA_IRQ_NUM_START
/** DMA IRQ start vector */
#define DMA_IRQ_NUM_START		20
#endif

#ifndef DMA_IRQ_PRIO
/** DMA IRQ priority */
#define DMA_IRQ_PRIO			(INT_PRI_MIN)
#endif

#ifndef CORE_DMAC_INTERNAL_VERSION
/** Version of the DMA controller */
#define CORE_DMAC_INTERNAL_VERSION		2
#endif

/*------ uDMA driver configurations end */

#endif /* _ARC_UDMA_CONFIG_H_ */

