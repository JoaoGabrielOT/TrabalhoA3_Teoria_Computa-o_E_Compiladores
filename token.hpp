// token.hpp
#pragma once
#include <string>

enum TokenTipo {
    VAR, PRINT, INPUT, IF, ELSE, WHILE, FOR, FUNC,
    INT, FLOAT, CHAR, BOOL, STRING, VOID,
    IDENTIFICADOR, NUMERO, TEXTO,
    DOIS_PONTOS, IGUAL, PONTO_E_VIRGULA,
    ABRE_PARENTESE, FECHA_PARENTESE,
    ABRE_CHAVE, FECHA_CHAVE,
    FIM_ARQUIVO,
    ERRO
};

struct Token {
    TokenTipo tipo;
    std::string valor;
};