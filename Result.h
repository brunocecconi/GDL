
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
