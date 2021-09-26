# Integer

Include <dcool/core.hpp> to use.

```cpp
template <
	dcool::core::Boolean hasSignessC, dcool::core::Length widthC, typename ConfigT = /* unspecified */
> requires (widthC > 0 || (!hasSignessC)) struct dcool::core::Integer;
```

A customized width integer-like extended integer type that can perform limited integral arithmetic operations with.

## Terms

### Oversized integer

Indicate a `dcool::core::Integer` that cannot be represented by neither `std::uintmax_t` nor `std::intmax_t`. Operations on such type might be considered inefficient.

### General integral type

A type that is either a fundamental integer type or extended integer type.

## Configuration `ConfigT`

Its member shall customize the list as decribed:

| Member | Default | Behavior |
| - | - | - |
| `static constexpr dcool::core::Boolean negatable` | `true` | The extended integer supports `operator -` if it takes value `true`; otherwise not. |
| `static constexpr dcool::core::Length foldedWidth` | 0 | The extended integer will have a `foldedWidth`-bit non-static bit-field member `folded` which is signed if `hasSigness`; Otherwise the member `folded` would be an object of unspecified type. `foldedWidth` should be greater than width of neither `std::uintmax_t` nor `std::intmax_t`. |
| `static constexpr dcool::core::SusbicionStrategy outOfRangeStrategy` | `hasSigness ?dcool::core::SusbicionStrategy::undefined : dcool::core::SusbicionStrategy::ignore` | Determine the behavior when out of range value is produced by certain arithmetic operation which would be an undefined overflow on fundamental signed integers. Value `dcool::core::SusbicionStrategy::ignore` shall not be taken if `hasSigness`. |

## Member types

| Name | Definition |
| - | - |
| `Config` | Defined by `using Config = ConfigT`. |

## Member constants

| Name | Definition |
| - | - |
| `hasSigness` | Defined by `static constexpr dcool::core::Boolean hasSigness = hasSignessC`. |
| `width` | Defined by `static constexpr dcool::core::Length width = widthC`. |
| `outOfRangeStrategy` | Determined by configuration. |

## Mimicked behaviors

All binary operators are implemented to be able to accept a fundamental integer that would be converted to an extended integer type of same signess existence and width with the same configuration as of the other operand.

All binary operators only accepts extended integer type with same configuration.

For all binary operators other than compound assignments and shift operators, the two operands of type `Left` and `Right` respectively would be implicitly converted to `dcool::core::ExtendedCommonIntegerType<Left, Right>` which can represent either operands.

For shift operators with left operand of type `Left`, the right operand would be implicitly converted to `dcool::core::ExtendedShiftStepType<Left>`.

The following behavior of integer is identical to fundamental integral types:

### Arithmetic operations

- Unary operator `+`, `-` (requires `negatable`), `~`;
- Binary operator `+`, `-`, `*` (Not yet supported on oversized integers), `/` (Not yet supported on oversized integers), `&`, `|`, `^`, `<<`, `>>` and their compound assignment versions.

Overflow or wrapping produced by these operators shall trigger corresponding behavior determined by `outOfRangeStrategy`.

### Comparison

- Binary operator `<=>`, `==`, `!=`, `<`, `>`, `<=`, `>=`.

## Conversions

An extended integer type can be implicitly converted to a general integer type of same configuration (if exist) if the source type can be represented by the target type. The target value shall be the same as the source value.

A fundamental integer type can be implicitly converted to an extended integer type if the source type can be represented by the target type. The target value shall be the same as the source value.

All other conversions between general integral types shall be explicit. If the value cannot be represented by the target type, behavior is identical to fundamental integer conversions.

## Helper types

```cpp
template <dcool::core::Length widthC, typename ConfigT_ = /* unspecified */> using dcool::core::UnsignedInteger = dcool::core::Integer<false, widthC, ConfigT>;

template <dcool::core::Length widthC, typename ConfigT_ = /* unspecified */> using dcool::core::SignedInteger = dcool::core::Integer<true, widthC, ConfigT>;
```

## Literals

### Designated width literal

`using namespace dcool::core::extendedIntegerLiteral;` to use.

Integral literal suffix `_UI*` (where `*` is a decimal integer of value *m*) represents a literal for `dcool::core::UnsignedInteger<m>`.

Integral literal suffix `_SI*` (where `*` is a decimal integer of value *m*) represents a literal for `dcool::core::SignedInteger<m>`.

For example, literal `123_SI7` has type `dcool::core::SignedInteger<7>`.

Designated width literals can only be used with *m* no greater than the width of `unsigned long long`.

This library only defines *m* from 0 (1 for signed) to `DCOOL_CORE_PROVIDED_EXTENDED_INTEGER_LITERAL_MAX_WIDTH` (which is defined to be no less than 64, the least width of `unsigned long long` required by ISO/IEC 14882:2020). If your implementation provides `unsigned long long` of greater width than `DCOOL_CORE_PROVIDED_EXTENDED_INTEGER_LITERAL_MAX_WIDTH`, you may define the corresponding literal by writing `DCOOL_CORE_DEFINE_EXTENDED_INTEGER_LITERAL(namespaceName, m)` (where `namespace` name is the possibly nested user-defined namespace where the lieral is to be defined) in the global namespace before use.

### Deduced width literal

The following literals can only be used with binary, octal or hexadecimal integral literals.

Integral literal suffix `_UI` represents a literal for `dcool::core::UnsignedInteger<n>`.

Integral literal suffix `_SI*` (where `*` is a decimal integer of value *n*) represents a literal for `dcool::core::SignedInteger<n>`.

The number `n` is determined by the number of digis (including the leading zeroes) written. For example, literal `0b11_UI` has type `dcool::core::UnsignedInteger<2>` and `0x0000'01234567'89ABCDEF_SI` has type `dcool::core::SignedInteger<80>`.
