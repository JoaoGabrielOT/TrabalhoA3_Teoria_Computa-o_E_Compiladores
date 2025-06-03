#pragma once
#include <vector>
#include <unordered_map>
#include "token.hpp"

class Parser {
public:
    Parser(const std::vector<Token>& tokens);
    bool parse();

private:
    std::vector<std::unordered_map<std::string, std::string>> escopos_;
    std::vector<Token> tokens_;
    size_t pos_;

    Token peek();
    Token advance();
    bool match(TokenTipo tipo);
    void entrarEscopo();
    void sairEscopo();
    bool declararVariavel(const std::string& nome, const std::string& tipo);
    bool variavelDeclarada(const std::string& nome);
    std::string tipoVariavel(const std::string& nome);

    bool parse_comando();
    bool parse_declaracao();
    bool parse_print();
    bool parse_input();
    bool parse_if();
    bool parse_while();
    bool parse_for();
    bool parse_func();

    void erro(const std::string& msg);
};