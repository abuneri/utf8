# (A)buneris (U)ni(c)ode: UTF-8

A simple UTF-8 encoded text type, mostly for learning purposes

## Implementation References
- https://www.rfc-editor.org/rfc/rfc3629
- https://en.wikipedia.org/wiki/UTF-8
- _The Unicode Standard, Version 15.0.0_
- https://www.unicode.org/reports/tr29/
- https://www.unicode.org/reports/tr10/
- https://www.unicode.org/reports/tr18/

## Building (Meson)
This project uses the [meson build generation system](https://mesonbuild.com/), which uses Ninja as a backend by default and works on Windows, macOS, and Linux platforms.

### Ninja
An example of how to build the project with the Ninja backend:

`meson setup [--reconfigure] --buildtype <type> <build-dir>`

### Visual Studio 2022
An example of how to customize the backend to develop with MSVC via Microsoft's Visual Studio 2022:

`meson setup [--reconfigure] --backend vs2022 --buildtype <type> <build-dir>`

## Features
- UTF-8 Validation
- UTF-8-BOM Support
- Grapheme Clustering
- **[TODO]** Collation
- **[TODO]** Regular Expressions
- Decode to Codepoints
- Encode from Codepoints
- **[TODO]** Small String Optimization
