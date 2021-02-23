# Class template `dcool::utility::Any`

Include `<dcool/utility.hpp>` to use.

```cpp
template <typename ConfigT = /* unspecified type */> struct dcool::utility::Any;
```

An alternative to `std::any` with configurable small object optimization limit and customizable operations.

## Configuration `ConfigT`

Its member shall customize the list as decribed:

| Member | Default | Behavior |
| - | - | - |
| Scoped enumeration type `Opcode` | *Unspecified* | See *Customized extended operations* for more details. |
| Type `ExtendedOpterationExecutor` | *Unspecified* | See *Customized extended operations* for more details. |
| Type `Pool` | `dcool::resource::DefaultPool` | The dynamic memory resource of any. |
| Type `Engine` | *Unspecified* | Provided `Engine engine`, `engine.pool()` shall evaluate to a reference to `Pool` for dynamic memory management, and `engine.extendedOpterationExecutor` shall evaluate to a reference to `ExtendedOpterationExecutor` for extended operations (See *Customized extended operations* for more details). |
| `static constexpr dcool::core::StorageRequirement squeezedTankage` | *Unspecfied* | If the item to be stored is storable in a statically allocated storage of `squeezedTankage`, implementation would attempt to avoid dynamic allocation. |
| `static constexpr dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy` | `dcool::core::defaultExceptionSafetyStrategy` | The default exception safety strategy of all operations. |

## Member types

| Name | Definition |
| - | - |
| `Opcode` | Determined by configuration `Opcode`. |

## Constructors

```cpp
constexpr Any() noexcept;
constexpr Any(Any const& other);
constexpr Any(Any&& other) noexcept(/* unspecified expression */);
template <typename ValueT> constexpr Any(ValueT&& value) noexcept(/* unspecified expression */);
template <typename ValueT, typename... ArgumentTs> constexpr Any(
	dcool::core::InPlaceTag, ArgumentTs&&... parameters
) noexcept(/* unspecified expression */);
```

- Overload 1: Construct an any that does not hold an object.
- Overload 2, 3: Construct as a copy of `other` (as-if).
- Overload 4: Construct an any that holds an object of decayed `ValueT` with `value`.
- Overload 5: Construct an any that holds an object of `ValueT` with `parameters`.

## Assignments

```cpp
constexpr auto operator =(Any const& other) noexcept(/* unspecified expression */) -> Any&;
constexpr auto operator =(Any&& other) noexcept(/* unspecified expression */) -> Any&;
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

### `executeCustomizedOperation`

```cpp
protected: constexpr void executeCustomizedOperation(Opcode opcode, void* customizedParameter) const;
```

Equivalent to `this->mutableEngine().extendedOperationExecutor()(dcool::core::typed<T>, opcode, customizedParameter)` (where `T` is the type of the holded object or `void` if no object is holded). The behavior is undefined if the `opcode` is not an user-defined value.

See *Customized extended operations* for more details.

## Customized extended operations

User may inherit an instantiated 'dcool::utility::Any' to implement customized extended operations on any object with compile-time type information. We will go through an example to implement a customized any type named `MyAny` that can perform convertion to string (and requires `std::to_string` accepts the holded object as the argument).

Firstly, we need to configure our own opcode type. It should be an scoped enumeration type that has `DCOOL_UTILITY_ANY_BASIC_OPERATIONS` (which would be expanded to a list of reserved identifiers seperated by comma) inside its definition to help define the basic operations used by dcool library. Our opcode looks like this:

```cpp
enum class MyOpcode {
	DCOOL_UTILITY_ANY_BASIC_OPERATIONS,
	toString
};
```

It does not matter where you put `DCOOL_UTILITY_ANY_BASIC_OPERATIONS` in your *enumerator-list*. You need to avoid defining any enumeration constant in your opcode type that shares the same value with the basic operations, which makes it impossible to portably implement with any constant defined after `DCOOL_UTILITY_ANY_BASIC_OPERATIONS` with a designated value.

We need to define a customized parameter type to feed `executeCustomizedOperation` since only one customized parameter is allowed.

```cpp
struct MyCustomizedParameter {
	MyAny const* self;
	std::string result;
};
```

We also need to define a functor type that handles our customized opcode. Our extended opteration executor looks like this:

```cpp
class MyExtendedOperationExecutor {
	public: template <typename ValueT> void operator ()(dcool::core::TypedTag<ValueT>, MyOpcode opcode, void* parameter) {
		auto typedParameter = static_cast<MyCustomizedParameter*>(parameter);
		// Dcool any will make sure the any is currently holding an 'ValueT' (if not 'void')
		if constexpr (dcool::core::NonVoid<ValueT>) {
			typedParameter->result = std::to_string(typedParameter->self->template access<ValueT>());
		}
	}
};
```

Note that a extended operation executor should always accept three argument as above and should be properly overloaded or templated to accept all possible `ValueT`.

Now we can define the config type for any like this:

```cpp
class MyAnyConfig {
	public: using Opcode = MyOpcode;
	public: using ExtendedOperationExecutor = MyExtendedOperationExecutor;
};
```

Our any type should inherit `dcool::core::Any` to access `executeCustomizedOperation` since it is protected. The rationale is that `executeCustomizedOperation` accepts an general pointer that should be manually casted to the exact customized parameter type and should be discouraged to expose. It is also encouraged use private inheritance to avoid exposing `executeCustomizedOperation` or any other members that might not be properly implemented for your type like `swapWith`.

```cpp
class MyAny: private dcool::utility::Any<MyAnyConfig> {
	// Other members omitted

	public: auto toString() const -> std::string {
		MyCustomizedParameter parameter = { .self = this };
		this->dcool::utility::Any<MyAnyConfig>::executeCustomizedOperation(MyOpcode::toString, &parameter);
		return parameter.result;
	}
};
```

Note that the parameter should exactly match the type you casted in the extended operation executor or behavior will be undefined.

For a more complicated example of extending `dcool::utility::Any`, you may read the implementation of `dcool::utility::Function`.
