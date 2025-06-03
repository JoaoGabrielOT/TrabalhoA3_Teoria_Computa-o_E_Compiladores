#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.hpp"
#include "parser.hpp"

std::string nome_token(TokenTipo tipo) {
    switch (tipo) {
        case VAR: return "VAR";
        case PRINT: return "PRINT";
        case INPUT: return "INPUT";
        case IF: return "IF";
        case ELSE: return "ELSE";
        case WHILE: return "WHILE";
        case FOR: return "FOR";
        case FUNC: return "FUNC";
        case INT: return "INT";
        case FLOAT: return "FLOAT";
        case CHAR: return "CHAR";
        case BOOL: return "BOOL";
        case STRING: return "STRING";
        case VOID: return "VOID";
        case IDENTIFICADOR: return "IDENTIFICADOR";
        case NUMERO: return "NUMERO";
        case TEXTO: return "TEXTO";
        case DOIS_PONTOS: return "DOIS_PONTOS";
        case IGUAL: return "IGUAL";
        case PONTO_E_VIRGULA: return "PONTO_E_VIRGULA";
        case ABRE_PARENTESE: return "ABRE_PARENTESE";
        case FECHA_PARENTESE: return "FECHA_PARENTESE";
        case ABRE_CHAVE: return "ABRE_CHAVE";
        case FECHA_CHAVE: return "FECHA_CHAVE";
        case FIM_ARQUIVO: return "FIM_ARQUIVO";
        case ERRO: return "ERRO";
        default: return "DESCONHECIDO";
    }
}

int main() {    
    std::ifstream arquivo("entrada.macslang");
    if (!arquivo) {
        std::cerr << "Não foi possível abrir o arquivo entrada.macslang\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << arquivo.rdbuf();
    std::string codigo = buffer.str();

    std::cout << "\033[1;34m==== LEXER ====\033[0m\n";
    Lexer lexer(codigo);
    std::vector<Token> tokens;
    Token token;

    do {
        token = lexer.proximo_token();
        tokens.push_back(token);

        // Debug: mostra os tokens
       std::cout << "Token { " << "\033[1;36m" << nome_token(token.tipo) << "\033[0m, " << "\033[1;33m\"" << token.valor << "\"\033[0m" << " }\n";
    } while (token.tipo != FIM_ARQUIVO && token.tipo != ERRO);

    if (token.tipo == ERRO) {
        std::cerr << "Erro léxico encontrado: " << token.valor << "\n";
        return 1;
    }

    std::cout << "\n\033[1;32m==== PARSER ====\033[0m\n";
    Parser parser(tokens);
    if (parser.parse()) {
        std::cout << "Código sintaticamente correto.\n";
    } else {
        std::cerr << "Erro de sintaxe detectado.\n";
        return 1;
    }

    return 0;
}
