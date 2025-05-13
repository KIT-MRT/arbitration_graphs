## Arbitration Graph Python Bindings

This directory contains Python bindings for the Arbitration Graph library.
The bindings are generated using `pybind11` and provide a convenient interface for Python users to interact with the underlying C++ code.

The interface is identical to the C++ interface except that it is following Python naming conventions.
See the [unit tests](test/) for examples of how to use the bindings.

### Building the Bindings

With the arbitration graph library installed (see [../README.md](../README.md)), you can build the Python bindings via pip:

```bash
pip install .
```

Alternatively, you can build the bindings using `cmake`:

```bash
mkdir build
cd build
cmake ..
cmake --build .
```

### Running the Tests

This package includes unit tests analogous to the C++ tests.
To run all tests, use the following command:

```bash
python -m unittest discover -s test -p "*.py"
```

