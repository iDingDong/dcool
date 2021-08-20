# Class template `dcool::utility::AnyMutex`

Include `<dcool/utility.hpp>` to use.

```cpp
template <typename ConfigT = /* unspecified type */> struct dcool::utility::AnyMutex;
```

A mutex-like type-erasure utility.

## Configuration `ConfigT`

Its member shall customize the list as decribed:

| Member | Default | Behavior |
| - | - | - |
| Type `ExtendedInformation` | `dcool::core::Pit` | See *Customized extended operations* for more details. |
| Type `Pool` | `dcool::resource::DefaultPool` | The dynamic memory resource of any mutex. |
| Type `Engine` | *Unspecified* | Provided `Engine engine`, `engine.pool()` shall evaluate to a reference to `Pool` for dynamic memory management. |
| Type `SupportedTimePoints` | `dcool::core::Types<>` | Represent a list of types instanitiated from template `std::chrono::time_point` that can be used for timed lock. If it either failes to meet `dcool::core::TypeList` or contains a element not instanitiated from template `std::chrono::time_point`, the behavior is undefined. |
| `static constexpr dcool::core::Boolean copyable` | `false` | The any mutex shall be copyable if it takes value `true`; otherwise non-copyable. |
| `static constexpr dcool::core::Boolean movable` | *Same as* `copyable` | The any mutex shall be moveable if it takes value `true`; otherwise non-moveable. |
| `static constexpr dcool::core::StorageRequirement squeezedTankage` | *Unspecfied* | If the item to be stored is storable in a statically allocated storage of `squeezedTankage`, implementation would attempt to avoid dynamic allocation. |
| `static constexpr dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy` | `dcool::core::defaultExceptionSafetyStrategy` | The default exception safety strategy of all operations. |

## Member types

| Name | Definition |
| - | - |
| `ExtendedInformation` | Determined by configuration `ExtendedInformation`. |

## Constructors

```cpp
constexpr AnyMutex() noexcept;
constexpr AnyMutex(AnAnyMutexy const& other);
constexpr AnyMutex(AnyMutex&& other) noexcept(/* unspecified expression */);
template <typename ValueT> constexpr AnyMutex(ValueT&& value) noexcept(/* unspecified expression */);
template <typename ValueT, typename... ArgumentTs> constexpr Any(
	dcool::core::TypedInPlaceTag<ValueT>, ArgumentTs&&... parameters
) noexcept(/* unspecified expression */);
```

- Overload 1: Construct an any mutex that does not hold an object.
- Overload 2, 3: Construct as a copy of `other` (as-if).
- Overload 4: Construct an any mutex that holds an object of decayed `ValueT` with `value`.
- Overload 5: Construct an any mutex that holds an object of `ValueT` with `parameters`.

## Assignments

```cpp
constexpr auto operator =(AnyMutex const& other) noexcept(/* unspecified expression */) -> AnyMutex&;
constexpr auto operator =(AnyMutex&& other) noexcept(/* unspecified expression */) -> AnyMutex&;
```

Assign as a copy of `other` (as-if).

## Non-static member functions

### `swapWith`

```cpp
constexpr void swapWith(Any& other) noexcept(/* unspecified expression */);
```

Swap with `other`.

### `valid`

```cpp
constexpr auto valid() const noexcept -> dcool::core::Boolean;
```

Returns true if the any holds an object, otherwise returns false.

### `storageRequirement`

```cpp
constexpr auto storageRequirement() const noexcept -> dcool::core::StorageRequirement;
```

Returns the storage requirement of the holded object if exists, otherwise returns `dcool::core::storageRequirement<0, 0>`.

### `typeInfo`

```cpp
constexpr auto typeInfo() const noexcept -> dcool::core::TypeInfo const&
```

Returns the typeid of the holded object if exists, otherwise returns `typeid(void)`.

### `extendedInformation`

```cpp
constexpr auto extendedInformation() const noexcept -> ExtendedInformation const&
```

Returns a reference to a potentially-overlapping extended information with static storage duration constructed by `ExtendedInformation(dcool::core::typed<ValueT>)` (where `ValueT` is the type of the holded object).

See *Customized extended operations* for more details.

### `access`

```cpp
template <typename ValueT> constexpr auto access() const noexcept -> ValueT const&;
template <typename ValueT> constexpr auto access() noexcept -> ValueT&;
```

Returns a reference to the holded object. The behavior is undefined if `ValueT` is not exactly the same as the holded object.

### `value`

```cpp
template <typename ValueT> constexpr auto value() const noexcept -> ValueT const&;
template <typename ValueT> constexpr auto value() noexcept -> ValueT&;
```

Returns a reference to the holded object if `ValueT` is exactly the same as the holded object, otherwise throws a `dcool::utility::BadAnyCast` (might be the same as `std::bad_any_cast`).

### `lock`

```cpp
constexpr void lock();
```

Throws a `dcool::utility::BadAnyMutexAction` if the any mutex does not hold an object. Otherwise equivalent to *underlying-object*`.lock()`;

### `tryLock`

```cpp
constexpr auto tryLock() -> dcool::core::Boolean;
```

Throws a `dcool::utility::BadAnyMutexAction` if the any mutex does not hold an object. Otherwise equivalent to *underlying-object*`.tryLock()` or *underlying-object*`.try_lock()`;

### `tryLockUntil`

```cpp
template <typename TimePointT> constexpr auto tryLockUntil(TimePointT const& deadline) -> dcool::core::Boolean;
```

The `deadline` as the argument shall be involved in overload resolution among the types in `SupportedTimePoints`.

Throws a `dcool::utility::BadAnyMutexAction` if the any mutex does not hold an object. Otherwise equivalent to *underlying-object*`.tryLockUntil(deadline)` or *underlying-object*`.try_lock_until(deadline)` if well-formed; Otherwise throws a `dcool::utility::BadAnyMutexAction`.

### `tryLockFor`

```cpp
template <typename DurationT> constexpr auto tryLockFor(DurationT const& duration) -> dcool::core::Boolean;
```

Equivalent to `this->tryLockUntil(std::chrono::steady_clock::now() + duration)`.

### `unlock`

```cpp
constexpr void unlock();
```

Throws a `dcool::utility::BadAnyMutexAction` if the any mutex does not hold an object. Otherwise equivalent to *underlying-object*`.unlock()`;

## Customized extended operations

Customizing extended operations is almost exactly the same as extending `dcool::utility::Any`, except that you should change all `dcool::utility::Any` to a proper `dcool::utility::AnuMutex` type.
