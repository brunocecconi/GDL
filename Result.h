/**
 * MIT License
 *
 * Copyright(c) 2023 Bruno Cecconi
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files(the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions :
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#ifndef BC_RESULT_H
#define BC_RESULT_H

#include <cstdint>
#include <string_view>

#define RESULT_DEFINE(SYMBOL_NAME, TYPE_NAME, DESCRIPTION, ERROR_MESSAGE)                                              \
	constexpr ResultInfo Result##SYMBOL_NAME()                                                                         \
	{                                                                                                                  \
		return {TYPE_NAME, DESCRIPTION, ERROR_MESSAGE};                                                                \
	}

#define RESULT_DEFINE_TAG(SYMBOL_NAME) RESULT_DEFINE(SYMBOL_NAME, nullptr, nullptr, nullptr)
#define RESULT_PARAM_OPT			   result_t& Result = *(result_t*)&ResultOk
#define RESULT_UNUSED_PARAM_OPT		   result_t& = *(result_t*)&ResultOk
#define RESULT_PARAM				   result_t& Result
#define RESULT_UNUSED_PARAM			   result_t&
#define RESULT_COMPOSED_PARAM_OPT	   result_composed_t& Result = *(result_t*)&ResultOk
#define RESULT_COMPOSED_PARAM		   result_composed_t& Result

// Used to check and set result and return from function as well. COND is the condition needed to fail.
#define RESULT_CHECK(COND, RESULT, ...)                                                                                \
	if (COND && (Result = RESULT))                                                                                     \
	return __VA_ARGS__

#define RESULT_RETURN_CHECK(COND, RESULT)	\
	if(COND) return RESULT

/**
 * @brief Structure that holds data information.
 *
 */
struct ResultInfo
{
	constexpr ResultInfo(const char* Name, const char* Description, const char* ErrorMessage)
		: Name{Name}, Description{Description}, ErrorMessage{ErrorMessage}
	{
	}

	const char *Name, *Description, *ErrorMessage;
};

/**
 * @brief Result type.
 *
 */
using result_t = ResultInfo (*)();

/**
 * @brief Result ensure argument type.
 *
 * Composes function result to check its arguments or anything you need more information about.
 * Ex: if you have a function that has as parameters two pointers, result_composed_t TestPointer(void* P1, void* P2){}.
 * You can with this way, check pointers and use ErrorNullPtr passing as second array position, if it fails, as
 * ResultArg0 or ResultArg1. I have made eight tag functions, if you need more than eight parameters to a function you
 * probably want to create a structure instead. And the user can check using 'if(R.Result == ResultNullPtr && R.Other ==
 * ResultArg0)' indicates that you are checking if function has returned a nullptr error for the first argument.
 *
 */
struct result_composed_t
{
	result_composed_t() = default;
	result_composed_t(const result_t Result) : Result{Result}
	{
	}
	result_composed_t(const result_t Result, const result_t Other) : Result{Result}, Other{Other}
	{
	}
	result_composed_t(const result_composed_t&)				   = default;
	result_composed_t(result_composed_t&&) noexcept			   = default;
	result_composed_t& operator=(const result_composed_t&)	   = default;
	result_composed_t& operator=(result_composed_t&&) noexcept = default;
	~result_composed_t()									   = default;
	operator result_t()
	{
		return Result;
	}
	result_t Result{};
	result_t Other{};
};

RESULT_DEFINE(Ok, "Ok", "Result ok.", "No error.");
RESULT_DEFINE(ErrorFail, "ErrorFail", "Generic error fail.", "Failed. An unknown error has occurred.");
RESULT_DEFINE(ErrorNullPtr, "ErrorNullPtr", "Error null pointer.", "Invalid pointer, it's null.");
RESULT_DEFINE(ErrorEmptyContainer, "ErrorEmptyContainer",
			  "Error empty container. Indicates that a data container isn't expected to be empty.",
			  "Failed. Invalid container data, it is empty.");
RESULT_DEFINE(
	ErrorNotEnoughMemory, "ErrorNotEnoughMemory",
	"Error not enough memory. Indicates that a structure doesn't have memory necessary for its internal logic.",
	"Failed. Target structure doesn't have enough memory for system internal logic.");
RESULT_DEFINE(ErrorOutOfBounds, "ErrorOutOfBounds",
			  "Error out of bounds. Indicates that a memory location was handled out of the buffer's memory range.",
			  "Failed. A memory location was handled out of the buffer's memory range.");

RESULT_DEFINE_TAG(Arg0);
RESULT_DEFINE_TAG(Arg1);
RESULT_DEFINE_TAG(Arg2);
RESULT_DEFINE_TAG(Arg3);
RESULT_DEFINE_TAG(Arg4);
RESULT_DEFINE_TAG(Arg5);
RESULT_DEFINE_TAG(Arg6);
RESULT_DEFINE_TAG(Arg7);

#endif
