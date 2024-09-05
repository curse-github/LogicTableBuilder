#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
using std::cout, std::endl, std::vector, std::string, std::remove;
#pragma region helping functions
#ifdef _WIN32
#include <windows.h>
enum class Color {Red=FOREGROUND_RED,Green=FOREGROUND_GREEN,Blue=FOREGROUND_BLUE,Cyan=FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY,White=FOREGROUND_RED|FOREGROUND_GREEN|FOREGROUND_BLUE|FOREGROUND_INTENSITY};
void setPrintColor(const Color& color) {// function that sets the console output color based on the enum passed in
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleTextAttribute(GetStdHandle((DWORD)-11),(int)color);
}
#else
enum class Color {Red=31,Green=32,Blue=34,Cyan=36,White=0};
void setPrintColor(const Color& color) {// function that sets the console output color based on the enum passed in
    cout << "\u001b[" << (int)color << 'm';
}
#endif
int getEndingCharIndex(const string& input, const size_t& startIndex, const char& startingChar, const char& endingChar) {// give the input information find the next ending parenthasis, bracket, or brace
    int count = 0;
    for (size_t j = startIndex; j < input.size(); j++)
        if (input[j]==startingChar) count++;
        else if (input[j]==endingChar)
            if (count==0) return j;
            else count--;
    return -1;
}
string removeSpaces(string input) {
    input.erase(remove(input.begin(), input.end(), ' '), input.end());
    return input;
}
#pragma endregion helping functions
vector<char> variables;// list of single character variable names
long variableValues = 0;// bitmap for the values of each variable bits 0-25 are 'a'-'z' and bits 26-51 are 'A'-'Z'
vector<string> tableFormulas;// holds each formula the the user inputs
vector<bool> isFormulaTautology;// holds whether each valud alwasy evaluated to true
vector<bool> isFormulaContradiction;// holds whether each formula always evaluated to false
vector<bool> isFormulaError;// holds whether each formula contained an error and is invalid
#pragma region evaluation functions
char getVar(const char& A) {
    if ((A=='T')||(A=='1')) return 'T'; else if ((A=='F')||(A=='0')) return 'F'; else if (A=='E') return 'E';
    else if (std::find(variables.cbegin(),variables.cend(),A)==variables.cend()) variables.push_back(A);// add the variables to the list if it is not already there
    if ((A>='a')&&(A<='z')) return ((variableValues&(1<<(A-'a')))==0)?'F':'T';
    else if ((A>='A')&&(A<='Z')&&(A!='E')) return ((variableValues&(1<<(A-'A'+26)))==0)?'F':'T';
    else return 'E';
}
void setVar(const char& A, const bool& value) {
    if ((A=='F')||(A=='T')||(A=='0')||(A=='1')) return;
    if ((A>='a')&&(A<='z')) { if (value) variableValues|=(1<<(A-'a')); else variableValues&=~(1<<(A-'a')); }
    else if ((A>='A')&&(A<='Z')) { if (value) variableValues|=(1<<(A-'A'+26)); else variableValues&=~(1<<(A-'A'+26)); }
}
string substrCharSubstr(string input, const size_t& start1, const size_t& len1, const char& chr, const size_t& start2, const size_t& len2) {
    return input.substr(start1,len1)+chr+input.substr(start2,len2);
}
char eval(string input,string originalExpression) {
    size_t inputSize = input.size();
    if (inputSize==0) return 'E';
    else if (inputSize==1) return getVar(input[0]);
    for (size_t i = 0; i < inputSize; i++)
        if ((input[i]=='(')||(input[i]=='[')||(input[i]=='{')) {
            int endingIndex = getEndingCharIndex(input,i+1,input[i],(input[i]=='(')?')':((input[i]=='[')?']':'}'));
            if (endingIndex==-1) return 'E';// did not find end
            string parenthasisInsides = input.substr(i+1,endingIndex-i-1);
            if ((originalExpression[0]!='\0')&&((endingIndex-i-1)>0)&&(std::find(tableFormulas.cbegin(),tableFormulas.cend(),parenthasisInsides)==tableFormulas.cend()))
                tableFormulas.insert(std::find(tableFormulas.cbegin(),tableFormulas.cend(),originalExpression), parenthasisInsides);
            return eval(substrCharSubstr(input,0,i,eval(parenthasisInsides,parenthasisInsides),endingIndex+1,inputSize-(i-endingIndex)-1),originalExpression);
        }
    for (size_t i = 0; i < inputSize; i++)// parse negations (~)
        if (input[i]=='~')
            return eval(substrCharSubstr(input,0,i,((getVar(input[i+1])=='T')?'F':'T'),i+2,inputSize-i-2),"");
    for (size_t i = 0; i < inputSize; i++)// parse conjunctions (*)
        if (input[i]=='*')
            return eval(substrCharSubstr(input,0,i-1,(((getVar(input[i-1])==getVar(input[i+1]))&&(getVar(input[i-1])=='T'))?'T':'F'),i+2,inputSize-i-2),"");
    for (size_t i = 0; i < inputSize; i++)// parse disjunctions (+)
        if (input[i]=='+')
            return eval(substrCharSubstr(input,0,i-1,(((getVar(input[i-1])=='T')||(getVar(input[i+1])=='T'))?'T':'F'),i+2,inputSize-i-2),"");
    for (size_t i = 0; i < inputSize; i++)// parse exclusive or (^)
        if (input[i]=='^')
            return eval(substrCharSubstr(input,0,i-1,((getVar(input[i-1])!=getVar(input[i+1]))?'T':'F'),i+2,inputSize-i-2),"");
    for (size_t i = 0; i < inputSize; i++)// parse implication (->)
        if ((input[i]=='-')&&(input[i-1]!='<')&&(input[i+1]=='>'))
            return eval(substrCharSubstr(input,0,i-1,(((getVar(input[i-1])!=getVar(input[i+2]))&&(getVar(input[i-1])=='T'))?'F':'T'),i+3,inputSize-i-3),"");
    for (size_t i = 0; i < inputSize; i++)// parse iff/bi-conditional (<->)
        if ((input[i]=='<')&&(input[i+1]=='-')&&(input[i+2]=='>'))
            return eval(substrCharSubstr(input,0,i-1,((getVar(input[i-1])==getVar(input[i+3]))?'T':'F'),i+4,inputSize-i-4),"");
    for (size_t i = 0; i < inputSize; i++)// parse equivilence/congruency operators (===)
        if ((input[i]=='=')&&(input[i+1]=='=')&&(input[i+2]=='='))
            return eval(substrCharSubstr(input,0,i-1,((getVar(input[i-1])==getVar(input[i+3]))?'T':'F'),i+4,inputSize-i-4),"");
    return 'E';// if it reaches the end without getting down to a single character, there is an error in the statement
}
#pragma endregion evaluation functions
#pragma region table printing functions
void printFormula(string formula) {// print the value as T, F, or E
    setPrintColor(Color::Blue); cout << ' ' << formula << ' ';
    setPrintColor(Color::White); cout << '|';
}
void printValue(char val, int numEndingSpaces) {// print the value as T, F, or E
    setPrintColor((val=='T') ? Color::Green : ((val=='F') ? (Color::Red) : (Color::Cyan)));
    cout << ' ' << (((val=='T')||(val=='F'))?val:'E') << ' ';
    for (size_t i = 0; i < numEndingSpaces; i++) cout << ' ';
    setPrintColor(Color::White); cout << '|';
}
void showTable(const char* mainFormula, bool showVars=true) {
    tableFormulas.push_back(mainFormula);
    for (size_t i = 0; i < tableFormulas.size(); i++) {// evaluate each formula to get the list of variables in them
        eval(removeSpaces(tableFormulas[i]),tableFormulas[i]);
        isFormulaTautology.push_back(true); isFormulaContradiction.push_back(true); isFormulaError.push_back(false);// initialize vectors containing data on if each expression is always true or always false
    }
    size_t numFormulas = tableFormulas.size();
    size_t numVariables = variables.size();
    if (showVars) for (size_t i = 0; i < numVariables; i++) printFormula(string(1,variables[i]));// print table header of each variable and formula
    for (size_t i = 0; i < numFormulas; i++) printFormula(tableFormulas[i]);
    for (int i = (1<<numVariables)-1; i >= 0 ; i--) {// print body of the table
        cout << endl;
        for (short j = numVariables; j > 0; j--) {
            bool val = (i&(1<<(j-1)))!=0;// get individual bits from the i variable for the T or F for each variable
            setVar(variables[numVariables-j],val);// set the value
            if (showVars) printValue(val?'T':'F',0);
        }
        for (size_t j = 0; j < numFormulas; j++) {// print the columns for each formula
            char returnVal = eval(removeSpaces(tableFormulas[j]),tableFormulas[j]);// evaluate the formula after setting each variables
            printValue(returnVal,tableFormulas[j].size()-1);// print value
            if (returnVal=='T') isFormulaContradiction[j]=false; else if (returnVal=='F') isFormulaTautology[j]=false; else isFormulaError[j]=true;
        }
    }
    for (size_t i = 0; i < numFormulas; i++) {// print if each expression is a tautology, contradiction, or contingency
        cout << endl;
        setPrintColor(Color::Blue); cout << ' ' << tableFormulas[i] << ' '; setPrintColor(Color::White);
        if (isFormulaError[i]) { cout << "has an "; setPrintColor(Color::Cyan); cout << "Error"; setPrintColor(Color::White); cout << '.'; }
        else if (isFormulaTautology[i]) { cout << "is a "; setPrintColor(Color::Green); cout << "Tautology"; setPrintColor(Color::White); cout << '.'; }
        else if (isFormulaContradiction[i]) { cout << "is a "; setPrintColor(Color::Red); cout << "Contradiction"; setPrintColor(Color::White); cout << '.'; }
        else cout << "is a Contingency.";
    }
    cout << endl << endl;
    variables.clear();// clear variables list and add last end line
    tableFormulas.clear();
    isFormulaTautology.clear();
    isFormulaContradiction.clear();
    isFormulaError.clear();
}
#pragma endregion table printing functions
int main() {
    showTable("(p+q) * (~p*~q)",true);
    showTable("(p<->q) -> (~p<->~q)",true);
    showTable("((p+q)*(~p^r)) === (p*r)",true);
    showTable("[(p->r)->q] <-> [p->(q->r)]",true);
    return 0;
}
/*  1: negation (not): '~'
    2: conjunction (and): '*'
    3: disjunction (or): '+'
    4: exclusive or (xor): '^'
    5: implication: '->'
    6: iff: '<->'
    7: equivalence: '==='
*/