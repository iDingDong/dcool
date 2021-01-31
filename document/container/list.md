# Class template list

Include `<dcool/container.hpp>` to use.

```cpp
template <typename ValueT, typename ConfigT = /* unspecified type */> struct dcool::core::container::List;
```

A sequence container that can be configured as a dynamic or static array.

## Configuration `ConfigT`

Its member shall customize the list as decribed:

| Member | Default | Behavior |
| - | - | - |
| `static constexpr dcool::core::Length storageCapacity` | `dcool::core::dynamicExtent` | The capacity of list shall be dynamic during run time if it takes value `dcool::core::dynamicExtent`; otherwise the capacity shall be equal to `storageCapacity`. |
| `static constexpr dcool::core::Boolean stuffed` | `false` | The length of list shall be equal to capacity (always full) if it takes value `true`; otherwise the length will be seperately recorded during runtime with performance penalty. |
| `static constexpr dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy` | `dcool::core::defaultExceptionSafetyStrategy` | The default exception safety strategy of all operations. |

### Examples

Given constant `dcool::core::Length N` (assumes `N != dcool::core::dynamicExtent`):

- Given `storageCapacity = dcool::core::dynamicExtent` and `stuffed = false`, the list shall behave similarly to `std::vector<Value>`.
- Given `storageCapacity = N` and `stuffed = false`, the list shall behave similarly to `boost::static_vector<Value, N>`.
- Given `storageCapacity = N` and `stuffed = true`, the list shall behave similarly to `std::array<Value, N>`.
- Given `storageCapacity = dcool::core::dynamicExtent` and `stuffed = true`, the list shall behave behave similarly to `std::vector<Value>` except that it cannot be reserved an will always have its length equals to its capacity.

## Member types

| Name | Definition |
| - | - |
| `Value` | Defined by `using Value = ValueT;`. |
| `Config` | Defined by `using Config = ConfigT;`. |
| `Iterator` | Contigious iterator that can iterate through this list. |
| `ConstIterator` | Contigious iterator that can iterate through this list. No write to any item through it permitted. |
| `Length` | An integer type that can represent the length or capacity of the list. |
| `Index` | An integer type that can represent the index of an item in the list. |

## Member constants

| Name | type | Definition |
| - | - | - |
| `storageCapacity` | `dcool::core::Length` | Determined by configuration `storageCapacity`. |
| `capacityFixed` | `dcool::core::Boolean` | `false` if `storageCapacity` equals to `dcool::core::dynamicExtent`; otherwise `true`. |
| `stuffed` | `dcool::core::Boolean` | Determined by configuration `stuffed`. |
| `exceptionSafetyStrategy` | `dcool::core::ExceptionSafetyStrategy` | Determined by configuration `exceptionSafetyStrategy`. |

## Constructors

```cpp
constexpr List() noexcept(/* unspecified expression */); // 1
constexpr List(List const& other) noexcept(/* unspecified expression */); // 2
constexpr List(List&& other) noexcept(/* unspecified expression */); // 3
constexpr List(Length capacity) noexcept(/* unspecified expression */); // 4
template <::dcool::core::InputIterator IteratorT> constexpr List(
	::dcool::core::RangeInputTag, IteratorT otherBegin, IteratorT otherEnd
) noexcept(/* unspecified expression */); // 5
```

- Overload 1: If `capacityFixed`, initialize the list with 0 capacity; Otherwise if `stuffed`, default-initialize `storageCapacity` count of items.
- Overload 2, 3: Construct as a copy of `other` (as-if).
- Overload 4: Construct with given capacity. If `stuffed`, default-initialize `capacity` items. This overload is unavailable if `capacityFixed`.
- Overload 5: Construct by copy all items from range [`otherBegin`, `otherEnd`).

## Assignments

```cpp
constexpr auto operator =(List const&) noexcept(/* unspecified expression */) -> List&;
constexpr auto operator =(List&&) noexcept(/* unspecified expression */) -> List&;
```

Assign as a copy of `other` (as-if).

## Non-static member functions

### `swapWith`

```cpp
constexpr void swapWith(Self_& other) noexcept(/* unspecified expression */);
```

Swap with `other`.

### `vacant`

```cpp
constexpr auto vacant() const noexcept -> ::dcool::core::Boolean;
```

Returns `true` if the list does not hold any item. Otherwise returns `false`.

### `full`

```cpp
constexpr auto full() const noexcept -> ::dcool::core::Boolean;
```

Returns `true` if the length of list equals to its capacity. Otherwise returns `false`.

### `length`

```cpp
constexpr auto length() const noexcept -> Length;
```

Returns the length of the list.

### `capacity`

```cpp
constexpr auto capacity() const noexcept -> Length;
```

Returns the capacity of the list.

### `begin`

```cpp
constexpr auto begin() const noexcept -> ConstIterator;
constexpr auto begin() noexcept -> Iterator;
```

Returns an iterator to the first item of the list.

### `end`

```cpp
constexpr auto end() const noexcept -> ConstIterator;
constexpr auto end() noexcept -> Iterator;
```

Returns an iterator to the one-past-last item of the list.

### `operator []`

```cpp
constexpr auto operator [](Index index) const noexcept -> Value const&;
constexpr auto operator [](Index index) noexcept -> Value&;
```

Returns a reference to the item at index `index`.

### `emplace`

```cpp
template <
	::dcool::core::ExceptionSafetyStrategy strategyC, typename... ArgumentTs
> constexpr auto emplace(Iterator position, ArgumentTs&&... parameters) noexcept(/* unspecified expression */) -> Iterator; // 1
template <typename... ArgumentTs> constexpr auto emplace(
	Iterator position, ArgumentTs&&... parameters
) noexcept(/* unspecified expression */) -> Iterator; // 2
```

If `stuffed` and `capacityFixed`, this member is not available.

Insert a new item at `position` in-place constructed with `parameters`. Overload 1 will use `strategyC` instead of the default exception safety strategy.

### `emplaceBack`

```cpp
template <
	::dcool::core::ExceptionSafetyStrategy strategyC, typename... ArgumentTs
> constexpr void emplaceBack(ArgumentTs&&... parameters) noexcept(/* unspecified expression */); // 1
template <typename... ArgumentTs> constexpr void emplaceBack(
	ArgumentTs&&... parameters
) noexcept(/* unspecified expression */); // 2
```

If `stuffed` and `capacityFixed`, this member is not available.

Insert a new item at the end of list in-place constructed with `parameters`. Overload 1 will use `strategyC` instead of the default exception safety strategy.

### `erase`

```cpp
template <
	::dcool::core::ExceptionSafetyStrategy strategyC = exceptionSafetyStrategy
> constexpr auto erase(Iterator position) noexcept(/* unspecified expression */) -> Iterator;
```

If `stuffed` and `capacityFixed`, this member is not available.

Erase an item at `position`. Use `strategyC` instead of the default exception safety strategy.

### `popBack`

```cpp
template <
	::dcool::core::ExceptionSafetyStrategy strategyC = exceptionSafetyStrategy
> constexpr void popBack() noexcept(/* unspecified expression */);
```

If `stuffed` and `capacityFixed`, this member is not available.

Erase the last item of the list. Use `strategyC` instead of the default exception safety strategy.