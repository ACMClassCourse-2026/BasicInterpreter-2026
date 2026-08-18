#include "basic/lexer.hpp"

#include <cctype>
#include <unordered_map>
#include <utility>

#include "basic/basic_error.hpp"

namespace basic {

namespace {

const std::unordered_map<std::string, TokenType> kKeywords = {
    {"DIM", TokenType::Dim},           {"AS", TokenType::As},
    {"LET", TokenType::Let},           {"PRINT", TokenType::Print},
    {"INPUT", TokenType::Input},       {"END", TokenType::End},
    {"REM", TokenType::Rem},           {"REDIM", TokenType::Redim},
    {"PRESERVE", TokenType::Preserve}, {"ERASE", TokenType::Erase},
    {"IF", TokenType::If},             {"THEN", TokenType::Then},
    {"ELSEIF", TokenType::ElseIf},     {"ELSE", TokenType::Else},
    {"FOR", TokenType::For},           {"TO", TokenType::To},
    {"STEP", TokenType::Step},         {"NEXT", TokenType::Next},
    {"EXIT", TokenType::Exit},         {"WHILE", TokenType::While},
    {"WEND", TokenType::Wend},         {"RUN", TokenType::Run},
    {"LIST", TokenType::List},         {"CLEAR", TokenType::Clear},
    {"QUIT", TokenType::Quit},         {"HELP", TokenType::Help},
    {"INTEGER", TokenType::Integer},   {"LONG", TokenType::Long},
    {"SINGLE", TokenType::Single},     {"DOUBLE", TokenType::Double},
    {"STRING", TokenType::String},     {"CINT", TokenType::CInt},
    {"CLNG", TokenType::CLng},         {"CSNG", TokenType::CSng},
    {"CDBL", TokenType::CDbl},         {"MOD", TokenType::Mod},
    {"NOT", TokenType::Not},           {"AND", TokenType::And},
    {"OR", TokenType::Or},
};

bool is_identifier_start(char character) {
    return std::isalpha(static_cast<unsigned char>(character)) != 0;
}

bool is_identifier_part(char character) {
    return std::isalnum(static_cast<unsigned char>(character)) != 0;
}

}  // namespace

TokenStream Lexer::tokenize(const std::string& source) const {
    std::vector<Token> tokens;
    std::size_t position = 0;
    while (position < source.size()) {
        const char character = source[position];
        if (std::isspace(static_cast<unsigned char>(character)) != 0) {
            ++position;
            continue;
        }

        const std::size_t column = position;
        if (is_identifier_start(character)) {
            ++position;
            while (position < source.size() &&
                   is_identifier_part(source[position])) {
                ++position;
            }
            std::string text = source.substr(column, position - column);
            const auto keyword = kKeywords.find(text);
            const TokenType type = keyword == kKeywords.end()
                                       ? TokenType::Identifier
                                       : keyword->second;
            tokens.push_back({type, std::move(text), column});
            if (type == TokenType::Rem) {
                while (position < source.size() &&
                       std::isspace(static_cast<unsigned char>(source[position])) !=
                           0) {
                    ++position;
                }
                if (position < source.size()) {
                    tokens.push_back({TokenType::RemText,
                                      source.substr(position), position});
                }
                position = source.size();
            }
            continue;
        }

        if (std::isdigit(static_cast<unsigned char>(character)) != 0) {
            ++position;
            while (position < source.size() &&
                   std::isdigit(static_cast<unsigned char>(source[position])) !=
                       0) {
                ++position;
            }
            TokenType type = TokenType::IntegerLiteral;
            if (position < source.size() && source[position] == '.') {
                type = TokenType::RealLiteral;
                ++position;
                if (position >= source.size() ||
                    std::isdigit(static_cast<unsigned char>(source[position])) ==
                        0) {
                    throw BasicError(ErrorCode::Syntax);
                }
                while (position < source.size() &&
                       std::isdigit(
                           static_cast<unsigned char>(source[position])) != 0) {
                    ++position;
                }
            }
            tokens.push_back(
                {type, source.substr(column, position - column), column});
            continue;
        }

        if (character == '"') {
            ++position;
            const std::size_t text_begin = position;
            while (position < source.size() && source[position] != '"') {
                ++position;
            }
            if (position >= source.size()) {
                throw BasicError(ErrorCode::Syntax);
            }
            tokens.push_back({TokenType::StringLiteral,
                              source.substr(text_begin, position - text_begin),
                              column});
            ++position;
            continue;
        }

        auto push_single = [&](TokenType type) {
            tokens.push_back({type, std::string(1, character), column});
            ++position;
        };
        switch (character) {
            case '+':
                push_single(TokenType::Plus);
                break;
            case '-':
                push_single(TokenType::Minus);
                break;
            case '*':
                push_single(TokenType::Multiply);
                break;
            case '/':
                push_single(TokenType::Divide);
                break;
            case '\\':
                push_single(TokenType::IntegerDivide);
                break;
            case '^':
                push_single(TokenType::Power);
                break;
            case '=':
                push_single(TokenType::Equal);
                break;
            case '(':
                push_single(TokenType::LeftParen);
                break;
            case ')':
                push_single(TokenType::RightParen);
                break;
            case ',':
                push_single(TokenType::Comma);
                break;
            case '%':
                push_single(TokenType::PercentSuffix);
                break;
            case '&':
                push_single(TokenType::AmpersandSuffix);
                break;
            case '!':
                push_single(TokenType::ExclamationSuffix);
                break;
            case '#':
                push_single(TokenType::HashSuffix);
                break;
            case '$':
                push_single(TokenType::DollarSuffix);
                break;
            case '<':
                ++position;
                if (position < source.size() && source[position] == '=') {
                    ++position;
                    tokens.push_back({TokenType::LessEqual, "<=", column});
                } else if (position < source.size() && source[position] == '>') {
                    ++position;
                    tokens.push_back({TokenType::NotEqual, "<>", column});
                } else {
                    tokens.push_back({TokenType::Less, "<", column});
                }
                break;
            case '>':
                ++position;
                if (position < source.size() && source[position] == '=') {
                    ++position;
                    tokens.push_back({TokenType::GreaterEqual, ">=", column});
                } else {
                    tokens.push_back({TokenType::Greater, ">", column});
                }
                break;
            default:
                throw BasicError(ErrorCode::Syntax);
        }
    }
    tokens.push_back({TokenType::EndOfInput, "", source.size()});
    return TokenStream(std::move(tokens));
}

}  // namespace basic
