/**
 * @file SpB_mxv.c
 * @wangluhan (wangluhan21s@ict.ac.cn)
 * @brief SpB_mxv primitive
 * @version 0.1
 * @date 2022-10-08
 *
 * @copyright Copyright (c) 2022
 *
 */
#include "SpB_mxv.h"
#include "OpenSparseBLAS.h"
#include "SpB_Vector.h"
// #include "SpB_mxv_accum.h"
#include "SpB_Error.h"
/**
 * @ingroup SpB_mxv
 * @brief Matrix multiplication vector
 * @param[out] y The result vector
 * @param[in] A The left input matrix
 * @param[in] x The right input vector
 * @return
 * <pre>
 *  SpB_SUCCESS : This primitive is executed correctly.
 *  SpB_FAILURE : This primitive runs with an error.
 * </pre>
 */
// SpB_Info SpB_mxv(
//     SpB_Vector y,
//     const SpB_Matrix A,
//     const SpB_Vector x,
//     void *VNEC)
// {
//     // SpB_Info info = SpB_FAILURE;
// }