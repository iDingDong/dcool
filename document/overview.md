# Overview

A library intended to provide more flexible alternatives to part of the standard library.

This library is a work-in-progress and is not expected to be fully stablized until we have at least one compiler fully implement ISO/IEC 14882:2020.

## Common concept

### The type template parameter configuration

The type template parameter configuration provides its nested members to customize the behavior. A required member is optional if marked with a default value or type.

If a user-provided configuration type have the member name occupied with a definition that does not suite the requirement, the behavior is undefined.

## Notes

### Consider any undocumented behavior related to this library unstable

In addition, names defined in namespace `*::detail_`, or names followed by tailing underscore are not even yet planned to be stablized in the future.

### Code previews does not necessarily express the exact look of implementation

Unless otherwise specified, there could be major differences as follows:

- Parameter names may be different;
- Additional constraints might be applied for better diagnostics;
- Type names might be aliases even if specified as `struct`, `union`, etc.;
- Dependent names might be disambiguated.
- Function prototypes behaves as-if how it can be called. For example, a function documented `void Foo()` might be implemented as `int Foo()`. This means you are discouraged to take a function pointer even if the function is documented here unless otherwise specified.
