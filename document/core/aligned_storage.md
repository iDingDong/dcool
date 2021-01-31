# Type template `dcool::core::AlignedStorage`

Include `<dcool/core.hpp>` to use.

```cpp
template <dcool::core::StorageRequirement requirement> using dcool::core::AlignedStorage = /* unspecified type */;
```

A trivial standard-layout type suitable for use as uninitialized storage for any object whose size is at most `dcool::core::size(requirement)` and whose alignment requirement is a divisor of `dcool::core::alignment(requirement)`. Further restrictions are identical to `std::aligned_storage_t<dcool::core::size(requirement), dcool::core::alignment(requirement)>`.

## Other helpers

### Type template `dcool::core::StorageFor`

```cpp
template <typename Value> using dcool::core::StorageFor = dcool::core::AlignedStorage<
	dcool::core::storageRequirementFor<Value>
>;
```
