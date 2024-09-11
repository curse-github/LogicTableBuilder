#include <iostream>
#include <string>
#include <algorithm>
#include <vector>
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
std::vector<int> numTruthCases;// holds how many truth cases each formula has
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
    if (input.size()==1) return getVar(input[0]);
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
        if (input[tildeI+1]=='~')// handle double negatives
            return eval(input.substr(0,tildeI)  +input.substr(tildeI+2),originalExpression);
        else
            return eval(input.substr(0,tildeI)  +((getVar(input[tildeI+1])=='T')?'F':'T')/*------------------------------------------------------------*/+input.substr(tildeI+2)   ,originalExpression);
    if (asteriskI!=std::string::npos)
        return eval(input.substr(0,asteriskI-1) +(((getVar(input[asteriskI-1])==getVar(input[asteriskI+1]))&&(getVar(input[asteriskI-1])=='T'))?'T':'F') +input.substr(asteriskI+2),originalExpression);
    if (plusI!=std::string::npos)
        return eval(input.substr(0,plusI-1)     +(((getVar(input[plusI-1])!=getVar(input[plusI+1]))||(getVar(input[plusI-1])=='T'))?'T':'F')/*---------*/+input.substr(plusI+2),originalExpression);
    if (carrotI!=std::string::npos)
        return eval(input.substr(0,carrotI-1)   +((getVar(input[carrotI-1])!=getVar(input[carrotI+1]))?'T':'F')/*--------------------------------------*/+input.substr(carrotI+2)  ,originalExpression);
    if ((dashI!=std::string::npos)&&(input[dashI-1]!='<')&&(input[dashI+1]=='>'))// check that the character after the '-' is a '>' and the character before is not a '<' and therefor not a bi-conditional
        return eval(input.substr(0,dashI-1)     +(((getVar(input[dashI-1])!=getVar(input[dashI+2]))&&(getVar(input[dashI-1])=='T'))?'F':'T')/*---------*/+input.substr(dashI+3)    ,originalExpression);
    if ((lessThanI!=std::string::npos)&&(input[lessThanI+1]=='-')&&(input[lessThanI+2]=='>'))// check that the character after '<' is a '-' and then a '>'
        return eval(input.substr(0,lessThanI-1) +((getVar(input[lessThanI-1])==getVar(input[lessThanI+3]))?'T':'F')/*----------------------------------*/+input.substr(lessThanI+4),originalExpression);
    if ((equalI!=std::string::npos)&&(input[equalI+1]=='=')&&(input[equalI+2]=='='))// check that the next two characters are '='
        return eval(input.substr(0,equalI-1)    +((getVar(input[equalI-1])==getVar(input[equalI+3]))?'T':'F')/*----------------------------------------*/+input.substr(equalI+4)   ,originalExpression);
    return 'E';// if it reaches the end without getting down to a single character, there is an error in the statement
}
void printLine(const bool& showingVars) {
    if (showingVars)
        std::cout << std::string(variables.size()*4,'-');
    for (size_t i = 0u;i < tableFormulas.size(); i++)
        std::cout << std::string((tableFormulas[i].size()+3),'-');
    std::cout << '-' << std::endl;
}
void printFormula(std::string formula) {// print the value as T, F, or E
    std::cout << '|' << ' ' << formula << ' ';
}
void printValue(const char& val, const int& numEndingSpaces) {// print the value as T, F, or E
    std::cout << '|';
    std::cout << ' ' << ((val=='T')?'1':((val=='F')?'0':'E')) << ' ';
    std::cout << std::string(numEndingSpaces,' ');
}
void showTable(const char* mainFormula, const bool& showVars=true) {
    tableFormulas.push_back(mainFormula);
    eval(removeSpaces(mainFormula),mainFormula);
    numTruthCases=std::vector(tableFormulas.size(),0);
    std::cout << std::endl;
    printLine(showVars);// dash line above table
    for (size_t i = 0; (i < variables.size())&&showVars; i++)
        printFormula(std::string(1,variables[i]));// print table header of each variable and formula
    for (size_t i = 0; i < tableFormulas.size(); i++)
        printFormula(tableFormulas[i]);
    std::cout << '|' << std::endl;
    printLine(showVars);// dash line between vars and values
    for (int i = (1<<variables.size())-1; i >= 0 ; i--) {// print body of the table
        for (short j = variables.size(); j > 0; j--) {
            bool val = (i&(1<<(j-1)))!=0;// get individual bits from the i variable for the T or F for each variable
            setVar(variables[variables.size()-j],val);// set the value
            if (showVars) printValue(val?'T':'F',0);
        }
        for (size_t j = 0; j < tableFormulas.size(); j++) {// print the columns for each formula
            char returnVal = eval(removeSpaces(tableFormulas[j]),tableFormulas[j]);// evaluate the formula after setting each variables
            printValue(returnVal,tableFormulas[j].size()-1);// print value
            if (returnVal=='T') numTruthCases[j]++;
        }
        std::cout << '|' << std::endl;
    }
    printLine(showVars);// dash line at bottom of table
    for (size_t i = 0; i < tableFormulas.size(); i++) {// print if each expression is a tautology, contradiction, or contingency
        std::cout << tableFormulas[i] << " has " << numTruthCases[i] << " truth cases and ";
        if (numTruthCases[i]==(1<<variables.size())) { std::cout << "is a Tautology"; }
        else if (numTruthCases[i]==0)                { std::cout << "is a Contradiction"; }
        else                                         { std::cout << "is a Contingency"; }
        std::cout << '.' << std::endl;
    }
    variables.clear();// clear variables list and add last end line
    tableFormulas.clear();
    numTruthCases.clear();
}
int main() {
    showTable("(p+q) * (~p*~q)",true);
    showTable("(p<->q) -> (~p<->~q)",true);
    showTable("(p+q)*(~p^r) === (p*r)",true);
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