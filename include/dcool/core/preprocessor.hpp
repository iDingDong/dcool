#ifndef DCOOL_CORE_PREPROCESSOR_HPP_INCLUDED_
#	define DCOOL_CORE_PREPROCESSOR_HPP_INCLUDED_ 1

#	include <dcool/core/select.hpp>

#	define DCOOL_CORE_PARAMETER(...) __VA_ARGS__

#	define DCOOL_CORE_COUNT(...) DCOOL_CORE_SELECT_63( \
	__VA_ARGS__ __VA_OPT__(,) \
	63, \
	62, \
	61, \
	60, \
	59, \
	58, \
	57, \
	56, \
	55, \
	54, \
	53, \
	52, \
	51, \
	50, \
	49, \
	48, \
	47, \
	46, \
	45, \
	44, \
	43, \
	42, \
	41, \
	40, \
	39, \
	38, \
	37, \
	36, \
	35, \
	34, \
	33, \
	32, \
	31, \
	30, \
	29, \
	28, \
	27, \
	26, \
	25, \
	24, \
	23, \
	22, \
	21, \
	20, \
	19, \
	18, \
	17, \
	16, \
	15, \
	14, \
	13, \
	12, \
	11, \
	10, \
	9, \
	8, \
	7, \
	6, \
	5, \
	4, \
	3, \
	2, \
	1, \
	0 \
)

#endif
