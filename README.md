[![libpampa](https://live.staticflickr.com/65535/49963724858_dd170ee43f_o.png)]()

[![License](https://img.shields.io/badge/license-Apache%202.0-informational.svg)](https://www.apache.org/licenses/LICENSE-2.0)
[![Codacy Badge](https://api.codacy.com/project/badge/Grade/6058e46317cd4c7cbadb33dc4e3d3283)](https://www.codacy.com?utm_source=github.com&amp;utm_medium=referral&amp;utm_content=oAGoulart/libpampa&amp;utm_campaign=Badge_Grade)
[![Build Status](https://dev.azure.com/agoulart/libpampa/_apis/build/status/oAGoulart.libpampa?branchName=master)](https://dev.azure.com/agoulart/libpampa/_build/latest?definitionId=1&branchName=master)

A library with some algorithms I implemented during my CS course @ unipampa.

## Usage

This library is supposed to be used just by including the files you need in your code, like this:

```c
#include "memory.h"
```

There are some caveats tho, some headers may require system specific libraries or dependencies (there isn't much I can do about that), here is a list of requirements for each header:

+ **memory.h**
  - on Windows: *Kernel32.lib*
+ **file.h**
  - on Windows: *Kernel32.lib* (because it uses **memory.h**)

Make sure to link these libraries when compiling your code, you can do this easily using CMake:

```cmake
if(CMAKE_HOST_SYSTEM_NAME STREQUAL "Windows")
  target_link_libraries(your_exe Kernel32.lib)
endif()
```

### Building examples and tests

To build the examples and tests provided, just use CMake!

```sh
mkdir build && cmake ../
```

Then you should be able to run the executables!

### Generating documentation

You can use the documentation on `docs/` as reference.

To generate documentation again, use Doxygen (`docs/` were generated on Debian x86-64):

```sh
doxygen Doxyfile
```

---

## Contributions

Feel free to leave your contribution here, I would really appreciate it!
Also, if you have any doubts or troubles using this library just contact me or leave an issue.
