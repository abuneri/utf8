# (A)buneris (U)ni(c)ode: UTF-8

A simple UTF-8 encoded text type, mostly for learning purposes

## Features
- Validation
- Decoding
- Encoding
- BOM Handling
- Grapheme Clustering
- **[TODO]** Collation
- **[TODO]** Small String Optimization
- **[TODO]** Regular Expressions

## Implementation References
- https://www.rfc-editor.org/rfc/rfc3629
- https://en.wikipedia.org/wiki/UTF-8
- _The Unicode Standard, Version 15.0.0_
    - https://www.unicode.org/reports/tr29/
    - https://www.unicode.org/reports/tr10/
    - https://www.unicode.org/reports/tr18/

## Installation
### Build and install project

```
mkdir build
cd build

# Without tests
cmake .. -DBUILD_TESTING=OFF
cmake --build . --config Release

# With tests
cmake ..
cmake --build . --config Release
ctest

cmake --install . --config Release
```

### Include package in your own CMake projects

```
# Your projects CMakeLists.txt

find_package(auc REQUIRED)

# ... configure <your_target> ...

target_link_libraries(<your_target>
    auc
)
```


## Example
```cpp
#include <auc/u8text.hpp>

int main() {
  auc::u8text text(u8"Ī咩鉼歺и(尤ۼñ>w");
  if (text.is_valid()) {
    for (const auc::codepoint& cp : text.get_codepoints()) {
        // ... codepoint-wise operations ...
    }

    for (const auc::graphemecluster& gc : text.get_grapheme_clusters()) {
        // ... grapheme cluster-wise operations ...
    }
  }
}

```

