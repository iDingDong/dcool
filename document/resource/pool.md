# Concept `dcool::resource::Pool`

```cpp
template <typename T, dcool::core::StorageRequirement requirement> concept dcool::resource::Pool;
```

Given the following definition (all required to be well-formed):

```cpp
using Adaptor = dcool::resource::PoolAdaptor<T, requirement>;
using Handle = Adaptor::Handle;
T pool;
Handle handle;
```

## Definition

the following expressions should be well-formed:

| Expression | Addtional Requirement |
| - | - |
| handle = `pool.allocate<requirement>()` | - |
| `pool.deallocate<requirement>(handle)` | - |

## Addtional required sementic
