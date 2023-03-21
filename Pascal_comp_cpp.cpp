#include <iostream>
#include <fstream>
#include "Lexer/src/Lexer.h"
#include "Parser/src/SParser.h"
#include "Parser/src/Parser.h"

void lex_analys_all(std::string dir) {

    for (int i = 1; std::ifstream(dir + std::to_string(i) + ".in"); i++) {
        std::ofstream fout;
        fout.open(dir + std::to_string(i) + ".out");
        std::vector<Lexeme> lexemes;
        Lexer lexer(dir + std::to_string(i) + ".in");
        lexemes = lexer.getLexems();
        for (auto i : lexemes) {
            fout << i.lex_x << '\t' << i.lex_y << '\t' << i.TypeToString[(int)i.lex_type] << '\t' << i.src_val << '\t' << i.val << '\n';
        }
        fout.close();
    }
}

void sparse_all(std::string dir) {

    for (int i = 1; std::ifstream(dir + std::to_string(i) + ".in"); i++) {
        std::vector<Lexeme> lexemes;
        Lexer lexer(dir + std::to_string(i) + ".in");
        lexemes = lexer.getLexems();
        SParser sparser = SParser(lexemes);
        sparser.printBinTree(dir + std::to_string(i) + ".out");
    }
}

void parse_all(std::string dir) {
    for (int i = 2; std::ifstream(dir + std::to_string(i) + ".in"); i++) {
        std::vector<Lexeme> lexemes;
        Lexer lexer(dir + std::to_string(i) + ".in");
        lexemes = lexer.getLexems();
        Parser parser = Parser(lexemes);
        std::unique_ptr<Node> root;
        try {
           root = parser.analyze();
        }
        catch (std::runtime_error e) {
            std::ofstream fout;
            fout.open(dir + std::to_string(i) + ".out", std::ios::app);
            fout << e.what();
        }
        root->print("", dir + std::to_string(i) + ".out");
        
    }
}

inline bool exists_test(const std::string& name) {
    std::ifstream f(name.c_str());
    return f.good();
}

void testing(std::string _module) {
    std::string buffer1 = "";
    std::string buffer2 = "";

    int gcount = 0, acount = 0;

    std::string dir1 = _module + "/tests/1.out";
    std::string dir2 = _module + "/tests/answ/1.txt";

    for (int i = 1; exists_test(dir1) && exists_test(dir2); ++i) {
        std::ifstream in1(_module + "/tests/" + std::to_string(i) + ".out");
        std::ifstream in2(_module + "/tests/answ/" + std::to_string(i) + ".txt");

        dir1 = _module + "/tests/" + std::to_string(i) + ".out";
        dir2 = _module + "/tests/answ/" + std::to_string(i) + ".txt";

        std::string line;
        while (getline(in1, line)) {
            buffer1 += line;
        }
        std::cout << '\n';
        while (getline(in2, line)) {
            buffer2 += line;
        }

        if (buffer1 == buffer2) {
            std::cout << i << " - TRUE";
            gcount++;
        }
        else {
            std::cout << i << " - FALSE";
        }
        std::cout << '\n';
        buffer1 = "";
        buffer2 = "";
        acount = i;
        in1.close(); in2.close();
    }

    std::cout << --gcount << " / " << --acount;
}


int main(int argc, char** argv) {
    std::string dir = "Lexer/tests/";
    std::vector<Lexeme> lexemes;
    Lexer lexer(dir + "2" + ".in");
    lexemes = lexer.getLexems();
    for (auto i : lexemes) {
        std::cout << i.lex_x << '\t' << i.lex_y << '\t' << i.TypeToString[(int)i.lex_type] << '\t' << i.src_val << '\t' << i.val << '\n';
    }
    //parse_all("Parser/tests/");
    //testing("Lexer");

    return 0;
}