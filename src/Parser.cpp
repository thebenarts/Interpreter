#include "Parser.h"
#include "Logger.h"
#include "Objects.h"
#include <format>

namespace interpreter
{
    Parser::Parser(LexerUniquePtr lexer) : mLexer(std::move(lexer))
    {
        VERIFY(!mLexer->mTokens.empty())
        {
            mCurrent = mLexer->mTokens.begin();
            mNext = mCurrent + 1;
            mEnd = mLexer->mTokens.end();

            RegisterParseFunctionPointers();
        }
    }

    void Parser::RegisterParseFunctionPointers()
    {
        const auto RegisterPrefixFunctionPtr = [this](TokenType tokenType, PrefixParseFunctionPtr functionPtr) {
            mPrefixFunctionPtrMap.insert_or_assign(tokenType, functionPtr);
        };

        const auto RegisterInfixFunctionPtr = [this](TokenType tokenType, InfixParseFunctionPtr functionPtr) {
            mInfixFunctionPtrMap.insert_or_assign(tokenType, functionPtr);
        };

        // Register Prefix Function pointers
        RegisterPrefixFunctionPtr(TokenType::TRUE, std::bind(&Parser::ParsePrimitiveExpression, this));
        RegisterPrefixFunctionPtr(TokenType::FALSE, std::bind(&Parser::ParsePrimitiveExpression, this));
        RegisterPrefixFunctionPtr(TokenType::IDENT, std::bind(&Parser::ParsePrimitiveExpression, this));
        RegisterPrefixFunctionPtr(TokenType::INT, std::bind(&Parser::ParsePrimitiveExpression, this));
        RegisterPrefixFunctionPtr(TokenType::BANG, std::bind(&Parser::ParsePrefixExpression, this));
        RegisterPrefixFunctionPtr(TokenType::MINUS, std::bind(&Parser::ParsePrefixExpression, this));
        RegisterPrefixFunctionPtr(TokenType::LPAREN, std::bind(&Parser::ParseGroupedExpression, this));
        RegisterPrefixFunctionPtr(TokenType::IF, std::bind(&Parser::ParseIfExpression, this));
        RegisterPrefixFunctionPtr(TokenType::FUNCTION, std::bind(&Parser::ParseFunctionExpression, this));

        // Register Infix Function pointers
        {
            using namespace std::placeholders;

            RegisterInfixFunctionPtr(TokenType::PLUS, std::bind(&Parser::ParseInfixExpression, this, _1));
            RegisterInfixFunctionPtr(TokenType::MINUS, std::bind(&Parser::ParseInfixExpression, this, _1));
            RegisterInfixFunctionPtr(TokenType::SLASH, std::bind(&Parser::ParseInfixExpression, this, _1));
            RegisterInfixFunctionPtr(TokenType::ASTERISK, std::bind(&Parser::ParseInfixExpression, this, _1));
            RegisterInfixFunctionPtr(TokenType::EQ, std::bind(&Parser::ParseInfixExpression, this, _1));
            RegisterInfixFunctionPtr(TokenType::NOT_EQ, std::bind(&Parser::ParseInfixExpression, this, _1));
            RegisterInfixFunctionPtr(TokenType::LT, std::bind(&Parser::ParseInfixExpression, this, _1));
            RegisterInfixFunctionPtr(TokenType::GT, std::bind(&Parser::ParseInfixExpression, this, _1));
            RegisterInfixFunctionPtr(TokenType::LPAREN, std::bind(&Parser::ParseCallExpression, this, _1));
        }

    }

    void Parser::AdvanceToken()
    {
        if (mNext == mEnd)
        {
            return;
        }

        mCurrent = mNext;
        mNext++;
    }

    const Token* Parser::GetCurrentTokenAndAdvance()
    {
        const Token* token{ GetCurrentToken() };
        AdvanceToken();
        return token;
    }

    const Token* Parser::GetCurrentToken()
    {
        if (mCurrent != mEnd)
        {
            return &(*mCurrent);
        }

        return nullptr;
    }

    const Token* Parser::GetNextToken()
    {
        if (mNext != mEnd)
        {
            return &(*mNext);
        }

        return nullptr;
    }

    std::tuple<const Token*, const Token*> Parser::GetTokens()
    {
        return { GetCurrentToken(),GetNextToken() };
    }

    ProgramUniquePtr Parser::ParseProgram()
    {
        auto program{ std::make_unique<ast::Program>() };
        while (GetCurrentToken() && !CurrentTokenIs(TokenType::ENDF))
        {
            StatementUniquePtr statement{ ParseStatement() };
            if (statement)
            {
                program->mStatements.push_back(std::move(statement));
            }
            AdvanceToken();
        }

        return program;
    }

    StatementUniquePtr Parser::ParseStatement()
    {
        const Token* token{ GetCurrentToken() };
        VERIFY(token)
        {
            switch (token->mType)
            {
            case TokenType::LET:
                return ParseLetStatement();
                break;
            case TokenType::RETURN:
                return ParseReturnStatement();
                break;
            default:
                return ParseExpressionStatement();
                break;
            }
        }
        return nullptr;
    }

    LetStatementUniquePtr Parser::ParseLetStatement()
    {
        auto statement{ std::make_unique<ast::LetStatement>() };
        // We only land here after checking the CurrentToken in ParseStatement so it is safe to dereference without checking here 
        statement->mToken = *GetCurrentToken(); // "let" token

        if (!ExpectNextTokenIs(TokenType::IDENT))
        {
            return nullptr;
        }

        VERIFY(GetNextToken())
        {
            AdvanceToken();
            statement->mIdentifier = std::move(ParsePrimitiveExpression()); // Identifier
        }

        if (!ExpectNextTokenIs(TokenType::ASSIGN))
        {
            return nullptr;
        }
        AdvanceToken(); // Current Token is now on =

        AdvanceToken(); // Current Token is now on the first token of the expression
        statement->mValue = ParseExpression(ast::Precedence::LOWEST);

        if (NextTokenIs(TokenType::SEMICOLON))
        {
            AdvanceToken();
        }

        return statement;
    }

    ReturnStatementUniquePtr Parser::ParseReturnStatement()
    {
        auto statement{ std::make_unique<ast::ReturnStatement>() };
        // We only land here after checking the CurrentToken in ParseStatement so it is safe to dereference without checking here 
        statement->mToken = *GetCurrentToken(); // "return" token

        // TODO: implement logic here to handle expressions
        AdvanceToken(); // advance to the first token of the Expression

        statement->mValue = ParseExpression(ast::Precedence::LOWEST);

        if (NextTokenIs(TokenType::SEMICOLON))
        {
            AdvanceToken();
        }

        return statement;
    }

    ExpressionStatementUniquePtr Parser::ParseExpressionStatement()
    {
        auto statement{ std::make_unique<ast::ExpressionStatement>() };
        statement->mToken = *GetCurrentToken();
        // TODO: Figure out a way so we don't store the same data twice... Essentially mToken is useless here.
        statement->mValue = std::move(ParseExpression(ast::LOWEST));

        if (NextTokenIs(TokenType::SEMICOLON))
        {
            AdvanceToken();
        }

        return statement;
    }

    BlockStatementUniquePtr Parser::ParseBlockStatement()
    {
        auto blockStatement{ std::make_unique<ast::BlockStatement>() };
        blockStatement->mToken = *GetCurrentToken(); // Should be "{"

        AdvanceToken();

        while (GetCurrentToken() && !CurrentTokenIs(TokenType::RBRACE) && !CurrentTokenIs(TokenType::ENDF))
        {
            auto statement{ ParseStatement() };
            if (statement)
            {
                blockStatement->mStatements.push_back(std::move(statement));
            }

            AdvanceToken();
        }

        return blockStatement;
    }

    ConditionBlockStatementUniquePtr Parser::ParseConditionBlockStatement()
    {
        auto conditionBlockStatement{ std::make_unique<ast::ConditionBlockStatement>() };
        if (!ExpectNextTokenIs(TokenType::LPAREN))
        {
            return nullptr;
        }
        conditionBlockStatement->mToken = *GetCurrentToken();   // "if" || "else if" token
        AdvanceToken(); // Advance from "if" || "else if" token -> "(" 
        AdvanceToken(); // Advance from "{" token -> ? we expect an expression to be here as the condition
        conditionBlockStatement->mCondition = ParseExpression(ast::Precedence::LOWEST);

        if (!ExpectNextTokenIs(TokenType::RPAREN))
        {
            return nullptr;
        }
        AdvanceToken(); // Advance to ")"

        if (!ExpectNextTokenIs(TokenType::LBRACE))
        {
            return nullptr;
        }
        AdvanceToken(); // Advance from ")" -> "{"
        conditionBlockStatement->mBlock = ParseBlockStatement();

        return conditionBlockStatement;
    }

    ExpressionUniquePtr Parser::ParseExpression(ast::Precedence precedence)
    {
        if (const Token * token{ GetCurrentToken() })
        {
            ExpressionUniquePtr expression;

            if (mPrefixFunctionPtrMap.contains(token->mType))
            {
                PrefixParseFunctionPtr prefixFunctionPtr{ mPrefixFunctionPtrMap.at(token->mType) };
                expression = prefixFunctionPtr();
            }
            else
            {
                LOG(MessageType::ERRORS, "type: ", token->mType,  " doesn't have a function associtated with it.");
            }

            // Added the safe check of GetNextToken() to safeguard the NextTokenIs(), technically speaking we wouldn't need either but it's more readable this way.
            while (GetNextToken() && !NextTokenIs(TokenType::SEMICOLON) && precedence < GetNextPrecedence())
            {
                const auto nextToken{ GetNextToken() };
                if (!nextToken)
                {
                    return expression;
                }

                const auto infixExpressionFunctionIterator{ mInfixFunctionPtrMap.find(nextToken->mType) };
                if (infixExpressionFunctionIterator == mInfixFunctionPtrMap.end())
                {
                    return expression;
                }

                AdvanceToken();

                expression = infixExpressionFunctionIterator->second(std::move(expression));
            }

            return expression;
        }

        return nullptr;
    }

    ExpressionUniquePtr Parser::ParsePrimitiveExpression()
    {
        auto expression{ std::make_unique<ast::PrimitiveExpression>() };
        // Pointer check done in ParseExpression
        expression->mToken = *GetCurrentToken();
        switch (expression->mToken.mType)
        {
        case TokenType::IDENT:
            expression->mExpressionType = ast::ExpressionType::IdentifierExpression;
            break;
        case TokenType::INT:
            expression->mExpressionType = ast::ExpressionType::IntegerExpression;
            break;
        case TokenType::TRUE: [[fall_through]]
        case TokenType::FALSE:
            expression->mExpressionType = ast::ExpressionType::BooleanExpression;
            break;
        }
        return expression;
    }

    ExpressionUniquePtr Parser::ParsePrefixExpression()
    {
        auto expression{ std::make_unique<ast::PrefixExpression>() };
        expression->mExpressionType = ast::ExpressionType::PrefixExpression;
        // Pointer check done in ParseExpression
        expression->mToken = *GetCurrentToken();
        expression->mOperator = *GetCurrentToken();

        AdvanceToken();
        expression->mRightSideValue = ParseExpression(ast::PREFIX);

        return expression;
    }

    ExpressionUniquePtr Parser::ParseInfixExpression(ExpressionUniquePtr leftExpression)
    {
        auto expression{ std::make_unique<ast::InfixExpression>() };
        expression->mExpressionType = ast::ExpressionType::InfixExpression;

        VERIFY(leftExpression)
        {
            expression->mLeftExpression = std::move(leftExpression);
        }

        // TODO: Once the parser is done confirm that this function only get's called from ParseExpression which means we can forego the check
        VERIFY(GetCurrentToken())
        {
            expression->mToken = *GetCurrentToken();
        }

        auto precedence{ GetCurrentPrecedence() };
        AdvanceToken();
        expression->mRightExpression = ParseExpression(precedence);

        return expression;
    }

    ExpressionUniquePtr Parser::ParseGroupedExpression()
    {
        AdvanceToken(); // Advance past (

        ExpressionUniquePtr expression{ ParseExpression(ast::Precedence::LOWEST) };

        if (!NextTokenIs(TokenType::RPAREN))
        {
            return nullptr;
        }
        AdvanceToken(); // Advance Past )

        return expression;
    }

    ExpressionUniquePtr Parser::ParseIfExpression()
    {
        auto expression{ std::make_unique<ast::IfExpression>() };
        if (!ExpectNextTokenIs(TokenType::LPAREN))
        {
            return nullptr;
        }
        // Parse If condition block
        expression->mIfConditionBlock = ParseConditionBlockStatement();

        while (GetNextToken() && NextTokenIs(TokenType::ELSE_IF))
        {
            AdvanceToken(); // Advance from "}" -> "else if"
            ConditionBlockStatementUniquePtr elseIfStatementBlock{ ParseConditionBlockStatement() };
            if (elseIfStatementBlock)
            {
                expression->mElseIfBlocks.push_back(std::move(elseIfStatementBlock));
            }
        }

        if (NextTokenIs(TokenType::ELSE))
        {
            AdvanceToken(); // Advance from "}" -> "else"

            if (!ExpectNextTokenIs(TokenType::LBRACE))
            {
                return nullptr;
            }
            AdvanceToken(); // Advance from "else" -> "{" 
            expression->mAlternative = ParseBlockStatement();
        }

        return expression;
    }

    ExpressionUniquePtr Parser::ParseFunctionExpression()
    {
        auto functionExpression{ std::make_unique<ast::FunctionExpression>() };
        // The token has been checked in ParseExpression(); no need to check before dereferencing
        functionExpression->mToken = *GetCurrentToken();    // should be "fn"

        if (!ExpectNextTokenIs(TokenType::LPAREN))
        {
            return nullptr;
        }
        AdvanceToken(); // "fn" -> "("
        functionExpression->mParameters = std::move(ParseFunctionParameters()); // Should leave with CurrentToken == ")"

        if (!ExpectNextTokenIs(TokenType::LBRACE))
        {
            return nullptr;
        }
        AdvanceToken(); // ")" -> "{"
        functionExpression->mBody = ParseBlockStatement();

        return functionExpression;
    }

    std::vector<ExpressionUniquePtr> Parser::ParseFunctionParameters()
    {
        if (NextTokenIs(TokenType::RPAREN))
        {
            AdvanceToken();
            return {};
        }

        std::vector<ExpressionUniquePtr> parameters;
        AdvanceToken(); // "(" -> start of parameter

        parameters.push_back(std::move(ParsePrimitiveExpression()));

        while (GetNextToken() && NextTokenIs(TokenType::COMMA))
        {
            AdvanceToken(); // last character of previous identifier -> ","
            AdvanceToken(); // "," -> first character of next parameter
            parameters.push_back(std::move(ParsePrimitiveExpression()));
        }

        if (!ExpectNextTokenIs(TokenType::RPAREN))
        {
            return {};
        }
        AdvanceToken(); // Advance to ")"

        return parameters;
    }

    ExpressionUniquePtr Parser::ParseCallExpression(ExpressionUniquePtr leftExpression)
    {
        auto callExpression{ std::make_unique<ast::CallExpression>() };
        callExpression->mToken = *GetCurrentToken();
        callExpression->mFunction = std::move(leftExpression);
        callExpression->mArguments = ParseCallArguments();

        return callExpression;
    }

    std::vector<ExpressionUniquePtr> Parser::ParseCallArguments()
    {
        if (NextTokenIs(TokenType::RPAREN))
        {
            AdvanceToken(); // -> ')'
            return {};
        }

        std::vector<ExpressionUniquePtr> arguments;
        AdvanceToken(); // -> first argument

        arguments.push_back(std::move(ParseExpression(ast::Precedence::LOWEST)));

        while (NextTokenIs(TokenType::COMMA))
        {
            AdvanceToken(); // -> ','
            AdvanceToken(); // -> first part of next argument
            arguments.push_back(std::move(ParseExpression(ast::Precedence::LOWEST)));
        }

        if (!ExpectNextTokenIs(TokenType::RPAREN))
        {
            return {};
        }
        AdvanceToken(); // -> ')'

        return arguments;
    }

    //ObjectSharedPtr Parser::Evaluate(ast::Node* node)
    //{
    //    VERIFY(node);   // To catch issues.

    //    // Helper for PrimitiveExpressions
    //    const auto GetAndValidateTokenPrimtive = [](ast::Expression* node) -> TokenPrimitive {
    //        const auto optionalToken{ node->TokenNode() };
    //        assert(optionalToken);
    //        return optionalToken->mLiteral;
    //    };

    //    switch (node->mNodeType)
    //    {
    //    case ast::NodeType::ExpressionStatement:
    //        // TODOBB: these could be changed to static casts as we hold metadata, but keeping them dynamic for a bit to make sure everything works
    //        if (const auto expressionStatement{ dynamic_cast<ast::ExpressionStatement*>(node) })
    //        {
    //            return Evaluate(expressionStatement->mValue.get());
    //        }
    //        else
    //        {
    //            assert(false);
    //        }
    //        break;
    //    case ast::NodeType::Expression:
    //        if (const auto expression{ dynamic_cast<ast::Expression*>(node) })
    //        {
    //            if (expression->mExpressionType == ast::ExpressionType::IntegerExpression)
    //            {
    //                const auto primitive{ GetAndValidateTokenPrimtive(expression) };
    //                return std::make_shared<IntegerType>(std::get<Number>(primitive));
    //            }
    //            else if (expression->mExpressionType == ast::ExpressionType::BooleanExpression)
    //            {
    //                const auto primitive{ GetAndValidateTokenPrimtive(expression) };
    //                return GetNativeBoolObject(std::get<bool>(primitive));
    //            }
    //            else if (expression->mExpressionType == ast::ExpressionType::PrefixExpression)
    //            {
    //                const auto prefixExpression{ dynamic_cast<ast::PrefixExpression*>(expression) };
    //                const auto right{ Evaluate(prefixExpression->mRightSideValue.get()) };
    //                return EvaluatePrefixExpression(prefixExpression->mOperator.mType, right);
    //            }
    //            else if (expression->mExpressionType == ast::ExpressionType::InfixExpression)
    //            {
    //                const auto infixExpression{ dynamic_cast<ast::InfixExpression*>(expression) };
    //                VERIFY(infixExpression)
    //                {
    //                    const auto left{ Evaluate(infixExpression->mLeftExpression.get()) };
    //                    const auto right{ Evaluate(infixExpression->mRightExpression.get()) };
    //                    return EvaluateInfixExpression(infixExpression->mToken.mType, left, right);
    //                }
    //            }
    //        }
    //        break;
    //    default:
    //        break;
    //    }

    //    return nullptr;
    //}

    //ObjectSharedPtr Parser::EvaluatePrefixExpression(TokenType operatorToken, const ObjectSharedPtr& right)
    //{
    //    switch (operatorToken)
    //    {
    //    case TokenType::BANG:   // "!"
    //        return EvaluatePrefixBangOperatorExpression(right);
    //        break;
    //    case TokenType::MINUS:  // "-"
    //        return EvaluatePrefixMinusOperatorExpression(right);
    //        break;
    //    default:
    //        LOG(MessageType::ERRORS, "No Prefix Evaluator for: ", operatorToken);
    //        return GetNativeNullObject();
    //    }
    //    return GetNativeNullObject();
    //}

    //ObjectSharedPtr Parser::EvaluatePrefixBangOperatorExpression(const ObjectSharedPtr& right)
    //{
    //    // TODOBB: swap dynamic_cast to static_cast once it's confirmed to work.
    //    if (right->Type() == "bool")
    //    {
    //        const auto boolObject{ dynamic_cast<BoolType*>(right.get()) };
    //        if (boolObject->mValue)
    //        {
    //            return GetNativeBoolObject(false);
    //        }

    //        return GetNativeBoolObject(true);
    //    }
    //    else if (right->Type() == "int")
    //    {
    //        const auto intObject{ dynamic_cast<IntegerType*>(right.get()) };
    //        if (intObject->mValue == 0)
    //        {
    //            return GetNativeBoolObject(true);
    //        }

    //        return GetNativeBoolObject(false);
    //    }
    //    else if (right->Type() == "NULL")
    //    {
    //        return GetNativeBoolObject(true);
    //    }

    //    return GetNativeBoolObject(false);
    //}

    //ObjectSharedPtr Parser::EvaluatePrefixMinusOperatorExpression(const ObjectSharedPtr& right)
    //{
    //    const auto intObject{ dynamic_cast<IntegerType*>(right.get()) };
    //    VERIFY(intObject)
    //    {
    //        intObject->mValue = ~intObject->mValue;
    //        intObject->mValue += 1;
    //        return right;
    //    }

    //    return GetNativeNullObject();
    //}

    //ObjectSharedPtr Parser::EvaluateInfixExpression(TokenType operatorToken, const ObjectSharedPtr& left, const ObjectSharedPtr& right)
    //{
    //    VERIFY(left && right)
    //    {
    //        if (left->Type() == right->Type())
    //        {
    //            if (left->Type() == ObjectTypes::INTEGER_OBJECT)
    //            {
    //                return EvaluateInfixIntegerExpression(operatorToken, left, right);
    //            }
    //        }
    //    }

    //    return GetNativeNullObject();
    //}

    //ObjectSharedPtr Parser::EvaluateInfixIntegerExpression(TokenType operatorToken, const ObjectSharedPtr& left, const ObjectSharedPtr& right)
    //{
    //    const auto leftInt{ dynamic_cast<IntegerType*>(left.get()) };
    //    const auto rightInt{ dynamic_cast<IntegerType*>(right.get()) };
    //    VERIFY(leftInt && rightInt)
    //    {
    //        switch (operatorToken)
    //        {
    //        case TokenType::PLUS:   // '+'
    //            return std::make_shared<IntegerType>(leftInt->mValue + rightInt->mValue);
    //            break;
    //        case TokenType::MINUS:  // '-'
    //            return std::make_shared<IntegerType>(leftInt->mValue - rightInt->mValue);
    //            break;
    //        case TokenType::ASTERISK:  // '*'
    //            return std::make_shared<IntegerType>(leftInt->mValue * rightInt->mValue);
    //            break;
    //        case TokenType::SLASH:  // '/'
    //            return std::make_shared<IntegerType>(leftInt->mValue / rightInt->mValue);
    //            break;
    //        default:
    //            LOG(MessageType::ERRORS, "Operator : ", operatorToken ," not supported by Number types.");
    //        }
    //    }

    //    return GetNativeNullObject();
    //}

    //ObjectSharedPtr Parser::GetNativeBoolObject(bool value)
    //{
    //    static std::shared_ptr<BoolType> nativeTrue{ std::make_shared<BoolType>(true) };
    //    static std::shared_ptr<BoolType> nativeFalse{ std::make_shared<BoolType>(false) };

    //    if (value)
    //    {
    //        return nativeTrue;
    //    }

    //    return nativeFalse;
    //}

    //ObjectSharedPtr Parser::GetNativeNullObject()
    //{
    //    static std::shared_ptr<NullType> nativeNull{ std::make_shared<NullType>() };

    //    return nativeNull;
    //}

    ast::Precedence Parser::GetNextPrecedence()
    {
        auto token{ GetNextToken() };
        VERIFY(token)
        {
            return GetPrecedence(*token);
        }

        return ast::Precedence::LOWEST;
    }

    ast::Precedence Parser::GetCurrentPrecedence()
    {
        auto token{ GetCurrentToken() };
        VERIFY(token)
        {
            return GetPrecedence(*token);
        }

        return ast::Precedence::LOWEST;
    }

    ast::Precedence Parser::GetPrecedence(const Token& token)
    {
        if (const auto precedenceIterator{ ast::sOperatorPrecedenceMap.find(token.mType) }; precedenceIterator != ast::sOperatorPrecedenceMap.end())
        {
            return precedenceIterator->second;
        }

        return ast::Precedence::LOWEST;
    }

    bool Parser::NextTokenIs(TokenType tokenType)
    {
        if (const Token * peekToken{ GetNextToken() })
        {
            return TokenIs(*peekToken, tokenType);
        }

        return false;
    }

    bool Parser::CurrentTokenIs(TokenType tokenType)
    {
        if (const Token * currentToken{ GetCurrentToken() })
        {
            return TokenIs(*currentToken, tokenType);
        }

        return false;
    }

    bool Parser::TokenIs(const Token& token, TokenType tokenType)
    {
        return token.mType == tokenType;
    }

    bool Parser::ExpectNextTokenIs(TokenType expectedType)
    {
        if (const Token * peekToken{ GetNextToken() })
        {
            if (TokenIs(*peekToken, expectedType))
            {
                return true;
            }

            LOG(MessageType::ERRORS, "line -> ", peekToken->mLineNumber, "character range -> ", peekToken->mCharacterRange[0], ", ", peekToken->mCharacterRange[1], "expected next token to be -> ", expectedType, " actual type -> ",
                peekToken->mType);
        }

        return false;
    }
}