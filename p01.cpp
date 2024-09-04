#include <iostream>
#include <string>
#include <locale>
#include <codecvt>
#include <algorithm>
#include <cmath>
#include <vector>
#ifdef _WIN32
    #include <windows.h>
#endif
using std::cout, std::endl, std::vector, std::wstring, std::wstring_convert, std::codecvt_utf8;
#pragma region helping functions
enum class Color {Red=0,Green=1,Blue=2,Cyan=3,White=4};
void setPrintColor(Color color) {// function that sets the console output color based on the enum passed in
#ifdef _WIN32
    SetConsoleOutputCP(CP_UTF8);
    const int codes[5] = {FOREGROUND_RED, FOREGROUND_GREEN, FOREGROUND_BLUE, FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY, FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY};
    SetConsoleTextAttribute(GetStdHandle((DWORD)-11),codes[(int)color]);
#else
    const char* codes[5] = {"\033[31m","\033[32m","\033[34m","\033[36m","\033[37m"};
    cout << codes[(int)color];
#endif
}
wstring removeSpaces(wstring input) {// function to return a copy of the input string without strings
    input.erase(std::remove(input.begin(), input.end(),L' '), input.end());
    return input;
}
int getEndingCharIndex(wstring input, size_t size, size_t startIndex, wchar_t startingChar, wchar_t endingChar) {// give the input information find the next ending parenthasis, bracket, or brace
    int count = 0;
    for (size_t j = startIndex; j < size; j++) {
        if (input[j]==startingChar) count++;
        else if (input[j]==endingChar) {
            if (count==0) return j;
            else count--;
        }
    }
    return -1;
}
#pragma endregion helping functions

vector<char> variables;// list of single character variable names
long variableValues = 0;// bitmap for the values of each variable bits 0-25 are 'a'-'z' and bits 26-51 are 'A'-'Z'
vector<wstring> tableFormulas;// holds each formula the the user inputs
vector<bool> isFormulaTautology;// holds whether each valud alwasy evaluated to true
vector<bool> isFormulaContradiction;// holds whether each formula always evaluated to false
vector<bool> isFormulaError;// holds wether each formula contained an error and is invalid
wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;// converts from std::wstring to std::string or wchar_t to char or whatever so you can print it

#pragma region evaluation functions
char getVar(const char& A) {
    if ((A=='T')||(A=='1')) return 'T'; else if ((A=='F')||(A=='0')) return 'F';
    else if (std::find(variables.cbegin(),variables.cend(),A)==variables.cend()) variables.push_back(A);// add the variables to the list if it is not already there
    if ((A>='A')&&(A<='Z')) return ((variableValues&(1<<(A-'A'+26)))==0)?'F':'T';
    else if ((A>='a')&&(A<='z')) return ((variableValues&(1<<(A-'a')))==0)?'F':'T';
    return 'E';
}
void setVar(const char& A, const bool& value) {
    if ((A=='T')||(A=='F')||(A=='0')||(A=='1')) return;
    if ((A>='A')&&(A<='Z')) {
        if (value) variableValues|=(1<<(A-'A'+26)); else variableValues&=~(1<<(A-'A'+26));
    } else if ((A>='a')&&(A<='z')) {
        if (value) variableValues|=(1<<(A-'a')); else variableValues&=~(1<<(A-'a'));
    }
}
char eval(const wstring input) {
    size_t inputSize = input.size();
    if (inputSize==1) return getVar(input[0]);
    for (size_t i = 0; i < inputSize; i++) {
        if ((input[i]=='(')||(input[i]=='[')||(input[i]=='{')) {
            int endingIndex = getEndingCharIndex(input,inputSize,i+1,input[i],(input[i]==L'(')?L')':((input[i]==L'[')?L']':L'}'));
            if (endingIndex==-1) return 'E';// did not find end
            wstring tmpStr = input.substr(0,i);
            tmpStr.push_back((wchar_t)eval(input.substr(i+1,endingIndex-i-1)));
            return eval(tmpStr+input.substr(endingIndex+1,inputSize-(i-endingIndex)-1));
        }
    }
    for (size_t i = 0; i < inputSize; i++) {// parse negations
        if ((input[i]==L'¬')||(input[i]=='~')||(input[i]=='!')){
            wstring tmpStr = input.substr(0,i);
            tmpStr.push_back((getVar(input[i+1])=='T')?'F':'T');
            return eval(tmpStr+input.substr(i+2,inputSize-i-2));
        }
    }
    for (size_t i = 0; i < inputSize; i++) {// parse conjunctions
        if ((input[i]==L'⋀')||(input[i]=='&')||(input[i]=='.')||(input[i]=='*')){
            wstring tmpStr = input.substr(0,i-1);
            getVar(input[i-1]);getVar(input[i+1]);// make sure variables get registered
            tmpStr.push_back(((getVar(input[i-1])=='T')&&(getVar(input[i+1])=='T'))?'T':'F');
            return eval(tmpStr+input.substr(i+2,inputSize-i-2));
        }
    }
    for (size_t i = 0; i < inputSize; i++) {// parse disjunctions
        if ((input[i]==L'⋁')||(input[i]=='|')||(input[i]=='+')){
            wstring tmpStr = input.substr(0,i-1);
            tmpStr.push_back(((getVar(input[i-1])=='T')||(getVar(input[i+1])=='T'))?'T':'F');
            return eval(tmpStr+input.substr(i+2,inputSize-i-2));
        }
    }
    for (size_t i = 0; i < inputSize; i++) {// parse exclusive or
        if ((input[i]==L'⊕')||(input[i]=='^')){
            wstring tmpStr = input.substr(0,i-1);
            tmpStr.push_back((getVar(input[i-1])!=getVar(input[i+1]))?'T':'F');
            return eval(tmpStr+input.substr(i+2,inputSize-i-2));
        }
    }
    for (size_t i = 0; i < inputSize; i++) {// parse implication
        int len = 0;
        if ((input[i]==L'→')||(input[i]==L'⊃')) len=1;
        else if ((input[i]=='-')&&(input[i-1]!='<')&&(input[i+1]=='>')) len=2;
        else continue;
        getVar(input[i-1]);getVar(input[i+len]);// make sure variables get registered
        wstring tmpStr = input.substr(0,i-1);
        tmpStr.push_back(((getVar(input[i-1])=='T')&&(getVar(input[i+len])=='F'))?'F':'T');
        return eval(tmpStr+input.substr(i+1+len,inputSize-i-1-len));
    }
    for (size_t i = 0; i < inputSize; i++) {// parse iff/bi-conditional
        int len = 0;
        if (input[i]==L'↔') len=1;
        else if ((input[i]=='<')&&(input[i+1]=='-')&&(input[i+2]=='>')) len=3;
        else continue;
        wstring tmpStr = input.substr(0,i-1);
        tmpStr.push_back((getVar(input[i-1])==getVar(input[i+len]))?'T':'F');
        return eval(tmpStr+input.substr(i+1+len,inputSize-i-1-len));
    }
    for (size_t i = 0; i < inputSize; i++) {// parse equivilence/congruency operators
        int len = 0;
        if (input[i]==L'≡') len=1;
        else if ((input[i]=='=')&&(input[i+1]=='=')&&(input[i+2]=='=')) len=3;
        else continue;
        wstring tmpStr = input.substr(0,i-1);
        tmpStr.push_back((getVar(input[i-1])==getVar(input[i+len]))?'T':'F');
        return eval(tmpStr+input.substr(i+1+len,inputSize-i-1-len));
    }
    return 'E';
}
#pragma endregion evaluation functions
#pragma region table printing functions
void printLine(bool showingVars) {
    size_t numFormulas = tableFormulas.size();
    size_t numVariables = variables.size();// print dashes for the width of the variable columns
    cout << '-'; if (showingVars) for (short i = numVariables; i > 0; i--) cout << "----";
    for (size_t i = 0; i < numFormulas; i++) {// print dashes for the width of the formula columns
        cout << "---"; for (short j = 0; j < static_cast<short>(tableFormulas[i].size()); j++) cout << '-';
    }
}
void printFormula(wstring formula) {// print the value as T, F, or E
    setPrintColor(Color::Blue); cout << ' ' << converter.to_bytes(formula); setPrintColor(Color::White);
}
void printValue(char val, int numEndingSpaces) {// print the value as T, F, or E
    if (val=='T') { setPrintColor(Color::Green); cout << " T "; setPrintColor(Color::White); }
    else if (val=='F') { setPrintColor(Color::Red); cout << " F "; setPrintColor(Color::White); }
    else { setPrintColor(Color::Cyan); cout << " E "; setPrintColor(Color::White); }
    for (size_t i = 0; i < numEndingSpaces; i++) cout << ' ';
    cout << '|';
}
void showTable(bool showVars) {
    size_t numFormulas = tableFormulas.size();
    size_t longestFormula = 0;
    for (size_t i = 0; i < numFormulas; i++) {// evaluate each formula to get the list of variables in them
        eval(removeSpaces(tableFormulas[i]));
        isFormulaTautology.push_back(true);// and initialize the vectors containing data on if each expression is always true or always false
        isFormulaContradiction.push_back(true);
        isFormulaError.push_back(false);
        if (tableFormulas[i].size()>longestFormula) longestFormula=tableFormulas[i].size();
    }
    size_t numVariables = variables.size();
    printLine(showVars); cout << endl;// print seperating line
    cout << '|';// print table header of each variable and formula
    if (showVars) for (size_t i = 0; i < numVariables; i++) { printFormula(wstring(1,(wchar_t)variables[i])); cout << " |"; }
    for (size_t i = 0; i < numFormulas; i++) { printFormula(tableFormulas[i]); cout << " |"; }
    cout << endl; printLine(showVars);cout << endl;// print seperating line
    for (int i = (1<<numVariables)-1; i >= 0 ; i--) {// print body of the table
        cout << "|";// print the columns for each variable
        for (short j = numVariables; j > 0; j--) {
            bool val = (i&(1<<(j-1)))!=0;// get individual bits from the i variable for the T or F for each variable
            setVar(variables[numVariables-j],val);// set the value
            if (showVars) printValue(val?'T':'F',0);
        }
        for (size_t j = 0; j < numFormulas; j++) {// print the columns for each formula
            char returnVal = eval(removeSpaces(tableFormulas[j]));// evaluate the formula after setting each variables
            printValue(returnVal,tableFormulas[j].size()-1);// print value
            if (returnVal=='T') isFormulaContradiction[j]=false; else if (returnVal=='F') isFormulaTautology[j]=false; else isFormulaError[j]=true;
        }
        cout << endl;
    }
    printLine(showVars); cout << endl;// seperating line
    for (size_t i = 0; i < numFormulas; i++) {// print if each expression is a tautology, contradiction, or contingency
        printFormula(tableFormulas[i]);
        for (size_t j = tableFormulas[i].size(); j < longestFormula; j++) cout << ' ';// line up each colon by adding spaces
        cout << ": ";
        if (isFormulaError[i]) { setPrintColor(Color::Red); cout << "Error in statement." << endl; setPrintColor(Color::White); }
        else if (isFormulaTautology[i]) { setPrintColor(Color::Green); cout << "Tautology." << endl; setPrintColor(Color::White); }
        else if (isFormulaContradiction[i]) { setPrintColor(Color::Red); cout << "Contradiction." << endl; setPrintColor(Color::White); }
        else cout << "Contingency." << endl;
    }
    cout << endl;
    variables.clear();// clear variables list and add last end line
    tableFormulas.clear();
    isFormulaTautology.clear();
    isFormulaContradiction.clear();
    isFormulaError.clear();
}
#pragma endregion table printing functions
int main(int argc, char* argv[]) {
    //homework problems
    /*tableFormulas.push_back(L"(p+q)*(~p*~q)");
    tableFormulas.push_back(L"(p<->q)->(~p<->~q)");
    tableFormulas.push_back(L"((p+q)*(~p^r))===(p*r)");
    tableFormulas.push_back(L"((p->r)->q)<->(p->(q->r))");*/

    //operations
    tableFormulas.push_back(L"¬P");
    cout << "negation" << endl;
    showTable(true);
    tableFormulas.push_back(L"P⋀ Q");
    cout << "conjunction" << endl;
    showTable(true);
    tableFormulas.push_back(L"P⋁ Q");
    cout << "disjunction" << endl;
    showTable(true);
    tableFormulas.push_back(L"P⊕ Q");
    cout << "exclusive or" << endl;
    showTable(true);
    tableFormulas.push_back(L"P→ Q");
    cout << "conditional" << endl;
    showTable(true);
    tableFormulas.push_back(L"P↔ Q");
    cout << "bi-conditional" << endl;
    showTable(true);
    tableFormulas.push_back(L"P≡ Q");
    cout << "equivilence" << endl;
    showTable(true);
    cout << endl << endl;
    
    // identities
    tableFormulas.push_back(L"p");
    tableFormulas.push_back(L"p→ q");
    tableFormulas.push_back(L"q");
    tableFormulas.push_back(L"((p)⋀ (p→ q))→ (q)");
    cout << "modus ponens" << endl;
    showTable(false);
    
    tableFormulas.push_back(L"¬q");
    tableFormulas.push_back(L"p→ q");
    tableFormulas.push_back(L"¬p");
    tableFormulas.push_back(L"((¬q)⋀ (p→ q))→ (¬p)");
    cout << "modus tollens" << endl;
    showTable(false);

    tableFormulas.push_back(L"p→ q");
    tableFormulas.push_back(L"q→ r");
    tableFormulas.push_back(L"p→ r");
    tableFormulas.push_back(L"((p→ q)⋀ (q→ r))→ (p→ r)");
    cout << "hypothetical syllogism" << endl;
    showTable(false);

    tableFormulas.push_back(L"p⋁ q");
    tableFormulas.push_back(L"¬p");
    tableFormulas.push_back(L"q");
    tableFormulas.push_back(L"((p⋁ q)⋀ (¬p))→ (q)");
    cout << "disjunctive syllogism" << endl;
    showTable(false);

    tableFormulas.push_back(L"p");
    tableFormulas.push_back(L"p⋁ q");
    tableFormulas.push_back(L"(p)→ (p⋁ q)");
    cout << "addition" << endl;
    showTable(false);

    tableFormulas.push_back(L"p⋀ q");
    tableFormulas.push_back(L"p");
    tableFormulas.push_back(L"(p⋀ q)→ (p)");
    cout << "simplification" << endl;
    showTable(false);

    tableFormulas.push_back(L"p⋁ q");
    tableFormulas.push_back(L"¬p⋁ r");
    tableFormulas.push_back(L"p⋁ r");
    tableFormulas.push_back(L"((p⋁ q)⋀ (¬p⋁ r))→ (q⋁ r)");
    showTable(true);
    cout << "resolution" << endl;

    return 0;
}
/*  1: negation (not): '¬', '~', '!'
    2: conjunction (and): '⋀', '&', '.', '*'
    3: disjunction (or): '⋁', '|', '+'
    4: exclusive or (xor): '⊕', '^'
    5: implication: '→', '⊃', '->'
    6: iff: '↔', '<->'
    7: equivalence: '≡', '==='   */