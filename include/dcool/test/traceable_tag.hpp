#ifndef DCOOL_TEST_TRACEABLE_TAG_HPP_INCLUDED_
#	define DCOOL_TEST_TRACEABLE_TAG_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>
#	include <dcool/concurrency.hpp>

#	include <numeric>
#	include <set>

namespace dcool::test {
	namespace detail_ {
		template <typename IdT_> class IdSet_ {
			public: using Id = IdT_;

			private: ::std::set<Id> m_ids_;
			private: ::dcool::concurrency::Mutex<> m_mutex_;

			public: auto insert(Id id_)& -> ::dcool::core::Boolean {
				::dcool::core::PhoneyLockGuard locker_(this->m_mutex_);
				return this->m_ids_.insert(id_).second;
			}

			public: auto erase(Id id_)& noexcept -> ::dcool::core::Boolean {
				::dcool::core::PhoneyLockGuard locker_(this->m_mutex_);
				return this->m_ids_.erase(id_) > 0;
			}

			public: auto count() const noexcept -> ::dcool::core::Size {
				::dcool::core::PhoneySharedLockGuard locker_(this->m_mutex_);
				return this->m_ids_.size();
			}
		};
	}

	template <typename ConfigT_ = ::dcool::core::Empty<>> class TraceableTag {
		public: using Config = ConfigT_;

		public: using Id = ::dcool::core::UnsignedMaxInteger;
		public: using IdSet = ::dcool::test::detail_::IdSet_<Id>;
		public: using Value = ::dcool::core::UnsignedMaxInteger;
		public: static constexpr Id invalidId = ::std::numeric_limits<Id>::max();

		private: Id const m_physicalId_ = allocateId_();
		private: Id m_logicalId_;
		private: Value m_value_;

		public: TraceableTag() noexcept: TraceableTag(this->logicalId()) {
		}

		public: explicit TraceableTag(
			Value value_
		) noexcept: m_logicalId_(this->physicalId()), m_value_(::dcool::core::move(value_)) {
			physicallyLivingSet_().insert(this->physicalId());
			logicallyLivingSet_().insert(this->logicalId());
		}

		public: TraceableTag(TraceableTag const& other_) noexcept: m_logicalId_(this->physicalId()), m_value_(other_.value()) {
			physicallyLivingSet_().insert(this->physicalId());
			logicallyLivingSet_().insert(this->logicalId());
		}

		public: TraceableTag(TraceableTag&& other_) noexcept: m_logicalId_(other_.logicalId()), m_value_(other_.value()) {
			other_.invalidate_();
			physicallyLivingSet_().insert(this->physicalId());
		}

		public: ~TraceableTag() noexcept {
			physicallyLivingSet_().erase(this->physicalId());
			if (this->valid()) {
				logicallyLivingSet_().erase(this->m_logicalId_);
			}
		}

		public: auto operator =(TraceableTag const& other_)& noexcept -> TraceableTag& {
			this->m_value_ = other_.value();
			return *this;
		}

		public: auto operator =(TraceableTag&& other_)& noexcept -> TraceableTag& {
			this->m_logicId_ = other_.logicId();
			this->m_value_ = other_.value();
			other_.invalidate_();
			return *this;
		}

		private: static auto allocateId_() noexcept -> Id {
			static ::dcool::concurrency::Atom<Id> nextId_ = 0;
			Id result_ = nextId_.fetchAdd(1, ::std::memory_order::relaxed);
			if (result_ == invalidId) {
				::dcool::core::terminate();
			}
			return result_;
		}

		private: static auto physicallyLivingSet_() noexcept -> IdSet& {
			static IdSet physicalLiving;
			return physicalLiving;
		}

		private: static auto logicallyLivingSet_() noexcept -> IdSet& {
			static IdSet logicallyLiving;
			return logicallyLiving;
		}

		public: static auto physicallyLivingCount() noexcept -> ::dcool::core::Size {
			return physicallyLivingSet_().count();
		}

		public: static auto logicallyLivingCount() noexcept -> ::dcool::core::Size {
			return logicallyLivingSet_().count();
		}

		public: auto physicalId() const noexcept -> Id {
			return this->m_physicalId_;
		}

		public: auto logicalId() const noexcept -> Id {
			this->checkValidity();
			return this->m_logicalId_;
		}

		public: auto value() const noexcept -> Id {
			this->checkValidity();
			return this->m_value_;
		}

		public: auto valid() const noexcept -> ::dcool::core::Boolean {
			return this->m_logicalId_ != invalidId;
		}

		private: void checkValidity() const noexcept {
			if (!(this->valid())) {
				::dcool::core::terminate();
			}
		}

		private: void invalidate_()& noexcept {
			this->checkValidity();
			this->m_logicalId_ = invalidId;
		}

		public: friend auto operator ==(TraceableTag const& left_, TraceableTag const& right_) noexcept -> ::dcool::core::Boolean {
			return left_.value() == right_.value();
		}
	};
}

#endif
