import time
import random
import arrays_operations


def generate_random_list(size, min_val=0, max_val=10000):
    return [random.randint(min_val, max_val) for _ in range(size)]


def measure_stat(function, array, name):
    result, duration = function(array)
    print(f"  ├─ [{name}] = {result:<15} (in {duration:.2f} ms)")
    return result


def measure_filter(function, array, threshold, name):
    result, duration = function(array, threshold)
    print(f"  └─ [{name} > {threshold}] matched {len(result)} items (in {duration:.2f} ms)")
    return result


def print_header():
    print("=" * 50)
    print(" Pybind11 + OpenMP: Array Operations Benchmark ")
    print("=" * 50)
    print("→ Operations: Maximum | Sum | Filter\n")


def main():
    sizes = [100000, 1000000, 10000000, 20000000]
    threshold = 9000000

    print_header()

    for size in sizes:
        print(f"\nArray size: {size:,}")
        print("  Performing operations...")
        array = generate_random_list(size, 0, size)

        measure_stat(arrays_operations.max, array, "Maximum")
        measure_stat(arrays_operations.sum, array, "Sum")
        measure_filter(arrays_operations.filter, array, threshold, "Filter")

    print("\n✔ Finished all statistics with OpenMP + Pybind11.\n")


if __name__ == "__main__":
    main()
