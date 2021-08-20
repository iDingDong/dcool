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
| Type `ExtendedInformation` | `dcool::core::Pit` | See *Customized extended operations* for more details. |
| Type `Pool` | `dcool::resource::DefaultPool` | The dynamic memory resource of any. |
| Type `Engine` | *Unspecified* | Provided `Engine engine`, `engine.pool()` shall evaluate to a reference to `Pool` for dynamic memory management. |
| `static constexpr dcool::core::Boolean copyable` | `true` | The any shall be copyable if it takes value `true`; otherwise non-copyable. |
| `static constexpr dcool::core::Boolean movable` | *Same as* `copyable` | The any shall be moveable if it takes value `true`; otherwise non-moveable. |
| `static constexpr dcool::core::StorageRequirement squeezedTankage` | *Unspecfied* | If the item to be stored is storable in a statically allocated storage of `squeezedTankage`, implementation would attempt to avoid dynamic allocation. |
| `static constexpr dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy` | `dcool::core::defaultExceptionSafetyStrategy` | The default exception safety strategy of all operations. |

## Member types

| Name | Definition |
| - | - |
| `ExtendedInformation` | Determined by configuration `ExtendedInformation`. |

## Constructors

```cpp
constexpr Any() noexcept;
constexpr Any(Any const& other);
constexpr Any(Any&& other) noexcept(/* unspecified expression */);
template <typename ValueT> constexpr Any(ValueT&& value) noexcept(/* unspecified expression */);
template <typename ValueT, typename... ArgumentTs> constexpr Any(
	dcool::core::TypedInPlaceTag<ValueT>, ArgumentTs&&... parameters
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

## Customized extended operations

User may inherit from or composite with an instantiated 'dcool::utility::Any' to implement customized extended operations on any object with compile-time type information. You will need to configure the `ExtendedInformation` type to hold constant data related to the stored object type.

### Example

We will implement a customized any type named `MyAny` that can perform convertion to string (and requires `std::to_string` accepts the holded object as the argument).

```cpp
class MyAny {
	// Firstly, we need to define a function that can convert an any with statically known holded object type to string.
	template <typename ValueT> static auto myToString(MyAny const& myAny) -> std::string {
		return std::to_string(myAny.m_underlying.access<ValueT>());
	}

	// We then need to define a customized 'virtual table' as our extended information.
	struct MyVirtualTable {
		using ConverterToString = auto (*)(MyAny const&) -> std::string;
		// Intended to mimic a virtual function.
		ConverterToString converterToString;

		// The extended information is required to be constructible with a single argument of type `dcool::core::TypedTag<ValueT>`
		// and should be properly overloaded or templated to accept all possibly holded object types.
		template <typename ValueT> MyVirtualTable(dcool::core::TypedTag<ValueT>) noexcept: converterToString(myToString<ValueT>) {
		}
	};

	// Now we need to define a config type for 'dcool::utility::Any'.
	class MyAnyConfig {
		public: using ExtendedInformation = MyVirtualTable;
	};

	// Other possible members are omitted.
	private: dcool::utility::Any<MyAnyConfig> m_underlying;

	// Constructor of `dcool::utility::Any` will store a reference to an constant `MyVirtualTable` object constructed with
	// `dcool::core::typed<ValueT>`.
	public: template <typename ValueT> MyAny(ValueT const& value): m_underlying(value) {
	}

	public: auto toString() const -> std::string {
		return this->m_underlying.extendedInformation().converterToString(*this);
	}
};
```

For a more complicated example of extending `dcool::utility::Any`, you may read the implementation of `dcool::utility::Function`.
