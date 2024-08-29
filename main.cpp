#include "./main.h"
using std::wstring_convert, std::codecvt_utf8, std::wstring;
using std::cout, std::endl;
using std::vector;

wstring removeSpaces(wstring input) {
    input.erase(std::remove(input.begin(), input.end(),L' '), input.end());
    return input;
}

#pragma region variable handling
char values[26] = {
    'F','F','F','F','F','F','F','F','F','F','F','F','F',
    'F','F','F','F','F','F','F','F','F','F','F','F','F'
};
vector<char> variables;
void addVar(const char& var) {
    if ((var=='T') || (var=='F') || (var=='1') || (var=='0')) return;
    if (std::find(variables.cbegin(),variables.cend(),var)==variables.cend()) {
        variables.push_back(var);
    }
}
char getVar(const char& A) {
    if ((A=='T')||(A=='F')) return A;
    if (A=='1') return 'T';
    if (A=='0') return 'F';
    return values[A-'A'];
}
void setVar(const char& A, const bool& value) {
    if (value) values[A-'A']='T';
    else values[A-'A']='F';
}
#pragma endregion variable handling

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
    if ((getVar(A)=='T') && (getVar(B)=='F')) return 'F';
    return 'T';
}
// A ↔ B
char iff(const char& A, const char& B) {
    if (getVar(A)==getVar(B)) return 'T';
    return 'F';
}
#pragma endregion operations

#pragma region operation parsing
// type 1 is negation
// type 2 is conjunction
// type 3 is disjunction
// type 4 is exclusive ot
// type 5 is implication
// type 6 is iff (if and only if)
// type of -1 is an invalid operator
struct operatorType {
    char type;
    char len;
};
// returns the struct defined above with the type and number of chars long the operator is
operatorType getOperator(const wchar_t* input) {
    switch(input[0]) {
        case L'¬': return {1,1};
        case L'~': return {1,1};
        case L'!': return {1,1};

        case L'⋀': return {2,1};
        case L'.': return {2,1};
        case L'&': return {2,1};
        case L'*': return {2,1};

        case L'⋁': return {3,1};
        case L'|': return {3,1};
        case L'+': return {3,1};

        case L'⊕': return {4,1};
        case L'^': return {4,1};

        case L'→': return {5,1};
        case L'⊃': return {5,1};
        case L'-': if (input[1]==L'>') return {5,2}; else return {-1,0};

        case L'↔': return {6,1};
        case L'≡': return {6,1};
        case L'=': if (input[1]==L'='&&input[2]==L'=') return {6,3}; else return {-1,0};
        case L'<': if (input[1]==L'-'&&input[2]==L'>') return {6,3}; else return {-1,0};

        default: return {-1,0};
    }
}
#pragma endregion operation parsing


int getEndingParenthesisIndex(const wchar_t* input,size_t size) {
    short numParenthasis = 0;
    for (size_t i = 1; i < size; i++) {
        if (input[i]=='(') numParenthasis++;
        else if (input[i]==')') {
            if (numParenthasis==0) {
                return i;
            } else numParenthasis--;
        } else continue;
    }
    return -1;
}
int getEndingBracketIndex(const wchar_t* input,size_t size) {
    short numBrackets = 0;
    for (size_t i = 1; i < size; i++) {
        if (input[i]=='[') numBrackets++;
        else if (input[i]==']') {
            if (numBrackets==0) {
                return i;
            } else numBrackets--;
        } else continue;
    }
    return -1;
}
int getEndingBraceIndex(const wchar_t* input,size_t size) {
    short numBraces = 0;
    for (size_t i = 1; i < size; i++) {
        if (input[i]=='{') numBraces++;
        else if (input[i]=='}') {
            if (numBraces==0) {
                return i;
            } else numBraces--;
        } else continue;
    }
    return -1;
}

char eval(const wchar_t* inputStr, size_t size) {
    char A='E';
    char B='E';
    int index=0;
    bool isANegated = false;
    while (true) {
        operatorType operatorStart = getOperator(inputStr+index);
        if (operatorStart.type==1) { isANegated=!isANegated;index++; }
        else break;
    }

    if ((inputStr[index]>='A' && inputStr[index]<='Z') || (inputStr[index]=='0') || (inputStr[index]=='1')) {
        addVar(inputStr[index]);
        A=getVar(inputStr[index]);
        index++;
    } else if ((inputStr[index]=='(')||(inputStr[index]=='[')||(inputStr[index]=='{')) {
        switch(inputStr[index]) {
            case L'(':
                {
                    int endingParenthesisIndex = getEndingParenthesisIndex(inputStr+index,size-index);
                    if (endingParenthesisIndex==-1) return 'E';
                    A=eval(inputStr+index+1,index+endingParenthesisIndex-1);
                    index+=endingParenthesisIndex+1;
                } break;
            case L'[':
                {
                    int endingBracketIndex = getEndingBracketIndex(inputStr+index,size-index);
                    if (endingBracketIndex==-1) return 'E';
                    A=eval(inputStr+index+1,index+endingBracketIndex-1);
                    index+=endingBracketIndex+1;
                } break;
            case L'{':
                {
                    int endingBraceIndex = getEndingBraceIndex(inputStr+index,size-index);
                    if (endingBraceIndex==-1) return 'E';
                    A=eval(inputStr+index+1,index+endingBraceIndex-1);
                    index+=endingBraceIndex+1;
                } break;
            default: return 'E';// should be impossible
        }
    } else return 'E';
    if (isANegated) A=negation(A);
    if (index==size) return A;

    operatorType operatorThing = getOperator(inputStr+index);
    if ((operatorThing.type!=-1)&&(operatorThing.type!=1)) {
        index+=operatorThing.len;
        B=eval(inputStr+index,size-index);
        //wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
        //cout << endl << A << converter.to_bytes((inputStr+index-operatorThing.len)[0]) << ' ' << B << endl;
        //cout << "operator type: " << (int)operatorThing.type << endl;
        switch(operatorThing.type) {
            case 2: return conjunction(A,B);
            case 3: return disjunction(A,B);
            case 4: return exclusiveOr(A,B);
            case 5: return implication(A,B);
            case 6: return iff(A,B);
            default: return 'E';
        }
    } else return 'E';
}
char eval(const wstring input) { return eval(input.c_str(),input.size()); }

vector<wstring> tableFormulas;
vector<bool> isFormulaTautology;
vector<bool> isFormulaContradiction;
vector<bool> isFormulaError;

void printLine() {
    size_t numFormulas = tableFormulas.size();
    size_t numVariables = variables.size();
    // print dashes for the width of the variable columns
    cout << '-'; for (short i = numVariables; i > 0; i--) cout << "----";
    // print dashes for the width of the formula columns
    for (size_t i = 0; i < numFormulas; i++) {
        cout << "---";
        short len = static_cast<short>(tableFormulas[i].size());
        for (short i = 0; i < len; i++) cout << '-';
    }
    cout << endl;
}

void showTable() {
    // used for converting the utf-8 strings to regular, so it can be printed
    wstring_convert<codecvt_utf8<wchar_t>, wchar_t> converter;
    
    // evaluate each formula to get the list of variables in them
    size_t numFormulas = tableFormulas.size();
    for (size_t i = 0; i < numFormulas; i++) {
        eval(removeSpaces(tableFormulas[i]));
        // and initialize the vectors containing data on if each expression is always true or always false
        isFormulaTautology.push_back(true);
        isFormulaContradiction.push_back(true);
        isFormulaError.push_back(false);
    }
    size_t numVariables = variables.size();
    // print seperating line
    printLine();
    // print table header of each variable and formula
    cout << '|';
    for (short i = 0; i < numVariables; i++) {
        setPrintColorBlue(); cout << ' ' << variables[i];
        setPrintColorNone(); cout << " |";
    }
    for (size_t i = 0; i < numFormulas; i++) {
        setPrintColorBlue(); cout << ' ' << converter.to_bytes(tableFormulas[i].c_str());
        setPrintColorNone(); cout << " |";
    }
    cout << endl;
    // print seperating line
    printLine();
    // print body of the table
    for (int i = (1<<numVariables)-1; i >= 0 ; i--) {
        cout << "|";
        // print the columns for each variable
        for (short j = numVariables; j > 0; j--) {
            // get each bit of the i variable to get a true or false value for each variable
            bool val = (i&(1<<(j-1)))!=0;
            // set the value
            setVar(variables[numVariables-j],val);
            // print the value as T or F
            if (val) setPrintColorGreen();else setPrintColorRed(); cout << ' ' << (val?'T':'F'); setPrintColorNone();cout << " |";
        }
        // print the columns for each formula
        for (size_t j = 0; j < numFormulas; j++) {
            // evaluate the formula after setting the values of the variables
            char returnVal = eval(removeSpaces(tableFormulas[j]));
            // print value as T or F
            cout << ' ';
            if (returnVal=='T') { setPrintColorGreen(); cout << "T "; isFormulaContradiction[j]=false; }
            else if (returnVal=='F') { setPrintColorRed(); cout << "F "; isFormulaTautology[j]=false; }
            else { setPrintColorCyan(); cout << "E "; isFormulaError[j]=true; }
            setPrintColorNone();
            // print more spaces to make the vertical lines line up
            for (size_t k = 0; k < tableFormulas[j].size()-1; k++) cout << ' ';
            cout << '|';
        }
        cout << endl;
    }
    // seperating line
    printLine();
    // find length of longest formula, to be able to line them up
    size_t longestFormula = 0;
    for (size_t i = 0; i < numFormulas; i++) {
        size_t formulaLen = tableFormulas[i].size();
        if (formulaLen>longestFormula) longestFormula=formulaLen;
    }
    
    // print if each expression is a tautology, contradiction, or contingency
    for (size_t i = 0; i < numFormulas; i++) {
        size_t formulaLen = tableFormulas[i].size();
        setPrintColorBlue();cout << converter.to_bytes(tableFormulas[i].c_str());
        // line up each colon by adding spaces
        for (size_t i = formulaLen; i < longestFormula; i++) cout << ' ';
        setPrintColorNone(); cout << ": ";
        if (isFormulaError[i]) { setPrintColorRed(); cout << "Error in statement"; }
        else if (isFormulaTautology[i]) cout << "Tautology (Always true)";
        else if (isFormulaContradiction[i]) cout << "Contradiction (Always false)";
        else cout << "Contingency.";
        cout << endl;
    }
    // clear variables list and add last end line
    variables.clear();
    tableFormulas.clear();
    isFormulaTautology.clear();
    isFormulaContradiction.clear();
    isFormulaError.clear();
}

/*
1: negation (not): '¬', '~', '!'
2: conjunction (and): '⋀', '.', '&', '*'
3: disjunction (or): '⋁', '|', '+'
4: exclusive or (xor): '⊕', '^'
5: implication: '→', '⊃', '->'
6: iff: '↔', '≡', '<->', '==='
*/
int main(int argc, char* argv[]) {
    /*tableFormulas.push_back(L"(P+Q)*(~P*~Q)");
    tableFormulas.push_back(L"(P<->Q)->(~P<->~Q)");
    tableFormulas.push_back(L"((P+Q)->(~P^R))===(P*R)");
    tableFormulas.push_back(L"((P->R)->Q)<->(P->(Q->R))");
    showTable();*/

    //tableFormulas.push_back(L"(P⋁ Q)⋀ (¬P⋀ ¬Q)");
    //tableFormulas.push_back(L"(P↔ Q)→ (¬P↔ ¬Q)");
    tableFormulas.push_back(L"P");
    tableFormulas.push_back(L"Q");
    tableFormulas.push_back(L"~P^R");
    tableFormulas.push_back(L"(P|Q)&(~P^R)");
    tableFormulas.push_back(L"((P⋁ Q)⋀ (¬P⊕ R))≡ (P⋀ R)");
    //tableFormulas.push_back(L"((P→ R)→ Q)↔ (P→ (Q→ R))");
    showTable();
    return 0;
}