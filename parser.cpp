#include <iostream>
#include <vector>
#include <string>
#include <cctype>
#include <map>
#include <fstream>
#include <unordered_set>

using namespace std;

enum TokenType
{
    T_INT,
    T_FLOAT,
    T_DOUBLE,
    T_STRING,
    T_CHAR,
    T_BOOL,
    T_ID,
    T_NUM,
    T_FLOATNUM,
    T_LITERAL,
    T_TRUE,
    T_FALSE,
    T_IF,
    T_ELSE,
    T_FOR,
    T_WHILE,
    T_RETURN,
    T_ASSIGN,
    T_PLUS,
    T_MINUS,
    T_MUL,
    T_DIV,
    T_LPAREN,
    T_RPAREN,
    T_LBRACE,
    T_RBRACE,
    T_SEMICOLON,
    T_GT,
    T_EOF,
};

struct Token
{
    TokenType type;
    string value;
    int line;
    int col;
};

class Lexer
{

private:
    string src;
    size_t pos;
    int line;
    int col;
    bool isFloat;

public:
    Lexer(const string &src)
    {
        this->src = src;
        this->pos = 0;
        line = 1;
        col = 1;
        isFloat = false;
    }

    vector<Token> tokenize()
    {
        vector<Token> tokens;
        while (pos < src.size())
        {
            char current = src[pos];

            if (isspace(current))
            {
                if (current == '\n')
                {
                    line++;
                    col = 1; // Reset column on new line
                }
                else
                {
                    col++; // Increment column for spaces
                }
                pos++;
                continue;
            }
            if (isdigit(current))
            {
                string word  = consumeNumber();
                if (isFloat)
                    tokens.push_back(Token{T_FLOATNUM, word, line, col});
                else
                    tokens.push_back(Token{T_NUM, word, line, col});
                isFloat = false;
                continue;
            }
            if (isalpha(current))
            {
                string word = consumeWord();
                if (word == "int")
                    tokens.push_back(Token{T_INT, word, line, col});
                else if (word == "float")
                    tokens.push_back(Token{T_FLOAT, word, line, col});
                else if (word == "double")
                    tokens.push_back(Token{T_DOUBLE, word, line, col});
                else if (word == "string")
                    tokens.push_back(Token{T_STRING, word, line, col});
                else if (word == "char")
                    tokens.push_back(Token{T_CHAR, word, line, col});
                else if (word == "bool")
                    tokens.push_back(Token{T_BOOL, word, line, col});
                else if (word == "true")
                    tokens.push_back(Token{T_TRUE, word, line, col});
                else if (word == "false")
                    tokens.push_back(Token{T_FALSE, word, line, col});
                else if (word == "if")
                    tokens.push_back(Token{T_IF, word, line, col});
                else if (word == "else")
                    tokens.push_back(Token{T_ELSE, word, line, col});
                else if (word == "return")
                    tokens.push_back(Token{T_RETURN, word, line, col});
                else
                    tokens.push_back(Token{T_ID, word, line, col});
                continue;
            }

            if(current == '"')
            {
                tokens.push_back(Token{T_LITERAL, consumeString('"'), line, col});
                continue;
            }

            if(current == '\'')
            {
                tokens.push_back(Token{T_LITERAL, consumeString('\''), line, col});
                continue;
            }

            switch (current)
            {
            case '=':
                tokens.push_back(Token{T_ASSIGN, "=", line, col});
                break;
            case '+':
                tokens.push_back(Token{T_PLUS, "+", line, col});
                break;
            case '-':
                tokens.push_back(Token{T_MINUS, "-", line, col});
                break;
            case '*':
                tokens.push_back(Token{T_MUL, "*", line, col});
                break;
            case '/':
                tokens.push_back(Token{T_DIV, "/", line, col});
                break;
            case '(':
                tokens.push_back(Token{T_LPAREN, "(", line, col});
                break;
            case ')':
                tokens.push_back(Token{T_RPAREN, ")", line, col});
                break;
            case '{':
                tokens.push_back(Token{T_LBRACE, "{", line, col});
                break;
            case '}':
                tokens.push_back(Token{T_RBRACE, "}", line, col});
                break;
            case ';':
                tokens.push_back(Token{T_SEMICOLON, ";", line, col});
                break;
            case '>':
                tokens.push_back(Token{T_GT, ">", line, col});
                break;
            default:
                cout << "Unexpected character: " << current << " at Ln " << line << ", Col " << col << endl;
                exit(1);
            }
            pos++;
            col++; // Increment column for each processed character
        }
        tokens.push_back(Token{T_EOF, "", line, col});
        return tokens;
    }

    string consumeNumber()
    {
        size_t start = pos;
        while (pos < src.size() && isdigit(src[pos]))
            pos++;

        if (pos < src.size()  && src[pos] == '.'){
            pos++;
            isFloat = true;
            while (pos < src.size() && isdigit(src[pos]))
                pos++;
        }

        return src.substr(start, pos - start);
    }

    string consumeWord()
    {
        size_t start = pos;
        while (pos < src.size() && isalnum(src[pos]))
            pos++;
        return src.substr(start, pos - start);
    }

    string consumeString(char ch)
    {
        size_t start = pos;
        pos++;
        while (pos < src.size() &&  src[pos] != ch)
            pos++;
        pos++;
        return src.substr(start, pos - start);
    }
};

class Parser
{

public:
    Parser(const vector<Token> &tokens)
    {
        this->tokens = tokens;
        this->pos = 0;
    }

    void parseProgram()
    {
        while (tokens[pos].type != T_EOF)
        {
            parseStatement();
        }
        cout << "Parsing completed successfully! No Syntax Error" << endl;
    }

private:
    vector<Token> tokens;
    size_t pos;
    int line;
    int word;

    void parseStatement()
    {
        if (tokens[pos].type == T_INT || tokens[pos].type == T_FLOAT || tokens[pos].type == T_DOUBLE || tokens[pos].type == T_STRING || tokens[pos].type == T_CHAR || tokens[pos].type == T_BOOL )
        {
            parseDeclaration(tokens[pos].type);
        }
        else if (tokens[pos].type == T_ID)
        {
            parseAssignment();
        }
        else if (tokens[pos].type == T_IF)
        {
            parseIfStatement();
        }
        else if (tokens[pos].type == T_RETURN)
        {
            parseReturnStatement();
        }
        else if (tokens[pos].type == T_LBRACE)
        {
            parseBlock();
        }
        else
        {
            reportError("unexpected token");
        }
    }

    void parseBlock()
    {
        expect(T_LBRACE);
        while (tokens[pos].type != T_RBRACE && tokens[pos].type != T_EOF)
        {
            parseStatement();
        }
        expect(T_RBRACE);
    }

    void parseDeclaration(TokenType type)
    {
        expect(type);
        expect(T_ID);
        expect(T_SEMICOLON);
    }

    void parseAssignment()
    {
        expect(T_ID);
        expect(T_ASSIGN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseIfStatement()
    {
        expect(T_IF);
        expect(T_LPAREN);
        parseExpression();
        expect(T_RPAREN);
        parseStatement();
        if (tokens[pos].type == T_ELSE)
        {
            expect(T_ELSE);
            parseStatement();
        }
    }

    void parseReturnStatement()
    {
        expect(T_RETURN);
        parseExpression();
        expect(T_SEMICOLON);
    }

    void parseExpression()
    {
        parseTerm();
        while (tokens[pos].type == T_PLUS || tokens[pos].type == T_MINUS)
        {
            pos++;
            parseTerm();
        }
        if (tokens[pos].type == T_GT)
        {
            pos++;
            parseExpression(); // After relational operator, parse the next expression
        }
    }

    void parseTerm()
    {
        parseFactor();
        while (tokens[pos].type == T_MUL || tokens[pos].type == T_DIV)
        {
            pos++;
            parseFactor();
        }
    }

    void parseFactor()
    {
        if (tokens[pos].type == T_NUM || tokens[pos].type == T_FLOATNUM || tokens[pos].type == T_TRUE || tokens[pos].type == T_FALSE || tokens[pos].type == T_ID)
        {
            pos++;
        }
        else if (tokens[pos].type == T_LPAREN)
        {
            expect(T_LPAREN);
            parseExpression();
            expect(T_RPAREN);
        }
        else
        {
           reportError("unexpected token");
        }
    }

    void expect(TokenType type)
    {
        if (tokens[pos].type == type)
        {
            pos++;
        }
        else
        {
            reportError("expected " + to_string(type) + " but found " + tokens[pos].value);
        }
    }

    void reportError(const string &message)
    {
        cout << "Syntax error: " << message
             << " at Ln " << tokens[pos].line
             << ", Col " << tokens[pos].col << ": '"
             << tokens[pos].value << "'" << endl;
        exit(1);
    }
};

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        cout << " Provide the file name as argument " << endl;

        return 0;
    }

    ifstream file(argv[1]);
    if (!file.is_open())
    {
        cout << " Error : Unable to open file " << endl;
        return 1;
    }
    string code = "";
    string line;
    while (getline(file, line))
    {
        code += line + "\n";
    }
    file.close();

    Lexer lexer(code);
    vector<Token> tokens = lexer.tokenize();

    Parser parser(tokens);
    parser.parseProgram();

    return 0;
}