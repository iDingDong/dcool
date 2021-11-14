# Class template `dcool::resource::PoolAdaptor`

```cpp
template <typename PoolT, dcool::core::StorageRequirement requirementC> class dcool::resource::PoolAdaptor;
```

This feature is unstable.

## Member types

| Name | Definition |
| - | - |
| `Pool` | Defined by `using Pool = PoolT;`. |
| `UnifiedConstHandle` | `Pool::UnifiedConstHandle` if available as a type; otherwise `void` |
| `UnifiedHandle` | `Pool::UnifiedHandle` if available as a type; otherwise `void` |
| `ConstHandle` | `Pool::ConstHandle<requirementC>` if available as a type; otherwise `UnifiedConstHandle` if not `void`; otherwise unspecified |
| `Handle` | `Pool::Handle<requirementC>` if available as a type; otherwise `UnifiedHandle` if not `void`; otherwise return type of `pool.allocate<requirementC>()` |
| `ConstHandleConverter` | `Pool::ConstHandleConverter<requirementC>` if available as a type; otherwise `dcool::core::DefaultBidirectionalImplicitConverter<Handle, void*>` |
| `HandleConverter` | `Pool::HandleConverter<requirementC>` if available as a type; otherwise `dcool::core::DefaultBidirectionalImplicitConverter<Handle, void*>` |

## Member constants

| Name | Definition |
| - | - |
| `requirement` | Defined by `static constexpr dcool::core::StorageRequirement requirement = requirementC`. |

## Static member functions

### `allocate`

```cpp
[[nodiscard]] static constexpr auto allocate(Pool& pool) -> Handle;
```

Returns `pool.allocate<requirement>()`.

### `deallocate`

```cpp
static constexpr void deallocate(Pool& pool, Handle handle) noexcept;
```

Calls `pool.deallocate<requirement>(handle)`.

### `handleConverter`

```cpp
static constexpr auto handleConverter(Pool& pool) noexcept -> `HandleConverter`;
```

Returns `pool.handleConverter()` if well-formed. Otherwise returns `dcool::core::DefaultBidirectionalImplicitConverter<Handle, void*>()`.
