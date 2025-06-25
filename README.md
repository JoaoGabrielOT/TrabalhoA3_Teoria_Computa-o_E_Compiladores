# 🛠️ MACSLang Compiler

Um compilador simples construído em C++ para a linguagem **MACSLang**, criada para fins educacionais. O projeto simula o processo de compilação passo a passo: desde a análise léxica até a verificação de tipos e comandos básicos.

## 📚 Etapas do Projeto

### 1. Analisador Léxico (`lexer.cpp` / `lexer.hpp`)

O **lexer** é responsável por **quebrar o código-fonte em tokens**. Ele identifica:
- Palavras-chave (`var`, `if`, `print`, etc.)
- Identificadores
- Operadores (`+`, `==`, `!=`, `=`, etc.)
- Tipos primitivos (`int`, `float`, `string`, `bool`, `char`)
- Literais (`números`, `strings`, `true/false`, etc.)
- Símbolos (`;`, `:`, `{`, `}`)

Exemplo de entrada:
```txt
var nome: string = "Lucas";
```

Tokens gerados:
```
[VAR][IDENTIFICADOR][DOIS_PONTOS][STRING][IGUAL]["Lucas"][PONTO_E_VIRGULA]
```

### 2. Analisador Sintático (`parser.cpp` / `parser.hpp`)

O **parser** recebe os tokens e verifica se eles formam estruturas válidas da linguagem. Ele reconhece:
- Declaração de variáveis
- Atribuições
- Comando `print()`
- Comando `input()`
- Comandos `if`, `else`, `while`, `for`
- Declaração de funções com `func`

Durante essa etapa, o parser também:
- Garante que variáveis sejam **declaradas antes do uso**
- Verifica **escopos** (como blocos `if`, `while`, `for`)
- Faz a **verificação semântica** básica (ex: `var x: string = 5;` gera erro)

### 3. Semântica

Já está implementada a checagem de:
- Declaração de variáveis
- Escopo correto
- Tipagem correta ao atribuir valores

Exemplo:
```txt
var idade: int = "vinte"; // gera erro: tipo incompatível
```

## 🚀 Funcionalidades Implementadas

✅ Analisador léxico completo  
✅ Reconhecimento de comandos: `var`, `print`, `input`, `if/else`, `while`, `for`, `func`  
✅ Verificação de tipos (int, float, string, char, bool)  
✅ Suporte a literais: números, texto, caracteres, booleanos  
✅ Suporte a escopos (funções, if, while, etc.)  
✅ Erros com mensagens coloridas (vermelho = erro, verde = sucesso)  

## 📂 Estrutura dos Arquivos

```bash
├── main.cpp            # Entrada principal do programa
├── lexer.hpp/.cpp      # Analisador léxico
├── parser.hpp/.cpp     # Analisador sintático e semântico
├── token.hpp           # Definição dos tipos de token
└── macslang.exe        # Arquivo de entrada de teste
```

## 🧪 Exemplo de Execução

### Código de entrada (`exemplo.txt`)
```txt
var nome: string = "Lucas";
print(nome);
```

### Passo a Passo da Execução

1. **main.cpp** lê o arquivo `exemplo.txt`
2. Cria um **objeto Lexer**
   - Chama `lexer.proximo_token()` até gerar todos os tokens
3. Cria um **objeto Parser**
   - Chama `parser.parse()` para interpretar os tokens
   - Executa `parse_declaracao()` → reconhece `var nome: string = "Lucas";`
     - Verifica se `nome` já existe no escopo
     - Verifica se `"Lucas"` é um valor do tipo `string`
   - Executa `parse_print()` → reconhece `print(nome);`
     - Verifica se `nome` foi declarado
4. Imprime no terminal:
```
Declaração reconhecida: var nome : string;
Comando print reconhecido
```

## 🧠 Gramática Livre de Contexto (GLC) usada

```ebnf
<programa>           → <lista_declaracoes>
<lista_declaracoes>  → <declaracao> <lista_declaracoes> | ε
<declaracao>         → VAR IDENT COLON <tipo> EQ <literal> SEMICOLON
<tipo>               → "int" | "float" | "string" | "char" | "bool"
<literal>            → <inteiro> | <float> | <string> | <char> | "true" | "false"
<inteiro>            → <digito> | <digito><inteiro>
<digito>             → 0 | 1 | 2 | 3 | 4 | 5 | 6 | 7 | 8 | 9
IDENT                → <letra> | <letra>IDENT
<letra>              → [a-zA-Z]+
```

## ✅ Como Compilar e Rodar

1. Compile o projeto:
```bash
g++ -std=c++17 main.cpp lexer.cpp parser.cpp -o macs
```

2. Crie seu código-fonte no arquivo `exemplo.txt`

3. Execute:
```bash
./macs exemplo.txt
```
