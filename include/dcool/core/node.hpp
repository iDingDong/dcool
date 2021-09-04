#ifndef DCOOL_CORE_NODE_HPP_INCLUDED_
#	define DCOOL_CORE_NODE_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/member_detector.hpp>
#	include <dcool/core/raw_pointer_operation.hpp>
#	include <dcool/core/type_transformer.hpp>
#	include <dcool/core/type_value_detector.hpp>
#	include <dcool/core/utility.hpp>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::core, HasTypeHeader, ExtractedHeaderType, Header)

namespace dcool::core {
	template <typename T_> concept ValueNode = requires (T_ node_, T_ const constNode_) {
		{ constNode_.value() } -> ::dcool::core::ConstReference;
		{ node_.value() } -> ::dcool::core::Reference;
		{ constNode_.header() } -> ::dcool::core::ConstReference;
		{ node_.header() } -> ::dcool::core::Reference;
		{ T_::retrieveFromHeader(constNode_.header()) } -> ::dcool::core::ConvertibleTo<T_ const&>;
		{ T_::retrieveFromHeader(node_.header()) } -> ::dcool::core::ConvertibleTo<T_&>;
	};

	template <::dcool::core::ValueNode NodeT_> using NodeHeaderType = ::dcool::core::IdenticallyQualifiedType<
		::dcool::core::ExtractedHeaderType<
			NodeT_, ::dcool::core::ReferenceRemovedType<decltype(::dcool::core::declval<NodeT_&>().header())>
		>,
		NodeT_
	>;

	template <::dcool::core::ValueNode NodeT_> using NodeValueType = ::dcool::core::ExtractedValueType<
		NodeT_, ::dcool::core::ReferenceRemovedType<decltype(::dcool::core::declval<NodeT_&>().value())>
	>;

	template <
		typename ValueT_, typename HeaderT_ = ::dcool::core::Empty<>
	> struct DefaultValueNode: private HeaderT_ {
		private: using Self_ = DefaultValueNode<ValueT_, HeaderT_>;
		private: using Super_ = HeaderT_;
		public: using Value = ValueT_;
		public: using Header = HeaderT_;

		private: [[no_unique_address]] Header m_header_;
		private: Value m_value_;

		public: template <typename... ArgumentTs__> constexpr DefaultValueNode(
			::dcool::core::InPlaceTag, ArgumentTs__&&... parameters_
		) noexcept(noexcept(Value(::dcool::core::forward<ArgumentTs__>(parameters_)...))): m_value_(
			::dcool::core::forward<ArgumentTs__>(parameters_)...
		) {
		}

		public: constexpr auto value() const noexcept -> Value const& {
			return this->m_value_;
		}

		public: constexpr auto value() noexcept -> Value& {
			return this->m_value_;
		}

		public: constexpr auto header() const noexcept -> Header const& {
			return *this;
		}

		public: constexpr auto header() noexcept -> Header& {
			return *this;
		}

		public: static constexpr auto retrieveFromHeader(Header const& header_) noexcept -> Self_ const& {
			return static_cast<Self_ const&>(header_);
		}

		public: static constexpr auto retrieveFromHeader(Header& header_) noexcept -> Self_& {
			return static_cast<Self_&>(header_);
		}
	};
}

#endif
