#include <iostream>
#include <vector>
#include <locale>
#include <codecvt>
#ifdef _WIN32
    #include <windows.h>
#endif
char trueValue = '1';
char falseValue = '0';
char getValue(bool val) {
    return val?trueValue:falseValue;
}
bool conditional(bool A, bool B) {
    return (A==false)||(B==true);
}
void printTruthCase(int val, int max, const char* formula) {
    std::cout << formula << " has " << val << " truth cases and ";
    if (val==max) std::cout << "is a Tautology." << std::endl;
    else if (val==0) std::cout << "is a Contradiction." << std::endl;
    else std::cout << "is a Contingency." << std::endl;
}
int main() {
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
#endif
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;// used for the "upper-score" thing at the bottom of each table
    // formula one
    bool p = true;
    bool q = true;
    bool r = true;
    std::vector<int> truthCases = { 0, 0, 0, 0, 0 };// print header
    std::cout << std::string(41,'-') << std::endl;
    std::cout << "| p | q | p+q | ~p*~q | (p+q) * (~p*~q) |" << std::endl;
    std::cout << std::string(41,'-') << std::endl;
    for (size_t i = 0; i < 4; i++) {
        p=(i&2)==0;
        q=(i&1)==0;
        if (p||q)             truthCases[0]++;
        if (!p&&!q)           truthCases[1]++;
        if ((p||q)==(!p&&!q)) truthCases[2]++;
        std::cout << "| " << getValue(p) << " | ";
        std::cout << getValue(q) << " | ";
        std::cout << getValue(p||q) << "   | ";
        std::cout << getValue(!p&&!q) << "     | ";
        std::cout << getValue((p||q)==(!p&&!q)) << "               |" << std::endl;
    }
    std::cout << std::string(41,'-') << std::endl;
    printTruthCase(truthCases[0],4,"p+q");
    printTruthCase(truthCases[1],4,"~p*~q");
    printTruthCase(truthCases[2],4,"(p+q) * (~p*~q)");
    std::cout << std::endl;

    // formula two
    truthCases = { 0, 0, 0, 0, 0 };// print header
    std::cout << std::string(50,'-') << std::endl;
    std::cout << "| p | q | p<->q | ~p<->~q | (p<->q) -> (~p<->~q) |" << std::endl;
    std::cout << std::string(50,'-') << std::endl;
    for (size_t i = 0; i < 4; i++) {// print each row
        p=(i&2)==0;
        q=(i&1)==0;
        if (p==q)                     truthCases[0]++;
        if (!p==!q)                   truthCases[1]++;
        if (conditional(p==q,!p==!q)) truthCases[2]++;
        std::cout << "| " << getValue(p) << " | ";
        std::cout << getValue(q) << " | ";
        std::cout << getValue(p==q) << "     | ";
        std::cout << getValue(!p==!q) << "       | ";
        std::cout << getValue(conditional(p==q,!p==!q)) << "                    |" << std::endl;
    }
    std::cout << std::string(50,'-') << std::endl;
    printTruthCase(truthCases[0],4,"p<->q");
    printTruthCase(truthCases[1],4,"~p<->~q");
    printTruthCase(truthCases[2],4,"(p<->q) -> (~p<->~q)");
    std::cout << std::endl;

    // formula three
    truthCases = { 0, 0, 0, 0, 0 };// print header
    std::cout << std::string(57,'-') << std::endl;
    std::cout << "| p | q | r | p+q | ~p^r | p*r | (p+q)*(~p^r) === (p*r) |" << std::endl;
    std::cout << std::string(57,'-') << std::endl;
    for (size_t i = 0; i < 8; i++) {// print each row
        p=(i&4)==0;
        q=(i&2)==0;
        r=(i&1)==0;
        if (p||q)                      truthCases[0]++;
        if (!p!=r)                     truthCases[1]++;
        if (p&&r)                      truthCases[2]++;
        if (((p||q)&&(!p!=r))==(p&&r)) truthCases[3]++;
        std::cout << "| " << getValue(p) << " | ";
        std::cout << getValue(q) << " | ";
        std::cout << getValue(r) << " | ";
        std::cout << getValue(p||q) << "   | ";
        std::cout << getValue(!p!=r) << "    | ";
        std::cout << getValue(p&&r) << "   | " << getValue(((p||q)&&(!p!=r))==(p&&r)) << "                      |" << std::endl;
    }
    std::cout << std::string(57,'-') << std::endl;
    printTruthCase(truthCases[0],8,"p+q");
    printTruthCase(truthCases[1],8,"~p^r");
    printTruthCase(truthCases[2],8,"p*r");
    printTruthCase(truthCases[3],8,"(p+q)*(~p^r) === (p*r)");
    std::cout << std::endl;

    // formula four
    truthCases = { 0, 0, 0, 0, 0 };// print header
    std::cout << std::string(81,'-') << std::endl;
    std::cout << "| p | r | q | p->r | (p->r)->q | q->r | p->(q->r) | [(p->r)->q] <-> [p->(q->r)] |" << std::endl;
    std::cout << std::string(81,'-') << std::endl;
    for (size_t i = 0; i < 8; i++) {// print each row
        p=(i&4)==0;
        r=(i&2)==0;
        q=(i&1)==0;
        if (conditional(p,r))                                                     truthCases[0]++;
        if (conditional(conditional(p,r),q))                                      truthCases[1]++;
        if (conditional(q,r))                                                     truthCases[2]++;
        if (conditional(p,conditional(q,r)))                                      truthCases[3]++;
        if ((conditional(conditional(p,r),q))==(conditional(p,conditional(q,r)))) truthCases[4]++;
        std::cout << "| " << getValue(p) << " | ";
        std::cout << getValue(r) << " | ";
        std::cout << getValue(q) << " | ";
        std::cout << getValue(conditional(p,r)) << "    | ";
        std::cout << getValue(conditional(conditional(p,r),q)) << "         | ";
        std::cout << getValue(conditional(q,r)) << "    | ";
        std::cout << getValue(conditional(p,conditional(q,r))) << "         | ";
        std::cout << getValue((conditional(conditional(p,r),q))==(conditional(p,conditional(q,r)))) << "                           |" << std::endl;
    }
    std::cout << std::string(81,'-') << std::endl;
    printTruthCase(truthCases[0],8,"p->r");
    printTruthCase(truthCases[1],8,"(p->r)->q");
    printTruthCase(truthCases[2],8,"q->r");
    printTruthCase(truthCases[3],8,"p->(q->r)");
    printTruthCase(truthCases[4],8,"[(p->r)->q] <-> [p->(q->r)]");
    return 0;
}