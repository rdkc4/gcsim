# GCSim
Heap manager simulator using mark-sweep and mark-compact garbage collectors, written in C++23.

### Getting Started

#### Prerequisites
- **Clang** (any version compatible with C++23)
- `make`
- **Pthread Support** (the build uses the -pthread flag)

#### Unit Tests (optional)
- **Google Test (gtest)** for running unit tests.

#### Installation

1. Clone the repository:
    ```bash
    git clone https://github.com/rdkc4/gcsim.git
    cd gcsim
    ```
2. Build the project:
    - -O1 optimization with debugging flags
        ```bash
        make [-j$(nproc)]
        ```
    - -O2 optimization, no debugging flags
        ```bash
        make perf [-j$(nproc)]
        ```
    - -O3 optimization, no debugging flags
        ```bash
        make benchmark [-j$(nproc)]
        ```

#### Usage
To run a simulation:
```bash
./gcsim [options]
```

Options:
- -gc, --garbage-collector : type of the garbage collector: mc, ms (mc - mark-compact, ms - mark-sweep)
- -i, --iterations         : number of simulation iterations, positive number
- -m, --mode               : simulation mode: stress, relaxed
- -M, --mutators           : number of concurrent mutators min 1, max 10
- -o, --output             : output file for simulation results
- -h, --help               : display this help text

#### Unit Tests
Running the tests:
```bash
make test [-j$(nproc)]
```