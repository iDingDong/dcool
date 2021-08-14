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

		public: auto testAndSet() noexcept -> ::dcool::core::Boolean {
			return this->m_underlying_.test_and_set();
		}

		public: void clear() noexcept {
			this->m_underlying_.clear();
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

		public: void set() noexcept {
			this->m_underlying_.clear();
		}

		public: auto testAndClear() noexcept -> ::dcool::core::Boolean {
			return !(this->m_underlying_.testAndSet());
		}
	};
}

#endif
