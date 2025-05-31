// lexer.hpp
#pragma once
#include <string>
#include "token.hpp"

class Lexer {
public:
    Lexer(const std::string& texto);
    Token proximo_token();

private:
    std::string texto_;
    size_t pos_;
    char atual_;

    void avanca();
    void pula_espaco();
    Token identifica_token();
    Token identifica_identificador_ou_palavra_chave();
    Token identifica_numero();
    Token identifica_texto();
};