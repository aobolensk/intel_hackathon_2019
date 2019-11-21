#include <iostream>
#include <omp.h>
#include <mutex>
#include "task.h"

void performQueries(int32_t nRows, int32_t nCols, int32_t nQueries, int32_t nRes, double* data, int32_t* queries, double* result)
{
    const int NUMBER_OF_THREADS = omp_get_max_threads();
    omp_lock_t lock;
    omp_init_lock(&lock);

    // std::cout << "number of threads: " << NUMBER_OF_THREADS << std::endl;

    #pragma omp parallel num_threads(NUMBER_OF_THREADS)
    {
        int tid = omp_get_thread_num();
        double *localResult = new double[sizeof(double) * nRes * nRes];
        int32_t start = nQueries * tid / omp_get_num_threads();
        int32_t finish = nQueries * (tid + 1) / omp_get_num_threads();
        // std::cout << "tid: " << tid << " start: " << start << " fin: " << finish << std::endl;

        #if 0
        for (int32_t queryId = start; queryId < finish; queryId++)
        {
            int32_t rowA = queries[queryId * 4 + 0];
            int32_t colA = queries[queryId * 4 + 1];
            int32_t rowB = queries[queryId * 4 + 2];
            int32_t colB = queries[queryId * 4 + 3];

            #pragma omp parallel for collapse(2)
            for (int32_t row = 0; row < nRes; row++)
            {
                for (int32_t col = 0; col < nRes; col++)
                {
                    int32_t cRowA = rowA + row;
                    int32_t cColA = colA + col;
                    int32_t cRowB = rowB + row;
                    int32_t cColB = colB + col;

                    localResult[row * nRes + col] += data[cRowA * nCols + cColA] * data[cRowB * nCols + cColB];
                }
            }
        }
        #else
        for (int32_t queryId = start; queryId < finish; queryId++)
        {
            int32_t rowA = queries[queryId * 4 + 0];
            int32_t colA = queries[queryId * 4 + 1];
            int32_t rowB = queries[queryId * 4 + 2];
            int32_t colB = queries[queryId * 4 + 3];

            #pragma omp parallel for
            for (int32_t row = 0; row < nRes; row++) {
                int32_t resultIndex = row * nRes;
                int32_t dataIndex1 = (rowA + row) * nCols + colA;
                int32_t dataIndex2 = (rowB + row) * nCols + colB;
                for (int32_t i = 0; i < nRes; ++i) {
                    localResult[resultIndex++] += data[dataIndex1++] * data[dataIndex2++];
                }
            }
        }
        #endif

        omp_set_lock(&lock);
        // std::cout << "copy start: " << tid << std::endl;
        #pragma omp parallel for
        for (int32_t i = 0; i < nRes * nRes; ++i) {
            result[i] += localResult[i];
        }
        omp_unset_lock(&lock);
        #pragma omp barrier
    }

    omp_destroy_lock(&lock);
}