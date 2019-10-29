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
#include "arc_mpu.h"

#define CALC_REGION_END_ADDR(start, size) \
		(start + size - (1 << ARC_FEATURE_MPU_ALIGNMENT_BITS))


void arc_mpu_enable(void)
{
#if ARC_FEATURE_MPU_VERSION == 2
	_arc_aux_write(AUX_MPU_EN,
		_arc_aux_read(AUX_MPU_EN) | AUX_MPU_EN_ENABLE);
#elif ARC_FEATURE_MPU_VERSION == 4
	arc_mpu_default(0);
#endif
}

void arc_mpu_disable(void)
{
#if ARC_FEATURE_MPU_VERSION == 2
	_arc_aux_write(AUX_MPU_EN,
		_arc_aux_read(AUX_MPU_EN) | AUX_MPU_EN_DISABLE);
#elif ARC_FEATURE_MPU_VERSION == 4
	arc_mpu_default(ARC_MPU_REGION_ALL_ATTR);
#endif
}


void arc_mpu_region(uint32_t index, uint32_t base, uint32_t size,
			 uint32_t region_attr)
{
	if (index >= ARC_FEATURE_MPU_REGIONS) {
		return;
	}

	region_attr &= AUX_MPU_ATTR_MASK;

	/* ARC MPU version 2 and version 3 have different aux reg interface */
#if ARC_FEATURE_MPU_VERSION == 2
	uint8_t bits = _arc_find_msb(size) - 1;
	index = 2 * index;

	if (bits < ARC_FEATURE_MPU_ALIGNMENT_BITS) {
		bits = ARC_FEATURE_MPU_ALIGNMENT_BITS;
	}

	if ((1 << bits) < size) {
		bits++;
	}

	if (size > 0) {
		region_attr |= AUX_MPU_RDP_REGION_SIZE(bits);
		base |= AUX_MPU_VALID_MASK;
	} else {
		base = 0;
	}

	_arc_aux_write(AUX_MPU_RDP0 + index, region_attr);
	_arc_aux_write(AUX_MPU_RDB0 + index, base);

#elif ARC_FEATURE_MPU_VERSION == 4
	if (size < (1 << ARC_FEATURE_MPU_ALIGNMENT_BITS)) {
		size = (1 << ARC_FEATURE_MPU_ALIGNMENT_BITS);
	}

	if (region_attr) {
		region_attr |= AUX_MPU_VALID_MASK;
	}

	_arc_aux_write(AUX_MPU_INDEX, index);
	_arc_aux_write(AUX_MPU_RSTART, base);
	_arc_aux_write(AUX_MPU_REND,
				CALC_REGION_END_ADDR(base, size));
	_arc_aux_write(AUX_MPU_RPER, region_attr);
#endif
}

void arc_mpu_default(uint32_t region_attr)
{
	uint32_t val = _arc_aux_read(AUX_MPU_EN) &
			(~AUX_MPU_ATTR_MASK);
	region_attr &= AUX_MPU_ATTR_MASK;

	_arc_aux_write(AUX_MPU_EN, region_attr | val);
}


int32_t arc_mpu_in_region(uint32_t index, uint32_t start, uint32_t size)
{
#if ARC_FEATURE_MPU_VERSION == 2
	uint32_t r_addr_start;
	uint32_t r_addr_end;
	uint32_t r_size_lshift;

	r_addr_start = _arc_aux_read(AUX_MPU_RDB0 + 2 * index)
			& (~AUX_MPU_VALID_MASK);
	r_size_lshift = _arc_aux_read(AUX_MPU_RDP0 + 2 * index)
			& AUX_MPU_ATTR_MASK;
	r_size_lshift = (r_size_lshift & 0x3) | ((r_size_lshift >> 7) & 0x1C);
	r_addr_end = r_addr_start  + (1 << (r_size_lshift + 1));

	if (start >= r_addr_start && (start + size) < r_addr_end) {
		return 1;
	}

#elif ARC_FEATURE_MPU_VERSION == 4

	if ((index == arc_mpu_probe(start)) &&
		(index == arc_mpu_probe(start + size))) {
		return 1;
	}
#endif

	return 0;
}

int32_t arc_mpu_probe(uint32_t addr)
{
	uint32_t index;

#if ARC_FEATURE_MPU_VERSION == 2
	uint32_t regions = ARC_FEATURE_MPU_REGIONS;

	for (index = 0; index < regions; index++) {
		if (arc_mpu_in_region(index, addr, 0)) {
			return (int32_t)index;
		}
	}

	return -1;

#elif ARC_FEATURE_MPU_VERSION == 4
	_arc_aux_write(AUX_MPU_PROBE, addr);
	index = _arc_aux_read(AUX_MPU_INDEX);

	/* if no match or multiple regions match, return error */
	if (index & (AUX_MPU_INDEX_DEFAULT | AUX_MPU_INDEX_MULT)) {
		return -1;
	} else {
		return (int32_t)index;
	}
#endif
}