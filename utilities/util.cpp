
#include "OpenSparseBLAS.h"
#include "utilities/util.hpp"
#include <fstream>
#include <iostream>

int mm_read_banner(FILE *f, MM_type_info *matcode)
{
    char line[MM_MAX_LINE_LENGTH];
    char banner[MM_MAX_TOKEN_LENGTH];
    char mtx[MM_MAX_TOKEN_LENGTH];
    char crd[MM_MAX_TOKEN_LENGTH];
    char data_type[MM_MAX_TOKEN_LENGTH];
    char storage_scheme[MM_MAX_TOKEN_LENGTH];
    char *p;

    mm_clear_typecode(matcode);

    if (fgets(line, MM_MAX_LINE_LENGTH, f) == NULL)
        return MM_PREMATURE_EOF;

    if (sscanf(line, "%s %s %s %s %s", banner, mtx, crd, data_type,
               storage_scheme) != 5)
        return MM_PREMATURE_EOF;

    /* convert to lower case */
    for (p = mtx; *p != '\0'; *p = tolower(*p), p++)
    {
    }
    for (p = crd; *p != '\0'; *p = tolower(*p), p++)
    {
    }
    for (p = data_type; *p != '\0'; *p = tolower(*p), p++)
    {
    }
    for (p = storage_scheme; *p != '\0'; *p = tolower(*p), p++)
    {
    }

    /* check for banner */
    if (strncmp(banner, MatrixMarketBanner, strlen(MatrixMarketBanner)) != 0)
        return MM_NO_HEADER;

    /* first field should be "mtx" */
    if (strcmp(mtx, MM_MTX_STR) != 0)
        return MM_UNSUPPORTED_TYPE;
    mm_set_matrix(matcode);

    /* second field describes whether this is a sparse matrix (in coordinate
       storage) or a dense array */
    if (strcmp(crd, MM_SPARSE_STR) == 0)
        mm_set_sparse(matcode);
    else if (strcmp(crd, MM_DENSE_STR) == 0)
        mm_set_dense(matcode);
    else
        return MM_UNSUPPORTED_TYPE;

    /* third field */
    if (strcmp(data_type, MM_REAL_STR) == 0)
        mm_set_real(matcode);
    else if (strcmp(data_type, MM_COMPLEX_STR) == 0)
        mm_set_complex(matcode);
    else if (strcmp(data_type, MM_PATTERN_STR) == 0)
        mm_set_pattern(matcode);
    else if (strcmp(data_type, MM_INT_STR) == 0)
        mm_set_integer(matcode);
    else
        return MM_UNSUPPORTED_TYPE;

    /* fourth field */
    if (strcmp(storage_scheme, MM_GENERAL_STR) == 0)
        mm_set_general(matcode);
    else if (strcmp(storage_scheme, MM_SYMM_STR) == 0)
        mm_set_symmetric(matcode);
    else if (strcmp(storage_scheme, MM_HERM_STR) == 0)
        mm_set_hermitian(matcode);
    else if (strcmp(storage_scheme, MM_SKEW_STR) == 0)
        mm_set_skew(matcode);
    else
        return MM_UNSUPPORTED_TYPE;

    return 0;
}

int mm_read_mtx_crd_size(FILE *f, SpB_Index *M, SpB_Index *N, SpB_Index *nz)
{
    char line[MM_MAX_LINE_LENGTH];
    int num_items_read;

    /* set return null parameter values, in case we exit with errors */
    *M = *N = *nz = 0;

    /* now continue scanning until you reach the end-of-comments */
    do
    {
        if (fgets(line, MM_MAX_LINE_LENGTH, f) == NULL)
            return MM_PREMATURE_EOF;
    } while (line[0] == '%');

    /* line[] is either blank or has M,N, nz */
    if (sscanf(line, "%lu %lu %lu", M, N, nz) == 3)
    {
        return 0;
    }
    else
    {
        do
        {
            num_items_read = fscanf(f, "%lu %lu %lu", M, N, nz);
            if (num_items_read == EOF)
                return MM_PREMATURE_EOF;
        } while (num_items_read != 3);
    }

    return 0;
}

int writeVec(const char *fname,
             int *v, SpB_Index n)
{
    std::ofstream out_file;
    out_file.open(fname, std::ios::out);
    if (!out_file.is_open())
    {
        std::cerr << "Error : Can't open the file \"" << fname << "\"" << std::endl;
        return -1;
    }
    out_file << "%%MatrixMarket matrix coordinate integer general\n";

    out_file << static_cast<SpB_Index>(1) << " " << n << " " << n << "\n";

    for (SpB_Index i = 0; i < n; ++i)
    {
        out_file << static_cast<SpB_Index>(1) << " " << i + 1 << " " << v[i] << "\n";
    }
    return 0;
}