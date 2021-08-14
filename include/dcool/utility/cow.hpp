#ifndef DCOOL_UTILITY_COW_HPP_INCLUDED_
#	define DCOOL_UTILITY_COW_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>
#	include <dcool/resource.hpp>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::utility::detail_, HasValueAtomicallyCountedForCow_, extractedActomicallyCountedValue_, atomicallyCounted
)

namespace dcool::utility {
	namespace detail_ {
		template <typename ConfigT_, typename ValueT_> class CowConfigAdaptor_ {
			private: using Self_ = CowConfigAdaptor_<ConfigT_, ValueT_>;
			public: using Config = ConfigT_;

			public: static constexpr ::dcool::core::Boolean atomicallyCounted =
				::dcool::utility::detail_::extractedActomicallyCountedValue_<Config>(false)
			;

			private: struct DefaultEngine_ {
				private: using Self_ = DefaultEngine_;

				public: friend auto operator ==(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
				public: friend auto operator !=(Self_ const&, Self_ const&) noexcept -> ::dcool::core::Boolean = default;
			};

			public: using Engine = ::dcool::core::ExtractedEngineType<Config, DefaultEngine_>;
		};
	}

	template <typename T_, typename ValueT_> concept CowConfig = requires {
		typename ::dcool::utility::detail_::CowConfigAdaptor_<T_, ValueT_>;
	};

	template <
		typename ConfigT_, typename ValueT_
	> requires ::dcool::utility::CowConfig<ConfigT_, ValueT_> using CowConfigAdaptor =
		::dcool::utility::detail_::CowConfigAdaptor_<ConfigT_, ValueT_>
	;

	template <typename ValueT_, ::dcool::utility::CowConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>> struct CowChassis {
		private: using Self_ = CowChassis<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::utility::CowConfigAdaptor<Config, Value>;
		public: using Engine = ConfigAdaptor_::Engine;
		public: static constexpr ::dcool::core::Boolean atomicallyCounted = ConfigAdaptor_::atomicallyCounted;

		private: struct SharedAgentConfig_ {
			static constexpr ::dcool::core::Boolean atomicallyCounted = atomicallyCounted;
			static constexpr ::dcool::core::UnsignedMaxInteger maxWeakReferenceCount = 0;
		};

		private: using Shared_ = ::dcool::resource::CompactSharedStrongPointer<ValueT_, SharedAgentConfig_>;

		private: Shared_ m_value_;

		public: template <typename... ArgumentTs__> constexpr void initialize(Engine& engine_) {
		}

		public: template <typename... ArgumentTs__> constexpr void initialize(
			Engine& engine_, ::dcool::core::InPlaceTag inPlace_, ArgumentTs__&&... parameters_
		) {
			this->m_value_ = Shared_(inPlace_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: constexpr void cloneTo(Engine& engine_, Engine& otherEngine_, Self_& other_) const noexcept {
			other_.m_value_ = this->m_value_;
		}

		public: constexpr void relocateTo(Engine& engine_, Engine& otherEngine_, Self_& other_) noexcept {
			other_.m_value_ = ::dcool::core::move(this->m_value_);
		}

		public: constexpr void swapWith(Engine& engine_, Engine& otherEngine_, Self_& other_) noexcept {
			::dcool::core::intelliSwap(this->m_value_, other_.m_value_);
		}

		public: constexpr void uninitialize(Engine& engine_) noexcept {
			this->m_value_.reset();
		}

		public: constexpr auto detach(Engine& engine_) {
			if (this->m_value_.strongReferenceCount() == 1) {
				return;
			}
			Self_ middleMan_;
			middleMan_.initialize(engine_, ::dcool::core::inPlace, this->access(engine_));
			this->uninitialize(engine_);
			middleMan_.relocateTo(engine_, engine_, *this);
		}

		public: constexpr auto access(Engine& engine_) const noexcept -> Value const& {
			return *(this->m_value_);
		}

		public: constexpr auto access(Engine& engine_) noexcept -> Value& {
			return *(this->m_value_);
		}

		public: constexpr auto value(Engine& engine_) const noexcept -> Value const& {
			return this->access(engine_);
		}

		public: constexpr auto mutableValue(Engine& engine_) noexcept -> Value& {
			this->detach(engine_);
			return *(this->m_value_);
		}
	};

	template <typename ValueT_, ::dcool::utility::CowConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>> struct Cow {
		private: using Self_ = Cow<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		public: using Chassis = ::dcool::utility::CowChassis<Value, Config>;
		public: using Engine = Chassis::Engine;

		private: Chassis m_chassis_;
		private: [[no_unique_address]] mutable Engine m_engine_;

		public: template <typename... ArgumentTs__> constexpr explicit Cow(
			::dcool::core::InPlaceTag inPlace_, ArgumentTs__&&... parameters_
		) {
			this->chassis().initialize(this->mutableEngine_(), inPlace_, ::dcool::core::forward<ArgumentTs__>(parameters_)...);
		}

		public: constexpr Cow(Self_ const& other_) noexcept: m_engine_(other_.m_engine_) {
			other_.chassis().cloneTo(other_.mutableEngine_(), this->mutableEngine_(), this->chassis());
		}

		public: constexpr Cow(Self_&& other_) noexcept: m_engine_(other_.m_engine_)  {
			other_.chassis().cloneTo(other_.mutableEngine_(), this->mutableEngine_(), this->chassis());
			other_.chassis().initialize(other_.mutableEngine_());
		}

		public: constexpr ~Cow() noexcept {
			this->chassis().uninitialize(this->mutableEngine_());
		}

		public: constexpr auto operator =(Self_ const& other_) noexcept -> Self_& {
			Self_ middleMan_(other_);
			this->swapWith(middleMan_);
			return *this;
		}

		public: constexpr auto operator =(Self_&& other_) noexcept -> Self_& {
			Self_ middleMan_(::dcool::core::move(other_));
			this->swapWith(middleMan_);
			return *this;
		}

		public: constexpr void swapWith(Self_& other_) noexcept {
			this->chassis().swapWith(this->mutableEngine_(), other_.mutableEngine_(), other_);
		}

		private: constexpr auto chassis() const noexcept -> Chassis const& {
			return this->m_chassis_;
		}

		private: constexpr auto chassis() noexcept -> Chassis& {
			return this->m_chassis_;
		}

		public: constexpr auto engine() const noexcept -> Engine const& {
			return this->m_engine_;
		}

		private: constexpr auto mutableEngine_() const noexcept -> Engine& {
			return this->m_engine_;
		}

		public: constexpr auto detach() {
			this->chassis().detach(this->mutableEngine_());
		}

		public: constexpr auto access() const noexcept -> Value const& {
			return this->chassis().access(this->mutableEngine_());
		}

		public: constexpr auto access() noexcept -> Value& {
			return this->chassis().access(this->mutableEngine_());
		}

		public: constexpr auto value() const noexcept -> Value const& {
			return this->chassis().value(this->mutableEngine_());
		}

		public: constexpr auto mutableValue() noexcept -> Value& {
			return this->chassis().mutableValue(this->mutableEngine_());
		}
	};
}

#endif
