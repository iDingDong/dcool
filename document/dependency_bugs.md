# Dependency Bugs

This document records bugs of the dependencies of this library encoutered during the implementation requiring a workaround or even blocking certain features to be implemented.

## Unresolved bugs

### Bug_1: class type NTTP causes ICE

- Compiler: GCC
- Info last updated: 2021-04-28
- Status: Unresolved (latest version 11.1.0)
- Report link: [GCC Bug #95159](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=95159)

Minial reprocase:

```cpp
#include <cstdlib>
#include <memory>

struct MemAttr {
	std::size_t size;
	std::size_t align;
};

template <typename T> constexpr MemAttr memAttrOf = MemAttr { .size = sizeof(T), .align = alignof(T) };

template <MemAttr attr> using AlignedStorage = ::std::aligned_storage_t<attr.size, attr.align>;

template <typename T> using AlignedStorageFor = AlignedStorage<memAttrOf<T>>;
```

The code above is rejected by compiler with message:

> ```terminal
> internal compiler error: Segmentation fault
> ```

The workaround is never to pass class type non-type template argument.

### Bug_2: Unused warning on used item

- Compiler: GCC
- Info last updated: 2021-04-28
- Status: Unresolved (latest version 11.1.0)
- Report link: [GCC Bug #61596](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=61596)

Minial reprocase:

```cpp
auto foo() {
  struct A {
    using Type = int;
  };
  return A();
}

auto bar() {
  return decltype(foo())::Type();
}
```

The code above is warned by compiler option `-Wunused-local-typedef`(enabled by `-Wall`) with message:

> ```terminal
> warning: typedef 'using Type = int' locally defined but not used
> ```

The `Type` is actually used in `bar`. The workaround is to define the above class outside function body.

### Bug_3: Copy operations not defaultable

- Compiler: GCC
- Info last updated: 2021-04-28
- Status: Unresolved (latest version 11.1.0)
- Report link: [GCC Bug #98614](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=98614)

Minial reprocase:

```cpp
#include <concepts>

template <typename T> struct A {
	A(A<T> const&) = default;
};

template <std::same_as<int> T> struct A<T> {
	A(A<T> const&) = default;
};
```

The second defaulted copy constructor will be rejected by compiler with message:

> ```terminal
> error: 'A<T>::A(const A<T>&)' cannot be defaulted
> ```

This bug only gets triggered by copy constructor/assignment in a class template specialization with concepts while move constructor/assignment or other form of class definitions are not affected.

The workaround is to omit the `<T>` in the argument declarator.

## Resolved bugs

None.
