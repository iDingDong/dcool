#ifndef DCOOL_CORE_CONCRETE_POOL_HPP_INCLUDED_
#	define DCOOL_CORE_CONCRETE_POOL_HPP_INCLUDED_ 1

#	include <dcool/core/alignment.hpp>
#	include <dcool/core/integer.hpp>
#	include <dcool/core/memory.hpp>

#	include <array>
#	include <limits>
#	include <stdexcept>

namespace dcool::core {
	template <
		::dcool::core::Length unitCountC_,
		::dcool::core::Size unitSizeC_,
		::dcool::core::Size alignmentC_ = ::dcool::core::defaultAlignmentFor<unitSizeC_>
	> class ConcretePool {
		private: using Self_ = ConcretePool<unitCountC_, unitSizeC_, alignmentC_>;
		public: static constexpr ::dcool::core::Length unitCount = unitCountC_;
		public: static constexpr ::dcool::core::Size maxSize = unitSizeC_;
		public: static constexpr ::dcool::core::Alignment maxAlignment= alignmentC_;
		public: static constexpr ::dcool::core::Alignment defaultAlignment= alignmentC_;

		public: using Length = ::dcool::core::StorableUnsignedIntegerType<maxSize, maxAlignment>;
		public: using Index = Length;
		public: using UnifiedHandle = Index;
		public: using UnifiedConstHandle = Index;
		public: using Handle = UnifiedHandle;
		public: using ConstHandle = UnifiedConstHandle;
		private: using UnitStorage_ = ::dcool::core::AlignedStorage<maxSize, maxAlignment>;

		static_assert(
			unitCountC_ <= ::std::numeric_limits<Index>::max(),
			"'dcool::core::ConcretePool' need chunk suffitient to hold an index."
		);

		private: ::std::array<UnitStorage_, unitCount> m_storage_;
		private: Index m_first_;

		public: constexpr ConcretePool() noexcept: m_first_(0) {
			for (Index i = 0; i < unitCount; ++i) {
				this->link_(i, i + 1);
			}
		}

		public: ConcretePool(Self_ const&) = delete;
		public: ConcretePool(Self_&&) = delete;
		public: auto operator =(Self_ const&) -> Self_& = delete;
		public: auto operator =(Self_&&) -> Self_& = delete;

		private: constexpr auto nextAvailableOf_(Index index_) const noexcept -> Index {
			return reinterpret_cast<Index const&>(this->m_storage_[index_]);
		}

		private: constexpr void link_(Index leading_, Index following_) noexcept {
			reinterpret_cast<Index&>(this->m_storage_[leading_]) = following_;
		}

		public: constexpr auto exhausted() const noexcept {
			return this->m_first_ = unitCount;
		}

		public: template <
			::dcool::core::Size sizeC__, ::dcool::core::Size alignmentC__ = defaultAlignment
		> constexpr auto allocate() -> Handle {
			if constexpr (sizeC__ > maxSize || alignmentC__ > maxAlignment) {
				throw ::std::bad_alloc("'dcool::core::ConcretePool' cannot provide requested chunk.");
			}
			if (this->exhausted()) {
				throw ::std::bad_alloc("Attempted to allocate from an exhausted 'dcool::core::ConcretePool'.");
			}
			Index result_ = this->m_first_;
			this->m_first_ = this->nextAvailableOf_(result_);
			return result_;
		}

		public: template <
			::dcool::core::Size sizeC__, ::dcool::core::Size alignmentC__ = defaultAlignment
		> constexpr void deallocate(Handle handle_) {
			if constexpr (sizeC__ > maxSize || alignmentC__ > maxAlignment) {
				::dcool::core::terminate();
			}
			this->link_(handle_, this->m_first_);
			this->m_first_ = handle_;
		}

		public: struct HandleConverter {
			Self_* pool_;

			constexpr auto operator ()(Handle index_) const noexcept -> void* {
				return ::dcool::core::addressOf(pool_->m_storage_[index_]);
			}

			constexpr auto operator ()(void const* pointer_) const noexcept -> Handle {
				return static_cast<Handle>(
					static_cast<UnitStorage_ const*>(pointer_) - (::dcool::core::addressOf(pool_->m_storage_[0]))
				);
			}
		};

		public: using ConstHandleConverter = HandleConverter;

		public: template <
			::dcool::core::Size sizeC__, ::dcool::core::Size alignmentC__ = defaultAlignment
		> constexpr auto handleConverter() noexcept {
			return HandleConverter(this);
		}

		public: template <
			::dcool::core::Size sizeC__, ::dcool::core::Size alignmentC__ = defaultAlignment
		> constexpr auto constHandleConverter() noexcept {
			return this->handleConverter();
		}
	};
}

#endif
