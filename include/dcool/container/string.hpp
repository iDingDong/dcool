#ifndef DCOOL_CONTAINER_STRING_HPP_INCLUDED_
#	define DCOOL_CONTAINER_STRING_HPP_INCLUDED_ 1

#	include <dcool/container/list.hpp>

#	include <dcool/core.hpp>

#	include <string>
#	include <iostream>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::container::detail_, HasTypeCharacter_, ExtractedCharacterType_, Character)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::container::detail_,
	HasTypeBaseStandardCharacterTrait_,
	ExtractedBaseStandardCharacterTraitType_,
	BaseStandardCharacterTrait
)
DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::container::detail_, HasTypeComparisonCategory_, ExtractedComparisonCategoryType_, ComparisonCategory
)
DCOOL_CORE_DEFINE_STATIC_MEMBER_CALLER(
	dcool::container::detail_, HasCallableCStringLengthOf_, cStringLengthOfOr_, cStringLengthOf
)
DCOOL_CORE_DEFINE_STATIC_MEMBER_CALLER(
	dcool::container::detail_, HasCallableCharacterCompare_, characterCompareOr_, characterCompare
)
DCOOL_CORE_DEFINE_STATIC_MEMBER_CALLER(
	dcool::container::detail_, HasCallableCharacterSequenceCompare_, characterSequenceCompareOr_, characterSequenceCompare
)
DCOOL_CORE_DEFINE_STATIC_MEMBER_CALLER(
	dcool::container::detail_, HasCallableCharacterWidthInBytes_, characterWidthInBytesOr_, characterWidthInBytes
)

namespace dcool::container {
	namespace detail_ {
		template <typename ConfigT_> class StringConfigAdaptor_ {
			private: using Self_ = StringConfigAdaptor_<ConfigT_>;
			public: using Config = ConfigT_;

			public: using Character = ::dcool::container::detail_::ExtractedCharacterType_<Config, char8_t>;
			public: using Value = Character;
			public: using Pool = ::dcool::resource::PoolType<Config>;
			public: using BaseStandardCharacterTrait = ::dcool::container::detail_::ExtractedBaseStandardCharacterTraitType_<
				Config, ::std::char_traits<Character>
			>;
			public: using ComparisonCategory = ::dcool::container::detail_::ExtractedComparisonCategoryType_<
				Config, typename BaseStandardCharacterTrait::comparison_category
			>;
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
				::dcool::core::exceptionSafetyStrategyOf<Config>
			;

			private: using DummyListConfigAdaptor_ = ::dcool::container::ListConfigAdaptor<Config, Value>;
			public: using Length = DummyListConfigAdaptor_::Length;
			public: using Engine = DummyListConfigAdaptor_::Engine;
			public:	static constexpr ::dcool::core::Boolean squeezedOnly = DummyListConfigAdaptor_::squeezedOnly;
			public: static constexpr Length squeezedCapacity = DummyListConfigAdaptor_::squeezedCapacity;
			public: static constexpr ::dcool::core::Boolean stuffed = DummyListConfigAdaptor_::stuffed;
			public: static constexpr ::dcool::core::Boolean circular = DummyListConfigAdaptor_::circular;

			public: static constexpr auto cStringLengthOf(Character const* cString_) noexcept -> ::dcool::core::Length {
				return ::dcool::container::detail_::cStringLengthOfOr_<Config>(
					[](Character const* cString_) -> ::dcool::core::Length {
						return BaseStandardCharacterTrait::length(cString_);
					},
					cString_
				);
			}

			public: static constexpr auto characterCompare(Character left_, Character right_) noexcept -> ComparisonCategory {
				return ::dcool::container::detail_::characterCompareOr_<Config>(
					[](Character left_, Character right_) noexcept -> ComparisonCategory {
						if (BaseStandardCharacterTrait::lt(left_, right_)) {
							return ComparisonCategory::less;
						}
						if (BaseStandardCharacterTrait::lt(right_, left_)) {
							return ComparisonCategory::greater;
						}
						if constexpr (requires {
							ComparisonCategory::equal;
						}) {
							if (BaseStandardCharacterTrait::eq(left_, right_)) {
								return ComparisonCategory::equal;
							}
						}
						return ComparisonCategory::equivalent;
					},
					left_,
					right_
				);
			}

			public: static constexpr auto characterSequenceCompare(
				Character const* left_, Character const* right_, Length length_
			) noexcept -> ComparisonCategory {
				return ::dcool::container::detail_::characterSequenceCompareOr_<Config>(
					[](Character const* left_, Character const* right_, Length length_) noexcept -> ComparisonCategory {
						int result_ = BaseStandardCharacterTrait::compare(left_, right_, length_);
						if (result_ < 0) {
							return ComparisonCategory::less;
						}
						if (result_ > 0) {
							return ComparisonCategory::greater;
						}
						if constexpr (requires {
							ComparisonCategory::equal;
						}) {
							return ComparisonCategory::equal;
						}
						return ComparisonCategory::equivalent;
					},
					left_,
					right_,
					length_
				);
			}

			// If this returns a value greater than 'length_', the caller should consider the character corrupted.
			public: static constexpr auto characterWidthInBytes(Character const* begin_, Length length_) noexcept -> Length {
				return ::dcool::container::detail_::characterSequenceCompareOr_<Config>(
					[](Character const* begin_, Length length_) noexcept -> Length {
						if constexpr (::dcool::core::isSame<Character, char>) {
							int result_ = std::mblen(begin_, length_);
							if (result_ < 0) {
								return length_ + 1;
							}
							return static_cast<Length>(result_);
						}
						if constexpr (::dcool::core::isSame<Character, char8_t>) {
							char8_t head_ = *begin_;
							if (head_ & 0b10000000 == 0) {
								return 1;
							}
							if (head_ & 0b11110000 == 0b11110000) {
								return 4;
							}
							if (head_ & 0b11100000 == 0b11100000) {
								return 3;
							}
							if (head_ & 0b11000000 == 0b11000000) {
								return 2;
							}
							return length_ + 1;
						}
						if constexpr (::dcool::core::isSame<Character, char16_t>) {
							char16_t head_ = *begin_;
							if (head_ & 0b10000000'00000000 == 0) {
								return 1;
							}
							if (head_ & 0b11111100'00000000 == 0b11011000'00000000) {
								return 2;
							}
							return length_ + 1;
						}
						return 1;
					},
					begin_,
					length_
				);
			}

			private: static constexpr ::dcool::core::Boolean characterTraitCustomized_ =
				::dcool::container::detail_::HasCallableCStringLengthOf_<Config, Character const*> ||
				::dcool::container::detail_::HasCallableCharacterCompare_<Config, Character, Character> ||
				::dcool::container::detail_::HasCallableCharacterSequenceCompare_<Config, Character const*, Character const*, Length>
			;

			private: struct StandardCharacterTrait_: public BaseStandardCharacterTrait {
				static constexpr auto length(Character const* cString_) noexcept -> ::dcool::core::Length {
					return cStringLengthOf(cString_);
				}

				static constexpr auto eq(Character left_, Character right_) noexcept -> ::dcool::core::Boolean {
					return characterCompare(left_, right_) == 0;
				}

				static constexpr auto lt(Character left_, Character right_) noexcept -> ::dcool::core::Boolean {
					return characterCompare(left_, right_) == ComparisonCategory::less;
				}

				static constexpr auto compare(
					Character const* left_, Character const* right_, ::dcool::core::Length length_
				) noexcept -> int {
					auto result_ = characterSequenceCompare(left_, right_, length_);
					if (result_ == ComparisonCategory::less) {
						return -1;
					} else if (result_ == ComparisonCategory::greater) {
						return 1;
					}
					return 0;
				}
			};

			public: using StandardCharacterTrait = ::dcool::core::ConditionalType<
				characterTraitCustomized_, StandardCharacterTrait_, BaseStandardCharacterTrait
			>;

			static_assert(::dcool::core::isTrivial<Character>);
		};
	}

	template <typename T_> concept StringConfig = requires {
		typename ::dcool::container::detail_::StringConfigAdaptor_<T_>;
	};

	template <typename ConfigT_> using StringConfigAdaptor = ::dcool::container::detail_::StringConfigAdaptor_<ConfigT_>;

	template <::dcool::container::StringConfig ConfigT_ = ::dcool::core::Empty<>> struct StringChassis {
		private: using Self_ = StringChassis<ConfigT_>;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::container::StringConfigAdaptor<Config>;
		public: using Character = ConfigAdaptor_::Character;
		public: using StandardCharacterTrait = ConfigAdaptor_::StandardCharacterTrait;
		public: using Pool = ConfigAdaptor_::Pool;
		public: using Engine = ConfigAdaptor_::Engine;
		public: template <
			typename AllocatorT__ = ::std::allocator<Character>
		> using StandardString = ::std::basic_string<Character, StandardCharacterTrait, AllocatorT__>;
		public: using StandardView = ::std::basic_string_view<Character, StandardCharacterTrait>;
		public: using Length = ConfigAdaptor_::Length;
		public: using ComparisonCategory = ConfigAdaptor_::ComparisonCategory;
		public: static constexpr ::dcool::core::Boolean squeezedOnly = ConfigAdaptor_::squeezedOnly;
		public: static constexpr Length squeezedCapacity = ConfigAdaptor_::squeezedCapacity;
		public: static constexpr ::dcool::core::Boolean stuffed = ConfigAdaptor_::stuffed;
		public: static constexpr ::dcool::core::Boolean circular = ConfigAdaptor_::circular;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy =
			ConfigAdaptor_::exceptionSafetyStrategy
		;

		public: struct UnderlyingConfig_ {
			using Pool = Pool;
			using Engine = Engine;
			public: static constexpr ::dcool::core::Boolean squeezedOnly = squeezedOnly;
			public: static constexpr Length squeezedCapacity = squeezedCapacity;
			public: static constexpr ::dcool::core::Boolean stuffed = stuffed;
			public: static constexpr ::dcool::core::Boolean circular = circular;
			public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy = exceptionSafetyStrategy;
		};

		private: using Underlying_ = ::dcool::container::ListChassis<Character, UnderlyingConfig_>;
		public: using Value = Underlying_::Value;
		public: using Index = Underlying_::Index;
		public: using Iterator = Underlying_::Iterator;
		public: using ConstIterator = Underlying_::ConstIterator;

		private: Underlying_ m_underlying_;

		public: constexpr void initlialize(Engine& engine_) noexcept {
			this->m_underlying_.initialize(engine_);
		}

		public: constexpr void initialize(Engine& engine_, Length length_) {
			this->m_underlying_.initialize(engine_, length_);
		}

		public: constexpr void initialize(Engine& engine_, Character const* cString_) {
			this->initialize(engine_, ::dcool::core::rangeInput, cString_, ConfigAdaptor_::cStringLengthOf(cString_));
		}

		public: constexpr void initialize(Engine& engine_, Character const* begin_, Character const* end_) {
			this->m_underlying_.initialize(engine_, ::dcool::core::rangeInput, begin_, end_);
		}

		public: template <::dcool::core::InputIterator IteratorT__> constexpr void initialize(
			Engine& engine_, ::dcool::core::RangeInputTag tag_, IteratorT__ otherBegin_, IteratorT__ otherEnd_
		) {
			this->m_underlying_.initialize(engine_, tag_, otherBegin_, otherEnd_);
		}

		public: template <::dcool::core::InputIterator IteratorT__> constexpr void initialize(
			Engine& engine_, ::dcool::core::RangeInputTag tag_, IteratorT__ otherBegin_, Length length_
		) {
			this->m_underlying_.initialize(engine_, tag_, otherBegin_, length_);
		}

		public: template <typename AllocatorT__> constexpr void initialize(
			Engine& engine_, StandardString<AllocatorT__> const& standardString_
		) {
			this->initialize(engine_, ::dcool::core::rangeInput, standardString_.begin(), standardString_.end());
		}

		public: constexpr void uninitialize(Engine& engine_) noexcept {
			this->m_underlying_.uninitialize(engine_);
		}

		public: template <typename ConfigT__> constexpr void cloneTo(
			Engine& engine_,
			::dcool::container::StringChassis<ConfigT__>::Engine& otherEngine_,
			::dcool::container::StringChassis<ConfigT__>& other_
		) const {
			this->m_underlying_.cloneTo(engine_, otherEngine_, other_.m_underlying_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void relocateTo(Engine& engine_, Engine& otherEngine_, Self_& other_) {
			this->m_underlying_.relocateTo(engine_, otherEngine_, other_.m_underlying_);
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy,
			::dcool::core::Boolean engineWillBeSwappedC__ = false
		> constexpr void swapWith(
			Engine& engine_, Engine& otherEngine_, Self_& other_
		) {
			this->m_underlying_.template swapWith<strategyC__, engineWillBeSwappedC__>(engine_, otherEngine_, other_.m_underlying_);
		}

		public: constexpr auto vacant(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
			return this->m_underlying_.vacant(engine_);
		}

		public: constexpr auto full(Engine& engine_) const noexcept -> ::dcool::core::Boolean {
			return this->m_underlying_.full(engine_);
		}

		public: constexpr auto length(Engine& engine_) const noexcept -> Length {
			return this->m_underlying_.length(engine_);
		}

		public: constexpr auto capacity(Engine& engine_) const noexcept -> Length {
			return this->m_underlying_.capacity(engine_);
		}

		public: constexpr auto data(Engine& engine_) const noexcept -> Character const* {
			return this->m_underlying_.data(engine_);
		}

		public: constexpr auto data(Engine& engine_) noexcept -> Character* {
			return this->m_underlying_.data(engine_);
		}

		public: constexpr auto begin(Engine& engine_) const noexcept -> ConstIterator {
			return this->m_underlying_.begin(engine_);
		}

		public: constexpr auto begin(Engine& engine_) noexcept -> Iterator {
			return this->m_underlying_.begin(engine_);
		}

		public: constexpr auto end(Engine& engine_) const noexcept -> ConstIterator {
			return this->m_underlying_.end(engine_);
		}

		public: constexpr auto end(Engine& engine_) noexcept -> Iterator {
			return this->m_underlying_.end(engine_);
		}

		public: constexpr auto rawPointerAt(Engine& engine_, Index index_) const noexcept -> Character const* {
			return this->m_underlying_.rawPointerAt(engine_);
		}

		public: constexpr auto rawPointerAt(Engine& engine_, Index index_) noexcept -> Character* {
			return this->m_underlying_.rawPointerAt(engine_);
		}

		public: constexpr auto access(Engine& engine_, Index index_) const noexcept -> Character const& {
			return this->m_underlying_.access(engine_);
		}

		public: constexpr auto access(Engine& engine_, Index index_) noexcept -> Character& {
			return this->m_underlying_.access(engine_);
		}

		public: constexpr auto front(Engine& engine_) const noexcept -> Character const& {
			return this->m_underlying_.front(engine_);
		}

		public: constexpr auto front(Engine& engine_) noexcept -> Character& {
			return this->m_underlying_.front(engine_);
		}

		public: constexpr auto back(Engine& engine_) const noexcept -> Character const& {
			return this->m_underlying_.back(engine_);
		}

		public: constexpr auto back(Engine& engine_) noexcept -> Character& {
			return this->m_underlying_.back(engine_);
		}

		public: constexpr void terminateByNull(Engine& engine_) requires (!stuffed) {
			if (this->m_underlying_.full(engine_)) {
				this->m_underlying_.forceExpandBack_(engine_, 1);
			}
			new (this->m_underlying_.rawPointerAt(engine_, this->length(engine_))) Character();
		}

		public: constexpr auto cString(Engine& engine_) const noexcept -> Character const* {
			return ::dcool::core::launder(this->data(engine_));
		}

		public: template <
			typename AllocatorT__ = ::std::allocator<Character>
		> constexpr auto toStandard(Engine& engine_) const noexcept -> StandardString<AllocatorT__> {
			return StandardString<AllocatorT__>(this->begin(engine_), this->end(engine_));
		}

		public: constexpr auto toStandardView(Engine& engine_) const noexcept -> StandardView requires (!circular) {
			return StandardView(this->begin(engine_), this->end(engine_));
		}

		public: static constexpr auto compare(
			Engine& leftEngine_, Self_ const& left_, Engine& rightEngine_, Self_ const& right_
		) noexcept -> ComparisonCategory {
			Length commonLength = ::dcool::core::min(left_.length(leftEngine_), right_.length(rightEngine_));
			auto compareResult_ = ConfigAdaptor_::characterSequenceCompare(
				left_.data(leftEngine_), right_.data(rightEngine_), commonLength
			);
			if (compareResult_ == 0) {
				if (left_.length(leftEngine_) < right_.length(rightEngine_)) {
					return ComparisonCategory::less;
				} else if (right_.length(rightEngine_) < left_.length(leftEngine_)) {
					return ComparisonCategory::greater;
				}
			}
			return compareResult_;
		}

		public: static constexpr auto equivalent(
			Engine& leftEngine_, Self_ const& left_, Engine& rightEngine_, Self_ const& right_
		) noexcept -> ::dcool::core::Boolean {
			if (left_.length(leftEngine_) != right_.length(rightEngine_)) {
				return false;
			}
			auto compareResult_ = ConfigAdaptor_::characterSequenceCompare(
				left_.data(leftEngine_), right_.data(rightEngine_), left_.length(leftEngine_)
			);
			return compareResult_ == 0;
		}
	};

	template <::dcool::container::StringConfig ConfigT_ = ::dcool::core::Empty<>> struct String {
		public: using Self_ = String<ConfigT_>;
		public: using Config = ConfigT_;

		public: using Chassis = ::dcool::container::StringChassis<Config>;
		public: using Value = Chassis::Value;
		public: using Character = Chassis::Character;
		public: using StandardCharacterTrait = Chassis::StandardCharacterTrait;
		public: using ComparisonCategory = Chassis::ComparisonCategory;
		public: using Length = Chassis::Length;
		public: using Index = Chassis::Index;
		public: using Engine = Chassis::Engine;
		public: using Iterator = Chassis::Iterator;
		public: using ConstIterator = Chassis::ConstIterator;
		public: template <
			typename AllocatorT__ = ::std::allocator<Character>
		> using StandardString = Chassis::template StandardString<AllocatorT__>;
		public: using StandardView = Chassis::StandardView;
		public: static constexpr ::dcool::core::ExceptionSafetyStrategy exceptionSafetyStrategy = Chassis::exceptionSafetyStrategy;

		private: Chassis m_chassis_;
		private: mutable Engine m_engine_;

		public: constexpr String() noexcept(noexcept(this->chassis().initialize(this->engine_()))) {
			this->chassis().initialize(this->engine_());
		}

		public: constexpr String(Self_ const& other_): m_engine_(other_.engine_()) {
			other_.chassis().cloneTo(other_.engine_(), this->engine_(), this->chassis());
		}

		public: constexpr String(Self_&& other_) noexcept: m_engine_(other_.engine_()) {
			other_.chassis().relocateTo(other_.engine_(), this->engine_(), this->chassis());
			other_.chassis().initialize(other_.engine_());
		}

		public: constexpr explicit String(Length capacity_) {
			this->chassis().initialize(this->engine_(), capacity_);
		}

		public: constexpr String(Character const* cString_) {
			this->chassis().initialize(this->engine_(), cString_);
		}

		public: constexpr String(Character const* cStringBegin_, Character const* cStringEnd_) {
			this->chassis().initialize(this->engine_(), cStringBegin_, cStringEnd_);
		}

		public: template <::dcool::core::InputIterator IteratorT_> constexpr String(
			::dcool::core::RangeInputTag tag_, IteratorT_ otherBegin_, Length count_
		) {
			this->chassis().initialize(this->engine_(), tag_, otherBegin_, count_);
		}

		public: template <::dcool::core::InputIterator IteratorT_> constexpr String(
			::dcool::core::RangeInputTag tag_, IteratorT_ otherBegin_, IteratorT_ otherEnd_
		) {
			this->chassis().initialize(this->engine_(), tag_, otherBegin_, otherEnd_);
		}

		public: template <typename AllocatorT__> constexpr explicit String(StandardString<AllocatorT__> const& standardString_) {
			this->chassis().initialize(this->engine_(), standardString_);
		}

		public: constexpr ~String() noexcept {
			this->chassis().uninitialize(this->engine_());
		}

		public: constexpr auto operator =(Self_ const& other_) -> Self_& {
			Self_ middleMan_(other_);
			this->swapWith(middleMan_);
			return *this;
		}

		public: constexpr auto operator =(Self_&& other_) noexcept -> Self_& {
			this->swapWith(other_);
			return *this;
		}

		public: template <
			::dcool::core::ExceptionSafetyStrategy strategyC__ = exceptionSafetyStrategy
		> constexpr void swapWith(Self_& other_) noexcept {
			this->chassis().template swapWith<strategyC__, true>(this->engine_(), other_.engine_(), other_.chassis());
			::dcool::core::intelliSwap(this->engine_(), other_.engine_());
		}

		public: constexpr auto vacant() const noexcept -> ::dcool::core::Boolean {
			return this->chassis().vacant(this->engine_());
		}

		public: constexpr auto full() const noexcept -> ::dcool::core::Boolean {
			return this->chassis().full(this->engine_());
		}

		public: constexpr auto length() const noexcept -> Length {
			return this->chassis().length(this->engine_());
		}

		public: constexpr auto capacity() const noexcept -> Length {
			return this->chassis().capacity(this->engine_());
		}

		public: constexpr auto chassis() const noexcept -> Chassis const& {
			return this->m_chassis_;
		}

		private: constexpr auto chassis() noexcept -> Chassis& {
			return this->m_chassis_;
		}

		public: constexpr auto engine() const noexcept -> Engine const& {
			return this->m_engine_;
		}

		private: constexpr auto engine_() const noexcept -> Engine& {
			return this->m_engine_;
		}

		public: constexpr auto begin() const noexcept -> ConstIterator {
			return this->chassis().begin(this->engine_());
		}

		public: constexpr auto begin() noexcept -> Iterator {
			return this->chassis().begin(this->engine_());
		}

		public: constexpr auto end() const noexcept -> ConstIterator {
			return this->chassis().end(this->engine_());
		}

		public: constexpr auto end() noexcept -> Iterator {
			return this->chassis().end(this->engine_());
		}

		public: constexpr auto position(Index index_) const noexcept -> ConstIterator {
			return this->chassis().position(this->engine_(), index_);
		}

		public: constexpr auto position(Index index_) noexcept -> Iterator {
			return this->chassis().position(this->engine_(), index_);
		}

		public: constexpr auto data() const noexcept -> Character const* {
			return this->chassis().data(this->engine_());
		}

		public: constexpr auto data() noexcept -> Character* {
			return this->chassis().data(this->engine_());
		}

		public: constexpr auto rawPointerAt(Index index_) const noexcept -> Character const* {
			return this->chassis().rawPointerAt(this->engine_(), index_);
		}

		public: constexpr auto rawPointerAt(Index index_) noexcept -> Character* {
			return this->chassis().rawPointerAt(this->engine_(), index_);
		}

		public: constexpr auto access(Index index_) const noexcept -> Character const& {
			return this->chassis().access(this->engine_(), index_);
		}

		public: constexpr auto access(Index index_) noexcept -> Character& {
			return this->chassis().access(this->engine_(), index_);
		}

		public: constexpr auto front() const noexcept -> Character const& {
			return this->chassis().front(this->engine_());
		}

		public: constexpr auto front() noexcept -> Character& {
			return this->chassis().front(this->engine_());
		}

		public: constexpr auto back() const noexcept -> Character const& {
			return this->chassis().back(this->engine_());
		}

		public: constexpr auto back() noexcept -> Character& {
			return this->chassis().back(this->engine_());
		}

		public: constexpr auto operator [](Index index_) const noexcept -> Character const& {
			return this->access(index_);
		}

		public: constexpr auto operator [](Index index_) noexcept -> Character& {
			return this->access(index_);
		}

		public: constexpr void terminateByNull() {
			this->chassis().terminateByNull(this->engine_());
		}

		public: template <
			typename AllocatorT__ = ::std::allocator<Character>
		> constexpr auto toStandard() const -> StandardString<AllocatorT__> {
			return this->chassis().template toStandard<AllocatorT__>(this->engine_());
		}

		public: constexpr explicit operator StandardString<>() const noexcept {
			return this->toStandard();
		}

		public: constexpr auto toStandardView() const noexcept -> StandardView {
			return this->chassis().toStandardView(this->engine_());
		}

		public: constexpr explicit operator StandardView() const noexcept {
			return this->toStandardView();
		}

		public: static constexpr auto compareValue(Self_ const& left_, Self_ const& right_) noexcept -> ComparisonCategory {
			return Chassis::compare(left_.engine_(), left_.chassis(), right_.engine_(), right_.chassis());
		}

		public: friend constexpr auto operator <=>(Self_ const& left_, Self_ const& right_) noexcept -> ComparisonCategory {
			return compareValue(left_, right_);
		}

		public: static constexpr auto valueEquivalent(Self_ const& left_, Self_ const& right_) noexcept -> ::dcool::core::Boolean {
			return Chassis::equivalent(left_.engine_(), left_.chassis(), right_.engine_(), right_.chassis());
		}

		public: friend constexpr auto operator ==(Self_ const& left_, Self_ const& right_) noexcept -> ::dcool::core::Boolean {
			return valueEquivalent(left_, right_);
		}

		public: friend constexpr auto operator <<(
			::std::basic_ostream<Character, StandardCharacterTrait>& left_, Self_ const& right_
		) -> ::std::ostream& {
			return left_ << right_.toStandardView();
		}

		public: friend constexpr auto operator >>(
			::std::basic_istream<Character, StandardCharacterTrait>& left_, Self_& right_
		) -> ::std::ostream& {
			StandardString<> middleMan_;
			left_ >> middleMan_;
			right_ = fromStandard(middleMan_);
			return left_;
		}
	};
}

#endif
