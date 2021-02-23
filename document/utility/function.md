# Class template `dcool::utility::Function`

Include `<dcool/utility.hpp>` to use.

```cpp
template <typename PrototypeT_, typename ConfigT = /* unspecified type */> struct dcool::utility::Function; // Undefined
template <
	typename ConfigT, typename ReturnT, typename... ParameterTs
> struct dcool::utility::Function<ReturnT(ParameterTs...), ConfigT>;
```

An alternative to `std::function` with configurable small object optimization limit and customizable operations.

## Configuration `ConfigT`

Its member shall customize the list as decribed:

| Member | Default | Behavior |
| - | - | - |
| Scoped enumeration type `Opcode` | *Unspecified* | See *Customized extended operations* for more details. |
| Type `ExtendedOpterationExecutor` | *Unspecified* | See *Customized extended operations* for more details. |
| Type `Pool` | `dcool::resource::DefaultPool` | The dynamic memory resource of function. |
| Type `Engine` | *Unspecified* | Provided `Engine engine`, `engine.pool()` shall evaluate to a reference to `Pool` for dynamic memory management, and `engine.extendedOpterationExecutor` shall evaluate to a reference to `ExtendedOpterationExecutor` for extended operations (See *Customized extended operations* for more details). |
| `static constexpr dcool::core::StorageRequirement squeezedTankage` | *Unspecfied* | If the item to be stored is storable in a statically allocated storage of `squeezedTankage`, implementation would attempt to avoid dynamic allocation. |
| `static constexpr dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy` | `dcool::core::defaultExceptionSafetyStrategy` | The default exception safety strategy of all operations. |

## Member types

| Name | Definition |
| - | - |
| `Opcode` | Determined by configuration `Opcode`. |
| `Return` | Defined by `using Return = ReturnT;`. |

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

### `typeInfo`

```cpp
constexpr auto typeInfo() const noexcept -> dcool::core::TypeInfo const&
```

Returns the typeid of the holded object if exists, otherwise returns `typeid(void)`.

### `storageRequirement`

```cpp
constexpr auto storageRequirement() const noexcept -> dcool::core::StorageRequirement;
```

Returns the storage requirement of the holded object if exists, otherwise returns `dcool::core::storageRequirement<0, 0>`.

### `storage`

```cpp
constexpr auto storage() const noexcept -> void const*;
constexpr auto storage() const noexcept -> void*;
```

Returns a pointer to the holded object if exists, otherwise returns a null pointer.

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
constexpr auto invokeSelf(ParameterTs... parameters) const -> Return;
constexpr auto invokeSelf(ParameterTs... parameters) -> Return;
```

Equivalent to `dcool::core::invoke(this->access<ValueT>, dcool::core::forward<ParameterTs>(parameters)...)` (where `ValueT` is the type of the object holded by function) if the function holds an object, otherwise throws a `dcool::utility::BadFunctionCall` (might be the same as `std::bad_function_call`).

For overload 1, if the const reference to the holded object is not invocable as above, throws a `dcool::utility::BadFunctionCall`.

### `operator ()`

```cpp
constexpr auto operator ()(ParameterTs... parameters) const -> Return;
constexpr auto operator ()(ParameterTs... parameters) -> Return;
```

Equivalent to `this->invokeSelf(dcool::core::forward<ParameterTs>(parameters)...)`.

### `executeCustomizedOperation`

```cpp
protected: constexpr void executeCustomizedOperation(Opcode opcode, void* customizedParameter) const;
```

Equivalent to `this->mutableEngine().extendedOperationExecutor()(dcool::core::typed<T>, opcode, customizedParameter)` (where `T` is the type of the holded object or `void` if no object is holded). The behavior is undefined if the `opcode` is not an user-defined value.

See *Customized extended operations* for more details.

## Customized extended operations

Customizing extended operations is almost exactly the same as extending `dcool::utility::Any`, except that your opcode type should contain `DCOOL_UTILITY_FUNCTION_BASIC_OPERATIONS` instead of `DCOOL_UTILITY_ANY_BASIC_OPERATIONS`, and you should inherit `dcool::utility::Function` instead of ``dcool::utility::Any`.
