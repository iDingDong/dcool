#include <dcool/core.hpp>
#include <dcool/test.hpp>

DCOOL_TEST_CASE(dcoolCore, integerBasics) {
	static_assert(::dcool::core::Trivial<dcool::core::UnsignedInteger<0>>);
	static_assert(::dcool::core::Trivial<dcool::core::UnsignedInteger<1>>);
	static_assert(
		::dcool::core::Trivial<dcool::core::UnsignedInteger<dcool::core::widthOfInteger<dcool::core::UnsignedMaxInteger>>>
	);
	static_assert(
		::dcool::core::Trivial<dcool::core::UnsignedInteger<dcool::core::widthOfInteger<dcool::core::UnsignedMaxInteger> + 1>>
	);
	static_assert(
		::dcool::core::Trivial<dcool::core::UnsignedInteger<dcool::core::widthOfInteger<dcool::core::UnsignedMaxInteger> * 2>>
	);
	static_assert(
		::dcool::core::Trivial<dcool::core::UnsignedInteger<dcool::core::widthOfInteger<dcool::core::UnsignedMaxInteger> * 2 + 1>>
	);
	static_assert(::dcool::core::Trivial<dcool::core::SignedInteger<1>>);
	static_assert(
		::dcool::core::Trivial<dcool::core::SignedInteger<dcool::core::widthOfInteger<dcool::core::SignedMaxInteger>>>
	);
	static_assert(
		::dcool::core::Trivial<dcool::core::SignedInteger<dcool::core::widthOfInteger<dcool::core::SignedMaxInteger> + 1>>
	);
	static_assert(
		::dcool::core::Trivial<dcool::core::SignedInteger<dcool::core::widthOfInteger<dcool::core::SignedMaxInteger> * 2>>
	);
	static_assert(
		::dcool::core::Trivial<dcool::core::SignedInteger<dcool::core::widthOfInteger<dcool::core::SignedMaxInteger> * 2 + 1>>
	);
	using namespace dcool::core::extendedIntegerLiteral;
	auto ui1 = 0_UI1;
	DCOOL_TEST_EXPECT(ui1 == 0);
	++ui1;
	DCOOL_TEST_EXPECT(ui1 == 1);
	++ui1;
	DCOOL_TEST_EXPECT(ui1 == 0);
	ui1 += 1_UI1;
	DCOOL_TEST_EXPECT(ui1 == 1);
	ui1 += 1_UI1;
	DCOOL_TEST_EXPECT(ui1 == 0);
	ui1 -= 1_UI1;
	DCOOL_TEST_EXPECT(ui1 == 1);
	auto ui6 = 0b011111_UI6;
	DCOOL_TEST_EXPECT(ui6 + ui6 == 0b111110);
	DCOOL_TEST_EXPECT(ui6 << 1_UI3 == 0b111110);
	ui6 *= 2;
	DCOOL_TEST_EXPECT(ui6 == 0b111110);
	DCOOL_TEST_EXPECT(ui6 >> 1_UI3 == 0b011111);
	++ui6;
	DCOOL_TEST_EXPECT(ui6 == ui6.max);
	ui6 += ui1;
	DCOOL_TEST_EXPECT(ui6 == 0);
	DCOOL_TEST_EXPECT(ui6 < ui1);
	auto si2 = -1_SI2;
	DCOOL_TEST_EXPECT(si2 == -1);
	si2 = -si2;
	DCOOL_TEST_EXPECT(si2 == 1);
	DCOOL_TEST_EXPECT(~si2 == -2);
}

DCOOL_TEST_CASE(dcoolCore, hugeIntegerShift) {
	using namespace dcool::core::extendedIntegerLiteral;
	auto ui264 = 0x78'1F2F3F4F'2F3F4F1F'3F4F1F2F'4F1F2F3F'10203040'20304010'30401020'40102030_UI;
	DCOOL_TEST_EXPECT((ui264 << 4U) == 0x81'F2F3F4F2'F3F4F1F3'F4F1F2F4'F1F2F3F1'02030402'03040103'04010204'01020300_UI);
	DCOOL_TEST_EXPECT((ui264 << 128U) == 0x3F'10203040'20304010'30401020'40102030'00000000'00000000'00000000'00000000_UI);
	DCOOL_TEST_EXPECT((ui264 << 132U) == 0xF1'02030402'03040103'04010204'01020300'00000000'00000000'00000000'00000000_UI);
	auto si264 = -0x78'1F2F3F4F'2F3F4F1F'3F4F1F2F'4F1F2F3F'10203040'20304010'30401020'40102030_SI;
	DCOOL_TEST_EXPECT(si264 < 0);
	DCOOL_TEST_EXPECT((si264 >> 1U) == -0x3C'0F979FA7'979FA78F'9FA78F97'A78F979F'88101820'10182008'18200810'20081018_SI);
	DCOOL_TEST_EXPECT((si264 >> 128U) == -0x78'1F2F3F4F'2F3F4F1F'3F4F1F2F'4F1F2F3F_SI);
	DCOOL_TEST_EXPECT((si264 >> 129U) == -0x3C'0F979FA7'979FA78F'9FA78F97'A78F979F_SI);
}
