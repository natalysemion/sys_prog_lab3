#include <iostream>
#include <fstream>
#include <string>
#include <cstring>
#include <regex>
#include <vector>

using namespace std;

const int MAX_FILENAME_LENGTH = 100;
const string test_directory = "Tests/";
const string result_directory = "Results/output";

struct Token {
    regex pattern;
    string name;
};

string tokenize(string program) {
        vector<Token> tokens = {
            {regex(R"(('.*))"), "Comment"},
            {regex(R"(("(?:[^"]|"")*"))"), "String"},
            {regex(R"(\s)"), "Whitespace"},
            {regex(
                R"((#Const\s)"
                R"(|#ExternalSource|#End ExternalSource)"
                R"(|#If .* Then[\s\S]+#End If)"
                R"(|#Region[\s\S]|#End Region)"
                R"(|#(Disable|Enable).*))"), "Directive"},
            {regex(R"((AddHandler|AddressOf|Alias|And|AndAlso|As|Boolean|ByRef|Byte)"
                R"(|ByVal|Call|Case|Catch|CBool|CByte|CChar|CDate|CDbl|CDec|Char)"
                        R"(|CInt|Class|CLng|CObj|Const|Continue|CSByte|CShort|CSng|CStr)"
                        R"(|CType|CUInt|CULng|CUShort|Date|Decimal|Declare|Default)"
                        R"(|Delegate|Dim|DirectCast|Do|Double|Each|Else|ElseIf|End|EndIf|Float)"
                        R"(|Enum|Erase|Error|Event|Exit|False|Finally|For|For|Each…Next)"
                        R"(|Friend|Function|Get|GetType|GetXMLNamespace|Global|GoSub|GoTo)"
                        R"(|Handles|If|Implements|Imports|In|Inherits|Integer|Interface|Is)"
                        R"(|IsNot|Let|Lib|Like|Long|Loop|Me|Mod|Module|MustInherit)"
                        R"(|MustOverride|MyBase|MyClass|Namespace|Narrowing|New|Next|Not)"
                        R"(|Nothing|NotInheritable|NotOverridable|Object|Of|On|Operator)"
                        R"(|Option|Optional|Or|OrElse|Out|Overloads|Overridable|Overrides)"
                        R"(|ParamArray|Partial|Private|Property|Protected|Public)"
                        R"(|RaiseEvent|ReadOnly|ReDim|REM|RemoveHandler|Resume|Return)"
                        R"(|SByte|Select|Set|Shadows|Shared|Short|Single|Static|Step|Stop)"
                        R"(|String|Structure|Sub|SyncLock|Then|Throw|To|True|Try|TryCast)"
                        R"(|TypeOf…Is|UInteger|ULong|UShort|Using|Variant|Wend|When|While)"
                        R"(|Widening|With|WithEvents|WriteOnly|Xor|#Else)(?=\s))"), "Keyword"},

            {regex("0x[0-9A-Fa-f]+"), "HexNumber"},
            {regex("0b[01]+"), "BinNumber"},
            {regex("[+-]?([0-9]*[.])?[0-9]+([eE][+-]?[0-9]+)?"), "Number"},
       /*     {regex(R"((?:^|\W)()"
    // floats
    R"([+-]?\d+\.\d+([eE][-+]?\d+)?)"
    R"(|[-+]?(?:[1-9]\d*|0)[eE][-+]?\d+)"
    // hexadecimal
    R"(|[+-]?&H[0-9a-fA-F]+)"
    // decimals
    R"(|[-+]?(?:[1-9]\d*|0)(?![[:digit:]]))"
    R"())"), "Number"},
*/
            {regex(R"(((?:_\w|[[:alpha:]])\w*))"), "Identifier"},
            {regex(R"(([;,:{}()_\[\]'\\."]))"), "Delimiter"},

            {regex(R"((Await)"
            // arithmetic and concatenation operators
            R"(|\^|\+|-|\*|\/|\|Mod|<<|>>)"
            // comparison operators
            R"(|=|<>|<|<=|>|>=|IsNot|Is|Like|TypeOf)"
            // logical and bitwise operators
            R"(|Not|And|AndAlso|Or|OrElse|Xor))"), "Operator"},

        };

    string result = "";
    while (!program.empty()) {
        smatch match;
        bool flag = true;
        for (const auto& token : tokens) {
            if (regex_search(program, match, token.pattern)) {
                if (match.position() == 0) {
                    if (token.name != " " && token.name != "Whitespace") {
                        string new_token =  token.name + ":   " + match.str() + "\n" ;
                        cout << new_token;
                        result += new_token;
                    }
                    program = match.suffix();
                    flag = false;
                    break;
                }
            }
        }


        if (flag) {
            size_t newline_pos = program.find('\n');
            if (newline_pos == string::npos) {
                result += "Uncategorized: " + program + "\n";
                cout <<"Uncategorized: " + program + "\n";
                break;
            } else {
                string uncategorized_text = program.substr(0, newline_pos);
                result += "Uncategorized: " + uncategorized_text + "\n";
                cout << "Uncategorized: " + uncategorized_text + "\n";
                program = program.substr(newline_pos + 1);
            }
        }
    }
    return result;
}



int main() {
    char input_file_path[MAX_FILENAME_LENGTH];
    string file_name;

    ifstream input_file;
    while (!input_file.is_open()) {
        cout << "Enter a file name: ";
        cin >> file_name;
        string file_path = test_directory + file_name;
        input_file.open(file_path.c_str());

        if (!input_file.is_open()) {
            cout << "Couldn't open the input file: " << file_path << endl;
        } else {
            strcpy(input_file_path, file_path.c_str());
        }
    }

    string program(istreambuf_iterator<char>(input_file), {});

    string tokenized = tokenize(program);

    char output_file_path[MAX_FILENAME_LENGTH];
    strcpy(output_file_path, result_directory.c_str());
    strcat(output_file_path, input_file_path + strlen(test_directory.c_str()) + 5);

    ofstream output_file(output_file_path);
    output_file << tokenized;
    cout << "Tokenized program written to the output file: " << output_file_path << endl;

    return 0;
}
