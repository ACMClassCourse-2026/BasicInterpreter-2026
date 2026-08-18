#include "basic/token.hpp"

#include <utility>

#include "basic/basic_error.hpp"

namespace basic {

namespace {

const Token kEndToken{TokenType::EndOfInput, "", 0};

}  // namespace

TokenStream::TokenStream(std::vector<Token> tokens) : tokens_(std::move(tokens)) {
    if (tokens_.empty() || tokens_.back().type != TokenType::EndOfInput) {
        const std::size_t column = tokens_.empty() ? 0 : tokens_.back().column +
                                                        tokens_.back().text.size();
        tokens_.push_back({TokenType::EndOfInput, "", column});
    }
}

const Token& TokenStream::peek(std::size_t offset) const {
    const std::size_t index = position_ + offset;
    if (index >= tokens_.size()) {
        return kEndToken;
    }
    return tokens_[index];
}

const Token& TokenStream::consume() {
    const Token& token = peek();
    if (token.type != TokenType::EndOfInput) {
        ++position_;
    }
    return token;
}

bool TokenStream::match(TokenType type) {
    if (peek().type != type) {
        return false;
    }
    consume();
    return true;
}

void TokenStream::expect(TokenType type) {
    if (!match(type)) {
        throw BasicError(ErrorCode::Syntax);
    }
}

bool TokenStream::at_end() const noexcept {
    return position_ >= tokens_.size() ||
           tokens_[position_].type == TokenType::EndOfInput;
}

std::size_t TokenStream::position() const noexcept { return position_; }

void TokenStream::reset() noexcept { position_ = 0; }

const std::vector<Token>& TokenStream::tokens() const noexcept { return tokens_; }

const char* token_type_name(TokenType type) noexcept {
    switch (type) {
        case TokenType::EndOfInput:
            return "end of input";
        case TokenType::Identifier:
            return "identifier";
        case TokenType::IntegerLiteral:
            return "integer literal";
        case TokenType::RealLiteral:
            return "real literal";
        case TokenType::StringLiteral:
            return "string literal";
        case TokenType::RemText:
            return "REM text";
        case TokenType::Dim:
            return "DIM";
        case TokenType::As:
            return "AS";
        case TokenType::Let:
            return "LET";
        case TokenType::Print:
            return "PRINT";
        case TokenType::Input:
            return "INPUT";
        case TokenType::End:
            return "END";
        case TokenType::Rem:
            return "REM";
        case TokenType::Redim:
            return "REDIM";
        case TokenType::Preserve:
            return "PRESERVE";
        case TokenType::Erase:
            return "ERASE";
        case TokenType::If:
            return "IF";
        case TokenType::Then:
            return "THEN";
        case TokenType::ElseIf:
            return "ELSEIF";
        case TokenType::Else:
            return "ELSE";
        case TokenType::For:
            return "FOR";
        case TokenType::To:
            return "TO";
        case TokenType::Step:
            return "STEP";
        case TokenType::Next:
            return "NEXT";
        case TokenType::Exit:
            return "EXIT";
        case TokenType::While:
            return "WHILE";
        case TokenType::Wend:
            return "WEND";
        case TokenType::Run:
            return "RUN";
        case TokenType::List:
            return "LIST";
        case TokenType::Clear:
            return "CLEAR";
        case TokenType::Quit:
            return "QUIT";
        case TokenType::Help:
            return "HELP";
        case TokenType::Integer:
            return "INTEGER";
        case TokenType::Long:
            return "LONG";
        case TokenType::Single:
            return "SINGLE";
        case TokenType::Double:
            return "DOUBLE";
        case TokenType::String:
            return "STRING";
        case TokenType::CInt:
            return "CINT";
        case TokenType::CLng:
            return "CLNG";
        case TokenType::CSng:
            return "CSNG";
        case TokenType::CDbl:
            return "CDBL";
        case TokenType::Mod:
            return "MOD";
        case TokenType::Not:
            return "NOT";
        case TokenType::And:
            return "AND";
        case TokenType::Or:
            return "OR";
        case TokenType::Plus:
            return "+";
        case TokenType::Minus:
            return "-";
        case TokenType::Multiply:
            return "*";
        case TokenType::Divide:
            return "/";
        case TokenType::IntegerDivide:
            return "\\";
        case TokenType::Power:
            return "^";
        case TokenType::Equal:
            return "=";
        case TokenType::NotEqual:
            return "<>";
        case TokenType::Greater:
            return ">";
        case TokenType::Less:
            return "<";
        case TokenType::GreaterEqual:
            return ">=";
        case TokenType::LessEqual:
            return "<=";
        case TokenType::LeftParen:
            return "(";
        case TokenType::RightParen:
            return ")";
        case TokenType::Comma:
            return ",";
        case TokenType::PercentSuffix:
            return "%";
        case TokenType::AmpersandSuffix:
            return "&";
        case TokenType::ExclamationSuffix:
            return "!";
        case TokenType::HashSuffix:
            return "#";
        case TokenType::DollarSuffix:
            return "$";
    }
    return "unknown";
}

}  // namespace basic
