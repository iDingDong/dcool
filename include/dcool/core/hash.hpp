#ifndef DCOOL_CORE_HASH_HPP_INCLUDED_
#	define DCOOL_CORE_HASH_HPP_INCLUDED_ 1

#	include <dcool/core/basic.hpp>
#	include <dcool/core/concept.hpp>

#	include <functional>
#	include <numeric>

namespace dcool::core {
	template <
		typename ValueT_,
		typename WrappedT_ = ::std::hash<ValueT_>,
		::dcool::core::UnsignedIntegral ResultT_ = ::dcool::core::Length
	> struct StandardHasherWrapperFor {
		private: using Self_ = StandardHasherWrapperFor<ValueT_, WrappedT_, ResultT_>;
		public: using Value = ValueT_;
		public: using Wrapped = WrappedT_;

		[[no_unique_address]] Wrapped underlying;

		constexpr auto operator ()(
			Value const& value_, ResultT_ limit_ = ::std::numeric_limits<ResultT_>::max()
		) const noexcept(noexcept(static_cast<ResultT_>(this->underlying(value_) % limit_))) -> ResultT_ {
			return static_cast<ResultT_>(this->underlying(value_) % limit_);
		}

		public: friend auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean {
			return true;
		}

		public: friend auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
	};

	template <
		typename ValueT_, ::dcool::core::UnsignedIntegral ResultT_ = ::dcool::core::Length
	> using DefaultHasherFor = ::dcool::core::StandardHasherWrapperFor<ValueT_, ::std::hash<ValueT_>, ResultT_>;
}

#endif
