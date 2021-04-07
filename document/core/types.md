# Class template `dcool::core::Types`

Include `<dcool/core.hpp>` to use.

```cpp
template <typename... Ts> struct dcool::core::Types;
```

Represents a list of types with its argument.

## Other helpers

```cpp
template <typename T> concept dcool::core::TypeList;
```

Indicates whether type `T` is a type instanitiated from template `dcool::core::Types`.

## Note

`dcool::core::Types` is intended as an alternative where variadic template parameters do not work ideally.
