/*
Web-Class-Helper Main File 1.0.9
This source code file is under MIT License.
Copyright (c) 2022 Class Tools Develop Team
Contributors: jsh-jsh ren-yc
*/
#include <stdio.h>
#include <windows.h>
#include <conio.h>
#include <sstream>
#include <bits/stdc++.h>
#include "WCH.h"
using namespace std;

struct TalkDate {
    int Year;
    int Month;
    int Day;
    int Hour;
    int Minute;
    int Second;
    int WeekDay;
    string Name;
};

multimap<int, pair<int, string>> mm;
TalkDate GetTime();
void PrintTime(TalkDate);
void PrintError();
void PrintOK();
void Bye();
inline void in_data(string);
inline void out_data(string);
void HideWindow(bool);
bool JudgeKey();
void Game();

int n;
bool cmd_line = true;
bool mode = false;
string op;
string UserName;
string Weekdayname[7] = {"Sunday.txt", "Monday.txt", "Tuesday.txt", "Wednesday.txt", "Thursday.txt", "Friday.txt", "Saturday.txt"};

ifstream fin;
ofstream fout;

int main() {
    system("mode con cols=100 lines=20");
    SetConsoleTitle("Web Class Helper 1.0.9");
    TalkDate q = GetTime();
    if (q.Month == 1 || q.Month == 2) {
        q.Month += 12;
        q.Year--;
    }
    int Week = (q.Day + 2 * q.Month + 3 * (q.Month + 1) / 5 + q.Year + q.Year / 4 - q.Year / 100 + q.Year / 400 + 1) % 7;
    HideWindow(1);
    fin.open(Weekdayname[Week].c_str());
    fin >> n;
    for (int i = 1; i <= n; i++) {
        int h = 0;
        int m = 0;
        string tname = "NULL";
        fin >> h >> m >> tname;
        mm.emplace(make_pair(h, make_pair(m, tname)));
    }
    printf("Web-Class-Helper 1.0.9\n");
    printf("Copyright (c) 2022 Class Tools Develop Team.\n");
    printf("Type 'help' to get help.\n\n");
    printf("Please input your username: ");
    cin >> UserName;
    cout << endl;
    while (op != "quit") {
        int h = GetTime().Hour;
        for (auto it = mm.equal_range(h).first; it != mm.equal_range(h).second; it++) {
            if ((it->second).first == GetTime().Minute && ((it->second).second).size() > 0) {
                printf("\a");
                MessageBox(NULL, ((it->second).second).c_str(), "Web Class Helper", MB_OK);
            }
        }
        if (JudgeKey()) {
            HideWindow(1);
            cmd_line = true;
        }
        if (cmd_line) {
            GetPath(mode, UserName);
            cin >> op;
        }
        if (op == "add") {
            int h = 0;
            int m = 0;
            string tname = "NULL";
            cin >> h >> m >> tname;
            if (h > 24 || m >= 60 || h < 1 || m < 0) {
                PrintError();
            } else {
                n++;
                mm.emplace(make_pair(h, make_pair(m, tname)));
                PrintOK();
            }
        } else if (op == "help") {
            printf("Commands:\n");
            printf("1. quit (Quit this program)\n");
            printf("2. add hour minute name (Add clock at hour:minute)\n");
            printf("3. delete hour minute name (Delete clock at hour:minute)\n");
            printf("4. change hour minute name (Change clock at hour:minute)\n");
            printf("5. ow (Get a sentence) **From web**\n");
            printf("6. hide (Hide the command line window)\n");
            printf("7. game (Guessing game)\n");
            printf("8. time (Get time at once)\n");
            printf("9. pi (Make a screenshot)\n");
            printf("10. mode cmd-mode (Switch command line mode cmd / bash)\n");
            printf("11. speedtest (Start a speed test with a GUI window)\n\n");
        } else if (op == "delete") {
            int h = 0;
            int m = 0;
            int flag = 0;
            string tname = "NULL";
            cin >> h >> m >> tname;
            for (auto it = mm.equal_range(h).first; it != mm.equal_range(h).second; it++) {
                if ((it->second).first == m && ((it->second).second) == tname) {
                    mm.erase(it);
                    PrintOK();
                    flag = 1;
                    n--;
                }
            }
            if (!flag) {
                PrintError();
            }
        } else if (op == "change") {
            int h = 0;
            int m = 0;
            int flag = 0;
            string tname = "NULL";
            cin >> h >> m >> tname;
            for (auto it = mm.equal_range(h).first; it != mm.equal_range(h).second; it++) {
                if ((it->second).first == m) {
                    ((it->second).second) = tname;
                    PrintOK();
                    flag = 1;
                }
            }
            if (!flag) {
                PrintError();
            }
        } else if (op == "ow") {
            GetGet();
        } else if (op == "hide") {
            HideWindow(0);
            cmd_line = false;
        } else if (op == "game") {
            Game();
        } else if (op == "time") {
            PrintTime(GetTime());
        }
        else if (op == "pi") {
            HideWindow(0);
            PutPicture();
            HideWindow(1);
            SaveImg();
        } else if (op == "mode") {
            string tmp;
            cin >> tmp;
            if (tmp == "cmd") {
                mode = true;
                printf("OK!\n\n");
            } else if (tmp == "bash") {
                mode = false;
                printf("OK!\n\n");
            } else {
                printf("Unknown mode.\n\n");
            }
        } else if (op == "speedtest") {
            system("start speedtest.exe");
            printf("OK!\n\n");
        } else if (op != "quit") {
            printf("Is it a system command? (Y/N): ");
            char tmp;
            cin >> tmp;
            if (tmp == 'Y')
            {
                system(op.c_str());
            }
            cout << endl;
        }
    }
    if (n != 0) {
        fout.open(Weekdayname[Week].c_str());
        fout << n << endl;
        for (auto it = mm.begin(); it != mm.end(); it++) {
            fout << (it -> first) << " " << (it -> second).first << " " << (it -> second).second << endl;
        }
    }
    return 0;
}

TalkDate GetTime() {
    TalkDate NowTime;
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
    NowTime.Name = "NULL";
    return NowTime;
}

void PrintTime(TalkDate a) {
    printf("%d/%02d/%02d %02d %02d %02d\n\n", a.Year, a.Month, a.Day, a.Hour, a.Minute, a.Second);
}

void PrintError() {
    printf("This input or code is wrong. Please see your code.\n\n");
}

inline void in_data(string fname) {
    freopen(fname.c_str(), "r", stdin);
}

inline void out_data(string fname) {
    freopen(fname.c_str(), "w", stdout);
}

void HideWindow(bool ju) {
    HWND hwnd = FindWindow("ConsoleWindowClass", NULL);
    if (hwnd) {
        ShowWindow(hwnd, ju);
    }
}

bool JudgeKey() {
    int a = GetKeyState(VK_CONTROL);
    int b = GetKeyState(VK_DOWN);
    if (a < 0 && b < 0) {
        return true;
    } else {
        return false;
    }
}

void Game() {
    srand(time(0));
    int n = rand() % 10000 + 1;
    string z = "0";
    try {
        while (stoi(z) != n) {
            cout << "Please input your number (1 ~ 10000): ";
            cin >> z;
            if (z[0] == '-' || z[0] == '0' || (z.size() > 5 && z != "10000")) {
                cout << "Number out of range." << endl;
            } else if (stoi(z) > n) {
                cout << "The answer is smaller." << endl;
            } else if (stoi(z) < n) {
                cout << "The answer is bigger." << endl;
            }
        }
    }
    catch (...) {
        cout << "Your input does not meet the requirements, game ending..." << endl << endl;
        return;
    }
    cout << "The number is " << n << "!!! You WIN!!!" << endl << endl;
    return;
}

void PrintOK() {
    printf("OK!\n\n");
}
