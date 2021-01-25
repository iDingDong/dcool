# Overview

A library intended to provide more flexible alternatives to part of the standard library.

This library is a work-in-progress and is not expected to be fully stablized until we have at least one compiler fully implement ISO/IEC 14882:2020.

## Notes

### Consider any undocumented behavior related to this library unstable

In addition, names defined in namespace `*::detail_`, or names followed by tailing underscore are not even yet planned to be stablized in the future.

### Code previews does not necessarily express the exact look of implementation

Unless otherwise specified, there could be major differences as follows:

- Parameter names may be different;
- Additional constraints might be applied for better diagnostics;
- Type names might be aliases even if specified as `struct`, `union`, etc.;
- Dependent names might be disambiguated.
