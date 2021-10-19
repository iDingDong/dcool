# Class template `dcool::concurrency::Atom`

Include `<dcool/concurrency.hpp>` to use.

```cpp
template <typename ValueT, typename ConfigT = /* unspecified type */> struct dcool::concurrency::Atom;
```

A custumizeable atomic-like type with `ValueT` holded.

`ValueT` is required to be trivially copyable.

## Configuration `ConfigT`

Its member shall customize the list as decribed:

| Member | Default | Behavior |
| - | - | - |
| `static constexpr dcool::core::Triboolean atomic` | `dcool::core::determinateTrue` | Supports all atomic operations and simulate non-atomic operations if it takes value `dcool::core::determinateTrue`; otherwise supports all kinds of operations if it takes value `dcool::core::indeterminate`; otherwise supports non-atomic operations only. |
| `static constexpr dcool::core::Length stampWidth` | `0` | Indicates the width of stamp in the atomic which would increment every time when the value of atom changes. |

### Note

`stampWidth` greater than 0 is intended to be helpful against ABA problems.

## Member types

| Name | Definition |
| - | - |
| `Value` | Defined by `using Value = ValueT;`. |
| `Stamp` | Defined by `using Stamp = dcool::core::UnsignedInteger<stampWidth>`. |
| `StampedValue` | See below. |

## Member constants

| `static constexpr dcool::core::Triboolean lockFree` | Implementation-defined. |
| `static constexpr dcool::core::Boolean is_always_lock_free` | Defined by `static constexpr dcool::core::Boolean is_always_lock_free = lockFree.isDeterminateTrue()` |

`StampedValue` is defined by:

```cpp
struct StampedValue {
	Value value;
	Stamp stamp;
};
```

## Member constants

| Name | Definition |
| - | - |
| `static constexpr dcool::core::Triboolean atomic` | Determined by configuration. |
| `static constexpr dcool::core::Length stampWidth` | Determined by configuration. |
| `static constexpr dcool::core::Boolean stamped` | Defined by `static constexpr dcool::core::Boolean stamped = (stampWidth > 0)`. |

## Constructors

```cpp
Atom() noexcept; // 1
Atom(Atom const&) = delete;
Atom(Atom&&) = delete;
Atom(Value const initialValue) noexcept; // 2
```

- Overload 1: Construct with holded value default-initialized.
- Overload 2: Construct with `initialValue` holded.

## Assignments

```cpp
auto operator =(Atom const&) -> Atom& = delete;
auto operator =(Atom&&) -> Atom& = delete;
```

## Optionally atomic member functions

### Overview

All `atomically*` members are atomic operations. Requires `!(atomic.isDeterminateFalse())`;

All `sequenced*` members are non-atomic operations. If `atomic.isDeterminateTrue()`, these operations might be simulated by a non-thread-safe serial of atomic sub-operations which is more efficient than atomic versions. If not `atomic.isDeterminateTrue()`, these operations could be even more efficient. Note that these members does not take any memory order arguments.

All other operations will call the corresponding atomic versions if `!(atomic.isDeterminateFalse())`, otherwise the non-atomic versions are called.

The `atomically*` and `sequenced*` versions are omitted in this document. Unless otherwise specified, these operations are provided correspondingly for all optionally atomic member functions except for the overloaded operators and versions marked for standard compatibility. For example, the documented member `load` has the following omitted members acompanied:

```cpp
constexpr auto atomicallyLoad(std::memory_order order = std::memory_order::seq_cst) const noexcept -> Value;
constexpr auto sequencedLoad() const noexcept -> Value;
```

Note the camel case naming.

Non-atomic operations should be handled with extreme care. Behavior is undefined if any non-atomic operation neither strongly happens before nor happens after another atomic operation (read-only operations are no except due to ISO/IEC 14882:2020 `std::atomic_ref` constraints). Behavior is undefined if any non-atomic write operation neither strongly happens before nor happens after another operation.

### Load operations

```cpp
constexpr auto loadStamped(
	std::memory_order order = std::memory_order::seq_cst
) const noexcept -> StampedValue requries (stamped);
constexpr auto load(std::memory_order order = std::memory_order::seq_cst) const noexcept -> Value;
```

Loads from the atom with memory order `order`.

Atomic versions are signal safe if `atomic.isDeterminateTrue() && lockFreeOnExecution()`.

### Conversion to `Value`

```cpp
constexpr operator Value() const noexcept;
```

Returns `this->load()`.

### Store operations

```cpp
constexpr void stampedStore(Value desired, std::memory_order order = std::memory_order::seq_cst)& noexcept requries (stamped);
constexpr void store(Value desired, std::memory_order order = std::memory_order::seq_cst)& noexcept;
```

Stores `desired` into the atom with memory order `order`.

Atomic versions are signal safe if `atomic.isDeterminateTrue() && lockFreeOnExecution() && (!stamped)`.

### Exchange operations

```cpp
constexpr void stampedExchangeStamped(
	Value desired, std::memory_order order = std::memory_order::seq_cst
)& noexcept -> StampedValue requries (stamped);
constexpr void stampedExchange(
	Value desired, std::memory_order order = std::memory_order::seq_cst
)& noexcept requries (stamped);
constexpr void exchange(Value desired, std::memory_order order = std::memory_order::seq_cst)& noexcept;
```

Stores `desired` into the atom with memory order `order`. Return the previously holded (stamped) value.

Atomic versions are signal safe if `atomic.isDeterminateTrue() && lockFreeOnExecution() && (!stamped)`.

### CAS operations

```cpp
constexpr auto stampedCompareExchangeStampedWeak(
	StampedValue& expected, Value const& desired_, std::memory_order order_ = std::memory_order::seq_cst
)& noexcept -> ::dcool::core::Boolean requires (stamped);
constexpr auto stampedCompareExchangeStampedWeak(
	StampedValue& expected, Value const& desired_, std::memory_order successOrder, std::memory_order failureOrder
)& noexcept -> ::dcool::core::Boolean requires (stamped);
constexpr auto stampedCompareExchangeWeak(
	Value& expected, Value const& desired, std::memory_order order = std::memory_order::seq_cst
)& noexcept -> ::dcool::core::Boolean requires (stamped);
constexpr auto stampedCompareExchangeWeak(
	Value& expected, Value const& desired, std::memory_order successOrder, std::memory_order failureOrder
)& noexcept -> ::dcool::core::Boolean requires (stamped);
constexpr auto compareExchangeWeak(
	Value& expected, Value const& desired, std::memory_order order = std::memory_order::seq_cst
)& noexcept -> ::dcool::core::Boolean;
constexpr auto compareExchangeWeak(
	Value& expected, Value const& desired, std::memory_order successOrder, std::memory_order failureOrder
)& noexcept -> ::dcool::core::Boolean;
constexpr auto compare_exchange_weak(
	Value& expected, Value const& desired, std::memory_order order = std::memory_order::seq_cst
)& noexcept -> ::dcool::core::Boolean; // For standard compatibility.
constexpr auto compare_exchange_weak(
	Value& expected, Value const& desired, std::memory_order successOrder, std::memory_order failureOrder
)& noexcept -> ::dcool::core::Boolean; // For standard compatibility.
constexpr auto stampedCompareExchangeStampedStrong(
	StampedValue& expected, Value const& desired_, std::memory_order order_ = std::memory_order::seq_cst
)& noexcept -> ::dcool::core::Boolean requires (stamped);
constexpr auto stampedCompareExchangeStampedStrong(
	StampedValue& expected, Value const& desired_, std::memory_order successOrder, std::memory_order failureOrder
)& noexcept -> ::dcool::core::Boolean requires (stamped);
constexpr auto stampedCompareExchangeStrong(
	Value& expected, Value const& desired, std::memory_order order = std::memory_order::seq_cst
)& noexcept -> ::dcool::core::Boolean requires (stamped);
constexpr auto stampedCompareExchangeStrong(
	Value& expected, Value const& desired, std::memory_order successOrder, std::memory_order failureOrder
)& noexcept -> ::dcool::core::Boolean requires (stamped);
constexpr auto compareExchangeStrong(
	Value& expected, Value const& desired, std::memory_order order = std::memory_order::seq_cst
)& noexcept -> ::dcool::core::Boolean);
constexpr auto compareExchangeStrong(
	Value& expected, Value const& desired, std::memory_order successOrder, std::memory_order failureOrder
)& noexcept -> ::dcool::core::Boolean;
constexpr auto compare_exchange_strong(
	Value& expected, Value const& desired, std::memory_order order = std::memory_order::seq_cst
)& noexcept -> ::dcool::core::Boolean; // For standard compatibility.
constexpr auto compare_exchange_strong(
	Value& expected, Value const& desired, std::memory_order successOrder, std::memory_order failureOrder
)& noexcept -> ::dcool::core::Boolean; // For standard compatibility.
```

Stores `desired` into the atom with memory order `order`/`successOrder` and returns `true` if `expected` and the stored (stamped) value has same value representation; otherwise loads the holded (stamped) value into `expected` with memory order `order`/`failureOrder` and returns `false`.

The weak versions may spuriously fail but can be more efficient.

Atomic versions are signal safe if `atomic.isDeterminateTrue() && lockFreeOnExecution() && (!stamped)`.

### Fetch transform operations

```cpp
template <typename TransformerT> constexpr auto stampedFetchTransformStamped(
	TransformerT&& transformer, std::memory_order order = std::memory_order::seq_cst
)& noexcept -> StampedValue requires (stamped);
template <typename TransformerT> constexpr auto stampedFetchTransformStamped(
	TransformerT&& transformer, std::memory_order transformOrder, std::memory_order loadOrder
)& noexcept -> StampedValue requires (stamped);
template <typename TransformerT> constexpr auto stampedFetchTransform(
	TransformerT&& transformer, std::memory_order order = std::memory_order::seq_cst
)& noexcept -> Value requires (stamped);
template <typename TransformerT> constexpr auto stampedFetchTransform(
	TransformerT&& transformer, std::memory_order transformOrder, std::memory_order loadOrder
)& noexcept -> Value requires (stamped);
template <typename TransformerT> constexpr auto fetchTransform(
	TransformerT&& transformer, std::memory_order order = std::memory_order::seq_cst
)& noexcept -> Value;
template <typename TransformerT> constexpr auto fetchTransform(
	TransformerT&& transformer, std::memory_order transformOrder, std::memory_order loadOrder
)& noexcept -> Value;
```

Transforms the previously holded value `previousValue` loaded with memory order `order`/`loadOrder` to `dcool::core::invoke(transformer, dcool::core::constantize(previousValue))` (`transformer` might be forwarded on the last invocation) with memory order `order`/`transformOrder` and returns `previousValue`. This operation will be reattempted if the previous value changes during this operation.

Evaluation of `dcool::core::invoke(transformer, dcool::core::constantize(previousValue))` is not part of the atomicity.

Note that if the same value was written into the atom by another operation, the value is not considered to be changed unless `stamped` and the stamp did not wrap to the same value.

For each `fetchTransform`/`FetchTransform` operation there is a corresponding `transformFetch`/`TransformFetch` operation which operates identically except that the `transformFetch`/`TransformFetch` operation returns the value after the modification.

### Fetch transform or load operations

```cpp
template <typename TransformerT> constexpr auto stampedFetchTransformOrLoadStamped(
	TransformerT&& transformer, std::memory_order order = std::memory_order::seq_cst
)& noexcept -> StampedValue requires (stamped);
template <typename TransformerT> constexpr auto stampedFetchTransformOrLoadStamped(
	TransformerT&& transformer, std::memory_order transformOrder, std::memory_order loadOrder
)& noexcept -> StampedValue requires (stamped);
template <typename TransformerT> constexpr auto stampedFetchTransformOrLoad(
	TransformerT&& transformer, std::memory_order order = std::memory_order::seq_cst
)& noexcept -> Value requires (stamped);
template <typename TransformerT> constexpr auto stampedFetchTransformOrLoad(
	TransformerT&& transformer, std::memory_order transformOrder, std::memory_order loadOrder
)& noexcept -> Value requires (stamped);
template <typename TransformerT> constexpr auto fetchTransformOrLoad(
	TransformerT&& transformer, std::memory_order order = std::memory_order::seq_cst
)& noexcept -> Value;
template <typename TransformerT> constexpr auto fetchTransformOrLoad(
	TransformerT&& transformer, std::memory_order transformOrder, std::memory_order loadOrder
)& noexcept -> Value;
```

With previously holded value `previousValue` loaded with memory order `order`/`loadOrder`, executes `auto newValue = dcool::core::invoke(transformer, dcool::core::constantize(previousValue))` (`transformer` might be forwarded on the last invocation), transforms the previously holded value to `newValue.access()` with memory order `order`/`transformOrder` if `newValue.valid()`. Returns `previousValue`. This operation will be reattempted if the previous value changes during this operation unless `!(newValue.valid())`.

Evaluation of `dcool::core::invoke(transformer, dcool::core::constantize(previousValue))`, `newValue.valid()` and `newValue.access()` are not part of the atomicity.

Note that if the same value was written into the atom by another operation, the value is not considered to be changed unless `stamped` and the stamp did not wrap to the same value.

For each `fetchTransformOrLoad`/`FetchTransformOrLoad` operation there is a corresponding `transformFetchOrLoad`/`TransformFetchOrLoad` operation which operates identically except that the `transformFetchOrLoad`/`TransformFetchOrLoad` operation returns the value after the modification.

### Wait operations

```cpp
constexpr void stampedWaitStamped(
	StampedValue const& old, std::memory_order order = ::std::memory_order::seq_cst
) const noexcept requires ((!(atomic.isDeterminateFalse())) && stamped)
constexpr void stampedWait(
	Value const& old, std::memory_order order = ::std::memory_order::seq_cst
) const noexcept requires ((!(atomic.isDeterminateFalse())) && stamped)
constexpr void wait(
	Value const& old, std::memory_order order = ::std::memory_order::seq_cst
) const noexcept requires (!(atomic.isDeterminateFalse()));
```

If the holded (stamped) value loaded with memory order `order` has the same representation as `old`, blocks until notified or spuriously waked up and repeat the operation.

These operations is intended to be no less efficient than a spinning check. With ISO/IEC 14882:2020 atomic wait operations, these operations can be even more efficient.

No non-atomic versions are provided.

### Wait fetch operations

```cpp
constexpr auto stampedWaitFetchStamped(
	StampedValue const& old, std::memory_order order = ::std::memory_order::seq_cst
) const noexcept -> StampedValue requires ((!(atomic.isDeterminateFalse())) && stamped)
constexpr auto stampedWaitFetch(
	Value const& old, std::memory_order order = ::std::memory_order::seq_cst
) const noexcept -> Value requires ((!(atomic.isDeterminateFalse())) && stamped)
constexpr auto waitFetch(
	Value const& old, std::memory_order order = ::std::memory_order::seq_cst
) const noexcept -> Value requires (!(atomic.isDeterminateFalse()));
```

If the holded (stamped) value `previousValue` loaded with memory order `order` has the same representation as `old`, blocks until notified or spuriously waked up and repeat the operation.

Returns the last loaded `previousValue`.

These operations is intended to be no less efficient than a spinning check. With ISO/IEC 14882:2020 atomic wait operations, these operations can be even more efficient.

No non-atomic versions are provided.

### Wait predicate operations

```cpp
template <typename PredicateT> constexpr void stampedWaitPredicateStamped(
	PredicateT&& predicate, std::memory_order order = ::std::memory_order::seq_cst
) const noexcept requires ((!(atomic.isDeterminateFalse())) && stamped);
template <typename PredicateT> constexpr void stampedWaitPredicate(
	PredicateT&& predicate, std::memory_order order = ::std::memory_order::seq_cst
) const noexcept requires ((!(atomic.isDeterminateFalse())) && stamped);
template <typename PredicateT> constexpr void waitPredicate(
	PredicateT&& predicate, std::memory_order order = ::std::memory_order::seq_cst
) const noexcept requires (!(atomic.isDeterminateFalse()));
```

If the holded (stamped) value `previousValue` loaded with memory order `order` does not satisfy `dcool::core::invoke(predicate, dcool::core::constantize(previousValue))` (`predicate` might be forwarded on the last invocation), blocks until notified or spuriously waked up and repeat the operation.

These operations is intended to be no less efficient than a spinning check. With ISO/IEC 14882:2020 atomic wait operations, these operations can be even more efficient.

No non-atomic versions are provided.

### Notify operations

```cpp
constexpr void notifyOne() noexcept requires(!(atomic.isDeterminateFalse()));
constexpr void notifyAll() noexcept requires(!(atomic.isDeterminateFalse()));
```

Notify one/all waiter(s) on this atom.

### Fetch arithmetic operations

```cpp
template <typename OperandT> constexpr auto fetch*Stamped(
	OperandT const& operand, std::memory_order order = std::memory_order::seq_cst
) requires (stamped);
template <typename OperandT> constexpr auto fetch*(
	OperandT const& operand, std::memory_order order = std::memory_order::seq_cst
);
template <typename OperandT> constexpr auto fetch_*(
	OperandT const& operand, std::memory_order order = std::memory_order::seq_cst
); // For standard compatibility.
```

Evaluate a binary operator expression with the previously holded value as the left operand and `operand` as the right operand, store the result value into the atom and return the previously holded (stamped) value.

The binary operator is determined as follows:

| Member name | Operator |
| - | - |
| `fetchAddStamped`, `fetchAdd`, `fetch_add` | `+` |
| `fetchSubtractStamped`, `fetchSubtract`, `fetch_sub` | `-` |
| `fetchBitwiseAndStamped`, `fetchBitwiseAnd`, `fetch_and` | `&` |
| `fetchBitwiseOrStamped`, `fetchBitwiseOr`, `fetch_or` | `|` |
| `fetchBitwiseExclusiveOrStamped`, `fetchBitwiseExclusiveOr`, `fetch_xor` | `^` |

Note that if the standard supports the corresponding `std::atomic_fetch_*`/`std::atomic_ref::fetch_*` for `Value` and `OperandT`, the operation will be performed through these standard functions for protential better performance unless `stamped`; otherwise (for example, a user-defined overloaded operator) the operation would be implemented using fetch transform operations.

Evaluation of the operators are not part of the atomicity unless the `std::atomic_fetch_*`/`std::atomic_ref::fetch_*` were used.

If the `std::atomic_fetch_*`/`std::atomic_ref::fetch_*` were used, atomic versions are signal safe if `atomic.isDeterminateTrue() && lockFreeOnExecution() && (!stamped)`.

For each `fetch*`/`Fetch*` operation there is a corresponding `*Fetch` operation which operates identically except that the `*Fetch` operation returns the value after the modification.

### Arithmetic compound assignment operators

```cpp
template <typename OperandT> constexpr auto operator +=(OperandT const& operand)& noexcept -> Value;
template <typename OperandT> constexpr auto operator -=(OperandT const& operand)& noexcept -> Value;
template <typename OperandT> constexpr auto operator &=(OperandT const& operand)& noexcept -> Value;
template <typename OperandT> constexpr auto operator |=(OperandT const& operand)& noexcept -> Value;
template <typename OperandT> constexpr auto operator ^=(OperandT const& operand)& noexcept -> Value;
```

Equivalent to corresponding `this->*Fetch(operand)`.

```cpp
constexpr auto operator ++()& noexcept -> Value; // 1
constexpr auto operator ++(::dcool::core::PostDisambiguator)& noexcept -> Value; // 2
constexpr auto operator --()& noexcept -> Value; // 3
constexpr auto operator --(::dcool::core::PostDisambiguator)& noexcept -> Value; //4
```

- Version 1: equivalent to `this->fetchAdd(1)`.
- Version 2: equivalent to `this->addFetch(1)`.
- Version 3: equivalent to `this->fetchSubtract(1)`.
- Version 4: equivalent to `this->subtractFetch(1)`.

## Static member functions

## Non-static member functions

### `lockFreeOnExecution`, `is_lock_free`

```cpp
constexpr auto lockFreeOnExecution() const noexcept -> ::dcool::core::Boolean;
constexpr auto is_lock_free() const noexcept -> ::dcool::core::Boolean; // For standard compatibility
```

Returns `true` if `lockFree.isDeterminateTrue()`; returns `false` if `lockFree.isDeterminateFalse()`; otherwise implementation-defined.

Indicates whether all operations are lock-free.
