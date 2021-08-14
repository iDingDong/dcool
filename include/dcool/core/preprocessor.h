#ifndef DCOOL_CORE_PREPROCESSOR_H_INCLUDED_
#	define DCOOL_CORE_PREPROCESSOR_H_INCLUDED_ 1

#	define DCOOL_CORE_IS_ONE_OF_2(value, first, second) ((value) == (first) || (value) == (second))
#	define DCOOL_CORE_IS_ONE_OF_3(value, first, second, third) \
	(DCOOL_CORE_IS_ONE_OF_2((value), (first), (second)) || (value) == (third))

#	define DCOOL_CORE_PARAMETER(...) __VA_ARGS__

#endif
