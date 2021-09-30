#ifndef DCOOL_RESOURCE_UNIQUE_POINTER_HPP_INCLUDED_
#	define DCOOL_RESOURCE_UNIQUE_POINTER_HPP_INCLUDED_ 1

#	include <dcool/resource/pool.hpp>
#	include <dcool/resource/dismissor.hpp>

#	include <memory>

namespace dcool::resource {
	template <typename ValueT_, typename DismissorT_ = ::dcool::resource::DefaultDestroyer> struct UniquePointer {
		public: using Self_ = UniquePointer<ValueT_, DismissorT_>;
		public: using Value = ValueT_;
		public: using Dismissor = DismissorT_;

		private: using UnderlyingDeleter_ = ::dcool::resource::StandardDeleter<DismissorT_>;
		private: using Underlying_ = ::std::unique_ptr<ValueT_, UnderlyingDeleter_>;

		private: Underlying_ m_underlying_;

		public: constexpr UniquePointer() noexcept = default;
		public: constexpr UniquePointer(Self_ const&) noexcept = delete;
		public: constexpr UniquePointer(Self_&&) = default;

		public: constexpr UniquePointer(ValueT_* pointer_) noexcept: m_underlying_(pointer_) {
		}

		public: template <::dcool::core::FormOfSame<Dismissor> DismissorT__> constexpr UniquePointer(
			ValueT_* pointer_, DismissorT__&& dismissor_
		) noexcept: m_underlying_(pointer_, UnderlyingDeleter_ { .dismissor = ::dcool::core::forward<DismissorT__>(dismissor_) }) {
		}

		public: constexpr auto operator =(Self_ const&) noexcept -> Self_& = delete;
		public: constexpr auto operator =(Self_&&) noexcept -> Self_& = default;

		public: constexpr auto dismissor() const noexcept -> Dismissor const& {
			return this->m_underlying_.get_deleter().dismissor;
		}

		public: constexpr auto dismissor() noexcept -> Dismissor& {
			return this->m_underlying_.get_deleter().dismissor;
		}

		public: constexpr auto dereferenceSelf() const noexcept -> Value& {
			return *(this->m_underlying_);
		}

		public: constexpr auto operator *() const noexcept -> Value& {
			return this->dereferenceSelf();
		}

		public: constexpr auto rawPointer() const noexcept -> Value* {
			return ::dcool::core::addressOf(this->dereferenceSelf());
		}

		public: constexpr auto operator ->() const noexcept -> Value* {
			return this->rawPointer();
		}

		public: constexpr void reset() noexcept {
			this->m_underlying_.reset();
		}

		public: friend constexpr auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
		public: friend constexpr auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	template <typename ValueT_, typename PoolT_, typename... ArgumentTs_> requires ::dcool::resource::PoolFor<
		::dcool::core::ReferenceRemovedType<PoolT_>, ValueT_
	> constexpr auto makeUniqueByPool(PoolT_&& pool_, ArgumentTs_&&... arguments_) {
		using Pool_ = ::dcool::core::ReferenceRemovedType<PoolT_>;
		ValueT_* pointer_ = static_cast<ValueT_*>(::dcool::resource::adaptedAllocatePointerFor<ValueT_>(pool_));
		try {
			pointer_ = new (pointer_) ValueT_(::dcool::core::forward<ArgumentTs_>(arguments_)...);
		} catch (...) {
			::dcool::resource::adaptedDeallocatePointerFor<ValueT_>(pool_, pointer_);
			throw;
		}
		return ::dcool::resource::UniquePointer<ValueT_, ::dcool::resource::PoolDestroyer<Pool_>>(
			pointer_, ::dcool::resource::PoolDestroyer<Pool_>{ .pool = ::dcool::core::forward<PoolT_>(pool_) }
		);
	}

	template <typename ValueT_, typename DismissorT_> constexpr auto makeUniqueGuard(ValueT_& value_, DismissorT_&& dismissor_) {
		return ::dcool::resource::UniquePointer<ValueT_, ::dcool::core::ReferenceRemovedType<DismissorT_>>(
			::dcool::core::addressOf(value_), ::dcool::core::forward<DismissorT_>(dismissor_)
		);
	}
}

#endif
