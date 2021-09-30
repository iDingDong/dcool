#ifndef DCOOL_ATOMIC_FLAG_HPP_INLCUDED_
#	define DCOOL_ATOMIC_FLAG_HPP_INLCUDED_ 1

#	include <dcool/core.hpp>

#	include <atomic>

namespace dcool::concurrency {
	struct AtomicFlag {
		private: using Self_ = AtomicFlag;

		private: ::std::atomic_flag m_underlying_;

		public: constexpr AtomicFlag() noexcept = default;

		public: AtomicFlag(::dcool::core::Boolean initial_) noexcept {
			if (initial_) {
				static_cast<void>(this->testAndSet());
			}
		}

#	if DCOOL_CPP_ATOMIC_FLAG_TEST_VERSION >= DCOOL_CPP_ATOMIC_FLAG_TEST_VERSION_FOR_P1135R6
		public: auto test(::std::memory_order order_ = ::std::memory_order::seq_cst) const noexcept -> ::dcool::core::Boolean {
			return this->m_underlying_.test(order_);
		}
#	endif

		public: auto testAndSet(::std::memory_order order_ = ::std::memory_order::seq_cst) noexcept -> ::dcool::core::Boolean {
			return this->m_underlying_.test_and_set(order_);
		}

		public: void clear(::std::memory_order order_ = ::std::memory_order::seq_cst) noexcept {
			this->m_underlying_.clear(order_);
		}
	};

	class NegativeAtomicFlag {
		private: using Self_ = NegativeAtomicFlag;

		private: ::dcool::concurrency::AtomicFlag m_underlying_;

		public: constexpr NegativeAtomicFlag() noexcept = default;

		public: NegativeAtomicFlag(::dcool::core::Boolean initial_) noexcept {
			if (!initial_) {
				static_cast<void>(this->testAndClear());
			}
		}

		public: void set(::std::memory_order order_ = ::std::memory_order::seq_cst) noexcept {
			this->m_underlying_.clear(order_);
		}

		public: auto testAndClear(::std::memory_order order_ = ::std::memory_order::seq_cst) noexcept -> ::dcool::core::Boolean {
			return !(this->m_underlying_.testAndSet(order_));
		}
	};
}

#endif
