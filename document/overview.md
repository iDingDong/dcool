# Overview

A library intended to provide more flexible alternatives to part of the standard library.

This library is a work-in-progress and is not expected to be fully stablized until we have at least one compiler fully implement ISO/IEC 14882:2020.

## Common concept

### The type template parameter configuration

The type template parameter configuration provides its nested members to customize the behavior. A required member is optional if marked with a default value or type.

If a user-provided configuration type have any public(or accessible by corresponding adaptor in any other ways) member name that is not required, the behavior is undefined unless the name contains an underscore.

If a user-provided configuration type have the public(or accessible by corresponding adaptor in any other ways) member name occupied with a definition that does not suite the requirement, the behavior is undefined.

## Reserved names

Unless otherwise specified, the behavior is undefined if a user-defined name

- is documented to be defined by this library, or
- begins with `DCOOL_` and all letters uppercase, or
- begins with `DCOOLER_` and all letters uppercase but not documented as a preprocessor configuration, or
- begins with `dcool` and ends with an underscore, or
- is in the namespace `dcool`, or
- is a macro defined before inclusion of one of this library's headers that does not have all letters uppercase, begins with no underscore and ends with an underscore.

## Compatibility

This section assumes that the program is well-formed and only contains only well-defined behaviors.

### ABI compatibility

Unless otherwise specified, ABI compatibility is preserved within the same minor version after 1.0.0 as long as the C++ implementation does not break it, unless it is considered to be wrongly implemented and will be inevitably broken by a bug fix.

### Source-code compatibility

Unless otherwise specified, source-code compatibility is preserved within the same major version after 1.0.0, unless it is considered to be wrongly implemented and will be inevitably broken by a bug fix.

## Notes

### Consider any undocumented behavior related to this library unstable

In addition, names defined in namespace `*::detail_`, or names followed by tailing underscore are not even yet planned to be stablized in the future.

### Code previews does not necessarily express the exact look of implementation

Unless otherwise specified, there could be major differences as follows:

- `public` access specifiers are omitted in the document, i. e., users are safe to assume all documented members are public unless otherwise specified.
- Parameter names may be different.
- Additional constraints might be applied to reject ill-formed or undefined cases early for better diagnostics.
- Type names might be aliases even if specified as `struct`, `union`, etc.
- Dependent names might be disambiguated.
- Function prototypes behaves as-if how it can be called. For example, a function documented `void Foo()` might be implemented as `int Foo()`. This means users are discouraged to take a function pointer even if the function is documented here unless otherwise specified.
