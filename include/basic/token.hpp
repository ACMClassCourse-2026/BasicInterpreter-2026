#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace basic {

enum class TokenType {
    EndOfInput,
    Identifier,
    IntegerLiteral,
    RealLiteral,
    StringLiteral,
    RemText,
    Dim,
    As,
    Let,
    Print,
    Input,
    End,
    Rem,
    Redim,
    Preserve,
    Erase,
    If,
    Then,
    ElseIf,
    Else,
    For,
    To,
    Step,
    Next,
    Exit,
    While,
    Wend,
    Run,
    List,
    Clear,
    Quit,
    Help,
    Integer,
    Long,
    Single,
    Double,
    String,
    CInt,
    CLng,
    CSng,
    CDbl,
    Mod,
    Not,
    And,
    Or,
    Plus,
    Minus,
    Multiply,
    Divide,
    IntegerDivide,
    Power,
    Equal,
    NotEqual,
    Greater,
    Less,
    GreaterEqual,
    LessEqual,
    LeftParen,
    RightParen,
    Comma,
    PercentSuffix,
    AmpersandSuffix,
    ExclamationSuffix,
    HashSuffix,
    DollarSuffix
};

struct Token {
    TokenType type{TokenType::EndOfInput};
    std::string text;
    std::size_t column{0};
};

class TokenStream {
public:
    TokenStream() = default;
    explicit TokenStream(std::vector<Token> tokens);

    const Token& peek(std::size_t offset = 0) const;
    const Token& consume();
    bool match(TokenType type);
    void expect(TokenType type);
    bool at_end() const noexcept;
    std::size_t position() const noexcept;
    void reset() noexcept;
    const std::vector<Token>& tokens() const noexcept;

private:
    std::vector<Token> tokens_;
    std::size_t position_{0};
};

const char* token_type_name(TokenType type) noexcept;

}  // namespace basic
