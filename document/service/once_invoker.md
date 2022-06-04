# Class template `dcool::service::OnceInvoker`

Include `<dcool/service.hpp>` to use.

Helper for calling a callable for exactly once.

```cpp
template <typename ConfigT = /* unspecified type */> struct dcool::service::OnceInvoker;
```

`dcool::service::OnceInvoker` can be used as an alternative to `std::once_flag`. Instead of a global function like `std::call_once`, `dcool::service::OnceInvoker` provides its member function `invoke` to commence the call.

## Configuration `ConfigT`

Its member shall customize the list as decribed:

| Member | Default | Behavior |
| - | - | - |
| `static constexpr dcool::service::OnceInvokerSynchronizationStrategy synchronizationStrategy` | `dcool::service::OnceInvokerSynchronizationStrategy::standard` | The strategy for synchronization among the accepted call request and subsequent attempts. See *Synchronization strategy* for more details. |

## Member constants

| Name | Definition |
| - | - |
| `synchronizationStrategy` | Determined by configuration `synchronizationStrategy`. |

## Constructors

```cpp
constexpr OnceInvoker() noexcept(/* unspecified expression */);
OnceInvoker(OnceInvoker const&) = delete;
```

Intialize the invoker.

## Assignments

```cpp
auto operator =(OnceInvoker const&) -> OnceInvoker& = delete;
```

## Non-static member functions

### `invoke`

```cpp
template <typename FunctionT, typename... ArgumentTs> void invoke(
	FunctionT&& function, ArgumentTs&&... parameters
) noexcept(/* unspecified expression */)
```

If any call to this member with the same invoker has already returned, returns right away (such a call is known as passive).

Otherwise, equivalently evaluates `dcool::core::invoke(dcool::core::forward<FunctionT>(function), dcool::core::forward<ArgumentTs>(parameters)...)` (such a call is known as active). If that evaluation throws an exception, it is propagated to the caller of this member, otherwise returns.

All active calls on the same invoker form a single total order consisting of zero or more exceptional calls, followed by one returning call. The end of each active call synchronizes-with the next active call in that order unless `synchronizationStrategy` is `dcool::service::OnceInvokerSynchronizationStrategy::none`.

The return from the returning call synchronizes-with the returns from all passive calls on the same invoker unless `synchronizationStrategy` is `dcool::service::OnceInvokerSynchronizationStrategy::none`: this means that all concurrent calls to this member are guaranteed to observe any side-effects made by the active call, with no additional synchronization.

## Synchronization strategy

The configuration `synchronizationStrategy` may take one of the following values:

- `dcool::service::OnceInvokerSynchronizationStrategy::standard`: will be implemented using `std::once_flag` and `std::call_once`.
- `dcool::service::OnceInvokerSynchronizationStrategy::regular`: will be implemented using double checked locking.
- `dcool::service::OnceInvokerSynchronizationStrategy::busy`: will be implemented using spinning locks.
- `dcool::service::OnceInvokerSynchronizationStrategy::none`: inter-thread attempts will not be synchronized.

## Notes

If you are using libstdc++, be sure to check `document/dependency_bugs#Bug_4` that may cause the behavior of member `invoke` to be broken when an exception is thrown if the synchronization strategy is set to `dcool::service::OnceInvokerSynchronizationStrategy::standard`. This is the predominant reason for this wheel to be reinvented.
