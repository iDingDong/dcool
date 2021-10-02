# Class template `dcool::concurrency::Mutex`

Include `<dcool/concurrency.hpp>` to use.

```cpp
template <typename ConfigT = /* unspecified type */> struct dcool::concurrency::Mutex;
```

A cu

## Configuration `ConfigT`

Its member shall customize the list as decribed:

| Member | Default | Behavior |
| - | - | - |
| `static constexpr dcool::core::Boolean spinnable` | `false` | The mutex will not block on any actions if it takes value `true`; otherwise not. |
| `static constexpr dcool::core::Boolean timed` | `false` | Certain mutex actions requires this to take value `true`. |
| `static constexpr dcool::core::UnsignedMaxInteger maxShare` | `0` | Indicates the maximum shared owners allowed at the same time. |
| `static constexpr dcool::core::Boolean downgradeable` | `false` | Certain mutex actions requires this to take value `true`. |
| `static constexpr dcool::core::Boolean upgradeable` | `false` | Certain mutex actions requires this to take value `true`. |
| `static constexpr dcool::core::Triboolean preferExclusive` | `maxShare > 0 && dcool::core::indeterminate` | No shared ownership shall be acquireable when a thread is requesting a exclusive ownership if it takes value `dcool::core::determinateTrue`; otherwise shared ownership shall be acquireable whenever exclusive ownership is not possessed by any thread if it takes value `dcool::core::determinateFalse`; otherwise it is unspecified which of the two previous behavior is to be applied. |

## Member constants

| Name | Definition |
| - | - |
| `static constexpr dcool::core::Boolean spinnable` | Determined by configuration. |
| `static constexpr dcool::core::Boolean timed` | Determined by configuration. |
| `static constexpr dcool::core::UnsignedMaxInteger maxShare` | Determined by configuration. |
| `static constexpr dcool::core::UnsignedMaxInteger shareable` | Defined by `static constexpr dcool::core::UnsignedMaxInteger shareable = (maxShare > 0)` |
| `static constexpr dcool::core::Boolean downgradeable` | `true` if `shareable` and configured to be `true`; |
| `static constexpr dcool::core::Boolean upgradeable` | `true` if `shareable` and configured to be `true`; |
| `static constexpr dcool::core::Triboolean preferExclusive` | `false` if not `shareable`; otherwise determined by configuration. |

## Constructors

```cpp
Mutex() noexcept(/* unspecified value */);
Mutex(Mutex const&) = delete;
Mutex(Mutex&&) = delete;
```

## Assignments

```cpp
auto operator =(Mutex const&) -> Mutex& = delete;
auto operator =(Mutex&&) -> Mutex& = delete;
```

## Non-static member functions

### `tryLock`

```cpp
auto tryLock() noexcept -> dcool::core::Boolean;
auto try_lock() noexcept -> dcool::core::Boolean; // For standard compatibility.
```

Attempt to acquire exclusive ownership. Returns true if success, otherwise returns false.

This function will return as soon as possible once other ownership is confirmed.

Behavior is undefined if any type of ownership is already possessed by the current thread.

Behavior is undefined if any type of active order is already placed the current thread.

### `lock`

```cpp
void lock() noexcept(spinnable);
```

Acquire exclusive ownership. Wait indefinitely until success.

Behavior is undefined if any type of ownership is already possessed by the current thread.

Behavior is undefined if any type of active order is already placed the current thread.

### `unlock`

```cpp
void unlock() noexcept;
```

Release exclusive ownership.

Behavior is undefined if exclusive ownership is not possessed by the current thread.

### `tryLockShared`

```cpp
auto tryLockShared() noexcept -> dcool::core::Boolean requires (shareable);
auto try_lock_shared() noexcept -> dcool::core::Boolean requires (shareable); // For standard compatibility.
```

Attempt to acquire shared ownership. Returns true if success, otherwise returns false.

This function will return as soon as possible once other exclusive ownership (possibly pending unless `preferExclusive.isDeterminateFalse()`) or excess shared ownership is confirmed.

Behavior is undefined if any type of ownership is already possessed by the current thread.

Behavior is undefined if any type of active order is already placed the current thread.

### `lockShared`

```cpp
void lockShared() noexcept(spinnable) requires (shareable);
void lock_shared() noexcept(spinnable) requires (shareable); // For standard compatibility.
```

Acquire shared ownership. Wait indefinitely until success.

Behavior is undefined if any type of ownership is already possessed by the current thread.

Behavior is undefined if any type of active order is already placed the current thread.

### `unlockShared`

```cpp
void unlockShared() noexcept requires (shareable);
void unlock_shared() noexcept requires (shareable); // For standard compatibility.
```

Release shared ownership.

Behavior is undefined if shared ownership is not possessed by the current thread.

### `tryUpgrade`

```cpp
auto tryUpgrade() noexcept -> dcool::core::Boolean requires (upgradeable);
```

Attempt to convert shared ownership to exclusive ownership. Returns true if success, otherwise returns false.

This function will return as soon as possible once other ownership (possibly pending unless `preferExclusive.isDeterminateFalse()`) is confirmed.

Behavior is undefined if shared ownership is not possessed by the current thread.

### `upgrade`

```cpp
void upgrade() noexcept(spinnable) requires (upgradeable);
```

Convert shared ownership to exclusive ownership. Wait indefinitely until success.

Behavior is undefined if shared ownership is not possessed by the current thread.

Behavior is undefined if another thread is waiting indefinitely to upgrade.

Behavior is undefined if another thread is waiting indefinitely for exclusive ownership unless `preferExclusive.isDeterminateFalse()`.

### `downgrade`

```cpp
void downgrade() noexcept requires (downgradeable);
```

Convert exclusive ownership to shared ownership.

Behavior is undefined if exclusive ownership is not possessed by the current thread.

### `tryOrderLock`

```cpp
auto tryOrderLock() noexcept -> dcool::core::Boolean requires (preferExclusive.isDeterminateTrue());
```

Attempt to place a lock order that prevents all subsequent ownership request when active. Returns true if success, otherwise returns false.

This function will return as soon as possible once other exclusive ownership (possibly pending) is confirmed.

Behavior is undefined if any type of ownership is already possessed by the current thread.

Behavior is undefined if any type of active order is already placed the current thread.

### `tryCompleteLockOrder`

```cpp
auto tryCompleteLockOrder() noexcept -> dcool::core::Boolean requires (preferExclusive.isDeterminateTrue());
```

Attempt to convert a lock order to an exclusive ownership. Returns true if success, otherwise returns false.

This function will return as soon as possible once other ownership is confirmed.

Behavior is undefined if no active lock order is placed by current thread.

### `orderLock`

```cpp
void orderLock() noexcept(spinnable) requires (preferExclusive.isDeterminateTrue());
```

Place a lock order that prevents all subsequent ownership request when active. Wait indefinately until success.

Behavior is undefined if any type of ownership is already possessed by the current thread.

Behavior is undefined if any type of active order is already placed the current thread.

### `completeLockOrder`

```cpp
void completeLockOrder() noexcept(spinnable) requires (preferExclusive.isDeterminateTrue());
```

Convert a lock order to an exclusive ownership. Wait indefinately until success.

Behavior is undefined if no active lock order is placed by current thread.

### `cancelLockOrder`

```cpp
void cancelLockOrder() noexcept requires (preferExclusive.isDeterminateTrue());
```

Cancel a lock order.

Behavior is undefined if no active lock order is placed by current thread.

### `tryOrderUpgrade`

```cpp
auto tryOrderUpgrade() noexcept -> dcool::core::Boolean requires (upgradeable && preferExclusive.isDeterminateTrue());
```

Attempt to place an upgrade order that prevents all subsequent ownership request when active. Returns true if success, otherwise returns false.

This function will return as soon as possible once other exclusive ownership (possibly pending) is confirmed.

Behavior is undefined if no shared ownership is possessed by current thread.

### `tryCompleteUpgradeOrder`

```cpp
auto tryCompleteUpgradeOrder() noexcept -> dcool::core::Boolean requires (upgradeable && preferExclusive.isDeterminateTrue());
```

Attempt to convert an upgrade order to an exclusive ownership. Returns true if success, otherwise returns false.

This function will return as soon as possible once other ownership is confirmed.

Behavior is undefined if no active upgrade order is placed by current thread.

### `orderUpgrade`

```cpp
void orderUpgrade() noexcept(spinnable) requires (upgradeable && preferExclusive.isDeterminateTrue());
```

Place an upgrade order that prevents all subsequent ownership request when active. Wait indefinately until success.

Behavior is undefined if no shared ownership is possessed by current thread.

### `completeUpgradeOrder`

```cpp
void completeUpgradeOrder() noexcept(spinnable) requires (upgradeable && preferExclusive.isDeterminateTrue());
```

Convert an upgrade order to an exclusive ownership. Wait indefinately until success.

Behavior is undefined if no active upgrade order is placed by current thread.

### `cancelUpgradeOrder`

```cpp
void cancelUpgradeOrder() noexcept requires (upgradeable && preferExclusive.isDeterminateTrue());
```

Cancel an upgrade order.

Behavior is undefined if no active lock order is placed by current thread.

### `tryLockUntil`

```cpp
template <typename TimePointT> auto tryLockUntil(TimePointT const deadline) noexcept -> dcool::core::Boolean requires (timed);
template <typename TimePointT> auto try_lock_until(
	TimePointT const deadline
) noexcept -> dcool::core::Boolean requires (timed); // For standard compatibility.
```

`TimePointT` should be instanitiated from template `std::chrono::time_point`.

Attempt to acquire exclusive ownership. Returns true if success, otherwise returns false.

This function will return as soon as possible once `deadline` is reached.

Behavior is undefined if any type of ownership is already possessed by the current thread.

Behavior is undefined if any type of active order is already placed the current thread.

### `tryLockFor`

```cpp
template <typename DurationT> auto tryLockFor(DurationT const duration) noexcept -> dcool::core::Boolean requires (timed);
template <typename DurationT> auto try_lock_for(
	DurationT const duration
) noexcept -> dcool::core::Boolean requires (timed); // For standard compatibility.
```

Equivalent to `this->tryLockUntil(std::chrono::steady_clock::now() + duration)`.

### `tryLockSharedUntil`

```cpp
template <typename TimePointT> auto tryLockSharedUntil(
	TimePointT const deadline
) noexcept -> dcool::core::Boolean requires (timed && shareable);
template <typename TimePointT> auto try_lock_shared_until(
	TimePointT const deadline
) noexcept -> dcool::core::Boolean requires (timed && shareable); // For standard compatibility.
```

`TimePointT` should be instanitiated from template `std::chrono::time_point`.

Attempt to acquire shared ownership. Returns true if success, otherwise returns false.

This function will return as soon as possible once `deadline` is reached.

Behavior is undefined if any type of ownership is already possessed by the current thread.

Behavior is undefined if any type of active order is already placed the current thread.

### `tryLockSharedFor`

```cpp
template <typename DurationT> auto tryLockSharedFor(
	DurationT const duration
) noexcept -> dcool::core::Boolean requires (timed && shareable);
template <typename DurationT> auto try_lock_shared_for(
	DurationT const duration
) noexcept -> dcool::core::Boolean requires (timed && shareable); // For standard compatibility.
```

Equivalent to `this->tryLockSharedUntil(std::chrono::steady_clock::now() + duration)`.

## Note

The behavior is undefined if a thread ends its execution with any owned ownership or active order.

The behavior is undefined if a mutex ends its execution with any owned ownership or active order of any thread.
