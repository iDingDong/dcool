# Named requirement *ArrayPool*

This feature is unstable.

## Definition

For type `Pool` that satisfy *ArrayPool* for `requirement` (which is the storage requirement for an object type):

`Pool` shall satisfy *Pool* for `requirement`.

Given the following definition (all shall be well-formed unless otherwise specified):

```cpp
using Adaptor = dcool::resource::ArrayPoolAdaptor<T, requirement>;
Pool pool;
Adaptor::ArrayHandle wildHandle;
Adaptor::ArrayConstHandle wildConstHandle;
void* wildPointer;
void const* wildConstHandle;
Adaptor::ArrayHandle const handle = /* Value returned by allocation from this pool */;
Adaptor::ArrayConstHandle const constHandle = handle;
void* const pointer = /* pointer allocated from this pool */;
void const* const constPointer = pointer;
Adaptor::Length length = /* unspecified value greater than 0 */;
Adaptor::ArrayHandleConverter handleConverter = Adaptor::arrayHandleConverter(pool);
Adaptor::ArrayConstHandleConverter constHandleConverter = Adaptor::arrayConstHandleConverter(pool);
```

The following expressions should be conditionally well-formed:

| Expression | Well-formedness | Addtional Requirement | Semantic requirement |
| - | - | - | - |
| `wildHandle = pool.allocate<requirement>(length)` | Unconditional. | | After evaluation, `wildHandle` should refer to a buffer where object of `requirement` can be in-place constructed on. Handle to the same buffer shall not be returned by subsequential allocation until `pool.deallocate<requirement>(wildHandle)` is evaluated and visible. |
| `pool.deallocate<requirement>(handle, length)` | Unconditional. | | `noexcept(pool.deallocate<requirement>(handle))` is `true`. |
| `handleConverter = pool.arrayHandleConverter()` | Optional if `Adaptor::ArrayHandle` and `void*` are convertible to each other. | | After evaluation, `handleConverter` shall still meet other requirement specified in this document. |
| `constHandleConverter = pool.arrayConstHandleConverter()` | Optional if `Adaptor::ArrayConstHandle` and `void const*` are convertible to each other | | After evaluation, `constHandleConverter` shall still meet other requirement specified in this document. |
| `wildPointer = handleConverter(handle)` | Unconditional. | | After evaluation, `wildPointer` shall point to the same buffer as `handle` refers to. |
| `wildHandle = handleConverter(pointer)` | Unconditional. | | After evaluation, `wildHandle` shall refer to the same buffer as `pointer` points to. |
| `wildConstPointer = constHandleConverter(constHandle)` | Unconditional. | | After evaluation, `wildConstPointer` shall point to the same buffer as `handle` refers to. |
| `wildConstHandle = constHandleConverter(constPointer)` | Unconditional. | | After evaluation, `wildConstHandle` shall refer to the same buffer as `constPointer` points to. |

## Conrresponding concept

```cpp
template <typename T, dcool::core::StorageRequirement requirementC> concept dcool::resource::ArrayPool;
```

For given `ArrayPool` that satisfies *ArrayPool* for `requirement`, `dcool::resource::ArrayPool<Pool, requirement>` shall evaluate to `true`.
