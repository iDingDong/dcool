# Class template Sealer

A strong exception safety helper.

```cpp
template <typename ValueT, typename ConfigT = /* unspecified type */> struct Sealer;
```

A `dcool::vigor::Sealer` object hold and owns another object of `ValueT`.

Unless otherwise specified, all operations have strong exception guarantee even if the holded value does not guarentee that by itself.

Unless otherwise specified, if type `Config::Mutex` is available, all operations will attempt to be sychronzied with an object `mutex` of `Config::Mutex` if the mutex satisfies ISO C++ named concept *Mutex*. If it also satisfies ISO C++ named concept *SharedMutex*, readonly operations will attempt to lock the mutex in shared mode.

## Member typee

| Name | Definition |
| - | - |
| `Value` | `ValueT` |
| `Config` | `ConfigT` |
| `Mutex` | `Config::Mutex` if available; otherwise an unspecified no-op mutex type. |

## Constructor

```cpp
constexpr Sealer() noexcept(/* unspecified expression */); // 1

constexpr Sealer(Sealer const& other) noexcept(/* unspecified expression */); // 2

template <typename... Arguments> constexpr Sealer(
	::dcool::core::InPlaceTag, Arguments&&... parameters
) noexcept(/* unspecified expression */); // 3
```

- Overload 1: Construct with holded value default-initialized.
- Overload 2: Construct with holded value copied from `other`.
- Overload 3: Construct with holded value initialized with the given `parameters`.

## Destructor

```cpp
constexpr ~Sealer() noexcept;
```

Destruct holded value.

## Assignment

```cpp
constexpr auto operator =(Sealer const& other) noexcept(/* unspecified expression */) -> Sealer&;
```

Copy the value holded by `other` to value holded by self.

## Other member

### `copiedValue`

```cpp
constexpr auto copiedValue() const noexcept -> Value
```

Returns a copy of the holded value.

### `emplace`

```cpp
template <typename... Arguments> constexpr void emplace(Arguments&&... parameters) noexcept(/* unspecified expression */);
```

Replace the holded value with a new value constructed by `parameters`.

### `runTransaction`

```cpp
template <typename TransactionT> constexpr void runTransaction(
	TransactionT&& transaction
) noexcept(/* unspecified expression */);
```

`transaction` will be invoked with a parameter of type `Value const&` refering the holded value. The invocation is expected to return a value that will be used to construct a new value to replace the holded value.

### `runObserver`

```cpp
template <typename ObserverT> constexpr void runObserver(
	ObserverT&& observer
) noexcept(/* unspecified expression */);
```

`observer` will be invoked with a parameter of type `Value const&` refering the holded value.

## Note

Implementation of `dcool::vigor::Sealer` **might** dynamically allocate storage for the stored object. When writing a new value to the object, the new value would be used to construct a new value on another allocated space and replace the old storage on success.
