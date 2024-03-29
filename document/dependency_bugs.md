# Dependency Bugs

This document records bugs of the dependencies of this library encoutered during the implementation requiring a workaround or even blocking certain features to be implemented.

## Unresolved bugs

### Bug_1: class type NTTP causes ICE

- Compiler: GCC
- Info last updated: 2021-09-23
- Status: Unresolved (latest version 11.2.0)
- Report link: [GCC Bug #102470](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=102470)

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
- Info last updated: 2021-09-23
- Status: Unresolved (latest version 11.2.0)
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
- Info last updated: 2021-09-23
- Status: Unresolved (latest version 11.2.0)
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

### Bug_4: Deadlock when calling `std::call_once` on the same `std::once_flag` again after a call throwing an exception

- Compiler: GCC
- Info last updated: 2021-10-11
- Status: Unresolved (latest version 11.2.0)
- Report link: [GCC Bug #66146](https://gcc.gnu.org/bugzilla/show_bug.cgi?id=66146)

Minial reprocase:

```cpp
#include <mutex>
#include <stdexcept>

int main() {
std::once_flag flag;
	try {
		std::call_once(
			flag,
			[]() {
				throw std::exception();
			}
		);
	} catch (std::exception const&) {
	}
	std::call_once(
		flag,
		[]() {
			throw std::exception();
		}
	);
	return 0;
}
```

The code above will deadlock with libstdc++ implementation using `pthread_once`.

The workaround is to manually guarantee that the flag is used for only once, or ensures the callable never throws.

## Resolved bugs

None so far.
