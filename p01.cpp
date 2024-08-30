#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <vector>
#include <algorithm>
#include <cmath>
#include <vector>
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
using std::cout, std::endl, std::vector, std::wstring, std::wstring_convert, std::codecvt_utf8;
wstring removeSpaces(wstring input) {
    input.erase(std::remove(input.begin(), input.end(),L' '), input.end());
    return input;
}

vector<char> variables;// list of 
long variableValues = 0;// bitmap for the values of each variable
char getVar(const char& A) {
    if ((A=='T')||(A=='1')) return 'T'; else if ((A=='F')||(A=='0')) return 'F';
    if (std::find(variables.cbegin(),variables.cend(),A)==variables.cend()) variables.push_back(A);// add the variables to the list if it is not already there
    return ((variableValues&(1<<(A-'A')))==0)?'F':'T';
}
void setVar(const char& A, const bool& value) {
    if (value) variableValues|=(1<<(A-'A')); else variableValues&=~(1<<(A-'A'));
}
wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
char eval(const wstring input) {
    size_t inputSize = input.size();
    if (inputSize==1) return getVar(input[0]);
    for (size_t i = 0; i < inputSize; i++) {
        if ((input[i]=='(')||(input[i]=='[')||(input[i]=='{')) {
            int parenthasisCount = 0;
            wstring tmpStr = input.substr(0,i);
            int endingIndex = -1;
            if (input[i]=='(') {
                for (size_t j = i+1; j < inputSize; j++) {
                    if (input[j]=='(') parenthasisCount++;
                    else if (input[j]==')') {
                        if (parenthasisCount==0) { endingIndex=j; break; }
                        else parenthasisCount--;
                    }
                }
            } else if (input[i]=='[') {
                for (size_t j = i+1; j < inputSize; j++) {
                    if (input[j]=='[') parenthasisCount++;
                    else if (input[j]==']') {
                        if (parenthasisCount==0) { endingIndex=j; break; }
                        else parenthasisCount--;
                    }
                }
            } else if (input[i]=='{') {
                for (size_t j = i+1; j < inputSize; j++) {
                    if (input[j]=='{') parenthasisCount++;
                    else if (input[j]=='}') {
                        if (parenthasisCount==0) { endingIndex=j; break; }
                        else parenthasisCount--;
                    }
                }
            }
            if (endingIndex==-1) return 'E';// did not find end
            tmpStr.push_back((wchar_t)eval(input.substr(i+1,endingIndex-i-1)));
            return eval(tmpStr+input.substr(endingIndex+1,inputSize-(i-endingIndex)-1));
        }
    }
    wstring tmpStr = L"";
    for (size_t i = 0; i < inputSize; i++) {// parse negations
        if ((input[i]==L'¬')||(input[i]=='~')||(input[i]=='!')){
            tmpStr+=input.substr(0,i);
            tmpStr.push_back((getVar(input[i+1])=='T')?'F':'T');
            return eval(tmpStr+input.substr(i+2,inputSize-i-2));
        }
    }
    for (size_t i = 0; i < inputSize; i++) {// parse conjunctions
        if ((input[i]==L'⋀')||(input[i]=='&')||(input[i]=='.')||(input[i]=='*')){
            tmpStr+=input.substr(0,i-1);
            tmpStr.push_back(((getVar(input[i-1])=='T')&&(getVar(input[i+1])=='T'))?'T':'F');
            return eval(tmpStr+input.substr(i+2,inputSize-i-2));
        }
    }
    for (size_t i = 0; i < inputSize; i++) {// parse disjunctions
        if ((input[i]==L'⋁')||(input[i]=='|')||(input[i]=='+')){
            tmpStr+=input.substr(0,i-1);
            tmpStr.push_back(((getVar(input[i-1])=='T')||(getVar(input[i+1])=='T'))?'T':'F');
            return eval(tmpStr+input.substr(i+2,inputSize-i-2));
        }
    }
    for (size_t i = 0; i < inputSize; i++) {// parse exclusive or
        if ((input[i]==L'⊕')||(input[i]=='^')){
            tmpStr+=input.substr(0,i-1);
            tmpStr.push_back((getVar(input[i-1])!=getVar(input[i+1]))?'T':'F');
            return eval(tmpStr+input.substr(i+2,inputSize-i-2));
        }
    }
    for (size_t i = 0; i < inputSize; i++) {// parse implication
        if ((input[i]==L'→')||(input[i]==L'⊃')){
            tmpStr+=input.substr(0,i-1);
            tmpStr.push_back(((getVar(input[i-1])=='T')&&(getVar(input[i+1])=='F'))?'F':'T');
            return eval(tmpStr+input.substr(i+2,inputSize-i-2));
        } else if ((input[i]=='-')&&(input[i-1]!='<')&&(input[i+1]=='>')) {
            tmpStr+=input.substr(0,i-1);
            tmpStr.push_back(((getVar(input[i-1])=='T')&&(getVar(input[i+2])=='F'))?'F':'T');
            return eval(tmpStr+input.substr(i+3,inputSize-i-3));
        }
    }
    for (size_t i = 0; i < inputSize; i++) {// parse iff and equivilence operators
        if ((input[i]==L'↔')||(input[i]==L'≡')) {
            tmpStr+=input.substr(0,i-1);
            tmpStr.push_back((getVar(input[i-1])==getVar(input[i+1]))?'T':'F');
            return eval(tmpStr+input.substr(i+2,inputSize-i-2));
        } else if (((input[i]=='<')&&(input[i+1]=='-')&&(input[i+2]=='>')) || ((input[i]=='=')&&(input[i+1]=='=')&&(input[i+2]=='='))) {
            tmpStr+=input.substr(0,i-1);
            tmpStr.push_back((getVar(input[i-1])==getVar(input[i+3]))?'T':'F');
            return eval(tmpStr+input.substr(i+4,inputSize-i-4));
        }
    }
    return 'E';
}
vector<wstring> tableFormulas;
vector<bool> isFormulaTautology;
vector<bool> isFormulaContradiction;
vector<bool> isFormulaError;
void printLine() {
    size_t numFormulas = tableFormulas.size();
    size_t numVariables = variables.size();// print dashes for the width of the variable columns
    cout << '-'; for (short i = numVariables; i > 0; i--) cout << "----";
    for (size_t i = 0; i < numFormulas; i++) {// print dashes for the width of the formula columns
        cout << "---"; for (short j = 0; j < static_cast<short>(tableFormulas[i].size()); j++) cout << '-';
    }
}
void printValue(char val) {
    if (val=='T') { setPrintColorGreen(); cout << " T "; }// print the value as T or F
    else if (val=='F') { setPrintColorRed(); cout << " F "; }
    else { setPrintColorCyan(); cout << " E "; }
    setPrintColorNone();
}
void showTable() {
    #ifdef _WIN32
        SetConsoleOutputCP(CP_UTF8);
    #endif
    size_t numFormulas = tableFormulas.size();
    for (size_t i = 0; i < numFormulas; i++) {// evaluate each formula to get the list of variables in them
        eval(removeSpaces(tableFormulas[i]));
        isFormulaTautology.push_back(true);// and initialize the vectors containing data on if each expression is always true or always false
        isFormulaContradiction.push_back(true);
        isFormulaError.push_back(false);
    }
    size_t numVariables = variables.size();
    printLine();cout << endl;// print seperating line
    cout << '|';// print table header of each variable and formula
    for (short i = 0; i < numVariables; i++) {
        setPrintColorBlue(); cout << ' ' << variables[i];
        setPrintColorNone(); cout << " |";
    }
    for (size_t i = 0; i < numFormulas; i++) {
        setPrintColorBlue(); cout << ' ' << converter.to_bytes(tableFormulas[i].c_str());
        setPrintColorNone(); cout << " |";
    }
    cout << endl;
    printLine();cout << endl;// print seperating line
    for (int i = (1<<numVariables)-1; i >= 0 ; i--) {// print body of the table
        cout << "|";// print the columns for each variable
        for (short j = numVariables; j > 0; j--) {
            bool val = (i&(1<<(j-1)))!=0;// get individual bits from the i variable for the T or F for each variable
            setVar(variables[numVariables-j],val);// set the value
            printValue(val?'T':'F');cout << '|';
        }
        for (size_t j = 0; j < numFormulas; j++) {// print the columns for each formula
            char returnVal = eval(removeSpaces(tableFormulas[j]));// evaluate the formula after setting each variables
            if (returnVal=='T') { setPrintColorGreen(); isFormulaContradiction[j]=false; }// print value as T or F
            else if (returnVal=='F') { setPrintColorRed(); isFormulaTautology[j]=false; }
            else { setPrintColorCyan(); cout << "E "; isFormulaError[j]=true; }
            cout << ' ' << returnVal << ' ';
            for (size_t k = 0; k < tableFormulas[j].size()-1; k++) cout << ' ';// print more spaces to make the vertical lines line up
            setPrintColorNone();cout << '|';
        }
        cout << endl;
    }
    printLine();cout << endl;// seperating line
    size_t longestFormula = 0;
    for (size_t i = 0; i < numFormulas; i++) {// find length of longest formula, to be able to line them up
        size_t formulaLen = tableFormulas[i].size();
        if (formulaLen>longestFormula) longestFormula=formulaLen;
    }
    for (size_t i = 0; i < numFormulas; i++) {// print if each expression is a tautology, contradiction, or contingency
        size_t formulaLen = tableFormulas[i].size();
        setPrintColorBlue(); cout << converter.to_bytes(tableFormulas[i].c_str());
        for (size_t i = formulaLen; i < longestFormula; i++) cout << ' ';// line up each colon by adding spaces
        setPrintColorNone(); cout << ": ";
        if (isFormulaError[i]) { setPrintColorRed(); cout << "Error in statement" << endl; setPrintColorNone(); }
        else if (isFormulaTautology[i]) cout << "Tautology (Always true)" << endl;
        else if (isFormulaContradiction[i]) cout << "Contradiction (Always false)" << endl;
        else cout << "Contingency." << endl;
    }
    cout << endl;
    variables.clear();// clear variables list and add last end line
    tableFormulas.clear();
    isFormulaTautology.clear();
    isFormulaContradiction.clear();
    isFormulaError.clear();
}
int main(int argc, char* argv[]) {
    tableFormulas.push_back(L"P+Q");
    tableFormulas.push_back(L"~P*~Q");
    tableFormulas.push_back(L"(P+Q)*(~P*~Q)");
    showTable();
    tableFormulas.push_back(L"P<->Q");
    tableFormulas.push_back(L"~P<->~Q");
    tableFormulas.push_back(L"(P<->Q)->(~P<->~Q)");
    showTable();
    tableFormulas.push_back(L"(P+Q)*(~P^R)");
    tableFormulas.push_back(L"P*R");
    tableFormulas.push_back(L"((P+Q)*(~P^R))===(P*R)");
    showTable();
    tableFormulas.push_back(L"(P->R)->Q");
    tableFormulas.push_back(L"P->(Q->R)");
    tableFormulas.push_back(L"((P->R)->Q)<->(P->(Q->R))");
    showTable();
    return 0;
}
/*  1: negation (not): '¬', '~', '!'
    2: conjunction (and): '⋀', '&', '.', '*'
    3: disjunction (or): '⋁', '|', '+'
    4: exclusive or (xor): '⊕', '^'
    5: implication: '→', '⊃', '->'
    6: iff: '↔', '<->'
    7: equivalence: '≡', '==='   */