#ifndef DCOOL_COROUTINE_GENERATOR_HPP_INCLUDED_
#	define DCOOL_COROUTINE_GENERATOR_HPP_INCLUDED_ 1

#	include <dcool/coroutine/basic.hpp>

#	include <dcool/core.hpp>

#	include <coroutine>

namespace dcool::coroutine {
	template <typename YieldedT_, typename ConfigT_ = ::dcool::core::Empty<>> class Generator;

	class GeneratorExhausted: public ::dcool::core::OutOfRange {
		public: using Super_ = ::dcool::core::OutOfRange;

		public: using Super_::Super_;
	};

	namespace detail_ {
		template <typename YieldedT_, typename ConfigT_> class GeneratorPromise_ {
			public: using Self_ = GeneratorPromise_<YieldedT_, ConfigT_>;
			public: using Yielded = YieldedT_;
			public: using Config = ConfigT_;

			public: using Value_ = ::dcool::core::ReferenceRemovedType<Yielded>;
			public: using Handle_ = ::dcool::coroutine::Handle<Self_>;

			private: Value_* m_yieldedValue_;
			private: ::dcool::core::ExceptionPointer m_unhandledException_;

			public: constexpr auto initial_suspend() const noexcept -> ::dcool::coroutine::SuspendAlways {
				return ::dcool::coroutine::suspendAlways;
			}

			public: constexpr auto final_suspend() const noexcept -> ::dcool::coroutine::SuspendAlways {
				return ::dcool::coroutine::suspendAlways;
			}

			public: template <::dcool::core::FormOfSame<Yielded> ValueT__> constexpr auto yield_value(
				ValueT__&& value_
			) noexcept -> ::dcool::coroutine::SuspendAlways requires (::dcool::core::ConvertibleTo<ValueT__&&, Yielded>) {
				this->m_yieldedValue_ = ::dcool::core::addressOf(value_);
				return ::dcool::coroutine::suspendAlways;
			}

			public: constexpr auto get_return_object() noexcept -> ::dcool::coroutine::Generator<YieldedT_, ConfigT_> {
				return ::dcool::coroutine::Generator<YieldedT_, ConfigT_>(Handle_::from_promise(*this));
			}

			public: constexpr void return_void() const noexcept {
			}

			public: constexpr void unhandled_exception() noexcept {
				this->m_unhandledException_ = ::std::current_exception();
			}

			public: constexpr auto value() const noexcept -> Yielded {
				return static_cast<Yielded>(*(this->m_yieldedValue_));
			}

			public: constexpr auto exceptionCausingUnwrap() const noexcept -> ::dcool::core::ExceptionPointer {
				return this->m_unhandledException_;
			}
		};

		struct GeneratorEnd_ {
		};

		constexpr ::dcool::coroutine::detail_::GeneratorEnd_ generatorEnd_;

		template <typename YieldedT_, typename ConfigT_> struct GeneratorIterator_ {
			public: using Self_ = GeneratorIterator_<YieldedT_, ConfigT_>;
			public: using Yielded = YieldedT_;
			public: using Config = ConfigT_;

			public: using Promise_ = ::dcool::coroutine::detail_::GeneratorPromise_<Yielded, Config>;
			public: using End = ::dcool::coroutine::detail_::GeneratorEnd_;
			public: using Handle_ = Promise_::Handle_;

			public: using value_type = Promise_::Value_;
			public: using difference_type = ::dcool::core::Difference;
			public: using iterator_category = ::dcool::core::InputIteratorTag;

			private: Handle_ m_handle_;

			public: constexpr GeneratorIterator_() noexcept = default;

			public: constexpr explicit GeneratorIterator_(Handle_ handle_) noexcept: m_handle_(handle_) {
			}

			public: constexpr auto operator *() const noexcept -> Yielded {
				return this->m_handle_.promise().value();
			}

			public: constexpr auto operator ++() noexcept -> Self_& {
				this->m_handle_.resume();
				if (this->m_handle_.done()) {
					::dcool::core::rethrowIfValid(this->m_handle_.promise().exceptionCausingUnwrap());
				}
				return *this;
			}

			public: friend constexpr auto operator ==(
				Self_ const& left_, End const& right_
			) noexcept -> ::dcool::core::Boolean {
				return left_.m_handle_ == ::dcool::core::nullPointer || left_.m_handle_.done();
			}

			public: friend constexpr auto operator !=(
				Self_ const& left_, End const& right_
			) noexcept -> ::dcool::core::Boolean {
				return !(left_ == right_);
			}

			public: friend constexpr auto operator ==(
				End const& left_, Self_ const& right_
			) noexcept -> ::dcool::core::Boolean {
				return right_ == left_;
			}

			public: friend constexpr auto operator !=(
				End const& left_, Self_ const& right_
			) noexcept -> ::dcool::core::Boolean {
				return right_ != left_;
			}
		};
	}

	template <typename YieldedT_, typename ConfigT_> class Generator {
		private: using Self_ = Generator<YieldedT_, ConfigT_>;
		public: using Yielded = YieldedT_;
		public: using Config = ConfigT_;

		public: using Iterator = ::dcool::coroutine::detail_::GeneratorIterator_<Yielded, Config>;
		public: using EndIterator = Iterator::End;
		private: using Promise_ = Iterator::Promise_;
		private: using Handle_ = Iterator::Handle_;
		public: friend Promise_;

		private: Handle_ m_handle_ = ::dcool::core::nullPointer;

		public: using promise_type = Promise_;

		public: constexpr Generator() = default;
		public: Generator(Self_ const&) = delete;

		public: constexpr Generator(Self_&& other_) noexcept: m_handle_(other_.m_handle_) {
			other_.m_handle_ = ::dcool::core::nullPointer;
		}

		private: constexpr Generator(Handle_ handle_) noexcept: m_handle_(handle_) {
		}

		public: constexpr ~Generator() noexcept {
			if (this->m_handle_ != ::dcool::core::nullPointer) {
				this->m_handle_.destroy();
			}
		}

		public: auto operator =(Self_ const&) -> Self_& = delete;

		private: constexpr auto operator =(Self_&& other_) noexcept -> Self_& {
			::dcool::core::intelliSwap(this->m_handle_, other_.m_handle_);
		}

		public: constexpr auto begin() noexcept -> Iterator {
			if (this->m_handle_ == ::dcool::core::nullPointer) {
				return Iterator();
			}
			auto result_ = Iterator(this->m_handle_);
			++result_;
			return result_;
		}

		public: constexpr auto end() noexcept -> EndIterator {
			return ::dcool::coroutine::detail_::generatorEnd_;
		}

		public: constexpr auto tryGenerate() noexcept -> ::dcool::core::Optional<Yielded> {
			Iterator begin_ = this->begin();
			if (begin_ == this->end()) {
				return ::dcool::core::nullOptional;
			}
			return ::dcool::core::dereference(begin_);
		}

		public: constexpr auto generate() noexcept -> Yielded {
			auto result_ = this->begin();
			if (!(result_.valid())) {
				throw ::dcool::coroutine::GeneratorExhausted("Attempting to generate from an exhausted generator.");
			}
			return result_.access();
		}
	};
}

#endif
