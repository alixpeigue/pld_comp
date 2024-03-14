#include "IRGenVisitor.h"
#include "Scope.h"
#include "ifccParser.h"
#include "ir.h"
#include "support/Any.h"
#include <memory>
#include <string>

antlrcpp::Any IRGenVisitor::visitProg(ifccParser::ProgContext *ctx) {
    // auto scope = std::make_unique<Scope>(nullptr);
    // this->currentBlock =
    //     std::make_unique<ir::BasicBlock>(std::move(scope), "main");
    // this->currentBlock->getScope().addVariable("#return", INT);
    // this->visitChildren(ctx);
    // this->ir.addRoot(std::move(this->currentBlock));

    // // Create first basic block
    // this->currentBlock =
    //     std::make_unique<ir::BasicBlock>(Scope(nullptr), ".BB0");
    // this->currentFunction = std::make_unique<ir::CFG>("main");

    // // Creating epilogue
    // auto epilogue = std::make_unique<ir::BasicBlock>(
    //     this->currentBlock->getScope(), ".main_ret");
    // this->currentFunction
    //     ->addBlock(epilogue)

    this->currentFunction = std::make_unique<ir::CFG>("main");

    Scope *scope = new Scope(nullptr);
    this->currentFunction->addScope(std::unique_ptr<Scope>(scope));

    std::string name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;

    this->currentBlock =
        std::make_unique<ir::BasicBlock>(scope, std::move(name));

    ir::BasicBlock *epilogue = new ir::BasicBlock(scope, ".main_ret");
    epilogue->setNext(std::make_unique<ir::Return>());
    this->currentFunction->setEpilogue(epilogue);

    this->visitChildren(ctx);
    currentBlock->getScope().addVariable("#return", INT);
    // this->currentFunction->mainScope = &this->currentBlock->getScope();
    this->currentFunction->addBlock(std::move(this->currentBlock));
    this->currentFunction->addBlock(std::unique_ptr<ir::BasicBlock>(epilogue));
    this->ir.push_back(std::move(this->currentFunction));
    return 0;
}

antlrcpp::Any IRGenVisitor::visitScope(ifccParser::ScopeContext *ctx) {

    // Create new block, new scope and setup unconditional jump
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

    // Visit inside scope
    this->visitChildren(ctx);

    // Create new block with previous scope and setup unconditional jump
    name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    block = std::make_unique<ir::BasicBlock>(&currentScope, name);

    this->currentBlock->setNext(
        std::make_unique<ir::UnconditionalJump>(block.get()));
    this->currentFunction->addBlock(std::move(this->currentBlock));
    this->currentBlock = std::move(block);

    return 0;
}

antlrcpp::Any IRGenVisitor::visitIf_stmt(ifccParser::If_stmtContext *ctx) {
    // Visit condition
    std::string condition = this->visit(ctx->expression());

    // Create new blocks and setup jumps
    Scope &currentScope = this->currentBlock->getScope();
    std::string name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    auto thenBlock = std::make_unique<ir::BasicBlock>(&currentScope, name);
    name = ".BB" + std::to_string(this->counterBlocks);
    ++counterBlocks;
    auto elseBlock = std::make_unique<ir::BasicBlock>(&currentScope, name);

    ir::BasicBlock *end = elseBlock.get();

    if (ctx->else_stmt) {
        name = ".BB" + std::to_string(this->counterBlocks);
        ++counterBlocks;
        end = new ir::BasicBlock(&currentScope, name);
    }

    auto conditional = std::make_unique<ir::ConditionalJump>(
        std::move(condition), thenBlock.get(), elseBlock.get());

    this->currentBlock->setNext(std::move(conditional));

    this->currentFunction->addBlock(std::move(this->currentBlock));

    this->currentBlock = std::move(thenBlock);

    this->visit(ctx->then_stmt);

    this->currentBlock->setNext(std::make_unique<ir::UnconditionalJump>(end));

    this->currentFunction->addBlock(std::move(this->currentBlock));

    this->currentBlock = std::move(elseBlock);

    if (ctx->else_stmt) {
        this->visit(ctx->else_stmt);
        this->currentBlock->setNext(
            std::make_unique<ir::UnconditionalJump>(end));
        this->currentFunction->addBlock(std::move(this->currentBlock));
        this->currentBlock = std::unique_ptr<ir::BasicBlock>(end);
    }

    return 0;
}

antlrcpp::Any IRGenVisitor::visitReturn_stmt(
    ifccParser::Return_stmtContext *ctx) {
    std::string toReturn = this->visit(ctx->expression());
    auto instruction = std::make_unique<ir::Affect>("#return", toReturn);
    this->currentBlock->addInstr(std::move(instruction));
    this->currentBlock->setNext(std::make_unique<ir::UnconditionalJump>(
        &this->currentFunction->getEpilogue()));
    return 0;
}

antlrcpp::Any IRGenVisitor::visitDeclaAffect(
    ifccParser::DeclaAffectContext *ctx) {
    this->currentBlock->getScope().addVariable(ctx->VARIABLE()->getText(), INT);
    if (ctx->expression()) {
        std::string to = ctx->VARIABLE()->getText();
        std::string from = (std::string)this->visit(ctx->expression());
        auto instruction = std::make_unique<ir::Affect>(to, from);
        this->currentBlock->addInstr(std::move(instruction));
    }
    return 0;
}

antlrcpp::Any IRGenVisitor::visitIntConst(ifccParser::IntConstContext *ctx) {
    int value = stoi(ctx->INT_CONST()->getText());

    ++this->counterTempVariables;
    std::string variableName = "#" + std::to_string(this->counterTempVariables);
    this->currentBlock->getScope().addVariable(variableName, INT);
    auto instruction = std::make_unique<ir::AffectConst>(variableName, value);
    this->currentBlock->addInstr(std::move(instruction));
    return variableName;
}

antlrcpp::Any IRGenVisitor::visitCharConst(ifccParser::CharConstContext *ctx) {
    int value = ctx->CHAR_CONST()->getText()[1];

    ++this->counterTempVariables;
    std::string variableName = "#" + std::to_string(this->counterTempVariables);
    this->currentBlock->getScope().addVariable(variableName, INT);
    auto instruction = std::make_unique<ir::AffectConst>(variableName, value);
    this->currentBlock->addInstr(std::move(instruction));
    return variableName;
}

antlrcpp::Any IRGenVisitor::visitVariable(ifccParser::VariableContext *ctx) {
    return ctx->VARIABLE()->getText();
}

antlrcpp::Any IRGenVisitor::visitAffect(ifccParser::AffectContext *ctx) {
    std::string to = ctx->VARIABLE()->getText();
    std::string from = (std::string)this->visit(ctx->expression());
    auto instruction = std::make_unique<ir::Affect>(to, from);
    this->currentBlock->addInstr(std::move(instruction));
    return from;
}

antlrcpp::Any IRGenVisitor::visitParen(ifccParser::ParenContext *ctx) {
    return visit(ctx->expression());
}

antlrcpp::Any IRGenVisitor::visitMult(ifccParser::MultContext *ctx) {
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, INT);
    std::unique_ptr<ir::IRInstr> instruction;
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
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, INT);
    std::unique_ptr<ir::IRInstr> instruction;
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
    std::string from = visit(ctx->expression());
    if (ctx->op->getText()[0] == '+') {
        return from;
    }
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, INT);
    std::unique_ptr<ir::UnaryOp> instruction;
    if (ctx->op->getText()[0] == '+') {
        instruction = std::make_unique<ir::UnaryOp>(ir::UnaryOp::NEG, to, from);
    } else {
        instruction = std::make_unique<ir::UnaryOp>(ir::UnaryOp::NOT, to, from);
    }
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}
