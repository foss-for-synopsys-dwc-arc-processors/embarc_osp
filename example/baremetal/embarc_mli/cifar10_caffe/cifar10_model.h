/*
* Copyright 2019, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

//
// Convolutional Neural network model for small pictures classification.
// Based on standart Caffe [1] example for CIFAR-10 [2] dataset
//
// Correspondence of classes:
//  0: airplane
//  1: automobile
//  2: bird
//  3: cat
//  4: deer
//  5: dog
//  6: frog
//  7: horse
//  8: ship
//  9: truck
//
//  [1] https://github.com/BVLC/caffe/tree/master/examples/cifar10
//  [2] https://www.cs.toronto.edu/~kriz/cifar.html

#ifndef _CIFAR10_MODEL_H_
#define _CIFAR10_MODEL_H_

#include <stdint.h>

#include "mli_types.h"

//=============================================
//
// Model interface
//
//=============================================
// Input tensor. To be filled with input image by user befor calling inference function (cifar10_cf_net).
#define IN_POINTS (32 * 32 * 3)
extern mli_tensor * const cifar10_cf_net_input;

// Output tensor for model. Will be filled with probabilities vector by model
#define OUT_POINTS (10)
extern mli_tensor * const cifar10_cf_net_output;

// Model inference function
//
// Get input data from cifar10_cf_net_input tensor (FX format), fed it to the neural network,
// and writes results to cifar10_cf_net_output tensor (FX format). It is user responsibility
// to prepare input tensor correctly before calling this function and get result from output tensor
// after function finish
void cifar10_cf_net(void);


//=============================================
//
// Model bit depth configuration
//
//=============================================
#define MODEL_FX_8       (8)
#define MODEL_FX_16      (16)
#define MODEL_FX_8W16D   (816)

#if !defined(MODEL_BIT_DEPTH)
#define MODEL_BIT_DEPTH (MODEL_FX_16)
#endif

#if !defined(MODEL_BIT_DEPTH) || \
    (MODEL_BIT_DEPTH != MODEL_FX_8 && MODEL_BIT_DEPTH != MODEL_FX_16 && MODEL_BIT_DEPTH != MODEL_FX_8W16D)
#error "MODEL_BIT_DEPTH must be defined correctly!"
#endif

#if (MODEL_BIT_DEPTH == MODEL_FX_8)
typedef int8_t d_type;
#else
typedef int16_t d_type;
#endif

#endif  //_CIFAR10_MODEL_H_
