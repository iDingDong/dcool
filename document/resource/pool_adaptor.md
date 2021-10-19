# class template `dcool::resource::PoolAdaptor`

```cpp
template <typename Pool, dcool::core::StorageRequirement requirement> class dcool::resource::PoolAdaptor;
```

## Member types

| Name | Definition |
| - | - |
| `UnifiedConstHandle` | `Pool::UnifiedConstHandle` if available as a type; otherwise `void` |
| `UnifiedHandle` | `Pool::UnifiedHandle` if available as a type; otherwise `void` |
| `ConstHandle` | `Pool::ConstHandle<requirement>` if available as a type; otherwise `UnifiedConstHandle` if not `void`; otherwise unspecified |
| `Handle` | `Pool::Handle<requirement>` if available as a type; otherwise `UnifiedHandle` if not `void`; otherwise return type of `pool.allocate<requirement>()` |
| `ConstHandleConverter` | `Pool::ConstHandleConverter<requirement>` if available as a type; otherwise `dcool::core::DefaultBidirectionalImplicitConverter<Handle, void*>` |
| `HandleConverter` | `Pool::HandleConverter<requirement>` if available as a type; otherwise `dcool::core::DefaultBidirectionalImplicitConverter<Handle, void*>` |

## Static member functions

```cpp
template <>
```
