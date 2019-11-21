#include <iostream>
#include <omp.h>
#include <mutex>
#include <algorithm>
#include "task.h"

struct str {
	int rowA, colA, rowB, colB;
};

bool f(const str& a, const str& b){
	if(a.rowA != b.rowA)
		return a.rowA < b.rowA;
	if(a.colA != b.colA)
		return a.colA < b.colA;
	if(a.rowB != b.rowB)
		return a.rowB < b.rowB;
	return a.colB < b.colB;
}

void performQueries(int32_t nRows, int32_t nCols, int32_t nQueries, int32_t nRes, double* data, int32_t* queries, double* result)
{
	str* queri = (str *)queries;
	std::sort(queri, queri+nQueries, f);
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
        for (int32_t queryId = start; queryId < finish; queryId++)
        {
            int32_t rowA = queri[queryId].rowA;
            int32_t colA = queri[queryId].colA;
            int32_t rowB = queri[queryId].rowB;
            int32_t colB = queri[queryId].colB;

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

        omp_set_lock(&lock);
        // std::cout << "copy start: " << tid << std::endl;
        #pragma omp parallel for
        for (int32_t i = 0; i < nRes * nRes; ++i) {
            result[i] += localResult[i];
        }
        omp_unset_lock(&lock);
    }

    omp_destroy_lock(&lock);
}