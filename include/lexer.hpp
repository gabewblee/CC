#pragma once

#include <string>

#include "token.hpp"

class Lexer {
public:
    /**
     * Lexer - Initializes the lexer with @src.
     * @src: The source file string to initialize with.
     */
    Lexer(std::string src);

    /**
     * next - Gets next token from the source file.
     * Returns: The next token. 
     */
    Token next();

    /**
     * loc - Gets current source file location.
     * Returns: The current source file location.
     */
    struct Location loc();

private:
    std::string     src_; /* Source file string   */
    struct Location loc_; /* Source file location */

    /**
     * eof - Verifies whether the end of file was reached.
     * Returns: True if reached, false otherwise.
     */
    bool eof();

    /**
     * tokenize - Initializes a token with the given fields.
     * @kind: The token kind to initialize with.
     * @lexeme: The token lexeme to initialize with.
     * @location: The token location to initialize with.
     * Returns: The initialized token.
     */
    Token tokenize(TokenKind kind, std::string lexeme, struct Location location);
    
    /**
     * peek - Peeks @ahead characters forward into the source file.
     * @ahead: The number of characters to peek forward by.
     * Returns: The peeked character.
     */
    char peek(std::size_t ahead = 0);

    /**
     * advance - Advances the source file cursor.
     */
    void advance();

    /**
     * handle_blank - Handles whitespaces.
     */
    void handle_blank();
    
    /**
     * handle_id - Handles identifiers.
     * Returns: The identifier token. 
     */
    Token handle_id();

    /**
     * handle_digit - Handles digits.
     * Returns: The digit token.
     */
    Token handle_digit();
};