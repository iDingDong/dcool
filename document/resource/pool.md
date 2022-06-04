# Named requirement *Pool*

This feature is unstable.

## Definition

For type `Pool` that satisfy *Pool* for `requirement` (which is the storage requirement for an object type):

Given the following definition (all shall be well-formed unless otherwise specified):

```cpp
using Adaptor = dcool::resource::PoolAdaptor<T, requirement>;
Pool pool;
Adaptor::Handle wildHandle;
Adaptor::ConstHandle wildConstHandle;
void* wildPointer;
void const* wildConstHandle;
Adaptor::Handle const handle = /* Value returned by allocation from this pool */;
Adaptor::ConstHandle const constHandle = handle;
void* const pointer = /* pointer allocated from this pool */;
void const* const constPointer = pointer;
Adaptor::HandleConverter handleConverter = Adaptor::handleConverter(pool);
Adaptor::ConstHandleConverter constHandleConverter = Adaptor::constHandleConverter(pool);
```

The following expressions should be conditionally well-formed:

| Expression | Well-formedness | Addtional Requirement | Semantic requirement |
| - | - | - | - |
| `wildHandle = pool.allocate<requirement>()` | Unconditional. | | After evaluation, `wildHandle` should refer to a buffer where object of `requirement` can be in-place constructed on. Handle to the same buffer shall not be returned by subsequential allocation until `pool.deallocate<requirement>(wildHandle)` is evaluated and visible. |
| `pool.deallocate<requirement>(handle)` | Unconditional. | | `noexcept(pool.deallocate<requirement>(handle))` is `true`. |
| `handleConverter = pool.handleConverter()` | Optional if `Adaptor::Handle` and `void*` are convertible to each other. | | After evaluation, `handleConverter` shall still meet other requirement specified in this document. |
| `constHandleConverter = pool.constHandleConverter();` | Optional if `Adaptor::ConstHandle` and `void const*` are convertible to each other | | After evaluation, `constHandleConverter` shall still meet other requirement specified in this document. |
| `wildPointer = handleConverter(handle)` | Unconditional. | | After evaluation, `wildPointer` shall point to the same buffer as `handle` refers to. |
| `wildHandle = handleConverter(pointer)` | Unconditional. | | After evaluation, `wildHandle` shall refer to the same buffer as `pointer` points to. |
| `wildConstPointer = constHandleConverter(constHandle)` | Unconditional. | | After evaluation, `wildConstPointer` shall point to the same buffer as `handle` refers to. |
| `wildConstHandle = constHandleConverter(constPointer)` | Unconditional. | | After evaluation, `wildConstHandle` shall refer to the same buffer as `constPointer` points to. |

## Conrresponding concept

```cpp
template <typename T, dcool::core::StorageRequirement requirementC> concept dcool::resource::Pool;
```

For given `Pool` that satisfies *Pool* for requirement, `dcool::resource::Pool<Pool, requirement>` shall evaluate to `true`.
