/*
* Copyright 2019-2021, Synopsys, Inc.
* All rights reserved.
*
* This source code is licensed under the BSD-3-Clause license found in
* the LICENSE file in the root directory of this source tree.
*
*/

/**
 * @file MLI Library Public Types
 *
 * @brief This header defines MLI Library data types
 */

#ifndef _MLI_TYPES_H_
#define _MLI_TYPES_H_

#include <stdint.h>


//================================================================
//
//              Library return codes
//
//=================================================================

/**
 * @brief return codes
 *
 * All functions return value of mli_status enumeration type to indicate if there was an error.
 *
 */
typedef enum _mli_status{
    MLI_STATUS_OK = 0,                /**< No error occurred */
    MLI_STATUS_BAD_TENSOR,            /**< Invalid tensor is passed to the function */
    MLI_STATUS_SHAPE_MISMATCH,        /**< Shape of tensors are not compatible for the function */
    MLI_STATUS_INCOMPATEBLE_TENSORS,
    MLI_STATUS_BAD_FUNC_CFG,         /**< Not valid configuration structure is passed */
    MLI_STATUS_NOT_ENGH_MEM,         /**< Capacity of output tensor is not enough for function result */
    MLI_STATUS_NOT_SUPPORTED,        /**< Function is not yet implemented, or inputs combinations is not supported */
    MLI_STATUS_SPEC_PARAM_MISMATCH,

    MLI_STATUS_ARGUMENT_ERROR,
    MLI_STATUS_LENGTH_ERROR,
    MLI_STATUS_SIZE_MISMATCH,

    MLI_STATUS_RANK_MISMATCH,
    MLI_STATUS_TYPE_MISMATCH,
    MLI_STATUS_MEM_BANK_MISMATCH,
    MLI_STATUS_MISALIGNMENT_ERROR,
    /* other return codes*/

    MLI_STATUS_LARGE_ENUM = 0x02000000  /**< Utility field. Prevent size optimization of public enums */
} mli_status;

//================================================================
//
//              Basic Data Types Definition
//
//=================================================================

#define MLI_MAX_RANK  (4)   /**< Maximum tensor rank (number of dimensions) supported by the library */
/**
 * @brief Tensor's basic element type and it's parameters
 *
 * Defines basic element type stored in tensor structure.
 * Based on this information library functions may define sizes,
 * algorithms for processing, and other  implementation specific things.
 */
typedef enum {
    MLI_EL_FX_4  = 0x4,   /**< 4 bit depth fixed point data with configurable number 
                               of fractional bits Data container is int8_t*/
    MLI_EL_FX_8  = 0x8,   /**< 8 bit depth fixed point data with configurable number 
                               of fractional bits Data container is int8_t*/
    MLI_EL_FX_16 = 0x10,  /**< 16 bit depth fixed point data with configurable number 
                               of fractional bits Data container is int16_t*/
    MLI_EL_SA_8  = 0x108, /**< 8 bit asymetrical signed data with configurable zero offset vector
                               and multiplier vector. Data container is int8_t */
    MLI_EL_SA_32 = 0x120, /**< 32 bit asymetrical signed data with configurable zero offset vector
                               and multiplier vector. Data container is int32_t */
    MLI_EL_FP_16 = 0x210,
    MLI_EL_FP_32 = 0x220,
    MLI_EL_LARGE_ENUM = 0x02000000      /**< Utility field. Prevent size optimization of public enums */
} mli_element_type;

typedef enum {
    MLI_EL_PARAM_SC16_ZP16 = 0, /**< element params have 16bit scale values and 16bit zeropoint */
    MLI_EL_PARAM_LARGE_ENUM = 0x02000000      /**< Prevent size optimization of public enums */
} mli_el_param_type;

/**
 * @brief Container union to represent polymorphic data.
 *
 * Stores pointer to data or a single value that intend to be directly used in arithmetical operations.
 *
 * NOTE: As compiler pointer to XY memory (or another fast memory) is a separate type, it should be somehow reflected if we are going to
 * use it in tensor type.
 */
typedef struct _mli_data_container {
    uint32_t capacity;
    union {
        int32_t* pi32;
        int16_t* pi16;
        int8_t*  pi8;
        float*   pf32;
        int32_t  i32;
        int16_t  i16;
        int8_t   i8;
        float    f32;
        void*    void_p;
    } mem;
} mli_data_container;

/**
 * @brief type parameters for arithmetical operations with tensor elements.
 *
 * Stores data type parameters required for arithmetical operations with tensor elements.
 * These parameters wrapped into union for future library extensibility but current version
 * supports only fixed point data with configurable number of fractional bits.
 * The union can be interpreted only as this structure.
 */
typedef union _mli_element_params {
    struct {
        int8_t frac_bits; /**< Number of fractional bits */
    } fx;

    struct {
        mli_el_param_type type;
        mli_data_container zero_point;  /**< 16bit signed zero point offset. Single value for all data in tensor if dim < 0 
                                        or pointer to an array of zero points regarding configured dimension (dim) otherwise.
                                        In case of array it's size can be looked up in the shape using the dimension to which the scales apply*/
        mli_data_container scale;       /** 16bit signed scale factors. Single value for all data in tensor if dim < 0 
                                        or pointer to an array of scale factors regarding configured dimension (dim) otherwise.
                                        In case of array it's size can be looked up in the shape using the dimension to which the scales apply*/
        int32_t dim;               /**< dimension of the tensor to which the array's of quantization parameters apply */
        mli_data_container scale_frac_bits;     /**< number of fractional bits in the elements of the scales array */
    } sa;
} mli_element_params;


/**
 * @brief Tensor type - main data container for all ML API algorithms
 *
 * Tensor is the main container type for all input and output data which must be processed by ML algo-rithm.
 * In general data for neural networks and other machine learning tasks is a multi-dimensional arrays of some
 * particular shape. So tensor structure includes not only data, but it’s shape, it’s type, and other data specific
 * parameters. To be more precise, saying “data” we mean input features, out-put features, layer weights and biases
 * but not layer parameters like padding or stride for convolution-al layers.
 */
typedef struct _mli_tensor {

    mli_data_container data;   /**< main data. Layer cast this pointer to actual type (XY ptr for L1) */

    int32_t mem_stride[MLI_MAX_RANK]; /**< Array with the distance (in elements) to the next element in the same dimension.
                                         To compute the size in bytes, the number of elements needs to be multiplied by the bytes per element.
                                         For example, for a matrix A[rows][columns], mem_stride[0] contains the distance
                                         to the next element (=1 in this example), and mem_stride[1] contains the distance from
                                         one row to the next (=columns in this example). The size of the array is defined by MLI_MAX_RANK. */

    uint32_t shape[MLI_MAX_RANK];   /**< Array with tensors dimensions. Dimensions must be stored in direct order
                                         starting from least changing one. For example:
                                         for matrix of shape [rows][columns], shape[0] = rows and shape[1] = columns */
    uint32_t rank;                  /**< Tensors rank with amount of dimensions. Must be smaller or equal MLI_MAX_RANK value */

    mli_element_type el_type;       /**< Type of elements stored in tensor */
    mli_element_params el_params;   /**< Parameters of elements stored in tensor. */
} mli_tensor;


/**
 * Lookup table config definition
 */
typedef struct _mli_lut{
    mli_data_container data;
    mli_element_type type;
    int32_t length;
    int32_t in_frac_bits;
    int32_t out_frac_bits;
    int32_t input_offset;
    int32_t output_offset;
} mli_lut;


//================================================================
//
//              Layers Configurations Definition
//
//=================================================================


/**
 * @brief RELU layer config definition
 *
 * enum used for selection of the type of ReLu activation function
 */
typedef enum {
    MLI_RELU_NONE = 0,  /**< no ReLu activation */
    MLI_RELU_GEN,       /**< General ReLU with output range [0, MAX_VAL]*/
    MLI_RELU_1,         /**< ReLU with output range [-1, 1] */
    MLI_RELU_6,         /**< ReLU with output range [0, 6] */
    MLI_RELU_LARGE_ENUM = 0x02000000      /**< Utility field. Prevent size optimization of public enums */
} mli_relu_type;



/**
 * @brief RELU layer config
 *
 * configuration struct to store the ReLu type
 */
typedef struct {
    mli_relu_type type;
} mli_relu_cfg;


/**
 * @brief Parametric RELU layer config
 *
 * Configuration struct to provide axis along which the function will be applied
 */
typedef struct {
    int32_t axis; /**< An axis along which the function will be computed.
                       Axis corresponds to index of tensor`s dimension starting from 0.
                       For instance, having future map in HWC layout, axis == 0 corresponds to H dimension.
                       If axis < 0 the function will be applied to the whole tensor */
} mli_prelu_cfg;

/**
 * @brief Softmax Layer config
 *
 * Configuration struct alias to store required parameter (axis)
 */
typedef mli_prelu_cfg mli_softmax_cfg;

/**
 * @brief L2 Normalization Layer config
 *
 * Configuration struct alias to store required parameter (axis)
 */
typedef mli_prelu_cfg mli_l2_normalize_cfg;

/**
 * @brief Fully Connected Layer config definition
 *
 * Data structure to provide the configuration for a Fully Connected function.
 */
typedef struct {
    mli_relu_cfg relu; /**< Type of ReLU activation applied to output values.*/
} mli_fully_connected_cfg;



/**
 * @brief Convolutional layer config definition
 *
 * Data structure to provide the configuration for a 2D convolution function.
 * The stride parameters can be used to get a subsampled output. (by stepping through the input)
 */
typedef struct {
    mli_relu_cfg relu;/**< Type of ReLU activation applied to output values.*/
    uint8_t stride_width; /**< Stride (step) of filter across width dimension of input.*/
    uint8_t stride_height;/**< Stride (step) of filter across height dimension of input.*/
    uint8_t padding_left; /**< Number of zero points implicitly added to the left side of input (width dimension).*/
    uint8_t padding_right;/**< Number of zero points implicitly added to the right side of input (width dimension).*/
    uint8_t padding_top;  /**< Number of zero points implicitly added to the upper side of input (height dimension).*/
    uint8_t padding_bottom;/**< Number of zero points implicitly added to the bottom side of input (height dimension).*/
    uint8_t dilation_width;  /**< If set to k>1, there will be k-1 implicitly added zero points between each
                                  filter point across width dimension. If set to 0 or 1, no dilation logic is used*/
    uint8_t dilation_height; /**< If set to k>1, there will be k-1 implicitly added zero points between each
                                  filter point across height dimension. If set to 0 or 1, no dilation logic is used*/
} mli_conv2d_cfg;



/**
 * @brief Pooling layer config definition
 *
 * Data structure to provide the configuration for pooling primitives.
 */
typedef struct {
    uint8_t kernel_width;  /**< Width for pooling function applying */
    uint8_t kernel_height; /**< Height for pooling function applying */
    uint8_t stride_width;  /**< Step to next pooling in width dimension of input*/
    uint8_t stride_height; /**< Step to next pooling in height dimension of input*/
    uint8_t padding_left; /**< Number of points implicitly added to the left side of input (width dimension).*/
    uint8_t padding_right;/**< Number of points implicitly added to the right side of input (width dimension).*/
    uint8_t padding_top;  /**< Number of points implicitly added to the upper side of input (height dimension).*/
    uint8_t padding_bottom;/**< Number of points implicitly added to the bottom side of input (height dimension).*/
} mli_pool_cfg;

/**
 * @brief Recurent layers processing mode definition
 *
 * enum used for selection of the type of processing mode for LSTM and Basic RNN primitives
 */
typedef enum {
    RNN_ONE_TO_ONE = 0,     /**< One-to-one mode. Process input tensor as single input frame */
    RNN_BATCH_TO_BATCH,     /**< Batch-to-batch mode. Process input tensor as sequence of frames to produce a sequence of outputs of the same size */
    RNN_BATCH_TO_LAST,      /**< Batch-to-last mode. Process input tensor as sequence of frames to produce a single (last in the sequence) output */
    RNN_MODE_LARGE_ENUM = 0x02000000      /**< Utility field. Prevent size optimization of public enums */
} mli_rnn_mode;



/**
 * @brief Recurent layers output activation definition
 *
 * enum used for selection of the type of output activation for LSTM and Basic RNN primitives
 */
typedef enum {
    RNN_ACT_NONE = 0,   /**< No activation.*/
    RNN_ACT_TANH,       /**< Hyperbolic tangent activation.*/
    RNN_ACT_SIGM,       /**< sigmoid (logistic) activation function.*/
    RNN_ACT_LARGE_ENUM = 0x02000000      /**< Utility field. Prevent size optimization of public enums */
} mli_rnn_out_activation;



/**
 * @brief Recurent layers direction definition
 *
 * enum used for selection of the input sequence processing direction for RNN, LSTM and GRU
 */
typedef enum {
    RNN_DIR_FORWARD = 0,   /**< Process input in forward direction.*/
    RNN_DIR_BACKWARD       /**< Process output in backward direction.*/
} mli_rnn_direction;



/**
 * @brief Recurent layers output mode definition
 *
 * enum used for selection of the type of output for RNN, LSTM and GRU
 */
typedef enum {
    RNN_OUT_LAST = 0,   /**< Preserve only the last result.*/
    RNN_OUT_ALL         /**< Preserve result of each iteration.*/
} mli_rnn_results;



/**
 * @brief Recurrent layers config definition
 *
 * Data structure to provide the configuration for LSTM and Basic RNN primitives.
 */
typedef struct {
    mli_rnn_mode mode;              /**< Recurrent layer processing mode.*/
    mli_rnn_out_activation act;     /**< Output activation type.*/
    mli_tensor *ir_tsr;             /**< Pointer to tensor for holding intermediate results. */
} mli_rnn_cell_cfg_depr;



/**
 * @brief RNN dense layer config definition
 *
 * Data structure to provide the configuration for RNN dense primitive.
 */
typedef struct {
    uint8_t inputs_num;   /**< Number of input tensors.*/
} mli_rnn_dense_cfg;



/**
 * @brief Recurrent layers config definition
 *
 * Data structure to provide the configuration for RNN, LSTM and GRU primitives.
 */
typedef struct {
    mli_rnn_direction direction;        /**< Input processing direction.*/
    mli_rnn_results results;            /**< Results to preserve.*/
    mli_rnn_out_activation act;         /**< Output activation type. */
    mli_data_container scratch_data;    /**< Container to keep intermediate results. */
} mli_rnn_cell_cfg;



/**
 * @brief Permute layer config definition
 *
 * Data structure to provide the permutation order to functions.
 */
typedef struct {
    uint8_t perm_dim[MLI_MAX_RANK];   /**< A permutation array. Dimensions order for output tensor. */
} mli_permute_cfg;



/**
 * @brief Padding2D layer config definition
 *
 * Data structure to provide the configuration for padding2D primitives.
 */
typedef struct {
    uint8_t padding_left;  /**< Number of zero points to be added to the left side of input (width dimension).*/
    uint8_t padding_right; /**< Number of zero points to be added to the right side of input (width dimension).*/
    uint8_t padding_top;   /**< Number of zero points to be added to the upper side of input (height dimension).*/
    uint8_t padding_bottom;/**< Number of zero points to be added to the bottom side of input (height dimension).*/
} mli_padding2d_cfg;



/**
 * @brief Concatenation layer config definition
 *
 * Data structure to provide the configuration for Concatenation primitives.
 */
typedef struct {
    uint8_t tensors_num;   /**< Number of tensors to concatenate (number of pointers in “inputs” array) */
    uint8_t axis;          /**< Axis for concatenation (dimension number starting from 0)*/
} mli_concat_cfg;


/**
 * @brief Point-to-subtensor helper config
 *
 * Data structure to provide coordinates and size of required subtensor in the input tensor
 */
typedef struct {
    uint32_t start_coord[MLI_MAX_RANK];   /**< subtensor start coodinates in the input tensor */
    uint8_t coord_num;                    /**< number of coodrdinates in the array */
    uint8_t first_out_dim_size;           /**< First output dimension size */
} mli_point_to_subtsr_cfg;

/**
 * @brief Create Subtensor helper config
 *
 * Data structure to provide coordinates and sizes of required subtensor in the input tensor
 * The size can be reduced in any dimension.
 */
typedef struct {
    uint32_t offset[MLI_MAX_RANK];   /**< subtensor start coordinates in the input tensor 
                                          The size of this array is determined by the rank of the input tensor */
    uint32_t size[MLI_MAX_RANK];     /**< Size of the sub tensor in elements per dimension
                                          the number of entries in this array is determind by the input tensor */
    uint32_t sub_tensor_rank;        /**< Rank of the sub tensor that will be produced */
} mli_sub_tensor_cfg;

/**
 * @brief Data movement layer config definition
 *
 * Data structure to provide the configuration for data movement operations.
 */
typedef struct _mli_mov_cfg {
    uint32_t offset[MLI_MAX_RANK];
    uint32_t size[MLI_MAX_RANK]; // if zero, compute from input and other parameters
    uint32_t sub_sample_step[MLI_MAX_RANK];
    uint32_t dst_offset[MLI_MAX_RANK];
    int32_t  dst_mem_stride[MLI_MAX_RANK]; // if zero, compute from input and other parameters
    uint8_t  perm_dim[MLI_MAX_RANK];
    uint8_t  padding_pre[MLI_MAX_RANK];
    uint8_t  padding_post[MLI_MAX_RANK];
} mli_mov_cfg_t;

/**
 * @brief Argmax helper config
 *
 * Data structure to provide axis along which the function will be computed in the input tensor and
 * number of indexes per slice to be returned.
 */
typedef struct {
    int32_t axis;   /**< An axis along which the function will be computed. Axis corresponds to index of tensor’s
                         dimension starting from 0. For instance, having future map in HWC layout, axis == 0
                         corresponds to H dimension. If axis < 0 the function will be applied to the whole tensor. */
    int32_t topk;   /**< Number of indexes per slice to be returned.*/
} mli_argmax_cfg;

#endif // _MLI_TYPES_H_
