/**
 * @file IRGenVisitor.cpp
 * @author H4231
 * @brief Visite les noeuds du programme converti en AST grace à antlr4 et creer une IR
 * @date 2024-04-02
 */

#include "CustomExceptions.h"
#include "IRGenVisitor.h"
#include "Scope.h"
#include "ifccParser.h"
#include "ir.h"
#include "support/Any.h"
#include <memory>
#include <string>

/**
 * @brief Visite le premier noeud de l'arbre de synthaxe
 *        Arrete le programme en cas d'erreur
 * 
 * @param ctx contexte antlr4 du noeud
 * @return Renvoie toujours 0
 */
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

/**
 * @brief Visite toutes les fonctions du programme
 * 
 * @param ctx contexte antlr4 du noeud
 * @return Renvoie toujours 0
 */
antlrcpp::Any IRGenVisitor::visitProg(ifccParser::ProgContext *ctx) {

    for (const auto &func : ctx->function()) {
        visit(func);
    }
    return 0;
}

/**
 * @brief Visite un block { ... }
 * 
 * @param ctx contexte antlr4 du noeud
 * @return Renvoie toujours 0
 */
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

/**
 * @brief Visite les blocs qui constituent le if 
 *        if ( ... ) { ... } else { ... }
 * 
 * @param ctx contexte antlr4 du noeud
 * @return Renvoie toujours 0
 */
antlrcpp::Any IRGenVisitor::visitIf_stmt(ifccParser::If_stmtContext *ctx) {
    // Extract the condition expression for the if statement.
    std::string condition = this->visit(ctx->expression());

    // Generate a name for the basic block representing the 'then' branch
    ir::BasicBlock *thenBlock = createBlock(".BB");
    ir::BasicBlock *elseBlock = createBlock(".BB");

    ir::BasicBlock *end = elseBlock;
    std::unique_ptr<ir::Next> endNext = currentBlock->getNext();

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

/**
 * @brief Visite le bloc du while
 *        while ( ... ) { ... }
 * 
 * @param ctx contexte antlr4 du noeud
 * @return Renvoie toujours 0
 */
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

/**
 * @brief Visite le bloc d'un do () while
 *        do ( ... ) while { ... }
 * 
 * @param ctx contexte antlr4 du noeud
 * @return Renvoie toujours 0
 */
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

/**
 * @brief Visite le bloc d'un switch
 * 
 * @param ctx 
 * @return antlrcpp::Any 
 */
antlrcpp::Any IRGenVisitor::visitSwitch_stmt(
    ifccParser::Switch_stmtContext *ctx) {
    // Retrieve the current scope.

    auto defaultBlock = this->createBlock(".BB");

    ir::BasicBlock *end = defaultBlock;
    std::unique_ptr<ir::Next> endNext = currentBlock->getNext();

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

/**
 * @brief Visite un continue;
 * 
 * @param ctx contexte antlr4 du noeud
 * @return Renvoie toujours 0
 */
antlrcpp::Any IRGenVisitor::visitContinue_stmt(
    ifccParser::Continue_stmtContext *ctx) {
    throw ContinueException();
    return 0;
}


/**
 * @brief Visite un break;
 * 
 * @param ctx contexte antlr4 du noeud
 * @return Renvoie toujours 0
 */
antlrcpp::Any IRGenVisitor::visitBreak_stmt(
    ifccParser::Break_stmtContext *ctx) {
    throw BreakException();
    return 0;
}

/**
 * @brief Visite un appel à une fonction
 * 
 * @param ctx contexte antlr4 du noeud
 * @return Renvoie le nom de la variable qui contient la valeur de retour de
 *         la fonction
 */
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
    this->currentBlock->getScope().addVariable(tempVarName, VarType::INT);

    return tempVarName;
}

/**
 * @brief Visite le bloc d'une fonction
 * 
 * @param ctx contexte antlr4 du noeud
 * @return retourne le nom de la fonction
 */
antlrcpp::Any IRGenVisitor::visitFunction(ifccParser::FunctionContext *ctx) {
    std::string funcName = ctx->VARIABLE(0)->getText();

    VarType type;

    if (ctx->type(0)->getText() == "int") {
        type = VarType::INT;
    } else {
        type = VarType::VOID;
    }

    // Create the function in the intermediate representation (IR).
    this->currentFunction = std::make_unique<ir::CFG>(funcName, type);

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
        this->currentFunction->addArg(ctx->VARIABLE(i)->getText(),
                                      VarType::INT);
        this->currentBlock->getScope().addVariable(ctx->VARIABLE(i)->getText(),
                                                   VarType::INT);
    }

    // Add a variable "#return" to the current block's scope.
    this->currentBlock->getScope().addVariable("#return", VarType::INT);

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

/**
 * @brief Visite le mot clé return ... ;
 * 
 * @param ctx contexte antlr4 du noeud
 * @return retourne le nom de la fonction
 */
antlrcpp::Any IRGenVisitor::visitReturn_stmt(
    ifccParser::Return_stmtContext *ctx) {
    if (ctx->expression()) {
        // Visit the expression to be returned.
        std::string toReturn = this->visit(ctx->expression());
        // Create an instruction to affect the return value.
        auto instruction = std::make_unique<ir::Affect>("#return", toReturn);
        this->currentBlock->addInstr(std::move(instruction));
    }
    // Set up an unconditional jump to the function's epilogue.
    this->currentBlock->setNext(std::make_unique<ir::UnconditionalJump>(
        &this->currentFunction->getEpilogue()));

    throw ReturnException();

    return 0;
}

/**
 * @brief visite les déclarations de la forme var = exp
 *        ajoute la variable à la table des symboles 
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie toujours 0
 */
antlrcpp::Any IRGenVisitor::visitDeclaAffect(
    ifccParser::DeclaAffectContext *ctx) {
    // Add a variable declaration to the current block's scope.
    this->currentBlock->getScope().addVariable(ctx->VARIABLE()->getText(),
                                               VarType::INT);
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

/**
 * @brief Visite un nombre entier, Creer une variable temporaire et l'ajoute
 *        à la table de symbole 
 * 
 * @param ctx contexte antlr4 du noeud
 * @return le nom de la varibale créé
 */
antlrcpp::Any IRGenVisitor::visitIntConst(ifccParser::IntConstContext *ctx) {
    // Extract the integer value from the context.
    int value = stoi(ctx->INT_CONST()->getText());

    // Generate a temporary variable name.
    ++this->counterTempVariables;
    std::string variableName = "#" + std::to_string(this->counterTempVariables);
    // Add the temporary variable to the current block's scope.
    this->currentBlock->getScope().addVariable(variableName, VarType::INT);
    // Create an instruction to affect the temporary variable with the integer
    // value.
    auto instruction = std::make_unique<ir::AffectConst>(variableName, value);
    this->currentBlock->addInstr(std::move(instruction));
    return variableName;
}

/**
 * @brief Visite un caractère entre cotes. Creer une variable temporaire et
 *        l'ajoute à la table de symbole 
 * 
 * @param ctx contexte antlr4 du noeud
 * @return le nom de la varibale créé
 */
antlrcpp::Any IRGenVisitor::visitCharConst(ifccParser::CharConstContext *ctx) {
    // Extract the character value from the context.
    int value = ctx->CHAR_CONST()->getText()[1];

    // Generate a temporary variable name.
    ++this->counterTempVariables;
    std::string variableName = "#" + std::to_string(this->counterTempVariables);
    // Add the temporary variable to the current block's scope.
    this->currentBlock->getScope().addVariable(variableName, VarType::INT);
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

/**
 * @brief Visite une expression de type affectation 
 *        a = b, a+=b, etc
 * 
 * @param ctx contexte antlr4 du noeud
 * @return le nom de la varibale du resultat
 */
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
        this->currentBlock->getScope().addVariable(tmpTo, VarType::INT);

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

/**
 * @brief Visite le contenue des parenthèses
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
antlrcpp::Any IRGenVisitor::visitParen(ifccParser::ParenContext *ctx) {
    return visit(ctx->expression());
}

/**
 * @brief Visite une expression de multiplication, division ou modulo
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
antlrcpp::Any IRGenVisitor::visitMult(ifccParser::MultContext *ctx) {
    // Visit the left and right expressions.
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    // Generate a temporary variable name.
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    // Add the temporary variable to the current block's scope.
    this->currentBlock->getScope().addVariable(to, VarType::INT);
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

/**
 * @brief Visite une expression d'addition ou de soustraction
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
antlrcpp::Any IRGenVisitor::visitAdd(ifccParser::AddContext *ctx) {
    // Visit the left and right expressions.
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    // Generate a temporary variable name.
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    // Add the temporary variable to the current block's scope.
    this->currentBlock->getScope().addVariable(to, VarType::INT);
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

/**
 * @brief Visite une expression de plus ou moins unaire
 *          -a, +a
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
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
    this->currentBlock->getScope().addVariable(to, VarType::INT);
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

/**
 * @brief Visite une expression de décalage binaire
 *          a << exp, a >> exp
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
antlrcpp::Any IRGenVisitor::visitShift(ifccParser::ShiftContext *ctx) {
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, VarType::INT);

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

/**
 * @brief Visite une expression de comparaison
 *         exp >= exp, exp < exp, etc
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
antlrcpp::Any IRGenVisitor::visitCompare(ifccParser::CompareContext *ctx) {
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, VarType::INT);

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

/**
 * @brief Visite une expression d'égalité
 *         exp != exp, exp == exp
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
antlrcpp::Any IRGenVisitor::visitCompareEq(ifccParser::CompareEqContext *ctx) {
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, VarType::INT);

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

/**
 * @brief Visite une expression d'ou exclusif
 *         exp ^ exp
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
antlrcpp::Any IRGenVisitor::visitXorBin(ifccParser::XorBinContext *ctx) {
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, VarType::INT);

    std::unique_ptr<ir::IRInstr> instruction;
    instruction =
        std::make_unique<ir::BinOp>(ir::BinOp::XOR_BIN, to, left, right);
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

/**
 * @brief Visite une expression d'ou
 *         exp | exp
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
antlrcpp::Any IRGenVisitor::visitOrBin(ifccParser::OrBinContext *ctx) {
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, VarType::INT);

    std::unique_ptr<ir::IRInstr> instruction;
    instruction =
        std::make_unique<ir::BinOp>(ir::BinOp::OR_BIN, to, left, right);
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

/**
 * @brief Visite une expression de et
 *         exp & exp
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
antlrcpp::Any IRGenVisitor::visitAndBin(ifccParser::AndBinContext *ctx) {
    std::string left = visit(ctx->expression(0));
    std::string right = visit(ctx->expression(1));
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, VarType::INT);

    std::unique_ptr<ir::IRInstr> instruction;
    instruction =
        std::make_unique<ir::BinOp>(ir::BinOp::AND_BIN, to, left, right);
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

/**
 * @brief Visite une expression de et logique paresseux
 *          exp && exp
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
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
    this->currentBlock->getScope().addVariable(tmp, VarType::INT);

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

/**
 * @brief Visite une expression de et logique paresseux
 *          exp || exp
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
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
    this->currentBlock->getScope().addVariable(tmp, VarType::INT);

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

/**
 * @brief Visite une expression de pré-incrémentation
 *          ++exp
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
antlrcpp::Any IRGenVisitor::visitPreInc(ifccParser::PreIncContext *ctx) {
    std::string from = ctx->VARIABLE()->getText();
    auto instruction =
        std::make_unique<ir::UnaryOp>(ir::UnaryOp::PRE_INC, from, from);
    this->currentBlock->addInstr(std::move(instruction));
    return from;
}

/**
 * @brief Visite une expression de pré-décrémentation
 *          --exp
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
antlrcpp::Any IRGenVisitor::visitPreDec(ifccParser::PreDecContext *ctx) {
    std::string from = ctx->VARIABLE()->getText();
    auto instruction =
        std::make_unique<ir::UnaryOp>(ir::UnaryOp::PRE_DEC, from, from);
    this->currentBlock->addInstr(std::move(instruction));
    return from;
}

/**
 * @brief Visite une expression de post-incrémentation
 *          exp++
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
antlrcpp::Any IRGenVisitor::visitPostInc(ifccParser::PostIncContext *ctx) {
    std::string from = ctx->VARIABLE()->getText();
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, VarType::INT);
    auto instruction =
        std::make_unique<ir::UnaryOp>(ir::UnaryOp::POST_INC, to, from);
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

/**
 * @brief Visite une expression de pré-décrementation
 *          exp--
 * 
 * @param ctx contexte antlr4 du noeud
 * @return renvoie le nom de la varibale qui contient le resultat de l'expression
 */
antlrcpp::Any IRGenVisitor::visitPostDec(ifccParser::PostDecContext *ctx) {
    std::string from = ctx->VARIABLE()->getText();
    ++counterTempVariables;
    std::string to = "#" + std::to_string(counterTempVariables);
    this->currentBlock->getScope().addVariable(to, VarType::INT);
    auto instruction =
        std::make_unique<ir::UnaryOp>(ir::UnaryOp::POST_DEC, to, from);
    this->currentBlock->addInstr(std::move(instruction));
    return to;
}

/* Creer le block et l'ajoute au CFG actuel */

/**
 * @brief Construit un bloc et l'ajoute au CFG actuel
 * 
 * @param name le nom du bloc
 * @return le bloc qui a été créé
 */
ir::BasicBlock *IRGenVisitor::createBlock(const std::string &name) {
    std::string n = name + std::to_string(this->counterBlocks);
    ++counterBlocks;
    Scope &currentScope = this->currentBlock->getScope();
    ir::BasicBlock *newBlock = new ir::BasicBlock(&currentScope, n);
    currentFunction->addBlock(std::unique_ptr<ir::BasicBlock>(newBlock));
    return newBlock;
}
