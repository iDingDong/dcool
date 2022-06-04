#ifndef DCOOL_BINARY_BIN_HPP_INCLUDED_
#	define DCOOL_BINARY_BIN_HPP_INCLUDED_

#	include <dcool/binary/endian.hpp>

#	include <dcool/core.hpp>
#	include <dcool/container.hpp>

namespace dcool::binary {
	namespace detail_ {
		template <typename ConfigT_> using BinConfigAdaptor_ = ::dcool::container::ListConfigAdaptor<ConfigT_, ::dcool::core::Byte>;
	}

	template <typename T_> concept BinConfig = requires {
		typename ::dcool::binary::detail_::BinConfigAdaptor_<T_>;
	};

	template <::dcool::binary::BinConfig ConfigT_> using BinConfigAdaptor = ::dcool::binary::detail_::BinConfigAdaptor_<ConfigT_>;

	template <::dcool::binary::BinConfig ConfigT_ = ::dcool::core::Empty<>> struct BinChassis {
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::binary::BinConfigAdaptor<Config>;
		public: using Unit = ::dcool::core::Byte;
		private: using Storage_ = ::dcool::container::ListChassis<Unit, Config>;
		public: using Engine = ConfigAdaptor_::Engine;
		public: using Length = Storage_::Length;

		private: Storage_ m_storage_;

		public: constexpr void initialize(Engine& engine_) noexcept {
			this->m_storage_.initialize(engine_);
		}

		public: constexpr void uninitialize(Engine& engine_) noexcept {
			this->m_storage_.uninitialize(engine_);
		}

		public: constexpr void cloneTo(Engine& engine_, Engine& otherEngine_, BinChassis& other_) const {
			this->m_storage_.cloneTo(engine_, otherEngine_, other_.m_storage_);
		}

		public: constexpr void relocateTo(Engine& engine_, Engine& otherEngine_, BinChassis& other_) noexcept {
			this->m_storage_.relocateTo(engine_, otherEngine_, other_.m_storage_);
		}

		public: template <
			typename ::dcool::core::Boolean engineWillBeSwappedC__ = false
		> constexpr void swapWith(Engine& engine_, Engine& otherEngine_, BinChassis& other_) noexcept {
			this->m_storage_.swapWith(engine_, otherEngine_, other_.m_storage_);
		}

		public: constexpr auto length(Engine& engine_) const noexcept -> Length {
			return this->m_storage_.length(engine_);
		}

		public: constexpr auto value(Engine& engine_) const noexcept -> void const* {
			return this->m_storage_.data(engine_);
		}

		public: constexpr auto value(Engine& engine_) noexcept -> void* {
			return this->m_storage_.data(engine_);
		}

		public: template <::dcool::core::Integral IntegerT__> constexpr void writeForNetwork(
			Engine& engine_, IntegerT__ const& value_
		) {
			this->write<::dcool::binary::Endian::network>(engine_, value_);
		}

		public: template <::dcool::binary::Endian targetEndianC__, ::dcool::core::Integral IntegerT__> constexpr void write(
			Engine& engine_, IntegerT__ const& value_
		) {
			::dcool::core::Index lengthBefore_ = this->length(engine_);
			this->pad(engine_, sizeof(IntegerT__));
			::dcool::binary::writeAs<targetEndianC__>(value_, ::dcool::core::stepByByte(this->value(engine_), lengthBefore_));
		}

		public: template <::dcool::core::TriviallyCopyable ObjectT__> constexpr void write(
			Engine& engine_, ObjectT__ const& object_
		) {
			this->write(engine_, ::dcool::core::addressOf(object_), sizeof(object_));
		}

		public: constexpr void write(Engine& engine_, void const* toWrite_, Length length_) {
			this->m_storage_.batchInsertN(engine_, this->m_storage_.end(engine_), reinterpret_cast<Unit const*>(toWrite_), length_);
		}

		public: constexpr void append(Engine& engine_, Engine& otherEngine_, BinChassis const& other_) {
			this->write(engine_, other_.value(otherEngine_), other_.length(otherEngine_));
		}

		public: constexpr void pad(Engine& engine_, Length size_) {
			this->m_storage_.uninitializedInsertN(engine_, this->m_storage_.end(engine_), size_);
		}

		public: constexpr void padZero(Engine& engine_, Length size_) {
			while (size_ > 0) {
				this->m_storage_.pushBack(engine_, static_cast<Unit>(0));
				--size_;
			}
		}

		public: constexpr void trimLeft(Engine& engine_, Length size_) {
			auto begin_ = this->m_storage_.begin(engine_);
			this->m_storage_.erase(engine_, begin_, begin_ + size_);
		}

		public: constexpr void trimRight(Engine& engine_, Length size_) {
			auto end_ = this->m_storage_.end(engine_);
			this->m_storage_.erase(engine_, end_ - size_, end_);
		}
	};

	template <::dcool::binary::BinConfig ConfigT_ = ::dcool::core::Empty<>> struct Bin {
		public: using Config = ConfigT_;

		public: using Chassis = ::dcool::binary::BinChassis<Config>;
		public: using Unit = Chassis::Unit;
		public: using Engine = Chassis::Engine;
		public: using Length = Chassis::Length;

		private: Chassis m_chassis_;
		private: [[no_unique_address]] mutable Engine m_engine_;

		public: constexpr Bin() noexcept {
			this->chassis().initialize(this->mutableEngine());
		}

		public: constexpr Bin(Bin const& other_): m_engine_(other_.mutableEngine()) {
			other_.chassis().cloneTo(other_.mutableEngine(), this->mutableEngine(), this->chassis());
		}

		public: constexpr Bin(Bin&& other_): m_engine_(other_.mutableEngine()) {
			other_.chassis().relocateTo(other_.mutableEngine(), this->mutableEngine(), this->chassis());
			other_.chassis().initialize(other_.mutableEngine());
		}

		public: constexpr ~Bin() noexcept {
			this->chassis().uninitialize(this->mutableEngine());
		}

		public: constexpr auto operator =(Bin const& other_) -> Bin& {
			Bin middleMan_(other_);
			this->swapWith(other_);
			return *this;
		}

		public: constexpr auto operator =(Bin&& other_) noexcept -> Bin& {
			this->swapWith(other_);
			return *this;
		}

		public: constexpr void swapWith(Bin& other_) noexcept {
			this->m_chassis_.swapWith<true>(this->mutableEngine(), other_.mutableEngine(), other_);
			::dcool::core::intelliSwap(this->mutableEngine(), other_.mutableEngine());
		}

		public: constexpr auto chassis() const noexcept -> Chassis const& {
			return this->m_chassis_;
		}

		protected: constexpr auto chassis() noexcept -> Chassis& {
			return this->m_chassis_;
		}

		public: constexpr auto engine() const noexcept -> Engine const& {
			return this->m_engine_;
		}

		protected: constexpr auto mutableEngine() const noexcept -> Engine& {
			return this->m_engine_;
		}

		public: constexpr auto length() const noexcept -> Length {
			return this->chassis().length(this->mutableEngine());
		}

		public: constexpr auto value() const noexcept -> void const* {
			return this->chassis().value(this->mutableEngine());
		}

		public: constexpr auto value() noexcept -> void* {
			return this->chassis().value(this->mutableEngine());
		}

		public: template <::dcool::core::Integral IntegerT__> constexpr void writeForNetwork(IntegerT__ const& value_) {
			this->chassis().writeForNetwork(this->mutableEngine(), value_);
		}

		public: template <
			::dcool::binary::Endian targetEndianC__, ::dcool::core::Integral IntegerT__
		> constexpr void write(IntegerT__ const& value_) {
			this->chassis().template write<targetEndianC__>(this->mutableEngine(), value_);
		}

		public: template <::dcool::core::TriviallyCopyable ObjectT__> constexpr void write(ObjectT__ const& object_) {
			this->chassis().write(this->mutableEngine(), object_);
		}

		public: constexpr void write(void const* toWrite_, Length length_) {
			this->chassis().write(this->mutableEngine(), toWrite_, length_);
		}

		public: constexpr void append(Bin const& other_) {
			this->chassis().append(this->mutableEngine(), other_.mutableEngine(), other_.chassis());
		}

		public: constexpr void pad(Length size_) {
			this->chassis().pad(this->mutableEngine(), size_);
		}

		public: constexpr void padZero(Length size_) {
			this->chassis().padZero(this->mutableEngine(), size_);
		}

		public: constexpr void trimLeft(Length size_) {
			this->chassis().trimLeft(this->mutableEngine(), size_);
		}

		public: constexpr void trimRight(Length size_) {
			this->chassis().trimRight(this->mutableEngine(), size_);
		}
	};
}

#endif
