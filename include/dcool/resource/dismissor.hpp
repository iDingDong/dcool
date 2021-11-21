#ifndef DCOOL_RESOURCE_DISMISSOR_HPP_INCLUDED_
#	define DCOOL_RESOURCE_DISMISSOR_HPP_INCLUDED_ 1

#	include <dcool/resource/pool.hpp>

#	include <dcool/core.hpp>

#	include <memory>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(dcool::resource, HasTypeDismissor, ExtractedDismissorType, Dismissor)

namespace dcool::resource {
	template <typename T_, typename ValueT_> concept PointerDismissorFor = ::dcool::core::Invocable<T_, ValueT_*>;

	template <typename T_, typename ValueT_> concept DismissorFor = ::dcool::core::Invocable<T_, ValueT_&>;

	template <typename ValueT_> class DefaultDestroyerFor {
		public: constexpr void operator ()(ValueT_& toDestroy_) const noexcept {
			delete ::dcool::core::addressOf(toDestroy_);
		}
	};

	template <typename ValueT_> constexpr ::dcool::resource::DefaultDestroyerFor<ValueT_> defaultDestroyerFor;

	class DefaultDestroyer {
		public: template <typename ValueT_> constexpr void operator ()(ValueT_& toDestroy_) const noexcept {
			::dcool::resource::defaultDestroyerFor<ValueT_>(toDestroy_);
		}
	};

	inline constexpr ::dcool::resource::DefaultDestroyer defaultDestoryer;

	template <typename ValueT_> class DefaultDestroyerExceptFor {
		template <typename ValueT__> constexpr void operator ()(ValueT__& toDestroy_) const noexcept {
			::dcool::resource::defaultDestoryer(toDestroy_);
		}

		public: constexpr void operator ()(ValueT_& toDestroy_) const noexcept {
		}
	};

	template <typename ValueT_> constexpr ::dcool::resource::DefaultDestroyerExceptFor<ValueT_> defaultDestroyerExceptFor;

	template <typename T_, typename... EngineTs_> concept SelfDestroyableWith = requires (T_ toDestroy_, EngineTs_&... engines_) {
		toDestroy_.destroySelfWith(engines_...);
	};

	template <typename... EngineTs_> class DefaultSelfDestoryInitiator {
		public: [[no_unique_address]] ::dcool::core::Tuple<EngineTs_...> engines;

		public: template <typename ValueT__> constexpr void operator ()(ValueT__& toDestroy_) const noexcept {
			auto selfDestroyCaller_ = [&toDestroy_] (auto&... engines_) {
				toDestroy_.destroySelfWith(engines_...);
			};
			::dcool::core::apply(selfDestroyCaller_, this->engines);
		}
	};

	template <typename T_, typename ValueT_> using DismissorType = ::dcool::resource::ExtractedDismissorType<
		T_, ::dcool::resource::DefaultDestroyer
	>;

	template <typename PoolT_> struct PoolDestroyer {
		public: PoolT_ pool;

		public: template <typename ValueT_> constexpr void operator ()(ValueT_& toDestroy_) noexcept {
			toDestroy_.~ValueT_();
			::dcool::resource::adaptedDeallocatePointerFor<ValueT_>(this->m_pool_, ::dcool::core::addressOf(toDestroy_));
		}
	};

	template <typename PrimaryDismissorT_, typename SecondaryDismissorT_> class MixedDismissor {
		private: using Self_ = MixedDismissor<PrimaryDismissorT_, SecondaryDismissorT_>;
		public: using PrimaryDismissor = PrimaryDismissorT_;
		public: using SecondaryDismissor = SecondaryDismissorT_;

		public: [[no_unique_address]] PrimaryDismissor primaryDismissor;
		public: [[no_unique_address]] SecondaryDismissor secondaryDismissor;
		private: [[no_unique_address]] ::dcool::core::StandardLayoutBreaker<Self_> m_standardLayoutBreaker_;

		public: template <
			::dcool::core::UnaryArgumentFor<PrimaryDismissor> ValueT__
		> constexpr void operator ()(ValueT__& toDestroy_) noexcept {
			this->primaryDismissor(toDestroy_);
		}

		public: template <typename ValueT__> constexpr void operator ()(ValueT__& toDestroy_) noexcept {
			this->secondaryDismissor(toDestroy_);
		}
	};

	template <typename DismissorT_> class StandardDeleter {
		public: DismissorT_ dismissor;

		public: template <typename ValueT__> constexpr void operator ()(ValueT__* pointer_) {
			this->dismissor(*pointer_);
		}
	};
}

#endif
