/*
* Copyright 2019-2021, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

/**
 * @file MLI Library Kernels API
 *
 * @brief This header includes declarations for kernels set of functions
 */

#ifndef _MLI_KERNELS_API_H_
#define _MLI_KERNELS_API_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "mli_types.h"

//================================================
//
// Convolution group of kernels
//
//================================================
/**
 * @brief 2D convolution
 *
 * @detail This kernel implements a general 2D convolution operation. It applies each filter of weights tensor 
 * to each framed area of the size of input tensor
 *
 * To implicitly insert additional points to sides of feature map (considering only width and height dimensions), 
 * ensure that you set the padding parameters. Padding influences how feature map is divided into patches 
 * for applying kernels because values of padded points are always zero.
 *
 * ReLU activation function may be applied to result of convolution. 
 *
 * For full list of specialized and highly optimized versions of kernel see @ref mli_krn_conv2d_spec_api.h
 * For more info on primitive see MLI Documentation
 * 
 * @param in      [I] Input feature map tensor (3-dimensional tensor)
 * @param weights [I] Convolution filters weights tensor (4-dimensional tensor)
 * @param bias    [I] Convolution filters biases tensor (1-dimensional tensor)
 * @param cfg     [I] Convolution parameters structure (for more info see @ref mli_conv2d_cfg)
 * @param out     [O] Output feature map tensor. Result is stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_conv2d_hwcn_fx16(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg * cfg,
        mli_tensor * out);

mli_status mli_krn_conv2d_hwcn_fx16_fx8_fx8(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg * cfg,
        mli_tensor * out);

mli_status mli_krn_conv2d_hwcn_sa8_sa8_sa32(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg * cfg,
        mli_tensor * out);

//========================================================
// Specializations for k1x1
//========================================================
mli_status mli_krn_conv2d_hwcn_fx16_k1x1(
        const mli_tensor* in,
        const mli_tensor* weights,
        const mli_tensor* bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

mli_status mli_krn_conv2d_hwcn_fx16_fx8_fx8_k1x1(
        const mli_tensor* in,
        const mli_tensor* weights,
        const mli_tensor* bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

mli_status mli_krn_conv2d_hwcn_sa8_sa8_sa32_k1x1(
        const mli_tensor* in,
        const mli_tensor* weights,
        const mli_tensor* bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

//========================================================
// Specializations for k3x3
//========================================================
mli_status mli_krn_conv2d_hwcn_fx16_k3x3(
        const mli_tensor* in,
        const mli_tensor* weights,
        const mli_tensor* bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

mli_status mli_krn_conv2d_hwcn_fx16_fx8_fx8_k3x3(
        const mli_tensor* in,
        const mli_tensor* weights,
        const mli_tensor* bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

mli_status mli_krn_conv2d_hwcn_sa8_sa8_sa32_k3x3(
        const mli_tensor* in,
        const mli_tensor* weights,
        const mli_tensor* bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

//========================================================
// Specializations for k5x5
//========================================================
mli_status mli_krn_conv2d_hwcn_fx16_k5x5(
        const mli_tensor* in,
        const mli_tensor* weights,
        const mli_tensor* bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

mli_status mli_krn_conv2d_hwcn_fx16_fx8_fx8_k5x5(
        const mli_tensor* in,
        const mli_tensor* weights,
        const mli_tensor* bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

mli_status mli_krn_conv2d_hwcn_sa8_sa8_sa32_k5x5(
        const mli_tensor* in,
        const mli_tensor* weights,
        const mli_tensor* bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

/**
 * @brief 2D Depthwise convolution
 *
 * @detail This kernel implements a 2D depthwise convolution operation applying each filter channel 
 * to each input channel separatelly. As a result, output image depth is the same as input image depth.
 *
 * MLI implementation of depthwise convolution is compatible with caffe implementation of convolution layer 
 * with group parameter equal to number of input channels. In comparison with TensorFlow implementation 
 * (tf.nn.depthwise_conv2d in python API), this implementation does not support channel multiplier feature. 
 * Hence, the last dimension of weights tensor must be equal to 1.
 *
 * ReLU activation function may be applied to result of convolution. 
 *
 * For full list of specialized and highly optimized versions of kernel see @ref mli_krn_depthwise_conv2d_spec_api.h
 * For more info on primitive see MLI Documentation
 *
 * @param in      [I] Input feature map tensor (3-dimensional tensor)
 * @param weights [I] Convolution filters weights tensor (4-dimensional tensor)
 * @param bias    [I] Convolution filters biases tensor (1-dimensional tensor)
 * @param cfg     [I] Convolution parameters structure (for more info see @ref mli_conv2d_cfg)
 * @param out     [O] Output feature map tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_depthwise_conv2d_hwcn_fx16(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg * cfg,
        mli_tensor * out);

mli_status mli_krn_depthwise_conv2d_hwcn_fx16_fx8_fx8(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg * cfg,
        mli_tensor * out);

mli_status mli_krn_depthwise_conv2d_hwcn_sa8_sa8_sa32(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg * cfg,
        mli_tensor * out);

//========================================================
// Specializations for k3x3
//========================================================
mli_status mli_krn_depthwise_conv2d_hwcn_fx16_k3x3(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

mli_status mli_krn_depthwise_conv2d_hwcn_fx16_fx8_fx8_k3x3(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

mli_status mli_krn_depthwise_conv2d_hwcn_sa8_sa8_sa32_k3x3(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

//========================================================
// Specializations for k5x5
//========================================================
mli_status mli_krn_depthwise_conv2d_hwcn_fx16_k5x5(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

mli_status mli_krn_depthwise_conv2d_hwcn_fx16_fx8_fx8_k5x5(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

mli_status mli_krn_depthwise_conv2d_hwcn_sa8_sa8_sa32_k5x5(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

/**
 * @brief 2D Group convolution
 *
 * @detail This kernel implements a grouped 2D convolution which applies general 2D convolution operation 
 * on a separate subset (groups) of inputs. Each group performs convolutions independent of the other groups 
 * to give M different outputs. These individual outputs are then concatenated together to give the final output. 
 *
 * TensorFlow-like “channel multiplier” functionality of depthwise convolution can be expressed by group 
 * convolution with number of groups equal to input channels and N equal to channel multiplier number of 
 * filters per each group. 
 *
 * ReLU activation function may be applied to result of convolution. 
 *
 * For full list of specialized and highly optimized versions of kernel see @ref mli_krn_group_conv2d_spec_api.h
 * For more info on primitive see MLI Documentation
 *
 * @param in      [I] Input feature map tensor (3-dimensional tensor)
 * @param weights [I] Convolution filters weights tensor (4-dimensional tensor)
 * @param bias    [I] Convolution filters biases tensor (1-dimensional tensor)
 * @param cfg     [I] Convolution parameters structure (for more info see @ref mli_conv2d_cfg)
 * @param out     [O] Output feature map tensor. Result will be stored here
 *
 * @return MLI status code
 */

mli_status mli_krn_group_conv2d_hwcn_fx16(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg * cfg,
        mli_tensor * out);

mli_status mli_krn_group_conv2d_hwcn_fx16_fx8_fx8(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg * cfg,
        mli_tensor * out);

mli_status mli_krn_group_conv2d_hwcn_sa8_sa8_sa32(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg * cfg,
        mli_tensor * out);

//========================================================
// Specializations for k3x3
//========================================================
mli_status mli_krn_group_conv2d_hwcn_fx16_k3x3(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

mli_status mli_krn_group_conv2d_hwcn_fx16_fx8_fx8_k3x3(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

mli_status mli_krn_group_conv2d_hwcn_sa8_sa8_sa32_k3x3(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

//========================================================
// Specializations for k5x5
//========================================================
mli_status mli_krn_group_conv2d_hwcn_fx16_k5x5(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

mli_status mli_krn_group_conv2d_hwcn_fx16_fx8_fx8_k5x5(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

mli_status mli_krn_group_conv2d_hwcn_sa8_sa8_sa32_k5x5(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_conv2d_cfg* cfg,
        mli_tensor* out);

/**
* @brief 2D Transpose convolution
*
* @detail This kernel implements a general 2D transposed convolution operation which work by swapping the
* forward and backward passes of a convolution i.e., from something that has the shape of the output of some 
* convolution to something that has the shape of its input while maintaining a connectivity pattern that is 
* compatible with said convolution.
*
* ReLU activation function may be applied to result of convolution. 
* Dilation parameter of convolutions config isn't applicable in MLI transposed convolution and is ignored.
*
* For More information on implementation, see MLI Documentation. 
*
* @param in      [I] Input feature map tensor (3-dimensional tensor)
* @param weights [I] Convolution filters weights tensor (4-dimensional tensor)
* @param bias    [I] Convolution filters biases tensor (1-dimensional tensor)
* @param cfg     [I] Convolution parameters structure (for more info see @ref mli_conv2d_cfg)
* @param out     [O] Output feature map tensor. Result will be stored here
*
* @return MLI status code
*/

mli_status mli_krn_transpose_conv2d_hwcn_fx16(
    const mli_tensor * in,
    const mli_tensor * weights,
    const mli_tensor * bias,
    const mli_conv2d_cfg * cfg,
    mli_tensor * out);

mli_status mli_krn_transpose_conv2d_hwcn_fx16_fx8_fx8(
    const mli_tensor * in,
    const mli_tensor * weights,
    const mli_tensor * bias,
    const mli_conv2d_cfg * cfg,
    mli_tensor * out);

mli_status mli_krn_transpose_conv2d_hwcn_sa8_sa8_sa32(
    const mli_tensor * in,
    const mli_tensor * weights,
    const mli_tensor * bias,
    const mli_conv2d_cfg * cfg,
    mli_tensor * out);

//========================================================
// Specializations for k2x2 and stride 2x2
//========================================================
mli_status mli_krn_transpose_conv2d_hwcn_fx16_k2x2_str2(
    const mli_tensor* in,
    const mli_tensor* weights,
    const mli_tensor* bias,
    const mli_conv2d_cfg* cfg,
    mli_tensor* out);

mli_status mli_krn_transpose_conv2d_hwcn_fx16_fx8_fx8_k2x2_str2(
    const mli_tensor* in,
    const mli_tensor* weights,
    const mli_tensor* bias,
    const mli_conv2d_cfg* cfg,
    mli_tensor* out);

mli_status mli_krn_transpose_conv2d_hwcn_sa8_sa8_sa32_k2x2_str2(
    const mli_tensor* in,
    const mli_tensor* weights,
    const mli_tensor* bias,
    const mli_conv2d_cfg* cfg,
    mli_tensor* out);


//========================================================
// Specializations for k4x4 and stride 2x2
//========================================================
mli_status mli_krn_transpose_conv2d_hwcn_fx16_k4x4_str2(
    const mli_tensor* in,
    const mli_tensor* weights,
    const mli_tensor* bias,
    const mli_conv2d_cfg* cfg,
    mli_tensor* out);

mli_status mli_krn_transpose_conv2d_hwcn_fx16_fx8_fx8_k4x4_str2(
    const mli_tensor* in,
    const mli_tensor* weights,
    const mli_tensor* bias,
    const mli_conv2d_cfg* cfg,
    mli_tensor* out);

mli_status mli_krn_transpose_conv2d_hwcn_sa8_sa8_sa32_k4x4_str2(
    const mli_tensor* in,
    const mli_tensor* weights,
    const mli_tensor* bias,
    const mli_conv2d_cfg* cfg,
    mli_tensor* out);

//================================================
//
// Pooling group of kernels
//
//================================================
/**
 * @brief Average pooling
 *
 * @detail This kernel implements an average pooling operation. Each channel of input is considered independently, 
 * which means that the analysis fragment includes only neighbor points of the channel. For each fragment 
 * of input tensor, average value over all considered ponts is defined as the output value. 
 * The fragment size is defined in configuration structure according to kernel_width and kernel_height values.
 *
 * Window positioning and moving is performed according to stride and padding parameters. 
 * This logic is similar to convolution 2D operation. Average Pooling primitive does not analyze an area smaller 
 * than kernel size (typically, this occurs on the right and bottom borders). In this case, ensure that you set 
 * padding parameters explicitly in order not to miss valid border values. Padded values do not participate 
 * in the calculations. So when a fragment includes padded values, only the existing values are analysed 
 * (this also implies reducing of divider for average calculation).
 *
 * For full list of specialized and highly optimized versions of kernel see @ref mli_krn_avepool_spec_api.h
 * For more info on primitive see MLI Documentation
 *
 * @param in      [I] Input feature map tensor (3-dimensional tensor)
 * @param cfg     [I] Pooling parameters structure (for more info see @ref mli_pool_cfg)
 * @param out     [O] Output feature map tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_avepool_hwc_sa8(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_avepool_hwc_sa8_k2x2(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_avepool_hwc_sa8_k3x3(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_avepool_hwc_fx8(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_avepool_hwc_fx8_k2x2(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_avepool_hwc_fx8_k3x3(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_avepool_hwc_fx16(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_avepool_hwc_fx16_k2x2(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_avepool_hwc_fx16_k3x3(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);

/**
 * @brief MAX pooling
 *
 * @detail This kernel implements a max pooling operation. Each channel of input is considered independently, 
 * which means that the analysis fragment includes only neighbor points of the channel. For each fragment 
 * of input tensor, maximum value is being defined as the output value. The fragment size is defined in configuration
 * structure according to kernel_width and kernel_height values.
 *
 * Splitting input on fragments is performed according to stride and padding parameters. This logic is similar to 
 * convolution 2D operation
 *
 * For full list of specialized and highly optimized versions of kernel see @ref mli_krn_maxpool_spec_api.h
 * For more info on primitive see MLI Documentation

 * @param in      [I] Input feature map tensor (3-dimensional tensor)
 * @param cfg     [I] Pooling parameters structure (for more info see @ref mli_pool_cfg)
 * @param out     [O] Output feature map tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_maxpool_hwc_sa8(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_maxpool_hwc_sa8_k2x2(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_maxpool_hwc_sa8_k3x3(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_maxpool_hwc_fx8(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_maxpool_hwc_fx8_k2x2(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_maxpool_hwc_fx8_k3x3(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_maxpool_hwc_fx16(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_maxpool_hwc_fx16_k2x2(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);
mli_status mli_krn_maxpool_hwc_fx16_k3x3(const mli_tensor * in, const mli_pool_cfg * cfg, mli_tensor * out);

//================================================
//
// Diverse group of kernels
//
//================================================
/**
 * @brief Argmax
 *
 * @detail This kernel returns the indexes of maximum values across the whole tensor, or for each slice across a dimension.
 *
 * Ensure that in tensor is quantized on the tensor level. It implies that the tensor contains a single scale factor and a single zero offset.
 * For out tensor only integer types are allowed (fx8, fx16, sa8, sa32). 
 * el_params field of out tensor is configured by the kernel to reflect fully integer values
 * (frac_bits = 0 for fx data format, zero_offset = 0, scale = 1 and scale_frac_bits = 0 for sa data format)
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in      [I] Input feature tensor (of any shape)
 * @param cfg     [I] Configuration structure (for more info see @ref mli_argmax_cfg)
 * @param out     [O] Output feature tensor. Result will be stored here.
 *
 * @return MLI status code
 */
mli_status mli_krn_argmax_sa8(
    const mli_tensor *in,
    const mli_argmax_cfg *cfg,
    mli_tensor *out);

mli_status mli_krn_argmax_fx16(
    const mli_tensor *in,
    const mli_argmax_cfg *cfg,
    mli_tensor *out);

//================================================
//
// Common group of kernels
//
//================================================
/**
 * @brief Fully connected
 *
 * @detail This kernel implements fully connected layer, also usually referred to as the inner product or dense layer.
 *
 * Ensure that the weight for this kernel is a 2-dimensional tensor (matrix of shape [M, N]),
 * and Bias must be 1-dimensional tensor of shape [M]. Shape of input tensor is not considered and only total number 
 * of elements is considered (must be equal to N). Kernel outputs a 1-dimensional tensor of shape [M].
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in      [I] Input feature tensor (of any shape)
 * @param weights [I] Weights tensor (2-dimensional tensor)
 * @param bias    [I] Biases tensor (1-dimensional tensor)
 * @param out     [O] Output feature tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_fully_connected_fx8( /* DEPRECATED */
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_fully_connected_cfg * cfg,
        mli_tensor * out);

mli_status mli_krn_fully_connected_fx16(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_fully_connected_cfg * cfg,
        mli_tensor * out);

mli_status mli_krn_fully_connected_fx16_fx8_fx8(
    const mli_tensor* in,
    const mli_tensor* weights,
    const mli_tensor* bias,
    const mli_fully_connected_cfg * cfg,
    mli_tensor* out);

mli_status mli_krn_fully_connected_fx8w16d( /* DEPRECATED */
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_fully_connected_cfg * cfg,
        mli_tensor * out);

mli_status mli_krn_fully_connected_sa8_sa8_sa32(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_fully_connected_cfg * cfg,
        mli_tensor * out);

/*
* mli_krn_fully_connected_sa8_sa8_sa32_ext_bias is a specialized version of the kernel
* which performs calculations much faster, but requires bias data to be adjusted according to the following formula:
*
* bias__i = orig_bias__i + SUM_j(in_zero_point * W__i_j)
* 
* For more info on the specialization see Fully Connected primitive description in the MLI Documentation
*/
mli_status mli_krn_fully_connected_sa8_sa8_sa32_ext_bias(
        const mli_tensor * in,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_fully_connected_cfg * cfg,
        mli_tensor * out);

/**
 * @brief Long Short Term Memory (LSTM) Cell
 *
 * @detail This kernel implements the default non-peephole implementation of long short term memory (LSTM) cell
 * with input (i), gate (g), forget (f) and out (o) gates  
 *
 * This kernel implies sequential processing of the set of inputs vectors which is passed by input tensor of shape 
 * (sequence_length, N) where N is the length of the single frame. Both directions of processing (forward and backward) 
 * are supported and defined by cfg structure. Kernel can output the intermediate results of each step, or only the result 
 * of the last step. Dense part of calculations uses scratch data from configuration structure for results, 
 * and consequently output and previous output tensors might use the same memory if it is acceptable to rewrite 
 * previous output data.
 *
 * For more info on primitive see MLI Documentation.
 *
 * @param in          [I] Input feature tensor. Must be a tensor of shape (sequence_length, input_elements).
 * @param prev_out    [I] Previous output feature tensor. Must be a one-dimensional tensor of shape (cell_elements).
 * @param weights_in  [I] Input Weights tensor (set of 4 matrixes in the [i,g,f,o] order: 3-dimensional tensor)
 * @param weights_out [I] Hidden Weights tensor (set of 4 matrixes in the [i,g,f,o] order: 3-dimensional tensor)
 * @param bias        [I] Biases tensor (set of 4 vectors in the [i,g,f,o] order: 2-dimensional tensor)
 * @param cfg         [I] RNN Configuration structure (for more info see @ref mli_rnn_cell_cfg)
 * @param cell        [I/O] Cell memory state (1-dimensional tensor)
 * @param out         [O] Output feature tensor. Result will be stored here (single output or sequence of outputs depending on mode)
 *
 * @return MLI status code
 */
mli_status mli_krn_lstm_cell_fx16(
        const mli_tensor * in,
        const mli_tensor * prev_out,
        const mli_tensor * weights_in,
        const mli_tensor * weights_out,
        const mli_tensor * bias,
        const mli_lut * tanh_lut,
        const mli_lut * sigm_lut,
        const mli_rnn_cell_cfg * cfg,
        mli_tensor * cell,
        mli_tensor * out);

mli_status mli_krn_lstm_cell_fx16_fx8_fx8(
        const mli_tensor * in,
        const mli_tensor * prev_out,
        const mli_tensor * weights_in,
        const mli_tensor * weights_out,
        const mli_tensor * bias,
        const mli_lut * tanh_lut,
        const mli_lut * sigm_lut,
        const mli_rnn_cell_cfg * cfg,
        mli_tensor * cell,
        mli_tensor * out);

mli_status mli_krn_lstm_cell_sa8_sa8_sa32(
        const mli_tensor * in,
        const mli_tensor * prev_out,
        const mli_tensor * weights_in,
        const mli_tensor * weights_out,
        const mli_tensor * bias,
        const mli_lut * tanh_lut,
        const mli_lut * sigm_lut,
        const mli_rnn_cell_cfg * cfg,
        mli_tensor * cell,
        mli_tensor * out);

/**
 * @brief Gated Recurrent Unit (GRU) Cell
 *
 * @detail This kernel implements the Gated Recurrent Unit (GRU) cell with update (z), reset (r) and new (n) gates 
 * in version where a reset gate is applied on the hidden state before matrix multiplication
 * 
 * This kernel implies sequential processing of the set of inputs vectors which is passed by input tensor 
 * of shape (sequence_length, N) where N is the length of the single frame. Both directions of processing (forward and backward) 
 * are supported and defined by cfg structure. Kernel can output the intermediate results of each step, or only the result 
 * of the last step.
 *
 * For more info on primitive see MLI Documentation.
 *
 * @param in          [I] Input feature tensor. Must be a tensor of shape (sequence_length, input_elements).
 * @param prev_out    [I] Previous output feature tensor. Must be a one-dimensional tensor of shape (out_elements).
 * @param weights_in  [I] Input Weights tensor (set of 3 matrixes in the [z,r,n] order: 3-dimensional tensor)
 * @param weights_out [I] Hidden Weights tensor (set of 3 matrixes in the [z,r,n] order: 3-dimensional tensor)
 * @param bias        [I] Biases tensor (set of 3 vectors in the [z,r,n] order: 2-dimensional tensor)
 * @param tanh_lut    [I] LUT table structure prepared for the hyperbolic tangent activation
 * @param sigm_lut    [I] LUT table structure prepared for sigmoid activation
 * @param cfg         [I] RNN Configuration structure (for more info see @ref mli_rnn_cell_cfg)
 * @param out         [O] Output feature tensor. Result will be stored here (single output or sequence of outputs depending on mode)
 *
 * @return MLI status code
 */
mli_status mli_krn_gru_cell_fx16(
        const mli_tensor * in,
        const mli_tensor * prev_out,
        const mli_tensor * weights_in,
        const mli_tensor * weights_out,
        const mli_tensor * bias,
        const mli_lut * tanh_lut,
        const mli_lut * sigm_lut,
        const mli_rnn_cell_cfg * cfg,
        mli_tensor * out);

mli_status mli_krn_gru_cell_fx16_fx8_fx8(
        const mli_tensor * in,
        const mli_tensor * prev_out,
        const mli_tensor * weights_in,
        const mli_tensor * weights_out,
        const mli_tensor * bias,
        const mli_lut * tanh_lut,
        const mli_lut * sigm_lut,
        const mli_rnn_cell_cfg * cfg,
        mli_tensor * out);

mli_status mli_krn_gru_cell_sa8_sa8_sa32(
        const mli_tensor * in,
        const mli_tensor * prev_out,
        const mli_tensor * weights_in,
        const mli_tensor * weights_out,
        const mli_tensor * bias,
        const mli_lut * tanh_lut,
        const mli_lut * sigm_lut,
        const mli_rnn_cell_cfg * cfg,
        mli_tensor * out);

/**
 * @brief Basic Recurrent Neural Network Cell
 *
 * @detail This kernel implements a single basic dense calculation typically used in the majority of RNN architectures
 *
 * The Kernel supports multiple number of inputs restricted by MLI_RNN_MAX_INPUT parameter
 *
 * For more info on primitive see MLI Documentation.
 *
 * @param in        [I] Pointer to the array of pointers to constant input tensors
 * @param weights   [I] Pointer to the array of pointers to constant weights tensors
 * @param bias      [I] Pointer to constant bias tensor
 * @param cfg       [I] Configuration structure (for more info see @ref mli_rnn_dense_cfg)
 * @param out       [O] Output feature tensor. Result will be stored here
 *
 * @return MLI status code
 */

mli_status mli_krn_rnn_dense_fx16(
        const mli_tensor ** in,
        const mli_tensor ** weights,
        const mli_tensor * bias,
        const mli_rnn_dense_cfg * cfg,
        mli_tensor * out);

mli_status mli_krn_rnn_dense_fx16_fx8_fx8(
        const mli_tensor ** in,
        const mli_tensor ** weights,
        const mli_tensor * bias,
        const mli_rnn_dense_cfg * cfg,
        mli_tensor * out);

mli_status mli_krn_rnn_dense_sa8_sa8_sa32(
        const mli_tensor ** in,
        const mli_tensor ** weights,
        const mli_tensor * bias,
        const mli_rnn_dense_cfg * cfg,
        mli_tensor * out);

mli_status mli_krn_basic_rnn_cell_fx8( /* DEPRECATED */
        const mli_tensor * in,
        const mli_tensor * prev_out,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_rnn_cell_cfg_depr * cfg,
        mli_tensor * out);

mli_status mli_krn_basic_rnn_cell_fx16( /* DEPRECATED */
        const mli_tensor * in,
        const mli_tensor * prev_out,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_rnn_cell_cfg_depr * cfg,
        mli_tensor * out);

mli_status mli_krn_basic_rnn_cell_fx8w16d( /* DEPRECATED */
        const mli_tensor * in,
        const mli_tensor * prev_out,
        const mli_tensor * weights,
        const mli_tensor * bias,
        const mli_rnn_cell_cfg_depr * cfg,
        mli_tensor * out);

//================================================
//
// Activation group of kernels
//
//================================================
/**
 * @brief ReLU Activation function
 *
 * @detail This kernel represents Rectified Linear unit (ReLU). It performs various types of the rectifier activation on input. 
 * The following types of ReLU supported by this type of kernel: General ReLU, ReLU1, ReLU6
 * Kernel outputs a tensor of the same shape and type as input tensor.
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in      [I] Input feature tensor (of any shape)
 * @param cfg     [I] Configuration structure (for more info see @ref mli_relu_cfg)
 * @param out     [O] Output feature tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_relu_fx8(const mli_tensor * in, const mli_relu_cfg * cfg, mli_tensor * out);
mli_status mli_krn_relu_fx16(const mli_tensor * in, const mli_relu_cfg * cfg, mli_tensor * out);
mli_status mli_krn_relu_sa8(const mli_tensor * in, const mli_relu_cfg * cfg, mli_tensor * out);

/**
 * @brief Leaky ReLU Activation function
 *
 * @detail This kernel represents Rectified Linear unit (ReLU) with a negative slope. 
 * The function accepts two tensors as input and one as output. The first input tensor is the feature map 
 * to be processed by the kernel, and the second input is a tensor-scalar that holds 
 * a negative slope coefficient(Note, special tensor-scalar form can be used. see mli_tensor description in MLI Documentation).
 * Kernel outputs a tensor of the same shape and type as input tensor.
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in           [I] Input feature tensor (of any shape)
 * @param slope_coeff  [I] Slope coefficient scalar tensor 
 * @param out          [O] Output feature tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_leaky_relu_fx8(const mli_tensor * in, const mli_tensor * slope_coeff, mli_tensor * out);
mli_status mli_krn_leaky_relu_fx16(const mli_tensor * in, const mli_tensor * slope_coeff, mli_tensor * out);
mli_status mli_krn_leaky_relu_sa8(const mli_tensor * in, const mli_tensor * slope_coeff, mli_tensor * out);

/**
 * @brief Parametric ReLU Activation function
 *
 * @detail This kernel represents Rectified Linear unit (ReLU) with a negative slope. 
 * The function accepts two tensors as input and one as output. The first input tensor is the feature map 
 * to be processed by the kernel, and the second input is a tensor that holds a negative slope coefficients
 * Kernel outputs a tensor of the same shape and type as input tensor.
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in           [I] Input feature tensor (of any shape)
 * @param slope_coeff  [I] Slope coefficients tensor 
 * @param cfg          [I] Parametric Relu Configuration 
 * @param out          [O] Output feature tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_prelu_fx8(
        const mli_tensor * in,
        const mli_tensor * slope_coeff,
        const mli_prelu_cfg *cfg,
        mli_tensor * out);

mli_status mli_krn_prelu_fx16(
        const mli_tensor * in,
        const mli_tensor * slope_coeff,
        const mli_prelu_cfg *cfg,
        mli_tensor * out);

mli_status mli_krn_prelu_sa8(
        const mli_tensor * in,
        const mli_tensor * slope_coeff,
        const mli_prelu_cfg *cfg,
        mli_tensor * out);

/**
 * @brief Sigmoid Activation function
 *
 * @detail This kernel performs sigmoid (also mentioned as logistic) activation function on input tensor element-wise
 * and stores the result to the output tensor. Kernel outputs a tensor of the same shape and type as input tensor.
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in      [I] Input feature tensor (of any shape)
 * @param out     [O] Output feature tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_sigm_fx8(const mli_tensor *in, const mli_lut *lut, mli_tensor *out);
mli_status mli_krn_sigm_fx16(const mli_tensor *in, const mli_lut *lut, mli_tensor *out);
mli_status mli_krn_sigm_sa8(const mli_tensor *in, const mli_lut *lut, mli_tensor *out);
mli_status mli_krn_sigm_create_lut(mli_lut *lut);
int32_t mli_krn_sigm_get_lut_size();

/**
 * @brief Hyperbolic Tangent Activation function
 *
 * @detail This kernel performs hyperbolic tangent activation function on input tensor element-wise 
 * and store result to the output tensor. Kernel outputs a tensor of the same shape and type as input tensor.
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in      [I] Input feature tensor (of any shape)
 * @param out     [O] Output feature tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_tanh_fx8(const mli_tensor *in, const mli_lut *lut, mli_tensor *out);
mli_status mli_krn_tanh_fx16(const mli_tensor *in, const mli_lut *lut, mli_tensor *out);
mli_status mli_krn_tanh_sa8(const mli_tensor *in, const mli_lut *lut, mli_tensor *out);
mli_status mli_krn_tanh_create_lut(mli_lut *lut);
int32_t mli_krn_tanh_get_lut_size();

/**
 * @brief Softmax
 *
 * @detail This kernel performs activation function which is a generalization of the logistic function.
 * The SoftMax function is often used as the final layer of a neural network-based classifier and it's output can be considered 
 * as a probability distribution over N different possible outcomes. The sum of all the entries tends to 1.
 * Softmax function might be applied to the whole tensor, or along a specific axis. 
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in      [I] Input feature tensor (of any shape)
 * @param out     [O] Output feature tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_softmax_sa8(const mli_tensor *in, const mli_lut *lut, const mli_softmax_cfg *cfg, mli_tensor* out);
mli_status mli_krn_softmax_fx16(const mli_tensor *in, const mli_lut *lut, const mli_softmax_cfg *cfg, mli_tensor* out);
mli_status mli_krn_softmax_fx8(const mli_tensor *in, const mli_lut *lut, const mli_softmax_cfg *cfg, mli_tensor * out); /* DEPRECATED */
mli_status mli_krn_softmax_create_lut(mli_lut *lut);
int32_t mli_krn_softmax_get_lut_size();

/**
 * @brief L2 Normalization Activation function
 *
 * @detail This kernel performs L2 Normalization activation function on input tensor element-wise 
 * and store result to the output tensor. Kernel outputs a tensor of the same shape and type as input tensor.
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in      [I] Input feature tensor (of any shape)
 * @param out     [O] Output feature tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_l2_normalize_fx16(const mli_tensor *in, const mli_tensor *epsilon,
                                     const mli_lut *lut, const mli_l2_normalize_cfg *cfg, mli_tensor *out);
mli_status mli_krn_l2_normalize_sa8(const mli_tensor *in, const mli_tensor *epsilon,
                                    const mli_lut *lut, const mli_l2_normalize_cfg *cfg, mli_tensor *out);
mli_status mli_krn_l2_normalize_create_lut(mli_lut *lut);
int32_t mli_krn_l2_normalize_get_lut_size();



//================================================
//
// Elementwise group of kernels
//
//================================================
/*
 * @brief Elementwise Addition
 *
 * @detail This kernel adds two tensors of the same shape element-wise  and stores results to the output tensor 
 * saving the shape of inputs. It supports simple broadcasting of single value (scalar tensor) on general tensor.
 * One of the operands can be a scalar (Note, special tensor-scalar form can be used. see mli_tensor description in 
 * MLI Documentation)
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in1     [I] First input feature tensor (or scalar tensor)
 * @param in2     [I] Second input feature tensor (or scalar tensor)
 * @param out     [O] Output feature tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_eltwise_add_fx8(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);
mli_status mli_krn_eltwise_add_fx16(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);
mli_status mli_krn_eltwise_add_sa8(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);

/*
 * @brief Elementwise Subtraction
 *
 * @detail This kernel subtracts element-wise, the second input tensor (subtrahend) from the first input tensor (minuend) 
 * and stores results to the output tensor It supports simple broadcasting of single value (scalar tensor) on general tensor.
 * One of the operands can be a scalar (Note, special tensor-scalar form can be used. see mli_tensor description in MLI Documentation)
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in1     [I] Minuend input feature tensor (or scalar tensor)
 * @param in2     [I] Subtrahend input feature tensor (or scalar tensor)
 * @param out     [O] Output feature tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_eltwise_sub_fx8(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);
mli_status mli_krn_eltwise_sub_fx16(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);
mli_status mli_krn_eltwise_sub_sa8(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);

/* @brief Elementwise Multiplication
 *
 * @detail This kernel multiplies two tensors of the same shape element-wise and store results to the output tensor 
 * saving the shape of inputs. It supports simple broadcasting of single value (scalar tensor) on general tensor.
 * One of the operands can be a scalar (Note, special tensor-scalar form can be used. see mli_tensor description in MLI Documentation)
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in1     [I] First input feature tensor (or scalar tensor)
 * @param in2     [I] Second input feature tensor (or scalar tensor) 
 * @param out     [O] Output feature tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_eltwise_mul_fx8(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);
mli_status mli_krn_eltwise_mul_fx16(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);
mli_status mli_krn_eltwise_mul_sa8(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);

/* @brief Elementwise MAX/MIN
 *
 * @detail This kernel finds element-wise maximum / minimum of inputs operands and store results to the output tensor
 * saving the shape of inputs. It supports simple broadcasting of single value (scalar tensor) on general tensor.
 * One of the operands can be a scalar (Note, special tensor-scalar form can be used. see mli_tensor description in MLI Documentation)
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in1     [I] First input feature tensor (or scalar tensor)
 * @param in2     [I] Second input feature tensor (or scalar tensor) 
 * @param out     [O] Output feature tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_eltwise_min_fx8(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);
mli_status mli_krn_eltwise_min_fx16(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);
mli_status mli_krn_eltwise_min_sa8(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);
mli_status mli_krn_eltwise_max_fx8(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);
mli_status mli_krn_eltwise_max_fx16(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);
mli_status mli_krn_eltwise_max_sa8(const mli_tensor * in1, const mli_tensor * in2, mli_tensor * out);



//================================================
//
// Data manipulation group of kernels
//
//================================================
/** 
 * @brief Permute Tensor
 *
 * @detail The kernel permutes dimensions of input tensor according to provided order. In other words, it transposes input tensors.
 * The new order of dimensions is given by perm_dim array of kernel configuration structure. Output dimension #idx 
 * corresponds to the dimension of input tensor with #perm_dim[idx]. Tensor's data is reordered according to new shape.
 *
 * For more info on primitive see MLI Documentation
 *
 * @param in      [I] Input tensor (of any shape)
 * @param cfg     [I] Permute parameters structure (for more info see @ref mli_permute_cfg)
 * @param out     [O] Output tensor. Result will be stored here
 *
 * @return MLI status code
 */
mli_status mli_krn_permute_fx8(const mli_tensor * in, const mli_permute_cfg * cfg, mli_tensor * out);
mli_status mli_krn_permute_fx16(const mli_tensor * in, const mli_permute_cfg * cfg, mli_tensor * out);
mli_status mli_krn_permute_sa8(const mli_tensor * in, const mli_permute_cfg * cfg, mli_tensor * out);

#ifdef __cplusplus
}
#endif
#endif    //_MLI_KERNELS_API_H_
