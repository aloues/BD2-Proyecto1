#include <iostream>
#include <unordered_map> // necesitamos esta libreria para definir los tokens o palabras reservadas
#include <algorithm>  // Para transform
#include <cctype>     // Para tolower
#include "Tokens.hpp" // necesitamos que nuestro scanner pueda identificar los tokens ya definidos anteriormente
 

class Scanner {
public:
  Scanner(const char* input);
  Token* nextToken();
  bool parenthesis_exist = false;
  ~Scanner();  
private:
  string input;
  int first, current;
  char nextChar();
  void rollBack();
  void startLexema();
  
  void delete_whitespace(char &c);

  Token* scan_number(char &c);
  Token* scan_string(char &c);
  Token* scan_alpha(char &c);
  Token* scan_lparen(char &c);
  Token* scan_rparen(char &c);

  string getLexema();
  unordered_map<string, Token::Type> reserved;
  Token::Type checkReserved(string lexema);
};

Scanner::Scanner(const char* s):input(s),first(0),current(0) {
  reserved["select"] = Token::SELECT;
  reserved["from"] = Token::FROM;
  reserved["table"] = Token::TABLE;
  reserved["where"] = Token::WHERE;
  reserved["create"] = Token::CREATE;
  reserved["insert"] = Token::INSERT;
  reserved["delete"] = Token::DELETE;
  reserved["values"] = Token::VALUES;
  reserved["into"] = Token::INTO;
  reserved["file"] = Token::FILE;
  reserved["using"] = Token::USING;
  reserved["index"] = Token::INDEX;
  reserved["hash"] = Token::HASH;
  reserved["avl"] = Token::AVL;
  reserved["btree"] = Token::BTREE;
  reserved["between"] = Token::BETWEEN;
  reserved["and"] = Token::AND;
  reserved["*"] = Token::ALL; 
}

Token::Type Scanner::checkReserved(string lexema) {
    transform(lexema.begin(), lexema.end(), lexema.begin(),
                   [](unsigned char c){ return tolower(c); });
    auto it = reserved.find(lexema);
    if (it == reserved.end())
        return Token::ID; // No es una palabra reservada, es un identificador
    else
        return it->second;
}

Scanner::~Scanner() { }

char Scanner::nextChar() {
  int c = input[current];
  current++;
  return c;
}

void Scanner::rollBack() { // retrocedo un estado
  current--;
}

void Scanner::startLexema() {
  first = current-1;
  return;
}

string Scanner::getLexema() {
  return input.substr(first,current-first);
}

void Scanner::delete_whitespace(char &c){
    while(c == ' ') c = nextChar();
}

Token* Scanner::scan_number(char &c){
    c = nextChar();
    while(isdigit(c)) c = nextChar();
    rollBack();
    return new Token(Token::NUMBER, getLexema());
}

Token* Scanner::scan_string(char &c){
    string value;  // Usamos una variable local para construir el lexema
    c = nextChar();  // Saltamos la primera comilla
    while (c != '"') {  // Lee hasta encontrar otra comilla
        value += c;  // Añade cada carácter al lexema
        c = nextChar();
    }
    return new Token(Token::STRING, value);  // Crea el token sin las comillas
}


Token* Scanner::scan_alpha(char &c) {
    string lexema;
    do {
        lexema += c;
        c = nextChar();
    } while (isalpha(c) || c == '_');  // Continuar mientras el carácter sea alfabético o '_'

    // El rollback se debe hacer solo si no es el fin de la entrada
    if (c != '\0') {
        rollBack();
    }

    std::transform(lexema.begin(), lexema.end(), lexema.begin(), ::tolower);  // Convertir a minúsculas
    auto it = reserved.find(lexema);
    if (it != reserved.end()) {
        return new Token(it->second, lexema);  // Si es una palabra reservada, retornar el token correspondiente
    }
    return new Token(Token::ID, lexema);  // De lo contrario, es un identificador
}


Token* Scanner::scan_lparen(char &c){
    parenthesis_exist = true;
    return new Token(Token::LPAREN, "(");
}

Token* Scanner::scan_rparen(char &c){
    while(c != ')') c = nextChar();
    parenthesis_exist = false;
    rollBack();
    return new Token(Token::RPAREN, getLexema());
}

Token* Scanner::nextToken() {
    char c;
    c = nextChar();
    delete_whitespace(c);
    cout << "Current character: " << c << endl;

    if (c == '\0' || c == '-') {
        return new Token(Token::END, "-");
    }

    startLexema();

    if (parenthesis_exist) return scan_rparen(c);
    else if (c == '"') return scan_string(c);
    else if (c == '(') return scan_lparen(c);
    else if (isalpha(c) || c == '_') return scan_alpha(c);
    else if (c == '*') return new Token(Token::ALL, "*");
    else if (c == '=') return new Token(Token::EQUAL, "=");
    else if (c == ')') return new Token(Token::RPAREN, ")");
    else if (isdigit(c)) return scan_number(c);

    else return new Token(Token::ERR, string("Unrecognized character: ") + c);
}
