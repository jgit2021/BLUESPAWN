#pragma once

#include "os.h"
#ifdef BLUESPAWN_WINDOWS
#include <Windows.h>
#endif


#include <string>
#include <vector>

/**
* Gets the Shannon Entropy of a string
* 
* @param in The string of which to calculate the entropy
*
* @return A double storing the Shannon Entropy of the string
*/
double GetShannonEntropy(const bstring& in);

/**
 * Converts a wide-string to a UTF-8 encoded string
 *
 * @param in The widestring to convert
 *
 * @return The given wide-string converted to a UTF-8 encoded string
 */
std::string WidestringToString(const bstring& in);

/**
 * Converts a UTF-8 encoded string to a wide-string
 *
 * @param in The string to convert
 *
 * @return The string converted to a wide-string
 */
bstring StringToWidestring(const std::string& in);

/**
 * Expands all enviroment strings present in the input
 * 
 * @param in The string to expand all environment strings for
 *
 * @return The string with all environment strings expanded.
 */
bstring ExpandEnvStringsW(const bstring& in);

/**
 * Expands all enviroment strings present in the input
 *
 * @param in The string to expand all environment strings for
 *
 * @return The string with all environment strings expanded.
 */
std::string ExpandEnvStringsA(const std::string& in);

/**
* Zero pads a number to the specified width, useful for zeropadding dates
*
* @param value The number to be zeropadded
* @param length The width to pad to. Defaults to 2
*
* @return A wstring with the number zeropadded to the requested width
*/
bstring ToWstringPad(DWORD value, size_t length = 2);

/**
 * Convert a string or wstring to uppercase. Note that the only
 * allowable template classes are std::string and bstring.
 *
 * @param in The string/wstring to convert to uppercase.
 *
 * @return A copy of the given string, converted to uppercase.
 */
template<class T>
T ToUpperCase(const T& in);
#define ToUpperCaseA ToUpperCase<std::string>
#define ToUpperCaseW ToUpperCase<bstring>

/**
 * Convert a string or wstring to lowercase. Note that the only
 * allowable template classes are std::string and bstring.
 *
 * @param in The string/wstring to convert to lowercase.
 *
 * @return A copy of the given string, converted to lowercase.
 */
template<class T>
T ToLowerCase(const T& in);
#define ToLowerCaseA ToLowerCase<std::string>
#define ToLowerCaseW ToLowerCase<bstring>

/**
 * Compares two strings, ignoring case. Note that the only allowable
 * template classes are std::string and bstring.
 *
 * @param in1 The first string to compare
 * @param in2 The second string to compare.
 *
 * @return true if the two strings are equal; false otherwise.
 */
template<class T>
bool CompareIgnoreCase(const T& in1, const T& in2);
#define CompareIgnoreCaseA CompareIgnoreCase<std::string>
#define CompareIgnoreCaseW CompareIgnoreCase<bstring>

/**
 * Replaces all instances of a substring found inside of a string
 *
 * @param string The string to modify
 * @param search The substring to be replaced
 * @param replacement The replacement for the substring
 *
 * @return A string with the replacements applied.
 */
template<class T>
T StringReplace(const T& string, const T& search, const T& replacement);
#define StringReplaceA StringReplace<std::string>
#define StringReplaceW StringReplace<bstring>
#define CompareIgnoreCaseW CompareIgnoreCase<bstring>

/**
 * Split a string into substrings based on a delimter. This currently
 * does not support regular expressions.
 *
 * @param in The string/wstring to split.
 * @param delimiter The substring to split on. 
 *
 * @return A vector containing the components of `in` split by `delimiter`
 */
template<class T>
std::vector<std::basic_string<T>> SplitString(const std::basic_string<T>& in, const std::basic_string<T>& delimiter);
#define SplitStringA SplitString<CHAR>
#define SplitStringW SplitString<WCHAR>
