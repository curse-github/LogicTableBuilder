#include <iostream>
#include <locale>
#include <codecvt>
char trueValue = '1';
char falseValue = '0';
char getValue(bool val) {
    return val?trueValue:falseValue;
}
bool conditional(bool A, bool B) {
    return (A==false)||(B==true);
}
int main() {
    std::wstring_convert<std::codecvt_utf8<wchar_t>> converter;// used for the "upper-score" thing at the bottom of each table
    // formula one
    bool p = true;
    bool q = true;
    bool r = true;
    int truthCases = 0;// print header
    std::cout << "_________________________________________" << std::endl;
    std::cout << "| p | q | p+q | ~p*~q | (p+q) * (~p*~q) |" << std::endl;
    std::cout << "|---------------------------------------|" << std::endl;
    for (size_t i = 0; i < 4; i++) {
        p=(i&2)==0;
        q=(i&1)==0;
        if ((p||q)==(!p&&!q)) truthCases++;
        std::cout << "| " << getValue(p) << " | ";
        std::cout << getValue(q) << " | ";
        std::cout << getValue(p||q) << "   | ";
        std::cout << getValue(!p&&!q) << "     | ";
        std::cout << getValue((p||q)==(!p&&!q)) << "               |" << std::endl;
    }
    std::cout << converter.to_bytes(std::wstring(41,L'‾')) << std::endl;
    std::cout << "(p+q) * (~p*~q) has " << truthCases << " truth cases." << std::endl;
    if (truthCases==0) std::cout << "(p+q) * (~p*~q) is a Contradiction." << std::endl;
    else if (truthCases==4) std::cout << "(p+q) * (~p*~q) is a Tautology." << std::endl;
    else std::cout << "(p+q) * (~p*~q) is a Contingency." << std::endl;
    std::cout << std::endl;
    // formula two
    p=true;
    q=true;
    truthCases=0;// print header
    std::cout << "__________________________________________________" << std::endl;
    std::cout << "| p | q | p<->q | ~p<->~q | (p<->q) -> (~p<->~q) |" << std::endl;
    std::cout << "|------------------------------------------------|" << std::endl;
    for (size_t i = 0; i < 4; i++) {// print each row
        p=(i&2)==0;
        q=(i&1)==0;
        if (conditional(p==q,!p==!q)) truthCases++;
        std::cout << "| " << getValue(p) << " | ";
        std::cout << getValue(q) << " | ";
        std::cout << getValue(p==q) << "     | ";
        std::cout << getValue(!p==!q) << "       | ";
        std::cout << getValue(conditional(p==q,!p==!q)) << "                    |" << std::endl;
    }
    std::cout << converter.to_bytes(std::wstring(50,L'‾')) << std::endl;
    std::cout << "(p<->q) -> (~p<->~q) has " << truthCases << " truth cases." << std::endl;
    if (truthCases==0) std::cout << "(p<->q) -> (~p<->~q) is a Contradiction." << std::endl;
    else if (truthCases==4) std::cout << "(p<->q) -> (~p<->~q) is a Tautology." << std::endl;
    else std::cout << "(p<->q) -> (~p<->~q) is a Contingency." << std::endl;
    std::cout << std::endl;
    // formula three
    p=true;
    q=true;
    truthCases=0;// print header
    std::cout << "________________________________________________________________________" << std::endl;
    std::cout << "| p | q | r | p+q | ~p^r | (p+q)*(~p^r) | p*r | (p+q)*(~p^r) === (p*r) |" << std::endl;
    std::cout << "|----------------------------------------------------------------------|" << std::endl;
    for (size_t i = 0; i < 8; i++) {// print each row
        p=(i&4)==0;
        q=(i&2)==0;
        r=(i&1)==0;
        if (((p||q)&&(!p!=r))==(p&&r)) truthCases++;
        std::cout << "| " << getValue(p) << " | ";
        std::cout << getValue(q) << " | ";
        std::cout << getValue(r) << " | ";
        std::cout << getValue(p||q) << "   | ";
        std::cout << getValue(!p!=r) << "    | ";
        std::cout << getValue((p||q)&&(!p!=r)) << "            | ";
        std::cout << getValue(p&&r) << "   | " << getValue(((p||q)&&(!p!=r))==(p&&r)) << "                      |" << std::endl;
    }
    std::cout << converter.to_bytes(std::wstring(72,L'‾')) << std::endl;
    std::cout << "(p+q)*(~p^r) === (p*r) has " << truthCases << " truth cases." << std::endl;
    if (truthCases==0) std::cout << "(p+q)*(~p^r) === (p*r) is a Contradiction." << std::endl;
    else if (truthCases==8) std::cout << "(p+q)*(~p^r) === (p*r) is a Tautology." << std::endl;
    else std::cout << "(p+q)*(~p^r) === (p*r) is a Contingency." << std::endl;
    std::cout << std::endl;
    // formula four
    p=true;
    q=true;
    r=true;
    truthCases=0;// print header
    std::cout << "_________________________________________________________________________________" << std::endl;
    std::cout << "| p | r | q | p->r | (p->r)->q | q->r | p->(q->r) | [(p->r)->q] <-> [p->(q->r)] |" << std::endl;
    std::cout << "|-------------------------------------------------------------------------------|" << std::endl;
    for (size_t i = 0; i < 8; i++) {// print each row
        p=(i&4)==0;
        r=(i&2)==0;
        q=(i&1)==0;
        if ((conditional(conditional(p,r),q))==(conditional(p,conditional(q,r)))) truthCases++;
        std::cout << "| " << getValue(p) << " | ";
        std::cout << getValue(r) << " | ";
        std::cout << getValue(q) << " | ";
        std::cout << getValue(conditional(p,r)) << "    | ";
        std::cout << getValue(conditional(conditional(p,r),q)) << "         | ";
        std::cout << getValue(conditional(q,r)) << "    | ";
        std::cout << getValue(conditional(p,conditional(q,r))) << "         | ";
        std::cout << getValue((conditional(conditional(p,r),q))==(conditional(p,conditional(q,r)))) << "                           |" << std::endl;
    }
    std::cout << converter.to_bytes(std::wstring(81,L'‾')) << std::endl;
    std::cout << "[(p->r)->q] <-> [p->(q->r)] has " << truthCases << " truth cases." << std::endl;
    if (truthCases==0) std::cout << "[(p->r)->q] <-> [p->(q->r)] is a Contradiction." << std::endl;
    else if (truthCases==8) std::cout << "[(p->r)->q] <-> [p->(q->r)] is a Tautology." << std::endl;
    else std::cout << "[(p->r)->q] <-> [p->(q->r)] is a Contingency." << std::endl;
    return 0;
}