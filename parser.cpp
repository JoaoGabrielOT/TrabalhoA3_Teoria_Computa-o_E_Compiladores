// parser.cpp
#include "parser.hpp"
#include <iostream>

Parser::Parser(const std::vector<Token>& tokens) : tokens_(tokens), pos_(0) {}

Token Parser::peek() {
    if (pos_ < tokens_.size()) {
        return tokens_[pos_];
    }
    return {FIM_ARQUIVO, ""};
}

Token Parser::advance() {
    if (pos_ < tokens_.size()) {
        return tokens_[pos_++];
    }
    return {FIM_ARQUIVO, ""};
}

bool Parser::match(TokenTipo tipo) {
    if (peek().tipo == tipo) {
        advance();
        return true;
    }
    return false;
}

void Parser::erro(const std::string& msg) {
    std::cerr << "Erro de sintaxe: " << msg << std::endl;
}

bool Parser::parse() {
    while (peek().tipo != FIM_ARQUIVO) {
        if (!parse_comando()) {
            return false;
        }
    }
    return true;
}

bool Parser::parse_comando() {
    Token t = peek();

    switch (t.tipo) {
        case VAR:
            return parse_declaracao();
        case PRINT:
            return parse_print();
        case INPUT:
            return parse_input();
        case IF:
            return parse_if();
        case WHILE:
            return parse_while();
        case FOR:
            return parse_for();
        case FUNC:
            return parse_func();
        default:
            erro("Comando inesperado: " + t.valor);
            return false;
    }
}

// var <id> : <tipo> = <valor opcional> ;
bool Parser::parse_declaracao() {
    if (!match(VAR)) {
        erro("Esperado 'var' no início da declaração");
        return false;
    }
    Token id = peek();
    if (!match(IDENTIFICADOR)) {
        erro("Esperado identificador depois de 'var'");
        return false;
    }
    if (!match(DOIS_PONTOS)) {
        erro("Esperado ':' depois do identificador");
        return false;
    }
    Token tipo = peek();
    if (!(match(INT) || match(FLOAT) || match(CHAR) || match(BOOL) || match(STRING))) {
        erro("Esperado tipo de dado (int, float, char, bool, string)");
        return false;
    }
    bool tem_valor = false;
    if (match(IGUAL)) {
        tem_valor = true;
        // aceita um número, texto ou identificador simples
        Token val = peek();
        if (!(match(NUMERO) || match(TEXTO) || match(IDENTIFICADOR))) {
            erro("Esperado valor após '='");
            return false;
        }
    }
    if (!match(PONTO_E_VIRGULA)) {
        erro("Esperado ';' no final da declaração");
        return false;
    }
    std::cout << "Declaração reconhecida: var " << id.valor << " : " << tipo.valor;
    if (tem_valor) {
        std::cout << " = valor";
    }
    std::cout << ";" << std::endl;
    return true;
}

// print(<expressão>);
bool Parser::parse_print() {
    if (!match(PRINT)) {
        erro("Esperado 'print'");
        return false;
    }
    if (!match(ABRE_PARENTESE)) {
        erro("Esperado '(' após 'print'");
        return false;
    }
    // Para simplificar, só aceita um identificador ou texto aqui
    if (!(match(IDENTIFICADOR) || match(TEXTO) || match(NUMERO))) {
        erro("Esperado valor para print");
        return false;
    }
    if (!match(FECHA_PARENTESE)) {
        erro("Esperado ')' após argumento do print");
        return false;
    }
    if (!match(PONTO_E_VIRGULA)) {
        erro("Esperado ';' após print");
        return false;
    }
    std::cout << "Comando print reconhecido" << std::endl;
    return true;
}

// input(<id>);
bool Parser::parse_input() {
    if (!match(INPUT)) {
        erro("Esperado 'input'");
        return false;
    }
    if (!match(ABRE_PARENTESE)) {
        erro("Esperado '(' após 'input'");
        return false;
    }
    if (!match(IDENTIFICADOR)) {
        erro("Esperado identificador dentro do input");
        return false;
    }
    if (!match(FECHA_PARENTESE)) {
        erro("Esperado ')' após input");
        return false;
    }
    if (!match(PONTO_E_VIRGULA)) {
        erro("Esperado ';' após input");
        return false;
    }
    std::cout << "Comando input reconhecido" << std::endl;
    return true;
}

// if (<condição>) { <comandos> } [else { <comandos> }]
bool Parser::parse_if() {
    if (!match(IF)) {
        erro("Esperado 'if'");
        return false;
    }
    if (!match(ABRE_PARENTESE)) {
        erro("Esperado '(' após 'if'");
        return false;
    }
    // Para simplificar, só aceita um identificador ou número
    if (!(match(IDENTIFICADOR) || match(NUMERO))) {
        erro("Esperado condição dentro do if");
        return false;
    }
    if (!match(FECHA_PARENTESE)) {
        erro("Esperado ')' após condição do if");
        return false;
    }
    if (!match(ABRE_CHAVE)) {
        erro("Esperado '{' após condição do if");
        return false;
    }
    while (peek().tipo != FECHA_CHAVE && peek().tipo != FIM_ARQUIVO) {
        if (!parse_comando()) return false;
    }
    if (!match(FECHA_CHAVE)) {
        erro("Esperado '}' para fechar bloco do if");
        return false;
    }
    if (match(ELSE)) {
        if (!match(ABRE_CHAVE)) {
            erro("Esperado '{' após else");
            return false;
        }
        while (peek().tipo != FECHA_CHAVE && peek().tipo != FIM_ARQUIVO) {
            if (!parse_comando()) return false;
        }
        if (!match(FECHA_CHAVE)) {
            erro("Esperado '}' para fechar bloco do else");
            return false;
        }
    }
    std::cout << "Comando if/else reconhecido" << std::endl;
    return true;
}

// while (<condição>) { <comandos> }
bool Parser::parse_while() {
    if (!match(WHILE)) {
        erro("Esperado 'while'");
        return false;
    }
    if (!match(ABRE_PARENTESE)) {
        erro("Esperado '(' após 'while'");
        return false;
    }
    if (!(match(IDENTIFICADOR) || match(NUMERO))) {
        erro("Esperado condição no while");
        return false;
    }
    if (!match(FECHA_PARENTESE)) {
        erro("Esperado ')' após condição do while");
        return false;
    }
    if (!match(ABRE_CHAVE)) {
        erro("Esperado '{' após condição do while");
        return false;
    }
    while (peek().tipo != FECHA_CHAVE && peek().tipo != FIM_ARQUIVO) {
        if (!parse_comando()) return false;
    }
    if (!match(FECHA_CHAVE)) {
        erro("Esperado '}' para fechar bloco do while");
        return false;
    }
    std::cout << "Comando while reconhecido" << std::endl;
    return true;
}

// for (<var decl>; <condição>; <incremento>) { <comandos> }
bool Parser::parse_for() {
    if (!match(FOR)) {
        erro("Esperado 'for'");
        return false;
    }
    if (!match(ABRE_PARENTESE)) {
        erro("Esperado '(' após 'for'");
        return false;
    }
    if (!parse_declaracao()) {
        erro("Esperado declaração no for");
        return false;
    }
    // Para simplificar, condição e incremento serão só identificadores ou números e ';'
    if (!(match(IDENTIFICADOR) || match(NUMERO))) {
        erro("Esperado condição no for");
        return false;
    }
    if (!match(PONTO_E_VIRGULA)) {
        erro("Esperado ';' após condição do for");
        return false;
    }
    if (!(match(IDENTIFICADOR) || match(NUMERO))) {
        erro("Esperado incremento no for");
        return false;
    }
    if (!match(FECHA_PARENTESE)) {
        erro("Esperado ')' após incremento do for");
        return false;
    }
    if (!match(ABRE_CHAVE)) {
        erro("Esperado '{' após for");
        return false;
    }
    while (peek().tipo != FECHA_CHAVE && peek().tipo != FIM_ARQUIVO) {
        if (!parse_comando()) return false;
    }
    if (!match(FECHA_CHAVE)) {
        erro("Esperado '}' para fechar bloco do for");
        return false;
    }
    std::cout << "Comando for reconhecido" << std::endl;
    return true;
}

// func <id> () { <comandos> }
bool Parser::parse_func() {
    if (!match(FUNC)) {
        erro("Esperado 'func'");
        return false;
    }
    if (!match(IDENTIFICADOR)) {
        erro("Esperado identificador após func");
        return false;
    }
    if (!match(ABRE_PARENTESE)) {
        erro("Esperado '(' após nome da função");
        return false;
    }
    if (!match(FECHA_PARENTESE)) {
        erro("Esperado ')' após '(' da função");
        return false;
    }
    if (!match(ABRE_CHAVE)) {
        erro("Esperado '{' após assinatura da função");
        return false;
    }
    while (peek().tipo != FECHA_CHAVE && peek().tipo != FIM_ARQUIVO) {
        if (!parse_comando()) return false;
    }
    if (!match(FECHA_CHAVE)) {
        erro("Esperado '}' para fechar bloco da função");
        return false;
    }
    std::cout << "Função reconhecida" << std::endl;
    return true;
}