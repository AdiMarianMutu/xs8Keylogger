#include <Windows.h>
#include <fstream>
#include <sstream>
#include <locale>
#include <codecvt>

class File {

public:

	static bool Exists(const std::basic_string<TCHAR> &FILE_NAME) {
		std::ifstream File(FILE_NAME.c_str());

		return (bool)File;
	}

	static std::string ReadAllText(const std::basic_string<TCHAR> &FILE_NAME) {
		std::ifstream File(FILE_NAME);

		if (File.good()) {
			std::stringstream Buffer;
			File.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8_utf16<char, 0x10ffff, std::consume_header>));
			Buffer << File.rdbuf();

			return Buffer.str();
		}

		return "";
	}
	static std::wstring ReadAllText(const std::basic_string<TCHAR> &FILE_NAME, bool _NULL) {
		std::wifstream File(FILE_NAME, std::ios::binary);

		if (File.good()) {
			std::wstringstream Buffer;
			File.imbue(std::locale(std::locale::empty(), new std::codecvt_utf8_utf16<wchar_t, 0x10ffff, std::consume_header>));
			Buffer << File.rdbuf();

			return Buffer.str();
		}
		return L"";
	}

	static bool WriteAllText(const std::basic_string<TCHAR> &FILE_NAME, const std::string &TEXT) {
		std::ofstream File(FILE_NAME, std::ios::binary);

		if (File.good()) {
			File.imbue(std::locale(File.getloc(), new std::codecvt_utf8_utf16<char, 0x10FFFF, std::consume_header>));
			File << TEXT;

			return true;
		}
		return false;
	}
	static bool WriteAllText(const std::basic_string<TCHAR> &FILE_NAME, const std::wstring &TEXT) {
		std::wofstream File(FILE_NAME, std::ios::binary);

		if (File.good()) {
			File.imbue(std::locale(File.getloc(), new std::codecvt_utf8_utf16<wchar_t, 0x10FFFF, std::consume_header>));
			File << TEXT;

			return true;
		}
		return false;
	}

	static bool AppendText(const std::basic_string<TCHAR> &FILE_NAME, const std::string &TEXT) {
		std::ofstream File(FILE_NAME, std::ios::binary | std::ios_base::app);
		
		if (File.good()) {
			File.imbue(std::locale(File.getloc(), new std::codecvt_utf8_utf16<char, 0x10FFFF, std::consume_header>));
			File << TEXT;

			return true;
		}
		return false;
	}
	static bool AppendText(const std::basic_string<TCHAR> &FILE_NAME, const std::wstring &TEXT) {
		std::wofstream File(FILE_NAME, std::ios::binary | std::ios::app);

		if (File.good()) {
			File.imbue(std::locale(File.getloc(), new std::codecvt_utf8_utf16<wchar_t, 0x10FFFF, std::consume_header>));
			File << TEXT;

			return true;
		}
		return false;
	}
	
	static bool Delete(const std::basic_string<TCHAR> &FILE_NAME) {
		if (DeleteFile(FILE_NAME.c_str()) == TRUE) { return true; }
		return false;
	}
};
