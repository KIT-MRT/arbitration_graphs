## Arbitration Graph Python Bindings

This directory contains Python bindings for the Arbitration Graph library.
The bindings are generated using `pybind11` and provide a convenient interface for Python users to interact with the underlying C++ code.

The interface is identical to the C++ interface except that it is following Python naming conventions.
See the [unit tests](test/) for examples of how to use the bindings.

> **Note:** The coordinator types are not yet implemented in the Python bindings.

### Building the Bindings

With the arbitration graph library installed (see [../README.md](../README.md)), you can build the Python bindings via pip:

```bash
# From local repository
git clone https://github.com/KIT-MRT/arbitration_graphs.git
cd arbitration_graphs/python_bindings
pip install .

# Or directly from GitHub
pip install git+https://github.com/KIT-MRT/arbitration_graphs.git#subdirectory=python_bindings
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
cd arbitration_graphs/python_bindings/test
python -m unittest discover
```

