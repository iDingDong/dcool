#ifndef DCOOL_CORE_VARIANT_HPP_INCLUDED_
#	define DCOOL_CORE_VARIANT_HPP_INCLUDED_

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>
#	include <dcool/core/exception.hpp>
#	include <dcool/core/integer.hpp>
#	include <dcool/core/type_list.hpp>
#	include <dcool/core/utility.hpp>

#	include <array>

namespace dcool::core {
	namespace detail_ {
		template <typename ConfigT_, ::dcool::core::TypeList ValuesT_> class VariantConfigAdaptor_ {
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
				::dcool::core::exceptionSafetyStrategyOf<ConfigT_>
			;
		};
	}

	template <typename T_, typename ValuesT_> concept VariantConfig = requires {
		typename ::dcool::core::detail_::VariantConfigAdaptor_<T_, ValuesT_>;
	};

	template <typename ConfigT_, ::dcool::core::TypeList ValuesT_> requires (
		::dcool::core::VariantConfig<ConfigT_, ValuesT_>
	) using VariantConfigAdaptor = ::dcool::core::detail_::VariantConfigAdaptor_<ConfigT_, ValuesT_>;

	namespace detail_ {
		using VariantValueDestructor_ = void (*)(void* value_);
		using VariantValueCloner_ = void (*)(void const* source_, void* destination_);
		using VariantValueRelocator_ = void (*)(void* source_, void* destination_);

		template <typename ValueT_> constexpr void destructVariantValue_(void* value_) noexcept {
			if constexpr (::dcool::core::NonVoid<ValueT_>) {
				::dcool::core::destruct(*reinterpret_cast<ValueT_*>(value_));
			}
		}

		template <typename ValueT_> constexpr void cloneVariantValue_(void const* source_, void* destination_) noexcept {
			if constexpr (::dcool::core::NonVoid<ValueT_>) {
				new (destination_) ValueT_(*reinterpret_cast<ValueT_ const*>(source_));
			}
		}

		template <typename ValueT_> constexpr void relocateVariantValue_(void* source_, void* destination_) noexcept {
			if constexpr (::dcool::core::NonVoid<ValueT_>) {
				new (destination_) ValueT_(::dcool::core::move(*reinterpret_cast<ValueT_*>(source_)));
				::dcool::core::detail_::destructVariantValue_<ValueT_>(source_);
			}
		}

		template <
			::dcool::core::Boolean needDestructorC_,
			::dcool::core::Boolean needClonderC_,
			::dcool::core::Boolean needRelocatorC_
		> struct VariantValueInformationTable_ {
			::dcool::core::StaticOptional<::dcool::core::detail_::VariantValueDestructor_, needDestructorC_> destructor_;
			::dcool::core::StaticOptional<::dcool::core::detail_::VariantValueCloner_, needClonderC_> cloner_;
			::dcool::core::StaticOptional<::dcool::core::detail_::VariantValueRelocator_, needRelocatorC_> relocator_;

			template <typename ValueT__> constexpr VariantValueInformationTable_(::dcool::core::TypedTag<ValueT__>) noexcept:
				destructor_ { &::dcool::core::detail_::destructVariantValue_ },
				cloner_ { &::dcool::core::detail_::cloneVariantValue_ },
				relocator_ { &::dcool::core::detail_::relocateVariantValue_ }
			{
			}
		};

		template <
			typename ConfigT_, typename... ValueTs_
		> requires (::dcool::core::VariantConfig<ConfigT_, ::dcool::core::Types<ValueTs_...>>) struct Variant_ {
			private: using Self_ = Variant_<ConfigT_, ValueTs_...>;
			public: using Config = ConfigT_;

			public: using Values = ::dcool::core::Types<ValueTs_...>;
			private: using ConfigAdaptor_ = ::dcool::core::VariantConfigAdaptor<Config, Values>;
			private: static constexpr ::dcool::core::Length alternativeCount_ = sizeof...(ValueTs_);
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
				ConfigAdaptor_::exceptionSafetyStrategy
			;

			public: using Length = ::dcool::core::IntegerType<alternativeCount_>;
			public: using Index = Length;
			public: template <Index indexC_> using ValueAt = typename Values::At<indexC_>;
			private: using Storage_ = ::dcool::core::CommonStorageFor<ValueTs_...>;
			public: static constexpr Length alternativeCount = alternativeCount_;
			public: template <typename ValueT__> static constexpr Index valueIndex = Values::template findIndex<ValueT__>;
			public: static constexpr ::dcool::core::Boolean triviallyDestructible = (
				::dcool::core::TriviallyDestructible<ValueTs_> && ...
			);
			public: static constexpr ::dcool::core::Boolean copyable = (::dcool::core::CopyConstructible<ValueTs_> && ...);
			public: static constexpr ::dcool::core::Boolean moveable = (::dcool::core::MoveConstructible<ValueTs_> && ...);

			private: using InformationTable_ = ::dcool::core::detail_::VariantValueInformationTable_<
				!triviallyDestructible, copyable, moveable
			>;

			private: static constexpr ::std::array<InformationTable_, alternativeCount + 1> informations_ = {
				InformationTable_(::dcool::core::typed<ValueTs_>)..., InformationTable_(::dcool::core::typed<void>)
			};

			private: Storage_ m_storage_;
			private: Index m_index_ = alternativeCount;

			public: constexpr Variant_() noexcept = default;

			public: constexpr Variant_(Self_ const& other_) requires (copyable): m_index_(other_.index()) {
				informations_[other_.index()].cloner_.value(other_.data_(), this->data_());
			}

			public: constexpr Variant_(Self_&& other_) requires (moveable): m_index_(other_.index()) {
				informations_[other_.index()].relocator_.value(other_.data_(), this->data_());
				other_.m_index_ = alternativeCount;
			}

			public: template <::dcool::core::FormOfDifferent<Self_> ValueT__> constexpr Variant_(
				ValueT__&& value_
			): Variant_(::dcool::core::typedInPlace<::dcool::core::DecayedType<ValueT__>>, ::dcool::core::forward<ValueT__>(value_)) {
			}

			public: template <::dcool::core::OneOf<ValueTs_...> ValueT__, typename... ArgumentTs__> constexpr Variant_(
				::dcool::core::TypedInPlaceTag<ValueT__>, ArgumentTs__&&... parameters_
			): m_index_(valueIndex<ValueT__>) {
				new (this->data_()) ValueT__(::dcool::core::forward<ArgumentTs__>(parameters_)...);
			}

			public: constexpr ~Variant_() noexcept requires (!triviallyDestructible) {
				this->destruct_();
			}

			public: constexpr ~Variant_() noexcept requires (triviallyDestructible) {
			}

			private: constexpr void destruct_() noexcept {
				if constexpr (!triviallyDestructible) {
					informations_[this->index()].destructor_.value(this->data_());
				}
			}

			public: constexpr auto operator =(Self_ const& other_) -> Self_& requires (copyable) {
				Self_ middleMan_(other_);
				this->swapWith(middleMan_);
				return *this;
			}

			public: constexpr auto operator =(Self_&& other_) -> Self_& requires (copyable) {
				this->swapWith(other_);
				return *this;
			}

			public: template <::dcool::core::FormOfDifferent<Self_> ValueT__> constexpr auto operator =(
				ValueT__&& value_
			) -> Self_& requires (copyable) {
				this->emplace<::dcool::core::DecayedType<ValueT__>>(::dcool::core::forward<ValueT__>(value_));
				return *this;
			}

			private: constexpr void cloneTo_(Self_& other_) const requires (copyable) {
				informations_[other_.index()].cloner_.value(this->data_(), other_.data_());
				other_.m_index_ = this->index();
			}

			private: constexpr void relocateTo_(Self_& other_) requires (moveable) {
				informations_[other_.index()].relocator_.value(this->data_(), other_.data_());
				other_.m_index_ = this->index();
			}

			public: template <
				::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategyC__ = exceptionSafetyStrategy
			> constexpr void swapWith(Self_& other_)& {
				Self_ middleMan_;
				this->relocateTo_(middleMan_);
				try {
					other_.relocateTo_(*this);
					try {
						middleMan_.relocateTo_(other_);
					} catch (...) {
						try {
							this->relocateTo_(other_);
						} catch(...) {
							::dcool::core::goWeak<exceptionSafetyStrategyC__>();
							other_->m_index_ = alternativeCount;
							throw;
						}
						try {
							middleMan_.relcateTo_(*this);
						} catch (...) {
							::dcool::core::goWeak<exceptionSafetyStrategyC__>();
							this->m_index_ = alternativeCount;
							throw;
						}
						middleMan_.m_index_ = alternativeCount;
						throw;
					}
				} catch (...) {
					try {
						middleMan_.relcateTo_(*this);
					} catch(...) {
						::dcool::core::goWeak<exceptionSafetyStrategyC__>();
						throw;
					}
					throw;
				}
				middleMan_.m_index_ = alternativeCount;
			}

			private: constexpr auto data_() const noexcept -> void const* {
				return ::dcool::core::addressOf(this->m_storage_);
			}

			private: constexpr auto data_() noexcept -> void* {
				return ::dcool::core::addressOf(this->m_storage_);
			}

			public: constexpr auto index() const noexcept -> Index {
				return this->m_index_;
			}

			public: constexpr auto valid() const noexcept -> ::dcool::core::Boolean {
				return this->index() < alternativeCount;
			}

			public: template <::dcool::core::OneOf<ValueTs_...> ValueT__> constexpr auto holding() const noexcept {
				return this->index() == valueIndex<ValueT__>;
			}

			public: template <Index indexC__> requires (indexC__ < alternativeCount) constexpr decltype(auto) access(
			) const noexcept {
				DCOOL_CORE_ASSERT(indexC__ == this->index());
				return *reinterpret_cast<ValueAt<indexC__> const*>(this->data_());
			}

			public: template <Index indexC__> requires (indexC__ < alternativeCount) constexpr decltype(auto) access(
			) noexcept {
				DCOOL_CORE_ASSERT(indexC__ == this->index());
				return *reinterpret_cast<ValueAt<indexC__>*>(this->data_());
			}

			public: template <::dcool::core::OneOf<ValueTs_...> ValueT__> constexpr decltype(auto) access() const noexcept {
				return this->access<valueIndex<ValueT__>>();
			}

			public: template <::dcool::core::OneOf<ValueTs_...> ValueT__> constexpr decltype(auto) access() noexcept {
				return this->access<valueIndex<ValueT__>>();
			}

			public: template <::dcool::core::OneOf<ValueTs_...> ValueT__, typename... ArgumentTs__> constexpr void emplace(
				ArgumentTs__&&... parameters_
			) {
				this->reset();
				new (this->data_()) ValueT__(::dcool::core::forward<ArgumentTs__>(parameters_)...);
				this->m_index_ = valueIndex<ValueT__>;
			}

			public: constexpr void reset()& noexcept {
				this->destruct_();
				this->m_index_ = alternativeCount;
			}
		};

		template <typename ConfigT_> struct Variant_<ConfigT_> {
			private: using Self_ = Variant_<ConfigT_>;
			public: using Config = ConfigT_;

			public: using Values = ::dcool::core::Types<>;
			public: using Length = ::dcool::core::IntegerType<0>;
			public: using Index = Length;
			public: static constexpr Length alternativeCount = 0;
			public: template <typename ValueT__> static constexpr Index valueIndex = 0;
			public: static constexpr ::dcool::core::Boolean triviallyDestructible = true;
			public: static constexpr ::dcool::core::Boolean copyable = true;
			public: static constexpr ::dcool::core::Boolean moveable = true;

			public: constexpr auto index() const noexcept -> Index {
				return 0;
			}
		};

		template <::dcool::core::TypeList ValuesT_, typename ConfigT_> struct VariantHelper_;

		template <typename ConfigT_, typename... ValueTs_> struct VariantHelper_<::dcool::core::Types<ValueTs_...>, ConfigT_> {
			using Result_ = ::dcool::core::detail_::Variant_<ConfigT_, ValueTs_...>;
		};
	}

	template <
		::dcool::core::TypeList ValuesT_, typename ConfigT_ = ::dcool::core::Empty<>
	> using Variant = ::dcool::core::detail_::VariantHelper_<ValuesT_, ConfigT_>::Result_;

	template <typename... ValueTs_> using DefaultVariant = ::dcool::core::Variant<::dcool::core::Types<ValueTs_...>>;
}

#endif
