#include "CustomExceptions.h"
#include "IRGenVisitor.h"
#include "Scope.h"
#include "ifccParser.h"
#include "ir.h"
#include "support/Any.h"
#include <memory>
#include <string>

antlrcpp::Any IRGenVisitor::visitAxiom(ifccParser::AxiomContext *ctx) {
    try {
        visitChildren(ctx);
    } catch (const ReturnException &e) {
        std::cerr << "'return' ne peut pas se trouver en dehors du programme\n";
        exit(1);
    } catch (const ContinueException &e) {
        std::cerr
            << "'continue' ne peut pas se trouver en dehors d'une boucle\n";
        exit(1);
    } catch (const BreakException &e) {
        std::cerr << "'break' ne peut pas se trouver en dehors d'une boucle ou "
                     "d'un switch\n";
        exit(1);
    }

    return 0;
}

antlrcpp::Any IRGenVisitor::visitProg(ifccParser::ProgContext *ctx) {

    for (const auto &func : ctx->function()) {
        visit(func);
    }
    return 0;
}

antlrcpp::Any IRGenVisitor::visitScope(ifccParser::ScopeContext *ctx) {
    // Create a new block, new scope, and set up an unconditional jump.
    Scope &currentScope = this->currentBlock->getScope();
    Scope *newScope = new Scope(&currentScope);
    currentFunction->addScope(std::unique_ptr<Scope>(newScope));
    std::string name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    auto scopeBlock = new ir::BasicBlock(newScope, name);
    currentFunction->addBlock(std::unique_ptr<ir::BasicBlock>(scopeBlock));

    // Create a new block with the previous scope and set up an unconditional
    // jump.
    name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    auto afterScopeBlock = new ir::BasicBlock(&currentScope, name);
    currentFunction->addBlock(std::unique_ptr<ir::BasicBlock>(afterScopeBlock));

    auto end = currentBlock->getNext();

    currentBlock->setNext(std::make_unique<ir::UnconditionalJump>(scopeBlock));
    scopeBlock->setNext(
        std::make_unique<ir::UnconditionalJump>(afterScopeBlock));
    afterScopeBlock->setNext(std::move(end));

    currentBlock = scopeBlock;

    visitChildren(ctx);

    currentBlock = afterScopeBlock;

    return 0;
}

// Visit the condition expression of the if statement.
antlrcpp::Any IRGenVisitor::visitIf_stmt(ifccParser::If_stmtContext *ctx) {
    // Extract the condition expression for the if statement.
    std::string condition = this->visit(ctx->expression());

    // Generate a name for the basic block representing the 'then' branch
    ir::BasicBlock *thenBlock = createBlock(".BB");
    ir::BasicBlock *elseBlock = createBlock(".BB");

    ir::BasicBlock *end = elseBlock;
    std::unique_ptr<ir::Next> endNext = std::move(currentBlock->getNext());

    // Check if there is an 'else' statement.
    if (ctx->else_stmt) {
        // If so, create a new block for the end of the if statement
        end = createBlock(".BB");
        elseBlock->setNext(std::make_unique<ir::UnconditionalJump>(end));
    }

    // Create a conditional jump instruction based on the condition.
    end->setNext(std::move(endNext));

    auto conditional = std::make_unique<ir::ConditionalJump>(
        std::move(condition), thenBlock, elseBlock);

    // Set the next block for the current block to be the conditional jump.
    this->currentBlock->setNext(std::move(conditional));

    this->currentBlock = std::move(thenBlock);
    this->currentBlock->setNext(std::make_unique<ir::UnconditionalJump>(end));

    try {
        this->visit(ctx->then_stmt);
    } catch (const ReturnException &) {
    }

    // Move to the 'else' block to generate code for the 'else' branch (if any).
    this->currentBlock = std::move(elseBlock);

    // If there is an 'else' statement, generate code for it.
    if (ctx->else_stmt) {
        this->currentBlock->setNext(
            std::make_unique<ir::UnconditionalJump>(end));

        try {
            this->visit(ctx->else_stmt);
        } catch (const ReturnException &) {
        }

        this->currentBlock = end;
    }

    // Return 0, indicating successful visitation.
    return 0;
}

// Visits a while loop statement.
antlrcpp::Any IRGenVisitor::visitWhile_stmt(
    ifccParser::While_stmtContext *ctx) {

    // Create a new basic block for the loop condition.
    ir::BasicBlock *conditionBlock = createBlock(".BB");

    // Create new basic blocks for the 'then' and 'else' branches of the loop.
    ir::BasicBlock *thenBlock = createBlock(".BB");
    thenBlock->setNext(std::make_unique<ir::UnconditionalJump>(conditionBlock));

    ir::BasicBlock *elseBlock = createBlock(".BB");
    elseBlock->setNext(this->currentBlock->getNext());

    this->currentBlock->setNext(
        std::make_unique<ir::UnconditionalJump>(conditionBlock));

    this->currentBlock = conditionBlock;
    auto condition = this->visit(ctx->expression());

    // Set the conditional jumlp on condition block
    conditionBlock->setNext(std::make_unique<ir::ConditionalJump>(
        std::move(condition), thenBlock, elseBlock));

    this->currentBlock = thenBlock;

    // Generate code for the loop body.
    try {
        this->visit(ctx->then_stmt);
    } catch (const ReturnException &e) {
    } catch (const BreakException &e) {
        this->currentBlock->setNext(
            std::make_unique<ir::UnconditionalJump>(elseBlock));
    } catch (const ContinueException &e) {
        this->currentBlock->setNext(
            std::make_unique<ir::UnconditionalJump>(conditionBlock));
    }

    // Move to the 'else' block.
    this->currentBlock = elseBlock;

    // Return 0, indicating successful visitation.
    return 0;
}

// Visits a do-while loop statement.
antlrcpp::Any IRGenVisitor::visitDo_while_stmt(
    ifccParser::Do_while_stmtContext *ctx) {

    // Create new basic blocks for the 'then' and 'else' branches of the loop.
    ir::BasicBlock *thenBlock = createBlock(".BB");
    ir::BasicBlock *elseBlock = createBlock(".BB");
    elseBlock->setNext(this->currentBlock->getNext());

    ir::BasicBlock *conditionBlock = createBlock(".BB");
    thenBlock->setNext(std::make_unique<ir::UnconditionalJump>(elseBlock));

    // Set up an unconditional jump from the current block to the 'then' block.
    this->currentBlock->setNext(
        std::make_unique<ir::UnconditionalJump>(thenBlock));

    this->currentBlock = conditionBlock;
    std::string condition = this->visit(ctx->expression());
    conditionBlock->setNext(std::make_unique<ir::ConditionalJump>(
        std::move(condition), thenBlock, elseBlock));

    // Move to the 'then' block.
    this->currentBlock = thenBlock;

    try {
        // Generate code for the loop body.
        this->visit(ctx->then_stmt);
        // Set the next block for the condition block to be the conditional
        // jump.
        this->currentBlock->setNext(
            std::make_unique<ir::UnconditionalJump>(conditionBlock));
    } catch (const ReturnException &e) {
    } catch (const BreakException &e) {
        this->currentBlock->setNext(
            std::make_unique<ir::UnconditionalJump>(elseBlock));
    } catch (const ContinueException &e) {
        this->currentBlock->setNext(
            std::make_unique<ir::UnconditionalJump>(conditionBlock));
    }

    // Move to the 'else' block.
    this->currentBlock = elseBlock;

    // Return 0, indicating successful visitation.
    return 0;
}

antlrcpp::Any IRGenVisitor::visitSwitch_stmt(
    ifccParser::Switch_stmtContext *ctx) {
    // Retrieve the current scope.
    Scope &currentScope = this->currentBlock->getScope();

    auto defaultBlock = this->createBlock(".BB");

    ir::BasicBlock *end = defaultBlock;
    std::unique_ptr<ir::Next> endNext = std::move(currentBlock->getNext());

    if (ctx->default_case_stmt()) {
        end = this->createBlock(".BB");
        defaultBlock->setNext(std::make_unique<ir::UnconditionalJump>(end));
    }

    end->setNext(std::move(endNext));

    currentBlock->setNext(
        std::make_unique<ir::UnconditionalJump>(defaultBlock));

    std::vector<ir::BasicBlock *> caseBlocks;
    std::vector<std::pair<std::string, ir::BasicBlock *>> caseTests;

    for (size_t i = 0; i < ctx->case_stmt().size(); ++i) {
        auto caseBlock = this->createBlock(".BB");
        caseBlock->setNext(
            std::make_unique<ir::UnconditionalJump>(defaultBlock));

        if (!caseBlocks.empty()) {
            caseBlocks.back()->setNext(
                std::make_unique<ir::UnconditionalJump>(caseBlock));
        }

        std::string test_case = ctx->case_stmt(i)->test_expr->getText();
        caseTests.push_back(std::make_pair(test_case, caseBlock));

        caseBlocks.push_back(caseBlock);
    }

    std::string expressionTest = this->visit(ctx->expression());

    if (!caseTests.empty()) {
        currentBlock->setNext(
            std::make_unique<ir::SwitchJump>(expressionTest, caseTests));
    }

    for (size_t i = 0; i < ctx->case_stmt().size(); ++i) {
        auto caseBlock = caseBlocks[i];
        currentBlock = caseBlock;

        try {
            visit(ctx->case_stmt(i));
        } catch (const ReturnException &e) {
        } catch (const BreakException &e) {
            caseBlock->setNext(std::make_unique<ir::UnconditionalJump>(end));
        }
    }

    currentBlock = defaultBlock;

    if (ctx->default_case_stmt()) {
        try {
            visit(ctx->default_case_stmt());
        } catch (const ReturnException &e) {
        } catch (const BreakException &e) {
        }
        currentBlock = end;
    }

    return 0;
}

antlrcpp::Any IRGenVisitor::visitContinue_stmt(
    ifccParser::Continue_stmtContext *ctx) {
    throw ContinueException();
    return 0;
}

antlrcpp::Any IRGenVisitor::visitBreak_stmt(
    ifccParser::Break_stmtContext *ctx) {
    throw BreakException();
    return 0;
}

antlrcpp::Any IRGenVisitor::visitFunc_call(ifccParser::Func_callContext *ctx) {
    std::string tempVarName = "#" + std::to_string(this->counterTempVariables);
    auto instruction =
        std::make_unique<ir::Call>(ctx->VARIABLE()->getText(), tempVarName);
    ++this->counterTempVariables;
    for (const auto &expr : ctx->expression()) {
        std::string name = this->visit(expr);

        instruction->addName(name);
    }
    this->currentBlock->addInstr(std::move(instruction));
    this->currentBlock->getScope().addVariable(tempVarName, INT);

    return tempVarName;
}

antlrcpp::Any IRGenVisitor::visitFunction(ifccParser::FunctionContext *ctx) {
    std::string funcName = ctx->VARIABLE(0)->getText();

    // Create the function in the intermediate representation (IR).
    this->currentFunction = std::make_unique<ir::CFG>(funcName);

    // Create a scope for the function.
    Scope *scope = new Scope(nullptr);
    this->currentFunction->addScope(std::unique_ptr<Scope>(scope));

    // Create the initial basic block for the function.
    std::string name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;

    // Create current block and add it to the function.
    this->currentBlock = new ir::BasicBlock(scope, std::move(name));
    this->currentFunction->addBlock(
        std::unique_ptr<ir::BasicBlock>(this->currentBlock));

    // Create the epilogue block for the function.
    ir::BasicBlock *epilogue =
        new ir::BasicBlock(scope, "." + funcName + "_ret");
    epilogue->setNext(std::make_unique<ir::Return>());
    this->currentFunction->setEpilogue(epilogue);
    this->currentBlock->setNext(
        std::make_unique<ir::UnconditionalJump>(epilogue));

    // Keep track of arguments and add them to the scope
    for (size_t i = 1; i < ctx->VARIABLE().size(); ++i) {
        this->currentFunction->addArg(ctx->VARIABLE(i)->getText(), INT);
        this->currentBlock->getScope().addVariable(ctx->VARIABLE(i)->getText(),
                                                   INT);
    }

    // Add a variable "#return" to the current block's scope.
    this->currentBlock->getScope().addVariable("#return", INT);

    try {
        // Visit children nodes to generate IR code.
        this->visitChildren(ctx);
    } catch (const ReturnException &) {
    }

    // Add epilogue block to the function.
    this->currentFunction->addBlock(std::unique_ptr<ir::BasicBlock>(epilogue));
    // Push the function to the list of functions in the IR.
    this->ir.push_back(std::move(this->currentFunction));

    return funcName;
}

antlrcpp::Any IRGenVisitor::visitReturn_stmt(
    ifccParser::Return_stmtContext *ctx) {
    // Visit the expression to be returned.
    std::string toReturn = this->visit(ctx->expression());
    // Create an instruction to affect the return value.
    auto instruction = std::make_unique<ir::Affect>("#return", toReturn);
    this->currentBlock->addInstr(std::move(instruction));
    // Set up an unconditional jump to the function's epilogue.
    this->currentBlock->setNext(std::make_unique<ir::UnconditionalJump>(
        &this->currentFunction->getEpilogue()));

    throw ReturnException();

    return 0;
}

antlrcpp::Any IRGenVisitor::visitDeclaAffect(
    ifccParser::DeclaAffectContext *ctx) {
    // Add a variable declaration to the current block's scope.
    this->currentBlock->getScope().addVariable(ctx->VARIABLE()->getText(), INT);
    if (ctx->expression()) {
        // If there's an expression, create an instruction to affect the
        // variable.
        std::string to = ctx->VARIABLE()->getText();
        std::string from = (std::string)this->visit(ctx->expression());
        auto instruction = std::make_unique<ir::Affect>(to, from);
        this->currentBlock->addInstr(std::move(instruction));
    }
    return 0;
}

antlrcpp::Any IRGenVisitor::visitIntConst(ifccParser::IntConstContext *ctx) {
    // Extract the integer value from the context.
    int value = stoi(ctx->INT_CONST()->getText());

    // Generate a temporary variable name.
    ++this->counterTempVariables;
    std::string variableName = "#" + std::to_string(this->counterTempVariables);
    // Add the temporary variable to the current block's scope.
    this->currentBlock->getScope().addVariable(variableName, INT);
    // Create an instruction to affect the temporary variable with the integer
    // value.
    auto instruction = std::make_unique<ir::AffectConst>(variableName, value);
    this->currentBlock->addInstr(std::move(instruction));
    return variableName;
}

antlrcpp::Any IRGenVisitor::visitCharConst(ifccParser::CharConstContext *ctx) {
    // Extract the character value from the context.
    int value = ctx->CHAR_CONST()->getText()[1];

    // Generate a temporary variable name.
    ++this->counterTempVariables;
    std::string variableName = "#" + std::to_string(this->counterTempVariables);
    // Add the temporary variable to the current block's scope.
    this->currentBlock->getScope().addVariable(variableName, INT);
    // Create an instruction to affect the temporary variable with the character
    // value.
    auto instruction = std::make_unique<ir::AffectConst>(variableName, value);
    this->currentBlock->addInstr(std::move(instruction));
    return variableName;
}

antlrcpp::Any IRGenVisitor::visitVariable(ifccParser::VariableContext *ctx) {
    // Return the name of the variable.
    return ctx->VARIABLE()->getText();
}

antlrcpp::Any IRGenVisitor::visitAffect(
    ifccParser::AffectContext *ctx) {                               // a op b
    std::string to = ctx->VARIABLE()->getText();                    // a
    std::string from = (std::string)this->visit(ctx->expression()); // b

    std::unique_ptr<ir::IRInstr> instructionAffect;
    if (ctx->op->getText() == "=") { // a = b
        instructionAffect = std::make_unique<ir::Affect>(to, from);
    } else {
        // creer le temp
        ++counterTempVariables;
        std::string tmpTo = "#" + std::to_string(counterTempVariables);
        this->currentBlock->getScope().addVariable(tmpTo, INT);

        // declare l'instruction operation
        ir::BinOp::BinOpType operateur;
        if (ctx->op->getText() == "+=") {
            operateur = ir::BinOp::ADD;
        } else if (ctx->op->getText() == "-=") {
            operateur = ir::BinOp::SUB;
        } else if (ctx->op->getText() == "*=") {
            operateur = ir::BinOp::MUL;
        } else if (ctx->op->getText() == "/=") {
            operateur = ir::BinOp::DIV;
        } else if (ctx->op->getText() == "&=") {
            operateur = ir::BinOp::AND_BIN;
        } else if (ctx->op->getText() == "^=") {
            operateur = ir::BinOp::XOR_BIN;
        } else if (ctx->op->getText() == "|=") {
            operateur = ir::BinOp::OR_BIN;
        }
        std::unique_ptr<ir::IRInstr> instructionOperation =
            std::make_unique<ir::BinOp>(operateur, tmpTo, to, from);
        instructionAffect = std::make_unique<ir::Affect>(to, tmpTo);

        // ajoute l'instruction operation
        this->currentBlock->addInstr(std::move(instructionOperation));
    }

    this->currentBlock->addInstr(std::move(instructionAffect));
    return to;
}

antlrcpp::Any IRGenVisitor::visitParen(ifccParser::ParenContext *ctx) {
    // Visit the expression inside the parentheses.
    return visit(ctx->expression());
}

antlrcpp::Any IRGenVisitor::visitMult(ifccParser::MultContext *ctx) {
    // Visit the left and right expressions.
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    // Generate a temporary variable name.
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    // Add the temporary variable to the current block's scope.
    this->currentBlock->getScope().addVariable(to, INT);
    std::unique_ptr<ir::IRInstr> instruction;
    // Determine the operation and create the corresponding instruction.
    if (ctx->op->getText()[0] == '*') {
        instruction =
            std::make_unique<ir::BinOp>(ir::BinOp::MUL, to, left, right);
    } else if (ctx->op->getText()[0] == '/') {
        instruction =
            std::make_unique<ir::BinOp>(ir::BinOp::DIV, to, left, right);
    } else {
        instruction =
            std::make_unique<ir::BinOp>(ir::BinOp::MOD, to, left, right);
    }
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

antlrcpp::Any IRGenVisitor::visitAdd(ifccParser::AddContext *ctx) {
    // Visit the left and right expressions.
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    // Generate a temporary variable name.
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    // Add the temporary variable to the current block's scope.
    this->currentBlock->getScope().addVariable(to, INT);
    std::unique_ptr<ir::IRInstr> instruction;
    // Determine the operation and create the corresponding instruction.
    if (ctx->op->getText()[0] == '+') {
        instruction =
            std::make_unique<ir::BinOp>(ir::BinOp::ADD, to, left, right);
    } else {
        instruction =
            std::make_unique<ir::BinOp>(ir::BinOp::SUB, to, left, right);
    }
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

antlrcpp::Any IRGenVisitor::visitUnaryAdd(ifccParser::UnaryAddContext *ctx) {
    // Visit the expression.
    std::string from = visit(ctx->expression());
    // If it's a unary add, return the expression as is.
    if (ctx->op->getText()[0] == '+') {
        return from;
    }
    // Generate a temporary variable name.
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    // Add the temporary variable to the current block's scope.
    this->currentBlock->getScope().addVariable(to, INT);
    std::unique_ptr<ir::UnaryOp> instruction;
    // Determine the operation and create the corresponding instruction.
    if (ctx->op->getText()[0] == '-') {
        instruction = std::make_unique<ir::UnaryOp>(ir::UnaryOp::NEG, to, from);
    } else {
        instruction = std::make_unique<ir::UnaryOp>(ir::UnaryOp::NOT, to, from);
    }
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

antlrcpp::Any IRGenVisitor::visitShift(ifccParser::ShiftContext *ctx) {
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, INT);

    std::unique_ptr<ir::IRInstr> instruction;
    if (ctx->op->getText() == "<<") {
        instruction =
            std::make_unique<ir::BinOp>(ir::BinOp::SHIFT_L, to, left, right);
    } else {
        instruction =
            std::make_unique<ir::BinOp>(ir::BinOp::SHIFT_R, to, left, right);
    }
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

antlrcpp::Any IRGenVisitor::visitCompare(ifccParser::CompareContext *ctx) {
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, INT);

    std::unique_ptr<ir::IRInstr> instruction;
    if (ctx->op->getText() == "<") {
        instruction =
            std::make_unique<ir::BinOp>(ir::BinOp::LT, to, left, right);
    } else if (ctx->op->getText() == ">") {
        instruction =
            std::make_unique<ir::BinOp>(ir::BinOp::GT, to, left, right);
    } else if (ctx->op->getText() == ">=") {
        instruction =
            std::make_unique<ir::BinOp>(ir::BinOp::GTE, to, left, right);
    } else if (ctx->op->getText() == "<=") {
        instruction =
            std::make_unique<ir::BinOp>(ir::BinOp::LTE, to, left, right);
    }
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

antlrcpp::Any IRGenVisitor::visitCompareEq(ifccParser::CompareEqContext *ctx) {
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, INT);

    std::unique_ptr<ir::IRInstr> instruction;
    if (ctx->op->getText() == "==") {
        instruction =
            std::make_unique<ir::BinOp>(ir::BinOp::EQ, to, left, right);
    } else if (ctx->op->getText() == "!=") {
        instruction =
            std::make_unique<ir::BinOp>(ir::BinOp::NEQ, to, left, right);
    }
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

antlrcpp::Any IRGenVisitor::visitXorBin(ifccParser::XorBinContext *ctx) {
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, INT);

    std::unique_ptr<ir::IRInstr> instruction;
    instruction =
        std::make_unique<ir::BinOp>(ir::BinOp::XOR_BIN, to, left, right);
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

antlrcpp::Any IRGenVisitor::visitOrBin(ifccParser::OrBinContext *ctx) {
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, INT);

    std::unique_ptr<ir::IRInstr> instruction;
    instruction =
        std::make_unique<ir::BinOp>(ir::BinOp::OR_BIN, to, left, right);
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

antlrcpp::Any IRGenVisitor::visitAndBin(ifccParser::AndBinContext *ctx) {
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, INT);

    std::unique_ptr<ir::IRInstr> instruction;
    instruction =
        std::make_unique<ir::BinOp>(ir::BinOp::AND_BIN, to, left, right);
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

antlrcpp::Any IRGenVisitor::visitAnd(ifccParser::AndContext *ctx) {
    /*
        b1 && b2
        if b1 {lazyBlock
            if b2 {trueBlock
                tmp = 1;
            }
        }
        {falseBlock
            tmp = 0;
        }
        {followBlock
    */
    std::string left = visit(ctx->expression(0));

    // on ajoute la variable de retour a l'index
    ++counterTempVariables;
    std::string tmp = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(tmp, INT);

    Scope &currentScope = this->currentBlock->getScope();

    // On créé le lazy block auquel on accede uniquement si la premiere partie
    // est vrai
    ir::BasicBlock *lazyBlock = createBlock(".AND");

    // On créé le bloc True auquel on accede si c'est vrai. Alors on met tmp a 1
    ir::BasicBlock *trueBlock = createBlock(".AND");

    // On créé le bloc auquel on accede si c'est faux. Alors on met tmp a 0
    ir::BasicBlock *falseBlock = createBlock(".AND");

    // On créé le bloc de suite
    ir::BasicBlock *followBlock = createBlock(".AND");

    // On jump vers le lazy ou le false en fonction de b1
    auto conditional1 =
        std::make_unique<ir::ConditionalJump>(left, lazyBlock, falseBlock);
    this->currentBlock->setNext(std::move(conditional1));

    // on se deplace dans le block lazy alors on peut evaluer l'expression de
    // droite
    this->currentBlock = std::move(lazyBlock);
    std::string right = visit(ctx->expression(1));
    auto conditional2 =
        std::make_unique<ir::ConditionalJump>(right, trueBlock, falseBlock);
    this->currentBlock->setNext(std::move(conditional2));

    // On se place dans le true block
    this->currentBlock = std::move(trueBlock);
    auto instructionTrue = std::make_unique<ir::AffectConst>(tmp, 1);
    ;
    this->currentBlock->addInstr(std::move(instructionTrue));
    this->currentBlock->setNext(
        std::make_unique<ir::UnconditionalJump>(followBlock));

    // On se place dans le false block
    this->currentBlock = std::move(falseBlock);
    auto instructionFalse = std::make_unique<ir::AffectConst>(tmp, 0);
    this->currentBlock->addInstr(std::move(instructionFalse));
    this->currentBlock->setNext(
        std::make_unique<ir::UnconditionalJump>(followBlock));

    // On se déplace dans le block de suite
    this->currentBlock = std::move(followBlock);

    return tmp;
}

antlrcpp::Any IRGenVisitor::visitOr(ifccParser::OrContext *ctx) {
    /*
        b1 || b2
        if (b1) ---> trueBlock | -/-> lazyBlock
        {lazyBlock
            if (b2) ---> trueBlock | -/-> falseBlock
        }
        {trueBlock
            tmp = 1
        } ---> follow
        {falseBlock
            tmp = 0
        } ---> follow
        {follow
    */
    std::string left = visit(ctx->expression(0));

    // on ajoute la variable de retour a l'index
    ++counterTempVariables;
    std::string tmp = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(tmp, INT);

    Scope &currentScope = this->currentBlock->getScope();

    // On créé le lazy block auquel on accede uniquement si la premiere partie
    // est fausse
    ir::BasicBlock *lazyBlock = createBlock(".OR");

    // On créé le bloc True auquel on accede si c'est vrai. Alors on met tmp a 1
    ir::BasicBlock *trueBlock = createBlock(".OR");

    // On créé le bloc auquel on accede si c'est faux. Alors on met tmp a 0
    ir::BasicBlock *falseBlock = createBlock(".OR");

    // On créé le bloc de suite
    ir::BasicBlock *followBlock = createBlock(".OR");

    // On jump vers le true ou le lazy en fonction de tmp
    auto conditional1 =
        std::make_unique<ir::ConditionalJump>(left, trueBlock, lazyBlock);
    this->currentBlock->setNext(std::move(conditional1));

    // on se deplace dans le block lazy alors on peut evaluer l'expression de
    // droite
    this->currentBlock = std::move(lazyBlock);
    std::string right = visit(ctx->expression(1));
    auto conditional2 =
        std::make_unique<ir::ConditionalJump>(right, trueBlock, falseBlock);
    this->currentBlock->setNext(std::move(conditional2));

    // On se place dans le true block
    this->currentBlock = std::move(trueBlock);
    auto instructionTrue = std::make_unique<ir::AffectConst>(tmp, 1);
    ;
    this->currentBlock->addInstr(std::move(instructionTrue));
    this->currentBlock->setNext(
        std::make_unique<ir::UnconditionalJump>(followBlock));

    // On se place dans le false block
    this->currentBlock = std::move(falseBlock);
    auto instructionFalse = std::make_unique<ir::AffectConst>(tmp, 0);
    this->currentBlock->addInstr(std::move(instructionFalse));
    this->currentBlock->setNext(
        std::make_unique<ir::UnconditionalJump>(followBlock));

    // On se déplace dans le block de suite
    this->currentBlock = std::move(followBlock);

    return tmp;
}

antlrcpp::Any IRGenVisitor::visitPreInc(ifccParser::PreIncContext *ctx) {
    std::string from = ctx->VARIABLE()->getText();
    auto instruction =
        std::make_unique<ir::UnaryOp>(ir::UnaryOp::PRE_INC, from, from);
    this->currentBlock->addInstr(std::move(instruction));
    return from;
}

antlrcpp::Any IRGenVisitor::visitPreDec(ifccParser::PreDecContext *ctx) {
    std::string from = ctx->VARIABLE()->getText();
    auto instruction =
        std::make_unique<ir::UnaryOp>(ir::UnaryOp::PRE_DEC, from, from);
    this->currentBlock->addInstr(std::move(instruction));
    return from;
}

antlrcpp::Any IRGenVisitor::visitPostInc(ifccParser::PostIncContext *ctx) {
    std::string from = ctx->VARIABLE()->getText();
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, INT);
    auto instruction =
        std::make_unique<ir::UnaryOp>(ir::UnaryOp::POST_INC, to, from);
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

antlrcpp::Any IRGenVisitor::visitPostDec(ifccParser::PostDecContext *ctx) {
    std::string from = ctx->VARIABLE()->getText();
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, INT);
    auto instruction =
        std::make_unique<ir::UnaryOp>(ir::UnaryOp::POST_DEC, to, from);
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

/* Creer le block et l'ajoute au CFG actuel */
ir::BasicBlock *IRGenVisitor::createBlock(const std::string &name) {
    std::string n = name + std::to_string(this->counterBlocks);
    ++counterBlocks;
    Scope &currentScope = this->currentBlock->getScope();
    ir::BasicBlock *newBlock = new ir::BasicBlock(&currentScope, n);
    currentFunction->addBlock(std::unique_ptr<ir::BasicBlock>(newBlock));
    return newBlock;
}
