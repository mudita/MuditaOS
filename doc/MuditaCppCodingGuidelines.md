# Mudita C++ Coding Guidelines

## Background

Mudita adopts the C++ Core Guidelines document, which is an open-source project to maintain modern authoritative guidelines for writing C++ code using the current C++ Standard. See http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines.

The Mudita C++ Coding Guidelines document is just an update to the C++ Core Guidelines. It adds new rules and exceptions to existing ones.

## Rules

All the C++ Core Guidelines rules are defined in the following document: http://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines.

All the C++ Core Guidelines rules are adopted to the Mudita C++ Core Guidelines.

## Mudita Ruleset

Mudita rules override and add exceptions to the C++ Core Guidelines ruleset.

### M-1. Quotes shall be used to include files relative to the including file. For all other headers, angle brackets shall be used.

See: [Motivation](https://isocpp.github.io/CppCoreGuidelines/CppCoreGuidelines#sf12-prefer-the-quoted-form-of-include-for-files-relative-to-the-including-file-and-the-angle-bracket-form-everywhere-else)  

### M-2. Relative include paths starting with ./ or ../ shall not be used.

### M-3. Sort the include files.

The includes shall be sorted in the following order:

- The header that corresponds to this .cpp file,
- Headers from the current directory,
- Headers from the same module,
- Headers from other modules,
- Headers from third-party modules,
- Headers from the standard library.

Example:
```
// file: module-gui/gui/widgets/Item.cpp

#include "Item.hpp"                  // Header that corresponds to this .cpp file
#include "Alignment.hpp"             // Header from a current directory
#include <gui/Common.hpp>            // Header from the same module
#include <Timers/Timer.hpp>          // Header from other module
#include <sml/include/boost/sml.hpp> // Header from 3rd-party module
#include <cstdint>                   // Header from the standard library
```

### M-4. utils::to_string(T t, size_t minStringLength = 0) shall be used with any 64bit numeric value.

The current `std::to_string()` implementation from the `stdlib` used in the MuditaOS doesn't handle 64bit numeric values properly. Use of `utils::to_string` is therefore recommended.

### M-5. Strongly typed parameters shall be preferred instead of passing booleans to functions and objects.

Prefer enum / struct params over bool, e.g.:
```
enum E { False, True };
```

### M-6. Follow AAA (Almost Always Auto) rule.

See: [Motivation](https://herbsutter.com/2013/08/12/gotw-94-solution-aaa-style-almost-always-auto/)

### M-7. Globally visible values (variables and constants) defined in header files shall be avoided.

If it is justified to use such values, use externally linked, compile-time initiated values with automatic type resolution, e.g.:
```
constexpr inline auto someString = "a string";
constexpr inline auto someValue = 10U;
```
instead of
```
const inline std::string someString = "a string";
const inline unsigned someValue = 10;
```

### M-8. Types with compile-time initialization shall be used to declare static class constants.
```
class Foo {
private:
  static constexpr auto someString = "a string";
};
```

### M-9. There shall be no comments like TODO, FIXME, etc.

Both upper- and lower-case of TODO, FIXME, etc. are prohibited.

### M-10. Non-obvious variables, constants, classes, structs, function and method declarations shall be documented using doxygen comments.

Doxygen comments shall not state the obvious. Don't literally describe what code does, unless the behaviour is non-obvious to a reader who understands C++ well.

_Note: Don’t document implementations if declarations have been already documented._

### M-11. For doxygen comments, /// syntax shall be used.
