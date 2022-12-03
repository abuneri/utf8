# (A)buneris (U)ni(c)ode: UTF-8

A simple UTF-8 encoded text type, mostly for learning purposes

## Implementation References
- https://www.rfc-editor.org/rfc/rfc3629
- https://en.wikipedia.org/wiki/UTF-8
- _The Unicode Standard, Version 15.0.0_
- https://www.unicode.org/reports/tr29/

## Building (Meson)
This project uses the meson build generation system, which uses Ninja as a backend by default and works on Windows, macOS, and Linux platforms.

### Windows (Visual Studio/MSVC)
An example of how to customize the backend to develop within Microsoft's Visual Studio 2022.
`meson setup [--reconfigure] --backend vs2022 --buildtype <type> <build-dir>`
