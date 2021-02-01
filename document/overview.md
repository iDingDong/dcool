# Overview

A library intended to provide more flexible alternatives to part of the standard library.

This library is a work-in-progress and is not expected to be fully stablized until we have at least one compiler fully implement ISO/IEC 14882:2020.

## Common concept

### The type template parameter configuration

The type template parameter configuration provides its nested members to customize the behavior. A required member is optional if marked with a default value or type.

If a user-provided configuration type have any member name that is not required, the behavior is undefined unless the name contains an underscore.

If a user-provided configuration type have the member name occupied with a definition that does not suite the requirement, the behavior is undefined.

## Reserved names

Unless otherwise specified, the behavior is undefined if a user-defined name

- is documented to be defined by this library, or
- is in the global namespace that begins with `DCOOL_` or `dcool`, or
- is in the namespace `dcool`, or
- is a macro defined before inclusion of one of this library's headers that begins with no underscore.

## Compatibility

This section assumes that the program contains only well-defined behaviors.

### ABI compatibility

Unless otherwise specified, ABI compatibility is preserved within the same minor version after 1.0.0, unless it is considered to be wrongly implemented and will be inevitably broken by a bug fix.

### Source-code compatibility

Unless otherwise specified, source-code compatibility is preserved within the same major version after 1.0.0, unless it is considered to be wrongly implemented and will be inevitably broken by a bug fix.

## Notes

### Consider any undocumented behavior related to this library unstable

In addition, names defined in namespace `*::detail_`, or names followed by tailing underscore are not even yet planned to be stablized in the future.

### Code previews does not necessarily express the exact look of implementation

Unless otherwise specified, there could be major differences as follows:

- Parameter names may be different.
- Additional constraints might be applied for better diagnostics.
- Type names might be aliases even if specified as `struct`, `union`, etc.
- Dependent names might be disambiguated.
- Function prototypes behaves as-if how it can be called. For example, a function documented `void Foo()` might be implemented as `int Foo()`. This means you are discouraged to take a function pointer even if the function is documented here unless otherwise specified.
