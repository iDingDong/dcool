#ifndef DCOOL_UTILITY_TYPE_ERASER_HPP_INCLUDED_
#	define DCOOL_UTILITY_TYPE_ERASER_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>
#	include <dcool/resource.hpp>

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::utility::detail_, HasTypeInferfaceForEraser_, ExtractedInterfaceTypeForEraser_, Interface
)

namespace dcool::utility {
	namespace detail_ {
		template <typename ConfigT_> class EraserConfigAdaptor_ {
			private: using Self_ = EraserConfigAdaptor_<ConfigT_>;
			public: using Config = ConfigT_;

			public: using Pool = ::dcool::resource::PoolType<Config>;
			public: using Interface = ::dcool::utility::detail_::ExtractedInterfaceTypeForEraser_<
				Config, ::dcool::core::Empty<Config>
			>;
			public: static constexpr ::dcool::core::Boolean extended = ::dcool::core::isPolymorphic<Interface>;
		};

		template <typename T_> concept ExtendedEraserConfig_ = ::dcool::utility::detail_::EraserConfigAdaptor_<T_>::extended;
	}

	template <typename T_> concept EraserConfig = requires {
		typename ::dcool::utility::detail_::EraserConfigAdaptor_<T_>;
	};

	template <typename ConfigT_> using EraserConfigAdaptor = ::dcool::utility::detail_::EraserConfigAdaptor_<ConfigT_>;

	template <
		typename ConfigT_ = ::dcool::core::Empty<>
	> class EraserHolderBase: public ::dcool::utility::EraserConfigAdaptor<ConfigT_>::Interface {
		private: using Self_ = EraserHolderBase<ConfigT_>;
		private: using Super_ = ::dcool::utility::EraserConfigAdaptor<ConfigT_>::Interface;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::utility::EraserConfigAdaptor<ConfigT_>;
		public: using Pool = ConfigAdaptor_::Pool;

		public: virtual ~EraserHolderBase() noexcept = default;
		public: virtual auto valueTypeInfo() const noexcept -> ::dcool::core::TypeInfo const& = 0;
		public: virtual auto valueAddress() const noexcept -> void const* = 0;
		public: virtual auto valueAddress() noexcept -> void* = 0;
		public: virtual auto allocateForClone(Pool& pool_) -> void* = 0;
		public: virtual void inPlaceCopyConstructTo(void* position_) = 0;
		public: virtual void inPlaceMoveConstructTo(void* position_) = 0;
		public: virtual void destructSelf() noexcept = 0;
		public: virtual void destroySelf(Pool& pool_) noexcept = 0;
	};

	template <
		typename ValueT_, typename ConfigT_ = ::dcool::core::Empty<>
	> class EraserHolder final: public ::dcool::utility::EraserHolderBase<ConfigT_> {
		private: using Self_ = EraserHolder<ValueT_, ConfigT_>;
		private: using Super_ = ::dcool::utility::EraserHolderBase<ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		public: using typename Super_::Pool;

		public: Value value;

		public: auto valueTypeInfo() const noexcept -> ::dcool::core::TypeInfo const& override {
			return typeid(Value);
		}

		public: auto valueAddress() const noexcept -> void const* override {
			return ::dcool::core::addressOf(this->value);
		}

		public: auto valueAddress() noexcept -> void* override {
			return ::dcool::core::addressOf(this->value);
		}

		public: auto allocateForClone(Pool& pool_) -> void* {
			return ::dcool::resource::adaptedAllocatePointerFor<Self_>(pool_);
		}

		public: void inPlaceCopyConstructTo(void* position_) override {
			new (static_cast<Self_*>(position_)) Self_(*this);
		}

		public: void inPlaceMoveConstructTo(void* position_) override {
			new (static_cast<Self_*>(position_)) Self_(::dcool::core::move(*this));
		}

		public: void destructSelf() noexcept override {
			::dcool::core::destruct(*this);
		}

		public: void destroySelf(Pool& pool_) noexcept override {
			::dcool::resource::destroyPointerByPoolFor(pool_, this);
		}
	};

	template <
		typename ValueT_, ::dcool::utility::detail_::ExtendedEraserConfig_ ConfigT_
	> class EraserHolder<ValueT_, ConfigT_>: public ::dcool::utility::EraserHolderBase<ConfigT_> {
		private: using Self_ = EraserHolder<ValueT_, ConfigT_>;
		private: using Super_ = ::dcool::utility::EraserHolderBase<ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		public: Value value;

		public: auto valueTypeInfo() const noexcept -> ::dcool::core::TypeInfo const& override {
			return typeid(Value);
		}

		public: auto valueAddress() const noexcept -> void const* override {
			return ::dcool::core::addressOf(this->value);
		}

		public: auto valueAddress() noexcept -> void* override {
			return ::dcool::core::addressOf(this->value);
		}
	};
}

#endif
