#include "task.h"

void performQueries(int32_t nRows, int32_t nCols, int32_t nQueries, int32_t nRes, double* data, int32_t* queries, double* result)
{
    // Reference unoptimized code, replace it with your solution

    for (int32_t queryId = 0; queryId < nQueries; queryId++)
    {
        int32_t rowA = queries[queryId * 4 + 0];
        int32_t colA = queries[queryId * 4 + 1];
        int32_t rowB = queries[queryId * 4 + 2];
        int32_t colB = queries[queryId * 4 + 3];

        #pragma omp parallel for collapse(2)
        for (int32_t col = 0; col < nRes; col++)
        {
            for (int32_t row = 0; row < nRes; row++)
            {
                int32_t cRowA = rowA + row;
                int32_t cColA = colA + col;
                int32_t cRowB = rowB + row;
                int32_t cColB = colB + col;

                result[row * nRes + col] += data[cRowA * nCols + cColA] * data[cRowB * nCols + cColB];
            }
        }
    }

    // Reference unoptimized code, replace it with your solution
}