#include "basic/parser.hpp"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <limits>
#include <optional>
#include <stdexcept>
#include <utility>

#include "basic/basic_error.hpp"

namespace basic {

namespace {

std::optional<BasicType> parse_suffix(TokenStream& tokens) {
    switch (tokens.peek().type) {
        case TokenType::PercentSuffix:
            tokens.consume();
            return BasicType::Integer;
        case TokenType::AmpersandSuffix:
            tokens.consume();
            return BasicType::Long;
        case TokenType::ExclamationSuffix:
            tokens.consume();
            return BasicType::Single;
        case TokenType::HashSuffix:
            tokens.consume();
            return BasicType::Double;
        case TokenType::DollarSuffix:
            tokens.consume();
            return BasicType::String;
        default:
            return std::nullopt;
    }
}

BasicType parse_type(TokenStream& tokens) {
    switch (tokens.consume().type) {
        case TokenType::Integer:
            return BasicType::Integer;
        case TokenType::Long:
            return BasicType::Long;
        case TokenType::Single:
            return BasicType::Single;
        case TokenType::Double:
            return BasicType::Double;
        case TokenType::String:
            return BasicType::String;
        default:
            throw BasicError(ErrorCode::Syntax);
    }
}

void expect_end(TokenStream& tokens) {
    if (!tokens.at_end()) {
        throw BasicError(ErrorCode::Syntax);
    }
}

Value parse_literal(const Token& token) {
    try {
        if (token.type == TokenType::IntegerLiteral) {
            const unsigned long long parsed = std::stoull(token.text);
            if (parsed <=
                static_cast<unsigned long long>(
                    std::numeric_limits<std::int32_t>::max())) {
                return Value(static_cast<std::int32_t>(parsed));
            }
            if (parsed <=
                static_cast<unsigned long long>(
                    std::numeric_limits<std::int64_t>::max())) {
                return Value(static_cast<std::int64_t>(parsed));
            }
            throw BasicError(ErrorCode::Overflow);
        }
        if (token.type == TokenType::RealLiteral) {
            const long double parsed = std::stold(token.text);
            if (!std::isfinite(parsed) ||
                parsed > std::numeric_limits<float>::max()) {
                throw BasicError(ErrorCode::Overflow);
            }
            return Value(static_cast<float>(parsed));
        }
        if (token.type == TokenType::StringLiteral) {
            return Value(token.text);
        }
    } catch (const BasicError&) {
        throw;
    } catch (const std::exception&) {
        throw BasicError(ErrorCode::Overflow);
    }
    throw BasicError(ErrorCode::Syntax);
}

Expression* make_unary(bool negative, Expression* operand) {
    try {
        return new UnaryExpression(negative, operand);
    } catch (...) {
        delete operand;
        throw;
    }
}

Expression* make_binary(ArithmeticOperator operation, Expression* left,
                        Expression* right) {
    try {
        return new BinaryExpression(operation, left, right);
    } catch (...) {
        delete left;
        delete right;
        throw;
    }
}

Expression* make_conversion(ConversionKind kind, Expression* operand) {
    try {
        return new ConversionExpression(kind, operand);
    } catch (...) {
        delete operand;
        throw;
    }
}

class ArithmeticParser {
public:
    explicit ArithmeticParser(TokenStream& tokens) : tokens_(tokens) {}

    Expression* parse() { return parse_additive(); }

private:
    Expression* parse_additive() {
        Expression* left = parse_multiplicative();
        while (tokens_.peek().type == TokenType::Plus ||
               tokens_.peek().type == TokenType::Minus) {
            const TokenType operation = tokens_.consume().type;
            Expression* right = nullptr;
            try {
                right = parse_multiplicative();
            } catch (...) {
                delete left;
                throw;
            }
            left = make_binary(operation == TokenType::Plus
                                   ? ArithmeticOperator::Add
                                   : ArithmeticOperator::Subtract,
                               left, right);
        }
        return left;
    }

    Expression* parse_multiplicative() {
        Expression* left = parse_unary();
        while (tokens_.peek().type == TokenType::Multiply ||
               tokens_.peek().type == TokenType::Divide ||
               tokens_.peek().type == TokenType::IntegerDivide ||
               tokens_.peek().type == TokenType::Mod) {
            const TokenType operation = tokens_.consume().type;
            Expression* right = nullptr;
            try {
                right = parse_unary();
            } catch (...) {
                delete left;
                throw;
            }
            ArithmeticOperator arithmetic = ArithmeticOperator::Multiply;
            if (operation == TokenType::Divide) {
                arithmetic = ArithmeticOperator::Divide;
            } else if (operation == TokenType::IntegerDivide) {
                arithmetic = ArithmeticOperator::IntegerDivide;
            } else if (operation == TokenType::Mod) {
                arithmetic = ArithmeticOperator::Modulo;
            }
            left = make_binary(arithmetic, left, right);
        }
        return left;
    }

    Expression* parse_unary() {
        if (tokens_.match(TokenType::Plus)) {
            return make_unary(false, parse_unary());
        }
        if (tokens_.match(TokenType::Minus)) {
            return make_unary(true, parse_unary());
        }
        return parse_power();
    }

    Expression* parse_power() {
        Expression* left = parse_primary();
        if (!tokens_.match(TokenType::Power)) {
            return left;
        }
        Expression* right = nullptr;
        try {
            right = parse_unary();
        } catch (...) {
            delete left;
            throw;
        }
        return make_binary(ArithmeticOperator::Power, left, right);
    }

    Expression* parse_primary() {
        const Token token = tokens_.consume();
        if (token.type == TokenType::IntegerLiteral ||
            token.type == TokenType::RealLiteral ||
            token.type == TokenType::StringLiteral) {
            return new LiteralExpression(parse_literal(token));
        }

        if (token.type == TokenType::LeftParen) {
            Expression* expression = parse();
            try {
                tokens_.expect(TokenType::RightParen);
            } catch (...) {
                delete expression;
                throw;
            }
            return expression;
        }

        if (token.type == TokenType::CInt || token.type == TokenType::CLng ||
            token.type == TokenType::CSng || token.type == TokenType::CDbl) {
            ConversionKind kind = ConversionKind::CInt;
            if (token.type == TokenType::CLng) {
                kind = ConversionKind::CLng;
            } else if (token.type == TokenType::CSng) {
                kind = ConversionKind::CSng;
            } else if (token.type == TokenType::CDbl) {
                kind = ConversionKind::CDbl;
            }
            tokens_.expect(TokenType::LeftParen);
            Expression* operand = parse();
            try {
                tokens_.expect(TokenType::RightParen);
            } catch (...) {
                delete operand;
                throw;
            }
            return make_conversion(kind, operand);
        }

        if (token.type != TokenType::Identifier) {
            throw BasicError(ErrorCode::Syntax);
        }

        const std::optional<BasicType> suffix = parse_suffix(tokens_);
        std::vector<Expression*> indices;
        try {
            if (tokens_.match(TokenType::LeftParen)) {
                if (tokens_.peek().type == TokenType::RightParen) {
                    throw BasicError(ErrorCode::Syntax);
                }
                indices.push_back(parse());
                while (tokens_.match(TokenType::Comma)) {
                    indices.push_back(parse());
                }
                tokens_.expect(TokenType::RightParen);
            }
            return new VariableExpression(token.text, suffix,
                                          std::move(indices));
        } catch (...) {
            for (Expression* expression : indices) {
                delete expression;
            }
            throw;
        }
    }

    TokenStream& tokens_;
};

ComparisonOperator parse_comparison_operator(TokenStream& tokens) {
    switch (tokens.consume().type) {
        case TokenType::Equal:
            return ComparisonOperator::Equal;
        case TokenType::NotEqual:
            return ComparisonOperator::NotEqual;
        case TokenType::Greater:
            return ComparisonOperator::Greater;
        case TokenType::Less:
            return ComparisonOperator::Less;
        case TokenType::GreaterEqual:
            return ComparisonOperator::GreaterEqual;
        case TokenType::LessEqual:
            return ComparisonOperator::LessEqual;
        default:
            throw BasicError(ErrorCode::Syntax);
    }
}

bool is_comparison(TokenType type) {
    return type == TokenType::Equal || type == TokenType::NotEqual ||
           type == TokenType::Greater || type == TokenType::Less ||
           type == TokenType::GreaterEqual || type == TokenType::LessEqual;
}

bool parenthesis_contains_condition(const TokenStream& tokens) {
    if (tokens.peek().type != TokenType::LeftParen) {
        return false;
    }
    std::size_t depth = 0;
    const std::vector<Token>& all = tokens.tokens();
    for (std::size_t index = tokens.position(); index < all.size(); ++index) {
        const TokenType type = all[index].type;
        if (type == TokenType::LeftParen) {
            ++depth;
        } else if (type == TokenType::RightParen) {
            if (--depth == 0) {
                return false;
            }
        } else if (depth > 0 &&
                   (is_comparison(type) || type == TokenType::Not ||
                    type == TokenType::And || type == TokenType::Or)) {
            return true;
        }
    }
    throw BasicError(ErrorCode::Syntax);
}

Condition* make_comparison(ComparisonOperator operation, Expression* left,
                           Expression* right) {
    try {
        return new ComparisonCondition(operation, left, right);
    } catch (...) {
        delete left;
        delete right;
        throw;
    }
}

Condition* make_not(Condition* operand) {
    try {
        return new NotCondition(operand);
    } catch (...) {
        delete operand;
        throw;
    }
}

Condition* make_logical(LogicalOperator operation, Condition* left,
                        Condition* right) {
    try {
        return new LogicalCondition(operation, left, right);
    } catch (...) {
        delete left;
        delete right;
        throw;
    }
}

class LogicalParser {
public:
    explicit LogicalParser(TokenStream& tokens) : tokens_(tokens) {}

    Condition* parse() { return parse_or(); }

private:
    Condition* parse_or() {
        Condition* left = parse_and();
        while (tokens_.match(TokenType::Or)) {
            Condition* right = nullptr;
            try {
                right = parse_and();
            } catch (...) {
                delete left;
                throw;
            }
            left = make_logical(LogicalOperator::Or, left, right);
        }
        return left;
    }

    Condition* parse_and() {
        Condition* left = parse_not();
        while (tokens_.match(TokenType::And)) {
            Condition* right = nullptr;
            try {
                right = parse_not();
            } catch (...) {
                delete left;
                throw;
            }
            left = make_logical(LogicalOperator::And, left, right);
        }
        return left;
    }

    Condition* parse_not() {
        if (tokens_.match(TokenType::Not)) {
            return make_not(parse_not());
        }
        if (parenthesis_contains_condition(tokens_)) {
            tokens_.consume();
            Condition* condition = parse_or();
            try {
                tokens_.expect(TokenType::RightParen);
            } catch (...) {
                delete condition;
                throw;
            }
            return condition;
        }
        return parse_comparison();
    }

    Condition* parse_comparison() {
        ArithmeticParser arithmetic(tokens_);
        Expression* left = arithmetic.parse();
        if (!is_comparison(tokens_.peek().type)) {
            delete left;
            throw BasicError(ErrorCode::Syntax);
        }
        const ComparisonOperator operation =
            parse_comparison_operator(tokens_);
        Expression* right = nullptr;
        try {
            right = arithmetic.parse();
        } catch (...) {
            delete left;
            throw;
        }
        return make_comparison(operation, left, right);
    }

    TokenStream& tokens_;
};

AssignmentTarget parse_target(TokenStream& tokens) {
    const Token name = tokens.consume();
    if (name.type != TokenType::Identifier) {
        throw BasicError(ErrorCode::Syntax);
    }
    const std::optional<BasicType> suffix = parse_suffix(tokens);
    std::vector<Expression*> indices;
    try {
        if (tokens.match(TokenType::LeftParen)) {
            ArithmeticParser arithmetic(tokens);
            indices.push_back(arithmetic.parse());
            while (tokens.match(TokenType::Comma)) {
                indices.push_back(arithmetic.parse());
            }
            tokens.expect(TokenType::RightParen);
        }
        return AssignmentTarget(name.text, suffix, std::move(indices));
    } catch (...) {
        for (Expression* expression : indices) {
            delete expression;
        }
        throw;
    }
}

Statement* parse_dim(TokenStream& tokens, const std::string& source) {
    tokens.expect(TokenType::Dim);
    const Token name = tokens.consume();
    if (name.type != TokenType::Identifier) {
        throw BasicError(ErrorCode::Syntax);
    }
    std::optional<BasicType> declared_type = parse_suffix(tokens);
    std::vector<Expression*> dimensions;
    bool dynamic = false;
    Expression* initializer = nullptr;
    try {
        if (tokens.match(TokenType::LeftParen)) {
            if (tokens.match(TokenType::RightParen)) {
                dynamic = true;
            } else {
                ArithmeticParser arithmetic(tokens);
                dimensions.push_back(arithmetic.parse());
                while (tokens.match(TokenType::Comma)) {
                    dimensions.push_back(arithmetic.parse());
                }
                tokens.expect(TokenType::RightParen);
            }
        }
        if (tokens.match(TokenType::As)) {
            const BasicType explicit_type = parse_type(tokens);
            if (declared_type.has_value() &&
                declared_type.value() != explicit_type) {
                throw BasicError(ErrorCode::Syntax);
            }
            declared_type = explicit_type;
        }
        if (tokens.match(TokenType::Equal)) {
            if (dynamic || !dimensions.empty()) {
                throw BasicError(ErrorCode::Syntax);
            }
            ArithmeticParser arithmetic(tokens);
            initializer = arithmetic.parse();
        }
        expect_end(tokens);
        return new DimStatement(source, name.text, declared_type,
                                std::move(dimensions), dynamic, initializer);
    } catch (...) {
        for (Expression* expression : dimensions) {
            delete expression;
        }
        delete initializer;
        throw;
    }
}

Statement* parse_simple(TokenStream& tokens, const std::string& source) {
    switch (tokens.peek().type) {
        case TokenType::Rem: {
            tokens.consume();
            if (tokens.peek().type == TokenType::RemText) {
                tokens.consume();
            }
            expect_end(tokens);
            return new RemStatement(source);
        }
        case TokenType::Dim:
            return parse_dim(tokens, source);
        case TokenType::Let: {
            tokens.consume();
            AssignmentTarget target = parse_target(tokens);
            tokens.expect(TokenType::Equal);
            ArithmeticParser arithmetic(tokens);
            Expression* expression = arithmetic.parse();
            try {
                expect_end(tokens);
                return new LetStatement(source, std::move(target), expression);
            } catch (...) {
                delete expression;
                throw;
            }
        }
        case TokenType::Print: {
            tokens.consume();
            ArithmeticParser arithmetic(tokens);
            Expression* expression = arithmetic.parse();
            try {
                expect_end(tokens);
                return new PrintStatement(source, expression);
            } catch (...) {
                delete expression;
                throw;
            }
        }
        case TokenType::Input: {
            tokens.consume();
            AssignmentTarget target = parse_target(tokens);
            expect_end(tokens);
            return new InputStatement(source, std::move(target));
        }
        case TokenType::End:
            tokens.consume();
            expect_end(tokens);
            return new EndStatement(source);
        case TokenType::Redim: {
            tokens.consume();
            const bool preserve = tokens.match(TokenType::Preserve);
            const Token name = tokens.consume();
            if (name.type != TokenType::Identifier) {
                throw BasicError(ErrorCode::Syntax);
            }
            parse_suffix(tokens);
            tokens.expect(TokenType::LeftParen);
            ArithmeticParser arithmetic(tokens);
            Expression* length = arithmetic.parse();
            try {
                tokens.expect(TokenType::RightParen);
                expect_end(tokens);
                return new RedimStatement(source, name.text, length, preserve);
            } catch (...) {
                delete length;
                throw;
            }
        }
        case TokenType::Erase: {
            tokens.consume();
            const Token name = tokens.consume();
            if (name.type != TokenType::Identifier) {
                throw BasicError(ErrorCode::Syntax);
            }
            parse_suffix(tokens);
            expect_end(tokens);
            return new EraseStatement(source, name.text);
        }
        default:
            throw BasicError(ErrorCode::Syntax);
    }
}

Condition* parse_if_header(TokenStream& tokens, TokenType keyword) {
    tokens.expect(keyword);
    LogicalParser logical(tokens);
    Condition* condition = logical.parse();
    try {
        tokens.expect(TokenType::Then);
        expect_end(tokens);
    } catch (...) {
        delete condition;
        throw;
    }
    return condition;
}

struct ForHeader {
    std::string variable;
    Expression* start{nullptr};
    Expression* end{nullptr};
    Expression* step{nullptr};
};

void clear_for_header(ForHeader& header) {
    delete header.start;
    delete header.end;
    delete header.step;
    header.start = nullptr;
    header.end = nullptr;
    header.step = nullptr;
}

ForHeader parse_for_header(TokenStream& tokens) {
    ForHeader header;
    tokens.expect(TokenType::For);
    const Token variable = tokens.consume();
    if (variable.type != TokenType::Identifier) {
        throw BasicError(ErrorCode::Syntax);
    }
    header.variable = variable.text;
    parse_suffix(tokens);
    tokens.expect(TokenType::Equal);
    try {
        ArithmeticParser arithmetic(tokens);
        header.start = arithmetic.parse();
        tokens.expect(TokenType::To);
        header.end = arithmetic.parse();
        if (tokens.match(TokenType::Step)) {
            header.step = arithmetic.parse();
        }
        expect_end(tokens);
        return header;
    } catch (...) {
        clear_for_header(header);
        throw;
    }
}

Condition* parse_while_header(TokenStream& tokens) {
    tokens.expect(TokenType::While);
    LogicalParser logical(tokens);
    Condition* condition = logical.parse();
    try {
        expect_end(tokens);
    } catch (...) {
        delete condition;
        throw;
    }
    return condition;
}

std::string parse_next(TokenStream& tokens) {
    tokens.expect(TokenType::Next);
    const Token variable = tokens.consume();
    if (variable.type != TokenType::Identifier) {
        throw BasicError(ErrorCode::Syntax);
    }
    parse_suffix(tokens);
    expect_end(tokens);
    return variable.text;
}

void validate_program_line(const Lexer& lexer, const std::string& source) {
    TokenStream tokens = lexer.tokenize(source);
    switch (tokens.peek().type) {
        case TokenType::If: {
            delete parse_if_header(tokens, TokenType::If);
            return;
        }
        case TokenType::ElseIf: {
            delete parse_if_header(tokens, TokenType::ElseIf);
            return;
        }
        case TokenType::Else:
            tokens.consume();
            expect_end(tokens);
            return;
        case TokenType::For: {
            ForHeader header = parse_for_header(tokens);
            clear_for_header(header);
            return;
        }
        case TokenType::Next:
            parse_next(tokens);
            return;
        case TokenType::While: {
            delete parse_while_header(tokens);
            return;
        }
        case TokenType::Wend:
            tokens.consume();
            expect_end(tokens);
            return;
        case TokenType::Exit:
            tokens.consume();
            tokens.expect(TokenType::For);
            expect_end(tokens);
            return;
        case TokenType::End:
            if (tokens.peek(1).type == TokenType::If) {
                tokens.consume();
                tokens.consume();
                expect_end(tokens);
                return;
            }
            break;
        default:
            break;
    }
    delete parse_simple(tokens, source);
}

enum class StopKind {
    Root,
    If,
    For,
    While
};

bool is_if_stop(const TokenStream& tokens) {
    return tokens.peek().type == TokenType::ElseIf ||
           tokens.peek().type == TokenType::Else ||
           (tokens.peek().type == TokenType::End &&
            tokens.peek(1).type == TokenType::If);
}

bool is_stop(const TokenStream& tokens, StopKind stop) {
    if (stop == StopKind::If) {
        return is_if_stop(tokens);
    }
    if (stop == StopKind::For) {
        return tokens.peek().type == TokenType::Next;
    }
    if (stop == StopKind::While) {
        return tokens.peek().type == TokenType::Wend;
    }
    return false;
}

bool is_unexpected_closer(const TokenStream& tokens) {
    return is_if_stop(tokens) || tokens.peek().type == TokenType::Next ||
           tokens.peek().type == TokenType::Wend;
}

void append_owned(BlockStatement* block, Statement* statement) {
    try {
        block->append(statement);
    } catch (...) {
        delete statement;
        throw;
    }
}

void delete_branches(std::vector<IfBranch>& branches) {
    for (IfBranch& branch : branches) {
        delete branch.condition;
        delete branch.body;
    }
    branches.clear();
}

BlockStatement* parse_block(const std::vector<SourceLine>& lines,
                            std::size_t& index, StopKind stop,
                            std::size_t loop_depth, const Lexer& lexer) {
    BlockStatement* block = new BlockStatement();
    try {
        while (index < lines.size()) {
            TokenStream tokens = lexer.tokenize(lines[index].source);
            if (tokens.at_end()) {
                throw BasicError(ErrorCode::Syntax);
            }
            if (is_stop(tokens, stop)) {
                return block;
            }
            if (is_unexpected_closer(tokens)) {
                throw BasicError(ErrorCode::Syntax);
            }

            if (tokens.peek().type == TokenType::If) {
                const std::string source = lines[index].source;
                std::vector<IfBranch> branches;
                try {
                    Condition* condition =
                        parse_if_header(tokens, TokenType::If);
                    try {
                        branches.push_back({condition, nullptr});
                    } catch (...) {
                        delete condition;
                        throw;
                    }
                    ++index;
                    BlockStatement* body = parse_block(
                        lines, index, StopKind::If, loop_depth, lexer);
                    branches.back().body = body;

                    bool saw_else = false;
                    while (index < lines.size()) {
                        TokenStream marker = lexer.tokenize(lines[index].source);
                        if (marker.peek().type == TokenType::ElseIf &&
                            !saw_else) {
                            condition = parse_if_header(marker,
                                                        TokenType::ElseIf);
                            try {
                                branches.push_back({condition, nullptr});
                            } catch (...) {
                                delete condition;
                                throw;
                            }
                            ++index;
                            body = parse_block(lines, index, StopKind::If,
                                               loop_depth, lexer);
                            branches.back().body = body;
                            continue;
                        }
                        if (marker.peek().type == TokenType::Else && !saw_else) {
                            marker.consume();
                            expect_end(marker);
                            saw_else = true;
                            branches.push_back({nullptr, nullptr});
                            ++index;
                            body = parse_block(lines, index, StopKind::If,
                                               loop_depth, lexer);
                            branches.back().body = body;
                            continue;
                        }
                        if (marker.peek().type == TokenType::End &&
                            marker.peek(1).type == TokenType::If) {
                            marker.consume();
                            marker.consume();
                            expect_end(marker);
                            ++index;
                            break;
                        }
                        throw BasicError(ErrorCode::Syntax);
                    }
                    if (index == lines.size()) {
                        const TokenStream last =
                            lexer.tokenize(lines.back().source);
                        if (!(last.peek().type == TokenType::End &&
                              last.peek(1).type == TokenType::If)) {
                            throw BasicError(ErrorCode::Syntax);
                        }
                    }
                    append_owned(block,
                                 new IfStatement(source, std::move(branches)));
                } catch (...) {
                    delete_branches(branches);
                    throw;
                }
                continue;
            }

            if (tokens.peek().type == TokenType::For) {
                const std::string source = lines[index].source;
                ForHeader header = parse_for_header(tokens);
                ++index;
                BlockStatement* body = nullptr;
                try {
                    body = parse_block(lines, index, StopKind::For,
                                       loop_depth + 1, lexer);
                    if (index >= lines.size()) {
                        throw BasicError(ErrorCode::Syntax);
                    }
                    TokenStream marker = lexer.tokenize(lines[index].source);
                    if (parse_next(marker) != header.variable) {
                        throw BasicError(ErrorCode::Syntax);
                    }
                    ++index;
                    Statement* statement = new ForStatement(
                        source, header.variable, header.start, header.end,
                        header.step, body);
                    header.start = nullptr;
                    header.end = nullptr;
                    header.step = nullptr;
                    body = nullptr;
                    append_owned(block, statement);
                } catch (...) {
                    clear_for_header(header);
                    delete body;
                    throw;
                }
                continue;
            }

            if (tokens.peek().type == TokenType::While) {
                const std::string source = lines[index].source;
                Condition* condition = parse_while_header(tokens);
                ++index;
                BlockStatement* body = nullptr;
                try {
                    body = parse_block(lines, index, StopKind::While,
                                       loop_depth, lexer);
                    if (index >= lines.size()) {
                        throw BasicError(ErrorCode::Syntax);
                    }
                    TokenStream marker = lexer.tokenize(lines[index].source);
                    marker.expect(TokenType::Wend);
                    expect_end(marker);
                    ++index;
                    Statement* statement =
                        new WhileStatement(source, condition, body);
                    condition = nullptr;
                    body = nullptr;
                    append_owned(block, statement);
                } catch (...) {
                    delete condition;
                    delete body;
                    throw;
                }
                continue;
            }

            if (tokens.peek().type == TokenType::Exit) {
                if (loop_depth == 0) {
                    throw BasicError(ErrorCode::Syntax);
                }
                tokens.consume();
                tokens.expect(TokenType::For);
                expect_end(tokens);
                append_owned(block, new ExitForStatement(lines[index].source));
                ++index;
                continue;
            }

            append_owned(block, parse_simple(tokens, lines[index].source));
            ++index;
        }
        if (stop != StopKind::Root) {
            throw BasicError(ErrorCode::Syntax);
        }
        return block;
    } catch (...) {
        delete block;
        throw;
    }
}

InterpreterCommand command_from_token(TokenType type) {
    switch (type) {
        case TokenType::Run:
            return InterpreterCommand::Run;
        case TokenType::List:
            return InterpreterCommand::List;
        case TokenType::Clear:
            return InterpreterCommand::Clear;
        case TokenType::Quit:
            return InterpreterCommand::Quit;
        case TokenType::Help:
            return InterpreterCommand::Help;
        default:
            throw BasicError(ErrorCode::Syntax);
    }
}

bool is_command(TokenType type) {
    return type == TokenType::Run || type == TokenType::List ||
           type == TokenType::Clear || type == TokenType::Quit ||
           type == TokenType::Help;
}

bool is_immediate_keyword(TokenType type) {
    return type == TokenType::Rem || type == TokenType::Dim ||
           type == TokenType::Let || type == TokenType::Print ||
           type == TokenType::Input || type == TokenType::Redim ||
           type == TokenType::Erase;
}

}  // namespace

ParsedInput::ParsedInput() = default;
ParsedInput::~ParsedInput() { delete statement_; }

ParsedInput::ParsedInput(ParsedInput&& other) noexcept
    : kind_(other.kind_),
      command_(other.command_),
      line_number_(other.line_number_),
      source_(std::move(other.source_)),
      statement_(other.statement_) {
    other.kind_ = ParsedInputKind::Empty;
    other.statement_ = nullptr;
}

ParsedInput& ParsedInput::operator=(ParsedInput&& other) noexcept {
    if (this != &other) {
        delete statement_;
        kind_ = other.kind_;
        command_ = other.command_;
        line_number_ = other.line_number_;
        source_ = std::move(other.source_);
        statement_ = other.statement_;
        other.kind_ = ParsedInputKind::Empty;
        other.statement_ = nullptr;
    }
    return *this;
}

ParsedInput ParsedInput::command(InterpreterCommand command) {
    ParsedInput result;
    result.kind_ = ParsedInputKind::Command;
    result.command_ = command;
    return result;
}

ParsedInput ParsedInput::program_line(std::int32_t line_number,
                                      std::string source) {
    ParsedInput result;
    result.kind_ = ParsedInputKind::ProgramLine;
    result.line_number_ = line_number;
    result.source_ = std::move(source);
    return result;
}

ParsedInput ParsedInput::delete_line(std::int32_t line_number) {
    ParsedInput result;
    result.kind_ = ParsedInputKind::DeleteLine;
    result.line_number_ = line_number;
    return result;
}

ParsedInput ParsedInput::immediate(Statement* statement) {
    ParsedInput result;
    result.kind_ = ParsedInputKind::ImmediateStatement;
    result.statement_ = statement;
    return result;
}

ParsedInputKind ParsedInput::kind() const noexcept { return kind_; }

InterpreterCommand ParsedInput::command_value() const {
    if (kind_ != ParsedInputKind::Command) {
        throw std::logic_error("ParsedInput is not a command");
    }
    return command_;
}

std::int32_t ParsedInput::line_number() const {
    if (kind_ != ParsedInputKind::ProgramLine &&
        kind_ != ParsedInputKind::DeleteLine) {
        throw std::logic_error("ParsedInput has no line number");
    }
    return line_number_;
}

const std::string& ParsedInput::source() const noexcept { return source_; }

Statement* ParsedInput::release_statement() noexcept {
    Statement* statement = statement_;
    statement_ = nullptr;
    return statement;
}

ParsedInput Parser::parse_input(const std::string& source) const {
    std::size_t first = 0;
    while (first < source.size() &&
           std::isspace(static_cast<unsigned char>(source[first])) != 0) {
        ++first;
    }
    if (first == source.size()) {
        return ParsedInput();
    }

    if (std::isdigit(static_cast<unsigned char>(source[first])) != 0) {
        std::size_t after_number = first;
        while (after_number < source.size() &&
               std::isdigit(
                   static_cast<unsigned char>(source[after_number])) != 0) {
            ++after_number;
        }
        if (after_number < source.size() &&
            std::isspace(static_cast<unsigned char>(source[after_number])) == 0) {
            throw BasicError(ErrorCode::Syntax);
        }
        unsigned long long parsed = 0;
        try {
            parsed = std::stoull(source.substr(first, after_number - first));
        } catch (const std::exception&) {
            throw BasicError(ErrorCode::Overflow);
        }
        if (parsed > static_cast<unsigned long long>(
                         std::numeric_limits<std::int32_t>::max())) {
            throw BasicError(ErrorCode::Syntax);
        }
        const std::size_t separator = after_number;
        while (after_number < source.size() &&
               std::isspace(
                   static_cast<unsigned char>(source[after_number])) != 0) {
            ++after_number;
        }
        const std::int32_t line_number = static_cast<std::int32_t>(parsed);
        if (after_number == source.size()) {
            return ParsedInput::delete_line(line_number);
        }
        const std::string statement_source = source.substr(separator + 1);
        validate_program_line(Lexer(), statement_source);
        return ParsedInput::program_line(line_number, statement_source);
    }

    TokenStream tokens = Lexer().tokenize(source.substr(first));
    if (is_command(tokens.peek().type)) {
        const InterpreterCommand command =
            command_from_token(tokens.consume().type);
        expect_end(tokens);
        return ParsedInput::command(command);
    }
    if (!is_immediate_keyword(tokens.peek().type)) {
        throw BasicError(ErrorCode::Syntax);
    }
    return ParsedInput::immediate(parse_simple(tokens, source.substr(first)));
}

ProgramAst* Parser::parse_program(const std::vector<SourceLine>& lines) const {
    for (std::size_t index = 0; index < lines.size(); ++index) {
        if (lines[index].line_number < 0 ||
            (index > 0 && lines[index - 1].line_number >=
                              lines[index].line_number)) {
            throw BasicError(ErrorCode::Syntax);
        }
    }
    std::size_t index = 0;
    BlockStatement* root = parse_block(lines, index, StopKind::Root, 0, Lexer());
    try {
        return new ProgramAst(root);
    } catch (...) {
        delete root;
        throw;
    }
}

Statement* Parser::parse_simple_statement(TokenStream& tokens,
                                          const std::string& source) const {
    return parse_simple(tokens, source);
}

Expression* Parser::parse_expression(TokenStream& tokens, int) const {
    ArithmeticParser arithmetic(tokens);
    return arithmetic.parse();
}

Condition* Parser::parse_condition(TokenStream& tokens) const {
    LogicalParser logical(tokens);
    return logical.parse();
}

}  // namespace basic
