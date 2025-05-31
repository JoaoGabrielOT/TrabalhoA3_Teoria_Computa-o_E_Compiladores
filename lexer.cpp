// lexer.cpp
#include "lexer.hpp"
#include <cctype>

Lexer::Lexer(const std::string& texto) : texto_(texto), pos_(0) {
    atual_ = texto_.empty() ? '\0' : texto_[0];
}

void Lexer::avanca() {
    pos_++;
    if (pos_ < texto_.size()) {
        atual_ = texto_[pos_];
    } else {
        atual_ = '\0';
    }
}

void Lexer::pula_espaco() {
    while (isspace(atual_)) {
        avanca();
    }
}

Token Lexer::proximo_token() {
    pula_espaco();
    if (atual_ == '\0') {
        return {FIM_ARQUIVO, ""};
    }

    if (atual_ == ':') {
        avanca();
        return {DOIS_PONTOS, ":"};
    }
    if (atual_ == '=') {
        avanca();
        return {IGUAL, "="};
    }
    if (atual_ == ';') {
        avanca();
        return {PONTO_E_VIRGULA, ";"};
    }
    if (atual_ == '(') {
        avanca();
        return {ABRE_PARENTESE, "("};
    }
    if (atual_ == ')') {
        avanca();
        return {FECHA_PARENTESE, ")"};
    }
    if (atual_ == '{') {
        avanca();
        return {ABRE_CHAVE, "{"};
    }
    if (atual_ == '}') {
        avanca();
        return {FECHA_CHAVE, "}"};
    }

    if (atual_ == '"') {
        return identifica_texto();
    }

    if (isdigit(atual_)) {
        return identifica_numero();
    }

    if (isalpha(atual_) || atual_ == '_') {
        return identifica_identificador_ou_palavra_chave();
    }

    std::string err(1, atual_);
    avanca();
    return {ERRO, err};
}

Token Lexer::identifica_texto() {
    avanca(); // pula aspas "
    std::string valor;
    while (atual_ != '"' && atual_ != '\0') {
        valor += atual_;
        avanca();
    }
    if (atual_ == '"') {
        avanca();
        return {TEXTO, valor};
    }
    return {ERRO, "String não fechada"};
}

Token Lexer::identifica_numero() {
    std::string valor;
    while (isdigit(atual_)) {
        valor += atual_;
        avanca();
    }
    // Para simplificar, não considera float aqui, pode melhorar depois.
    return {NUMERO, valor};
}

Token Lexer::identifica_identificador_ou_palavra_chave() {
    std::string valor;
    while (isalnum(atual_) || atual_ == '_') {
        valor += atual_;
        avanca();
    }

    if (valor == "var") return {VAR, valor};
    if (valor == "print") return {PRINT, valor};
    if (valor == "input") return {INPUT, valor};
    if (valor == "if") return {IF, valor};
    if (valor == "else") return {ELSE, valor};
    if (valor == "while") return {WHILE, valor};
    if (valor == "for") return {FOR, valor};
    if (valor == "func") return {FUNC, valor};
    if (valor == "int") return {INT, valor};
    if (valor == "float") return {FLOAT, valor};
    if (valor == "char") return {CHAR, valor};
    if (valor == "bool") return {BOOL, valor};
    if (valor == "string") return {STRING, valor};
    if (valor == "void") return {VOID, valor};

    return {IDENTIFICADOR, valor};
}