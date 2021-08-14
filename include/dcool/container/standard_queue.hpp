#ifndef DCOOL_CONTAINER_STANDARD_QUEUE_HPP_INCLUDED_
#	define DCOOL_CONTAINER_STANDARD_QUEUE_HPP_INCLUDED_ 1

#	include <functional>
#	include <queue>
#	include <vector>

namespace dcool::container {
	template <
		typename ValueT_,
		typename ContainerT_ = ::std::vector<ValueT_>,
		typename LessThanComparerT_ = ::std::less<typename ContainerT_::value_type>
	> class StandardPriorityQueue: private ::std::priority_queue<ValueT_, ContainerT_, LessThanComparerT_> {
		private: using Self_ = StandardPriorityQueue<ValueT_, ContainerT_, LessThanComparerT_>;
		private: using Super_ = ::std::priority_queue<ValueT_, ContainerT_, LessThanComparerT_>;
		public: using Value = ValueT_;

		public: using Length = Super_::size_type;

		public: using Super_::Super_;
		public: using Super_::top;
		public: using Super_::push;
		public: using Super_::emplace;
		public: using Super_::pop;

		public: constexpr auto vacant() const noexcept -> ::dcool::core::Boolean {
			return this->Super_::empty();
		}

		public: constexpr auto length() const noexcept -> Length {
			return this->Super_::size();
		}

		public: constexpr auto popValue() -> Value {
			Value output_ = ::dcool::core::move(this->Super_::c.front());
			this->pop();
			return output_;
		}
	};
}

#endif
