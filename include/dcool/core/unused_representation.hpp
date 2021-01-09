#ifndef DCOOL_CORE_UNUSED_REPRESENTATION_HPP_INCLUDED_
#	define DCOOL_CORE_UNUSED_REPRESENTATION_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>

namespace dcool::core {
	namespace detail_ {
		template <typename T_> concept UnusedRepresentationFromMember_ = requires {
			{ T_::unusedRepresentation() } -> ::dcool::core::SameAs<T_>;
		};

		template <typename T_> concept WritableToUnusedRepresetationByMember_ = requires (T_ object_) {
			object_.setToUnusedRepresentation();
		};

		template <typename ObjectT_> constexpr void setToUnusedRepresentationByAdl_(ObjectT_& object_) {
			// Customizable with ADL
			setToUnusedRepresentation(object_);
		}

		template <typename T_> concept WritableToUnusedRepresetationByAdl_ = requires (T_ object_) {
			::dcool::core::detail_::setToUnusedRepresentationByAdl_(object_);
		};

		template <typename T_> concept UnusedRepresetationVerifiableByMember_ = requires (T_ object_) {
			{ object_.verifyUnusedRepresentation() } -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
		};

		template <typename ObjectT_> constexpr auto verifiyUnusedRepresentationByAdl_(ObjectT_& object_) -> ::dcool::core::Boolean {
			// Customizable with ADL
			return verifyUnusedRepresentation(object_);
		}

		template <typename T_> concept UnusedRepresetationVerifiableByAdl_ = requires (T_ object_) {
			{
				::dcool::core::detail_::verifiyUnusedRepresentationByAdl_(object_)
			} -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
		};

		template <typename ObjectT_> constexpr void indirectlySetToUnusedRepresentation_(ObjectT_& object_) {
			object_ = ObjectT_::unusedRepresentation();
		}

		template <
			::dcool::core::detail_::WritableToUnusedRepresetationByAdl_ ObjectT_
		> constexpr void indirectlySetToUnusedRepresentation_(ObjectT_& object_) {
			::dcool::core::detail_::setToUnusedRepresentationByAdl_(object_);
		}

		template <typename ObjectT_> constexpr void setToUnusedRepresentation_(ObjectT_& object_) {
			::dcool::core::detail_::indirectlySetToUnusedRepresentation_(object_);
		}

		template <
			::dcool::core::detail_::WritableToUnusedRepresetationByMember_ ObjectT_
		> constexpr void setToUnusedRepresentation_(ObjectT_& object_) {
			object_.setToUnusedRepresentation();
		}

		template <typename ObjectT_> constexpr auto unusedRepresentationOf_() -> ObjectT_ {
			ObjectT_ object_;
			::dcool::core::detail_::setToUnusedRepresentation_(object_);
			return object_;
		}

		template <
			::dcool::core::detail_::UnusedRepresentationFromMember_ ObjectT_
		> constexpr auto unusedRepresentationOf_() -> ObjectT_ {
			return ObjectT_::unusedRepresentation();
		}

		template <
			typename ObjectT_
		> constexpr auto indirectlyVerifiyUnusedRepresentation_(ObjectT_& object_) -> ::dcool::core::Boolean {
			return object_ == ::dcool::core::detail_::unusedRepresentationOf_<ObjectT_>();
		}

		template <
			::dcool::core::detail_::UnusedRepresetationVerifiableByAdl_ ObjectT_
		> constexpr auto indirectlyVerifiyUnusedRepresentation_(ObjectT_& object_) -> ::dcool::core::Boolean {
			return ::dcool::core::detail_::verifiyUnusedRepresentationByAdl_(object_);
		}

		template <typename ObjectT_> constexpr auto verifyUnusedRepresentation_(ObjectT_& object_) -> ::dcool::core::Boolean {
			return ::dcool::core::detail_::indirectlyVerifiyUnusedRepresentation_(object_);
		}

		template <
			::dcool::core::detail_::WritableToUnusedRepresetationByMember_ ObjectT_
		> constexpr auto verifyUnusedRepresentation_(ObjectT_& object_) -> ::dcool::core::Boolean {
			return object_.verifyUnusedRepresentation();
		}
	}

	template <typename T_> concept UnusedRepresentable = requires (T_ object_) {
		{ ::dcool::core::detail_::unusedRepresentationOf_<T_>() } -> ::dcool::core::SameAs<T_>;
		::dcool::core::detail_::setToUnusedRepresentation_(object_);
		{ ::dcool::core::detail_::verifyUnusedRepresentation_(object_) } -> ::dcool::core::ConvertibleTo<::dcool::core::Boolean>;
	};

	template <::dcool::core::UnusedRepresentable ObjectT_> constexpr auto unusedRepresentationOf() -> ObjectT_ {
		return ::dcool::core::detail_::unusedRepresentationOf_<ObjectT_>();
	}

	template <::dcool::core::UnusedRepresentable ObjectT_> constexpr void intelliSetToUnusedRepresentation(ObjectT_ object_) {
		return ::dcool::core::detail_::setToUnusedRepresentation_(object_);
	}

	template <
		::dcool::core::UnusedRepresentable ObjectT_
	> constexpr auto intelliVerifyUnusedRepresentation(ObjectT_ object_) -> ::dcool::core::Boolean {
		return ::dcool::core::detail_::verifyUnusedRepresentation_(object_);
	}
};

#endif