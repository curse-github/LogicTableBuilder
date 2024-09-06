#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
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
        std::cout << "\u001b[" << (int)color << 'm';
    }
#endif
int getEndingCharIndex(const std::string& input, const size_t& startIndex, const char& startingChar, const char& endingChar) {// give the input information find the next ending parenthasis, bracket, or brace
    int count = 0;
    for (size_t j = startIndex; j < input.size(); j++)
        if (input[j]==startingChar) count++;
        else if (input[j]==endingChar)
            if (count==0) return j;
            else count--;
    return -1;
}
std::string removeSpaces(std::string input) {
    input.erase(std::remove(input.begin(), input.end(), ' '), input.end());
    return input;
}
std::vector<char> variables;// list of single character variable names
long variableValues = 0;// bitmap for the values of each variable bits 0-25 are 'a'-'z' and bits 26-51 are 'A'-'Z'
std::vector<std::string> tableFormulas;// holds each formula the the user inputs
std::vector<bool> isFormulaTautology;// holds whether each valud alwasy evaluated to true
std::vector<bool> isFormulaContradiction;// holds whether each formula always evaluated to false
std::vector<bool> isFormulaError;// holds whether each formula contained an error and is invalid
char getVar(const char& A) {
    if ((A=='T')||(A=='1')) return 'T';
    else if ((A=='F')||(A=='0')) return 'F';
    else if ((A=='E')||(A>'z')||(A<'A')) return 'E';
    else if (std::find(variables.cbegin(),variables.cend(),A)==variables.cend()) variables.push_back(A);// add the variables to the list if it is not already there
    return ((variableValues&(1<<((A>='a')?(A-'a'):(A-'A'+26))))==0)?'F':'T';
}
void setVar(const char& A, const bool& value) {
    if ((A=='F')||(A=='T')||(A=='0')||(A=='1')||(A>'z')||(A<'A')) return;
    if (value) variableValues|=(1<<((A>='a')?(A-'a'):(A-'A'+26)));
    else variableValues&=~(1<<((A>='a')?(A-'a'):(A-'A'+26)));
}
char eval(std::string input,std::string originalExpression) {
    size_t inputSize = input.size();
    if (inputSize==0) return 'E';
    else if (inputSize==1) return getVar(input[0]);
    size_t parenthasisI = input.find_first_of("([{");
    if (parenthasisI!=std::string::npos) {
        int endingIndex = getEndingCharIndex(input,parenthasisI+1,input[parenthasisI],(input[parenthasisI]=='(')?')':((input[parenthasisI]=='[')?']':'}'));
        if (endingIndex==-1) return 'E';// did not find end
        std::string parenthasisInsides = input.substr(parenthasisI+1,endingIndex-parenthasisI-1);
        if ((originalExpression[0]!='\0')&&((endingIndex-parenthasisI-1)>0)&&(std::find(tableFormulas.cbegin(),tableFormulas.cend(),parenthasisInsides)==tableFormulas.cend()))
            tableFormulas.insert(std::find(tableFormulas.cbegin(),tableFormulas.cend(),originalExpression), parenthasisInsides);
        return eval(input.substr(0,parenthasisI)+eval(parenthasisInsides,parenthasisInsides)+input.substr(endingIndex+1),originalExpression);
    }
    size_t tildeI    = input.find('~');// find negations (~)
    size_t asteriskI = input.find('*');// find conjunctions (*)
    size_t plusI     = input.find('+');// find disjunctions (+)
    size_t carrotI   = input.find('^');// find exclusive or (^)
    size_t dashI     = input.find('-');// find implication (->)
    size_t lessThanI = input.find('<');// find iff/bi-conditional (<->)
    size_t equalI    = input.find('=');// find equivilence/congruency operators (===)
    if (tildeI!=std::string::npos)
        return eval(input.substr(0,tildeI)+((getVar(input[tildeI+1])=='T')?'F':'T')+input.substr(tildeI+2),originalExpression);
    if (asteriskI!=std::string::npos)
        return eval(input.substr(0,asteriskI-1)+(((getVar(input[asteriskI-1])==getVar(input[asteriskI+1]))&&(getVar(input[asteriskI-1])=='T'))?'T':'F')+input.substr(asteriskI+2),originalExpression);
    if (plusI!=std::string::npos)
        return eval(input.substr(0,plusI-1)+(((getVar(input[plusI-1])!=getVar(input[plusI+1]))||(getVar(input[plusI-1])=='T')||(getVar(input[plusI+1])=='T'))?'T':'F')+input.substr(plusI+2),originalExpression);
    if (carrotI!=std::string::npos)
        return eval(input.substr(0,carrotI-1)+((getVar(input[carrotI-1])!=getVar(input[carrotI+1]))?'T':'F')+input.substr(carrotI+2),originalExpression);
    if ((dashI!=std::string::npos)&&(input[dashI-1]!='<')&&(input[dashI+1]=='>'))// check that the character after the '-' is a '>' and the character before is not a '<' and therefor a bi-conditional
        return eval(input.substr(0,dashI-1)+(((getVar(input[dashI-1])!=getVar(input[dashI+2]))&&(getVar(input[dashI-1])=='T'))?'F':'T')+input.substr(dashI+3),originalExpression);
    if ((lessThanI!=std::string::npos)&&(input[lessThanI+1]=='-')&&(input[lessThanI+2]=='>'))// check that the character after '<' is a '-' and then a '>'
        return eval(input.substr(0,lessThanI-1)+((getVar(input[lessThanI-1])==getVar(input[lessThanI+3]))?'T':'F')+input.substr(lessThanI+4),originalExpression);
    if ((equalI!=std::string::npos)&&(input[equalI+1]=='=')&&(input[equalI+2]=='='))// check that the next two characters are '='
        return eval(input.substr(0,equalI-1)+((getVar(input[equalI-1])==getVar(input[equalI+3]))?'T':'F')+input.substr(equalI+4),originalExpression);
    return 'E';// if it reaches the end without getting down to a single character, there is an error in the statement
}

void printLine(const bool& showingVars) {
    if (showingVars) for (size_t i = 0; i < variables.size(); i++) std::cout << "----";
    for (size_t i = 0; i < tableFormulas.size(); i++) for(size_t j = 0; j < tableFormulas[i].size()+3; j++) std::cout << '-';
    std::cout << std::endl;
}
void printFormula(std::string formula) {// print the value as T, F, or E
    setPrintColor(Color::Blue); std::cout << ' ' << formula << ' ';
    setPrintColor(Color::White); std::cout << '|';
}
void printValue(const char& val, const int& numEndingSpaces) {// print the value as T, F, or E
    setPrintColor((val=='T') ? Color::Green : ((val=='F') ? (Color::Red) : (Color::Cyan)));
    std::cout << ' ' << (((val=='T')||(val=='F'))?val:'E') << ' ';
    for (size_t i = 0; i < numEndingSpaces; i++) std::cout << ' ';
    setPrintColor(Color::White); std::cout << '|';
}
void showTable(const char* mainFormula, const bool& showVars=true) {
    tableFormulas.push_back(mainFormula);
    for (size_t i = 0; i < tableFormulas.size(); i++) {// evaluate each formula to get the list of variables in them
        eval(removeSpaces(tableFormulas[i]),tableFormulas[i]);
        isFormulaTautology.push_back(true);// initialize std::vectors containing data on if each expression is always true or always false
        isFormulaContradiction.push_back(true);
        isFormulaError.push_back(false);
    }
    printLine(showVars);
    if (showVars) for (size_t i = 0; i < variables.size(); i++) printFormula(std::string(1,variables[i]));// print table header of each variable and formula
    for (size_t i = 0; i < tableFormulas.size(); i++) printFormula(tableFormulas[i]);
    std::cout << std::endl;
    printLine(showVars);
    for (int i = (1<<variables.size())-1; i >= 0 ; i--) {// print body of the table
        for (short j = variables.size(); j > 0; j--) {
            bool val = (i&(1<<(j-1)))!=0;// get individual bits from the i variable for the T or F for each variable
            setVar(variables[variables.size()-j],val);// set the value
            if (showVars) printValue(val?'T':'F',0);
        }
        for (size_t j = 0; j < tableFormulas.size(); j++) {// print the columns for each formula
            char returnVal = eval(removeSpaces(tableFormulas[j]),tableFormulas[j]);// evaluate the formula after setting each variables
            printValue(returnVal,tableFormulas[j].size()-1);// print value
            if (returnVal=='T') isFormulaContradiction[j]=false;
            else if (returnVal=='F') isFormulaTautology[j]=false;
            else isFormulaError[j]=true;
        }
        std::cout << std::endl;
    }
    printLine(showVars);
    for (size_t i = 0; i < tableFormulas.size(); i++)// print if each expression is a tautology, contradiction, or contingency
        if (isFormulaError[i])              { setPrintColor(Color::Blue); std::cout << std::endl << ' ' << tableFormulas[i]; setPrintColor(Color::White); std::cout << " has an "; setPrintColor(Color::Cyan);  std::cout << "Error";         setPrintColor(Color::White); std::cout << '.'; }
        else if (isFormulaTautology[i])     { setPrintColor(Color::Blue); std::cout << std::endl << ' ' << tableFormulas[i]; setPrintColor(Color::White); std::cout << " is a ";   setPrintColor(Color::Green); std::cout << "Tautology";     setPrintColor(Color::White); std::cout << '.'; }
        else if (isFormulaContradiction[i]) { setPrintColor(Color::Blue); std::cout << std::endl << ' ' << tableFormulas[i]; setPrintColor(Color::White); std::cout << " is a ";   setPrintColor(Color::Red);   std::cout << "Contradiction"; setPrintColor(Color::White); std::cout << '.'; }
        else                                { setPrintColor(Color::Blue); std::cout << std::endl << ' ' << tableFormulas[i]; setPrintColor(Color::White); std::cout << " is a Contingency."; }
    std::cout << std::endl << std::endl;
    variables.clear();// clear variables list and add last end line
    tableFormulas.clear();
    isFormulaTautology.clear();
    isFormulaContradiction.clear();
    isFormulaError.clear();
}

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