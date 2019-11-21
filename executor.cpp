#include <random>
#include <chrono>
#include <cstring>
#include <fstream>

#include <cstdint>

using std::int32_t;

#include "task.h"

const double minValue = 0.0;
const double maxValue = 10.0;

void generateData(int32_t nRows, int32_t nCols, double* data, std::mt19937& rng)
{
    for (int32_t row = 0; row < nRows; row++)
    {
        for (int32_t col = 0; col < nCols; col++)
        {
            data[row * nCols + col] = (rng() / (double)rng.max()) * (maxValue - minValue) + minValue;
        }
    }
}

void generateQueries(int32_t nRows, int32_t nCols, int32_t nQueries, int32_t nRes, int32_t* queries, std::mt19937& rng)
{
    for (int32_t queryId = 0; queryId < nQueries; queryId++)
    {
        int32_t rowA = rng() % (nRows - nRes);
        int32_t colA = rng() % (nCols - nRes);
        int32_t rowB = rng() % (nRows - nRes);
        int32_t colB = rng() % (nCols - nRes);

        queries[queryId * 4 + 0] = rowA;
        queries[queryId * 4 + 1] = colA;
        queries[queryId * 4 + 2] = rowB;
        queries[queryId * 4 + 3] = colB;
    }
}

void fillResult(int32_t nRes, double* result)
{
    std::memset(result, 0, sizeof(double) * nRes * nRes);
}

int32_t main(int32_t argc, char *argv[])
{
    if (argc < 6)
    {
        printf ("Usage model: nRows nCols nQueries nRes seed [path_to_reference_result]");
        return 0;
    }
    
    const int32_t nRows = atoi(argv[1]);
    const int32_t nCols = atoi(argv[2]);
    const int32_t nQueries = atoi(argv[3]);
    const int32_t nRes = atoi(argv[4]);
    const int32_t seed = atoi(argv[5]);

    bool checkCorrectness = false;
    double* refResult = NULL;

    if (argc >= 7)
    {
        checkCorrectness = true;

        refResult = new double[nRes * nRes];

        std::ifstream refFile(argv[6]);
        if (!refFile.is_open())
        {
            printf ("File with reference result is missing: %s\n", argv[6]);
            return 0;
        }

        for (int i = 0; i < nRes; i++)
        {
            for (int j = 0; j < nRes; j++)
            {
                refFile >> refResult[i * nRes + j];
            }
        }
    }

    double* data = new double[nRows * nCols];
    int32_t* queries = new int32_t[nQueries * 4];
    double* result = new double[nRes * nRes];

    std::mt19937 rng(seed);
    generateData(nRows, nCols, data, rng);
    generateQueries(nRows, nCols, nQueries, nRes, queries, rng);
    fillResult(nRes, result);

    auto start = std::chrono::high_resolution_clock::now();
    performQueries(nRows, nCols, nQueries, nRes, data, queries, result);
    auto finish = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(finish - start).count();
    printf ("Execution time = %.5f seconds\n", duration / 1000000.0);

    if (checkCorrectness)
    {
        double maxDiff = 0.0;
        for (int32_t i = 0; i < nRes; i++)
        {
            for (int32_t j = 0; j < nRes; j++)
            {
                double curDiff = std::abs(result[i * nRes + j] - refResult[i * nRes + j]);
                if (refResult[i * nRes + j] > 1.0)
                    curDiff = std::min(curDiff, curDiff / refResult[i * nRes + j]);
                maxDiff = std::max(maxDiff, curDiff);
            }
        }
        printf ("Relative difference = %.15f\n", maxDiff);
        if (maxDiff > 1e-6)
        {
            printf ("Relative difference between your and reference outputs exceeds maximal threshold: %.15f > %.15f\n", maxDiff, 1e-6);
        }
    }

    delete [] data;
    delete [] queries;
    delete [] result;

    if (refResult)
    {
        delete [] refResult;
    }

    return 0;
}

