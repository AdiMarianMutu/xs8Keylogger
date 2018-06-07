#include <string>
#include <codecvt>
#include <algorithm>
#include <iterator>
#include <iostream>
#include <memory>
#include <cstdio>
#include <time.h>

class _String {

private:

public:

	static std::wstring ToWString(const std::string &str) {
		size_t   _size = strlen(str.c_str())+1;
		wchar_t* _wstr = new wchar_t[_size];
		mbstowcs_s(NULL, _wstr, _size, str.c_str(), _TRUNCATE);
		return _wstr;
	}

	static std::string ToString(const std::wstring &str) {
		std::wstring_convert<std::codecvt_utf8_utf16<wchar_t>> _c;
		return _c.to_bytes(str);
	}

	static void Upper(char* str) {
		for (UINT32 i = 0; i < strlen(str); i++) {
			str[i] = toupper(str[i]);
		}
	}
	static void Upper(wchar_t* str) {
		for (UINT32 i = 0; i < wcslen(str); i++) {
			str[i] = towupper(str[i]);
		}
	}
	static void Lower(char* str) {
		for (UINT32 i = 0; i < strlen(str); i++) {
			str[i] = tolower(str[i]);
		}
	}
	static void Lower(wchar_t* str) {
		for (UINT32 i = 0; i < wcslen(str); i++) {
			str[i] = towlower(str[i]);
		}
	}

	static std::string ReplaceChar(std::string &strSrc, const char* _from, const char* _to) {
		if (strlen(_from) > 1 || strlen(_to) > 1) {
			return "";
		}

		std::string _s;
		char        _f = _from[0];
		char        _t = _to[0];
		
		if (_to == "") {           // If the new char is an "empty" char, use remove_copy
			std::remove_copy(strSrc.begin(), strSrc.end(), std::back_inserter(_s), _f);
			return _s;
		} else {
			_s = strSrc;
			std::replace(_s.begin(), _s.end(), _f, _t);
			return _s;
		}
		return "";
	}
	static std::wstring ReplaceChar(std::wstring &strSrc, const wchar_t* _from, const wchar_t* _to) {
		if (wcslen(_from) > 1 || wcslen(_to) > 1) {
			return L"";
		}

		std::wstring _s;
		wchar_t        _f = _from[0];
		wchar_t        _t = _to[0];

		if (_to == L"") {          // If the new char is an "empty" char, use remove_copy
			std::remove_copy(strSrc.begin(), strSrc.end(), std::back_inserter(_s), _f);
			return _s;
		} else {
			_s = strSrc;
			std::replace(_s.begin(), _s.end(), _f, _t);
			return _s;
		}
		return L"";
	}
	
	static std::basic_string<TCHAR> GetDateTimeNow() {
		std::basic_string<TCHAR> _r;
		time_t _tm = time(NULL);

		struct tm * _curtime = localtime(&_tm);

		_r = asctime(_curtime);
		_r.erase(_r.length() - 1); // Remove the last '\n' char from the string
		return _r;
	}
};
