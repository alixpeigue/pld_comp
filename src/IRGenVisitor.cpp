#include "IRGenVisitor.h"
#include "Scope.h"
#include "ifccParser.h"
#include "ir.h"
#include "support/Any.h"
#include <memory>
#include <string>

antlrcpp::Any IRGenVisitor::visitProg(ifccParser::ProgContext *ctx) {
    // Create the main function in the intermediate representation (IR).
    this->currentFunction = std::make_unique<ir::CFG>("main");

    // Create a global scope for the program.
    Scope *scope = new Scope(nullptr);
    this->currentFunction->addScope(std::unique_ptr<Scope>(scope));

    // Create the initial basic block for the program.
    std::string name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    this->currentBlock =
        std::make_unique<ir::BasicBlock>(scope, std::move(name));

    // Create the epilogue block for the main function.
    ir::BasicBlock *epilogue = new ir::BasicBlock(scope, ".main_ret");
    epilogue->setNext(std::make_unique<ir::Return>());
    this->currentFunction->setEpilogue(epilogue);

    // Visit children nodes to generate IR code.
    this->visitChildren(ctx);
    // Add a variable "#return" to the current block's scope.
    currentBlock->getScope().addVariable("#return", INT);
    // Add current block and epilogue block to the main function.
    this->currentFunction->addBlock(std::move(this->currentBlock));
    this->currentFunction->addBlock(std::unique_ptr<ir::BasicBlock>(epilogue));
    // Push the main function to the list of functions in the IR.
    this->ir.push_back(std::move(this->currentFunction));
    return 0;
}

antlrcpp::Any IRGenVisitor::visitScope(ifccParser::ScopeContext *ctx) {
    // Create a new block, new scope, and set up an unconditional jump.
    Scope &currentScope = this->currentBlock->getScope();
    Scope *newScope = new Scope(&currentScope);
    this->currentFunction->addScope(std::unique_ptr<Scope>(newScope));
    std::string name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    auto block = std::make_unique<ir::BasicBlock>(newScope, name);
    std::swap(block, this->currentBlock);
    block->setNext(
        std::make_unique<ir::UnconditionalJump>(this->currentBlock.get()));
    this->currentFunction->addBlock(std::move(block));

    // Visit inside the scope to generate IR code for its contents.
    this->visitChildren(ctx);

    // Create a new block with the previous scope and set up an unconditional jump.
    name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    block = std::make_unique<ir::BasicBlock>(&currentScope, name);

    this->currentBlock->setNext(
        std::make_unique<ir::UnconditionalJump>(block.get()));
    this->currentFunction->addBlock(std::move(this->currentBlock));
    this->currentBlock = std::move(block);

    return 0;
}

// Visit the condition expression of the if statement.
antlrcpp::Any IRGenVisitor::visitIf_stmt(ifccParser::If_stmtContext *ctx) {
    // Extract the condition expression for the if statement.
    std::string condition = this->visit(ctx->expression());

    // Create new blocks for the then and else branches, and set up jumps.
    Scope &currentScope = this->currentBlock->getScope();
    
    // Generate a name for the basic block representing the 'then' branch.
    std::string name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    auto thenBlock = std::make_unique<ir::BasicBlock>(&currentScope, name);

    // Generate a name for the basic block representing the 'else' branch.
    name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    auto elseBlock = std::make_unique<ir::BasicBlock>(&currentScope, name);

    // Initialize the 'end' block to point to the else block.
    ir::BasicBlock *end = elseBlock.get();

    // Check if there is an 'else' statement.
    if (ctx->else_stmt) {
        // If so, create a new block for the end of the if statement.
        name = ".BB" + std::to_string(this->counterBlocks);
        ++counterBlocks;
        end = new ir::BasicBlock(&currentScope, name);
    }

    // Create a conditional jump instruction based on the condition.
    auto conditional = std::make_unique<ir::ConditionalJump>(
        std::move(condition), thenBlock.get(), elseBlock.get());

    // Set the next block for the current block to be the conditional jump.
    this->currentBlock->setNext(std::move(conditional));

    // Add the current block to the list of blocks in the current function.
    this->currentFunction->addBlock(std::move(this->currentBlock));

    // Move to the 'then' block to generate code for the 'then' branch.
    this->currentBlock = std::move(thenBlock);

    // Generate code for the 'then' branch of the if statement.
    this->visit(ctx->then_stmt);

    // Set up an unconditional jump to the end of the if statement.
    this->currentBlock->setNext(std::make_unique<ir::UnconditionalJump>(end));

    // Add the 'then' block to the list of blocks in the current function.
    this->currentFunction->addBlock(std::move(this->currentBlock));

    // Move to the 'else' block to generate code for the 'else' branch (if any).
    this->currentBlock = std::move(elseBlock);

    // If there is an 'else' statement, generate code for it.
    if (ctx->else_stmt) {
        this->visit(ctx->else_stmt);

        // Set up an unconditional jump to the end of the if statement.
        this->currentBlock->setNext(
            std::make_unique<ir::UnconditionalJump>(end));

        // Add the 'else' block to the list of blocks in the current function.
        this->currentFunction->addBlock(std::move(this->currentBlock));

        // Set the current block to the 'end' block.
        this->currentBlock = std::unique_ptr<ir::BasicBlock>(end);
    }

    // Return 0, indicating successful visitation.
    return 0;
}

// Visits a while loop statement.
antlrcpp::Any IRGenVisitor::visitWhile_stmt(ifccParser::While_stmtContext *ctx) {
    // Retrieve the current scope.
    Scope &currentScope = this->currentBlock->getScope();

    // Create a new basic block for the loop condition.
    std::string name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    auto conditionBlock = std::make_unique<ir::BasicBlock>(&currentScope, name);
    ir::BasicBlock *conditionBlockPtr = conditionBlock.get();

    // Create new basic blocks for the 'then' and 'else' branches of the loop.
    name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    auto thenBlock = std::make_unique<ir::BasicBlock>(&currentScope, name);

    name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    auto elseBlock = std::make_unique<ir::BasicBlock>(&currentScope, name);

    // Set up an unconditional jump from the current block to the condition block.
    this->currentBlock->setNext(std::make_unique<ir::UnconditionalJump>(conditionBlockPtr));
    // Add the current block to the list of blocks in the current function.
    this->currentFunction->addBlock(std::move(this->currentBlock));
    // Move to the condition block.
    this->currentBlock = std::move(conditionBlock);

    // Generate code for the loop condition.
    std::string condition = this->visit(ctx->expression());
    // Create a conditional jump based on the loop condition.
    auto conditional = std::make_unique<ir::ConditionalJump>(
        std::move(condition), thenBlock.get(), elseBlock.get());
    // Set the next block for the condition block to be the conditional jump.
    this->currentBlock->setNext(std::move(conditional));
    // Add the condition block to the list of blocks in the current function.
    this->currentFunction->addBlock(std::move(this->currentBlock));
    // Move to the 'then' block.
    this->currentBlock = std::move(thenBlock);

    // Generate code for the loop body.
    this->visit(ctx->then_stmt);
    // Set up an unconditional jump from the 'then' block back to the condition block.
    this->currentBlock->setNext(std::make_unique<ir::UnconditionalJump>(conditionBlockPtr));
    // Add the 'then' block to the list of blocks in the current function.
    this->currentFunction->addBlock(std::move(this->currentBlock));

    // Move to the 'else' block.
    this->currentBlock = std::move(elseBlock);

    // Return 0, indicating successful visitation.
    return 0;
}

// Visits a do-while loop statement.
antlrcpp::Any IRGenVisitor::visitDo_while_stmt(ifccParser::Do_while_stmtContext *ctx) {
    // Retrieve the current scope.
    Scope &currentScope = this->currentBlock->getScope();

    // Create new basic blocks for the 'then' and 'else' branches of the loop.
    std::string name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    auto thenBlock = std::make_unique<ir::BasicBlock>(&currentScope, name);
    ir::BasicBlock *thenBlockPtr = thenBlock.get();

    name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    auto elseBlock = std::make_unique<ir::BasicBlock>(&currentScope, name);

    // Set up an unconditional jump from the current block to the 'then' block.
    this->currentBlock->setNext(std::make_unique<ir::UnconditionalJump>(thenBlockPtr));
    // Add the current block to the list of blocks in the current function.
    this->currentFunction->addBlock(std::move(this->currentBlock));
    // Move to the 'then' block.
    this->currentBlock = std::move(thenBlock);

    // Generate code for the loop body.
    this->visit(ctx->then_stmt);

    // Generate code for the loop condition.
    std::string condition = this->visit(ctx->expression());
    // Create a conditional jump based on the loop condition.
    auto conditional = std::make_unique<ir::ConditionalJump>(
        std::move(condition), thenBlockPtr, elseBlock.get());
    // Set the next block for the condition block to be the conditional jump.
    this->currentBlock->setNext(std::move(conditional));
    // Add the 'then' block to the list of blocks in the current function.
    this->currentFunction->addBlock(std::move(this->currentBlock));

    // Move to the 'else' block.
    this->currentBlock = std::move(elseBlock);

    // Return 0, indicating successful visitation.
    return 0;
}

antlrcpp::Any IRGenVisitor::visitFunc_call(ifccParser::Func_callContext *ctx) {
    std::string tempVarName = "#" + std::to_string(this->counterTempVariables);
    auto instruction = std::make_unique<ir::Call>(ctx->VARIABLE()->getText(), tempVarName);
    ++this->counterTempVariables;
    for (const auto & expr : ctx->expression()) {
        std::string name = this->visit(expr);

        instruction->addName(name);
    }
    this->currentBlock->addInstr(std::move(instruction));
    this->currentBlock->getScope().addVariable(tempVarName, INT);

    return tempVarName;
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
    return 0;
}

antlrcpp::Any IRGenVisitor::visitDeclaAffect(
    ifccParser::DeclaAffectContext *ctx) {
    // Add a variable declaration to the current block's scope.
    this->currentBlock->getScope().addVariable(ctx->VARIABLE()->getText(), INT);
    if (ctx->expression()) {
        // If there's an expression, create an instruction to affect the variable.
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
    // Create an instruction to affect the temporary variable with the integer value.
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
    // Create an instruction to affect the temporary variable with the character value.
    auto instruction = std::make_unique<ir::AffectConst>(variableName, value);
    this->currentBlock->addInstr(std::move(instruction));
    return variableName;
}

antlrcpp::Any IRGenVisitor::visitVariable(ifccParser::VariableContext *ctx) {
    // Return the name of the variable.
    return ctx->VARIABLE()->getText();
}

antlrcpp::Any IRGenVisitor::visitAffect(ifccParser::AffectContext *ctx) {
    // Extract the source and destination variables from the context.
    std::string to = ctx->VARIABLE()->getText();
    std::string from = (std::string)this->visit(ctx->expression());
    // Create an instruction to affect the destination variable with the source.
    auto instruction = std::make_unique<ir::Affect>(to, from);
    this->currentBlock->addInstr(std::move(instruction));
    return from;
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
