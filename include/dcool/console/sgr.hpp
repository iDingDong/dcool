#ifndef DCOOL_CONSOLE_SGR_HPP_INCLUDED_
#	define DCOOL_CONSOLE_SGR_HPP_INCLUDED_ 1

#	include <dcool/console/basic.hpp>

#	include <dcool/core.hpp>

#	include <iostream>
#	include <string>

#	define DCOOL_CONSOLE_SGR_RESET DCOOL_CONSOLE_CSI "m"

DCOOL_CORE_DEFINE_TYPE_MEMBER_DETECTOR(
	dcool::console::detail_, HasTypeColorConfigForSgr_, ExtractedColorConfigForSgrType_, ColorConfig
)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(dcool::console::detail_, HasValueWidthForColor_, extractedWidthForColorValue_, width)
DCOOL_CORE_DEFINE_CONSTANT_MEMBER_DETECTOR(
	dcool::console::detail_, HasValueBrightnessWidthForColor_, extractedBrightnessWidthForColorValue_, brightnessWidth
)

namespace dcool::console {
	namespace detail_ {
		template <typename ConfigT_> class ColorConfigAdaptor_ {
			public: using Config = ConfigT_;

			public: static constexpr ::dcool::core::Length width = ::dcool::console::detail_::extractedWidthForColorValue_<Config>(1);
			public: static constexpr ::dcool::core::Length brightnessWidth =
				::dcool::console::detail_::extractedBrightnessWidthForColorValue_<Config>(1)
			;
		};
	}

	template <typename T_> concept ColorConfig = requires {
		typename ::dcool::console::detail_::ColorConfigAdaptor_<T_>;
	};

	template <
		::dcool::console::ColorConfig ConfigT_
	> using ColorConfigAdaptor = ::dcool::console::detail_::ColorConfigAdaptor_<ConfigT_>;

	template <typename T_> concept ColorWithBrightnessConfig =
		::dcool::console::ColorConfig<T_> && ::dcool::console::ColorConfigAdaptor<T_>::brightnessWidth > 0
	;

	template <::dcool::console::ColorConfig ConfigT_ = ::dcool::core::Empty<>> struct Color {
		private: using Self_ = Color;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::console::ColorConfigAdaptor<Config>;
		public: static constexpr ::dcool::core::Length width = ConfigAdaptor_::width;
		public: static constexpr ::dcool::core::Length brightnessWidth = ConfigAdaptor_::brightnessWidth;

		private: using Underlying_ = ::dcool::core::LimbType<width>;

		public: Underlying_ red: width;
		public: Underlying_ green: width;
		public: Underlying_ blue: width;
	};

	template <::dcool::console::ColorWithBrightnessConfig ConfigT_> struct Color<ConfigT_> {
		private: using Self_ = Color;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::console::ColorConfigAdaptor<Config>;
		public: static constexpr ::dcool::core::Length width = ConfigAdaptor_::width;
		public: static constexpr ::dcool::core::Length brightnessWidth = ConfigAdaptor_::brightnessWidth;

		private: using Underlying_ = ::dcool::core::LimbType<width>;

		public: Underlying_ red: width;
		public: Underlying_ green: width;
		public: Underlying_ blue: width;
		public: Underlying_ brightness: brightnessWidth;
	};

	template <typename ConfigT_> constexpr auto operator +=(
		::dcool::console::Color<ConfigT_>& left_, ::dcool::console::Color<ConfigT_> const& right_
	) noexcept -> ::dcool::console::Color<ConfigT_>& {
		left_.red += right_.red;
		left_.green += right_.green;
		left_.blue += right_.blue;
		if constexpr (::dcool::console::Color<ConfigT_>::brightnessWidth > 0) {
			left_.brightness += right_.brightness;
		}
		return left_;
	}

	template <typename ConfigT_> constexpr auto operator +(
		::dcool::console::Color<ConfigT_> const& left_, ::dcool::console::Color<ConfigT_> const& right_
	) noexcept -> ::dcool::console::Color<ConfigT_> {
		::dcool::console::Color<ConfigT_> result_ = left_;
		result_ += right_;
		return result_;
	}

	namespace detail_ {
		template <
			::dcool::core::Length widthC_, ::dcool::core::Length brightnessWidthC_
		> struct ColorOfWidthConfig_ {
			public: static constexpr ::dcool::core::Length width = widthC_;
			public: static constexpr ::dcool::core::Length brightnessWidth = brightnessWidthC_;
		};
	}

	template <
		::dcool::core::Length widthC_, ::dcool::core::Length brightnessWidthC_
	> using ColorOfWidth = ::dcool::console::Color<::dcool::console::detail_::ColorOfWidthConfig_<widthC_, brightnessWidthC_>>;

	using Color8 = ::dcool::console::Color<>;
	using Color16 = ::dcool::console::ColorOfWidth<1, 1>;

	inline constexpr ::dcool::console::Color8 black8 = {
	};
	inline constexpr ::dcool::console::Color8 red8 = {
		.red = 1
	};
	inline constexpr ::dcool::console::Color8 green8 = {
		.green = 1
	};
	inline constexpr ::dcool::console::Color8 blue8 = {
		.blue = 1
	};
	inline constexpr ::dcool::console::Color8 yellow8 = red8 + green8;
	inline constexpr ::dcool::console::Color8 cyan8 = green8 + blue8;
	inline constexpr ::dcool::console::Color8 purple8 = blue8 + red8;
	inline constexpr ::dcool::console::Color8 white8 = red8 + cyan8;

	template <typename ColorT_> struct ColorPair {
		ColorT_ foreground;
		ColorT_ background;
	};

	using Color8Pair = ::dcool::console::ColorPair<::dcool::console::Color8>;
	using Color16Pair = ::dcool::console::ColorPair<::dcool::console::Color16>;

	namespace detail_ {
		template <typename ColorT_> constexpr auto singleBitColorToSgrParameter_(ColorT_ const& color_) -> char {
			return '0' + (color_.red | (color_.green << 1) | (color_.blue << 2));
		}
	}

	inline void applySgrReset(::std::ostream& stream_) {
		stream_ << DCOOL_CONSOLE_SGR_RESET;
	}

	inline void applyForegroundColorSgrParameter(::std::ostream& stream_, ::dcool::console::Color8 const& color_) {
		stream_ << '3' << ::dcool::console::detail_::singleBitColorToSgrParameter_(color_);
	}

	inline void applyForegroundColorSgrParameter(::std::ostream& stream_, ::dcool::console::Color16 const& color_) {
		stream_ << (color_.brightness ? '9' : '3') << ::dcool::console::detail_::singleBitColorToSgrParameter_(color_);
	}

	template <typename ColorT_> constexpr void applyForegroundColorSgr(::std::ostream& stream_, ColorT_ const& color_) {
		stream_ << DCOOL_CONSOLE_CSI;
		::dcool::console::applyForegroundColorSgrParameter(stream_, color_);
		stream_ << 'm';
	}

	inline void applyBackgroundColorSgrParameter(::std::ostream& stream_, ::dcool::console::Color8 const& color_) {
		stream_ << '4' << ::dcool::console::detail_::singleBitColorToSgrParameter_(color_);
	}

	inline void applyBackgroundColorSgrParameter(::std::ostream& stream_, ::dcool::console::Color16 const& color_) {
		stream_ << (color_.brightness ? "10" : "4") << ::dcool::console::detail_::singleBitColorToSgrParameter_(color_);
	}

	template <typename ColorT_> constexpr void applyBackgroundColorSgr(::std::ostream& stream_, ColorT_ const& color_) {
		stream_ << DCOOL_CONSOLE_CSI;
		::dcool::console::applyBackgroundColorSgrParameter(stream_, color_);
		stream_ << 'm';
	}

	template <typename ColorT_> constexpr void applyColorSgrParameter(
		::std::ostream& stream_, ::dcool::console::ColorPair<ColorT_> const& colorPair_
	) {
		::dcool::console::applyForegroundColorSgrParameter(stream_, colorPair_.foreground);
		stream_ << ';';
		::dcool::console::applyBackgroundColorSgrParameter(stream_, colorPair_.background);
	}

	template <typename ColorPairT_> constexpr void applyColorSgr(::std::ostream& stream_, ColorPairT_ const& color_) {
		stream_ << DCOOL_CONSOLE_CSI;
		::dcool::console::applyColorSgrParameter(stream_, color_);
		stream_ << 'm';
	}

	inline void applyUnderlineEnabledSgr(::std::ostream& stream_) {
		stream_ << '4';
	}

	inline void applyUnderlineDisabledSgr(::std::ostream& stream_) {
		stream_ << "24";
	}

	inline void applyStrikeEnabledSgr(::std::ostream& stream_) {
		stream_ << '9';
	}

	inline void applyStrikeDisabledSgr(::std::ostream& stream_) {
		stream_ << "29";
	}

	namespace detail_ {
		template <typename ConfigT_> class SgrConfigAdaptor_ {
			public: using Config = ConfigT_;

			public: using ColorConfig = ::dcool::console::detail_::ExtractedColorConfigForSgrType_<Config, ::dcool::core::Empty<>>;
		};
	}

	template <typename T_> concept SgrConfig = requires {
		typename ::dcool::console::detail_::SgrConfigAdaptor_<T_>;
	};

	template <
		::dcool::console::SgrConfig ConfigT_
	> using SgrConfigAdaptor = ::dcool::console::detail_::SgrConfigAdaptor_<ConfigT_>;

	template <::dcool::console::SgrConfig ConfigT_ = ::dcool::core::Empty<>> struct Sgr {
		private: using Self_ = Sgr;
		public: using Config = ConfigT_;

		private: using ConfigAdaptor_ = ::dcool::console::SgrConfigAdaptor<Config>;
		public: using ColorConfig = ConfigAdaptor_::ColorConfig;
		public: using Color = ::dcool::console::Color<ColorConfig>;

		public: enum class Intensity {
			normal,
			deEmphasized,
			emphasized
		};

		public: enum class BlinkFrequency {
			none,
			slow,
			rapid
		};

		public: ::dcool::console::ColorPair<Color> color;
		public: ::dcool::core::Boolean underlined;
		public: ::dcool::core::Boolean striked;
		public: Intensity intensity;
		public: BlinkFrequency blinkFrequency;

		public: void applyTo(::std::ostream& stream_) const {
			if (this->underlined) {
				::dcool::console::applyUnderlineEnabledSgr(stream_);
			} else {
				::dcool::console::applyUnderlineDisabledSgr(stream_);
			}
			stream_ << ';';
			if (this->striked) {
				::dcool::console::applyStrikeEnabledSgr(stream_);
			} else {
				::dcool::console::applyStrikeDisabledSgr(stream_);
			}
			stream_ << ';';
			stream_ << ::dcool::console::applyForegroundColorSgrParameter(this->color.foregroundColor);
			stream_ << ::dcool::console::applyBackgroundColorSgrParameter(this->color.backgroundColor);
			stream_ << ';';
			switch (this->intensity) {
				case Intensity::deEmphasized:
				stream_ << '2';
				break;
				case Intensity::emphasized:
				stream_ << '1';
				break;
				default:
				stream_ << "22";
				break;
			}
			stream_ << ';';
			switch (this->blinkFrequency) {
				case BlinkFrequency::slow:
				stream_ << '5';
				break;
				case BlinkFrequency::rapid:
				stream_ << '6';
				break;
				default:
				stream_ << "25";
				break;
			}
			stream_ << 'm';
		}
	};
}

#endif
