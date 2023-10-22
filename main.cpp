#include <iostream>
#include <vector>
#include <regex>
#include <map>
#include <set>
#include <cctype>  // for isspace()
#include <fstream>

using namespace std;

enum TokenType {
    COMMENT,
    STRING_CONST,

    STRING_ERROR, 
    FLOATING_POINT,
    HEXADECIMAL,
    DECIMAL_NUMBER,
    
    CHAR_CONST,
    PREPROCESSOR_DIR, // Not common in PHP, but we'll keep it for now
    
    RESERVED_WORD,
    OPERATOR,
    PUNCTUATION,
    IDENTIFIER,
    ERROR
};
const char* TokenTypeStr[] = {
    "COMMENT",
    "STRING_CONST",
    "STRING_ERROR",
    "FLOATING_POINT",
    "HEXADECIMAL",
    "DECIMAL_NUMBER",
    "CHAR_CONST",
    "PREPROCESSOR_DIR",
    "RESERVED_WORD",
    "OPERATOR",
    "PUNCTUATION",
    "IDENTIFIER",
    "ERROR"
};
// Map of token types to their regex patterns
const int ERROR_ROKEN = 2;
map<TokenType, regex> tokenPatterns = {
    //{COMMENT, regex(R"(//.*)")},
    {COMMENT, regex(R"(//.*|/\*(.|\n)*?\*/)")},
    {STRING_CONST, regex(R"((?:"(?:[^"\\]|\\.)*")|(?:'(?:[^'\\]|\\.)*'))")},
    {STRING_ERROR, regex(R"((?:"[^"]*')|(?:'[^']*"))")},
    {PREPROCESSOR_DIR, regex(R"(\<\?php|\?\>)")},

    {PUNCTUATION, regex(R"([,;:\(\)\{\}\$])")},

    {FLOATING_POINT, regex(R"(\b\d+\.\d+\b)")},
    {HEXADECIMAL, regex(R"(\b0x[0-9a-fA-F]+\b)")},
    {DECIMAL_NUMBER, regex(R"(\b[0-9]+\b)")}, // wrong detects "443434"

    //{STRING_CONST, regex(R"((?:\bdefine\s*\(\s*["']([A-Z_][A-Z0-9_]*)["']\s*,)|(?:\bconst\s+([A-Z_][A-Z0-9_]*)\s*=))")},
    {RESERVED_WORD, regex(R"(\b(echo|if|else|while|for|function|class|public|private|protected|return|true|false|null)\b)")},
    {OPERATOR, regex(R"(\+|\-|\*|/|==|!=|&&|\|\||>|<|={1})")},
    {PUNCTUATION, regex(R"([,;:\(\)\{\}\$])")},
    {IDENTIFIER, regex(R"(\b[a-zA-Z_][a-zA-Z0-9_]*\b)")}
};

// Store the tokens here
map<TokenType, set<string>> tokenTables;
string out_str;

void lexicalAnalysis(string sourceCode) {
    string copySourceCode = sourceCode;
    for(auto& pair: tokenPatterns) {
        auto tokenType = pair.first;
        auto pattern = pair.second;

        smatch match;

        string::const_iterator searchStart(sourceCode.cbegin());
        int start_pos = 0;
        while(regex_search(searchStart, sourceCode.cend(), match, pattern)) {
            int start_idx = match.position();
            //bool to_add = true;
            bool to_add = true;
            for (int i = start_idx; i < start_idx + match[0].length(); i++){
                int idx = start_pos + i;
                if (copySourceCode[idx] == '_' || copySourceCode[idx] == '|'){
                    searchStart = searchStart + 1;
                    start_pos++;
                    to_add = false;
                    break;
                } 
            }
            if (to_add == false) continue;
            for (int i = start_idx; i < start_idx + match[0].length(); i++){
                int idx = start_pos + i;
                if (isspace(copySourceCode[idx])) continue;
                char to_fill = '_';
                if(tokenType == STRING_ERROR) to_fill = '|';
                copySourceCode[idx] = to_fill;
                //sourceCode[idx] = '_';
            }
            //cout << copySourceCode << endl;
            string token = match[0];
            tokenTables[tokenType].insert(token);
            start_pos += match.suffix().first - searchStart;
            searchStart = match.suffix().first;
        }
        //sourceCode = copySourceCode;
    }
    for (int i = 0; i < copySourceCode.length(); i++){
        if (copySourceCode[i] == '|') copySourceCode[i] = sourceCode[i];
    }
    out_str = copySourceCode;
    //cout << "-------------------------";
    //cout << copySourceCode << endl;
    //cout << "-------------------------";
}

void displayTokens(string code) {
    cout << code << endl;
    cout << "-------------------------" << endl;
    cout << out_str << endl;
    cout << "-------------------------" << endl;
    for(auto& pair: tokenTables) {
        auto tokenType = pair.first;
        auto tokens = pair.second;

        cout << "Token Type: " << TokenTypeStr[tokenType] << endl;
        for(auto& token: tokens) {
            cout << token << endl;
        }
        cout << "---------------------------" << endl;
    }
}

void displayTokensToFile(string code, const string& filename) {
    ofstream file(filename);  // Open the file for writing

    if (!file.is_open()) {
        cerr << "Failed to open the file for writing!" << endl;
        return;
    }

    file << code << endl;
    file << "-------------------------" << endl;
    file << out_str << endl;  // Assuming out_str is a global variable or passed as a parameter
    file << "-------------------------" << endl;

    for (auto& pair : tokenTables) {
        auto tokenType = pair.first;
        auto tokens = pair.second;

        file << "Token Type: " << TokenTypeStr[tokenType] << endl;  // Assuming TokenTypeStr is a global map or passed as a parameter
        for (auto& token : tokens) {
            file << token << endl;
        }
        file << "---------------------------" << endl;
    }

    file.close();  // Close the file after writing
}

int main() {
    string sourceCode;
    
    // Replace with the path to your .txt file
    ifstream file("code.txt");  

    if (!file.is_open()) {
        cerr << "Failed to open the file!" << endl;
        return 1;
    }

    // Read the file content into sourceCode
    string line;
    while (getline(file, line)) {
        sourceCode += line + '\n';
    }

    file.close();

    
    lexicalAnalysis(sourceCode);
    displayTokens(sourceCode);
    displayTokensToFile(sourceCode, "output.txt");

    return 0;
}




