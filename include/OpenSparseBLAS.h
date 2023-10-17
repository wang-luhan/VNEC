#pragma once
#include "stdint.h"
#include "stdbool.h"

#ifdef __STDC_VERSION__
// ANSI C17: 201710L
// ANSI C11: 201112L
// ANSI C99: 199901L
// ANSI C95: 199409L
#define SpB_STDC_VERSION __STDC_VERSION__
#else
// assume ANSI C90 / C89
#define SpB_STDC_VERSION 199001L
#endif

#ifdef __cplusplus
extern "C"
{
#endif

#define SpB_MAX_NAME_LENGTH 128
    /**
     * @defgroup core
     * @brief OpenSparseBLAS objects
     * @details Definition and basic operation of OpenSparseBLAS objects.
     * @{
     */

    /**
     * @brief The usual return value of the OpenSparseBLAS methods
     */
    typedef enum
    {
        SpB_SUCCESS = 0,  ///< Indicates successful method execution
        SpB_FAILURE = 1,  ///< Indicates method execution failure
        SpB_NO_VALUE = -1 ///< Using the extract related method, the zero element is not stored in the object and the value is returned.
    } SpB_Info;

    /**
     * @brief Built-in data types supported by OpenSparseBLAS
     */
    typedef enum
    {
        SpB_NULL_TYPE,
        SpB_BOOL,   ///< in C: bool
        SpB_INT8,   ///< in C: int8_t
        SpB_INT16,  ///< in C: int16_t
        SpB_INT32,  ///< in C: int32_t
        SpB_INT64,  ///< in C: int64_t
        SpB_UINT8,  ///< in C: uint8_t
        SpB_UINT16, ///< in C: uint16_t
        SpB_UINT32, ///< in C: uint32_t
        SpB_UINT64, ///< in C: uint64_t
        SpB_FP32,   ///< in C: float
        SpB_FP64,   ///< in C: double
        SpB_Type_N
    } SpB_Type;

    /**
     * @brief Built-in descriptors supported by OpenSparseBLAS
     */
    typedef enum
    {
        SpB_DESC_NULL,                        ///< Do not perform any other operations
        SpB_REPLACE = 0b1,                    ///< Clear the output object before assigning computed values.
        SpB_STRUCTURE = 0b10,                 ///< The write mask is constructed from the structure (pattern of stored values) of the associated object. The stored values are not examined.
        SpB_COMP = 0b100,                     ///< Use the complement of the associated object.
        SpB_TRAN0 = 0b1000,                   ///< Transpose the input object 0
        SpB_TRAN1 = 0b10000,                  ///< Transpose the input object 1
        SpB_DESC_RC = SpB_REPLACE + SpB_COMP, ///< \link SpB_REPLACE \endlink + \link SpB_COMP \endlink
        SpB_DESC_T0 = SpB_TRAN0,              ///< \link SpB_TRAN0 \endlink
        N_DESCRIPTOR                          ///< Number of SpB_Descriptor
    } SpB_Descriptor;

    /**
     * @brief Built-in binary operator supported by OpenSparseBLAS
     */
    typedef enum
    {
        SpB_BINARYOP_NULL,    ///< NULL operator
        SpB_BINARYOP_BOOL_ST, ///< the START mark for BOOL BinaryOp
        SpB_LOR,              ///< logical OR
        SpB_LAND,             ///< logical AND
        SpB_LXOR,             ///< logical XOR
        SpB_LXNOR,            ///< logical XNOR
        SpB_BINARYOP_BOOL_ED, ///< the END mark for BOOL BinaryOp
        SpB_BINARYOP_INT8_ST, ///< the START mark for INT8 BinaryOp
        SpB_PLUS_INT8,
        SpB_MINUS_INT8,
        SpB_DIV_INT8,          ///< INT8 division
        SpB_BINARYOP_INT8_ED,  ///< the END mark for INT8 BinaryOp
        SpB_BINARYOP_INT16_ST, ///< the START mark for INT16 BinaryOp
        SpB_PLUS_INT16,
        SpB_MINUS_INT16,
        SpB_DIV_INT16,         ///< INT16 division
        SpB_BINARYOP_INT16_ED, ///< the END mark for INT16 BinaryOp
        SpB_BINARYOP_INT32_ST, ///< the START mark for INT32 BinaryOp
        SpB_PLUS_INT32,        ///< INT32 addition
        SpB_MINUS_INT32,
        SpB_DIV_INT32,         ///< INT32 division
        SpB_BINARYOP_INT32_ED, ///< the END mark for INT32 BinaryOp
        SpB_BINARYOP_INT64_ST, ///< the START mark for INT64 BinaryOp
        SpB_PLUS_INT64,
        SpB_MINUS_INT64,
        SpB_DIV_INT64,         ///< INT64 division
        SpB_BINARYOP_INT64_ED, ///< the END mark for INT64 BinaryOp
        SpB_BINARYOP_UINT8_ST, ///< the START mark for UINT8 BinaryOp
        SpB_PLUS_UINT8,
        SpB_MINUS_UINT8,
        SpB_DIV_UINT8,          ///< UINT8 division
        SpB_BINARYOP_UINT8_ED,  ///< the END mark for UINT8 BinaryOp
        SpB_BINARYOP_UINT16_ST, ///< the START mark for UINT16 BinaryOp
        SpB_PLUS_UINT16,
        SpB_MINUS_UINT16,
        SpB_DIV_UINT16,         ///< UINT16 division
        SpB_BINARYOP_UINT16_ED, ///< the END mark for UINT16 BinaryOp
        SpB_BINARYOP_UINT32_ST, ///< the START mark for UINT32 BinaryOp
        SpB_PLUS_UINT32,
        SpB_MINUS_UINT32,
        SpB_DIV_UINT32,         ///< UINT32 division
        SpB_BINARYOP_UINT32_ED, ///< the END mark for UINT32 BinaryOp
        SpB_BINARYOP_UINT64_ST, ///< the START mark for UINT64 BinaryOp
        SpB_PLUS_UINT64,
        SpB_MINUS_UINT64,
        SpB_DIV_UINT64,         ///< UINT64 division
        SpB_BINARYOP_UINT64_ED, ///< the END mark for UINT64 BinaryOp
        SpB_BINARYOP_FP32_ST,   ///< the START mark for FP32 BinaryOp
        SpB_PLUS_FP32,          ///< FP32 addition
        SpB_TIMES_FP32,         ///< FP32 multiplication
        SpB_DIV_FP32,           ///< FP32 division
        SpB_MAX_FP32,           ///< FP32 MAX
        SpB_MINUS_FP32,         ///< FP32 MINUS
        SpB_BINARYOP_FP32_ED,   ///< the END mark for FP32 BinaryOp
        SpB_BINARYOP_FP64_ST,   ///< the START mark for FP64 BinaryOp
        SpB_PLUS_FP64,
        SpB_MINUS_FP64,
        SpB_DIV_FP64,         ///< FP64 division
        SpB_BINARYOP_FP64_ED, ///< the END mark for FP64 BinaryOp
    } SpB_BinaryOp;
    // typedef struct GB_BinaryOp_opaque *SpB_BinaryOp ;

    /**
     * @brief built-in unary operators supported by OpenSparseBLAS, z = f(x)
     */
    typedef enum
    {
        SpB_UNARYOP_NULL,      ///< NULL operator
        SpB_UNARYOP_BOOL_ST,   ///< the START mark for BOOL UnaryOp
        SpB_ABS_BOOL,          ///< z = abs(x)
        SpB_UNARYOP_BOOL_ED,   ///< the END mark for BOOL UnaryOp
        SpB_UNARYOP_INT8_ST,   ///< the START mark for INT8 UnaryOp
        SpB_ABS_INT8,          ///< z = abs(x)
        SpB_UNARYOP_INT8_ED,   ///< the END mark for INT8 UnaryOp
        SpB_UNARYOP_INT16_ST,  ///< the START mark for INT16 UnaryOp
        SpB_ABS_INT16,         ///< z = abs(x)
        SpB_UNARYOP_INT16_ED,  ///< the END mark for INT16 UnaryOp
        SpB_UNARYOP_INT32_ST,  ///< the START mark for INT32 UnaryOp
        SpB_ABS_INT32,         ///< z = abs(x)
        SpB_UNARYOP_INT32_ED,  ///< the END mark for INT32 UnaryOp
        SpB_UNARYOP_INT64_ST,  ///< the START mark for INT64 UnaryOp
        SpB_ABS_INT64,         ///< z = abs(x)
        SpB_UNARYOP_INT64_ED,  ///< the END mark for INT64 UnaryOp
        SpB_UNARYOP_UINT8_ST,  ///< the START mark for UINT8 UnaryOp
        SpB_ABS_UINT8,         ///< z = abs(x)
        SpB_UNARYOP_UINT8_ED,  ///< the END mark for UINT8 UnaryOp
        SpB_UNARYOP_UINT16_ST, ///< the START mark for UINT16 UnaryOp
        SpB_ABS_UINT16,        ///< z = abs(x)
        SpB_UNARYOP_UINT16_ED, ///< the END mark for UINT16 UnaryOp
        SpB_UNARYOP_UINT32_ST, ///< the START mark for UINT32 UnaryOp
        SpB_ABS_UINT32,        ///< z = abs(x)
        SpB_UNARYOP_UINT32_ED, ///< the END mark for UINT32 UnaryOp
        SpB_UNARYOP_UINT64_ST, ///< the START mark for UINT64 UnaryOp
        SpB_ABS_UINT64,        ///< z = abs(x)
        SpB_UNARYOP_UINT64_ED, ///< the END mark for UINT64 UnaryOp
        SpB_UNARYOP_FP32_ST,   ///< the START mark for FP32 UnaryOp
        SpB_ABS_FP32,          ///< z = abs(x)
        SpB_MINV_FP32,         ///< z = 1/x
        SpB_AINV_FP32,         ///< z = -x
        SpB_UNARYOP_FP32_ED,   ///< the END mark for FP32 UnaryOp
        SpB_UNARYOP_FP64_ST,   ///< the START mark for FP64 UnaryOp
        SpB_ABS_FP64,          ///< z = abs(x)
        SpB_UNARYOP_FP64_ED,   ///< the END mark for FP64 UnaryOp
        // SpB_DIV_FP32,      ///< z = x/y
        // SpB_TIMES_FP32,    ///< z = x*y
        // SpB_MINUS_FP32,    ///< z = x-y
        // SpB_PLUS_FP32,     ///< z = x+y
    } SpB_UnaryOp;
    /**
     * @brief Built-in Monoid operator supported by OpenSparseBLAS
     */
    typedef enum
    {
        SpB_MONOID_NULL,
        SpB_MONOID_BOOL_ST,
        SpB_LOR_MONOID_BOOL,  ///< logical OR
        SpB_LAND_MONOID_BOOL, ///<  logical AND
        SpB_LXOR_MONOID_BOOL, ///< logical OR
        SpB_MONOID_BOOL_ED,
        SpB_MONOID_INT8_ST,
        SpB_PLUS_MONOID_INT8,
        SpB_MONOID_INT8_ED,
        SpB_MONOID_INT16_ST,
        SpB_PLUS_MONOID_INT16,
        SpB_MONOID_INT16_ED,
        SpB_MONOID_INT32_ST,
        SpB_PLUS_MONOID_INT32,
        SpB_MONOID_INT32_ED,
        SpB_MONOID_INT64_ST,
        SpB_PLUS_MONOID_INT64,
        SpB_MONOID_INT64_ED,
        SpB_MONOID_UINT8_ST,
        SpB_PLUS_MONOID_UINT8,
        SpB_MONOID_UINT8_ED,
        SpB_MONOID_UINT16_ST,
        SpB_PLUS_MONOID_UINT16,
        SpB_MONOID_UINT16_ED,
        SpB_MONOID_UINT32_ST,
        SpB_PLUS_MONOID_UINT32,
        SpB_MONOID_UINT32_ED,
        SpB_MONOID_UINT64_ST,
        SpB_PLUS_MONOID_UINT64,
        SpB_MONOID_UINT64_ED,
        SpB_MONOID_FP32_ST,
        SpB_PLUS_MONOID_FP32,
        SpB_MONOID_FP32_ED,
        SpB_MONOID_FP64_ST,
        SpB_PLUS_MONOID_FP64,
        SpB_MONOID_FP64_ED
    } SpB_Monoid;

    /**
     * @brief Built-in Semiring operator supported by OpenSparseBLAS
     */
    typedef enum
    {
        SpB_LOR_LAND_SEMIRING_BOOL,
        SpB_PLUS_TIMES_SEMIRING_INT32,
        SpB_PLUS_TIMES_SEMIRING_FP32,
        SpB_PLUS_SECOND_SEMIRING_FP32,
        SpB_PLUS_ONE_SEMIRING_INT64,
        N_SEMIRING ///< Number of Semiring
    } SpB_Semiring;

    /**
     * @brief Type of index objects used in OpenSparseBLAS
     * @details It define by :
     * @code{.c}
     * typedef unsigned long int SpB_Index;
     * @endcode
     * The range of valid values for a variable of type SpB_Index is [0, SpB_INDEX_MAX] where the largest index value permissible is defined with a macro, SpB_INDEX_MAX.
     */
    typedef unsigned long int SpB_Index;
    /**
     * @brief A method argument literal to indicate that all indices of an input array should be used.
     *
     */
    extern SpB_Index *SpB_ALL;

/**
 * @brief The largest SpB_Index value permissible
 */
#define SpB_INDEX_MAX ((SpB_Index)0x0fffffffffffffff)

    /**
     * @brief OpenGrapblas initialization function
     * @details An instance of the GraphBLAS C API implementation as seen by an application. An application can have only one context between the start and end of the application. A context begins with the first thread that calls \link SpB_init \endlink and ends with the first thread to call \link SpB_finalize \endlink. It is an error for \link SpB_init \endlink or \link SpB_finalize \endlink to be called more than one time within an application.
     */
    void SpB_init();
    /**
     * @brief OpenGrapblas finalization  function
     * @details An instance of the GraphBLAS C API implementation as seen by an application. An application can have only one context between the start and end of the application. A context begins with the first thread that calls \link SpB_init \endlink and ends with the first thread to call \link SpB_finalize \endlink. It is an error for \link SpB_init \endlink or \link SpB_finalize \endlink to be called more than one time within an application.
     */
    void SpB_finalize();

    /**
     * @defgroup SpB_Scalar
     * @brief SpB_Scalar class
     * @{
     */

    /**
     * @brief SpB_Scalar_opaque
     */
    struct SpB_Scalar_opaque;

    /**
     * @brief One scalar which can store any SpB_Type element.
     */
    typedef struct SpB_Scalar_opaque *SpB_Scalar;

    /**
     * @brief Creates a new empty scalar with specified domain.
     * @param[in,out] s On successful return, contains a handle to the newly created OpenSparseBLAS scalar.
     * @param[in] d The type corresponding to the domain of the scalar being created.
     * @return SpB_Info
     */
    SpB_Info SpB_Scalar_new(SpB_Scalar *s, SpB_Type type);

    /**
     * @brief Destroys a previously created SpB_Scalar object and releases any resources associated with the object.
     *
     * @param[in,out] s An existing SpB_Vector object to be destroyed. The object must have been created by an explicit call to a \link SpB_Scalar_new \endlink constructor. On successful completion of SpB_Scalar_free, obj behaves as an uninitialized object.
     * @return SpB_Info
     */
    SpB_Info SpB_Scalar_free(SpB_Scalar *s);

    ///@}

    /**
     * @defgroup SpB_Vector
     * @brief SpB_Vector class
     * @details SpB_Vector is inherited as a base class by \link SpB_Vector_Sparse \endlink and \link SpB_Vector_Dense \endlink. They represent the storage of vector elements in sparse and dense formats, respectively.
     * @{
     */

    /**
     * @brief SpB_Vector_opaque
     */
    struct SpB_Vector_opaque;

    /**
     * @brief One-dimensional collection of elements; can be sparse.
     */
    typedef struct SpB_Vector_opaque *SpB_Vector;

    /**
     * @brief A Null SpB_Vector
     * @details It is used for the mask argument to call a none-mask vxm or mxv operation.
     */
    extern SpB_Vector SpB_Vector_NULL;

    /**
     * @brief Creates a new vector with specified domain and size
     * @param[in,out] v On successful return, contains a handle to the newly created OpenSparseBLAS vector
     * @param[in] type The type corresponding to the domain of the vector being created. Can be one of the predefined GraphBLAS types in \link SpB_Type \endlink.
     * @param[in] n The size of the vector being created.
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_new(SpB_Vector *v, SpB_Type type, SpB_Index n);

    /**
     * @brief Destroys a previously created SpB_Vector object and releases any resources associated with the object.
     *
     * @param[in,out] v An existing SpB_Vector object to be destroyed. The object must have been created by an explicit call to a \link SpB_Vector_new \endlink constructor. On successful completion of SpB_Vector_free, obj behaves as an uninitialized object.
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_free(SpB_Vector *v);

    /**
     * @brief Creates a new vector with the same domain, size, and contents as another vector
     * @param[in,out] v1 successful return, contains a handle to the newly created OpenSparseBLAS vector.
     * @param[in] v2 The OpenSparseBLAS vector to be duplicated.
     */
    SpB_Info SpB_Vector_dup(SpB_Vector *v1, SpB_Vector v2);

    /**
     * @brief Retrieve the number of stored elements (tuples) in a vector.
     * @param[out] nvals On successful return, this is set to the number of stored elements (tuples) in the vector.
     * @param[in] v An existing GraphBLAS vector being queried.
     */
    SpB_Info SpB_Vector_nvals(SpB_Index *nvals, SpB_Vector v);

    //------------------------------------------------------------------------------
    // SpB_Vector_build
    //------------------------------------------------------------------------------

    // SpB_Vector_build:  C = sparse (I,X), but using any
    // associative operator to assemble duplicate entries.

    /**
     * @anchor SpB_Vector_build
     * @name SpB_Vector_build
     * @brief Store elements from tuples into a vetctor
     * @details
     * @brief Store elements from tuples into a vetctor
     *
     * @param[in,out] w An existing Vector object to store the result.
     * @param[in] indices Pointer to an array of indices.
     * @param[in] values Pointer to an array of scalars of a type that is compatible with the domain of vector **w**.
     * @param[in] n The number of entries contained in each array (the same for indices and values).
     * @param[in] dup An associative and commutative binary operator to apply when duplicate values for the same location are present in the input arrays. All three domains of **dup** must be the same. If dup is SpB_BINARYOP_NULL, then duplicate locations will result in an error.
     * @return SpB_Info
     */
    ///@{
    /**
     * @brief Store BOOL elements from tuples into a matrix
     * @details Please refer to \ref SpB_Vector_build for function introduction details
     */
    extern SpB_Info SpB_Vector_build_BOOL(
        SpB_Vector w,
        const SpB_Index *indices,
        const bool *values,
        SpB_Index n,
        const SpB_BinaryOp dup);

    /**
     * @brief Store INT32 elements from tuples into a matrix
     * @details Please refer to \ref SpB_Vector_build for function introduction details
     */
    extern SpB_Info SpB_Vector_build_INT32(
        SpB_Vector w,
        const SpB_Index *indices,
        const int32_t *values,
        SpB_Index n,
        const SpB_BinaryOp dup);
    ///@}

    //------------------------------------------------------------------------------
    // SpB_Vector_setElement
    //------------------------------------------------------------------------------

    // Set a single scalar in a vector, w(i) = x, typecasting from the type of x to
    // the type of w as needed.

    /**
     * @anchor SpB_Vector_setElement
     * @name SpB_Vector_setElement
     * @brief Set one element of a vector to a given value.
     * @param[in,out] w An existing GraphBLAS vector for which an element is to be assigned.
     * @param[in] x Scalar assign. Its domain (type) must be compatible with the domain of w.
     * @param[in] i The location of the element to be assigned.
     * @return SpB_Info
     */
    ///@{
    /**
     * @brief Set a single SpB_Scalar element in a vector
     * @details Please refer to \ref SpB_Vector_setElement for function introduction details
     */
    extern SpB_Info SpB_Vector_setElement_Scalar // w(i) = x
        (
            SpB_Vector w, // vector to modify
            SpB_Scalar s, // scalar to assign to w(i)
            SpB_Index i   // row index
        );
    /**
     * @brief Set a single BOOL element in a vector
     * @details Please refer to \ref SpB_Vector_setElement for function introduction details
     */
    extern SpB_Info SpB_Vector_setElement_BOOL // w(i) = x
        (
            SpB_Vector w, // vector to modify
            bool x,       // scalar to assign to w(i)
            SpB_Index i   // row index
        );

    /**
     * @brief Set a single INT8 element in a vector
     * @details Please refer to \ref SpB_Vector_setElement for function introduction details
     */
    extern SpB_Info SpB_Vector_setElement_INT8 // w(i) = x
        (
            SpB_Vector w, // vector to modify
            int8_t x,     // scalar to assign to w(i)
            SpB_Index i   // row index
        );

    /**
     * @brief Set a single UINT8 element in a vector
     * @details Please refer to \ref SpB_Vector_setElement for function introduction details
     */
    extern SpB_Info SpB_Vector_setElement_UINT8 // w(i) = x
        (
            SpB_Vector w, // vector to modify
            uint8_t x,    // scalar to assign to w(i)
            SpB_Index i   // row index
        );

    /**
     * @brief Set a single INT16 element in a vector
     * @details Please refer to \ref SpB_Vector_setElement for function introduction details
     */
    extern SpB_Info SpB_Vector_setElement_INT16 // w(i) = x
        (
            SpB_Vector w, // vector to modify
            int16_t x,    // scalar to assign to w(i)
            SpB_Index i   // row index
        );

    /**
     * @brief Set a single UINT16 element in a vector
     * @details Please refer to \ref SpB_Vector_setElement for function introduction details
     */
    extern SpB_Info SpB_Vector_setElement_UINT16 // w(i) = x
        (
            SpB_Vector w, // vector to modify
            uint16_t x,   // scalar to assign to w(i)
            SpB_Index i   // row index
        );

    /**
     * @brief Set a single INT32element in a vector
     * @details Please refer to \ref SpB_Vector_setElement for function introduction details
     */
    extern SpB_Info SpB_Vector_setElement_INT32 // w(i) = x
        (
            SpB_Vector w, // vector to modify
            int32_t x,    // scalar to assign to w(i)
            SpB_Index i   // row index
        );

    /**
     * @brief Set a single UINT32 element in a vector
     * @details Please refer to \ref SpB_Vector_setElement for function introduction details
     */
    extern SpB_Info SpB_Vector_setElement_UINT32 // w(i) = x
        (
            SpB_Vector w, // vector to modify
            uint32_t x,   // scalar to assign to w(i)
            SpB_Index i   // row index
        );

    /**
     * @brief Set a single INT64 element in a vector
     * @details Please refer to \ref SpB_Vector_setElement for function introduction details
     */
    extern SpB_Info SpB_Vector_setElement_INT64 // w(i) = x
        (
            SpB_Vector w, // vector to modify
            int64_t x,    // scalar to assign to w(i)
            SpB_Index i   // row index
        );

    /**
     * @brief Set a single UINT64 element in a vector
     * @details Please refer to \ref SpB_Vector_setElement for function introduction details
     */
    extern SpB_Info SpB_Vector_setElement_UINT64 // w(i) = x
        (
            SpB_Vector w, // vector to modify
            uint64_t x,   // scalar to assign to w(i)
            SpB_Index i   // row index
        );

    /**
     * @brief Set a single FP32 element in a vector
     * @details Please refer to \ref SpB_Vector_setElement for function introduction details
     */
    extern SpB_Info SpB_Vector_setElement_FP32 // w(i) = x
        (
            SpB_Vector w, // vector to modify
            float x,      // scalar to assign to w(i)
            SpB_Index i   // row index
        );

    /**
     * @brief Set a single FP64 element in a vector
     * @details Please refer to \ref SpB_Vector_setElement for function introduction details
     */
    extern SpB_Info SpB_Vector_setElement_FP64 // w(i) = x
        (
            SpB_Vector w, // vector to modify
            double x,     // scalar to assign to w(i)
            SpB_Index i   // row index
        );
    ///@}
    // Type-generic version:  x can be any supported C type or void * for a
    // user-defined type.

    /*

    extern
    SpB_Info SpB_Vector_setElement          // w(i) = x
    (
        SpB_Vector w,                       // vector to modify
        <type> x,                           // scalar to assign to w(i)
        SpB_Index i                         // row index
    ) ;

    */

    /* #define SpB_CASE(p, prefix, func, type, T)                    \
        const type p : SpB_TOKEN_PASTING4(prefix, _, func, _##T), \
                       type p : SpB_TOKEN_PASTING4(prefix, _, func, _##T)
    #if SpB_STDC_VERSION >= 201112L
    #define SpB_CASES(p, prefix, func)                   \
        SpB_CASE(p, prefix, func, bool, BOOL),           \
            SpB_CASE(p, prefix, func, int8_t, INT8),     \
            SpB_CASE(p, prefix, func, int16_t, INT16),   \
            SpB_CASE(p, prefix, func, int32_t, INT32),   \
            SpB_CASE(p, prefix, func, int64_t, INT64),   \
            SpB_CASE(p, prefix, func, uint8_t, UINT8),   \
            SpB_CASE(p, prefix, func, uint16_t, UINT16), \
            SpB_CASE(p, prefix, func, uint32_t, UINT32), \
            SpB_CASE(p, prefix, func, uint64_t, UINT64), \
            SpB_CASE(p, prefix, func, float, FP32),      \
            SpB_CASE(p, prefix, func, double, FP64)
    #endif */
    /*
    #if SpB_STDC_VERSION >= 201112L
    #define SpB_Vector_setElement(w, x, i)       \
        _Generic(                                \
            (x),                                 \
            SpB_CASES(, SpB, Vector_setElement), \
            default                              \
            : SpB_Vector_setElement_Scalar)(w, x, i)
    #endif */

    //------------------------------------------------------------------------------
    // SpB_Vector_extractElement
    //------------------------------------------------------------------------------

    // Extract a single entry from a vector, x = v(i), typecasting from the type of
    // v to the type of x as needed.

    /**
     * @anchor SpB_Vector_extractElement
     * @name SpB_Vector_extractElement
     * @brief Extract a single element from a vector.
     * @param[in,out] x An existing scalar of whose domain is compatible with the domain of vector v. On successful return, this scalar holds the result of the extract.
     * @param[in] v The OpenSparseBLAS vector from which an element is extracted.
     * @param[in] i The location in v to extract.
     * @return SpB_Info
     */
    ///@{

    /**
     * @brief Extract a single SpB_Scalar element from a vector.
     * @details Please refer to \ref SpB_Vector_extractElement for function introduction details
     */
    extern SpB_Info SpB_Vector_extractElement_Scalar // x = v(i)
        (
            SpB_Scalar x,       // scalar extracted
            const SpB_Vector v, // vector to extract an entry from
            SpB_Index i         // row index
        );

    /**
     * @brief Extract a single BOOL element from a vector.
     * @details Please refer to \ref SpB_Vector_extractElement for function introduction details
     */
    extern SpB_Info SpB_Vector_extractElement_BOOL // x = v(i)
        (
            bool *x,            // scalar extracted
            const SpB_Vector v, // vector to extract an entry from
            SpB_Index i         // row index
        );

    /**
     * @brief Extract a single INT8 element from a vector.
     * @details Please refer to \ref SpB_Vector_extractElement for function introduction details
     */
    extern SpB_Info SpB_Vector_extractElement_INT8 // x = v(i)
        (
            int8_t *x,          // scalar extracted
            const SpB_Vector v, // vector to extract an entry from
            SpB_Index i         // row index
        );

    /**
     * @brief Extract a single UINT8 element from a vector.
     * @details Please refer to \ref SpB_Vector_extractElement for function introduction details
     */
    extern SpB_Info SpB_Vector_extractElement_UINT8 // x = v(i)
        (
            uint8_t *x,         // scalar extracted
            const SpB_Vector v, // vector to extract an entry from
            SpB_Index i         // row index
        );

    /**
     * @brief Extract a single INT16 element from a vector.
     * @details Please refer to \ref SpB_Vector_extractElement for function introduction details
     */
    extern SpB_Info SpB_Vector_extractElement_INT16 // x = v(i)
        (
            int16_t *x,         // scalar extracted
            const SpB_Vector v, // vector to extract an entry from
            SpB_Index i         // row index
        );

    /**
     * @brief Extract a single UINT16 element from a vector.
     * @details Please refer to \ref SpB_Vector_extractElement for function introduction details
     */
    extern SpB_Info SpB_Vector_extractElement_UINT16 // x = v(i)
        (
            uint16_t *x,        // scalar extracted
            const SpB_Vector v, // vector to extract an entry from
            SpB_Index i         // row index
        );

    /**
     * @brief Extract a single INT32 element from a vector.
     * @details Please refer to \ref SpB_Vector_extractElement for function introduction details
     */
    extern SpB_Info SpB_Vector_extractElement_INT32 // x = v(i)
        (
            int32_t *x,         // scalar extracted
            const SpB_Vector v, // vector to extract an entry from
            SpB_Index i         // row index
        );

    /**
     * @brief Extract a single UINT32 element from a vector.
     * @details Please refer to \ref SpB_Vector_extractElement for function introduction details
     */
    extern SpB_Info SpB_Vector_extractElement_UINT32 // x = v(i)
        (
            uint32_t *x,        // scalar extracted
            const SpB_Vector v, // vector to extract an entry from
            SpB_Index i         // row index
        );

    /**
     * @brief Extract a single INT64 element from a vector.
     * @details Please refer to \ref SpB_Vector_extractElement for function introduction details
     */
    extern SpB_Info SpB_Vector_extractElement_INT64 // x = v(i)
        (
            int64_t *x,         // scalar extracted
            const SpB_Vector v, // vector to extract an entry from
            SpB_Index i         // row index
        );

    /**
     * @brief Extract a single UINT64 element from a vector.
     * @details Please refer to \ref SpB_Vector_extractElement for function introduction details
     */
    extern SpB_Info SpB_Vector_extractElement_UINT64 // x = v(i)
        (
            uint64_t *x,        // scalar extracted
            const SpB_Vector v, // vector to extract an entry from
            SpB_Index i         // row index
        );

    /**
     * @brief Extract a single FP32 element from a vector.
     * @details Please refer to \ref SpB_Vector_extractElement for function introduction details
     */
    extern SpB_Info SpB_Vector_extractElement_FP32 // x = v(i)
        (
            float *x,           // scalar extracted
            const SpB_Vector v, // vector to extract an entry from
            SpB_Index i         // row index
        );

    /**
     * @brief Extract a single FP64 element from a vector.
     * @details Please refer to \ref SpB_Vector_extractElement for function introduction details
     */
    extern SpB_Info SpB_Vector_extractElement_FP64 // x = v(i)
        (
            double *x,          // scalar extracted
            const SpB_Vector v, // vector to extract an entry from
            SpB_Index i         // row index
        );

    ///@}
    // Type-generic version:  x can be a pointer to any supported C type or void *
    // for a user-defined type.

    /*

    extern
    SpB_Info SpB_Vector_extractElement  // x = v(i)
    (
        <type> *x,                      // scalar extracted
        const SpB_Vector v,             // vector to extract an entry from
        SpB_Index i                     // row index
    ) ;

    */
    /* #if SpB_STDC_VERSION >= 201112L
    #define SpB_Vector_extractElement(x, v, i)        \
        _Generic(                                     \
            (x),                                      \
            SpB_CASES(*, SpB, Vector_extractElement), \
            default                                   \
            : SpB_Vector_extractElement_Scalar)(x, v, i)
    #endif */

    //------------------------------------------------------------------------------
    // SpB_Vector_extractTuples
    //------------------------------------------------------------------------------

    // Extracts all tuples from a vector, like [I,~,X] = find (v).  If
    // any parameter I and/or X is NULL, then that component is not extracted.  For
    // example, to extract just the row indices, pass I as non-NULL, and X as NULL.
    // This is like [I,~,~] = find (v).

    /**
     * @anchor SpB_Vector_extractTuples
     * @name SpB_Vector_extractTuples
     * @brief Extract tuples from a vector
     * @param[out] I Pointer to an array of indices that is large enough to hold all of the stored values’ indices.
     * @param[out] X Pointer to an array of scalars of a type that is large enough to hold all of the stored values whose type is compatible with D(v).
     * @param[in] nvals Pointer to a value indicating (on input) the number of elements the values and indices arrays can hold. Upon return, it will contain the number of values written to the arrays.
     * @param[in] v An existing OpenSparseBLAS vector.
     * @return SpB_Info
     */
    ///@{
    /**
     * @brief Extract the contents of a OpenSparseBLAS vector into non-opaque data structures.
     * @details Please refer to \ref SpB_Vector_extractTuples for function introduction details
     */
    extern SpB_Info SpB_Vector_extractTuples_BOOL // [I,~,X] = find (v)
        (
            SpB_Index *I,      // array for returning row indices of tuples
            bool *X,           // array for returning values of tuples
            SpB_Index *nvals,  // I, X size on input; # tuples on output
            const SpB_Vector v // vector to extract tuples from
        );

    /**
     * @brief Extract the contents of a OpenSparseBLAS vector into non-opaque data structures.
     * @details Please refer to \ref SpB_Vector_extractTuples for function introduction details
     */
    extern SpB_Info SpB_Vector_extractTuples_INT8 // [I,~,X] = find (v)
        (
            SpB_Index *I,      // array for returning row indices of tuples
            int8_t *X,         // array for returning values of tuples
            SpB_Index *nvals,  // I, X size on input; # tuples on output
            const SpB_Vector v // vector to extract tuples from
        );
    /**
     * @brief Extract the contents of a OpenSparseBLAS vector into non-opaque data structures.
     * @details Please refer to \ref SpB_Vector_extractTuples for function introduction details
     */
    extern SpB_Info SpB_Vector_extractTuples_UINT8 // [I,~,X] = find (v)
        (
            SpB_Index *I,      // array for returning row indices of tuples
            uint8_t *X,        // array for returning values of tuples
            SpB_Index *nvals,  // I, X size on input; # tuples on output
            const SpB_Vector v // vector to extract tuples from
        );
    /**
     * @brief Extract the contents of a OpenSparseBLAS vector into non-opaque data structures.
     * @details Please refer to \ref SpB_Vector_extractTuples for function introduction details
     */
    extern SpB_Info SpB_Vector_extractTuples_INT16 // [I,~,X] = find (v)
        (
            SpB_Index *I,      // array for returning row indices of tuples
            int16_t *X,        // array for returning values of tuples
            SpB_Index *nvals,  // I, X size on input; # tuples on output
            const SpB_Vector v // vector to extract tuples from
        );
    /**
     * @brief Extract the contents of a OpenSparseBLAS vector into non-opaque data structures.
     * @details Please refer to \ref SpB_Vector_extractTuples for function introduction details
     */
    extern SpB_Info SpB_Vector_extractTuples_UINT16 // [I,~,X] = find (v)
        (
            SpB_Index *I,      // array for returning row indices of tuples
            uint16_t *X,       // array for returning values of tuples
            SpB_Index *nvals,  // I, X size on input; # tuples on output
            const SpB_Vector v // vector to extract tuples from
        );
    /**
     * @brief Extract the contents of a OpenSparseBLAS vector into non-opaque data structures.
     * @details Please refer to \ref SpB_Vector_extractTuples for function introduction details
     */
    extern SpB_Info SpB_Vector_extractTuples_INT32 // [I,~,X] = find (v)
        (
            SpB_Index *I,      // array for returning row indices of tuples
            int32_t *X,        // array for returning values of tuples
            SpB_Index *nvals,  // I, X size on input; # tuples on output
            const SpB_Vector v // vector to extract tuples from
        );
    /**
     * @brief Extract the contents of a OpenSparseBLAS vector into non-opaque data structures.
     * @details Please refer to \ref SpB_Vector_extractTuples for function introduction details
     */
    extern SpB_Info SpB_Vector_extractTuples_UINT32 // [I,~,X] = find (v)
        (
            SpB_Index *I,      // array for returning row indices of tuples
            uint32_t *X,       // array for returning values of tuples
            SpB_Index *nvals,  // I, X size on input; # tuples on output
            const SpB_Vector v // vector to extract tuples from
        );
    /**
     * @brief Extract the contents of a OpenSparseBLAS vector into non-opaque data structures.
     * @details Please refer to \ref SpB_Vector_extractTuples for function introduction details
     */
    extern SpB_Info SpB_Vector_extractTuples_INT64 // [I,~,X] = find (v)
        (
            SpB_Index *I,      // array for returning row indices of tuples
            int64_t *X,        // array for returning values of tuples
            SpB_Index *nvals,  // I, X size on input; # tuples on output
            const SpB_Vector v // vector to extract tuples from
        );
    /**
     * @brief Extract the contents of a OpenSparseBLAS vector into non-opaque data structures.
     * @details Please refer to \ref SpB_Vector_extractTuples for function introduction details
     */
    extern SpB_Info SpB_Vector_extractTuples_UINT64 // [I,~,X] = find (v)
        (
            SpB_Index *I,      // array for returning row indices of tuples
            uint64_t *X,       // array for returning values of tuples
            SpB_Index *nvals,  // I, X size on input; # tuples on output
            const SpB_Vector v // vector to extract tuples from
        );
    /**
     * @brief Extract the contents of a OpenSparseBLAS vector into non-opaque data structures.
     * @details Please refer to \ref SpB_Vector_extractTuples for function introduction details
     */
    extern SpB_Info SpB_Vector_extractTuples_FP32 // [I,~,X] = find (v)
        (
            SpB_Index *I,      // array for returning row indices of tuples
            float *X,          // array for returning values of tuples
            SpB_Index *nvals,  // I, X size on input; # tuples on output
            const SpB_Vector v // vector to extract tuples from
        );
    /**
     * @brief Extract the contents of a OpenSparseBLAS vector into non-opaque data structures.
     * @details Please refer to \ref SpB_Vector_extractTuples for function introduction details
     */
    extern SpB_Info SpB_Vector_extractTuples_FP64 // [I,~,X] = find (v)
        (
            SpB_Index *I,      // array for returning row indices of tuples
            double *X,         // array for returning values of tuples
            SpB_Index *nvals,  // I, X size on input; # tuples on output
            const SpB_Vector v // vector to extract tuples from
        );
    ///@}
    // Type-generic version:  X can be a pointer to any supported C type or void *
    // for a user-defined type.

    /*

    GB_PUBLIC
    SpB_Info SpB_Vector_extractTuples           // [I,~,X] = find (v)
    (
        SpB_Index *I,               // array for returning row indices of tuples
        <type> *X,                  // array for returning values of tuples
        SpB_Index *nvals,           // I, X size on input; # tuples on output
        const SpB_Vector v          // vector to extract tuples from
    ) ;

    */

    /* #if SpB_STDC_VERSION >= 201112L
    #define SpB_Vector_extractTuples(I, X, nvals, v) \
        _Generic(                                    \
            (X),                                     \
            SpB_CASES(*, SpB, Vector_extractTuples))(I, X, nvals, v)
    #endif */

    /**@}*/ // end for SpB_Vector

    /**
     * @defgroup SpB_Matrix
     * @brief SpB_Matrix class
     * @{
     */
    struct SpB_Matrix_opaque;
    /**
     * @brief Two-dimensional collection of elements; typically sparse.
     */
    typedef struct SpB_Matrix_opaque *SpB_Matrix;
    /**
     * @brief A Null SpB_Matrix
     * @details It is used for the mask argument to call a none-mask vxm, mxv or mxm operation.
     */
    extern SpB_Matrix SpB_Matrix_NULL;

    /**
     * @brief Get the data type of a matrix
     *
     * @param[out] t On successful return, contains the result type of the input matrix.
     * @param[in] m An existing OpenSparseBLAS matrix being queried.
     * @return SpB_Info
     */
    SpB_Info SpB_Matrix_type(SpB_Type *t, SpB_Matrix m);
    /**
     * @brief Number of rows in a matrix
     * @details Retrieve the number of rows in a matrix.
     * @param[out] n On successful return, contains the number of rows in the matrix.
     * @param[in] m An existing OpenSparseBLAS matrix being queried.
     * @return SpB_Info
     */
    SpB_Info SpB_Matrix_nrows(SpB_Index *n, SpB_Matrix m);

    /**
     * @brief Number of columns in a matrix
     * @details Retrieve the number of columns in a matrix
     *
     * @param n On successful return, contains the number of columns in the matrix.
     * @param m An existing OpenSparseBLAS matrix being queried.
     * @return SpB_Info
     */
    SpB_Info SpB_Matrix_ncols(SpB_Index *n, SpB_Matrix m);

    /**
     * @brief Construct new matrix
     * @details Creates a new matrix with specified domain and dimensions.
     * @param[in,out] m On successful return, contains a handle to the newly created OpenSparseBLAS matrix.
     * @param[in] type The type corresponding to the domain of the matrix being created. Can be one of the predefined \link SpB_Type \endlink.
     * @param[in] nrows The number of rows of the matrix being created.
     * @param[in] ncols The number of columns of the matrix being created.
     * @return SpB_Info
     */
    SpB_Info SpB_Matrix_new(SpB_Matrix *m, SpB_Type type, SpB_Index nrows, SpB_Index ncols);

    /**
     * @brief Destroy a matrix and release its resources
     * @details Destroys a previously created OpenSparseBLAS matrix object and releases any resources associated with the matrix.
     * @param[in,out] A An existing GraphBLAS matrix to be destroyed. The object must have been created by an explicit call to a OpenSparseBLAS matrix constructor. On successful completion, matrix A behaves as an uninitialized matrix object.
     * @return SpB_Info
     */
    SpB_Info SpB_Matrix_free(SpB_Matrix *A);

    //------------------------------------------------------------------------------
    // SpB_Matrix_build
    //------------------------------------------------------------------------------

    /**
     * @anchor SpB_Matrix_build
     * @name SpB_Matrix_build
     * @brief Store elements from tuples into a matrix
     * @details
     * @brief Store elements from tuples into a matrix
     *
     * @param[in,out] C An existing Matrix object to store the result.
     * @param[in] I Pointer to an array of row indices.
     * @param[in] J Pointer to an array of column indices.
     * @param[in] X Pointer to an array of scalars of a type that is compatible with the domain of matrix, C.
     * @param[in] nvals The number of entries contained in each array (the same for row_indices, col_indices and values).
     * @param[in] dup An associative and commutative binary operator to apply when duplicate values for the sam  location are present in the input arrays.
     * @return SpB_Info
     */
    ///@{
    /**
     * @brief Store BOOL elements from tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_build for function introduction details
     */
    extern SpB_Info SpB_Matrix_build_BOOL // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const bool *X,         // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store INT8 elements from tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_build for function introduction details
     */
    extern SpB_Info SpB_Matrix_build_INT8 // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const int8_t *X,       // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store UINT8 elements from tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_build for function introduction details
     */
    extern SpB_Info SpB_Matrix_build_UINT8 // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const uint8_t *X,      // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store INT16 elements from tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_build for function introduction details
     */
    extern SpB_Info SpB_Matrix_build_INT16 // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const int16_t *X,      // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store UINT16 elements from tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_build for function introduction details
     */
    extern SpB_Info SpB_Matrix_build_UINT16 // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const uint16_t *X,     // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store INT32 elements from tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_build for function introduction details
     */
    extern SpB_Info SpB_Matrix_build_INT32 // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const int32_t *X,      // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store UINT32 elements from tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_build for function introduction details
     */
    extern SpB_Info SpB_Matrix_build_UINT32 // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const uint32_t *X,     // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store INT64 elements from tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_build for function introduction details
     */
    extern SpB_Info SpB_Matrix_build_INT64 // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const int64_t *X,      // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store UINT64 elements from tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_build for function introduction details
     */
    extern SpB_Info SpB_Matrix_build_UINT64 // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const uint64_t *X,     // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store FP32 elements from tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_build for function introduction details
     */
    extern SpB_Info SpB_Matrix_build_FP32 // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const float *X,        // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store FP64 elements from tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_build for function introduction details
     */
    extern SpB_Info SpB_Matrix_build_FP64 // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const double *X,       // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );
    ///@}

    /*

   GB_PUBLIC
   SpB_Info SpB_Matrix_build           // build a matrix from (I,J,X) tuples
   (
       SpB_Matrix C,                   // matrix to build
       const SpB_Index *I,             // array of row indices of tuples
       const SpB_Index *J,             // array of column indices of tuples
       const <type> *X,                // array of values of tuples
       SpB_Index nvals,                // number of tuples
       const SpB_BinaryOp dup          // binary function to assemble duplicates
   ) ;

   */
    /* #if SpB_STDC_VERSION >= 201112L
    #define SpB_Matrix_build(C, I, J, X, nvals, dup) \
        _Generic(                                    \
            (X),                                     \
            SpB_CASES(*, SpB, Matrix_build))(C, I, J, X, nvals, dup)
    #endif */

    /**
       * @anchor SpB_Matrix_fast_build_ext
       * @name SpB_Matrix_fast_build_ext
       * @brief Store elements from sorted tuples into a matrix
       * @details If the incoming tuples are ordered (in order of CSC or CSR format), the matrix created by calling this interface can be created more quickly. This is because the interface can make direct copies of ordered tuples, eliminating the need to sort incoming tuples. \n <b>However, it is worth noting that the sorting rules must be consistent with the format of the GraphBLAS matrix, otherwise it will lead to undefined errors. Passing in unsorted tuples will also result in undefined errors. Therefore, it is recommended to call this interface with caution.</b>

       *
       * @param[in,out] C An existing Matrix object to store the result.
       * @param[in] I Pointer to an array of sorted row indices.
       * @param[in] J Pointer to an array of sorted column indices.
       * @param[in] X Pointer to an array of sorted scalars of a type that is compatible with the domain of matrix, C.
       * @param[in] nvals The number of entries contained in each array (the same for row_indices, col_indices and values).
       * @param[in] dup An associative and commutative binary operator to apply when duplicate values for the sam  location are present in the input arrays.
       * @return SpB_Info
       */
    ///@{
    /**
     * @brief Store BOOL elements from sorted tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_fast_build_ext for function introduction details
     */
    extern SpB_Info SpB_Matrix_fast_build_BOOL_ext // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const bool *X,         // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store INT8 elements from sorted tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_fast_build_ext for function introduction details
     */
    extern SpB_Info SpB_Matrix_fast_build_INT8_ext // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const int8_t *X,       // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store UINT8 elements from sorted tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_fast_build_ext for function introduction details
     */
    extern SpB_Info SpB_Matrix_fast_build_UINT8_ext // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const uint8_t *X,      // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store INT16 elements from sorted tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_fast_build_ext for function introduction details
     */
    extern SpB_Info SpB_Matrix_fast_build_INT16_ext // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const int16_t *X,      // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store UINT16 elements from sorted tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_fast_build_ext for function introduction details
     */
    extern SpB_Info SpB_Matrix_fast_build_UINT16_ext // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const uint16_t *X,     // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store INT32 elements from sorted tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_fast_build_ext for function introduction details
     */
    extern SpB_Info SpB_Matrix_fast_build_INT32_ext // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const int32_t *X,      // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store UINT32 elements from sorted tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_fast_build_ext for function introduction details
     */
    extern SpB_Info SpB_Matrix_fast_build_UINT32_ext // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const uint32_t *X,     // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store INT64 elements from sorted tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_fast_build_ext for function introduction details
     */
    extern SpB_Info SpB_Matrix_fast_build_INT64_ext // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const int64_t *X,      // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store UINT64 elements from sorted tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_fast_build_ext for function introduction details
     */
    extern SpB_Info SpB_Matrix_fast_build_UINT64_ext // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const uint64_t *X,     // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store FP32 elements from sorted tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_fast_build_ext for function introduction details
     */
    extern SpB_Info SpB_Matrix_fast_build_FP32_ext // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const float *X,        // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );

    /**
     * @brief Store FP64 elements from sorted tuples into a matrix
     * @details Please refer to \ref SpB_Matrix_fast_build_ext for function introduction details
     */
    extern SpB_Info SpB_Matrix_fast_build_FP64_ext // build a matrix from (I,J,X) tuples
        (
            SpB_Matrix C,          // matrix to build
            const SpB_Index *I,    // array of row indices of tuples
            const SpB_Index *J,    // array of column indices of tuples
            const double *X,       // array of values of tuples
            SpB_Index nvals,       // number of tuples
            const SpB_BinaryOp dup // binary function to assemble duplicates
        );
    ///@}

    //------------------------------------------------------------------------------
    // SpB_Matrix_setElement
    //------------------------------------------------------------------------------

    // Set a single entry in a matrix, C(i,j) = x, typecasting
    // from the type of x to the type of C, as needed.

    /**
     * @anchor SpB_Matrix_setElement
     * @name  SpB_Matrix_setElement
     * @brief Set a single element in matrix
     * @details Set one element of a matrix to a given value.
     * @param[in,out] C An existing OpenSparseBLAS matrix for which an element is to be assigned.
     * @param[in] x Scalar to assign. Its domain (type) must be compatible with the domain of C.
     * @param[in] i Row index of element to be assigned
     * @param[in] j Column index of element to be assigned
     * @return SpB_Info
     */

    ///@{

    /**
     * @brief Set a single BOOL element in matrix
     * @details Please refer to \ref SpB_Matrix_setElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_setElement_BOOL // C (i,j) = x
        (
            SpB_Matrix C, // matrix to modify
            bool x,       // scalar to assign to C(i,j)
            SpB_Index i,  // row index
            SpB_Index j   // column index
        );

    /**
     * @brief Set a single INT8 element in matrix
     * @details Please refer to \ref SpB_Matrix_setElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_setElement_INT8 // C (i,j) = x
        (
            SpB_Matrix C, // matrix to modify
            int8_t x,     // scalar to assign to C(i,j)
            SpB_Index i,  // row index
            SpB_Index j   // column index
        );

    /**
     * @brief Set a single UINT8 element in matrix
     * @details Please refer to \ref SpB_Matrix_setElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_setElement_UINT8 // C (i,j) = x
        (
            SpB_Matrix C, // matrix to modify
            uint8_t x,    // scalar to assign to C(i,j)
            SpB_Index i,  // row index
            SpB_Index j   // column index
        );

    /**
     * @brief Set a single INT16 element in matrix
     * @details Please refer to \ref SpB_Matrix_setElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_setElement_INT16 // C (i,j) = x
        (
            SpB_Matrix C, // matrix to modify
            int16_t x,    // scalar to assign to C(i,j)
            SpB_Index i,  // row index
            SpB_Index j   // column index
        );

    /**
     * @brief Set a single UINT16 element in matrix
     * @details Please refer to \ref SpB_Matrix_setElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_setElement_UINT16 // C (i,j) = x
        (
            SpB_Matrix C, // matrix to modify
            uint16_t x,   // scalar to assign to C(i,j)
            SpB_Index i,  // row index
            SpB_Index j   // column index
        );

    /**
     * @brief Set a single INT32 element in matrix
     * @details Please refer to \ref SpB_Matrix_setElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_setElement_INT32 // C (i,j) = x
        (
            SpB_Matrix C, // matrix to modify
            int32_t x,    // scalar to assign to C(i,j)
            SpB_Index i,  // row index
            SpB_Index j   // column index
        );
    /**
     * @brief Set a single UINT32 element in matrix
     * @details Please refer to \ref SpB_Matrix_setElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_setElement_UINT32 // C (i,j) = x
        (
            SpB_Matrix C, // matrix to modify
            uint32_t x,   // scalar to assign to C(i,j)
            SpB_Index i,  // row index
            SpB_Index j   // column index
        );
    /**
     * @brief Set a single INT64 element in matrix
     * @details Please refer to \ref SpB_Matrix_setElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_setElement_INT64 // C (i,j) = x
        (
            SpB_Matrix C, // matrix to modify
            int64_t x,    // scalar to assign to C(i,j)
            SpB_Index i,  // row index
            SpB_Index j   // column index
        );
    /**
     * @brief Set a single UINT64 element in matrix
     * @details Please refer to \ref SpB_Matrix_setElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_setElement_UINT64 // C (i,j) = x
        (
            SpB_Matrix C, // matrix to modify
            uint64_t x,   // scalar to assign to C(i,j)
            SpB_Index i,  // row index
            SpB_Index j   // column index
        );

    /**
     * @brief Set a single FP32 element in matrix
     * @details Please refer to \ref SpB_Matrix_setElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_setElement_FP32 // C (i,j) = x
        (
            SpB_Matrix C, // matrix to modify
            float x,      // scalar to assign to C(i,j)
            SpB_Index i,  // row index
            SpB_Index j   // column index
        );

    /**
     * @brief Set a single FP64 element in matrix
     * @details Please refer to \ref SpB_Matrix_setElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_setElement_FP64 // C (i,j) = x
        (
            SpB_Matrix C, // matrix to modify
            double x,     // scalar to assign to C(i,j)
            SpB_Index i,  // row index
            SpB_Index j   // column index
        );
    ///@}
    // Type-generic version:  x can be any supported C type or void * for a
    // user-defined type.

    /*

    GB_PUBLIC
    SpB_Info SpB_Matrix_setElement          // C (i,j) = x
    (
        SpB_Matrix C,                       // matrix to modify
        <type> x,                           // scalar to assign to C(i,j)
        SpB_Index i,                        // row index
        SpB_Index j                         // column index
    ) ;

    */

#if SpB_STDC_VERSION >= 201112L
#define SpB_Matrix_setElement(C, x, i, j) \
    _Generic(                             \
        (x),                              \
        SpB_CASES(, SpB, Matrix_setElement))(C, x, i, j)
#endif

    //------------------------------------------------------------------------------
    // SpB_Matrix_extractElement
    //------------------------------------------------------------------------------

    // Extract a single entry from a matrix, x = A(i,j), typecasting from the type
    // of A to the type of x, as needed.

    /**
     * @anchor SpB_Matrix_extractElement
     * @name SpB_Matrix_extractElement
     * @brief Extract a single element from a matrix
     * @details Extract one element of a matrix into a scalar.
     * @param[in,out] x An existing scalar whose domain is compatible with the domain of matrix A. On successful return, this scalar holds the result of the extract. Any previous value stored in val or s is overwritten.
     * @param[in] A The OpenSparseBLAS matrix from which an element is extracted.
     * @param[in] i The row index of location in A to extract.
     * @param[in] j The column index of location in A to extract.
     * @return SpB_Info
     */

    ///@{

    /**
     * @brief Extract a single SpB_Scalar element from a matrix
     * @details Please refer to \ref SpB_Matrix_extractElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_extractElement_Scalar // x = A(i,j)
        (
            SpB_Scalar x,       // extracted scalar
            const SpB_Matrix A, // matrix to extract a scalar from
            SpB_Index i,        // row index
            SpB_Index j         // column index
        );

    /**
     * @brief Extract a single BOOL element from a matrix
     * @details Please refer to \ref SpB_Matrix_extractElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_extractElement_BOOL // x = A(i,j)
        (
            bool *x,            // extracted scalar
            const SpB_Matrix A, // matrix to extract a scalar from
            SpB_Index i,        // row index
            SpB_Index j         // column index
        );

    /**
     * @brief Extract a single INT8 element from a matrix
     * @details Please refer to \ref SpB_Matrix_extractElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_extractElement_INT8 // x = A(i,j)
        (
            int8_t *x,          // extracted scalar
            const SpB_Matrix A, // matrix to extract a scalar from
            SpB_Index i,        // row index
            SpB_Index j         // column index
        );
    /**
     * @brief Extract a single UINT8 element from a matrix
     * @details Please refer to \ref SpB_Matrix_extractElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_extractElement_UINT8 // x = A(i,j)
        (
            uint8_t *x,         // extracted scalar
            const SpB_Matrix A, // matrix to extract a scalar from
            SpB_Index i,        // row index
            SpB_Index j         // column index
        );
    /**
     * @brief Extract a single INT16 element from a matrix
     * @details Please refer to \ref SpB_Matrix_extractElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_extractElement_INT16 // x = A(i,j)
        (
            int16_t *x,         // extracted scalar
            const SpB_Matrix A, // matrix to extract a scalar from
            SpB_Index i,        // row index
            SpB_Index j         // column index
        );
    /**
     * @brief Extract a single UINT16 element from a matrix
     * @details Please refer to \ref SpB_Matrix_extractElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_extractElement_UINT16 // x = A(i,j)
        (
            uint16_t *x,        // extracted scalar
            const SpB_Matrix A, // matrix to extract a scalar from
            SpB_Index i,        // row index
            SpB_Index j         // column index
        );
    /**
     * @brief Extract a single INT32 element from a matrix
     * @details Please refer to \ref SpB_Matrix_extractElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_extractElement_INT32 // x = A(i,j)
        (
            int32_t *x,         // extracted scalar
            const SpB_Matrix A, // matrix to extract a scalar from
            SpB_Index i,        // row index
            SpB_Index j         // column index
        );
    /**
     * @brief Extract a single UINT32 element from a matrix
     * @details Please refer to \ref SpB_Matrix_extractElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_extractElement_UINT32 // x = A(i,j)
        (
            uint32_t *x,        // extracted scalar
            const SpB_Matrix A, // matrix to extract a scalar from
            SpB_Index i,        // row index
            SpB_Index j         // column index
        );
    /**
     * @brief Extract a single INT64 element from a matrix
     * @details Please refer to \ref SpB_Matrix_extractElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_extractElement_INT64 // x = A(i,j)
        (
            int64_t *x,         // extracted scalar
            const SpB_Matrix A, // matrix to extract a scalar from
            SpB_Index i,        // row index
            SpB_Index j         // column index
        );
    /**
     * @brief Extract a single UINT64 element from a matrix
     * @details Please refer to \ref SpB_Matrix_extractElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_extractElement_UINT64 // x = A(i,j)
        (
            uint64_t *x,        // extracted scalar
            const SpB_Matrix A, // matrix to extract a scalar from
            SpB_Index i,        // row index
            SpB_Index j         // column index
        );
    /**
     * @brief Extract a single FP32 element from a matrix
     * @details Please refer to \ref SpB_Matrix_extractElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_extractElement_FP32 // x = A(i,j)
        (
            float *x,           // extracted scalar
            const SpB_Matrix A, // matrix to extract a scalar from
            SpB_Index i,        // row index
            SpB_Index j         // column index
        );
    /**
     * @brief Extract a single FP64 element from a matrix
     * @details Please refer to \ref SpB_Matrix_extractElement for function introduction details
     */
    extern SpB_Info SpB_Matrix_extractElement_FP64 // x = A(i,j)
        (
            double *x,          // extracted scalar
            const SpB_Matrix A, // matrix to extract a scalar from
            SpB_Index i,        // row index
            SpB_Index j         // column index
        );
    ///@} //end for SpB_Matrix_extractElement
    // Type-generic version:  x can be a pointer to any supported C type or void *
    // for a user-defined type.

    /*

    GB_PUBLIC
    SpB_Info SpB_Matrix_extractElement      // x = A(i,j)
    (
        <type> *x,                          // extracted scalar
        const SpB_Matrix A,                 // matrix to extract a scalar from
        SpB_Index i,                        // row index
        SpB_Index j                         // column index
    ) ;

    */
    /* #if SpB_STDC_VERSION >= 201112L
    #define SpB_Matrix_extractElement(x, A, i, j) \
        _Generic(                                 \
            (x),                                  \
            SpB_CASES(*, SpB, Matrix_extractElement))(x, A, i, j)
    #endif */

    /**@}*/ // end for SpB_Matrix
    /**@}*/ // end for core

    /**
     * @defgroup operator
     * @brief The basic operations of OpenSparseBLAS
     * @details This module contains the basic operations of OpenSparseBLAS,
     *  and they are designed and implemented based on
     * <a href="https://graphblas.org/docs/GraphBLAS_API_C_v2.0.0.pdf">GraphBLAS API 2.0</a> .
     * @{
     */

    /**
     * @defgroup SpB_assign
     * @brief Assign the contents of a subset of a matrix or vector.
     * @details Assign values from one scalar, one \link SpB_Vector \endlink or one \link SpB_Matrix \endlink to a subset of a vector or a matrix as specified by a set of indices. The size of the input vector/matrix is the same size as the index array provided.
     * @{
     */

    /**
     * @brief Assign an BOOL value to a subset of a SpB_Vector as specified by a set of indices.
     * @details The size of the input vector is the same size as the index array provided.
     * @param[in,out] w An existing OpenSparseBLAS vector. On input, the vector provides values that may be accumulated with the result of the assign operation. On output, this vector holds the results of the operation.
     * @param[in] mask An optional "write" mask that controls which results from this operation are stored into the output vector w. The mask dimensions must match those of the vector w
     * @param[in] accum An optional binary operator used for accumulating entries into existing w entries.
     * @param[in] u The value is assigned to a subset of w
     * @param[in] indices Pointer to the ordered set (array) of indices corresponding to the locations in w that are to be assigned. If all elements of w are to be assigned in order from 0 to nindices - 1, then \link SpB_ALL \endlink should be specified.
     * @param[in] nindices The number of values in indices array. Must be equal to size(u).
     * @param[in] desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_assign_BOOL(SpB_Vector w, const SpB_Vector mask, const SpB_BinaryOp accum, const bool u, const SpB_Index *indices, SpB_Index nindices, const SpB_Descriptor desc);

    /**
     * @brief Assign an INT8 value to a subset of a SpB_Vector as specified by a set of indices.
     * @details Please refer to \link SpB_Vector_assign_BOOL \endlink for function introduction details
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_assign_INT8(SpB_Vector, const SpB_Vector, const SpB_BinaryOp, const int8_t u, const SpB_Index *, SpB_Index, const SpB_Descriptor);

    /**
     * @brief Assign an INT16 value to a subset of a SpB_Vector as specified by a set of indices.
     * @details Please refer to \link SpB_Vector_assign_BOOL \endlink for function introduction details
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_assign_INT16(SpB_Vector, const SpB_Vector, const SpB_BinaryOp, const int16_t u, const SpB_Index *, SpB_Index, const SpB_Descriptor);

    /**
     * @brief Assign an INT32 value to a subset of a SpB_Vector as specified by a set of indices.
     * @details Please refer to \link SpB_Vector_assign_BOOL \endlink for function introduction details
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_assign_INT32(SpB_Vector, const SpB_Vector, const SpB_BinaryOp, const int32_t u, const SpB_Index *, SpB_Index, const SpB_Descriptor);

    /**
     * @brief Assign an UINT8 value to a subset of a SpB_Vector as specified by a set of indices.
     * @details Please refer to \link SpB_Vector_assign_BOOL \endlink for function introduction details
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_assign_UINT8(SpB_Vector, const SpB_Vector, const SpB_BinaryOp, const uint8_t u, const SpB_Index *, SpB_Index, const SpB_Descriptor);

    /**
     * @brief Assign an UINT16 value to a subset of a SpB_Vector as specified by a set of indices.
     * @details Please refer to \link SpB_Vector_assign_BOOL \endlink for function introduction details
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_assign_UINT16(SpB_Vector, const SpB_Vector, const SpB_BinaryOp, const uint16_t u, const SpB_Index *, SpB_Index, const SpB_Descriptor);

    /**
     * @brief Assign an UINT32 value to a subset of a SpB_Vector as specified by a set of indices.
     * @details Please refer to \link SpB_Vector_assign_BOOL \endlink for function introduction details
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_assign_UINT32(SpB_Vector, const SpB_Vector, const SpB_BinaryOp, const uint32_t u, const SpB_Index *, SpB_Index, const SpB_Descriptor);

    /**
     * @brief Assign an UINT64 value to a subset of a SpB_Vector as specified by a set of indices.
     * @details Please refer to \link SpB_Vector_assign_BOOL \endlink for function introduction details
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_assign_UINT64(SpB_Vector, const SpB_Vector, const SpB_BinaryOp, const uint64_t u, const SpB_Index *, SpB_Index, const SpB_Descriptor);

    /**
     * @brief Assign an FP32 value to a subset of a SpB_Vector as specified by a set of indices.
     * @details Please refer to \link SpB_Vector_assign_BOOL \endlink for function introduction details
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_assign_FP32(SpB_Vector, const SpB_Vector, const SpB_BinaryOp, const float u, const SpB_Index *, SpB_Index, const SpB_Descriptor);

    /**
     * @brief Assign an FP64 value to a subset of a SpB_Vector as specified by a set of indices.
     * @details Please refer to \link SpB_Vector_assign_BOOL \endlink for function introduction details
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_assign_FP64(SpB_Vector, const SpB_Vector, const SpB_BinaryOp, const double u, const SpB_Index *, SpB_Index, const SpB_Descriptor);

    /**
     * @brief Assign the contents a vector to a subset of elements in one row of a matrix. Note that since the output cannot be transposed, a different variant of assign is provided to assign to a column of a matrix.
     *
     * @param[in,out] C An existing OpenSparseBLAS Matrix. On input, the matrix provides values that may be accumulated with the result of the assign operation. On output, this matrix holds the results of the operation.
     * @param[in] mask An optional "write" mask that controls which results from this operation are stored into the output matrix C. The mask dimensions must match those of the matrix C
     * @param[in] accum An optional binary operator used for accumulating entries into existing w entries.
     * @param[in] u The OpenSparseBLAS vector whose contents are assigned to (a subset of) a row of C.
     * @param[in] row_index The index of the row in C to assign. Must be in the range [0, nrows(C)).
     * @param[in] col_indices Pointer to the ordered set (array) of indices corresponding to the locations in the specified row of C that are to be assigned. If all elements of the row in C are to be assigned in order from index 0 to ncols - 1, then \link SpB_ALL \endlink should be specified.
     * @param[in] ncols The number of values in col_indices array. Must be equal to size(u).
     * @param[in] desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    SpB_Info SpB_Row_assign(SpB_Matrix C, const SpB_Vector mask, const SpB_BinaryOp accum, const SpB_Vector u, SpB_Index row_index, const SpB_Index *col_indices, SpB_Index ncols, const SpB_Descriptor desc);

    /**
     * @brief Assign values from one GraphBLAS vector to a subset of a vector as specified by a set of indices.
     * @details  The size of the input vector is the same size as the index array provided.
     * @param[in,out] w An existing OpenSparseBLAS vector. On input, the vector provides values that may be accumulated with the result of the assign operation. On output, this vector holds the results of the operation.
     * @param[in] mask An optional "write" mask that controls which results from this operation are stored into the output vector w. The mask dimensions must match those of the vector w
     * @param[in] accum An optional binary operator used for accumulating entries into existing w entries.
     * @param[in] u The GraphBLAS vector whose contents are assigned to a subset of w.
     * @param[in] indices Pointer to the ordered set (array) of indices corresponding to the locations in w that are to be assigned. If all elements of w are to be assigned in order from 0 to nindices - 1, then \link SpB_ALL \endlink should be specified.
     * @param[in] nindices The number of values in indices array. Must be equal to size(u).
     * @param[in] desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_assign(SpB_Vector w, const SpB_Vector mask, const SpB_BinaryOp accum, const SpB_Vector u, const SpB_Index *indices, SpB_Index nindices, const SpB_Descriptor desc);
    /**@} */ // end for SpB_assign

    /**
     * @defgroup SpB_extract
     * @brief Extract a subset of a matrix or vector.
     * @{
     */

    /**
     * @brief Extract from one column of a matrix into a vector. Note that with the transpose descriptor for the source matrix, elements of an arbitrary row of the matrix can be extracted with this function as well.
     *
     * @param[in,out] w An existing OpenSparseBLAS vector. On input, the vector provides values that may be accumulated with the result of the extract operation. On output, this vector holds the results of the operation.
     * @param[in] mask An optional "write" mask that controls which results from this operation are stored into the output vector w. The mask dimensions must match those of the vector w
     * @param accum An optional binary operator used for accumulating entries into existing w entries.
     * @param A The OpenSparseBLAS matrix from which the column subset is extracted.
     * @param row_indices Pointer to the ordered set (array) of indices corresponding to the locations within the specified column of A from which elements are extracted. If elements in all rows of A are to be extracted in order, \link SpB_ALL \endlink should be specified.
     * @param nrows The number of indices in the row_indices array. Must be equal to size(w).
     * @param col_index The index of the column of A from which to extract values. It must be in the range [0, ncols(A)).
     * @param desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    SpB_Info SpB_Col_extract(SpB_Vector w,
                             const SpB_Vector mask,
                             const SpB_BinaryOp accum,
                             const SpB_Matrix A,
                             const SpB_Index *row_indices,
                             SpB_Index nrows,
                             SpB_Index col_index,
                             const SpB_Descriptor desc);

    /**@} */ // end for SpB_extract

    /**
     * @defgroup SpB_eWiseAdd
     * @brief Element-wise addition
     * @details The difference between eWiseAdd and eWiseMult is not about the element-wise operation but how the index sets are treated. eWiseAdd returns an object whose indices are the "union" of the indices of the inputs whereas eWiseMult returns an object whose indices are the "intersection" of the indices of the inputs. In both cases, the passed semiring, monoid, or operator operates on the set of values from the resulting index set.
     * @{
     */

    /**
     * @brief Perform element-wise (general) addition on the elements of two vectors, producing a third vector as result.
     * @param[in,out] w An existing OpenSparseBLAS vector. On input, the vector provides values that may be accumulated with the result of the element-wise operation. On output, this vector holds the results of the operation.
     * @param[in] mask An optional "write" mask that controls which results from this operation are stored into the output vector w. The mask dimensions must match those of the vector w. If the \link SpB_STRUCTURE \endlink descriptor is not set for the mask, the domain of the mask vector must be of type bool or any of the predefined "built-in" types \link SpB_Type \endlink. If the default mask is desired (i.e., a mask that is all true with the dimensions of w), \link SpB_Vector_NULL \endlink should be specified.
     * @param accum An optional binary operator used for accumulating entries into existing w entries. If assignment rather than accumulation is desired, \link SpB_BINARYOP_NULL \endlink should be specified.
     * @param[in] op The binary operator used in the element-wise "sum" operation.
     * @param[in] u The OpenSparseBLAS vector holding the values for the left-hand vector in the operation.
     * @param[in] v The OpenSparseBLAS vector holding the values for the right-hand vector in the operation.
     * @param[in] desc An optional operation descriptor. If the description is not needed to assist , \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_eWiseAdd_BinaryOp(SpB_Vector w, const SpB_Vector mask, const SpB_BinaryOp accum, SpB_BinaryOp op, const SpB_Vector u, const SpB_Vector v, const SpB_Descriptor desc);

    /**@}*/ // end for eWiseAdd

    /**
     * @defgroup SpB_eWiseMult
     * @brief Element-wise multiplication
     * @details The difference between eWiseAdd and eWiseMult is not about the element-wise operation but how the index sets are treated. eWiseAdd returns an object whose indices are the "union" of the indices of the inputs whereas eWiseMult returns an object whose indices are the "intersection" of the indices of the inputs. In both cases, the passed semiring, monoid, or operator operates on the set of values from the resulting index set.
     * @{
     */

    /**
     * @brief Perform element-wise (general) multiplication on the intersection of elements of two vectors, producing a third vector as result.
     * @param[in,out] w An existing GraphBLAS vector. On input, the vector provides values that may be accumulated with the result of the element-wise operation. On output, this vector holds the results of the operation.
     * @param[in] mask An optional "write" mask that controls which results from this operation are stored into the output vector w. The mask dimensions must match those of the vector w. If the \link SpB_STRUCTURE \endlink descriptor is not set for the mask, the domain of the mask vector must be of type bool or any of the predefined "built-in" types \link SpB_Type \endlink. If the default mask is desired (i.e., a mask that is all true with the dimensions of w), \link SpB_Vector_NULL \endlink should be specified.
     * @param[in] accum An optional binary operator used for accumulating entries into existing w entries. If assignment rather than accumulation is desired, SpB_NULL should be specified.
     * @param[in] accum The binary operator used in the element-wise "product" operation.
     * @param[in] u The OpenSparseBLAS vector holding the values for the left-hand vector in the operation.
     * @param[in] v The OpenSparseBLAS vector holding the values for the right-hand vector in the operation.
     * @param[in] desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     */
    SpB_Info SpB_Vector_eWiseMult_BinaryOp(SpB_Vector w, const SpB_Vector mask, const SpB_BinaryOp accum, const SpB_BinaryOp op, const SpB_Vector u, const SpB_Vector v, const SpB_Descriptor desc);
    /**@}*/ // end for eWiseMult

    /**
     * @defgroup SpB_vxm
     * @brief Matrix-Vector multiplication module
     * @details This module mainly implements SpB_mxv, whose core operators are SpMSpV and SpMV, both of which are very important for graph algorithms to achieve Pull/Push optimization \cite yang2018implementing.
     * @{
     */

    /**
     * @brief Vector multiplication matrix
     * @param[out] w The result vector
     * @param[in] mask_in The mask vector
     * @param[in] accum The accumulate Binary Operator
     * @param[in] semiring  The semiring
     * @param[in] u The left input vector
     * @param[in] A The right input matrix
     * @param[in] desc The descriptor
     * @return
     * <pre>
     *  SpB_SUCCESS : This primitive is executed correctly.
     *  SpB_FAILURE : This primitive runs with an error.
     * </pre>
     */
    SpB_Info SpB_vxm(SpB_Vector w, SpB_Vector mask_in, SpB_BinaryOp accum, SpB_Semiring semiring, SpB_Vector u, SpB_Matrix A, SpB_Descriptor desc);

    /**@}*/ // end for SpB_vxm

    /**
     * @defgroup SpB_mxv
     * @brief Matrix-Vector multiplication module
     * @details This module mainly implements SpB_mxv, whose core operators are SpMSpV and SpMV, both of which are very important for graph algorithms to achieve Pull/Push optimization \cite yang2018implementing.
     * @{
     */
    // SpB_Info SpB_mxv(SpB_Vector w, const SpB_Vector mask, const SpB_BinaryOp acuum, const SpB_Semiring op, const SpB_Matrix A, const SpB_Vector u, const SpB_Descriptor desc);
    SpB_Info SpB_mxv(SpB_Vector y, const SpB_Matrix A, const SpB_Vector x, void *VNEC);
    /**@}*/ // end for SpB_mxv

    typedef enum
    {
        SpB_VNEC_D = 0,
        SpB_VNEC_S = 1,
    } SpB_VNEC_type;

    typedef struct coord
    {
        int x;
        int y;
    } coord;

    typedef struct VNEC_D_FP32 {
        coord *thread_coord_start;
        coord *thread_coord_end;
        int *ecr_indices;
        int *NEC_NUM;
        int **use_x_indices;
        float **ecr_xx_val;
        int *v_row_ptr;
        int *col_start;
        float * val_align;
    } VNEC_D_FP32;
    void SpMV_VNEC_D_FP32(SpB_Vector y,
                      const SpB_Matrix A,
                      const SpB_Vector x,
                      VNEC_D_FP32 *mat_thd);

    struct VNEC_D_FP32 *Build_VNEC_D_FP32(const SpB_Matrix A);
    SpB_VNEC_type IRD_VNEC(const SpB_Matrix A, SpB_Type type);

    struct csr_f32
    {
        float *nnz;
        int *col, *row_begin, *row_end;
        int *task_start;
        int *task_end;
    };
    typedef struct VNEC_S_FP32
    {
        /*
            ECR
        */
        int *NEC_NUM;
        int **use_x_indices;
        float **ecr_xx_val;
        /*
            vectorization
        */
        int *spvv16_len;
        int **tasks;
        struct csr_f32 reorder_mat;
    } VNEC_S_FP32;
    void SpMV_VNEC_S_FP32(SpB_Vector y,
                      const SpB_Matrix A,
                      const SpB_Vector x,
                      VNEC_S_FP32 *mat_thd);

    struct VNEC_S_FP32 *Build_VNEC_S_FP32(const SpB_Matrix A);

    typedef struct VNEC_L {
        int num_merge_items;
        int items_per_thread;
        int *diagonal_start;
        int *diagonal_end;
        coord *thread_coord_start;
        coord *thread_coord_end;
        int *nz_indices;
    } VNEC_L;
    struct VNEC_L *Build_VNEC_L(const SpB_Matrix A);
    void SpMV_VNEC_L_FP32(SpB_Vector y, const SpB_Matrix A, const SpB_Vector x, VNEC_L *mat_thd);

    
    typedef struct VNEC_D_FP64 {
        coord *thread_coord_start;
        coord *thread_coord_end;
        int *ecr_indices;
        int *NEC_NUM;
        int **use_x_indices;
        double **ecr_xx_val;
        int *v_row_ptr;
        int *col_start;
        double * val_align;
    } VNEC_D_FP64;
    void SpMV_VNEC_D_FP64(SpB_Vector y,
                      const SpB_Matrix A,
                      const SpB_Vector x,
                      VNEC_D_FP64 *mat_thd);

    struct VNEC_D_FP64 *Build_VNEC_D_FP64(const SpB_Matrix A);
    struct csr_f64
    {
        double *nnz;
        int *col, *row_begin, *row_end;
        int *task_start;
        int *task_end;
    };
    typedef struct VNEC_S_FP64
    {
        /*
            ECR
        */
        int *NEC_NUM;
        int **use_x_indices;
        double **ecr_xx_val;
        /*
            vectorization
        */
        int *spvv8_len;
        int **tasks;
        struct csr_f64 reorder_mat;
    } VNEC_S_FP64;
    void SpMV_VNEC_S_FP64(SpB_Vector y,
                      const SpB_Matrix A,
                      const SpB_Vector x,
                      VNEC_S_FP64 *mat_thd);

    struct VNEC_S_FP64 *Build_VNEC_S_FP64(const SpB_Matrix A);

    void SpMV_VNEC_L_FP64(SpB_Vector y, const SpB_Matrix A, const SpB_Vector x, VNEC_L *mat_thd);
    /**
     * @defgroup SpB_mxm
     * @brief Matrix-Matrix multiplication module
     * @details This module mainly implements matrix and matrix multiplication operations. Since in the initial design, we only consider the matrix to be stored using sparse format, its core operator is SpGEMM.
     * @{
     */
    SpB_Info SpB_mxm(SpB_Matrix C, const SpB_Matrix mask, const SpB_BinaryOp accum, const SpB_Semiring op, const SpB_Matrix A, const SpB_Matrix B, const SpB_Descriptor desc);
    /**@}*/ // end for SpB_mxm

    /**
     * @defgroup SpB_reduce
     * @brief Perform a reduction across the elements of an object
     * @details These perform a reduction across a \link SpB_Matrix \endlink or \link SpB_Vector \endlink to produce a \link SpB_Vector \endlink or scalar.
     * @{
     */

    /**
     * @brief Reduce all stored values into a single BOOL scalar.
     * @param[in,out] c Pointer to scalar to store final reduced value into. On input, the scalar provides a value that may be accumulated (optionally) with the result of the reduction operation. On output, this scalar holds the results of the operation.
     * @param[in] accum An optional binary operator used for accumulating entries into an existing scalar c value. If assignment rather than accumulation is desired, \link SpB_BINARYOP_NULL \endlink should be specified.
     * @param[in] monoid The monoid used in the reduction operation. The reduction operator must be commutative and associative; otherwise, the outcome of the operation is undefined.
     * @param[in] u The OpenSparseBLAS vector on which reduction will be performed.
     * @param desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    extern SpB_Info SpB_Vector_reduce_BOOL // c = accum (c, reduce_to_scalar (u))
        (
            bool *c,                  // result scalar
            const SpB_BinaryOp accum, // optional accum for c=accum(c,t)
            const SpB_Monoid monoid,  // monoid to do the reduction
            const SpB_Vector u,       // vector to reduce
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single INT8 scalar
     * @details Please refer to \link SpB_Vector_reduce_BOOL \endlink for function introduction details
     */
    extern SpB_Info SpB_Vector_reduce_INT8 // c = accum (c, reduce_to_scalar (u))
        (
            int8_t *c,                // result scalar
            const SpB_BinaryOp accum, // optional accum for c=accum(c,t)
            const SpB_Monoid monoid,  // monoid to do the reduction
            const SpB_Vector u,       // vector to reduce
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single UINT8 scalar
     * @details Please refer to \link SpB_Vector_reduce_BOOL \endlink for function introduction details
     */
    extern SpB_Info SpB_Vector_reduce_UINT8 // c = accum (c, reduce_to_scalar (u))
        (
            uint8_t *c,               // result scalar
            const SpB_BinaryOp accum, // optional accum for c=accum(c,t)
            const SpB_Monoid monoid,  // monoid to do the reduction
            const SpB_Vector u,       // vector to reduce
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single INT16 scalar
     * @details Please refer to \link SpB_Vector_reduce_BOOL \endlink for function introduction details
     */
    extern SpB_Info SpB_Vector_reduce_INT16 // c = accum (c, reduce_to_scalar (u))
        (
            int16_t *c,               // result scalar
            const SpB_BinaryOp accum, // optional accum for c=accum(c,t)
            const SpB_Monoid monoid,  // monoid to do the reduction
            const SpB_Vector u,       // vector to reduce
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single UINT16 scalar
     * @details Please refer to \link SpB_Vector_reduce_BOOL \endlink for function introduction details
     */
    extern SpB_Info SpB_Vector_reduce_UINT16 // c = accum (c, reduce_to_scalar (u))
        (
            uint16_t *c,              // result scalar
            const SpB_BinaryOp accum, // optional accum for c=accum(c,t)
            const SpB_Monoid monoid,  // monoid to do the reduction
            const SpB_Vector u,       // vector to reduce
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single INT32 scalar
     * @details Please refer to \link SpB_Vector_reduce_BOOL \endlink for function introduction details
     */
    extern SpB_Info SpB_Vector_reduce_INT32 // c = accum (c, reduce_to_scalar (u))
        (
            int32_t *c,               // result scalar
            const SpB_BinaryOp accum, // optional accum for c=accum(c,t)
            const SpB_Monoid monoid,  // monoid to do the reduction
            const SpB_Vector u,       // vector to reduce
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single UINT32 scalar
     * @details Please refer to \link SpB_Vector_reduce_BOOL \endlink for function introduction details
     */
    extern SpB_Info SpB_Vector_reduce_UINT32 // c = accum (c, reduce_to_scalar (u))
        (
            uint32_t *c,              // result scalar
            const SpB_BinaryOp accum, // optional accum for c=accum(c,t)
            const SpB_Monoid monoid,  // monoid to do the reduction
            const SpB_Vector u,       // vector to reduce
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single INT64 scalar
     * @details Please refer to \link SpB_Vector_reduce_BOOL \endlink for function introduction details
     */
    extern SpB_Info SpB_Vector_reduce_INT64 // c = accum (c, reduce_to_scalar (u))
        (
            int64_t *c,               // result scalar
            const SpB_BinaryOp accum, // optional accum for c=accum(c,t)
            const SpB_Monoid monoid,  // monoid to do the reduction
            const SpB_Vector u,       // vector to reduce
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single UINT64 scalar
     * @details Please refer to \link SpB_Vector_reduce_BOOL \endlink for function introduction details
     */
    extern SpB_Info SpB_Vector_reduce_UINT64 // c = accum (c, reduce_to_scalar (u))
        (
            uint64_t *c,              // result scalar
            const SpB_BinaryOp accum, // optional accum for c=accum(c,t)
            const SpB_Monoid monoid,  // monoid to do the reduction
            const SpB_Vector u,       // vector to reduce
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single FP32 scalar
     * @details Please refer to \link SpB_Vector_reduce_BOOL \endlink for function introduction details
     */
    extern SpB_Info SpB_Vector_reduce_FP32 // c = accum (c, reduce_to_scalar (u))
        (
            float *c,                 // result scalar
            const SpB_BinaryOp accum, // optional accum for c=accum(c,t)
            const SpB_Monoid monoid,  // monoid to do the reduction
            const SpB_Vector u,       // vector to reduce
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single FP64 scalar
     * @details Please refer to \link SpB_Vector_reduce_BOOL \endlink for function introduction details
     */
    extern SpB_Info SpB_Vector_reduce_FP64 // c = accum (c, reduce_to_scalar (u))
        (
            double *c,                // result scalar
            const SpB_BinaryOp accum, // optional accum for c=accum(c,t)
            const SpB_Monoid monoid,  // monoid to do the reduction
            const SpB_Vector u,       // vector to reduce
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single BOOL scalar.
     * @param[in,out] c Pointer to scalar to store final reduced value into. On input, the scalar provides a value that may be accumulated (optionally) with the result of the reduction operation. On output, this scalar holds the results of the operation.
     * @param[in] accum An optional binary operator used for accumulating entries into an existing scalar c value. If assignment rather than accumulation is desired, \link SpB_BINARYOP_NULL \endlink should be specified.
     * @param[in] monoid The monoid used in the reduction operation. The reduction operator must be commutative and associative; otherwise, the outcome of the operation is undefined.
     * @param[in] A The OpenSparseBLAS matrix on which reduction will be performed.
     * @param desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    extern SpB_Info SpB_Matrix_reduce_BOOL // c = accum (c, reduce_to_scalar (A))
        (
            bool *c,
            const SpB_BinaryOp accum,
            const SpB_Monoid monoid,
            const SpB_Matrix A,
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single INT8 scalar.
     * @param[in,out] c Pointer to scalar to store final reduced value into. On input, the scalar provides a value that may be accumulated (optionally) with the result of the reduction operation. On output, this scalar holds the results of the operation.
     * @param[in] accum An optional binary operator used for accumulating entries into an existing scalar c value. If assignment rather than accumulation is desired, \link SpB_BINARYOP_NULL \endlink should be specified.
     * @param[in] monoid The monoid used in the reduction operation. The reduction operator must be commutative and associative; otherwise, the outcome of the operation is undefined.
     * @param[in] A The OpenSparseBLAS matrix on which reduction will be performed.
     * @param desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    extern SpB_Info SpB_Matrix_reduce_INT8 // c = accum (c, reduce_to_scalar (A))
        (
            int8_t *c,
            const SpB_BinaryOp accum,
            const SpB_Monoid monoid,
            const SpB_Matrix A,
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single INT16 scalar.
     * @param[in,out] c Pointer to scalar to store final reduced value into. On input, the scalar provides a value that may be accumulated (optionally) with the result of the reduction operation. On output, this scalar holds the results of the operation.
     * @param[in] accum An optional binary operator used for accumulating entries into an existing scalar c value. If assignment rather than accumulation is desired, \link SpB_BINARYOP_NULL \endlink should be specified.
     * @param[in] monoid The monoid used in the reduction operation. The reduction operator must be commutative and associative; otherwise, the outcome of the operation is undefined.
     * @param[in] A The OpenSparseBLAS matrix on which reduction will be performed.
     * @param desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    extern SpB_Info SpB_Matrix_reduce_INT16 // c = accum (c, reduce_to_scalar (A))
        (
            int16_t *c,
            const SpB_BinaryOp accum,
            const SpB_Monoid monoid,
            const SpB_Matrix A,
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single INT32 scalar.
     * @param[in,out] c Pointer to scalar to store final reduced value into. On input, the scalar provides a value that may be accumulated (optionally) with the result of the reduction operation. On output, this scalar holds the results of the operation.
     * @param[in] accum An optional binary operator used for accumulating entries into an existing scalar c value. If assignment rather than accumulation is desired, \link SpB_BINARYOP_NULL \endlink should be specified.
     * @param[in] monoid The monoid used in the reduction operation. The reduction operator must be commutative and associative; otherwise, the outcome of the operation is undefined.
     * @param[in] A The OpenSparseBLAS matrix on which reduction will be performed.
     * @param desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    extern SpB_Info SpB_Matrix_reduce_INT32 // c = accum (c, reduce_to_scalar (A))
        (
            int32_t *c,
            const SpB_BinaryOp accum,
            const SpB_Monoid monoid,
            const SpB_Matrix A,
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single INT64 scalar.
     * @param[in,out] c Pointer to scalar to store final reduced value into. On input, the scalar provides a value that may be accumulated (optionally) with the result of the reduction operation. On output, this scalar holds the results of the operation.
     * @param[in] accum An optional binary operator used for accumulating entries into an existing scalar c value. If assignment rather than accumulation is desired, \link SpB_BINARYOP_NULL \endlink should be specified.
     * @param[in] monoid The monoid used in the reduction operation. The reduction operator must be commutative and associative; otherwise, the outcome of the operation is undefined.
     * @param[in] A The OpenSparseBLAS matrix on which reduction will be performed.
     * @param desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    extern SpB_Info SpB_Matrix_reduce_INT64 // c = accum (c, reduce_to_scalar (A))
        (
            int64_t *c,
            const SpB_BinaryOp accum,
            const SpB_Monoid monoid,
            const SpB_Matrix A,
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single UINT8 scalar.
     * @param[in,out] c Pointer to scalar to store final reduced value into. On input, the scalar provides a value that may be accumulated (optionally) with the result of the reduction operation. On output, this scalar holds the results of the operation.
     * @param[in] accum An optional binary operator used for accumulating entries into an existing scalar c value. If assignment rather than accumulation is desired, \link SpB_BINARYOP_NULL \endlink should be specified.
     * @param[in] monoid The monoid used in the reduction operation. The reduction operator must be commutative and associative; otherwise, the outcome of the operation is undefined.
     * @param[in] A The OpenSparseBLAS matrix on which reduction will be performed.
     * @param desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    extern SpB_Info SpB_Matrix_reduce_UINT8 // c = accum (c, reduce_to_scalar (A))
        (
            uint8_t *c,
            const SpB_BinaryOp accum,
            const SpB_Monoid monoid,
            const SpB_Matrix A,
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single UINT16 scalar.
     * @param[in,out] c Pointer to scalar to store final reduced value into. On input, the scalar provides a value that may be accumulated (optionally) with the result of the reduction operation. On output, this scalar holds the results of the operation.
     * @param[in] accum An optional binary operator used for accumulating entries into an existing scalar c value. If assignment rather than accumulation is desired, \link SpB_BINARYOP_NULL \endlink should be specified.
     * @param[in] monoid The monoid used in the reduction operation. The reduction operator must be commutative and associative; otherwise, the outcome of the operation is undefined.
     * @param[in] A The OpenSparseBLAS matrix on which reduction will be performed.
     * @param desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    extern SpB_Info SpB_Matrix_reduce_UINT16 // c = accum (c, reduce_to_scalar (A))
        (
            uint16_t *c,
            const SpB_BinaryOp accum,
            const SpB_Monoid monoid,
            const SpB_Matrix A,
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single UINT32 scalar.
     * @param[in,out] c Pointer to scalar to store final reduced value into. On input, the scalar provides a value that may be accumulated (optionally) with the result of the reduction operation. On output, this scalar holds the results of the operation.
     * @param[in] accum An optional binary operator used for accumulating entries into an existing scalar c value. If assignment rather than accumulation is desired, \link SpB_BINARYOP_NULL \endlink should be specified.
     * @param[in] monoid The monoid used in the reduction operation. The reduction operator must be commutative and associative; otherwise, the outcome of the operation is undefined.
     * @param[in] A The OpenSparseBLAS matrix on which reduction will be performed.
     * @param desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    extern SpB_Info SpB_Matrix_reduce_UINT32 // c = accum (c, reduce_to_scalar (A))
        (
            uint32_t *c,
            const SpB_BinaryOp accum,
            const SpB_Monoid monoid,
            const SpB_Matrix A,
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single UINT64 scalar.
     * @param[in,out] c Pointer to scalar to store final reduced value into. On input, the scalar provides a value that may be accumulated (optionally) with the result of the reduction operation. On output, this scalar holds the results of the operation.
     * @param[in] accum An optional binary operator used for accumulating entries into an existing scalar c value. If assignment rather than accumulation is desired, \link SpB_BINARYOP_NULL \endlink should be specified.
     * @param[in] monoid The monoid used in the reduction operation. The reduction operator must be commutative and associative; otherwise, the outcome of the operation is undefined.
     * @param[in] A The OpenSparseBLAS matrix on which reduction will be performed.
     * @param desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    extern SpB_Info SpB_Matrix_reduce_UINT64 // c = accum (c, reduce_to_scalar (A))
        (
            uint64_t *c,
            const SpB_BinaryOp accum,
            const SpB_Monoid monoid,
            const SpB_Matrix A,
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single FP32 scalar.
     * @param[in,out] c Pointer to scalar to store final reduced value into. On input, the scalar provides a value that may be accumulated (optionally) with the result of the reduction operation. On output, this scalar holds the results of the operation.
     * @param[in] accum An optional binary operator used for accumulating entries into an existing scalar c value. If assignment rather than accumulation is desired, \link SpB_BINARYOP_NULL \endlink should be specified.
     * @param[in] monoid The monoid used in the reduction operation. The reduction operator must be commutative and associative; otherwise, the outcome of the operation is undefined.
     * @param[in] A The OpenSparseBLAS matrix on which reduction will be performed.
     * @param desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    extern SpB_Info SpB_Matrix_reduce_FP32 // c = accum (c, reduce_to_scalar (A))
        (
            float *c,
            const SpB_BinaryOp accum,
            const SpB_Monoid monoid,
            const SpB_Matrix A,
            const SpB_Descriptor desc);

    /**
     * @brief Reduce all stored values into a single FP64scalar.
     * @param[in,out] c Pointer to scalar to store final reduced value into. On input, the scalar provides a value that may be accumulated (optionally) with the result of the reduction operation. On output, this scalar holds the results of the operation.
     * @param[in] accum An optional binary operator used for accumulating entries into an existing scalar c value. If assignment rather than accumulation is desired, \link SpB_BINARYOP_NULL \endlink should be specified.
     * @param[in] monoid The monoid used in the reduction operation. The reduction operator must be commutative and associative; otherwise, the outcome of the operation is undefined.
     * @param[in] A The OpenSparseBLAS matrix on which reduction will be performed.
     * @param desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    extern SpB_Info SpB_Matrix_reduce_FP64 // c = accum (c, reduce_to_scalar (A))
        (
            double *c,
            const SpB_BinaryOp accum,
            const SpB_Monoid monoid,
            const SpB_Matrix A,
            const SpB_Descriptor desc);

    /**
     * @brief This performs a reduction across rows of a matrix to produce a vector. If reduction down columns is desired, the input matrix should be transposed using the descriptor.
     * @param[in,out] w An existing GraphBLAS vector. On input, the vector provides values that may be accumulated with the result of the reduction operation. On output, this vector holds the results of the operation.
     * @param[in] mask An optional “write” mask that controls which results from this operation are stored into the output vector w.
     * @param[in] accum An optional binary operator used for accumulating entries into an existing scalar c value. If assignment rather than accumulation is desired, \link SpB_BINARYOP_NULL \endlink should be specified.
     * @param[in] monoid The monoid used in the reduction operation. The reduction operator must be commutative and associative; otherwise, the outcome of the operation is undefined.
     * @param[in] A The OpenSparseBLAS matrix on which reduction will be performed.
     * @param desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    extern SpB_Info SpB_Matrix_reduce_Monoid // c = accum (c, reduce_to_vector (A))
        (
            SpB_Vector w,
            const SpB_Vector mask,
            const SpB_BinaryOp accum,
            const SpB_Monoid op,
            const SpB_Matrix A,
            const SpB_Descriptor desc);

    extern SpB_Info SpB_Matrix_reduce_BinaryOp(
        SpB_Vector w,
        const SpB_Vector mask,
        const SpB_BinaryOp accum,
        const SpB_BinaryOp op,
        const SpB_Matrix A,
        const SpB_Descriptor desc);

    /**@}*/ // end for SpB_reduce

    /**
     * @defgroup SpB_apply
     * @brief Computes the transformation of the values of the elements of a vector or a matrix using a unary function, or a binary function where one argument is bound to a scalar.
     * @details Assign values from one scalar, one \link SpB_Vector \endlink or one \link SpB_Matrix \endlink to a subset of a vector or a matrix as specified by a set of indices. The size of the input vector/matrix is the same size as the index array provided.
     * @{
     */

    /**
     * @brief Computes the transformation of the values of the stored elements of a vector using a binary operator and a scalar value.
     * @details  bind-second + scalar value.
     * @param[in,out] w An existing OpenSparseBLAS vector. On input, the vector provides values that may be accumulated with the result of the assign operation. On output, this vector holds the results of the operation.
     * @param[in] mask An optional "write" mask that controls which results from this operation are stored into the output vector w. The mask dimensions must match those of the vector w
     * @param[in] accum An optional binary operator used for accumulating entries into existing w entries.
     * @param[in] op A binary operator applied to each element of input vector, u, and the scalar value, val.
     * @param[in] u The GraphBLAS vector whose elements are passed to the binary operator as the right-hand (second) argument in the bind-first variant, or the left-hand (first) argument in the bind-second variant.
     * @param[in] val Scalar value that is passed to the binary operator as the left-hand (first) argument in the bind-first variant, or the right-hand (second) argument in the bind-second variant.
     * @param[in] desc An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_apply_BinaryOp2nd_INT32(SpB_Vector w, const SpB_Vector mask, const SpB_BinaryOp accum, const SpB_BinaryOp op, const SpB_Vector u, int val, const SpB_Descriptor desc);

    /**
     * @brief The specified scalar value is FP32.
     * @details Please refer to \link SpB_Vector_apply_BinaryOp2nd_INT32 \endlink for function introduction details
     * @return SpB_Info
     */

    SpB_Info SpB_Vector_apply_BinaryOp2nd_FP32(SpB_Vector w, const SpB_Vector mask, const SpB_BinaryOp accum, const SpB_BinaryOp op, const SpB_Vector u, float val, const SpB_Descriptor desc);

    /**
     * @brief Computes the transformation of the values of the elements of a vector using a unary function.
     *
     * @param[in,out] w An existing GraphBLAS vector. On input, the vector provides values that may be accumulated with the result of the apply operation. On output, this vector holds the results of the operation.
     * @param[in] mask  An optional "write" mask that controls which results from this operation are stored into the output matrix C. The mask dimensions must match those of the matrix C
     * @param[in] accum An optional binary operator used for accumulating entries into existing w entries.
     * @param[in] op    A unary operator applied to each element of input vector u.
     * @param[in] u     The GraphBLAS vector to which the unary function is applied.
     * @param[in] desc  An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_apply(SpB_Vector w, const SpB_Vector mask, const SpB_BinaryOp accum, const SpB_UnaryOp op, const SpB_Vector u, const SpB_Descriptor desc);
    /**@} */ // end for SpB_apply



    /**
     * @defgroup SpB_transpose
     * @brief Transpose rows and columns of a matrix.
     * @details This version computes a new matrix that is the transpose of the source matrix.
     * @{
     */

    /**
     * @brief Computes the transformation of the values of the elements of a vector using a unary function.
     *
     * @param[in,out] C An existing GraphBLAS matrix. On input, the matrix provides values that may be accumulated with the result of the transpose operation. On output, the matrix holds the results of the operation.
     * @param[in] mask  An optional “write” mask that controls which results from this operation are stored into the output matrix C.
     * @param[in] accum An optional binary operator used for accumulating entries into existing C entries.
     * @param[in] A     The GraphBLAS matrix on which transposition will be performed.
     * @param[in] desc  An optional operation descriptor. If the description is not needed to assist, \link SpB_DESC_NULL \endlink should be specified.
     * @return SpB_Info
     */
    SpB_Info SpB_transpose(SpB_Matrix C, const SpB_Matrix mask, const SpB_BinaryOp accum, const SpB_Matrix A, const SpB_Descriptor desc);
    /**@} */ // end for SpB_transpose

    /**@}*/ // end for operator

    /**
     * @defgroup Extension
     * @brief Extend the GraphBLAS API with some functions that control the underlying runtime methods.
     * @{
     */

    /**
     * @defgroup ObjectFormat
     * @brief Object format control module
     * @details The following code shows the default format for setting the matrix construction:
     * \code
     * SpB_Object_Format_ext format;
     * format.matrix_format = SpB_CSR;
     * SpB_Global_Option_set_ext(SpB_MATRIX_FORMAT, format);
     * \endcode
     * @{
     */

    /**
     * @brief Object options available for format setting
     *
     */
    typedef enum
    {
        SpB_VECTOR_FORMAT, ///< You can set the format of the vector
        SpB_MATRIX_FORMAT  ///< You can set the format of the matrix
    } SpB_Option_Field_ext;

    /**
     * @Available vector formats
     *
     */
    typedef enum
    {
        SpB_DENSE = 0,            ///< Dense Format: All elements of the vector including the zero element are represented
        SpB_SPARSE = 1,           ///< Sparse Format: The non-zero elements of the vector are represented
        SpB_VECTOR_NO_FORMAT = -1 ///< format not defined (Now useless)
    } SpB_Vector_Format_Value_ext;

    /**
     * @Available matrix formats
     *
     */
    typedef enum
    {
        SpB_CSC = 0,              ///< CSR: compressed sparse row format
        SpB_CSR = 1,              ///< CSC: compressed sparse column format
        SpB_MATRIX_NO_FORMAT = -1 ///< format not defined (Now useless)
    } SpB_Matrix_Format_Value_ext;

    /**
     * @brief The argument of \link SpB_Global_Option_set_ext \endlink
     *
     */
    typedef union
    {
        SpB_Vector_Format_Value_ext vector_format;
        SpB_Matrix_Format_Value_ext matrix_format;
    } SpB_Object_Format_ext;

    /**
     * @brief Function to set the global default object format
     *
     * @param[in] field The field needs to be set
     * @param[in] value The value will be set for this field
     * @return SpB_Info
     */
    SpB_Info SpB_Global_Option_set_ext // set a global default option
        (
            SpB_Option_Field_ext field, // option to change
            SpB_Object_Format_ext value // value to change it to
        );

    /**
     * @brief Creates a new vector with specified domain, size and format.
     *
     * @param[in,out] v On successful return, contains a handle to the newly created OpenSparseBLAS vector.
     * @param[in] type The type corresponding to the domain of the vector being created. Can be one of the predefined \link SpB_Type \enlink.
     * @param[in] n The size of the vector being created.
     * @param[in] format The format of the vector being created.
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_new_format_ext(SpB_Vector *v,
                                       SpB_Type type,
                                       SpB_Index n,
                                       SpB_Vector_Format_Value_ext format);

    /**
     * @brief Get the format of the given vector
     *
     * @param[in] v The OpenSparseBLAS vector which format will be retured
     * @return SpB_Vector_Format_Value_ext
     */
    SpB_Vector_Format_Value_ext SpB_Vector_format_ext(const SpB_Vector v);

    /**
     * @brief Creates a new vector in a way that copies the format of a given vector.
     * @details The new vector format will be the same as the format of the given vector, but will not copy its value.
     * @param out The new vector
     * @param in The given vector
     * @return SpB_Info
     */
    SpB_Info SpB_Vector_copy_format_ext(SpB_Vector *out, const SpB_Vector in);

    /**
     * @brief Creates a new matrix with specified domain, size and format.
     *
     * @param[in,out] v On successful return, contains a handle to the newly created OpenSparseBLAS matrix.
     * @param[in] type The type corresponding to the domain of the matrix being created. Can be one of the predefined \link SpB_Type \enlink.
     * @param[in] row The row number of the matrix being created.
     * @param[in] col The col number of the matrix being created.
     * @param[in] format The format of the matrix being created.
     * @return SpB_Info
     */
    SpB_Info SpB_Matrix_new_format_ext(SpB_Matrix *m,
                                       SpB_Type type,
                                       SpB_Index row,
                                       SpB_Index col,
                                       SpB_Matrix_Format_Value_ext format);

    /**
     * @brief Get the format of the given matrix
     *
     * @param[in] m The OpenSparseBLAS matrix which format will be retured
     * @return SpB_Matrix_Format_Value_ext
     */
    SpB_Matrix_Format_Value_ext SpB_Matrix_format_ext(const SpB_Matrix m);
    /**@}*/ // end for ObjectFormat

    /**
     * @brief Decide whether to print warning message.
     *
     */
    SpB_Info SpB_Warning_switch_set_ext(bool val);
    /**@}*/ // end for Extension

#ifdef __cplusplus
}
#endif

// The declaration of C++ code
#ifdef __cplusplus

#define SpB_VECTOR_EXTRACT_CPP(type, T) \
    SpB_Info SpB_Vector_extractElement(type *x, const SpB_Vector v, SpB_Index i);

SpB_VECTOR_EXTRACT_CPP(bool, BOOL);
SpB_VECTOR_EXTRACT_CPP(int8_t, INT8);
SpB_VECTOR_EXTRACT_CPP(int16_t, INT16);
SpB_VECTOR_EXTRACT_CPP(int32_t, INT32);
SpB_VECTOR_EXTRACT_CPP(int64_t, INT64);
SpB_VECTOR_EXTRACT_CPP(uint8_t, UINT8);
SpB_VECTOR_EXTRACT_CPP(uint16_t, UINT16);
SpB_VECTOR_EXTRACT_CPP(uint32_t, UINT32);
SpB_VECTOR_EXTRACT_CPP(uint64_t, UINT64);
SpB_VECTOR_EXTRACT_CPP(float, FP32);
SpB_VECTOR_EXTRACT_CPP(double, FP64);
#undef SpB_VECTOR_EXTRACT_CPP

#define SpB_VECTOR_EXTRACT_TUPLES_CPP(type, T) \
    SpB_Info SpB_Vector_extractTuples(SpB_Index *I, type *X, SpB_Index *p_nvals, const SpB_Vector v);

SpB_VECTOR_EXTRACT_TUPLES_CPP(bool, BOOL);
SpB_VECTOR_EXTRACT_TUPLES_CPP(int8_t, INT8);
SpB_VECTOR_EXTRACT_TUPLES_CPP(int16_t, INT16);
SpB_VECTOR_EXTRACT_TUPLES_CPP(int32_t, INT32);
SpB_VECTOR_EXTRACT_TUPLES_CPP(int64_t, INT64);
SpB_VECTOR_EXTRACT_TUPLES_CPP(uint8_t, UINT8);
SpB_VECTOR_EXTRACT_TUPLES_CPP(uint16_t, UINT16);
SpB_VECTOR_EXTRACT_TUPLES_CPP(uint32_t, UINT32);
SpB_VECTOR_EXTRACT_TUPLES_CPP(uint64_t, UINT64);
SpB_VECTOR_EXTRACT_TUPLES_CPP(float, FP32);
SpB_VECTOR_EXTRACT_TUPLES_CPP(double, FP64);
#undef SpB_VECTOR_EXTRACT_TUPLES_CPP

#define SpB_VECTOR_SET_CPP(type, T) \
    SpB_Info SpB_Vector_setElement(SpB_Vector v, type x, SpB_Index i);

SpB_VECTOR_SET_CPP(bool, BOOL);
SpB_VECTOR_SET_CPP(int8_t, INT8);
SpB_VECTOR_SET_CPP(int16_t, INT16);
SpB_VECTOR_SET_CPP(int32_t, INT32);
SpB_VECTOR_SET_CPP(int64_t, INT64);
SpB_VECTOR_SET_CPP(uint8_t, UINT8);
SpB_VECTOR_SET_CPP(uint16_t, UINT16);
SpB_VECTOR_SET_CPP(uint32_t, UINT32);
SpB_VECTOR_SET_CPP(uint64_t, UINT64);
SpB_VECTOR_SET_CPP(float, FP32);
SpB_VECTOR_SET_CPP(double, FP64);
#undef SpB_VECTOR_SET_CPP

#endif

/**
 * @defgroup algorithm
 * @brief The basic graph algorithms
 * @details This module contains a series of basic graph algorithms implemented using OpenSparseBLAS.
 *
 */