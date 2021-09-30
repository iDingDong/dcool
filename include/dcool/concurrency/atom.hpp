#ifndef DCOOL_CONCURRENCY_ATOM_HPP_INCLUDED_
#	define DCOOL_CONCURRENCY_ATOM_HPP_INCLUDED_ 1

#	include <dcool/core.hpp>

#	include <atomic>

DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueAtomicForAtom_, extractedAtomicForAtomValue_, atomic
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::concurrency::detail_, HasValueStampWidthForAtom_, extractedStampWidthForAtomValue_, stampWidth
)

namespace dcool::concurrency {
	namespace detail_ {
		template <typename ConfigT_, typename ValueT_> class AtomConfigAdaptor_ {
			private: using Self_ = AtomConfigAdaptor_<ConfigT_, ValueT_>;
			public: using Config = ConfigT_;
			public: using Value = ValueT_;
			static_assert(::dcool::core::TriviallyCopyable<Value>);

			public: static constexpr ::dcool::core::Boolean atomic =
				::dcool::concurrency::detail_::extractedAtomicForAtomValue_<Config>(true)
			;
			public: static constexpr ::dcool::core::Boolean stampWidth =
				atomic ?
					::dcool::concurrency::detail_::extractedStampWidthForAtomValue_<Config>(::dcool::core::UnsignedMaxInteger(0)) :
					0
			;
		};
	}

	template <typename T_, typename ValueT_> concept AtomConfig = requires {
		typename ::dcool::concurrency::detail_::AtomConfigAdaptor_<T_, ValueT_>;
	};

	template <typename ConfigT_, typename ValueT_> requires ::dcool::concurrency::AtomConfig<
		ConfigT_, ValueT_
	> using AtomConfigAdaptor = ::dcool::concurrency::detail_::AtomConfigAdaptor_<ConfigT_, ValueT_>;

	template <typename ValueT_, ::dcool::concurrency::AtomConfig<ValueT_> ConfigT_ = ::dcool::core::Empty<>> struct Atom {
		private: using Self_ = Atom<ValueT_, ConfigT_>;
		public: using Value = ValueT_;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::concurrency::AtomConfigAdaptor<Config, Value>;
		public: static constexpr ::dcool::core::Length atomic = ConfigAdaptor_::atomic;
		public: static constexpr ::dcool::core::Length stampWidth = ConfigAdaptor_::stampWidth;

		public: using Stamp = ::dcool::core::UnsignedInteger<stampWidth>;

		public: struct StampedValue {
			[[no_unique_address]] ValueT_ value;
			[[no_unique_address]] Stamp stamp;
		};

		private: using Underlying_ = ::dcool::core::ConditionalType<atomic, ::std::atomic<StampedValue>, StampedValue>;

		private: Underlying_ m_underlying_;

		public: constexpr Atom() noexcept = default;
		public: Atom(Self_ const&) = delete;

		public: constexpr Atom(Value desired_) noexcept: m_underlying_ {
			.value = desired_,
			.stamp = Stamp(0)
		} {
		}

		public: auto operator =(Self_ const&)& -> Self_& = delete;

		public: constexpr auto operator =(Value const& desired_)& noexcept -> Self_& {
			this->store(desired_);
			return *this;
		}

		public: constexpr auto loadStamped(
			::std::memory_order order_ = ::std::memory_order::seq_cst
		) const noexcept -> StampedValue {
			StampedValue result_;
			if constexpr (atomic) {
				result_ = this->m_underlying_.load(order_);
			} else {
				result_ = this->m_underlying_;
			}
			return result_;
		}

		public: constexpr auto load(::std::memory_order order_ = ::std::memory_order::seq_cst) const noexcept -> Value {
			return this->loadStamped(order_).value;
		}

		public: constexpr auto exchange(
			Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		)& noexcept -> Value {
			StampedValue oldValue_ = this->m_underlying_.load(::std::memory_order::relaxed);
			if constexpr (atomic) {
				StampedValue desiredValue_ = {
					.value = desired_,
					.stamp = oldValue_.stamp + 1
				};
				while (!(this->m_underlying_.compare_exchange_weak(oldValue_, desiredValue_, order_))) {
					desiredValue_.stamp = oldValue_.stamp + 1;
				}
			} else {
				this->m_underlying_.value = desired_;
			}
			return oldValue_.value;
		}

		public: constexpr void store(Value const& desired_, ::std::memory_order order_ = ::std::memory_order::seq_cst)& noexcept {
			static_cast<void>(this->exchange(desired_, order_));
		}

		private: template <typename TransformerT__> constexpr auto transformValue_(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> StampedValue {
			StampedValue oldValue_ = this->m_underlying_.load(::std::memory_order::relaxed);
			if constexpr (atomic) {
				StampedValue desiredValue_ = {
					.value = ::dcool::core::invoke(transformer_, ::dcool::core::constantize(oldValue_.value)),
					.stamp = oldValue_.stamp + 1
				};
				while (!(this->m_underlying_.compare_exchange_weak(oldValue_, desiredValue_, order_))) {
					desiredValue_.value = ::dcool::core::invoke(transformer_, ::dcool::core::constantize(oldValue_.value));
					desiredValue_.stamp = oldValue_.stamp + 1;
				}
			} else {
				this->m_underlying_.value = ::dcool::core::invoke(transformer_, ::dcool::core::constantize(oldValue_.value));
			}
			return oldValue_;
		}

		private: template <typename TransformerT__> constexpr auto transformOrLoad_(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> StampedValue {
			StampedValue oldValue_ = this->m_underlying_.load(::std::memory_order::relaxed);
			auto newValue_ = ::dcool::core::invoke(transformer_, ::dcool::core::constantize(oldValue_.value));
			if (newValue_ == ::dcool::core::nullOptional) {
				return oldValue_;
			}
			if constexpr (atomic) {
				StampedValue desiredValue_ = {
					.value = newValue_.access(),
					.stamp = oldValue_.stamp + 1
				};
				while (!(this->m_underlying_.compare_exchange_weak(oldValue_, desiredValue_, order_))) {
					newValue_ = ::dcool::core::invoke(transformer_, ::dcool::core::constantize(oldValue_.value));
					if (newValue_ == ::dcool::core::nullOptional) {
						return oldValue_;
					}
					desiredValue_.value = ::dcool::core::invoke(transformer_, ::dcool::core::constantize(oldValue_.value));
					desiredValue_.stamp = oldValue_.stamp + 1;
				}
			} else {
				this->m_underlying_.value = newValue_.access();
			}
			return oldValue_;
		}

		public: template <typename TransformerT__> constexpr auto transform(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> Value {
			return this->transformValue_(::dcool::core::forward<TransformerT__>(transformer_), order_).value;
		}

		public: template <typename TransformerT__> constexpr auto transformOrLoad(
			TransformerT__&& transformer_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> Value {
			return this->transformOrLoad_(::dcool::core::forward<TransformerT__>(transformer_), order_).value;
		}

		public: template <typename OtherOperandT__> constexpr auto fetchAdd(
			OtherOperandT__ const& other_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> Value {
			return this->transform(
				[&other_](Value const& value) noexcept -> Value {
					return value + other_;
				},
				order_
			);
		}

		public: template <typename OtherOperandT__> constexpr auto fetchSubtract(
			OtherOperandT__ const& other_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> Value {
			return this->transform(
				[&other_](Value const& value) noexcept -> Value {
					return value - other_;
				},
				order_
			);
		}

		public: template <typename OtherOperandT__> constexpr auto fetchMultiply(
			OtherOperandT__ const& other_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> Value {
			return this->transform(
				[&other_](Value const& value) noexcept -> Value {
					return value * other_;
				},
				order_
			);
		}

		public: template <typename OtherOperandT__> constexpr auto fetchDivide(
			OtherOperandT__ const& other_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> Value {
			return this->transform(
				[&other_](Value const& value) noexcept -> Value {
					return value / other_;
				},
				order_
			);
		}

		public: template <typename OtherOperandT__> constexpr auto fetchModulo(
			OtherOperandT__ const& other_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> Value {
			return this->transform(
				[&other_](Value const& value) noexcept -> Value {
					return value % other_;
				},
				order_
			);
		}

		public: template <typename OtherOperandT__> constexpr auto fetchBitwiseAnd(
			OtherOperandT__ const& other_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> Value {
			return this->transform(
				[&other_](Value const& value) noexcept -> Value {
					return value & other_;
				},
				order_
			);
		}

		public: template <typename OtherOperandT__> constexpr auto fetchBitwiseOr(
			OtherOperandT__ const& other_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> Value {
			return this->transform(
				[&other_](Value const& value) noexcept -> Value {
					return value | other_;
				},
				order_
			);
		}

		public: template <typename OtherOperandT__> constexpr auto fetchBitwiseXor(
			OtherOperandT__ const& other_, ::std::memory_order order_ = ::std::memory_order::seq_cst
		) noexcept -> Value {
			return this->transform(
				[&other_](Value const& value) noexcept -> Value {
					return value ^ other_;
				},
				order_
			);
		}
	};
}

#endif
