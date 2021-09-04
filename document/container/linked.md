# Class template `dcool::container::Linked` and `dcool::container::BidirectionalLinked`

Include `<dcool/container.hpp>` to use.

```cpp
template <typename ValueT, typename ConfigT = /* unspecified type */> struct dcool::container::Linked;
template <typename ValueT, typename ConfigT = /* unspecified type */> struct dcool::container::BidirectionalLinked;
```

A sequence container that is implemented as a linked list.

## Configuration `ConfigT`

Its member shall customize the linked as decribed:

| Member | Default | Behavior |
| - | - | - |
| Type `LinkedNode` | *Unspecified* | Work as the linked list node. See *Customized node* for more details. |

## Member types

| Name | Definition |
| - | - |
| `Value` | Defined by `using Value = ValueT;`. |
| `Config` | Defined by `using Config = ConfigT;`. |
| `LinkedNode` | Determined by configuration `LinkedNode`. |
| `Handle` | Defined by `using Handle = void*;` until `dcool::resource::Pool` is stablized. |
| `Iterator` | Contiguous iterator (random access if `circular`) that can iterate through this linked. |
| `ConstIterator` | Contiguous iterator (random access if `circular`) that can iterate through this linked. No write through it permitted. |

## Member constants

| Name | Definition |
| - | - |
| `bidirectional` | Always `true` for `dcool::container::BidirectionalLinked`; only `true` for `dcool::container::Linked` if `LinkedNode` is bidirectional. |

## Constructors

```cpp
constexpr Linked() noexcept(/* unspecified expression */); // 1
constexpr BidirectionalLinked() noexcept(/* unspecified expression */); // 2
constexpr Linked(Linked const& other) noexcept(/* unspecified expression */); // 3
constexpr BidirectionalLinked(BidirectionalLinked const& other) noexcept(/* unspecified expression */); // 4
constexpr Linked(Linked&& other) noexcept(/* unspecified expression */); // 5
constexpr BidirectionalLinked(BidirectionalLinked&& other) noexcept(/* unspecified expression */); // 6
```

- Overload 1, 2: Initialize the linked with 0 items.
- Overload 3~6: Construct as a copy of `other` (as-if).

## Assignments

```cpp
constexpr auto operator =(Linked const& other) noexcept(/* unspecified expression */) -> Linked&;
constexpr auto operator =(BidirectionalLinked const& other) noexcept(/* unspecified expression */) -> BidirectionalLinked&;
constexpr auto operator =(Linked&& other) noexcept(/* unspecified expression */) -> Linked&;
constexpr auto operator =(BidirectionalLinked&& other) noexcept(/* unspecified expression */) -> BidirectionalLinked&;
```

Assign as a copy of `other` (as-if).

## Non-static member functions

### `swapWith`

```cpp
constexpr void swapWith(Linked& other) noexcept(/* unspecified expression */);
constexpr void swapWith(BidirectionalLinked& other) noexcept(/* unspecified expression */);
```

Swap with `other`.

#### Complexity

Constant.

### `vacant`

```cpp
constexpr auto vacant() const noexcept -> ::dcool::core::Boolean;
```

Returns `true` if the linked does not hold any item. Otherwise returns `false`.

#### Complexity

Constant.

### `beforeBegin`

```cpp
constexpr auto beforeBegin() const noexcept -> ConstIterator;
constexpr auto beforeBegin() noexcept -> Iterator;
```

Returns an iterator to the one-past-first item of the linked.

### `begin`

```cpp
constexpr auto begin() const noexcept -> ConstIterator;
constexpr auto begin() noexcept -> Iterator;
```

Returns an iterator to the first item of the linked.

#### Complexity

Constant.

### `end`

```cpp
constexpr auto end() const noexcept -> ConstIterator;
constexpr auto end() noexcept -> Iterator;
```

Returns an iterator to the one-past-last item of the linked.

#### Complexity

Constant.

### `emplace`

```cpp
template <typename... ArgumentTs> constexpr auto emplace(
	Iterator position, ArgumentTs&&... parameters
) noexcept(/* unspecified expression */) -> Iterator requires (bidirectional);
```

Insert a new item at `position` in-place constructed with `parameters`.

#### Complexity

Constant.

### `emplaceAfter`

```cpp
template <typename... ArgumentTs> constexpr auto emplaceAfter(
	Iterator position, ArgumentTs&&... parameters
) noexcept(/* unspecified expression */) -> Iterator;
```

Insert a new item after `position` in-place constructed with `parameters`.

#### Complexity

Constant.

### `emplaceFront`

```cpp
template <typename... ArgumentTs> constexpr void emplaceFront(
	ArgumentTs&&... parameters
) noexcept(/* unspecified expression */) requires (bidirectional);
```

If `stuffed` and `squeezedOnly`, this member is not available.

Insert a new item at the beginning of the linked in-place constructed with `parameters`.

#### Complexity

Constant.

### `emplaceBack`

```cpp
template <typename... ArgumentTs> constexpr void emplaceBack(
	ArgumentTs&&... parameters
) noexcept(/* unspecified expression */) requires (bidirectional);
```

If `stuffed` and `squeezedOnly`, this member is not available.

Insert a new item at the end of the linked in-place constructed with `parameters`.

#### Complexity

Constant.

### `erase`

```cpp
constexpr auto erase(Iterator position) noexcept -> Iterator requires (bidirectional);
```

Erase an item at `position`. Returns an iterator to the position after the last erased item.

#### Complexity

Constant.

### `eraseAfter`

```cpp
constexpr auto eraseAfter(Iterator position) noexcept -> Iterator;
```

Erase an item after `position`. Returns an iterator to the position after the last erased item.

#### Complexity

Constant.

## Customized node

Users can provide a customized node to replace the default one. The node type object should consist of a header subobject storing the link to the next node and optional previous node. The header type should be instantiatable as the valueless sentry node of the linked.

Given the following definitions:

```cpp
LinkedNode& node = /* unspecified */;
LinkedNode const& cNode = /* unspecified */;
Handle handle = /* unspecified */;
```

The following requirements need to be satisfied:

- Has a member type `Header`;
- `node.header()` evaluates to a `Header&` that refers to the header of the node.
- `cnode.header()` evaluates to a `Header const&` that refers to the header of the node.
- `cnode.header().nextHandle()` evaluates to a handle to the next node.
- `node.header().setNextHandle(handle)` change the next of `node`.
- `node.value()` evaluates to a `Value&` that refers to the item stored by the node.
- `cnode.value()` evaluates to a `Value const&` that refers to the item stored by the node.

If the node is expected to be bidirectional, it should satisfy the following additional requirements:

- `cnode.header().previousHandle()` evaluates to a handle to the previous node.
- `node.header().setPreviousHandle(handle)` change the previous of `node`.
