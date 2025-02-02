/*
Web Class Helper APIs Module Header File 2.0.1
This source code file is under MIT License.
Copyright (c) 2022 Class Tools Develop Team
Contributors: jsh-jsh ren-yc
*/
#ifndef APIS_H
#define APIS_H
#include "file-process.hpp"
#include "init.hpp"
#include "commands.hpp"
#include "functions.hpp"
#include "basic.hpp"

extern const wstring WCH_WDName[7];
extern map <wstring, function <void ()>> WCH_command_support;
extern vector <wstring> WCH_command_list;
extern multimap <int, pair <int, wstring>> WCH_clock_list;
extern set <wstring> WCH_task_list;
extern set <pair <wstring, wstring>> WCH_work_list;
extern wstring WCH_window_title;
extern HWND WCH_hWnd;
extern HMENU WCH_hMenu;
extern int WCH_clock_num;
extern int WCH_task_num;
extern int WCH_work_num;
extern int WCH_clock_change;
extern int WCH_task_change;
extern int WCH_work_change;
extern int WCH_ProgressBarCount;
extern int WCH_ProgressBarTot;
extern int WCH_InputTimes;
extern bool WCH_cmd_line;
extern bool WCH_anti_idle;
extern bool WCH_program_end;
extern wstring WCH_command;
extern wstring WCH_ProgressBarStr;
extern ifstream fin;
extern wifstream wfin;
extern ofstream fout;
extern wofstream wfout;
extern Json::StreamWriterBuilder bui;
WCH_Time WCH_GetTime();
void WCH_Sleep(int _ms);
void WCH_printlog(wstring _mode, wstring _info);
void WCH_read();
bool WCH_save_func();
int WCH_GetNumDigits(int _n);

void WCH_Sleep(int _ms) {
	// Sleep.
	while (_ms > 0 && !WCH_program_end) {
		Sleep(100);
		_ms -= 100;
	}
}

void WCH_PrintColor(WORD _mode) {
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), _mode);
}

void WCH_PrintChar(int _times, wchar_t _c) {
	// Print space.
	while (_times > 0 && !WCH_program_end) {
		_times--;
		wcout << _c;
	}
}

wstring StrToWstr(string str) {
	wstring result;
	int len = MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), NULL, 0);
	TCHAR* buffer = new TCHAR[len + 1];
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), (int)str.size(), buffer, len);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}

string WstrToStr(wstring wstr) {
	string result;
	int len = WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.size(), NULL, 0, NULL, NULL);
	char* buffer = new char[len + 1];
	WideCharToMultiByte(CP_ACP, 0, wstr.c_str(), (int)wstr.size(), buffer, len, NULL, NULL);
	buffer[len] = '\0';
	result.append(buffer);
	delete[] buffer;
	return result;
}

string UrlEncode(const wstring& input) {
	string output;
	int cbNeeded = WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, NULL, 0, NULL, NULL);
	if (cbNeeded > 0) {
		char* utf8 = new char[cbNeeded];
		if (WideCharToMultiByte(CP_UTF8, 0, input.c_str(), -1, utf8, cbNeeded, NULL, NULL) != 0) {
			for (char* p = utf8; *p; *p++) {
				char onehex[5];
				_snprintf(onehex, sizeof(onehex), "%%%02.2X", (unsigned char)*p);
				output.append(onehex);
			}
		}
		delete[] utf8;
	}
	return output;
}

string UTF8ToANSI(string strUTF8) {
	UINT nLen = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, NULL, 0);
	WCHAR* wszBuffer = new WCHAR[nLen + 1];
	nLen = MultiByteToWideChar(CP_UTF8, 0, strUTF8.c_str(), -1, wszBuffer, nLen);
	wszBuffer[nLen] = 0;
	nLen = WideCharToMultiByte(936, 0, wszBuffer, -1, NULL, 0, NULL, 0);
	CHAR* szBuffer = new CHAR[nLen + 1];
	nLen = WideCharToMultiByte(936, 0, wszBuffer, -1, szBuffer, nLen, NULL, 0);
	szBuffer[nLen] = 0;
	strUTF8 = szBuffer;
	delete[] szBuffer;
	delete[] wszBuffer;
	return strUTF8;
}

vector <wstring> WCH_split(const wstring &_in) {
	#ifdef _DEBUG
	WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Spliting string: \"" + _in + L"\"");
	#endif
	vector <wstring> _res;
	wstring _tmp;
	bool flag = false;
	for (int i = 0; i < (int)_in.size(); i++) {
		if (_in[i] == L' ' && !flag && i != 0) {
			if (_in[i - 1] != L'"') {
				_res.push_back(_tmp);
				_tmp = L"";
			}
		} else if (_in[i] == L'"') {
			if (flag) {
				_res.push_back(_tmp);
				_tmp = L"";
				flag = false;
			} else {
				flag = true;
			}
		} else {
			_tmp += _in[i];
		}
	}
	if (_tmp != L"") {
		_res.push_back(_tmp);
	}
	if (flag) {
		_res.clear();
		_res.push_back(L"Incorrect");
	}
	#ifdef _DEBUG
	if ((int)_res.size() != 0) {
		if ((int)_res.size() != 1) {
			wstring _debug = L"Splited result: \"" + _res[0] + L"\", ";
			for (int i = 1; i < (int)_res.size() - 1; i++) {
				_debug += L"\"" + _res[i] + L"\", ";
			}
			_debug += L"\"" + _res[(int)_res.size() - 1] + L"\"";
			WCH_printlog(WCH_LOG_STATUS_DEBUG, _debug);
		} else {
			WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Splited result: \"" + _res[0] + L"\"");
		}
	}
	#endif
	return _res;
}

WCH_Time WCH_GetTime() {
	// Get current time and return a WCH_Time object.
	WCH_Time NowTime;
	time_t rawtime;
	struct tm *ptminfo;
	time(&rawtime);
	ptminfo = localtime(&rawtime);
	NowTime.Year = ptminfo -> tm_year + 1900;
	NowTime.Month = ptminfo -> tm_mon + 1;
	NowTime.Day = ptminfo -> tm_mday;
	NowTime.Hour = ptminfo -> tm_hour;
	NowTime.Minute = ptminfo -> tm_min;
	NowTime.Second = ptminfo -> tm_sec;
	return NowTime;
}

wstring WCH_GetCompileTime() {
	// Get program compile time.
	vector <wstring> spi = WCH_split(StrToWstr(__DATE__));
	map <wstring, int> mon;
	mon[L"Jan"] = 1;
	mon[L"Feb"] = 2;
	mon[L"Mar"] = 3;
	mon[L"Apr"] = 4;
	mon[L"May"] = 5;
	mon[L"Jun"] = 6;
	mon[L"Jul"] = 7;
	mon[L"Aug"] = 8;
	mon[L"Sep"] = 9;
	mon[L"Oct"] = 10;
	mon[L"Nov"] = 11;
	mon[L"Dec"] = 12;
	return format(L"{}/{:02}/{} {}", spi[2], mon[spi[0]], spi[1], StrToWstr(__TIME__));
}

void WCH_SetWindowStatus(bool flag) {
	// Set the window status by Windows API.
	ShowWindow(WCH_hWnd, flag);
	WCH_cmd_line = flag;
	WCH_printlog(WCH_LOG_STATUS_INFO, format(L"\"CONSOLE\" argument \"STATUS\" was set to {}", (flag == true ? L"\"SHOW\"" : L"\"HIDE\"")));
}

void WCH_SetTrayStatus(bool flag) {
	// Set the tray status by Windows API.
	ShowWindow(FindWindowW(L"Shell_trayWnd", NULL), (flag == true ? SW_SHOW : SW_HIDE));
	WCH_printlog(WCH_LOG_STATUS_INFO, format(L"\"TRAY\" argument \"STATUS\" was set to {}", (flag == true ? L"\"SHOW\"" : L"\"HIDE\"")));
}

void WCH_PutPicture() {
	// Press PrintScreen. (Keyboard)
	keybd_event(VK_SNAPSHOT, 0, 0, 0);
	keybd_event(VK_SNAPSHOT, 0, KEYEVENTF_KEYUP, 0);
	WCH_printlog(WCH_LOG_STATUS_INFO, L"Copying screenshot to clipboard");
}

void WCH_SaveImg() {
	// Save screenshot to file.
	WCHAR cUserNameBuffer[256] = {0};
	DWORD dwUserNameSize = 256;
	GetUserNameW(cUserNameBuffer, &dwUserNameSize);
	WCH_Time now = WCH_GetTime();
	wstring SavePath = format(L"C:\\Users\\{}\\Pictures\\{:04}{:02}{:02}{:02}{:02}{:02}.jpg", cUserNameBuffer, now.Year, now.Month, now.Day, now.Hour, now.Minute, now.Second);
	HDC hdcScreen = ::GetDC(NULL);
	int oDpi = GetDpiForWindow(GetDesktopWindow());
	double dDpi = 0;
	switch (oDpi) {
		case 96:
			dDpi = 1;
			break;
		case 120:
			dDpi = 1.25;
			break;
		case 144:
			dDpi = 1.5;
			break;
		case 192:
			dDpi = 2;
			break;
		default:
			WCH_printlog(WCH_LOG_STATUS_ERROR, L"Cannot get the DPI, please try to restart this program");
			wcout << L"Cannot get the DPI, please try to restart this program." << endl;
			break;
	}
	int nBitPerPixel = GetDeviceCaps(hdcScreen, BITSPIXEL);
	int nWidth = (int)round(GetDeviceCaps(hdcScreen, HORZRES) * dDpi);
	int nHeight = (int)round(GetDeviceCaps(hdcScreen, VERTRES) * dDpi);
	HDC hMemDC;
	HBITMAP hBitmap, hOldBitmap;
	hMemDC = CreateCompatibleDC(hdcScreen);
	hBitmap = CreateCompatibleBitmap(hdcScreen, nWidth, nHeight);
	hOldBitmap = (HBITMAP)SelectObject(hMemDC, hBitmap);
	BitBlt(hMemDC, 0, 0, nWidth, nHeight, hdcScreen, 0, 0, SRCCOPY);
	GdiplusWrapper gdiplus {};
	gdiplus.SaveImage(hBitmap, SavePath.c_str(), L"image/jpeg");
	DeleteDC(hdcScreen);
	DeleteDC(hMemDC);
	DeleteObject(hBitmap);
	WCH_printlog(WCH_LOG_STATUS_INFO, L"Saving image to \"" + SavePath + L"\"");
}

void WCH_CheckAndDeleteFile(wstring _filename) {
	// Delete a file if it exists.
	if (_waccess(_filename.c_str(), 0) != -1) {
		DeleteFileW(_filename.c_str());
	}
}

bool WCH_FileIsBlank(wstring _filename) {
	// Check if a file is blank.
	if (_waccess(_filename.c_str(), 0) != -1) {
		wfin.open(_filename, ios::in);
		wstring _line;
		while (getline(wfin, _line)) {
			if ((int)_line.size() != 0) {
				wfin.close();
				return false;
			}
		}
		wfin.close();
		return true;
	}
	return false;
}

bool WCH_TaskKill(wstring name) {
	// Kill a task by system command.
	_wsystem((L"TASKKILL /F /IM " + name + L" > WCH_SYSTEM_NORMAL.tmp 2> WCH_SYSTEM_ERROR.tmp").c_str());
	bool _res = (!WCH_FileIsBlank(L"WCH_SYSTEM_NORMAL.tmp") && WCH_FileIsBlank(L"WCH_SYSTEM_ERROR.tmp"));
	DeleteFileW(L"WCH_SYSTEM_NORMAL.tmp");
	DeleteFileW(L"WCH_SYSTEM_ERROR.tmp");
	return _res;
}

bool WCH_CheckVersion(const WCH_Version _Fir, const WCH_Version _Sec) {
	if (_Fir.X < _Sec.X) {
		return true;
	} else if (_Fir.X > _Sec.X) {
		return false;
	} else {
		if (_Fir.Y < _Sec.Y) {
			return true;
		} else if (_Fir.Y > _Sec.Y) {
			return false;
		} else {
			if (_Fir.Z < _Sec.Z) {
				return true;
			} else {
				return false;
			}
		}
	}
}

WCH_Version WCH_GetVersion(wstring _in) {
	// Get version from string.
	WCH_Version _res;
	int _pos = (int)_in.find(L".");
	if (_pos != wstring::npos) {
		_res.X = stoi(_in.substr(0, _pos));
		_in = _in.substr(_pos + 1);
	}
	_pos = (int)_in.find(L".");
	if (_pos != wstring::npos) {
		_res.Y = stoi(_in.substr(0, _pos));
		_in = _in.substr(_pos + 1);
	}
	_res.Z = (int)stoi(_in);
	return _res;
}

int WCH_GetNumDigits(int _n) {
	// Get digits of a number.
	int _cnt = 1;
	while ((_n /= 10) != 0) {
		_cnt++;
	}
	return _cnt;
}

void WCH_PrintProgressBar(int _sur, int _n, bool _flag) {
	// Print a progress bar.
	wstring _ETAStr = format(L"{:02}:{:02}:{:02}", (int)(_sur / 3600), (int)((_sur % 3600) / 60), (int)(_sur % 60));
	if (_flag) {
		WCH_PrintChar(WCH_ProgressBarCount, '\b');
	}
	WCH_PrintColor(0x0A);
	for (int i = 0; i < _n / 2; i++) {
		wcout << WCH_ProgressBarStr;
	}
	WCH_PrintColor(0x0C);
	for (int i = _n / 2; i < 50; i++) {
		wcout << WCH_ProgressBarStr;
	}
	WCH_PrintColor(0x02);
	wcout << L" " << _n << L"%";
	WCH_PrintColor(0x07);
	wcout << L" ETA ";
	WCH_PrintColor(0x09);
	wcout << _ETAStr;
	WCH_PrintColor(0x07);
	WCH_ProgressBarCount = 65 + WCH_GetNumDigits(_n);
}

void WCH_ProgressBar() {
	// Progress bar.
	int _pro = 100 / WCH_ProgressBarTot;
	WCH_PrintProgressBar(WCH_ProgressBarTot, 0, false);
	for (int i = WCH_ProgressBarTot - 1; i > 0 && !WCH_program_end; i--) {
		WCH_Sleep(1000);
		WCH_PrintProgressBar(i, (WCH_ProgressBarTot - i) * _pro, true);
	}
	WCH_Sleep(1000);
	WCH_PrintProgressBar(0, 100, true);
	wcout << endl;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	// Window processing module.
	NOTIFYICONDATA nid {};
	switch (message) {
		case WM_HOTKEY:
			#ifdef _DEBUG
			WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Entering \"WndProc()\": \"WM_HOTKEY\" & \"wParam = " + to_wstring(wParam) + L"\" & \"lParam = " + to_wstring(lParam) + L"\"");
			#endif
			if (wParam == WCH_HOTKEY_SHOW) {
				if (!WCH_program_end) {
					if (WCH_anti_idle) {
						WCH_anti_idle = false;
						WCH_SetTrayStatus(true);
					}
					WCH_SetWindowStatus(true);
				}
			}
			break;
		case WM_CREATE:
			#ifdef _DEBUG
			WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Entering \"WndProc()\": \"WM_CREATE\"");
			#endif
			nid.cbSize = sizeof(nid);
			nid.hWnd = hWnd;
			nid.uID = 0;
			nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
			nid.uCallbackMessage = WM_USER;
			nid.hIcon = (HICON)LoadImageW(NULL, L"WCH.ico", IMAGE_ICON, 0, 0, LR_LOADFROMFILE);
			wcscpy(nid.szTip, WCH_window_title.c_str());
			Shell_NotifyIconW(NIM_ADD, &nid);
			WCH_hMenu = CreatePopupMenu();
			AppendMenuW(WCH_hMenu, MF_STRING, WCH_MENU_SHOW, L"Show (Disable anti-idle)");
			AppendMenuW(WCH_hMenu, MF_SEPARATOR, 0, NULL);
			AppendMenuW(WCH_hMenu, MF_STRING, WCH_MENU_QUIT, L"Quit");
			break;
		case WM_USER:
			if (lParam == WM_LBUTTONDOWN) {
				#ifdef _DEBUG
				WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Entering \"WndProc()\": \"WM_USER\" & \"WM_LBUTTONDOWN\"");
				#endif
				if (!WCH_program_end) {
					if (WCH_anti_idle) {
						WCH_anti_idle = false;
						WCH_SetTrayStatus(true);
					}
					WCH_SetWindowStatus(true);
				}
			} else if (lParam == WM_RBUTTONDOWN) {
				POINT pt;
				int xx;
				GetCursorPos(&pt);
				SetForegroundWindow(hWnd);
				xx = TrackPopupMenu(WCH_hMenu, TPM_RETURNCMD, pt.x, pt.y, NULL, hWnd, NULL);
				#ifdef _DEBUG
				WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Entering \"WndProc()\": \"WM_USER\" & \"WM_RBUTTONDOWN\" & \"xx = " + to_wstring(xx) + L"\"");
				#endif
				if (xx == WCH_MENU_SHOW) {
					#ifdef _DEBUG
					WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Entering \"WndProc()\": \"WM_USER\" & \"WM_RBUTTONDOWN\" & \"WCH_MENU_SHOW\"");
					#endif
					if (!WCH_program_end) {
						if (WCH_anti_idle) {
							WCH_anti_idle = false;
							WCH_SetTrayStatus(true);
						}
						WCH_SetWindowStatus(true);
					}
				} else if (xx == WCH_MENU_QUIT) {
					#ifdef _DEBUG
					WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Entering \"WndProc()\": \"WM_USER\" & \"WM_RBUTTONDOWN\" & \"WCH_MENU_QUIT\"");
					#endif
					WCH_command_list.clear();
					WCH_command_list.push_back(L"quit");
					wcout << endl;
					exit(0);
				} else if (xx == 0) {
					PostMessageW(hWnd, WM_LBUTTONDOWN, NULL, NULL);
				}
			}
			break;
		case WM_DESTROY:
			#ifdef _DEBUG
			WCH_printlog(WCH_LOG_STATUS_DEBUG, L"Entering \"WndProc()\": \"WM_DESTROY\"");
			#endif
			Shell_NotifyIconW(NIM_DELETE, &nid);
			PostQuitMessage(0);
			break;
		default:
			if (message == RegisterWindowMessageW(L"TaskbarCreated")) {
				SendMessageW(hWnd, WM_CREATE, wParam, lParam);
			}
			break;
	}
	return DefWindowProcW(hWnd, message, wParam, lParam);
}

void WCH_ShowBugMessagebox(int errorcode, wstring errormsg) {
	// Show messagebox to inform a bug to user.
	wcout << L"\a";
	if (MessageBoxW(NULL, (L"Oops! An error occurred.\nPlease inform our developers with the error message by open a new Issue in our GitHub Repository.\nError message: " + to_wstring(errorcode) + L" " + errormsg + L"\nWould you like to visit the Issues page now?").c_str(), L"WCH ERROR", MB_ICONERROR | MB_YESNO) == IDYES) {
		_wsystem(L"start resources/website/issues.url");
	}
}

void WCH_signalHandler() {
	// Signal handler.
	signal(SIGINT, [](int signum) {
		WCH_cmd_line = false;
		WCH_program_end = true;
		WCH_PrintColor(0x07);
		wcout << endl;
		WCH_printlog(WCH_LOG_STATUS_ERROR, L"Signal " + to_wstring(signum) + L" detected (Program interrupted)");
		Sleep(500);
		WCH_SetWindowStatus(false);
		WCH_ShowBugMessagebox(signum, L"Program interrupted");
		exit(signum);
	});
	signal(SIGABRT, [](int signum) {
		WCH_cmd_line = false;
		WCH_program_end = true;
		WCH_PrintColor(0x07);
		wcout << endl;
		WCH_printlog(WCH_LOG_STATUS_ERROR, L"Signal " + to_wstring(signum) + L" detected (Program aborted)");
		Sleep(500);
		WCH_SetWindowStatus(false);
		WCH_ShowBugMessagebox(signum, L"Program aborted");
		exit(signum);
	});
	signal(SIGFPE, [](int signum) {
		WCH_cmd_line = false;
		WCH_program_end = true;
		WCH_PrintColor(0x07);
		wcout << endl;
		WCH_printlog(WCH_LOG_STATUS_ERROR, L"Signal " + to_wstring(signum) + L" detected (Operation overflow)");
		Sleep(500);
		WCH_SetWindowStatus(false);
		WCH_ShowBugMessagebox(signum, L"Operation overflow");
		exit(signum);
	});
	signal(SIGILL, [](int signum) {
		WCH_cmd_line = false;
		WCH_program_end = true;
		WCH_PrintColor(0x07);
		wcout << endl;
		WCH_printlog(WCH_LOG_STATUS_ERROR, L"Signal " + to_wstring(signum) + L" detected (Illegal instruction)");
		Sleep(500);
		WCH_SetWindowStatus(false);
		WCH_ShowBugMessagebox(signum, L"Illegal instruction");
		exit(signum);
	});
	signal(SIGSEGV, [](int signum) {
		WCH_cmd_line = false;
		WCH_program_end = true;
		WCH_PrintColor(0x07);
		wcout << endl;
		WCH_printlog(WCH_LOG_STATUS_ERROR, L"Signal " + to_wstring(signum) + L" detected (Access to illegal memory)");
		Sleep(500);
		WCH_SetWindowStatus(false);
		WCH_ShowBugMessagebox(signum, L"Access to illegal memory");
		exit(signum);
	});
}

#endif