#include <stdlib.h>
#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "task.h"

void performQueries(int32_t nRows, int32_t nCols, int32_t nQueries, int32_t nRes, double* data, int32_t* queries, double* result)
{
    // Reference unoptimized code, replace it with your solution

    int pipefd[2];
    pid_t cpid;

    pipe(pipefd);
    cpid = fork();

    if (cpid == 0) // child
    {
        for (int32_t queryId = 0; queryId < nQueries / 2; queryId++)
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

                    result[row * nRes + col] += data[cRowA * nCols + cColA] * data[cRowB * nCols + cColB];
                }
            }
        }
        // std::cout << "send " << sizeof(double) * nRes * nRes << std::endl;
        close(pipefd[0]);
        write(pipefd[1], result, sizeof(double) * nRes * nRes);
        close(pipefd[1]);
        exit(0);
    }
    else
    { // parent
        for (int32_t queryId = nQueries / 2; queryId < nQueries; queryId++)
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

                    result[row * nRes + col] += data[cRowA * nCols + cColA] * data[cRowB * nCols + cColB];
                }
            }
        }
        double *fromChild = new double[nRes * nRes];
        close(pipefd[1]);
        char buf[8];
        int ind = 0;
        while (read(pipefd[0], &buf, 8) > 0) {
            fromChild[ind] = *(double*)buf;
            ++ind;
        }
        close(pipefd[0]);
        wait(NULL);
        for (int i = 0; i < nRes * nRes; ++i) {
            result[i] += fromChild[i];
        }
    }

    // Reference unoptimized code, replace it with your solution
}