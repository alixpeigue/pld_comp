// Generated from /home/relmir/Bureau/PLD_COMP_HEXA/pld-comp/grammar/ifcc.g4 by ANTLR 4.13.1
import org.antlr.v4.runtime.tree.ParseTreeListener;

/**
 * This interface defines a complete listener for a parse tree produced by
 * {@link ifccParser}.
 */
public interface ifccListener extends ParseTreeListener {
	/**
	 * Enter a parse tree produced by {@link ifccParser#axiom}.
	 * @param ctx the parse tree
	 */
	void enterAxiom(ifccParser.AxiomContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#axiom}.
	 * @param ctx the parse tree
	 */
	void exitAxiom(ifccParser.AxiomContext ctx);
	/**
	 * Enter a parse tree produced by {@link ifccParser#prog}.
	 * @param ctx the parse tree
	 */
	void enterProg(ifccParser.ProgContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#prog}.
	 * @param ctx the parse tree
	 */
	void exitProg(ifccParser.ProgContext ctx);
	/**
	 * Enter a parse tree produced by {@link ifccParser#declaration}.
	 * @param ctx the parse tree
	 */
	void enterDeclaration(ifccParser.DeclarationContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#declaration}.
	 * @param ctx the parse tree
	 */
	void exitDeclaration(ifccParser.DeclarationContext ctx);
	/**
	 * Enter a parse tree produced by {@link ifccParser#declaAffect}.
	 * @param ctx the parse tree
	 */
	void enterDeclaAffect(ifccParser.DeclaAffectContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#declaAffect}.
	 * @param ctx the parse tree
	 */
	void exitDeclaAffect(ifccParser.DeclaAffectContext ctx);
	/**
	 * Enter a parse tree produced by {@link ifccParser#return_stmt}.
	 * @param ctx the parse tree
	 */
	void enterReturn_stmt(ifccParser.Return_stmtContext ctx);
	/**
	 * Exit a parse tree produced by {@link ifccParser#return_stmt}.
	 * @param ctx the parse tree
	 */
	void exitReturn_stmt(ifccParser.Return_stmtContext ctx);
	/**
	 * Enter a parse tree produced by the {@code add}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void enterAdd(ifccParser.AddContext ctx);
	/**
	 * Exit a parse tree produced by the {@code add}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void exitAdd(ifccParser.AddContext ctx);
	/**
	 * Enter a parse tree produced by the {@code paren}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void enterParen(ifccParser.ParenContext ctx);
	/**
	 * Exit a parse tree produced by the {@code paren}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void exitParen(ifccParser.ParenContext ctx);
	/**
	 * Enter a parse tree produced by the {@code mult}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void enterMult(ifccParser.MultContext ctx);
	/**
	 * Exit a parse tree produced by the {@code mult}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void exitMult(ifccParser.MultContext ctx);
	/**
	 * Enter a parse tree produced by the {@code affect}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void enterAffect(ifccParser.AffectContext ctx);
	/**
	 * Exit a parse tree produced by the {@code affect}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void exitAffect(ifccParser.AffectContext ctx);
	/**
	 * Enter a parse tree produced by the {@code charConst}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void enterCharConst(ifccParser.CharConstContext ctx);
	/**
	 * Exit a parse tree produced by the {@code charConst}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void exitCharConst(ifccParser.CharConstContext ctx);
	/**
	 * Enter a parse tree produced by the {@code shift}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void enterShift(ifccParser.ShiftContext ctx);
	/**
	 * Exit a parse tree produced by the {@code shift}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void exitShift(ifccParser.ShiftContext ctx);
	/**
	 * Enter a parse tree produced by the {@code variable}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void enterVariable(ifccParser.VariableContext ctx);
	/**
	 * Exit a parse tree produced by the {@code variable}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void exitVariable(ifccParser.VariableContext ctx);
	/**
	 * Enter a parse tree produced by the {@code unaryAdd}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void enterUnaryAdd(ifccParser.UnaryAddContext ctx);
	/**
	 * Exit a parse tree produced by the {@code unaryAdd}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void exitUnaryAdd(ifccParser.UnaryAddContext ctx);
	/**
	 * Enter a parse tree produced by the {@code intConst}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void enterIntConst(ifccParser.IntConstContext ctx);
	/**
	 * Exit a parse tree produced by the {@code intConst}
	 * labeled alternative in {@link ifccParser#expression}.
	 * @param ctx the parse tree
	 */
	void exitIntConst(ifccParser.IntConstContext ctx);
}