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
    std::cerr << "\033[1;31mErro de sintaxe: " << msg << "\033[0m" << std::endl;
}

bool Parser::parse() {
    bool sucesso = true;
    while (peek().tipo != FIM_ARQUIVO) {
        if (!parse_comando()) {
            sucesso = false;
        }
    }
    return sucesso;
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
            advance();
            return false;
    }
}

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

    if (!declararVariavel(id.valor, tipo.valor)) {
        erro("Variável '" + id.valor + "' já foi declarada.");
        return false;
    }

    if (match(IGUAL)) {
        Token valor = peek();
        if (!parse_expressao()) {
            erro("Expressão inválida após '='");
            return false;
        }

        // Verificação semântica de tipo
        if ((tipo.valor == "string" && valor.tipo != TEXTO) ||
            (tipo.valor == "int" && valor.tipo != NUMERO) ||
            (tipo.valor == "float" && valor.tipo != NUMERO) ||
            (tipo.valor == "char" && valor.tipo != CHAR) ||
            (tipo.valor == "bool" && !(valor.tipo == TRUE || valor.tipo == FALSE))) {
            erro("Tipo incompatível: variável '" + id.valor + "' é do tipo '" + tipo.valor + "' mas recebeu valor do tipo '" + valor.valor + "'");
            return false;
        }
    }

    if (!match(PONTO_E_VIRGULA)) {
        erro("Esperado ';' no final da declaração");
        return false;
    }

    std::cout << "\033[1;32mDeclaração reconhecida:\033[0m var " << id.valor << " : " << tipo.valor << ";" << std::endl;
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

    if (!parse_expressao()) {
        erro("Expressão inválida no print");
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

    std::cout << "\033[1;32mComando print reconhecido\033[0m" << std::endl;
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

    // Verifica se identificador foi declarado
    if (peek().tipo == IDENTIFICADOR) {
        if (!variavelDeclarada(peek().valor)) {
            erro("Variável '" + peek().valor + "' não declarada antes do uso em input");
            return false;
        }
        advance();
    } else {
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

    std::cout << "\033[1;32mComando input reconhecido\033[0m" << std::endl;
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

    if (peek().tipo == IDENTIFICADOR) {
        if (!variavelDeclarada(peek().valor)) {
            erro("Variável '" + peek().valor + "' não declarada na condição do if");
            return false;
        }
        advance();
    } else if (peek().tipo == NUMERO) {
        advance();
    } else {
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

    entrarEscopo();

    while (peek().tipo != FECHA_CHAVE && peek().tipo != FIM_ARQUIVO) {
        if (!parse_comando()) {
            sairEscopo();
            return false;
        }
    }

    if (!match(FECHA_CHAVE)) {
        erro("Esperado '}' para fechar bloco do if");
        sairEscopo();
        return false;
    }

    sairEscopo();

    if (match(ELSE)) {
        if (!match(ABRE_CHAVE)) {
            erro("Esperado '{' após else");
            return false;
        }

        entrarEscopo();

        while (peek().tipo != FECHA_CHAVE && peek().tipo != FIM_ARQUIVO) {
            if (!parse_comando()) {
                sairEscopo();
                return false;
            }
        }

        if (!match(FECHA_CHAVE)) {
            erro("Esperado '}' para fechar bloco do else");
            sairEscopo();
            return false;
        }

        sairEscopo();
    }

    std::cout << "\033[1;32mComando if/else reconhecido\033[0m" << std::endl;
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

    if (peek().tipo == IDENTIFICADOR) {
        if (!variavelDeclarada(peek().valor)) {
            erro("Variável '" + peek().valor + "' não declarada na condição do while");
            return false;
        }
        advance();
    } else if (peek().tipo == NUMERO) {
        advance();
    } else {
        erro("Esperado condição no while");
        return false;
    }

    if (!match(FECHA_PARENTESE)) {
        erro("Esperado ')' após condição do while");
        return false;
    }

    if (!match(ABRE_CHAVE)) {
        erro("Esperado '{' após while");
        return false;
    }

    entrarEscopo();

    while (peek().tipo != FECHA_CHAVE && peek().tipo != FIM_ARQUIVO) {
        if (!parse_comando()) {
            sairEscopo();
            return false;
        }
    }

    if (!match(FECHA_CHAVE)) {
        erro("Esperado '}' para fechar bloco do while");
        sairEscopo();
        return false;
    }

    sairEscopo();

    std::cout << "\033[1;32mComando while reconhecido\033[0m" << std::endl;
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

    entrarEscopo();  // Escopo inclui declaração da variável de controle

    if (!parse_declaracao()) {
        erro("Erro na declaração do 'for'");
        sairEscopo();  // garante limpeza
        return false;
    }

    // Condição
    if (peek().tipo == IDENTIFICADOR) {
        if (!variavelDeclarada(peek().valor)) {
            erro("Variável '" + peek().valor + "' não declarada na condição do 'for'");
            sairEscopo();
            return false;
        }
        advance();
    } else if (peek().tipo == NUMERO) {
        advance();
    } else {
        erro("Esperado condição válida no 'for'");
        sairEscopo();
        return false;
    }

    if (!match(PONTO_E_VIRGULA)) {
        erro("Esperado ';' após condição do 'for'");
        sairEscopo();
        return false;
    }

    // Incremento
    if (peek().tipo == IDENTIFICADOR) {
        if (!variavelDeclarada(peek().valor)) {
            erro("Variável '" + peek().valor + "' não declarada no incremento do 'for'");
            sairEscopo();
            return false;
        }
        advance();
    } else {
        erro("Esperado identificador no incremento do 'for'");
        sairEscopo();
        return false;
    }

    if (!match(FECHA_PARENTESE)) {
        erro("Esperado ')' para fechar cabeçalho do 'for'");
        sairEscopo();
        return false;
    }

    if (!match(ABRE_CHAVE)) {
        erro("Esperado '{' para abrir corpo do 'for'");
        sairEscopo();
        return false;
    }

    while (peek().tipo != FECHA_CHAVE && peek().tipo != FIM_ARQUIVO) {
        if (!parse_comando()) {
            sairEscopo();
            return false;
        }
    }

    if (!match(FECHA_CHAVE)) {
        erro("Esperado '}' para fechar corpo do 'for'");
        sairEscopo();
        return false;
    }

    sairEscopo();  // Fecha escopo do for inteiro (declaração + corpo)

    std::cout << "\033[1;32mComando for reconhecido\033[0m" << std::endl;
    return true;
}

// func <id> () { <comandos> }
bool Parser::parse_func() {
    if (!match(FUNC)) {
        erro("Esperado 'func'");
        return false;
    }

    Token nome = peek();
    if (!match(IDENTIFICADOR)) {
        erro("Esperado identificador após 'func'");
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

    if (!match(DOIS_PONTOS)) {
        erro("Esperado ':' após parênteses da função");
        return false;
    }

    Token tipo = peek();
    if (!(match(INT) || match(FLOAT) || match(CHAR) || match(BOOL) || match(STRING) || match(VOID))) {
        erro("Esperado tipo de retorno da função (int, float, char, bool, string ou void)");
        return false;
    }

    if (!match(ABRE_CHAVE)) {
        erro("Esperado '{' após assinatura da função");
        return false;
    }

    entrarEscopo(); // Escopo do corpo da função

    while (peek().tipo != FECHA_CHAVE && peek().tipo != FIM_ARQUIVO) {
        if (!parse_comando()) {
            sairEscopo();
            return false;
        }
    }

    sairEscopo();

    if (!match(FECHA_CHAVE)) {
        erro("Esperado '}' para fechar bloco da função");
        return false;
    }

    std::cout << "\033[1;32mFunção reconhecida\033[0m" << std::endl;
    return true;
}

bool Parser::parse_expressao() {
    if (!parse_termo()) return false;

    while (true) {
        TokenTipo tipo = peek().tipo;
        if (tipo == IGUAL_IGUAL || tipo == DIFERENTE || tipo == MAIOR || tipo == MENOR ||
            tipo == MAIOR_IGUAL || tipo == MENOR_IGUAL || tipo == MAIS || tipo == MENOS ||
            tipo == E_LOGICO || tipo == OU_LOGICO) {
            advance();
            if (!parse_termo()) {
                erro("Esperado termo após operador");
                return false;
            }
        } else {
            break;
        }
    }

    return true;
}

bool Parser::parse_termo() {
    if (!parse_fator()) return false;

    while (peek().tipo == VEZES || peek().tipo == DIVIDIDO) {
        advance();
        if (!parse_fator()) {
            erro("Esperado fator após operador");
            return false;
        }
    }

    return true;
}

bool Parser::parse_fator() {
    Token atual = peek();

    if (atual.tipo == TRUE || atual.tipo == FALSE) {
    advance();
    return true;
    }


    if (atual.tipo == NUMERO || atual.tipo == TEXTO || atual.tipo == CHAR) {
        advance();
        return true;
    }

    if (atual.tipo == IDENTIFICADOR) {
        if (!variavelDeclarada(atual.valor)) {
            erro("Variável '" + atual.valor + "' não declarada");
            return false;
        }
        advance();
        return true;
    }

    if (match(ABRE_PARENTESE)) {
        if (!parse_expressao()) return false;
        if (!match(FECHA_PARENTESE)) {
            erro("Esperado ')' após expressão");
            return false;
        }
        return true;
    }

    erro("Fator inesperado: " + atual.valor);
    return false;
}






void Parser::entrarEscopo() {
    escopos_.emplace_back();
}

void Parser::sairEscopo() {
    if (!escopos_.empty()) {
        escopos_.pop_back();
    }
}

bool Parser::declararVariavel(const std::string& nome, const std::string& tipo) {
    if (escopos_.empty()) entrarEscopo(); 

    auto& atual = escopos_.back();
    if (atual.count(nome)) return false;

    atual[nome] = tipo;
    return true;
}

bool Parser::variavelDeclarada(const std::string& nome) {
    for (auto it = escopos_.rbegin(); it != escopos_.rend(); ++it) {
        if (it->count(nome)) return true;
    }
    return false;
}

std::string Parser::tipoVariavel(const std::string& nome) {
    for (auto it = escopos_.rbegin(); it != escopos_.rend(); ++it) {
        auto found = it->find(nome);
        if (found != it->end()) return found->second;
    }
    return "";
}