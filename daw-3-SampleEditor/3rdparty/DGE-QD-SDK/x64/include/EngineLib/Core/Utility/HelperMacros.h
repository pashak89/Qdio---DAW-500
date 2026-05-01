#pragma once

#include <Core/BaseTypes/BaseTypes.h>

namespace DGE
{
	namespace Core
	{
		namespace Utility
		{
/* NOTE: In these macros, "1" means true, and "0" means false. */
#define EXPAND(x) x

#define _GLUE(X, Y) X##Y
#define GLUE(X, Y) _GLUE(X, Y)

/* Returns the 100th argument. */
#define _ARG_100(_, _100, _99, _98, _97, _96, _95, _94, _93, _92, _91, _90, _89, _88, _87, _86,  \
				 _85, _84, _83, _82, _81, _80, _79, _78, _77, _76, _75, _74, _73, _72, _71, _70, \
				 _69, _68, _67, _66, _65, _64, _63, _62, _61, _60, _59, _58, _57, _56, _55, _54, \
				 _53, _52, _51, _50, _49, _48, _47, _46, _45, _44, _43, _42, _41, _40, _39, _38, \
				 _37, _36, _35, _34, _33, _32, _31, _30, _29, _28, _27, _26, _25, _24, _23, _22, \
				 _21, _20, _19, _18, _17, _16, _15, _14, _13, _12, _11, _10, _9, _8, _7, _6, _5, \
				 _4, _3, _2, X_, ...)                                                            \
	X_

/* Returns whether __VA_ARGS__ has a comma (up to 100 arguments). */
#define HAS_COMMA(...)                                                                             \
	EXPAND(_ARG_100(__VA_ARGS__, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, \
					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  \
					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,  \
					1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0))

/* Produces a comma if followed by a parenthesis. */
#define _TRIGGER_PARENTHESIS_(...) ,
#define _PASTE5(_0, _1, _2, _3, _4) _0##_1##_2##_3##_4
#define _IS_EMPTY_CASE_0001 ,
/* Returns true if inputs expand to (false, false, false, true) */
#define _IS_EMPTY(_0, _1, _2, _3) HAS_COMMA(_PASTE5(_IS_EMPTY_CASE_, _0, _1, _2, _3))
/* Returns whether __VA_ARGS__ is empty. */
#define IS_EMPTY(...)                                                                             \
	_IS_EMPTY(						  /* Testing for an argument with a comma                     \
										 e.g. "ARG1, ARG2", "ARG1, ...", or "," */                \
			  HAS_COMMA(__VA_ARGS__), /* Testing for an argument around parenthesis               \
										 e.g. "(ARG1)", "(...)", or "()" */                       \
			  HAS_COMMA(                                                                          \
				  _TRIGGER_PARENTHESIS_                                                           \
					  __VA_ARGS__), /* Testing for a macro as an argument, which will             \
									   expand the parenthesis, possibly generating a comma. */    \
			  HAS_COMMA(                                                                          \
				  __VA_ARGS__(/*empty*/)), /* If all previous checks are false, __VA_ARGS__ does  \
											  not generate a comma by itself, nor with            \
											  _TRIGGER_PARENTHESIS_ behind it, nor with () after  \
											  it. Therefore, "_TRIGGER_PARENTHESIS_ __VA_ARGS__   \
											  ()" only generates a comma if __VA_ARGS__ is empty. \
											  So, this tests for an empty __VA_ARGS__ (given the  \
											  previous conditionals are false). */                \
			  HAS_COMMA(_TRIGGER_PARENTHESIS_ __VA_ARGS__(/*empty*/)))

#define _VAR_COUNT_EMPTY_1(...) 0
#define _VAR_COUNT_EMPTY_0(...)                                                                   \
	EXPAND(_ARG_100(__VA_ARGS__, 100, 99, 98, 97, 96, 95, 94, 93, 92, 91, 90, 89, 88, 87, 86, 85, \
					84, 83, 82, 81, 80, 79, 78, 77, 76, 75, 74, 73, 72, 71, 70, 69, 68, 67, 66,   \
					65, 64, 63, 62, 61, 60, 59, 58, 57, 56, 55, 54, 53, 52, 51, 50, 49, 48, 47,   \
					46, 45, 44, 43, 42, 41, 40, 39, 38, 37, 36, 35, 34, 33, 32, 31, 30, 29, 28,   \
					27, 26, 25, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 12, 11, 10, 9, 8, \
					7, 6, 5, 4, 3, 2, 1))
#define VAR_COUNT(...) GLUE(_VAR_COUNT_EMPTY_, IS_EMPTY(__VA_ARGS__))(__VA_ARGS__)

#define FIRSTARG(X, ...) (X)
#define RESTARGS(X, ...) (__VA_ARGS__)
#define FOREACH(MACRO, LIST) FOREACH_(VAR_COUNT LIST, MACRO, LIST)
#define FOREACH_(N, M, LIST) FOREACH__(N, M, LIST)
#define FOREACH__(N, M, LIST) FOREACH_##N(M, LIST)
#define FOREACH_1(M, LIST) M LIST
#define FOREACH_2(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_1(M, RESTARGS LIST)
#define FOREACH_3(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_2(M, RESTARGS LIST)
#define FOREACH_4(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_3(M, RESTARGS LIST)
#define FOREACH_5(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_4(M, RESTARGS LIST)
#define FOREACH_6(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_5(M, RESTARGS LIST)
#define FOREACH_7(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_6(M, RESTARGS LIST)
#define FOREACH_8(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_7(M, RESTARGS LIST)
#define FOREACH_9(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_8(M, RESTARGS LIST)
#define FOREACH_10(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_9(M, RESTARGS LIST)
#define FOREACH_11(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_10(M, RESTARGS LIST)
#define FOREACH_12(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_11(M, RESTARGS LIST)
#define FOREACH_13(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_12(M, RESTARGS LIST)
#define FOREACH_14(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_13(M, RESTARGS LIST)
#define FOREACH_15(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_14(M, RESTARGS LIST)
#define FOREACH_16(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_15(M, RESTARGS LIST)
#define FOREACH_17(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_16(M, RESTARGS LIST)
#define FOREACH_18(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_17(M, RESTARGS LIST)
#define FOREACH_19(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_18(M, RESTARGS LIST)
#define FOREACH_20(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_19(M, RESTARGS LIST)
#define FOREACH_21(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_20(M, RESTARGS LIST)
#define FOREACH_22(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_21(M, RESTARGS LIST)
#define FOREACH_23(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_22(M, RESTARGS LIST)
#define FOREACH_24(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_23(M, RESTARGS LIST)
#define FOREACH_25(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_24(M, RESTARGS LIST)
#define FOREACH_26(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_25(M, RESTARGS LIST)
#define FOREACH_27(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_26(M, RESTARGS LIST)
#define FOREACH_28(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_27(M, RESTARGS LIST)
#define FOREACH_29(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_28(M, RESTARGS LIST)
#define FOREACH_30(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_29(M, RESTARGS LIST)
#define FOREACH_31(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_30(M, RESTARGS LIST)
#define FOREACH_32(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_31(M, RESTARGS LIST)
#define FOREACH_33(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_32(M, RESTARGS LIST)
#define FOREACH_34(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_33(M, RESTARGS LIST)
#define FOREACH_35(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_34(M, RESTARGS LIST)
#define FOREACH_36(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_35(M, RESTARGS LIST)
#define FOREACH_37(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_36(M, RESTARGS LIST)
#define FOREACH_38(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_37(M, RESTARGS LIST)
#define FOREACH_39(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_38(M, RESTARGS LIST)
#define FOREACH_40(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_39(M, RESTARGS LIST)
#define FOREACH_41(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_40(M, RESTARGS LIST)
#define FOREACH_42(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_41(M, RESTARGS LIST)
#define FOREACH_43(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_42(M, RESTARGS LIST)
#define FOREACH_44(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_43(M, RESTARGS LIST)
#define FOREACH_45(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_44(M, RESTARGS LIST)
#define FOREACH_46(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_45(M, RESTARGS LIST)
#define FOREACH_47(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_46(M, RESTARGS LIST)
#define FOREACH_48(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_47(M, RESTARGS LIST)
#define FOREACH_49(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_48(M, RESTARGS LIST)
#define FOREACH_50(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_49(M, RESTARGS LIST)
#define FOREACH_51(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_50(M, RESTARGS LIST)
#define FOREACH_52(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_51(M, RESTARGS LIST)
#define FOREACH_53(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_52(M, RESTARGS LIST)
#define FOREACH_54(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_53(M, RESTARGS LIST)
#define FOREACH_55(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_54(M, RESTARGS LIST)
#define FOREACH_56(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_55(M, RESTARGS LIST)
#define FOREACH_57(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_56(M, RESTARGS LIST)
#define FOREACH_58(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_57(M, RESTARGS LIST)
#define FOREACH_59(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_58(M, RESTARGS LIST)
#define FOREACH_60(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_59(M, RESTARGS LIST)
#define FOREACH_61(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_60(M, RESTARGS LIST)
#define FOREACH_62(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_61(M, RESTARGS LIST)
#define FOREACH_63(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_62(M, RESTARGS LIST)
#define FOREACH_64(M, LIST) EXPAND(M FIRSTARG LIST) FOREACH_63(M, RESTARGS LIST)

#define STRINGIFY(X) #X,
#define FOREACH_STRINGIFY(...) FOREACH(STRINGIFY, (__VA_ARGS__))

#if DGE_Platform != DGE_Windows_Platform
#define D_OPT(...) __VA_OPT__(, ) __VA_ARGS__
#else
#define D_OPT(...) , __VA_ARGS__
#endif
		} // namespace Utility
	}	  // namespace Core
} // namespace DGE
