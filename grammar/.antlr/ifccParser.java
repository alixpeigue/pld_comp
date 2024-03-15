// Generated from /home/relmir/Bureau/PLD_COMP_HEXA/pld-comp/grammar/ifcc.g4 by ANTLR 4.13.1
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.misc.*;
import org.antlr.v4.runtime.tree.*;
import java.util.List;
import java.util.Iterator;
import java.util.ArrayList;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast", "CheckReturnValue"})
public class ifccParser extends Parser {
	static { RuntimeMetaData.checkVersion("4.13.1", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		T__0=1, T__1=2, T__2=3, T__3=4, T__4=5, T__5=6, T__6=7, T__7=8, T__8=9, 
		T__9=10, T__10=11, T__11=12, T__12=13, T__13=14, T__14=15, T__15=16, T__16=17, 
		T__17=18, T__18=19, T__19=20, T__20=21, T__21=22, RETURN=23, INT_CONST=24, 
		CHAR_CONST=25, VARIABLE=26, COMMENT=27, DIRECTIVE=28, WS=29;
	public static final int
		RULE_axiom = 0, RULE_prog = 1, RULE_declaration = 2, RULE_declaAffect = 3, 
		RULE_return_stmt = 4, RULE_expression = 5;
	private static String[] makeRuleNames() {
		return new String[] {
			"axiom", "prog", "declaration", "declaAffect", "return_stmt", "expression"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, "'int'", "'main'", "'('", "')'", "'{'", "';'", "'}'", "','", "'='", 
			"'-'", "'+'", "'*'", "'/'", "'%'", "'<<'", "'>>'", "'<'", "'>'", "'<='", 
			"'>='", "'=='", "'!='", "'return'"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, null, null, null, null, null, null, null, null, null, null, "RETURN", 
			"INT_CONST", "CHAR_CONST", "VARIABLE", "COMMENT", "DIRECTIVE", "WS"
		};
	}
	private static final String[] _SYMBOLIC_NAMES = makeSymbolicNames();
	public static final Vocabulary VOCABULARY = new VocabularyImpl(_LITERAL_NAMES, _SYMBOLIC_NAMES);

	/**
	 * @deprecated Use {@link #VOCABULARY} instead.
	 */
	@Deprecated
	public static final String[] tokenNames;
	static {
		tokenNames = new String[_SYMBOLIC_NAMES.length];
		for (int i = 0; i < tokenNames.length; i++) {
			tokenNames[i] = VOCABULARY.getLiteralName(i);
			if (tokenNames[i] == null) {
				tokenNames[i] = VOCABULARY.getSymbolicName(i);
			}

			if (tokenNames[i] == null) {
				tokenNames[i] = "<INVALID>";
			}
		}
	}

	@Override
	@Deprecated
	public String[] getTokenNames() {
		return tokenNames;
	}

	@Override

	public Vocabulary getVocabulary() {
		return VOCABULARY;
	}

	@Override
	public String getGrammarFileName() { return "ifcc.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public ATN getATN() { return _ATN; }

	public ifccParser(TokenStream input) {
		super(input);
		_interp = new ParserATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@SuppressWarnings("CheckReturnValue")
	public static class AxiomContext extends ParserRuleContext {
		public ProgContext prog() {
			return getRuleContext(ProgContext.class,0);
		}
		public TerminalNode EOF() { return getToken(ifccParser.EOF, 0); }
		public AxiomContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_axiom; }
	}

	public final AxiomContext axiom() throws RecognitionException {
		AxiomContext _localctx = new AxiomContext(_ctx, getState());
		enterRule(_localctx, 0, RULE_axiom);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(12);
			prog();
			setState(13);
			match(EOF);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class ProgContext extends ParserRuleContext {
		public Return_stmtContext return_stmt() {
			return getRuleContext(Return_stmtContext.class,0);
		}
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public List<DeclarationContext> declaration() {
			return getRuleContexts(DeclarationContext.class);
		}
		public DeclarationContext declaration(int i) {
			return getRuleContext(DeclarationContext.class,i);
		}
		public ProgContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_prog; }
	}

	public final ProgContext prog() throws RecognitionException {
		ProgContext _localctx = new ProgContext(_ctx, getState());
		enterRule(_localctx, 2, RULE_prog);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(15);
			match(T__0);
			setState(16);
			match(T__1);
			setState(17);
			match(T__2);
			setState(18);
			match(T__3);
			setState(19);
			match(T__4);
			setState(28);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while ((((_la) & ~0x3f) == 0 && ((1L << _la) & 117443594L) != 0)) {
				{
				setState(26);
				_errHandler.sync(this);
				switch (_input.LA(1)) {
				case T__2:
				case T__9:
				case T__10:
				case INT_CONST:
				case CHAR_CONST:
				case VARIABLE:
					{
					setState(20);
					expression(0);
					setState(21);
					match(T__5);
					}
					break;
				case T__0:
					{
					setState(23);
					declaration();
					setState(24);
					match(T__5);
					}
					break;
				default:
					throw new NoViableAltException(this);
				}
				}
				setState(30);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			setState(31);
			return_stmt();
			setState(32);
			match(T__6);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class DeclarationContext extends ParserRuleContext {
		public List<DeclaAffectContext> declaAffect() {
			return getRuleContexts(DeclaAffectContext.class);
		}
		public DeclaAffectContext declaAffect(int i) {
			return getRuleContext(DeclaAffectContext.class,i);
		}
		public DeclarationContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_declaration; }
	}

	public final DeclarationContext declaration() throws RecognitionException {
		DeclarationContext _localctx = new DeclarationContext(_ctx, getState());
		enterRule(_localctx, 4, RULE_declaration);
		int _la;
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(34);
			match(T__0);
			setState(35);
			declaAffect();
			setState(40);
			_errHandler.sync(this);
			_la = _input.LA(1);
			while (_la==T__7) {
				{
				{
				setState(36);
				match(T__7);
				setState(37);
				declaAffect();
				}
				}
				setState(42);
				_errHandler.sync(this);
				_la = _input.LA(1);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class DeclaAffectContext extends ParserRuleContext {
		public TerminalNode VARIABLE() { return getToken(ifccParser.VARIABLE, 0); }
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public DeclaAffectContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_declaAffect; }
	}

	public final DeclaAffectContext declaAffect() throws RecognitionException {
		DeclaAffectContext _localctx = new DeclaAffectContext(_ctx, getState());
		enterRule(_localctx, 6, RULE_declaAffect);
		try {
			setState(47);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,3,_ctx) ) {
			case 1:
				enterOuterAlt(_localctx, 1);
				{
				setState(43);
				match(VARIABLE);
				setState(44);
				match(T__8);
				setState(45);
				expression(0);
				}
				break;
			case 2:
				enterOuterAlt(_localctx, 2);
				{
				setState(46);
				match(VARIABLE);
				}
				break;
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class Return_stmtContext extends ParserRuleContext {
		public TerminalNode RETURN() { return getToken(ifccParser.RETURN, 0); }
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public Return_stmtContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_return_stmt; }
	}

	public final Return_stmtContext return_stmt() throws RecognitionException {
		Return_stmtContext _localctx = new Return_stmtContext(_ctx, getState());
		enterRule(_localctx, 8, RULE_return_stmt);
		try {
			enterOuterAlt(_localctx, 1);
			{
			setState(49);
			match(RETURN);
			setState(50);
			expression(0);
			setState(51);
			match(T__5);
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			exitRule();
		}
		return _localctx;
	}

	@SuppressWarnings("CheckReturnValue")
	public static class ExpressionContext extends ParserRuleContext {
		public ExpressionContext(ParserRuleContext parent, int invokingState) {
			super(parent, invokingState);
		}
		@Override public int getRuleIndex() { return RULE_expression; }
	 
		public ExpressionContext() { }
		public void copyFrom(ExpressionContext ctx) {
			super.copyFrom(ctx);
		}
	}
	@SuppressWarnings("CheckReturnValue")
	public static class AddContext extends ExpressionContext {
		public Token op;
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public AddContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	@SuppressWarnings("CheckReturnValue")
	public static class ParenContext extends ExpressionContext {
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public ParenContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	@SuppressWarnings("CheckReturnValue")
	public static class CompareContext extends ExpressionContext {
		public Token op;
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public CompareContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	@SuppressWarnings("CheckReturnValue")
	public static class MultContext extends ExpressionContext {
		public Token op;
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public MultContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	@SuppressWarnings("CheckReturnValue")
	public static class AffectContext extends ExpressionContext {
		public TerminalNode VARIABLE() { return getToken(ifccParser.VARIABLE, 0); }
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public AffectContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	@SuppressWarnings("CheckReturnValue")
	public static class CharConstContext extends ExpressionContext {
		public TerminalNode CHAR_CONST() { return getToken(ifccParser.CHAR_CONST, 0); }
		public CharConstContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	@SuppressWarnings("CheckReturnValue")
	public static class ShiftContext extends ExpressionContext {
		public Token op;
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public ShiftContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	@SuppressWarnings("CheckReturnValue")
	public static class VariableContext extends ExpressionContext {
		public TerminalNode VARIABLE() { return getToken(ifccParser.VARIABLE, 0); }
		public VariableContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	@SuppressWarnings("CheckReturnValue")
	public static class UnaryAddContext extends ExpressionContext {
		public Token op;
		public ExpressionContext expression() {
			return getRuleContext(ExpressionContext.class,0);
		}
		public UnaryAddContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	@SuppressWarnings("CheckReturnValue")
	public static class CompareEqContext extends ExpressionContext {
		public Token op;
		public List<ExpressionContext> expression() {
			return getRuleContexts(ExpressionContext.class);
		}
		public ExpressionContext expression(int i) {
			return getRuleContext(ExpressionContext.class,i);
		}
		public CompareEqContext(ExpressionContext ctx) { copyFrom(ctx); }
	}
	@SuppressWarnings("CheckReturnValue")
	public static class IntConstContext extends ExpressionContext {
		public TerminalNode INT_CONST() { return getToken(ifccParser.INT_CONST, 0); }
		public IntConstContext(ExpressionContext ctx) { copyFrom(ctx); }
	}

	public final ExpressionContext expression() throws RecognitionException {
		return expression(0);
	}

	private ExpressionContext expression(int _p) throws RecognitionException {
		ParserRuleContext _parentctx = _ctx;
		int _parentState = getState();
		ExpressionContext _localctx = new ExpressionContext(_ctx, _parentState);
		ExpressionContext _prevctx = _localctx;
		int _startState = 10;
		enterRecursionRule(_localctx, 10, RULE_expression, _p);
		int _la;
		try {
			int _alt;
			enterOuterAlt(_localctx, 1);
			{
			setState(66);
			_errHandler.sync(this);
			switch ( getInterpreter().adaptivePredict(_input,4,_ctx) ) {
			case 1:
				{
				_localctx = new IntConstContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;

				setState(54);
				match(INT_CONST);
				}
				break;
			case 2:
				{
				_localctx = new CharConstContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(55);
				match(CHAR_CONST);
				}
				break;
			case 3:
				{
				_localctx = new VariableContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(56);
				match(VARIABLE);
				}
				break;
			case 4:
				{
				_localctx = new ParenContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(57);
				match(T__2);
				setState(58);
				expression(0);
				setState(59);
				match(T__3);
				}
				break;
			case 5:
				{
				_localctx = new UnaryAddContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(61);
				((UnaryAddContext)_localctx).op = _input.LT(1);
				_la = _input.LA(1);
				if ( !(_la==T__9 || _la==T__10) ) {
					((UnaryAddContext)_localctx).op = (Token)_errHandler.recoverInline(this);
				}
				else {
					if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
					_errHandler.reportMatch(this);
					consume();
				}
				setState(62);
				expression(7);
				}
				break;
			case 6:
				{
				_localctx = new AffectContext(_localctx);
				_ctx = _localctx;
				_prevctx = _localctx;
				setState(63);
				match(VARIABLE);
				setState(64);
				match(T__8);
				setState(65);
				expression(1);
				}
				break;
			}
			_ctx.stop = _input.LT(-1);
			setState(85);
			_errHandler.sync(this);
			_alt = getInterpreter().adaptivePredict(_input,6,_ctx);
			while ( _alt!=2 && _alt!=org.antlr.v4.runtime.atn.ATN.INVALID_ALT_NUMBER ) {
				if ( _alt==1 ) {
					if ( _parseListeners!=null ) triggerExitRuleEvent();
					_prevctx = _localctx;
					{
					setState(83);
					_errHandler.sync(this);
					switch ( getInterpreter().adaptivePredict(_input,5,_ctx) ) {
					case 1:
						{
						_localctx = new MultContext(new ExpressionContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expression);
						setState(68);
						if (!(precpred(_ctx, 6))) throw new FailedPredicateException(this, "precpred(_ctx, 6)");
						setState(69);
						((MultContext)_localctx).op = _input.LT(1);
						_la = _input.LA(1);
						if ( !((((_la) & ~0x3f) == 0 && ((1L << _la) & 28672L) != 0)) ) {
							((MultContext)_localctx).op = (Token)_errHandler.recoverInline(this);
						}
						else {
							if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
							_errHandler.reportMatch(this);
							consume();
						}
						setState(70);
						expression(7);
						}
						break;
					case 2:
						{
						_localctx = new AddContext(new ExpressionContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expression);
						setState(71);
						if (!(precpred(_ctx, 5))) throw new FailedPredicateException(this, "precpred(_ctx, 5)");
						setState(72);
						((AddContext)_localctx).op = _input.LT(1);
						_la = _input.LA(1);
						if ( !(_la==T__9 || _la==T__10) ) {
							((AddContext)_localctx).op = (Token)_errHandler.recoverInline(this);
						}
						else {
							if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
							_errHandler.reportMatch(this);
							consume();
						}
						setState(73);
						expression(6);
						}
						break;
					case 3:
						{
						_localctx = new ShiftContext(new ExpressionContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expression);
						setState(74);
						if (!(precpred(_ctx, 4))) throw new FailedPredicateException(this, "precpred(_ctx, 4)");
						setState(75);
						((ShiftContext)_localctx).op = _input.LT(1);
						_la = _input.LA(1);
						if ( !(_la==T__14 || _la==T__15) ) {
							((ShiftContext)_localctx).op = (Token)_errHandler.recoverInline(this);
						}
						else {
							if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
							_errHandler.reportMatch(this);
							consume();
						}
						setState(76);
						expression(5);
						}
						break;
					case 4:
						{
						_localctx = new CompareContext(new ExpressionContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expression);
						setState(77);
						if (!(precpred(_ctx, 3))) throw new FailedPredicateException(this, "precpred(_ctx, 3)");
						setState(78);
						((CompareContext)_localctx).op = _input.LT(1);
						_la = _input.LA(1);
						if ( !((((_la) & ~0x3f) == 0 && ((1L << _la) & 1966080L) != 0)) ) {
							((CompareContext)_localctx).op = (Token)_errHandler.recoverInline(this);
						}
						else {
							if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
							_errHandler.reportMatch(this);
							consume();
						}
						setState(79);
						expression(4);
						}
						break;
					case 5:
						{
						_localctx = new CompareEqContext(new ExpressionContext(_parentctx, _parentState));
						pushNewRecursionContext(_localctx, _startState, RULE_expression);
						setState(80);
						if (!(precpred(_ctx, 2))) throw new FailedPredicateException(this, "precpred(_ctx, 2)");
						setState(81);
						((CompareEqContext)_localctx).op = _input.LT(1);
						_la = _input.LA(1);
						if ( !(_la==T__20 || _la==T__21) ) {
							((CompareEqContext)_localctx).op = (Token)_errHandler.recoverInline(this);
						}
						else {
							if ( _input.LA(1)==Token.EOF ) matchedEOF = true;
							_errHandler.reportMatch(this);
							consume();
						}
						setState(82);
						expression(3);
						}
						break;
					}
					} 
				}
				setState(87);
				_errHandler.sync(this);
				_alt = getInterpreter().adaptivePredict(_input,6,_ctx);
			}
			}
		}
		catch (RecognitionException re) {
			_localctx.exception = re;
			_errHandler.reportError(this, re);
			_errHandler.recover(this, re);
		}
		finally {
			unrollRecursionContexts(_parentctx);
		}
		return _localctx;
	}

	public boolean sempred(RuleContext _localctx, int ruleIndex, int predIndex) {
		switch (ruleIndex) {
		case 5:
			return expression_sempred((ExpressionContext)_localctx, predIndex);
		}
		return true;
	}
	private boolean expression_sempred(ExpressionContext _localctx, int predIndex) {
		switch (predIndex) {
		case 0:
			return precpred(_ctx, 6);
		case 1:
			return precpred(_ctx, 5);
		case 2:
			return precpred(_ctx, 4);
		case 3:
			return precpred(_ctx, 3);
		case 4:
			return precpred(_ctx, 2);
		}
		return true;
	}

	public static final String _serializedATN =
		"\u0004\u0001\u001dY\u0002\u0000\u0007\u0000\u0002\u0001\u0007\u0001\u0002"+
		"\u0002\u0007\u0002\u0002\u0003\u0007\u0003\u0002\u0004\u0007\u0004\u0002"+
		"\u0005\u0007\u0005\u0001\u0000\u0001\u0000\u0001\u0000\u0001\u0001\u0001"+
		"\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001"+
		"\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0005\u0001\u001b\b\u0001\n"+
		"\u0001\f\u0001\u001e\t\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001"+
		"\u0002\u0001\u0002\u0001\u0002\u0001\u0002\u0005\u0002\'\b\u0002\n\u0002"+
		"\f\u0002*\t\u0002\u0001\u0003\u0001\u0003\u0001\u0003\u0001\u0003\u0003"+
		"\u00030\b\u0003\u0001\u0004\u0001\u0004\u0001\u0004\u0001\u0004\u0001"+
		"\u0005\u0001\u0005\u0001\u0005\u0001\u0005\u0001\u0005\u0001\u0005\u0001"+
		"\u0005\u0001\u0005\u0001\u0005\u0001\u0005\u0001\u0005\u0001\u0005\u0001"+
		"\u0005\u0003\u0005C\b\u0005\u0001\u0005\u0001\u0005\u0001\u0005\u0001"+
		"\u0005\u0001\u0005\u0001\u0005\u0001\u0005\u0001\u0005\u0001\u0005\u0001"+
		"\u0005\u0001\u0005\u0001\u0005\u0001\u0005\u0001\u0005\u0001\u0005\u0005"+
		"\u0005T\b\u0005\n\u0005\f\u0005W\t\u0005\u0001\u0005\u0000\u0001\n\u0006"+
		"\u0000\u0002\u0004\u0006\b\n\u0000\u0005\u0001\u0000\n\u000b\u0001\u0000"+
		"\f\u000e\u0001\u0000\u000f\u0010\u0001\u0000\u0011\u0014\u0001\u0000\u0015"+
		"\u0016`\u0000\f\u0001\u0000\u0000\u0000\u0002\u000f\u0001\u0000\u0000"+
		"\u0000\u0004\"\u0001\u0000\u0000\u0000\u0006/\u0001\u0000\u0000\u0000"+
		"\b1\u0001\u0000\u0000\u0000\nB\u0001\u0000\u0000\u0000\f\r\u0003\u0002"+
		"\u0001\u0000\r\u000e\u0005\u0000\u0000\u0001\u000e\u0001\u0001\u0000\u0000"+
		"\u0000\u000f\u0010\u0005\u0001\u0000\u0000\u0010\u0011\u0005\u0002\u0000"+
		"\u0000\u0011\u0012\u0005\u0003\u0000\u0000\u0012\u0013\u0005\u0004\u0000"+
		"\u0000\u0013\u001c\u0005\u0005\u0000\u0000\u0014\u0015\u0003\n\u0005\u0000"+
		"\u0015\u0016\u0005\u0006\u0000\u0000\u0016\u001b\u0001\u0000\u0000\u0000"+
		"\u0017\u0018\u0003\u0004\u0002\u0000\u0018\u0019\u0005\u0006\u0000\u0000"+
		"\u0019\u001b\u0001\u0000\u0000\u0000\u001a\u0014\u0001\u0000\u0000\u0000"+
		"\u001a\u0017\u0001\u0000\u0000\u0000\u001b\u001e\u0001\u0000\u0000\u0000"+
		"\u001c\u001a\u0001\u0000\u0000\u0000\u001c\u001d\u0001\u0000\u0000\u0000"+
		"\u001d\u001f\u0001\u0000\u0000\u0000\u001e\u001c\u0001\u0000\u0000\u0000"+
		"\u001f \u0003\b\u0004\u0000 !\u0005\u0007\u0000\u0000!\u0003\u0001\u0000"+
		"\u0000\u0000\"#\u0005\u0001\u0000\u0000#(\u0003\u0006\u0003\u0000$%\u0005"+
		"\b\u0000\u0000%\'\u0003\u0006\u0003\u0000&$\u0001\u0000\u0000\u0000\'"+
		"*\u0001\u0000\u0000\u0000(&\u0001\u0000\u0000\u0000()\u0001\u0000\u0000"+
		"\u0000)\u0005\u0001\u0000\u0000\u0000*(\u0001\u0000\u0000\u0000+,\u0005"+
		"\u001a\u0000\u0000,-\u0005\t\u0000\u0000-0\u0003\n\u0005\u0000.0\u0005"+
		"\u001a\u0000\u0000/+\u0001\u0000\u0000\u0000/.\u0001\u0000\u0000\u0000"+
		"0\u0007\u0001\u0000\u0000\u000012\u0005\u0017\u0000\u000023\u0003\n\u0005"+
		"\u000034\u0005\u0006\u0000\u00004\t\u0001\u0000\u0000\u000056\u0006\u0005"+
		"\uffff\uffff\u00006C\u0005\u0018\u0000\u00007C\u0005\u0019\u0000\u0000"+
		"8C\u0005\u001a\u0000\u00009:\u0005\u0003\u0000\u0000:;\u0003\n\u0005\u0000"+
		";<\u0005\u0004\u0000\u0000<C\u0001\u0000\u0000\u0000=>\u0007\u0000\u0000"+
		"\u0000>C\u0003\n\u0005\u0007?@\u0005\u001a\u0000\u0000@A\u0005\t\u0000"+
		"\u0000AC\u0003\n\u0005\u0001B5\u0001\u0000\u0000\u0000B7\u0001\u0000\u0000"+
		"\u0000B8\u0001\u0000\u0000\u0000B9\u0001\u0000\u0000\u0000B=\u0001\u0000"+
		"\u0000\u0000B?\u0001\u0000\u0000\u0000CU\u0001\u0000\u0000\u0000DE\n\u0006"+
		"\u0000\u0000EF\u0007\u0001\u0000\u0000FT\u0003\n\u0005\u0007GH\n\u0005"+
		"\u0000\u0000HI\u0007\u0000\u0000\u0000IT\u0003\n\u0005\u0006JK\n\u0004"+
		"\u0000\u0000KL\u0007\u0002\u0000\u0000LT\u0003\n\u0005\u0005MN\n\u0003"+
		"\u0000\u0000NO\u0007\u0003\u0000\u0000OT\u0003\n\u0005\u0004PQ\n\u0002"+
		"\u0000\u0000QR\u0007\u0004\u0000\u0000RT\u0003\n\u0005\u0003SD\u0001\u0000"+
		"\u0000\u0000SG\u0001\u0000\u0000\u0000SJ\u0001\u0000\u0000\u0000SM\u0001"+
		"\u0000\u0000\u0000SP\u0001\u0000\u0000\u0000TW\u0001\u0000\u0000\u0000"+
		"US\u0001\u0000\u0000\u0000UV\u0001\u0000\u0000\u0000V\u000b\u0001\u0000"+
		"\u0000\u0000WU\u0001\u0000\u0000\u0000\u0007\u001a\u001c(/BSU";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}