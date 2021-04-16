# Class template `dcool::utility::OverloadedFunction`

Include `<dcool/utility.hpp>` to use.

```cpp
template <
	dcool::core::TypeList PrototypesT_, typename ConfigT = /* unspecified type */
> struct dcool::utility::OverloadedFunction;
```

An alternative to `dcool::utility::function` that can be 'overloaded'.

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
| `Prototypes` | Defined by `using Prototypes = PrototypesT_;`. |

## Constructors

```cpp
constexpr OverloadedFunction() noexcept;
constexpr OverloadedFunction(Function const& other);
constexpr OverloadedFunction(Function&& other) noexcept(/* unspecified expression */);
template <typename ValueT> constexpr OverloadedFunction(ValueT&& value) noexcept(/* unspecified expression */);
template <typename ValueT, typename... ArgumentTs> constexpr OverloadedFunction(
	dcool::core::InPlaceTag, ArgumentTs&&... parameters
) noexcept(/* unspecified expression */);
```

- Overload 1: Construct an function that does not hold an object.
- Overload 2, 3: Construct as a copy of `other` (as-if).
- Overload 4: Construct an function that holds an object of decayed `ValueT` with `value`.
- Overload 5: Construct an function that holds an object of `ValueT` with `parameters`.

## Assignments

```cpp
constexpr auto operator =(OverloadedFunction const& other) noexcept(/* unspecified expression */) -> OverloadedFunction&;
constexpr auto operator =(OverloadedFunction&& other) noexcept(/* unspecified expression */) -> OverloadedFunction&;
```

Assign as a copy of `other` (as-if).

## Non-static member functions

### `swapWith`

```cpp
constexpr void swapWith(OverloadedFunction& other) noexcept(/* unspecified expression */);
```

Swap with `other`.

### `valid`

```cpp
constexpr auto valid() const noexcept -> dcool::core::Boolean;
```

Returns true if the function holds an object, otherwise returns false.

### `immutablyInvocable`

```cpp
template <dcool::core::Index indexC> constexpr auto immutablyInvocable() const noexcept -> dcool::core::Boolean;
```

Returns true if the function's overload version at `indexC` can be invoked with a constant reference to this function without directly throwing a `dcool::utility::BadFunctionCall`, otherwise returns false.

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
template <typename... ArgumentTs> constexpr decltype(auto) invokeSelf(ArgumentTs&&... parameters) const noexcept(/* unspecified expression */);
template <typename... ArgumentTs> constexpr decltype(auto) invokeSelf(ArgumentTs&&... parameters) noexcept(/* unspecified expression */);
```

Invoke the holded object with forwarded `parameters` if the function holds an object, otherwise terminate execution if corresponding prototype is marked `noexcept`, otherwise throws a `dcool::utility::BadFunctionCall` (might be the same as `std::bad_function_call`).

For overload 1, if the const reference to the holded object is not invocable as above, terminate execution if corresponding prototype is marked `noexcept`, otherwise throws a `dcool::utility::BadFunctionCall`.

### `operator ()`

```cpp
template <typename... ArgumentTs> constexpr decltype(auto) operator ()(ArgumentTs&&... parameters) const noexcept(/* unspecified expression */);
template <typename... ArgumentTs> constexpr decltype(auto) operator ()(ArgumentTs&&... parameters) noexcept(/* unspecified expression */);
```

Equivalent to `this->invokeSelf(dcool::core::forward<ArgumentTs>(parameters)...)`.

## Other helpers

### Type alias template `dcool::utility::DefaultOverloadedFunction`

```cpp
template <typename... PrototypeTs> using dcool::utility::DefaultOverloadedFunction = ::dcool::utility::OverloadedFunction<
	::dcool::core::Types<PrototypeTs...>
>;
```

## Example

```cpp
struct IsIntInsteadOfDouble {
	auto operator ()(int) noexcept -> dcool::core::Boolean {
		return true;
	}

	auto operator ()(double) noexcept -> dcool::core::Boolean {
		return false;
	}
};

dcool::utility::DefaultOverloadedFunction<
	auto(int) noexcept -> dcool::core::Boolean, auto(double) noexcept -> dcool::core::Boolean
> isIntInsteadOfDouble = IsIntInsteadOfDouble();

dcool::core::Boolean result1 = isIntInsteadOfDouble(1);
dcool::core::Boolean result2 = isIntInsteadOfDouble(1.0);
```

You can assume `result1` is `true` and `result2` is `false`;

## Customized extended operations

Customizing extended operations is almost exactly the same as extending `dcool::utility::Function`, except that you should change all `dcool::utility::Function` to a proper `dcool::utility::OverloadedFunction` type.
