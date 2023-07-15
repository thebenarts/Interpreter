#include "Utility.h"
#include "Parser.h"
#include "AbstractSyntaxTree.h"
#define CATCH_CONFIG_MAIN
#include "catch.hpp"

#undef TRUE
#undef FALSE
namespace interpreter
{
    namespace test
    {
        struct StringAndBool {
            std::string identifier;
            bool boolean;
        };

        struct InfixExpressionData
        {
            TokenPrimitive left;
            TokenType opType;
            TokenPrimitive right;
        };

        bool TestIdentifier(ast::Expression* expression, std::string_view expectedValue)
        {
            REQUIRE(expression->mExpressionType == ast::ExpressionType::IdentifierExpression);
            const auto identifierToken{ expression->TokenNode() };
            REQUIRE(identifierToken);
            REQUIRE(std::holds_alternative<std::string>(identifierToken->mLiteral));
            const auto identifier{ std::get<std::string>(identifierToken->mLiteral) };
            REQUIRE(identifier == expectedValue);
            return true;
        }

        bool TestInteger(ast::Expression* expression, Number expectedValue)
        {
            REQUIRE(expression->mExpressionType == ast::ExpressionType::IntegerExpression);
            const auto integerToken{ expression->TokenNode() };
            REQUIRE(integerToken);
            REQUIRE(std::holds_alternative<Number>(integerToken->mLiteral));
            const auto number{ std::get<Number>(integerToken->mLiteral) };
            REQUIRE(number == expectedValue);
            return true;
        }

        bool TestBoolean(ast::Expression* expression, bool expectedValue)
        {
            REQUIRE(expression->mExpressionType == ast::ExpressionType::BooleanExpression);
            const auto boolToken{ expression->TokenNode() };
            REQUIRE(boolToken);
            const auto booleanValue{ std::get<bool>(boolToken->mLiteral) };
            REQUIRE(booleanValue == expectedValue);
            return true;
        }

        bool TestPrimitiveExpression(ast::Expression* expression, TokenPrimitive expectedValue)
        {
            std::visit([&expression](auto&& arg) {
                using ExpectedType = std::decay_t<decltype(arg)>;

                if constexpr (std::is_same_v<std::string, ExpectedType>)
                {
                    return TestIdentifier(expression, arg);
                }
                else if constexpr (std::is_same_v<Number, ExpectedType>)
                {
                    return TestInteger(expression, arg);
                }
                else if constexpr (std::is_same_v<bool, ExpectedType>)
                {
                    return TestBoolean(expression, arg);
                }
                else
                {
                    REQUIRE(false);
                    return false;
                }
                }, expectedValue);

            return true;
        }

        template<typename T>
        bool TestInfixExpression(ast::Expression* expression, T left, TokenType opType, T right)
        {
            REQUIRE(expression->mExpressionType == ast::ExpressionType::InfixExpression);
            const auto infixExpression{ dynamic_cast<ast::InfixExpression*>(expression) };
            REQUIRE(infixExpression);
            TestPrimitiveExpression(infixExpression->mLeftExpression.get(), left);
            REQUIRE(infixExpression->TokenNode()->mType == opType);
            REQUIRE(infixExpression->mRightExpression.get());
            TestPrimitiveExpression(infixExpression->mRightExpression.get(), right);
            return true;
        }
    }

    TEST_CASE("LEXER TEST")
    {
        // TestData 
        //`let five = 5;
        //let ten = 10;
        //let add = fn(x, y) {
        //x + y;
        //};
        //let result = add(five, ten);
        std::string lexerInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/lexerTestData.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<interpreter::Lexer>(lexerInput) };
        std::vector<interpreter::Token> expected
        {
            {TokenType::LET, "let"},
            {TokenType::IDENT, "five"},
            {TokenType::ASSIGN, "="},
            {TokenType::INT, 5},
            {TokenType::SEMICOLON, ";"},
            {TokenType::LET, "let"},
            {TokenType::IDENT, "ten"},
            {TokenType::ASSIGN, "="},
            {TokenType::INT, 10},
            {TokenType::SEMICOLON, ";"},
            {TokenType::LET, "let"},
            {TokenType::IDENT, "add"},
            {TokenType::ASSIGN, "="},
            {TokenType::FUNCTION, "fn"},
            {TokenType::LPAREN, "("},
            {TokenType::IDENT, "x"},
            {TokenType::COMMA, ","},
            {TokenType::IDENT, "y"},
            {TokenType::RPAREN, ")"},
            {TokenType::LBRACE, "{"},
            {TokenType::IDENT, "x"},
            {TokenType::PLUS, "+"},
            {TokenType::IDENT, "y"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::RBRACE, "}"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::LET, "let"},
            {TokenType::IDENT, "result"},
            {TokenType::ASSIGN, "="},
            {TokenType::IDENT, "add"},
            {TokenType::LPAREN, "("},
            {TokenType::IDENT, "five"},
            {TokenType::COMMA, ","},
            {TokenType::IDENT, "ten"},
            {TokenType::RPAREN, ")"},
            {TokenType::SEMICOLON, ";"},
            {TokenType::BANG, "!"},
            {TokenType::MINUS, "-"},
            {TokenType::SLASH, "/"},
            {TokenType::ASTERISK, "*"},
            {TokenType::INT, 5},
            {TokenType::SEMICOLON, ";"},
            {TokenType::INT, 5},
            {TokenType::LT, "<"},
            {TokenType::INT, 10},
            {TokenType::GT, ">"},
            {TokenType::INT, 5},
            {TokenType::SEMICOLON, ";"},
            {TokenType::IF, "if"},
            {TokenType::LPAREN, "("},
            {TokenType::INT, 5},
            {TokenType::LT, "<"},
            {TokenType::INT, 10},
            {TokenType::RPAREN, ")"},
            {TokenType::LBRACE, "{"},
            {TokenType::RETURN, "return"},
            {TokenType::TRUE, true},
            {TokenType::SEMICOLON, ";"},
            {TokenType::RBRACE, "}"},
            {TokenType::ELSE, "else"},
            {TokenType::LBRACE, "{"},
            {TokenType::RETURN, "return"},
            {TokenType::FALSE, false},
            {TokenType::SEMICOLON, ";"},
            {TokenType::RBRACE, "}"},
            {TokenType::INT, 10},
            {TokenType::EQ, "=="},
            {TokenType::INT, 10},
            {TokenType::SEMICOLON, ";"},
            {TokenType::INT, 10},
            {TokenType::NOT_EQ, "!="},
            {TokenType::INT, 9},
            {TokenType::SEMICOLON, ";"},
            {TokenType::ENDF, ""}
        };
        std::vector<Token> results{ lexer->GetTokenCopies() };

        REQUIRE(expected.size() == results.size());

        for (int i = 0; i < std::min(results.size(), expected.size()); i++)
        {
            REQUIRE(expected[i].mType == results[i].mType);
            if (expected[i].mType == TokenType::ENDF)
            {
                continue;
            }

            if (std::holds_alternative<std::string>(results[i].mLiteral))
            {
                REQUIRE(std::get<std::string>(expected[i].mLiteral) == std::get<std::string>(results[i].mLiteral));
            }
            else if (std::holds_alternative<Number>(results[i].mLiteral))
            {
                REQUIRE(std::get<Number>(expected[i].mLiteral) == std::get<Number>(results[i].mLiteral));
            }
            else
            {
                REQUIRE(std::get<bool>(expected[i].mLiteral) == std::get<bool>(results[i].mLiteral));
            }
        }
    }

    TEST_CASE("PARSER Statement tests")
    {
        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/parserTestData.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        REQUIRE(program != nullptr);

        std::vector<std::string> expectedIdentifiers{ "x","y","foobar" };
        REQUIRE(program->mStatements.size() == 3);

        for (int i = 0; i != 3; i++)
        {
            ast::LetStatement* letStatement{ dynamic_cast<ast::LetStatement*>(program->mStatements[i].get()) };
            REQUIRE(letStatement != nullptr);

            REQUIRE(letStatement->mToken.mType == TokenType::LET);
            if (letStatement->mIdentifier)
            {
                if (const auto token{ letStatement->mIdentifier->TokenNode() })
                {
                    REQUIRE(std::holds_alternative<std::string>(token->mLiteral));
                    if (std::holds_alternative<std::string>(token->mLiteral))
                    {
                        REQUIRE(std::get<std::string>(token->mLiteral) == expectedIdentifiers[i]);
                    }
                }
            }
        }
    }

    TEST_CASE("IdentExpressionTests")
    {
        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/identifierExpressionStatementsTest.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        REQUIRE(program != nullptr);
        REQUIRE(program->mStatements.size() == 1);
        REQUIRE(program->mStatements[0]->mNodeType == ast::NodeType::ExpressionStatement);

        ast::ExpressionStatement* expressionStatement{ dynamic_cast<ast::ExpressionStatement*>(program->mStatements[0].get()) };
        REQUIRE(expressionStatement);
        REQUIRE(expressionStatement->mValue);

        auto token{ program->mStatements[0]->TokenNode() };
        REQUIRE(token);
        REQUIRE(token->mType == TokenType::IDENT);
        REQUIRE(std::holds_alternative<std::string>(token->mLiteral) == true);
        REQUIRE(std::get<std::string>(token->mLiteral) == "foobar");

        auto expressionToken{ expressionStatement->mValue->TokenNode() };
        REQUIRE(expressionToken);
        REQUIRE(expressionToken->mType == TokenType::IDENT);
        REQUIRE(std::holds_alternative<std::string>(expressionToken->mLiteral) == true);
        REQUIRE(std::get<std::string>(expressionToken->mLiteral) == "foobar");
    }

    TEST_CASE("IntegerExpressionTests")
    {
        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/integerExpressionStatementsTest.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        REQUIRE(program != nullptr);
        REQUIRE(program->mStatements.size() == 1);
        REQUIRE(program->mStatements[0]->mNodeType == ast::NodeType::ExpressionStatement);

        ast::ExpressionStatement* expressionStatement{ dynamic_cast<ast::ExpressionStatement*>(program->mStatements[0].get()) };
        REQUIRE(expressionStatement);
        REQUIRE(expressionStatement->mValue);

        auto token{ program->mStatements[0]->TokenNode() };
        REQUIRE(token);
        REQUIRE(token->mType == TokenType::INT);
        REQUIRE(std::holds_alternative<Number>(token->mLiteral) == true);
        REQUIRE(std::get<Number>(token->mLiteral) == 5);

        auto expressionToken{ expressionStatement->mValue->TokenNode() };
        REQUIRE(expressionToken);
        REQUIRE(expressionToken->mType == TokenType::INT);
        REQUIRE(std::holds_alternative<Number>(expressionToken->mLiteral) == true);
        REQUIRE(std::get<Number>(expressionToken->mLiteral) == 5);
    }

    TEST_CASE("PrefixOperatorExpressionTests")
    {
        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/prefixOperatorExpressionStatementsTest.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        std::vector<std::vector<Token>> prefixTests
        {
            {{TokenType::BANG,"!"},{TokenType::INT, 5}},
            {{TokenType::MINUS, "-"},{TokenType::INT, 15}}
        };

        REQUIRE(program != nullptr);
        REQUIRE(program->mStatements.size() == 2);
        for (int i = 0; i != 2; i++)
        {
            REQUIRE(program->mStatements[i]->mNodeType == ast::NodeType::ExpressionStatement);

            ast::ExpressionStatement* expressionStatement{ dynamic_cast<ast::ExpressionStatement*>(program->mStatements[i].get()) };
            REQUIRE(expressionStatement);
            REQUIRE(expressionStatement->mValue);

            REQUIRE(expressionStatement->mValue->mExpressionType == ast::ExpressionType::PrefixExpression);
            const auto prefixExpression{ dynamic_cast<ast::PrefixExpression*>(expressionStatement->mValue.get()) };
            REQUIRE(prefixExpression);

            REQUIRE(prefixExpression->mToken.mType == prefixTests[i][0].mType);
            REQUIRE(prefixExpression->mRightSideValue->TokenNode()->mType == prefixTests[i][1].mType);
            REQUIRE(utility::CompareTokens(*prefixExpression->mRightSideValue->TokenNode(), prefixTests[i][1]));
        }
    }

    TEST_CASE("InfixOperatorExpressionTests")
    {
        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/operatorPrecedenceTest.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        std::string operatorPrecedenceTests
        {
            "((-a) * b)\n"
            "(!(-a))\n"
            "((a + b) + c)\n"
            "((a + b) - c)\n"
            "((a * b) * c)\n"
            "((a * b) / c)\n"
            "(a + (b / c))\n"
            "(((a + (b * c)) + (d / e)) - f)\n"
            "(3 + 4)\n((-5) * 5)\n"
            "((5 > 4) == (3 < 4))\n"
            "((5 < 4) != (3 > 4))\n"
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))\n"
            "((3 + (4 * 5)) == ((3 * 1) + (4 * 5)))\n"
        };

        REQUIRE(program != nullptr);
        REQUIRE(program->Log() == operatorPrecedenceTests);
    }

    TEST_CASE("BooleanExpressionTests")
    {
        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/booleanExpressionTest.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };


        std::vector<bool> booleans{ true,false };
        std::vector<test::StringAndBool> letTest{ {"foobar", true}, { "boofar", false } };

        for (int i = 0; i != 2; i++)
        {
            const auto statement{ program->mStatements[i].get() };
            REQUIRE(statement->mNodeType == ast::NodeType::ExpressionStatement);
            const auto expressionStatement{ dynamic_cast<ast::ExpressionStatement*>(statement) };
            REQUIRE(expressionStatement->mValue);
            REQUIRE(expressionStatement->mValue->mExpressionType == ast::ExpressionType::BooleanExpression);
            test::TestPrimitiveExpression(expressionStatement->mValue.get(), booleans[i]);
        }

        for (int i = 2; i != 4; i++)
        {
            const auto statement{ program->mStatements[i].get() };
            REQUIRE(statement->mNodeType == ast::NodeType::LetStatement);
            const auto letStatement{ dynamic_cast<ast::LetStatement*>(statement) };
            test::TestPrimitiveExpression(letStatement->mIdentifier.get(), letTest[i - 2].identifier);
            test::TestPrimitiveExpression(letStatement->mValue.get(), letTest[i - 2].boolean);
        }
        REQUIRE(program != nullptr);
    }

    TEST_CASE("InfixExpressionTest2")
    {
        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/infixExpressionTest2.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        std::vector<test::InfixExpressionData> expected{ {true,TokenType::EQ,true},{5,TokenType::NOT_EQ,3},{"you", TokenType::NOT_EQ, "me"} };

        for (int i = 0; i != 3; i++)
        {
            ast::Statement* statement{ program->mStatements[i].get() };
            REQUIRE(statement->mNodeType == ast::NodeType::ExpressionStatement);
            const auto expressionStatement{ dynamic_cast<ast::ExpressionStatement*>(statement) };

            ast::Expression* expression{ expressionStatement->mValue.get() };
            REQUIRE(expression);
            const auto infixExpression{ dynamic_cast<ast::InfixExpression*>(expression) };
            REQUIRE(infixExpression);

            test::TestInfixExpression(expression, expected[i].left, expected[i].opType, expected[i].right);
        }
    }

    TEST_CASE("GroupedExpressionsTest")
    {
        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/groupedExpressionTest.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        std::string groupedExpressionTest{
            "((1 + (2 + 3)) + 4)\n"
            "((5 + 5) * 2)\n"
            "(2 / (5 + 5))\n"
            "(-(5 + 5))\n"
            "(!(true == true))\n"
        };

        REQUIRE(program != nullptr);
        REQUIRE(program->Log() == groupedExpressionTest);
    }

    TEST_CASE("IfExpressionTest")
    {
        //  if (x < y)
        //  {
        //      x
        //  }

        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/ifExpressionTest.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        REQUIRE(program->mStatements.size() == 1);
        ast::Statement* statement{ program->mStatements[0].get() };
        REQUIRE(statement->mNodeType == ast::NodeType::ExpressionStatement);
        const auto expressionStatement{ dynamic_cast<ast::ExpressionStatement*>(statement) };
        REQUIRE(expressionStatement);

        REQUIRE(expressionStatement->mValue);
        ast::Expression* expression{ expressionStatement->mValue.get() };
        REQUIRE(expression->mExpressionType == ast::ExpressionType::IfExpression);
        const auto ifExpression{ dynamic_cast<ast::IfExpression*>(expression) };
        REQUIRE(ifExpression);

        ast::Statement* ifStatement{ ifExpression->mIfConditionBlock.get() };
        REQUIRE(ifStatement);
        REQUIRE(ifStatement->mNodeType == ast::NodeType::ConditionBlockStatement);
        const auto ifConditionBlockStatement{ dynamic_cast<ast::ConditionBlockStatement*>(ifStatement) };
        REQUIRE(ifConditionBlockStatement);

        ast::Expression* ifConditionExpression{ ifConditionBlockStatement->mCondition.get() };
        REQUIRE(ifConditionExpression);
        test::TestInfixExpression(ifConditionExpression, "x", TokenType::LT, "y");

        ast::Statement* ifBStatement{ ifConditionBlockStatement->mBlock.get() };
        REQUIRE(ifBStatement);
        REQUIRE(ifBStatement->mNodeType == ast::NodeType::BlockStatement);
        const auto ifBlockStatement{ dynamic_cast<ast::BlockStatement*>(ifBStatement) };
        REQUIRE(ifBlockStatement);
        REQUIRE(ifBlockStatement->mStatements.size() == 1);
        ast::Statement* consequenceStatement{ ifBlockStatement->mStatements[0].get()};
        REQUIRE(consequenceStatement->mNodeType == ast::NodeType::ExpressionStatement);
        const auto consequenceExpressionStatement{ dynamic_cast<ast::ExpressionStatement*>(consequenceStatement) };
        REQUIRE(consequenceExpressionStatement);
        REQUIRE(consequenceExpressionStatement->mValue);
        test::TestPrimitiveExpression(consequenceExpressionStatement->mValue.get(), "x");
        REQUIRE(!ifExpression->mAlternative);
    }

    TEST_CASE("IfElseExpressionTest")
    {
        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/ifElseExpressionTest.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        REQUIRE(program->mStatements.size() == 1);
        ast::Statement* statement{ program->mStatements[0].get() };
        REQUIRE(statement->mNodeType == ast::NodeType::ExpressionStatement);
        const auto expressionStatement{ dynamic_cast<ast::ExpressionStatement*>(statement) };
        REQUIRE(expressionStatement);

        REQUIRE(expressionStatement->mValue);
        ast::Expression* expression{ expressionStatement->mValue.get() };
        REQUIRE(expression->mExpressionType == ast::ExpressionType::IfExpression);
        const auto ifExpression{ dynamic_cast<ast::IfExpression*>(expression) };
        REQUIRE(ifExpression);

        ast::Statement* ifStatement{ ifExpression->mIfConditionBlock.get() };
        REQUIRE(ifStatement);
        REQUIRE(ifStatement->mNodeType == ast::NodeType::ConditionBlockStatement);
        const auto ifConditionBlockStatement{ dynamic_cast<ast::ConditionBlockStatement*>(ifStatement) };
        REQUIRE(ifConditionBlockStatement);

        ast::Expression* ifConditionExpression{ ifConditionBlockStatement->mCondition.get() };
        REQUIRE(ifConditionExpression);
        test::TestInfixExpression(ifConditionExpression, "x", TokenType::LT, "y");

        ast::Statement* ifBStatement{ ifConditionBlockStatement->mBlock.get() };
        REQUIRE(ifBStatement);
        REQUIRE(ifBStatement->mNodeType == ast::NodeType::BlockStatement);
        const auto ifBlockStatement{ dynamic_cast<ast::BlockStatement*>(ifBStatement) };
        REQUIRE(ifBlockStatement);
        REQUIRE(ifBlockStatement->mStatements.size() == 1);
        ast::Statement* consequenceStatement{ ifBlockStatement->mStatements[0].get()};
        REQUIRE(consequenceStatement->mNodeType == ast::NodeType::ExpressionStatement);
        const auto consequenceExpressionStatement{ dynamic_cast<ast::ExpressionStatement*>(consequenceStatement) };
        REQUIRE(consequenceExpressionStatement);
        REQUIRE(consequenceExpressionStatement->mValue);
        test::TestPrimitiveExpression(consequenceExpressionStatement->mValue.get(), "x");

        REQUIRE(ifExpression->mAlternative);
        ast::Statement* alternativeStatement{ ifExpression->mAlternative->mStatements[0].get()};
        REQUIRE(alternativeStatement);
        const auto alternativeExpressionStatement{ dynamic_cast<ast::ExpressionStatement*>(alternativeStatement) };
        REQUIRE(alternativeExpressionStatement);
        REQUIRE(alternativeExpressionStatement->mValue);
        test::TestPrimitiveExpression(alternativeExpressionStatement->mValue.get(), "y");
    }

    TEST_CASE("ElseIfTest")
    {
        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/elseIfTest.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        REQUIRE(program->mStatements.size() == 1);
        ast::Statement* statement{ program->mStatements[0].get() };
        REQUIRE(statement->mNodeType == ast::NodeType::ExpressionStatement);
        const auto expressionStatement{ dynamic_cast<ast::ExpressionStatement*>(statement) };
        REQUIRE(expressionStatement);

        REQUIRE(expressionStatement->mValue);
        ast::Expression* expression{ expressionStatement->mValue.get() };
        REQUIRE(expression->mExpressionType == ast::ExpressionType::IfExpression);
        const auto ifExpression{ dynamic_cast<ast::IfExpression*>(expression) };
        REQUIRE(ifExpression);

        ast::Statement* ifStatement{ ifExpression->mIfConditionBlock.get() };
        REQUIRE(ifStatement);
        REQUIRE(ifStatement->mNodeType == ast::NodeType::ConditionBlockStatement);
        const auto ifConditionBlockStatement{ dynamic_cast<ast::ConditionBlockStatement*>(ifStatement) };
        REQUIRE(ifConditionBlockStatement);

        ast::Expression* ifConditionExpression{ ifConditionBlockStatement->mCondition.get() };
        REQUIRE(ifConditionExpression);
        test::TestInfixExpression(ifConditionExpression, "x", TokenType::LT, "y");

        ast::Statement* ifBStatement{ ifConditionBlockStatement->mBlock.get() };
        REQUIRE(ifBStatement);
        REQUIRE(ifBStatement->mNodeType == ast::NodeType::BlockStatement);
        const auto ifBlockStatement{ dynamic_cast<ast::BlockStatement*>(ifBStatement) };
        REQUIRE(ifBlockStatement);
        REQUIRE(ifBlockStatement->mStatements.size() == 1);
        ast::Statement* consequenceStatement{ ifBlockStatement->mStatements[0].get()};
        REQUIRE(consequenceStatement->mNodeType == ast::NodeType::ExpressionStatement);
        const auto consequenceExpressionStatement{ dynamic_cast<ast::ExpressionStatement*>(consequenceStatement) };
        REQUIRE(consequenceExpressionStatement);
        REQUIRE(consequenceExpressionStatement->mValue);
        test::TestPrimitiveExpression(consequenceExpressionStatement->mValue.get(), "work");

        std::vector<test::InfixExpressionData> testData{ {5, TokenType::LT, 4}, {4, TokenType::NOT_EQ, 5} };
        REQUIRE(!ifExpression->mElseIfBlocks.empty());
        for (int i = 0; i != 2; i++)
        {
            ast::ConditionBlockStatement* elseConditionBlock{ ifExpression->mElseIfBlocks[i].get() };
            REQUIRE(elseConditionBlock);
            ast::Expression* elseCondition{ elseConditionBlock->mCondition.get() };
            REQUIRE(elseCondition);
            test::TestInfixExpression(elseCondition, testData[i].left, testData[i].opType, testData[i].right);
            REQUIRE(elseConditionBlock->mBlock);
            ast::Statement* elseStatement{ elseConditionBlock->mBlock->mStatements[0].get() };
            REQUIRE(elseStatement);
            const auto elseExpressionStatement{ dynamic_cast<ast::ExpressionStatement*>(elseStatement) };
            REQUIRE(elseExpressionStatement);
            test::TestPrimitiveExpression(elseExpressionStatement->mValue.get(), "work");
        }

        REQUIRE(ifExpression->mAlternative);
        ast::Statement* alternativeStatement{ ifExpression->mAlternative->mStatements[0].get()};
        REQUIRE(alternativeStatement);
        const auto alternativeExpressionStatement{ dynamic_cast<ast::ExpressionStatement*>(alternativeStatement) };
        REQUIRE(alternativeExpressionStatement);
        REQUIRE(alternativeExpressionStatement->mValue);
        test::TestPrimitiveExpression(alternativeExpressionStatement->mValue.get(), "work");
        std::cout << program->Log();
    }

    TEST_CASE("FunctionExpressionTest")
    {
        // fn(x,y) { x + y; }

        std::string parserInput{ interpreter::utility::ReadTextFile("E:/dev/Interpreter/tests/input/functionExpressionTest.txt") };
        interpreter::LexerUniquePtr lexer{ std::make_unique<Lexer>(parserInput) };
        interpreter::Parser parser{ std::move(lexer) };
        interpreter::ProgramUniquePtr program{ parser.ParseProgram() };

        REQUIRE(program);
        REQUIRE(program->mStatements.size() == 1);
        ast::ExpressionStatement* expressionStatement{ dynamic_cast<ast::ExpressionStatement*>(program->mStatements[0].get())};
        REQUIRE(expressionStatement);

        ast::FunctionExpression* functionExpression{ dynamic_cast<ast::FunctionExpression*>(expressionStatement->mValue.get()) };
        REQUIRE(functionExpression);
        REQUIRE(functionExpression->mParameters.size() == 2);
        test::TestPrimitiveExpression(functionExpression->mParameters[0].get(), "x");
        test::TestPrimitiveExpression(functionExpression->mParameters[1].get(), "y");

        REQUIRE(functionExpression->mBody);
        REQUIRE(functionExpression->mBody->mStatements.size() == 1);
        ast::ExpressionStatement* bodyStatement{ dynamic_cast<ast::ExpressionStatement*>(functionExpression->mBody->mStatements[0].get()) };
        ast::Expression* infixExpression{ bodyStatement->mValue.get() };
        test::TestInfixExpression(infixExpression, "x", TokenType::PLUS, "y");
    }
}
