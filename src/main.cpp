#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <cctype>
#include <algorithm>
#include <optional>

using namespace std;

enum class TokenType {
    CYA,
    NUMBER
};

struct Token {
    TokenType type;
    optional<string> value;
};

vector<Token> tokenize(ifstream& file) {
    vector<Token> tokens;
    string line;
    while (getline(file, line)) {
         istringstream stream(line);
        string word;
        while (stream >> word) {
            if (word == "cya") {
                tokens.push_back({TokenType::CYA, {}});
            } else if (all_of(word.begin(), word.end(), ::isdigit)) {
                tokens.push_back({TokenType::NUMBER, word});
            }
        }
    }

    return tokens;
}

string tokens_to_asm(const vector<Token>& tokens) {
    stringstream output;
    output << "global _start\n_start:\n";
    for (int i = 0; i < tokens.size(); i++){
        const Token& token = tokens[i];
        if (token.type == TokenType::CYA) {
            if (tokens.size() <= i + 1 || tokens[i + 1].type != TokenType::NUMBER) return "err";

            output << "mov rax, 60\n";
            output << "mov rdi, " << *tokens[i + 1].value << "\n";
            output << "syscall";
        }
    }
    return output.str();
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " <file_path>" << endl;
        return EXIT_FAILURE;
    }

    ifstream file(argv[1]);
    if (!file) {
        cerr << "Error: Could not open the file." << endl;
        return EXIT_FAILURE;
    }

    vector<Token> tokens = tokenize(file);
    ofstream asm_file("build/out.asm");
    if (!asm_file) {
        cerr << "Error: Could not create out.asm file." << endl;
        return EXIT_FAILURE;
    }

    asm_file << tokens_to_asm(tokens);
    asm_file.close();

    system("nasm -felf64 build/out.asm");
    system("ld -o out build/out.o");

    return EXIT_SUCCESS;
}
