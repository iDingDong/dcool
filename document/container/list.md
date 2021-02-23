# Class template `dcool::container::List`

Include `<dcool/container.hpp>` to use.

```cpp
template <typename ValueT, typename ConfigT = /* unspecified type */> struct dcool::container::List;
```

A sequence container that is stored in a contiguous storage.

## Terms

### squeezed

If all items are stored inside a size-fixed storage, we call the list a squeezed list.

## Configuration `ConfigT`

Its member shall customize the list as decribed:

| Member | Default | Behavior |
| - | - | - |
| `static constexpr dcool::core::Boolean squeezedOnly` | `false` | All items will only be stored inside a size-fixed storage (prefers static allocation) if it takes value `true`; otherwise items will be stored in an dynamically allocated storage if capacity exceeds `squeezedCapacity`. |
| `static constexpr dcool::core::Length squeezedCapacity` | 0 | The capacity provided by a size-fixed storage shall be equal to `squeezedCapacity`. Behavior is undefined if `squeezedOnly` takes value true and `squeezedCapacity` takes value 0. which means `squeezedCapacity` is always expected to be user-defined if `squeezedOnly`. |
| `static constexpr dcool::core::Boolean stuffed` | `false` | The length of list shall be equal to capacity (always full) if it takes value `true`; otherwise the length will be seperately recorded during runtime with performance penalty. |
| `static constexpr dcool::core::Boolean circular` | `false` | The list will behave as a ring buffer, reducing the runtime cost to insert or erase near the front of the list (if not `stuffed`) at the cost of perfomance penalty on other list operations if it takes value `true`; otherwise the list items will always be stored contiguously. |
| `static constexpr dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy` | `dcool::core::defaultExceptionSafetyStrategy` | The default exception safety strategy of all operations. |

### Note

If the list has a `squeezedCapacity` greater than 0, implementation may allocate the size-fixed storage inside the `dcool::container::List` object, which means the `dcool::container::List` object might be huge if `squeezedCapacity` takes a large value. Also, squeezed list might cost more to move or swap. Operations on a possibly squeezed list might cost more to guarantee exception safety, or have to downgrade to weak exception safety on certain occasions.

### Examples

Given constant `dcool::core::Length N` (assumes `N > 0` is true):

- Given `squeezedOnly = false`, `squeezedCapacity = 0` and `stuffed = false`, the list shall behave similarly to `std::vector<Value>`.
- Given `squeezedOnly = false`, `squeezedCapacity = N` and `stuffed = false`, the list shall behave similarly to `boost::small_vector<Value, N>`.
- Given `squeezedOnly = true`, `squeezedCapacity = N` and `stuffed = false`, the list shall behave similarly to `boost::static_vector<Value, N>`.
- Given `squeezedOnly = true`, `squeezedCapacity = N` and `stuffed = true`, the list shall behave similarly to `std::array<Value, N>`.
- Given `squeezedOnly = false`, `squeezedCapacity = 0` and `stuffed = true`, the list shall behave behave similarly to `std::vector<Value>` except that it cannot be reserved an will always have its length equals to its capacity.

## Member types

| Name | Definition |
| - | - |
| `Value` | Defined by `using Value = ValueT;`. |
| `Config` | Defined by `using Config = ConfigT;`. |
| `Iterator` | Contiguous iterator (random access if `circular`) that can iterate through this list. |
| `ConstIterator` | Contiguous iterator (random access if `circular`) that can iterate through this list. No write through it permitted. |
| `Length` | An integer type that can represent the length or capacity of the list. |
| `Index` | An integer type that can represent the index of an item in the list. |

## Member constants

| Name | type | Definition |
| - | - | - |
| `squeezedOnly` | `dcool::core::Boolean` | Determined by configuration `squeezedOnly`. |
| `squeezedCapacity` | `dcool::core::Length` | Determined by configuration `squeezedCapacity`. |
| `stuffed` | `dcool::core::Boolean` | Determined by configuration `stuffed`. |
| `circular` | `dcool::core::Boolean` | Determined by configuration `circular`. |
| `exceptionSafetyStrategy` | `dcool::core::ExceptionSafetyStrategy` | Determined by configuration `exceptionSafetyStrategy`. |

## Constructors

```cpp
constexpr List() noexcept(/* unspecified expression */); // 1
constexpr List(List const& other) noexcept(/* unspecified expression */); // 2
constexpr List(List&& other) noexcept(/* unspecified expression */); // 3
constexpr List(Length capacity) noexcept(/* unspecified expression */); // 4
template <dcool::core::InputIterator IteratorT> constexpr List(
	dcool::core::RangeInputTag, IteratorT otherBegin, Length count
) noexcept(/* unspecified expression */); // 5
template <dcool::core::InputIterator IteratorT> constexpr List(
	dcool::core::RangeInputTag, IteratorT otherBegin, IteratorT otherEnd
) noexcept(/* unspecified expression */); // 6
```

- Overload 1: If `squeezedOnly`, initialize the list with 0 capacity; Otherwise if `stuffed`, default-initialize `squeezedCapacity` count of items.
- Overload 2, 3: Construct as a copy of `other` (as-if).
- Overload 4: Construct with capacity no less than argument `capacity`. If `stuffed`, the capacity of the constructed list would be exactly `capacity`, and `capacity` number of items will be default-initialized. This overload is unavailable if `squeezedOnly`.
- Overload 5: Construct by copy all items from range [`otherBegin`, `otherEnd`) where `otherEnd` is `count` step ahead of `otherBegin`. This overload is unavailable if `squeezedOnly` and `stuffed`.
- Overload 6: Construct by copy all items from range [`otherBegin`, `otherEnd`). This overload is unavailable if `squeezedOnly` and `stuffed`.

## Assignments

```cpp
constexpr auto operator =(List const& other) noexcept(/* unspecified expression */) -> List&;
constexpr auto operator =(List&& other) noexcept(/* unspecified expression */) -> List&;
```

Assign as a copy of `other` (as-if).

## Non-static member functions

### `swapWith`

```cpp
constexpr void swapWith(List& other) noexcept(/* unspecified expression */);
```

Swap with `other`.

#### Complexity

Linear if `squeezedOnly`, otherwise constant.

### `vacant`

```cpp
constexpr auto vacant() const noexcept -> ::dcool::core::Boolean;
```

Returns `true` if the list does not hold any item. Otherwise returns `false`.

#### Complexity

Constant.

### `full`

```cpp
constexpr auto full() const noexcept -> ::dcool::core::Boolean;
```

Returns `true` if the length of list equals to its capacity. Otherwise returns `false`.

#### Complexity

Constant.

### `length`

```cpp
constexpr auto length() const noexcept -> Length;
```

Returns the length of the list.

#### Complexity

Constant.

### `capacity`

```cpp
constexpr auto capacity() const noexcept -> Length;
```

Returns the capacity of the list.

#### Complexity

Constant.

### `begin`

```cpp
constexpr auto begin() const noexcept -> ConstIterator;
constexpr auto begin() noexcept -> Iterator;
```

Returns an iterator to the first item of the list.

#### Complexity

Constant.

### `end`

```cpp
constexpr auto end() const noexcept -> ConstIterator;
constexpr auto end() noexcept -> Iterator;
```

Returns an iterator to the one-past-last item of the list.

#### Complexity

Constant.

### `operator []`

```cpp
constexpr auto operator [](Index index) const noexcept -> Value const&;
constexpr auto operator [](Index index) noexcept -> Value&;
```

Returns a reference to the item at index `index`.

#### Complexity

Constant.

### `emplace`

```cpp
template <
	::dcool::core::ExceptionSafetyStrategy strategyC, typename... ArgumentTs
> constexpr auto emplace(Iterator position, ArgumentTs&&... parameters) noexcept(/* unspecified expression */) -> Iterator; // 1
template <typename... ArgumentTs> constexpr auto emplace(
	Iterator position, ArgumentTs&&... parameters
) noexcept(/* unspecified expression */) -> Iterator; // 2
```

If `stuffed` and `squeezedOnly`, this member is not available.

Insert a new item at `position` in-place constructed with `parameters`. Overload 1 will use `strategyC` instead of the default exception safety strategy.

Invalidates all previously obtained iterators.

#### Complexity

Linear if full and cannot in-place expand, otherwise linear by distance to either boundary of the list if circular, otherwise linear by distance to the end of the list if circular.

### `emplaceFront`

```cpp
template <
	::dcool::core::ExceptionSafetyStrategy strategyC, typename... ArgumentTs
> constexpr void emplaceFront(ArgumentTs&&... parameters) noexcept(/* unspecified expression */); // 1
template <typename... ArgumentTs> constexpr void emplaceFront(
	ArgumentTs&&... parameters
) noexcept(/* unspecified expression */); // 2
```

If `stuffed` and `squeezedOnly`, this member is not available.

Insert a new item at the beginning of list in-place constructed with `parameters`. Overload 1 will use `strategyC` instead of the default exception safety strategy.

Invalidates all previously obtained iterators.

#### Complexity

Linear if full and cannot in-place expand, otherwise constant if `circular`, otherwise linear.

### `emplaceBack`

```cpp
template <
	::dcool::core::ExceptionSafetyStrategy strategyC, typename... ArgumentTs
> constexpr void emplaceBack(ArgumentTs&&... parameters) noexcept(/* unspecified expression */); // 1
template <typename... ArgumentTs> constexpr void emplaceBack(
	ArgumentTs&&... parameters
) noexcept(/* unspecified expression */); // 2
```

If `stuffed` and `squeezedOnly`, this member is not available.

Insert a new item at the end of list in-place constructed with `parameters`. Overload 1 will use `strategyC` instead of the default exception safety strategy.

Invalidates all previously obtained iterators.

#### Complexity

Linear if full and cannot in-place expand, otherwise constant.

### `pushFront`

```cpp
template <
	::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
> constexpr void pushFront(Value const& value);
template <
	::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
> constexpr void pushFront(Value&& value);
```

If `stuffed` and `squeezedOnly`, this member is not available.

Insert a new item at the beginning of list in-place constructed with `value`. Overload 1 will use `strategyC` instead of the default exception safety strategy.

Invalidates all previously obtained iterators.

#### Complexity

Linear if full and cannot in-place expand, otherwise constant if `circular`, otherwise linear.

### `pushBack`

```cpp
template <
	::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
> constexpr void pushBack(Value const& value);
template <
	::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
> constexpr void pushBack(Value&& value);
```

If `stuffed` and `squeezedOnly`, this member is not available.

Insert a new item at the end of list in-place constructed with `value`. Overload 1 will use `strategyC` instead of the default exception safety strategy.

Invalidates all previously obtained iterators.

#### Complexity

Linear if full and cannot in-place expand, otherwise constant.

### `erase`

```cpp
template <
	::dcool::core::ExceptionSafetyStrategy strategyC = exceptionSafetyStrategy
> constexpr auto erase(Iterator position) noexcept(/* unspecified expression */) -> Iterator;
```

If `stuffed` and `squeezedOnly`, this member is not available.

Erase an item at `position`. Use `strategyC` instead of the default exception safety strategy.

Invalidates all previously obtained iterators.

#### Complexity

Linear if `stuffed`, otherwise linear if `dcool::core::atAnyCost(strategyC)` and not `squeezedOnly`, otherwise linear by distance to either boundary of the list if `circular`, otherwise linear by distance to the end of the list if circular.

### `popFront`

```cpp
template <
	::dcool::core::ExceptionSafetyStrategy strategyC = exceptionSafetyStrategy
> constexpr void popFront() noexcept(/* unspecified expression */);
```

If `stuffed` and `squeezedOnly`, this member is not available.

Erase the first item of the list. Use `strategyC` instead of the default exception safety strategy.

Invalidates all previously obtained iterators.

#### Complexity

Linear if `stuffed`, otherwise linear if `dcool::core::atAnyCost(strategyC)` and not `squeezedOnly`, otherwise constant if `circular`, otherwise linear.

### `popBack`

```cpp
template <
	::dcool::core::ExceptionSafetyStrategy strategyC = exceptionSafetyStrategy
> constexpr void popBack() noexcept(/* unspecified expression */);
```

If `stuffed` and `squeezedOnly`, this member is not available.

Erase the last item of the list. Use `strategyC` instead of the default exception safety strategy.

Invalidates all previously obtained iterators.

#### Complexity

Linear if `stuffed`, otherwise constant.
