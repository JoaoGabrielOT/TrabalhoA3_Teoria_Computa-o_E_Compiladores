// main.cpp
#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.hpp"
#include "parser.hpp"

int main() {
    std::ifstream arquivo("entrada.macslang");
    if (!arquivo) {
        std::cerr << "Não foi possível abrir o arquivo entrada.macslang\n";
        return 1;
    }

    std::stringstream buffer;
    buffer << arquivo.rdbuf();
    std::string codigo = buffer.str();

    Lexer lexer(codigo);
    std::vector<Token> tokens;
    Token token;

    do {
        token = lexer.proximo_token();
        tokens.push_back(token);

        // Debug: mostra os tokens
        std::cout << "Token: tipo=" << token.tipo << ", valor='" << token.valor << "'\n";
    } while (token.tipo != FIM_ARQUIVO && token.tipo != ERRO);

    if (token.tipo == ERRO) {
        std::cerr << "Erro léxico encontrado: " << token.valor << "\n";
        return 1;
    }

    Parser parser(tokens);
    if (parser.parse()) {
        std::cout << "Código sintaticamente correto.\n";
    } else {
        std::cerr << "Erro de sintaxe detectado.\n";
        return 1;
    }

    return 0;
}
