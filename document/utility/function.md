# Class template `dcool::utility::Function`

Include `<dcool/utility.hpp>` to use.

```cpp
template <typename PrototypeT, typename ConfigT = /* unspecified type */> struct dcool::utility::Function; // Undefined
template <
	typename ConfigT, dcool::core::Boolean noexceptInvocableC, typename ReturnT, typename... ParameterTs
> struct dcool::utility::Function<auto(ParameterTs...) noexcept(noexceptInvocableC) -> ReturnT, ConfigT>;
```

An alternative to `std::function` with configurable small object optimization limit and customizable operations.

## Configuration `ConfigT`

Its member shall customize the list as decribed:

| Member | Default | Behavior |
| - | - | - |
| Type `ExtendedInformation` | `dcool::core::Pit` | See *Customized extended operations* for more details. |
| Type `Pool` | `dcool::resource::DefaultPool` | The dynamic memory resource of function. |
| Type `Engine` | *Unspecified* | Provided `Engine engine`, `engine.pool()` shall evaluate to a reference to `Pool` for dynamic memory management, and `engine.extendedOpterationExecutor` shall evaluate to a reference to `ExtendedOpterationExecutor` for extended operations (See *Customized extended operations* for more details). |
| `static constexpr dcool::core::StorageRequirement squeezedTankage` | *Unspecfied* | If the item to be stored is storable in a statically allocated storage of `squeezedTankage`, implementation would attempt to avoid dynamic allocation. |
| `static constexpr dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy` | `dcool::core::defaultExceptionSafetyStrategy` | The default exception safety strategy of all operations. |

## Member types

| Name | Definition |
| - | - |
| `ExtendedInformation` | Determined by configuration `ExtendedInformation`. |
| `Prototype` | Defined by `using Prototype = auto(ParameterTs...) noexcept(noexceptInvocableC) -> ReturnT;`. |
| `Result` | Defined by `using Result = ResultT;`. |

## Member constant

| Member | Definition |
| - | - |
| `static constexpr dcool::core::Boolean noexceptInvocable` | Equals to `noexceptInvocableC` |

## Constructors

```cpp
constexpr Function() noexcept;
constexpr Function(Function const& other);
constexpr Function(Function&& other) noexcept(/* unspecified expression */);
template <typename ValueT> constexpr Function(ValueT&& value) noexcept(/* unspecified expression */);
template <typename ValueT, typename... ArgumentTs> constexpr Function(
	dcool::core::InPlaceTag, ArgumentTs&&... parameters
) noexcept(/* unspecified expression */);
```

- Overload 1: Construct an function that does not hold an object.
- Overload 2, 3: Construct as a copy of `other` (as-if).
- Overload 4: Construct an function that holds an object of decayed `ValueT` with `value`.
- Overload 5: Construct an function that holds an object of `ValueT` with `parameters`.

## Assignments

```cpp
constexpr auto operator =(Function const& other) noexcept(/* unspecified expression */) -> Function&;
constexpr auto operator =(Function&& other) noexcept(/* unspecified expression */) -> Function&;
```

Assign as a copy of `other` (as-if).

## Non-static member functions

### `swapWith`

```cpp
constexpr void swapWith(Function& other) noexcept(/* unspecified expression */);
```

Swap with `other`.

### `valid`

```cpp
constexpr auto valid() const noexcept -> dcool::core::Boolean;
```

Returns true if the function holds an object, otherwise returns false.

### `immutablyInvocable`

```cpp
constexpr auto immutablyInvocable() const noexcept -> dcool::core::Boolean;
```

Returns true if the function can be invoked with a constant reference to this function without directly throwing a `dcool::utility::BadFunctionCall`, otherwise returns false.

Note that `dcool::utility::BadFunctionCall` indirectly thrown when invoking the holded object does not count.

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

Returns a reference to the extended information with static storage duration constructed by `ExtendedInformation(dcool::core::typed<ValueT>)` (where `ValueT` is the type of the holded object).

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

### `invokeSelf`

```cpp
constexpr auto invokeSelf(ParameterTs... parameters) const noexcept(noexceptInvocable) -> Return;
constexpr auto invokeSelf(ParameterTs... parameters) noexcept(noexceptInvocable) -> Return;
```

Equivalent to `dcool::core::invoke(this->access<ValueT>, dcool::core::forward<ParameterTs>(parameters)...)` (where `ValueT` is the type of the object holded by function) if the function holds an object, otherwise terminate execution if `noexceptInvocable`, otherwise throws a `dcool::utility::BadFunctionCall` (might be the same as `std::bad_function_call`).

For overload 1, if the const reference to the holded object is not invocable as above, terminate execution if `noexceptInvocable`, otherwise throws a `dcool::utility::BadFunctionCall`.

### `operator ()`

```cpp
constexpr auto operator ()(ParameterTs... parameters) const noexcept(noexceptInvocable) -> Return;
constexpr auto operator ()(ParameterTs... parameters) noexcept(noexceptInvocable) -> Return;
```

Equivalent to `this->invokeSelf(dcool::core::forward<ParameterTs>(parameters)...)`.

## Customized extended operations

Customizing extended operations is almost exactly the same as extending `dcool::utility::Any`, except that you should change all `dcool::utility::Any` to a proper `dcool::utility::Function` type.

## Notes

One of the major differences between `dcool::utility::Function` and `std::function` is that `std::function` acts more like a callable reference with ownership. If the stored object can only be invoked through a non-const reference, `std::function` will still commence the invocation if the `std::function` its self is const-qualified while `dcool::utility::Function` will throw an exception. If you want to emulate the behavior of `std::function` here, you can wrap your callable object in a struct and mark it `mutable`.
