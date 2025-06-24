#include "lexer.hpp"
#include <cctype>

Lexer::Lexer(const std::string & texto) : texto_(texto), pos_(0) {
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

    // Comentário //
    if (atual_ == '/' && pos_ + 1 < texto_.size() && texto_[pos_ + 1] == '/') {
        while (atual_ != '\n' && atual_ != '\0') {
            avanca();
        }
        return proximo_token();
    }

    // Fim do texto
    if (atual_ == '\0') {
        return {FIM_ARQUIVO, ""};
    }

    // Operadores compostos (duplos)
    if (atual_ == '=' && pos_ + 1 < texto_.size() && texto_[pos_ + 1] == '=') {
        avanca(); avanca();
        return {IGUAL_IGUAL, "=="};
    }
    if (atual_ == '!' && pos_ + 1 < texto_.size() && texto_[pos_ + 1] == '=') {
        avanca(); avanca();
        return {DIFERENTE, "!="};
    }
    if (atual_ == '<' && pos_ + 1 < texto_.size() && texto_[pos_ + 1] == '=') {
        avanca(); avanca();
        return {MENOR_IGUAL, "<="};
    }
    if (atual_ == '>' && pos_ + 1 < texto_.size() && texto_[pos_ + 1] == '=') {
        avanca(); avanca();
        return {MAIOR_IGUAL, ">="};
    }
    if (atual_ == '&' && pos_ + 1 < texto_.size() && texto_[pos_ + 1] == '&') {
        avanca(); avanca();
        return {E_LOGICO, "&&"};
    }
    if (atual_ == '|' && pos_ + 1 < texto_.size() && texto_[pos_ + 1] == '|') {
        avanca(); avanca();
        return {OU_LOGICO, "||"};
    }

    // Operadores simples
    if (atual_ == '+') { avanca(); return {MAIS, "+"}; }
    if (atual_ == '-') { avanca(); return {MENOS, "-"}; }
    if (atual_ == '*') { avanca(); return {VEZES, "*"}; }
    if (atual_ == '/') { avanca(); return {DIVIDIDO, "/"}; }
    if (atual_ == '!') { avanca(); return {NEGACAO, "!"}; }
    if (atual_ == '<') { avanca(); return {MENOR, "<"}; }
    if (atual_ == '>') { avanca(); return {MAIOR, ">"}; }
    if (atual_ == '=') { avanca(); return {IGUAL, "="}; }
    if (atual_ == ':') { avanca(); return {DOIS_PONTOS, ":"}; }

    // Pontuação
    if (atual_ == ';') { avanca(); return {PONTO_E_VIRGULA, ";"}; }
    if (atual_ == '(') { avanca(); return {ABRE_PARENTESE, "("}; }
    if (atual_ == ')') { avanca(); return {FECHA_PARENTESE, ")"}; }
    if (atual_ == '{') { avanca(); return {ABRE_CHAVE, "{"}; }
    if (atual_ == '}') { avanca(); return {FECHA_CHAVE, "}"}; }

    // Constante de caractere
    if (atual_ == '\'') {
        avanca(); // pula a aspa
        if (atual_ != '\0' && texto_[pos_ + 1] == '\'') {
            char c = atual_;
            avanca(); // consome o caractere
            avanca(); // consome a aspa final
            return {CHAR, std::string(1, c)};
        } else {
            return {ERRO, "Caractere mal formatado"};
        }
    }

    // String
    if (atual_ == '"') {
        return identifica_texto();
    }

    // Número
    if (isdigit(atual_)) {
        return identifica_numero();
    }

    // Identificador ou palavra-chave
    if (isalpha(atual_) || atual_ == '_') {
        return identifica_identificador_ou_palavra_chave();
    }

    // Qualquer outro caractere inválido
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
    bool tem_ponto = false;

    while (isdigit(atual_) || (atual_ == '.' && !tem_ponto)) {
        if (atual_ == '.') {
            tem_ponto = true;
        }
        valor += atual_;
        avanca();
    }

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

    if (valor == "true") return {TRUE, valor};
    if (valor == "false") return {FALSE, valor};

    return {IDENTIFICADOR, valor};
}
