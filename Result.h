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
		return {ResultInfo{TYPE_NAME, DESCRIPTION, ERROR_MESSAGE}};                                                    \
	}

#define RESULT_PARAM_OPT Result& Result = *(Result*)&ResultOk
#define RESULT_PARAM	 Result& Result

/**
 * @brief Structure that holds data information.
 * 
*/
struct ResultInfo
{
	constexpr ResultInfo(std::string_view Name, std::string_view Description, std::string_view ErrorMessage)
		: Name{Name}, Description{Description}, ErrorMessage{ErrorMessage}
	{
	}

	const std::string_view Name, Description, ErrorMessage;
};

using Result = ResultInfo (*)();

constexpr ResultInfo ResultOk()
{
	return { "Ok", "Result ok.", "No error." };
}
constexpr ResultInfo ResultFail()
{
	return { "Fail", "Generic fail.", "Failed. An unknow error has occurred." };
}
constexpr ResultInfo ResultNullPtr()
{
	return { "NullPtr", "Null pointer.", "Invalid pointer, it's null." };
}

#endif
