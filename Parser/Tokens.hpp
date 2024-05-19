#ifndef TOKENS_HPP
#define TOKENS_HPP

#include<iostream>
#include<string>
using namespace std;

class Token { // creamos una clase para los token
public:
    enum Type { // definimos los tipos de token que pueden existir
        SELECT, ALL, FROM, TABLE,
        WHERE, CREATE, INSERT, DELETE,
        VALUES, INTO, FILE, USING,
        INDEX, HASH, AVL, BTREE,
        BETWEEN, AND, STRING, NUMBER,
        EQUAL, ID, LPAREN=0, RPAREN,
        END, ERR
    };
    static const char* token_names[26]; // en total son 26 tipos
    Type type;
    string lexema; // cada token es representado por una cadena de caracteres llamada lexema
    Token(Type): type(type) { // por default, cada tipo de token empieza con un lexema o cadena de sring vacío
        lexema="";
    }
    Token(Type type, const string lexema): type(type), lexema(lexema) {};
};

const char* Token::token_names[26] = { // este array token_names contiene los lexemas de cada token
        "SELECT", "ALL", "FROM", "TABLE",
        "WHERE", "CREATE", "INSERT", "DELETE",
        "VALUES", "INTO", "FILE", "USING",
        "INDEX", "HASH", "AVL", "BTREE",
        "BETWEEN", "AND", "STRING", "NUMBER",
        "EQUAL", "ID", "LPAREN", "RPAREN",
        "END", "ERR"
};

std::ostream& operator << ( std::ostream& outs, const Token & tok ) {
    if (tok.lexema.empty())
        return outs << Token::token_names[tok.type];
    else
        return outs << Token::token_names[tok.type] << "(" << tok.lexema << ")";
}

std::ostream& operator << ( std::ostream& outs, const Token* tok ) {
    return outs << *tok;
}

#endif // TOKENS_HPP
