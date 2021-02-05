# Raw pointer getter

Include <dcool/core.hpp> to use.

```cpp
template <typename PointerT> constexpr auto rawPointerToLivingOf(
	PointerT const& pointer
) noexcept(/* unspecified expression */);
template <typename PointerT> constexpr auto rawPointerOf(PointerT const& pointer) noexcept(/* unspecified expression */);
```

Returns a raw pointer type representing the same reference as `pointer`.

`rawPointerToLivingOf` will attempt to take the address of dereferenced `pointer`. If `pointer` does not point to a living object, `rawPointerToLivingOf` will produce undefined behavior.

`rawPointerOf` returns `pointer.rawPointer()` if it evaluates to a raw pointer, otherwise returns `std::to_address(pointer)`.

## Notes

`rawPointerToLivingOf` expects to always return a pointer ready for dereferencing, i.e. properly laundered if necessary. `rawPointerOf` never expects to get the pointer returned dereferenced other than for placement new.