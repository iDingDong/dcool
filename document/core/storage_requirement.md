# Type `dcool::core::StorageRequirement`

Include `<dcool/core.hpp>` to use.

Represent size and alignment requirement.

```cpp
using dcool::core::StorageRequirement = /* unspecified trivial type */;
```

## Helper function

### `size`

```cpp
constexpr auto size(dcool::core::StorageRequirement requirement) -> dcool::core::Size;
```

Returns the size requirement holded by `requirement`. This helper is expected to be removed after `document/dependency_bugs.md#Bug_1` gets fixed.

### `alignment`

```cpp
constexpr auto alignment(dcool::core::StorageRequirement requirement) -> dcool::core::Alignment;
```

Returns the alignment requirement holded by `requirement`. This helper is expected to be removed after `document/dependency_bugs.md#Bug_1` gets fixed.

## Other helper

### Variable template `dcool::core::storageRequirement`

```cpp
template <
	dcool::core::Size size,
	dcool::core::Alignment alignment = /* unspecified */
> constexpr dcool::core::StorageRequirement dcool::core::storageRequirement;
```

A constant storage requirement holding size equals to `size` and alignment equals to `alignment`. This helper is expected to be removed after `document/dependency_bugs.md#Bug_1` gets fixed.

The default value for `alignment` is the same as `std::aligned_storage_t<size>`'s alignment requirement. If the default value is not used, `alignment` must be the value of `alignof(T)` for some type `T`, or the behavior is undefined.

### Variable template `dcool::core::storageRequirementFor`

```cpp
template <typename Value> constexpr dcool::core::StorageRequirement dcool::core::storageRequirementFor =
	dcool::core::storageRequirement<sizeof(Value), alignof(Value)>
;
```
