#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
#include <vector>
#include <limits>
#include <omp.h>
#include <chrono>

namespace py = pybind11;

int max(const std::vector<int>& arr) {
    int global_max = std::numeric_limits<int>::min();
    int n = arr.size();

    #pragma omp parallel num_threads(10)
    {
        int local_max = std::numeric_limits<int>::min();
        #pragma omp for collapse(2)
        for (int i = 0; i < n; ++i) {
            if (arr[i] > local_max) {
                local_max = arr[i];
            }
        }

        #pragma omp critical
        {
            if (local_max > global_max) {
                global_max = local_max;
            }
        }
    }

    return global_max;
}

long long sum(const std::vector<int>& arr) {
    long long sum = 0;
    #pragma omp parallel for reduction(+:sum)
    for (int i = 0; i < arr.size(); ++i) {
        sum += arr[i];
    }
    return sum;
}

std::vector<int> filter(const std::vector<int>& arr, int threshold) {
    int n = arr.size();
    std::vector<std::vector<int>> thread_results;

    int num_threads = omp_get_max_threads();
    thread_results.resize(num_threads);

    #pragma omp parallel
    {
        int tid = omp_get_thread_num();
        std::vector<int>& local_result = thread_results[tid];

        #pragma omp for nowait
        for (int i = 0; i < n; ++i) {
            if (arr[i] > threshold) {
                local_result.push_back(arr[i]);
            }
        }
    }

    // Злиття результатів з усіх потоків
    std::vector<int> result;
    for (const auto& vec : thread_results) {
        result.insert(result.end(), vec.begin(), vec.end());
    }

    return result;
}

PYBIND11_MODULE(arrays_operations, m) {
    m.doc() = "Parallel max, sum using OpenMP";

    m.def("max", [](const std::vector<int>& arr) {
        auto start = std::chrono::high_resolution_clock::now();
        int result = max(arr);
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end - start).count();
        return py::make_tuple(result, duration);
    });

    m.def("sum", [](const std::vector<int>& arr) {
        auto start = std::chrono::high_resolution_clock::now();
        long long result = sum(arr);
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end - start).count();
        return py::make_tuple(result, duration);
    });

    m.def("filter", [](const std::vector<int>& arr, int threshold) {
        auto start = std::chrono::high_resolution_clock::now();
        std::vector<int> result = filter(arr, threshold);
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end - start).count();
        return py::make_tuple(result, duration);
    }, py::arg("arr"), py::arg("threshold"));
}
