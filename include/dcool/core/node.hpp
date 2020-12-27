#ifndef DCOOL_CORE_NODE_HPP_INCLUDED_
#	define DCOOL_CORE_NODE_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/member_detector.hpp>
#	include <dcool/core/type_transformer.hpp>
#	include <dcool/core/type_value_detector.hpp>

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
}

#endif
