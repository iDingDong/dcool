#ifndef DCOOL_CORE_REPLICATOR_HPP_INCLUDED_
#	define DCOOL_CORE_REPLICATOR_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>

namespace dcool::core {
	template <typename ValueT_, typename ConfigT_ = ::dcool::core::Empty<>> class Replicator {
		private: using Self_ = Replicator<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: Value* m_value_ = ::dcool::core::nullPointer;

		public: constexpr Replicator() noexcept {
		}

		public: constexpr Replicator(Value& value_) noexcept: m_value_(::dcool::core::addressOf(value_)) {
		}

		public: constexpr auto rawPointer() const noexcept -> Value* {
			return this->m_value_;
		}

		public: constexpr auto dereferenceSelf() const noexcept -> Value& {
			return *(this->rawPointer());
		}

		public: constexpr auto operator ->() const noexcept -> Value& {
			return this->rawPointer();
		}

		public: constexpr auto operator *() const noexcept -> Value& {
			return this->dereferenceSelf();
		}

		public: constexpr auto operator ++() noexcept -> Self_& {
			return *this;
		}

		public: constexpr auto operator ++(::dcool::core::PostDisambiguator) noexcept -> Self_ {
			return *this;
		}
	};
}

#endif
