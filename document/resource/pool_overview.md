# Pool overview

This feature is unstable.

Pool is intended to be an alternative for standard allocators.

## Define customized pool

A minial example of a simple pool:

```cpp
class MyPool {
	public: template <dcool::core::StorageRequirement requirement> void* allocate() {
		return operator new(dcool::core::size(requirement), std::align_val_t(dcool::core::alignment(requirement)));
	}

	public: template <dcool::core::StorageRequirement requirement> deallocate(void* pointer) {
		return operator delete(pointer, dcool::core::size(requirement), std::align_val_t(dcool::core::alignment(requirement)));
	}
};
```
