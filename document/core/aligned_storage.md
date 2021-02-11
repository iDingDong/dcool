# Type template `dcool::core::AlignedStorage`

Include `<dcool/core.hpp>` to use.

```cpp
template <dcool::core::StorageRequirement requirementC> using dcool::core::AlignedStorage = /* unspecified type */;
```

A trivial standard-layout type suitable for use as uninitialized storage for any object whose size is at most `dcool::core::size(requirementC)` and whose alignment requirement is a divisor of `dcool::core::alignment(requirementC)`. Further restrictions are identical to `std::aligned_storage_t<dcool::core::size(requirementC), dcool::core::alignment(requirementC)>`.

## Other helpers

### Type template `dcool::core::StorageFor`

```cpp
template <typename Value> using dcool::core::StorageFor = dcool::core::AlignedStorage<
	dcool::core::storageRequirementFor<Value>
>;
```
