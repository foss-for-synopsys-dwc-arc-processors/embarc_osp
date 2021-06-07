/*
* Copyright 2019-2021, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

/**
 * @file MLI Library Helpers API
 *
 * @brief This header includes declarations for helpers set of functions
 */

#ifndef _MLI_HELPERS_API_H_
#define _MLI_HELPERS_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mli_types.h"
#include <stdbool.h>

/**
 * Set of helper defines to index the shape array.
 */
#define FMAP_H_DIM_CHW 1 // height
#define FMAP_W_DIM_CHW 2 // width
#define FMAP_C_DIM_CHW 0 // channels

#define KRNL_H_DIM_CHW 2 // kernel height
#define KRNL_W_DIM_CHW 3 // kernel width
#define KRNL_D_DIM_CHW 1 // kernel depth aka input channels
#define KRNL_C_DIM_CHW 0 // output channels

#define FMAP_H_DIM_HWC 0 // height
#define FMAP_W_DIM_HWC 1 // width
#define FMAP_C_DIM_HWC 2 // channels

#define KRNL_H_DIM_HWC 1 // kernel height
#define KRNL_W_DIM_HWC 2 // kernel width
#define KRNL_D_DIM_HWC 3 // kernel depth aka input channels
#define KRNL_C_DIM_HWC 0 // output channels

#define KRNL_H_DIM_HWCN 0 // kernel height
#define KRNL_W_DIM_HWCN 1 // kernel width
#define KRNL_D_DIM_HWCN 2 // kernel depth aka input channels
#define KRNL_C_DIM_HWCN 3 // output channels

// for Depthwise convolution hwc kernel
#define KRNL_DW_H_DIM_HW1N 0 // Depthwise convolution hwc kernel height
#define KRNL_DW_W_DIM_HW1N 1 // Depthwise convolution hwc kernel width
#define KRNL_DW_D_DIM_HW1N 2 // Depthwise convolution hwc kernel depth (must be == 1)
#define KRNL_DW_N_DIM_HW1N 3 // Depthwise convolution hwc output channels

// for Recurrent kernels
#define KRNL_RNN_W_IN_ELEMS_DIM 1 // Input elements dimension of RNN weights 
#define KRNL_RNN_W_OUT_ELEMS_DIM 2 // Output elements dimension of RNN weights

/** 
 * @brief Count Number of Elements in Tensor
 *
 * @detail Function calculates the number of elements in a tensor starting from the provided 
 * dimension number (dimension numbering starts from 0). If start_dim=0 function calculates total number of elements 
 * Function returns 0 if input tensor rank is invalid or start_dim is bigger then input rank
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in         [I] Input tensor (of any shape)
 * @param start_dim  [I] Start dimension for counting
 *
 * @return Number of elements in tensor
 */
uint32_t mli_hlp_count_elem_num(const mli_tensor *in, uint32_t start_dim);

/** 
 * @brief Get Tensor Basic Element Size
 *
 * @detail This function returns size of tensor basic element in bytes. It returns 0 if  
 * pointer to tensor is invalid, or tensor contains unsupported element type.
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in         [I] Input tensor (of any shape)
 *
 * @return Size of tensor basic element (bytes)
 */
uint32_t mli_hlp_tensor_element_size(const mli_tensor *in);


/**
 * @brief compare data pointer of two tensors
 *
 * @detail This function compares the data pointers of two tensors and returns true if the pointers are equal.
 * If one or both of the tensors are scalar tensors, the compare will return false.
 *
 * @param in1         [I] Input tensor
 * @param in2         [I] Input tensor
 *
 * @return bool indicating if pointers are equal
 */
bool mli_hlp_tensor_data_ptr_cmp(const mli_tensor *in1, const mli_tensor *in2);

/**
 * @brief Convert Tensor
 *
 * @detail This function can be used only for FX and SA quantization types.
 * This function should be used in all places where it is known that neither of the source or destination tensor is a float tensor.
 * This function copies elements from input tensor to output with data conversion according to
 * the output tensor type parameters. This operation does not change tensor shape. It copies it from input to output.
 *
 * For more info on primitive see MLI Documentation
 *
 * @param src      [I] Input tensor (of any shape)
 * @param dst      [O] Output tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_hlp_convert_tensor_safx(const mli_tensor *src, mli_tensor *dst);

/** 
 * @brief Convert Tensor
 *
 * @detail This function copies elements from input tensor to output with data conversion according to 
 * the output tensor type parameters. This operation does not change tensor shape. It copies it from input to output.
 * For conversions with equal container size, in-place computation is permitted.
 *
 * For more info on primitive see MLI Documentation
 *
 * @param src      [I] Input tensor (of any shape)
 * @param dst      [O] Output tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_hlp_convert_tensor(const mli_tensor *src, mli_tensor *dst);

/** 
 * @brief Point to Sub-Tensor
 *
 * @detail This function points to sub tensors in input tensor. This can be considered as indexing in 
 * a multidimensional array. This function performs operations on pointers and doesn't copy data 
 * (only points to subsequence of data in input). For this reason, this function takes only parameters that 
 * can be translated to starting coordinates and size of required data.
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in      [I] Input tensor (of any shape)
 * @param cfg     [I] Configuration structure (for more info see @ref mli_point_to_subtsr_cfg)
 * @param out     [O] Output tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_hlp_point_to_subtensor(const mli_tensor *in, const mli_point_to_subtsr_cfg *cfg, mli_tensor *out);

/**
 * @brief Create a Sub-Tensor from a larger tensor
 *
 * @detail This function points to sub tensors in input tensor. This function performs operations 
 * on pointers and doesn't copy data (only points to subsequence of data in input).
 * For this reason, depending on the parameters, it can happen that the sub tensor contains
 * data that is not adjacent in memory.
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in      [I] Input tensor (of any shape)
 * @param cfg     [I] Configuration structure (for more info see @ref mli_sub_tensor_cfg)
 * @param out     [O] Output tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_hlp_create_subtensor(const mli_tensor *in, const mli_sub_tensor_cfg *cfg, mli_tensor *out);

int32_t mli_hlp_tensor_scale_shift(const mli_tensor *in, const uint32_t scale_idx);

int32_t mli_hlp_tensor_scale(const mli_tensor *in, const uint32_t scale_idx);

int16_t mli_hlp_tensor_zero_offset(const mli_tensor *in, const uint32_t zero_idx);

/**
 * @brief Get a string with the compile time options of the lib
 *
 * @detail This function returns a string with the settings of the following compile options:
 * - MLI_BUILD_REFERENCE
 * - ROUND_MODE
 * - MLI_DEBUG_MODE
 *
 * @return string with compile time options
 */
const char* mli_hlp_compile_options_string();

/**
* @brief Get number of accumulator guard bits for specific MAC variance.
*
* @detail An addition might result in overflow if all bits of operands are used and both operands 
          hold the maximum (or minimum) values. It means that an extra bit is required for this operation. 
          But if sum of several operands is needed (accumulation), more than one extra bit is required to ensure
          that the result does not overflow. This function returns a number of such extra bits used in accumulation
          for MAC (multiply-and-accumulate) based kernels. Separate function exists for each combination of input operands.
*
* @return number of accumulator guard bits used in MAC based kernels (all convolutions, FC, reccurent layers):
*           mli_hlp_accu_guard_bits_sa8_sa8 - MAC with sa8 x sa8 input operands 
*           mli_hlp_accu_guard_bits_fx16_fx16 - MAC with fx16 x fx16 input operands 
*           mli_hlp_accu_guard_bits_fx16_fx8 - MAC with fx16 x fx8 input operands 
*/
uint8_t mli_hlp_accu_guard_bits_sa8_sa8();
uint8_t mli_hlp_accu_guard_bits_fx16_fx16();
uint8_t mli_hlp_accu_guard_bits_fx16_fx8();

/** 
 * @brief Set mem strides
 *
 * @detail This function updates the mem_stride fields based on the rank and shape of the input tensor
 *
 * @param in      [I] Input tensor (with valid shape and rank)
 *
 * @return void
 */
void mli_hlp_set_tensor_mem_strides(mli_tensor* in);

#ifdef __cplusplus
}
#endif

#endif //_MLI_HELPERS_API_H_
