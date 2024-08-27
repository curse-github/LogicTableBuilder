#include "./main.h"
using std::wstring_convert, std::codecvt_utf8, std::wstring;
using std::cout, std::wcout, std::endl;
using std::vector;

char values[26] = {
    'F','F','F','F','F','F','F','F','F','F','F','F','F',
    'F','F','F','F','F','F','T','F','F','F','F','F','F'
};
vector<char> variables;
void addVar(const char& var) {
    if ((var=='T') || (var=='F')) return;
    if (std::find(variables.cbegin(),variables.cend(),var)==variables.cend()) {
        variables.push_back(var);
    }
}
char getVar(const char& A) {
    return values[A-'A'];
}
void setVar(const char& A, const bool& value) {
    if (value) values[A-'A']='T';
    else values[A-'A']='F';
}
#pragma region operations
// ¬A
char negation(const char& A) {
    if (getVar(A)=='F') return 'T';
    return 'F';
}
// A ⋀ B
char conjunction(const char& A, const char& B) {
    if ((getVar(A)=='T') && (getVar(B)=='T')) return 'T';
    return 'F';
}
// A ⋁ B
char disjunction(const char& A, const char& B) {
    if ((getVar(A)=='T') || (getVar(B)=='T')) return 'T';
    return 'F';
}
// A ⊕ B
char exclusiveOr(const char& A, const char& B) {
    // ¬(A ↔ B)
    return negation(iff(A,B));// this has the same truth table
}
// A → B
char implication(const char& A, const char& B) {
    if ((getVar(A)=='T') || (getVar(B)=='F')) return 'F';
    return 'T';
}
// A ↔ B
char iff(const char& A, const char& B) {
    if (getVar(A)==getVar(B)) return 'T';
    return 'F';
}
#pragma endregion operations
char getOperator(const wchar_t& input) {
    switch(input) {
        case L'¬': return 1;
        case L'~': return 1;
        case L'!': return 1;
        case L'⋀': return 2;
        case L'.': return 2;
        case L'&': return 2;
        case L'⋁': return 3;
        case L'|': return 3;
        case L'⊕': return 4;
        case L'^': return 4;
        case L'→': return 5;
        case L'⊃': return 5;
        case L'↔': return 6;
        case L'≡': return 6;
        default: return -1;
    }
}
char evalSimple(const wchar_t* input,size_t size) {
    if (getOperator(input[0])==1) {
        if (input[1]<'A' || input[1]>'Z') return 'E';
        else if (size>2) return 'E';
        else { addVar(input[1]); return negation(input[1]); }
    }
    else if (input[0]<'A' || input[0]>'Z') return 'E';
    else if (size==1) { addVar(input[0]); return getVar(input[0]); }
    else if (size!=3) return 'E';
    else if (input[2]<'A' || input[2]>'Z') return 'E';
    else {
        switch(getOperator(input[1])) {
            case 2: addVar(input[0]); addVar(input[2]); return conjunction(input[0],input[2]);
            case 3: addVar(input[0]); addVar(input[2]); return disjunction(input[0],input[2]);
            case 4: addVar(input[0]); addVar(input[2]); return exclusiveOr(input[0],input[2]);
            case 5: addVar(input[0]); addVar(input[2]); return implication(input[0],input[2]);
            case 6: addVar(input[0]); addVar(input[2]); return iff(input[0],input[2]);
            default: return 'E';
        }
    }
}

vector<wstring> tableFormulas;
void printLine() {
    size_t numFormulas = tableFormulas.size();
    size_t numVariables = variables.size();
    cout << '-'; for (short i = numVariables; i > 0; i--) cout << "----";
    for (size_t i = 0; i < numFormulas; i++) {
        cout << "---";
        short len = static_cast<short>(tableFormulas[i].size());
        for (short i = 0; i < len; i++) cout << '-';
    }
    cout << endl;
}
void showTable() {
    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;

    size_t numFormulas = tableFormulas.size();
    for (size_t i = 0; i < numFormulas; i++) {
        evalSimple(tableFormulas[i].c_str(),tableFormulas[i].size());
    }
    size_t numVariables = variables.size();
    // seperating line
    printLine();
    // truth table header
    cout << "|";
    for (short i = 0; i < numVariables; i++) cout << " \033[34m" << variables[i] << "\033[0m |";
    for (size_t i = 0; i < numFormulas; i++) {
        cout << " \033[34m" << converter.to_bytes(tableFormulas[i]) << "\033[0m |";
    }
    cout << endl;
    // seperating line
    printLine();
    // truth table body
    for (int i = (1<<numVariables)-1; i >= 0 ; i--) {
        cout << "|";
        for (short j = numVariables; j > 0; j--) {
            bool val = (i&(1<<(numVariables-j)))!=0;
            setVar(variables[j-1],val);
            cout << ' ' << (val?"\033[32mT":"\033[31mF") << "\033[0m |";
        }
        for (size_t j = 0; j < numFormulas; j++) {
            char returnVal = evalSimple(tableFormulas[j].c_str(),tableFormulas[j].size());
            cout << ' ' << ((returnVal=='T')?"\033[32mT":((returnVal=='F')?"\033[31mF":"\033[36mE")) << "\033[0m ";
            for (size_t k = 0; k < tableFormulas[j].size()-1; k++) {
                cout << ' ';
            }
            cout << '|';
        }
        cout << endl;
    }
    // seperating line
    printLine();
    // clear variables list and add last end line
    cout << endl;
    variables.clear();
}
/*
1: negation (not): '¬', '~', '!'
2: conjunction (and): '⋀', '.', '&'
3: disjunction (or): '⋁', '|'
4: exclusive or (xor): '⊕', '^'
5: implication: '→', '⊃'
6: iff: '↔', '≡'
*/
int main(int argc, char* argv[]) {
    tableFormulas.push_back(L"¬A");
    tableFormulas.push_back(L"A⋀B");
    tableFormulas.push_back(L"A⋁B");
    tableFormulas.push_back(L"A⊕B");
    tableFormulas.push_back(L"A→B");
    tableFormulas.push_back(L"A↔B");
    showTable();
    return 0;
}