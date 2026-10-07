#pragma once

#include <string>

#include "token.hpp"

class Lexer {
public:
    /**
     * Lexer - Initializes the lexer with the source file string.
     * @src: The source file string to initialize with.
     */
    Lexer(std::string src);

    /**
     * next - Gets next source file token.
     * Returns: The next source file token.
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
     * eof - Checks whether end of file was reached.
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
     * either - Tokenizes a one or two character token based on whether the current
                character is @c.
     * @c: The character to differentiate by.
     * @yes: The token kind if the current character is @c.
     * @no: The token kind if the current character is not @c.
     * @loc: The token location.
     * Returns: The one or two character token.
     */
    Token either(char c, TokenKind yes, TokenKind no, struct Location loc);

    /**
     * peek - Peeks @ahead characters forward into the source file.
     * @ahead: The number of characters to peek forward by.
     * Returns: The peeked character.
     */
    char peek(std::size_t ahead = 0);

    /**
     * advance - Advances the source file position by one.
     */
    void advance();

    /**
     * handle_blank - Handles whitespaces and comments.
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

    /**
     * handle_quote - Handles characters and strings.
     * @quote: The delimiting quote, either '\'' or '"'.
     * Returns: The quoted token.
     */
    Token handle_quote(char quote);
};