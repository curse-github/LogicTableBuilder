#include <iostream>

#include <string>
#include <locale>
#include <codecvt>
#ifdef _WIN32
    #include <windows.h>
    void setPrintColorRed(){ SetConsoleTextAttribute(GetStdHandle((DWORD)-11),FOREGROUND_RED); }
    void setPrintColorGreen(){ SetConsoleTextAttribute(GetStdHandle((DWORD)-11),FOREGROUND_GREEN); }
    void setPrintColorBlue(){ SetConsoleTextAttribute(GetStdHandle((DWORD)-11),FOREGROUND_BLUE); }
    void setPrintColorCyan(){ SetConsoleTextAttribute(GetStdHandle((DWORD)-11),FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); }
    void setPrintColorNone(){ SetConsoleTextAttribute(GetStdHandle((DWORD)-11), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY); }
#else
    void setPrintColorRed(){ std::cout << "\033[31m";}
    void setPrintColorGreen(){ std::cout << "\033[32m";}
    void setPrintColorBlue(){ std::cout << "\033[34m";}
    void setPrintColorCyan(){ std::cout << "\033[36m";}
    void setPrintColorNone(){ std::cout << "\033[37m";}
#endif

#include <vector>
#include <algorithm>

#include <cmath>

char get(const char& A);
char negation(const char& A);
char conjunction(const char& A, const char& B);
char disjunction(const char& A, const char& B);
char exclusiveOr(const char& A, const char& B);
char implication(const char& A, const char& B);
char iff(const char& A, const char& B);