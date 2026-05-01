/* Copyright (C) 2025, Abbas Aliakbari - All Rights Reserved
 * SPDX-License-Identifier: MIT
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 *
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR
 * ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
 * CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
 * WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * You should have received a copy of the MIT license with
 * this file. If not, please write to: a.aliakbari91@gmail.com
 *
 * Written by Abbas Aliakbari
 *
 * @section DESCRIPTION
 *
 * String class is using for storing a string type in the engine
 */
#pragma once
#include <Core/BaseTypes/BaseTypes.h>
#include <vector>

namespace DGE
{
	namespace Core
	{
		namespace BaseType
		{
#define ToDChar(p) (const DChar*) DString(p)

#define ToWideChar(p) (const wchar_t*) WString(p)
#define WST (WString)
#define ST (DString)

			class WString;

			/// \brief For Ansi String with wide operators
			class DGE_CoreAPI DString
			{
				std::string m_stdStr;

			public:
				/// \brief Constructor convert const char* into String
				///
				/// \details Convert const char* into String that get length of input array of char
				/// from strlen(const char *)
				DString(const char* ch);

				/// \brief Constructor convert const char into String
				///
				/// \note if isHex is true then store ch in hexadecimal format. if fixedLength is
				/// greater than zero then try to store ch with specified fixedLength and will be
				/// used fixedFiller character for filling the result string
				DString(char ch, bool isHex = false, int fixedLength = 0, char fixedFiller = '0');

				/// \brief Constructor convert const reference String into String
				DString(const DString& str);

				/// \brief Constructor convert const reference DByteArray into String
				DString(const DByteArray& byteArray);

				/// \brief Constructor convert const rvalue String into String
				DString(DString&& str) noexcept;

				/// \brief This is default constructor that create an empty String
				DString();

				/// \brief Constructor convert std::string into String
				DString(string str);
#if DGE_Platform == DGE_Windows_Platform
				/// \brief Constructor convert LPSTR into String
				inline DString(LPSTR str)
				{
					if (str == nullptr || strlen(str) == 0)
					{
						m_stdStr = "";
						return;
					}
					m_stdStr = str;
				}
#endif
				/// \brief Constructor convert int into String
				///
				/// \note if isHex is true then store ch in hexadecimal format. if fixedLength is
				/// greater than zero then try to store ch with specified fixedLength and will be
				/// used fixedFiller character for filling the result string
				DString(uint16 number, bool isHex = false, int fixedLength = 0,
						char fixedFiller = '0');

				DString(uint32 number, bool isHex = false, int fixedLength = 0,
						char fixedFiller = '0');

				DString(uint64 number, bool isHex = false, int fixedLength = 0,
						char fixedFiller = '0');

				DString(int16 number, bool isHex = false, int fixedLength = 0,
						char fixedFiller = '0');

				DString(int32 number, bool isHex = false, int fixedLength = 0,
						char fixedFiller = '0');

				DString(int64 number, bool isHex = false, int fixedLength = 0,
						char fixedFiller = '0');

				DString(float number);
				DString(double number);
				DString(bool flag);

				/// \brief Constructor convert std::wstring into String
				DString(wstring str);

				/// \brief Convert stored string to char* type
				///
				/// \return Return an array of char that it's converted string
				const char* toChar() const;
				//				inline String operator+(String str)
				//				{
				//					string s1 = this->Data;
				//					s1 += str.Data;
				//					String st = s1;
				//					return (st);
				//				}

				/// \brief Replace content of this class with given str parameter
				///
				/// \return Return a reference to this class that is modified.
				DString& operator=(const DString& str);

				/// \brief Replace content of this class with given str parameter
				///
				/// \return Return a reference to this class that is modified.
				DString& operator=(DString&& str);

				/// \brief Copy content of vector of char into this String variable
				///
				/// \return Return a reference to this class that is modified.
				DString& operator=(std::vector<char>& str);

				/// \brief Concatenate a String and an int together and return result of
				/// concatenation of String and int16.
				///
				/// \return Return result of concatenation of String and int16.
				DString operator+(int16 data) const;

				/// \brief Concatenate a String and an int32 together and return result of
				/// concatenation of String and int16.
				///
				/// \return Return result of concatenation of String and int32.
				DString operator+(int32 data) const;

				/// \brief Concatenate a String and an int64 together and return result of
				/// concatenation of String and int16.
				///
				/// \return Return result of concatenation of String and int64.
				DString operator+(int64 data) const;

				/// \brief Concatenate a String and an unsigned int16 together and return result of
				/// concatenation of String and unsigned int.
				///
				/// \return Return result of concatenation of String and unsigned int16.
				DString operator+(uint16 data) const;

				/// \brief Concatenate a String and an unsigned short together and return result of
				/// concatenation of String and unsigned short.
				///
				/// \return Return result of concatenation of String and unsigned short.
				DString operator+(uint32 data) const;

				/// \brief Concatenate a String and an unsigned long together and return result of
				/// concatenation of String and unsigned long.
				///
				/// \return Return result of concatenation of String and unsigned long.
				DString operator+(uint64 data) const;

				/// \brief Concatenate a String and a double together and return result of
				/// concatenation of String and double.
				///
				/// \return Return result of concatenation of String and double.
				DString operator+(double data) const;

				/// \brief Concatenate two Strings together and store result of
				/// concatenation of two Strings in this object.
				///
				/// \return Return result of concatenation of two Strings .
				DString operator+=(const DString& str);

				/// \brief Concatenate a String and an int together and store result of
				/// concatenation of String and int in this object.
				///
				/// \return Return result of concatenation of String and int.
				DString operator+=(char data);

				/// \brief Concatenate a String and an int together and store result of
				/// concatenation of String and int in this object.
				///
				/// \return Return result of concatenation of String and int.
				DString operator+=(int data);

				/// \brief Concatenate a String and an unsigned int together and store result of
				/// concatenation of String and unsigned int in this object.
				///
				/// \return Return result of concatenation of String and unsigned int.
				DString operator+=(unsigned int data);

				/// \brief Concatenate a String and an unsigned short together and store result of
				/// concatenation of String and unsigned short in this object.
				///
				/// \return Return result of concatenation of String and unsigned short.
				DString operator+=(unsigned short data);

				/// \brief Concatenate a String and an unsigned long together and store result of
				/// concatenation of String and unsigned long in this object.
				///
				/// \return Return result of concatenation of String and unsigned long.
				DString operator+=(unsigned long data);

				/// \brief Concatenate a String and a double together and store result of
				/// concatenation of String and double in this object.
				///
				/// \return Return result of concatenation of String and double.
				DString operator+=(double data);

				/// \brief Compare content of two strings that return true if left side string is
				/// smaller else return false.
				///
				/// \return Return true if left side string is smaller
				/// else return false.
				bool operator<(const DString& str1) const;

				/// \brief Compare content of two strings that return true if left side string is
				/// greater else return false.
				///
				/// \return Return true if left side string is greater
				/// else return false.
				bool operator>(const DString& str1) const;

				/// \brief Compare content of two strings that return true if left side string isn't
				/// equal with right side else return false.
				///
				/// \return Return true if left side string isn't equal with right side
				/// else return false.
				bool operator!=(const DString& str) const;

				/// \brief Compare content of two strings that return true if left side string isn't
				/// equal with right side else return false.
				///
				/// \return Return true if left side string is greater
				/// else return false.
				bool operator==(const DString& str) const;

				/// \brief Compare content of two strings that return true if left side string isn't
				/// equal with right side else return false.
				///
				/// \return Return true if left side string is greater
				/// else return false.
				bool operator==(const char* str) const;

				/// \brief Resize string to size of content equal to size parameter
				void resize(uint32 size);

				/// \brief Convert String to std::wstring
				///
				/// \return Return result of converting String to std::wstring
				WString getWString();

				/// \brief Convert content of String to std::string
				///
				/// \return Return result of converting content of String to \c std::string
				std::string toStdString() const;

				/// \brief Convert content of String to \c int
				///
				/// \param isHex Indicate whether content of String is in hex format
				///
				/// \return Return result of converting content of String to \c int
				int toInt(bool isHex = false) const;

				/// \brief Convert content of String to \c double
				///
				/// \return Return result of converting content of String to \c double
				double toDouble() const;

				/// \brief Convert content of String to \c boolean
				///
				/// \return Return result of converting content of String to \c boolean
				bool toBool() const;

				/// \brief Convert content of String to \c std::string
				///
				/// \return Return result of converting content of String to \c std::string
				operator string() const;

				/// \brief Convert content of String to const array of \c char
				///
				/// \return Return result of converting content of String to const array of \c char
				explicit operator const char*() const;

				/// \brief Convert content of String to array of char
				///
				/// \return Return result of converting content of String to array of \c char
				explicit operator char*() const;

				/// \brief Convert content of String to array of bytes
				///
				/// \return Return result of converting content of String to array of \c bytes
				operator const DByteArray() const;

				/// \brief Compare content of two strings that return true if left side string is
				/// smaller else return false.
				///
				/// \return Return true if left side string is smaller
				/// else return false.
				bool operator()(DString&& n1, DString&& n2) const;

				/// \brief Given reference of two string and compare content of two strings that
				/// return true if left side string is smaller else return false.
				///
				/// \return Return true if left side string is smaller
				/// else return false.
				bool operator()(const DString& n1, const DString& n2) const;

				/// \brief Get a reference to the character at specified position \c index.
				///
				/// \return Return a reference to the character at specified position \c index.
				char& operator[](uint32 index);

				/// \brief Determined is exist compareString in this String
				///
				/// \param compareString a string for checking existence in this String
				///
				/// \return Return true if is existed given compareString in this String else Return false.
				bool contains(const DString& compareString) const;

				/// \brief Determined is exist compareString in this String
				///
				/// \param compareString a string for checking existence in this String
				///
				/// \param pos position of first occurrence of compareString in this string
				///
				/// \return Return pair of result of founding and location of first occurrence if is
				/// existed given compareString in this String else Return false.
				std::pair<bool, uint64> contains(const DString& compareString, uint64& pos) const;

				/// \brief Determined is exist compareString in this String
				///
				/// \param startPos start position of comparing specified string for checking
				/// existence in this String
				///
				/// \param compareString a string for checking existence in this String
				///
				/// \param pos position of first occurrence of compareString in this string
				///
				/// \return Return pair of result of founding and location of first occurrence if is
				/// existed given compareString in this String else Return false.
				std::pair<bool, uint64> contains(uint64 startPos, const DString& compareString,
												 uint64& pos) const;

				/// \brief Get a lowercase of content of this String.
				///
				/// \return Return lowercase of content.
				DString toLower() const;

				/// \brief Get a uppercase of content of this String.
				///
				/// \return Return uppercase of content.
				DString toUpper() const;

				/// \brief It's the * operator for converting content of String to const array of \c
				/// char
				///
				/// \return Return result of converting content of String to const array of \c char
				const char* operator*();
				/*const Array<String> Splite(Char* pattern)
				{
				Array<String> arr;
				string temp = Data;
				size_t size = strlen(pattern);
				string::size_type si = 0;
				string subs;
				while ((si = temp.find(pattern)) != string::npos)
				{
				subs = temp.substr(0, si);
				arr.add(subs);
				temp.erase(0, si + size);
				}
				arr.add(temp);
				return arr;
				}*/

				/// \brief Convert from \c void* pdata to String
				///
				/// \return Return result of converting from \c void* pdata to String
				static DString convert(void* pdata);

				/// \brief Convert from String to \c void* pdata
				///
				/// \return Return result of converting from String to \c void* pdata
				// static void* retConvert(String pdata);
#if DGE_Platform == DGE_Windows_Platform
				/// \brief Convert from String to \c LPSTR
				///
				/// \return Return result of converting from String to \c LPSTR
				inline LPSTR ToLPSTR() { return (char*) m_stdStr.data(); }
#endif

				/// \brief Check String is empty or no.
				///
				/// \return Return true if String is empty else return false
				bool isEmpty() const;

				/// \brief Remove all characters from String
				void clear();

				/// \brief Get current length of String
				///
				/// \return Return length of String
				uint64_t size() const;

				/// \brief Replace searchString with replaceString everywhere that will be found in
				/// this string
				DString replace(const DString& searchString, const DString& replaceString) const;
				/// \brief Replace searchString with replaceString everywhere that will be found in
				/// this string
				DString replace(DString&& searchString, DString&& replaceString) const;

				/// \brief Replace searchString with replaceString everywhere that will be found in
				/// this string
				DString replace(uint64 pos, const DString& searchString,
								const DString& replaceString);

				/// \brief Retrieve last character in this string
				///
				/// \return Last Character in this String
				char last() const;

				/// \brief Retrieve first character in this string
				///
				/// \return First Character in this String
				char first() const;

				/// \brief Find searchWord in this string and return sub-string from last
				/// occurrences to begin or end of this string
				///
				/// \param searchWord The string is used for searching in this string
				/// \param returnRemain If it's true then return return String from location of find
				/// word to the end of this string otherwise return from begin of this string to
				/// location of find word
				DString findLastOf(const DString& searchWord, bool returnRemain = true) const;

				/// \brief Return sub-string of this string class with specified begin position in
				/// startPos and specified size of sub-string in length.
				DString subString(uint64 startPos, uint64 length);

				/// \brief Return removed whitespaced string from the string that is stored in this class.
				DString removeWhiteSpace();
			};

			/// \brief For Wide String with Wide Operator
			class DGE_CoreAPI WString
			{
				wstring m_stdStr;

			public:
				/// \brief Constructor convert \c const char* into WString
				///
				/// \details Convert const char* into WString that get help from std::string class
				/// for this conversation
				inline WString(const char* ch)
				{
					auto tempStr = std::string(ch);
					m_stdStr	 = std::wstring(tempStr.begin(), tempStr.end());
				}

				/// \brief Constructor convert \c const wchar_t* into WString
				///
				/// \details Convert const wchar_t* into WString directly with using std::wstring class
				inline WString(const wchar_t* ch) { m_stdStr = std::wstring(ch); }

				/// \brief Constructor convert \c std::string into WString
				///
				/// \details Convert \c std::string into WString
				inline WString(string str) { m_stdStr = std::wstring(str.begin(), str.end()); }

				/// \brief Constructor convert \c std::wstring into WString
				///
				/// \details Convert \c std::wstring into WString
				inline WString(wstring str = L"") { m_stdStr = str; }

#if DGE_Platform == DGE_Windows_Platform
				/// \brief Constructor convert \c LPCWSTR into WString
				///
				/// \details Convert \c LPCWSTR into WString
				/*inline WString(LPCWSTR str)
				{
					m_stdStr = str;
				}*/
#endif
				/// \brief Constructor convert \c int into WString
				///
				/// \details Convert \c int into WString
				inline WString(int data)
				{
					wstringstream myString;
					myString << data;
					myString >> m_stdStr;
				}

				/// \brief Constructor convert \c float into WString
				///
				/// \details Convert \c float into WString
				inline WString(float data)
				{
					wstringstream myString;
					myString << data;
					myString >> m_stdStr;
				}

				/// \brief Constructor convert \c String into WString
				///
				/// \details Convert \c String into WString
				inline WString(DString str)
				{
					auto stdStr = str.toStdString();
					m_stdStr	= std::wstring(stdStr.begin(), stdStr.end());
				}

				/// \brief Convert stored string to \c wchar_t* type
				///
				/// \return Return an array of wchar_t that it's converted string
				inline const wchar_t* toWChar() { return m_stdStr.c_str(); }

				/// \brief Concatenate two WString together and return result of concatenation of
				/// WString.
				///
				/// \return Return result of concatenation of WString.
				inline WString operator+(WString str)
				{
					WString s1 = this->m_stdStr + str.m_stdStr;
					return s1;
				}

				/// \brief Concatenate a WString and an int together and return result of
				/// concatenation of WString and int.
				///
				/// \return Return result of concatenation of WString and int.
				inline WString operator+(int data)
				{
					wstring		  s1 = this->m_stdStr;
					wstringstream myString;
					myString << data;
					s1 += myString.str();
					WString st = s1;
					return (st);
				}

				/// \brief Concatenate a WString and an unsigned int together and return result of
				/// concatenation of WString and unsigned int.
				///
				/// \return Return result of concatenation of WString and unsigned int.
				inline WString operator+(unsigned int data)
				{
					wstring		  s1 = this->m_stdStr;
					wstringstream myString;
					myString << data;
					s1 += myString.str();
					WString st = s1;
					return (st);
				}

				/// \brief Concatenate a WString and an unsigned short together and return result of
				/// concatenation of WString and unsigned short.
				///
				/// \return Return result of concatenation of WString and unsigned short.
				inline WString operator+(unsigned short data)
				{
					wstring		  s1 = this->m_stdStr;
					wstringstream myString;
					myString << data;
					s1 += myString.str();
					WString st = s1;
					return (st);
				}

				/// \brief Concatenate a WString and a double together and return result of
				/// concatenation of WString and double.
				///
				/// \return Return result of concatenation of WString and double.
				inline WString operator+(double data)
				{
					wstring		  s1 = this->m_stdStr;
					wstringstream myString;
					myString << data;
					s1 += myString.str();
					WString st = s1;
					return (st);
				}

				/// \brief Concatenate a WString and an unsigned long together and return result of
				/// concatenation of WString and unsigned long.
				///
				/// \return Return result of concatenation of WString and unsigned long.
				inline WString operator+(unsigned long data)
				{
					wstring		  s1 = this->m_stdStr;
					wstringstream myString;
					myString << data;
					s1 += myString.str();
					WString st = s1;
					return (st);
				}

				/// \brief Concatenate two Strings together and store result of
				/// concatenation of two WString in this object.
				///
				/// \return Return result of concatenation of two WString .
				inline WString operator+=(WString str)
				{
					this->m_stdStr += str.m_stdStr;
					// WString st=this->Data;
					return (*this);
				}

				/// \brief Concatenate a WString and an int together and store result of
				/// concatenation of WString and int in this object.
				///
				/// \return Return result of concatenation of WString and int.
				inline WString operator+=(int data)
				{
					wstringstream myString;
					myString << data;
					this->m_stdStr += myString.str();
					// String st=this->Data;
					return (*this);
				}

				/// \brief Concatenate a WString and a double together and store result of
				/// concatenation of WString and double in this object.
				///
				/// \return Return result of concatenation of WString and double.
				inline WString operator+=(double data)
				{
					wstringstream myString;
					myString << data;
					this->m_stdStr += myString.str();
					// String st=this->Data;
					return (*this);
				}

				/// \brief Concatenate a WString and an unsigned int together and store result of
				/// concatenation of WString and unsigned int in this object.
				///
				/// \return Return result of concatenation of WString and unsigned int.
				inline WString operator+=(unsigned int data)
				{
					wstringstream myString;
					myString << data;
					this->m_stdStr += myString.str();
					// String st=this->Data;
					return (*this);
				}

				/// \brief Concatenate a WString and an unsigned long together and store result of
				/// concatenation of WString and unsigned long in this object.
				///
				/// \return Return result of concatenation of WString and unsigned long.
				inline WString operator+=(unsigned long data)
				{
					wstringstream myString;
					myString << data;
					this->m_stdStr += myString.str();
					// String st=this->Data;
					return (*this);
				}

				/// \brief Compare content of two WStrings that return true if left side WString
				/// isn't equal with right side else return false.
				///
				/// \return Return true if left side WString isn't equal with right side
				/// else return false.
				inline bool operator!=(const WString& str) const
				{
					return this->m_stdStr != str.m_stdStr;
				}

				/// \brief Compare content of two WStrings that return true if left side WString
				/// isn't equal with right side else return false.
				///
				/// \return Return true if left side WString is greater
				/// else return false.
				inline bool operator==(const WString& str) const
				{
					return this->m_stdStr == str.m_stdStr;
				}

				/// \brief Resize string to size of content equal to size parameter
				inline void resize(uint32 size)
				{
					m_stdStr.resize(size);
					// throw Error("Engine","String","resize","Change Size Of String When Need to do
					// this","");
				}

				/// \brief Convert WString to std::wstring
				///
				/// \return Return result of converting WString to std::wstring
				inline wstring getWString() { return m_stdStr; }

				/// \brief Convert content of WString to \c std::wstring
				///
				/// \return Return result of converting content of WString to \c std::wstring
				inline operator wstring() { return m_stdStr; }

				/// \brief Convert content of WString to const array of \c wchar_t
				///
				/// \return Return result of converting content of WString to const array of \c wchar_t
				inline operator const wchar_t*() { return m_stdStr.c_str(); }

				/// \brief Convert content of WString to array of \c wchar_t
				///
				/// \return Return result of converting content of WString to array of \c wchar_t
				inline operator wchar_t*() { return (wchar_t*) m_stdStr.c_str(); }

				/// \brief Convert content of WString to const array of \c char
				///
				/// \return Return result of converting content of WString to const array of \c char
				inline operator const char*() { return DString(m_stdStr).toChar(); }

				/// \brief Convert content of WString to array of char
				///
				/// \return Return result of converting content of WString to array of \c char
				inline operator char*() { return (char*) DString(m_stdStr).toChar(); }

				/// \brief Convert content of WString to \c int
				///
				/// \return Return result of converting content of WString to \c int
				inline int toInt()
				{
					wstringstream myString;
					int			  data;
					myString << m_stdStr;
					myString >> data;
					return data;
				}
#if DGE_Platform == DGE_Windows_Platform
				/// \brief Convert content of WString to \c LPWSTR
				///
				/// \return Return result of converting content of WString to \c LPWSTR
				inline LPWSTR toLPWSTR() { return (wchar_t*) m_stdStr.c_str(); }

				// \brief Convert content of WString to \c LPCWSTR
				///
				/// \return Return result of converting content of WString to \c LPCWSTR
				inline LPCWSTR toLPCWSTR() { return (wchar_t*) m_stdStr.c_str(); }
#endif
				/// \brief Convert content of WString to String
				///
				/// \return Return result of converting content of WString to \c String
				inline DString getString() { return DString(m_stdStr); }

				/// \brief Check WString is empty or no.
				///
				/// \return Return true if WString is empty else return false
				inline bool isEmpty() { return m_stdStr.empty(); }

				/// \brief Remove all characters from WString
				inline void clear() { m_stdStr.clear(); }

				/// \brief Get current length of WString
				///
				/// \return Return length of WString
				uint64_t size() const { return m_stdStr.size(); }
			};

			inline WString DString::getWString()
			{
				return WString(std::wstring(m_stdStr.cbegin(), m_stdStr.cend()));
			}

			/// \brief Concatenate a String and a char* together and return result of
			/// concatenation of String and char*.
			///
			/// \return Return result of concatenation of String and char*.
			inline DString operator+(const DString& lhs, const char* rhs)
			{
				DString result = lhs;
				result += rhs;
				return result;
			}

			/// \brief Concatenate a String and a char* together and return result of
			/// concatenation of String and char*.
			///
			/// \return Return result of concatenation of String and char*.
			inline DString operator+(const char* lhs, const DString& rhs)
			{
				DString result = lhs;
				result += rhs;
				return result;
			}

			/// \brief Concatenate a String and a char* together and return result of
			/// concatenation of String and char*.
			///
			/// \return Return result of concatenation of String and char*.
			inline DString operator+(const DString& lhs, const DString& rhs)
			{
				DString result = lhs;
				result += rhs;
				return result;
			}

			using StringList  = std::vector<DString>;
			using WStringList = std::vector<WString>;
		} // namespace BaseType
	}	  // namespace Core
} // namespace DGE

template<>
struct std::hash<DGE::Core::BaseType::DString>
{
	size_t operator()(const DGE::Core::BaseType::DString& p) const
	{
		return std::hash<std::string>()(p.toStdString());
	}
};
