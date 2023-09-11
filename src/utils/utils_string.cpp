# include "webserv.hpp"

bool isAlnumStr(const std::string& input)
{
	for (std::string::const_iterator it = input.begin(); it != input.end(); it++)
		if (!isalnum(*it))
			return false;
	return true;
}

bool isAlnumStrPlus(const std::string& input, const std::string& allowedChars)
{
	for (std::string::const_iterator it = input.begin(); it != input.end(); it++)
		if (!isalnum(*it) && allowedChars.find(*it) == std::string::npos)
			return false;
	return true;
}

bool isSameNoCase(const std::string& str_a, const std::string& str_b)
{
	return strToLower(str_a) == strToLower(str_b);
}

bool isStringInVec(const std::string& string, const std::vector<std::string>& vector)
{
	if (std::find(vector.begin(), vector.end(), string) != vector.end())
		return true;
	return false;
}

std::string strToLower(const std::string& str)
{
	std::string	lowerStr = str;

	for (std::string::iterator it = lowerStr.begin(); it != lowerStr.end(); it++)
		*it = tolower(*it);
	return lowerStr;
}

std::string trimWhitespace(std::string& input)
{
	size_t	start = input.find_first_not_of(WHITESPACE);
	size_t	end = input.find_last_not_of(WHITESPACE);
	if (start == std::string::npos)
		input = "";
	else
		input = input.substr(start, end - start + 1);
	return input;
}

/*
Returns a substring from the beginning of the passed string ref to
the beginning of the first occurence of the 2nd argument.
Deletes the substring and the 2nd argument from the passed string.
*/
std::string splitEraseStr(std::string& input, const std::string& targetString)
{
	std::string	element;
	size_t 		len;

	len = input.find(targetString);
	if (len == std::string::npos)
	{
		element = input;
		input.erase();
	}
	else
	{
		element = input.substr(0, len);
		input.erase(0, len + targetString.size());
	}
	trimWhitespace(element);
	return element;
}

/*
Returns a substring from the beginning of the passed string ref to
the first occurence of any char from the 2nd argument.
Deletes the substring from the passed string, but not the encountered
delimiting char.
Calls trimWhitespace function on both the remainder of the string ref
and the returned element.
*/
std::string splitEraseTrimChars(std::string& input, const std::string& targetChars)
{
	std::string element;
	size_t 		len;

	len = input.find_first_of(targetChars);
	if (len == std::string::npos)
	{
		element = input;
		input.erase();
	}
	else
	{
		element = input.substr(0, len);
		input.erase(0, len);
		trimWhitespace(input);
	}
	trimWhitespace(element);
	return element;
}

/*
Returns a string vector.
First argument is the string ref to operate on.
2nd argument is a string containing the characters of which any single one delimits
the final strings.
The 3rd argument denotes the end of the region to be processed.
*/
std::vector<std::string> splitEraseStrVec(std::string& input, const std::string& targetChars, const std::string& endOfParsing)
{
	std::vector<std::string>	stringVector;
	std::string					parseRegion, element;
	
	parseRegion = splitEraseTrimChars(input, endOfParsing);
	input.erase(0, endOfParsing.size());
	while (!parseRegion.empty())
	{
		element = splitEraseTrimChars(parseRegion, targetChars);
		stringVector.push_back(element);
	}
	return stringVector;
}

/*
Splits a string ref and returns the first instruction it contains,
defined as all characters from the string ref's start until
 - the next semicolon
 or
 - within the next pair of curly braces.
 Deletes the instruction from the string ref.
*/
std::string getInstruction(std::string& inputStr)
{
	std::string	instruction;
	size_t		len_semicolon;

	// Check whether semicolon delimits the instruction
	len_semicolon = inputStr.find(";");
	if (len_semicolon < inputStr.find("{"))
	{
		instruction = inputStr.substr(0, len_semicolon);
		inputStr.erase(0, len_semicolon + 1);
		trimWhitespace(instruction);
		return instruction;
	}

	// Curly braces must now delimit the instruction
	size_t	i;
	int		bracesDepth;

	i = inputStr.find("{");
	if (i == std::string::npos || inputStr.find("}") < i)
		throw std::runtime_error(E_INVALIDENDTOKEN + inputStr + '\n');
	bracesDepth = 1;
	while (inputStr[++i] && bracesDepth > 0 && bracesDepth < 3)
	{
		if (inputStr[i] == '{')
			bracesDepth++;
		else if (inputStr[i] == '}')
			bracesDepth--;
	}
	if (bracesDepth != 0)
		throw std::runtime_error(E_INVALIDBRACE + inputStr + '\n');
	instruction = inputStr.substr(0, i);
	inputStr.erase(0, i);
	instruction.replace(instruction.find("{"), 1, " ");
	instruction.replace(instruction.find_last_of("}"), 1, " ");
	trimWhitespace(instruction);
	return instruction;
}

/*
Operates on a string ref that contains key-value pairs.
1st arg is the string ref to operate on.
2nd is end of key (e.g. "=").
3rd is end of value and therefore also end of key-value pair (e.g. ";").
4th is end of region to parse in 1st arg (e.g. "\r\n\r\n"). Pass "" to parse entire 1st arg.
Erases the parsed region from the string ref.
*/
std::map<std::string, std::string> parseStrMap(std::string& input, const std::string& endOfKey, const std::string& endOfValue, const std::string& endOfMap)
{
	std::map<std::string, std::string> 	stringMap;
	std::string 						key, value;

	if (endOfMap.empty())
	{
		while (!input.empty())
		{
			key = splitEraseStr(input, endOfKey);
			value = splitEraseStr(input, endOfValue);
			stringMap.insert(std::make_pair(strToLower(key), value));
		}
	}
	else // same function as before, except for the if()
	{
		while (!input.empty())
		{
			if (input.find(endOfMap) == 0)
			{
				input = input.substr(endOfMap.size());
				return stringMap;
			}
			key = splitEraseStr(input, endOfKey);
			value = splitEraseStr(input, endOfValue);
			stringMap.insert(std::make_pair(strToLower(key), value));
		}
	}

	return stringMap;
}
