#ifndef DCOOL_CORE_LIST_HPP_INCLUDED_
#	define DCOOL_CORE_LIST_HPP_INCLUDED_ 1

#	include <dcool/core/utility.hpp>
#	include <dcool/core/integer.hpp>
#	include <dcool/core/memory.hpp>

namespace dcool::core {
	template <typename T_> concept StaticListConfig = true;

	namespace detail_ {
		template <::dcool::core::SignedMaxInteger endIndexC_> struct ArrayLightIteratorBase_ {
			private: using Self_ = ArrayLightIteratorBase_<endIndexC_>;
			public: static constexpr ::dcool::core::SignedMaxInteger endIndex_ = endIndexC_;

			public: using Length = ::dcool::core::IntegerType<endIndex_>;
			public: using Index = Length;
			public: using Difference = ::dcool::core::IntegerType<endIndex_, -endIndex_>;

			private: Index m_index_;

			public: constexpr ArrayLightIteratorBase_(Index index_) noexcept: m_index_(index_) {
			}

			public: constexpr void advance(Difference step_ = 1) noexcept {
				this->m_index_ += step_;
			}

			public: constexpr void retreat(Difference step_ = 1) noexcept {
				this->advance(-step_);
			}

			public: constexpr auto next(Difference step_ = 1) noexcept {
				return ArrayLightIteratorBase_(this->m_index_ + step_);
			}

			public: constexpr auto previous(Difference step_ = 1) noexcept {
				return this->next(-step_);
			}

			public: constexpr auto equalsTo(Self_ const& other_) const noexcept {
				return this->m_index_ == other_.m_index_;
			}

			public: static constexpr auto equal(Self_ const& first_, Self_ const& second_) noexcept {
				return first_.equalsTo(second_);
			}

			public: friend constexpr auto operator ==(Self_ const& first_, Self_ const& second_) noexcept {
				return equal(first_, second_);
			}
		};
	}

	template <
		typename ValueT_, ::dcool::core::Length capacityC_, ::dcool::core::StaticListConfig ConfigT_ = ::dcool::core::Empty<>
	> struct StaticList {
		private: using Self_ = StaticList<ValueT_, capacityC_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;
		private: static constexpr ::dcool::core::Length capacity_ = capacityC_;

		public: using Length = ::dcool::core::IntegerType<capacity_>;
		public: using StorageType = ::dcool::core::StorageFor<Value[capacity_]>;

		private: StorageType m_storage_;
		private: Length m_length_;

		public: constexpr StaticList() noexcept: m_length_(0) {
		}

		public: constexpr auto length() const noexcept -> Length {
			return this->m_length_;
		}

		public: consteval auto capacity() const noexcept -> Length {
			return static_cast<Length>(capacity_);
		}

		public: constexpr auto empty() const noexcept {
			return this->length() == 0;
		}

		public: constexpr auto full() const noexcept {
			return this->length() ==  this->capacity();
		}
	};
}

#endif
