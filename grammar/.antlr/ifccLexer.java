// Generated from /home/swan/Compilo/pld_comp/grammar/ifcc.g4 by ANTLR 4.13.1
import org.antlr.v4.runtime.Lexer;
import org.antlr.v4.runtime.CharStream;
import org.antlr.v4.runtime.Token;
import org.antlr.v4.runtime.TokenStream;
import org.antlr.v4.runtime.*;
import org.antlr.v4.runtime.atn.*;
import org.antlr.v4.runtime.dfa.DFA;
import org.antlr.v4.runtime.misc.*;

@SuppressWarnings({"all", "warnings", "unchecked", "unused", "cast", "CheckReturnValue", "this-escape"})
public class ifccLexer extends Lexer {
	static { RuntimeMetaData.checkVersion("4.13.1", RuntimeMetaData.VERSION); }

	protected static final DFA[] _decisionToDFA;
	protected static final PredictionContextCache _sharedContextCache =
		new PredictionContextCache();
	public static final int
		T__0=1, T__1=2, T__2=3, T__3=4, T__4=5, T__5=6, T__6=7, T__7=8, T__8=9, 
		T__9=10, T__10=11, T__11=12, T__12=13, T__13=14, T__14=15, IF=16, ELSE=17, 
		DO=18, WHILE=19, RETURN=20, INT_CONST=21, CHAR_CONST=22, VARIABLE=23, 
		COMMENT=24, DIRECTIVE=25, WS=26;
	public static String[] channelNames = {
		"DEFAULT_TOKEN_CHANNEL", "HIDDEN"
	};

	public static String[] modeNames = {
		"DEFAULT_MODE"
	};

	private static String[] makeRuleNames() {
		return new String[] {
			"T__0", "T__1", "T__2", "T__3", "T__4", "T__5", "T__6", "T__7", "T__8", 
			"T__9", "T__10", "T__11", "T__12", "T__13", "T__14", "IF", "ELSE", "DO", 
			"WHILE", "RETURN", "INT_CONST", "CHAR_CONST", "VARIABLE", "COMMENT", 
			"DIRECTIVE", "WS"
		};
	}
	public static final String[] ruleNames = makeRuleNames();

	private static String[] makeLiteralNames() {
		return new String[] {
			null, "'int'", "'main'", "'('", "')'", "'{'", "'}'", "';'", "','", "'='", 
			"'-'", "'+'", "'!'", "'*'", "'/'", "'%'", "'if'", "'else'", "'do'", "'while'", 
			"'return'"
		};
	}
	private static final String[] _LITERAL_NAMES = makeLiteralNames();
	private static String[] makeSymbolicNames() {
		return new String[] {
			null, null, null, null, null, null, null, null, null, null, null, null, 
			null, null, null, null, "IF", "ELSE", "DO", "WHILE", "RETURN", "INT_CONST", 
			"CHAR_CONST", "VARIABLE", "COMMENT", "DIRECTIVE", "WS"
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


	public ifccLexer(CharStream input) {
		super(input);
		_interp = new LexerATNSimulator(this,_ATN,_decisionToDFA,_sharedContextCache);
	}

	@Override
	public String getGrammarFileName() { return "ifcc.g4"; }

	@Override
	public String[] getRuleNames() { return ruleNames; }

	@Override
	public String getSerializedATN() { return _serializedATN; }

	@Override
	public String[] getChannelNames() { return channelNames; }

	@Override
	public String[] getModeNames() { return modeNames; }

	@Override
	public ATN getATN() { return _ATN; }

	public static final String _serializedATN =
		"\u0004\u0000\u001a\u009d\u0006\uffff\uffff\u0002\u0000\u0007\u0000\u0002"+
		"\u0001\u0007\u0001\u0002\u0002\u0007\u0002\u0002\u0003\u0007\u0003\u0002"+
		"\u0004\u0007\u0004\u0002\u0005\u0007\u0005\u0002\u0006\u0007\u0006\u0002"+
		"\u0007\u0007\u0007\u0002\b\u0007\b\u0002\t\u0007\t\u0002\n\u0007\n\u0002"+
		"\u000b\u0007\u000b\u0002\f\u0007\f\u0002\r\u0007\r\u0002\u000e\u0007\u000e"+
		"\u0002\u000f\u0007\u000f\u0002\u0010\u0007\u0010\u0002\u0011\u0007\u0011"+
		"\u0002\u0012\u0007\u0012\u0002\u0013\u0007\u0013\u0002\u0014\u0007\u0014"+
		"\u0002\u0015\u0007\u0015\u0002\u0016\u0007\u0016\u0002\u0017\u0007\u0017"+
		"\u0002\u0018\u0007\u0018\u0002\u0019\u0007\u0019\u0001\u0000\u0001\u0000"+
		"\u0001\u0000\u0001\u0000\u0001\u0001\u0001\u0001\u0001\u0001\u0001\u0001"+
		"\u0001\u0001\u0001\u0002\u0001\u0002\u0001\u0003\u0001\u0003\u0001\u0004"+
		"\u0001\u0004\u0001\u0005\u0001\u0005\u0001\u0006\u0001\u0006\u0001\u0007"+
		"\u0001\u0007\u0001\b\u0001\b\u0001\t\u0001\t\u0001\n\u0001\n\u0001\u000b"+
		"\u0001\u000b\u0001\f\u0001\f\u0001\r\u0001\r\u0001\u000e\u0001\u000e\u0001"+
		"\u000f\u0001\u000f\u0001\u000f\u0001\u0010\u0001\u0010\u0001\u0010\u0001"+
		"\u0010\u0001\u0010\u0001\u0011\u0001\u0011\u0001\u0011\u0001\u0012\u0001"+
		"\u0012\u0001\u0012\u0001\u0012\u0001\u0012\u0001\u0012\u0001\u0013\u0001"+
		"\u0013\u0001\u0013\u0001\u0013\u0001\u0013\u0001\u0013\u0001\u0013\u0001"+
		"\u0014\u0004\u0014r\b\u0014\u000b\u0014\f\u0014s\u0001\u0015\u0001\u0015"+
		"\u0001\u0015\u0001\u0015\u0001\u0016\u0001\u0016\u0005\u0016|\b\u0016"+
		"\n\u0016\f\u0016\u007f\t\u0016\u0001\u0017\u0001\u0017\u0001\u0017\u0001"+
		"\u0017\u0005\u0017\u0085\b\u0017\n\u0017\f\u0017\u0088\t\u0017\u0001\u0017"+
		"\u0001\u0017\u0001\u0017\u0001\u0017\u0001\u0017\u0001\u0018\u0001\u0018"+
		"\u0005\u0018\u0091\b\u0018\n\u0018\f\u0018\u0094\t\u0018\u0001\u0018\u0001"+
		"\u0018\u0001\u0018\u0001\u0018\u0001\u0019\u0001\u0019\u0001\u0019\u0001"+
		"\u0019\u0002\u0086\u0092\u0000\u001a\u0001\u0001\u0003\u0002\u0005\u0003"+
		"\u0007\u0004\t\u0005\u000b\u0006\r\u0007\u000f\b\u0011\t\u0013\n\u0015"+
		"\u000b\u0017\f\u0019\r\u001b\u000e\u001d\u000f\u001f\u0010!\u0011#\u0012"+
		"%\u0013\'\u0014)\u0015+\u0016-\u0017/\u00181\u00193\u001a\u0001\u0000"+
		"\u0004\u0001\u000009\u0003\u0000AZ__az\u0004\u000009AZ__az\u0003\u0000"+
		"\t\n\r\r  \u00a0\u0000\u0001\u0001\u0000\u0000\u0000\u0000\u0003\u0001"+
		"\u0000\u0000\u0000\u0000\u0005\u0001\u0000\u0000\u0000\u0000\u0007\u0001"+
		"\u0000\u0000\u0000\u0000\t\u0001\u0000\u0000\u0000\u0000\u000b\u0001\u0000"+
		"\u0000\u0000\u0000\r\u0001\u0000\u0000\u0000\u0000\u000f\u0001\u0000\u0000"+
		"\u0000\u0000\u0011\u0001\u0000\u0000\u0000\u0000\u0013\u0001\u0000\u0000"+
		"\u0000\u0000\u0015\u0001\u0000\u0000\u0000\u0000\u0017\u0001\u0000\u0000"+
		"\u0000\u0000\u0019\u0001\u0000\u0000\u0000\u0000\u001b\u0001\u0000\u0000"+
		"\u0000\u0000\u001d\u0001\u0000\u0000\u0000\u0000\u001f\u0001\u0000\u0000"+
		"\u0000\u0000!\u0001\u0000\u0000\u0000\u0000#\u0001\u0000\u0000\u0000\u0000"+
		"%\u0001\u0000\u0000\u0000\u0000\'\u0001\u0000\u0000\u0000\u0000)\u0001"+
		"\u0000\u0000\u0000\u0000+\u0001\u0000\u0000\u0000\u0000-\u0001\u0000\u0000"+
		"\u0000\u0000/\u0001\u0000\u0000\u0000\u00001\u0001\u0000\u0000\u0000\u0000"+
		"3\u0001\u0000\u0000\u0000\u00015\u0001\u0000\u0000\u0000\u00039\u0001"+
		"\u0000\u0000\u0000\u0005>\u0001\u0000\u0000\u0000\u0007@\u0001\u0000\u0000"+
		"\u0000\tB\u0001\u0000\u0000\u0000\u000bD\u0001\u0000\u0000\u0000\rF\u0001"+
		"\u0000\u0000\u0000\u000fH\u0001\u0000\u0000\u0000\u0011J\u0001\u0000\u0000"+
		"\u0000\u0013L\u0001\u0000\u0000\u0000\u0015N\u0001\u0000\u0000\u0000\u0017"+
		"P\u0001\u0000\u0000\u0000\u0019R\u0001\u0000\u0000\u0000\u001bT\u0001"+
		"\u0000\u0000\u0000\u001dV\u0001\u0000\u0000\u0000\u001fX\u0001\u0000\u0000"+
		"\u0000![\u0001\u0000\u0000\u0000#`\u0001\u0000\u0000\u0000%c\u0001\u0000"+
		"\u0000\u0000\'i\u0001\u0000\u0000\u0000)q\u0001\u0000\u0000\u0000+u\u0001"+
		"\u0000\u0000\u0000-y\u0001\u0000\u0000\u0000/\u0080\u0001\u0000\u0000"+
		"\u00001\u008e\u0001\u0000\u0000\u00003\u0099\u0001\u0000\u0000\u00005"+
		"6\u0005i\u0000\u000067\u0005n\u0000\u000078\u0005t\u0000\u00008\u0002"+
		"\u0001\u0000\u0000\u00009:\u0005m\u0000\u0000:;\u0005a\u0000\u0000;<\u0005"+
		"i\u0000\u0000<=\u0005n\u0000\u0000=\u0004\u0001\u0000\u0000\u0000>?\u0005"+
		"(\u0000\u0000?\u0006\u0001\u0000\u0000\u0000@A\u0005)\u0000\u0000A\b\u0001"+
		"\u0000\u0000\u0000BC\u0005{\u0000\u0000C\n\u0001\u0000\u0000\u0000DE\u0005"+
		"}\u0000\u0000E\f\u0001\u0000\u0000\u0000FG\u0005;\u0000\u0000G\u000e\u0001"+
		"\u0000\u0000\u0000HI\u0005,\u0000\u0000I\u0010\u0001\u0000\u0000\u0000"+
		"JK\u0005=\u0000\u0000K\u0012\u0001\u0000\u0000\u0000LM\u0005-\u0000\u0000"+
		"M\u0014\u0001\u0000\u0000\u0000NO\u0005+\u0000\u0000O\u0016\u0001\u0000"+
		"\u0000\u0000PQ\u0005!\u0000\u0000Q\u0018\u0001\u0000\u0000\u0000RS\u0005"+
		"*\u0000\u0000S\u001a\u0001\u0000\u0000\u0000TU\u0005/\u0000\u0000U\u001c"+
		"\u0001\u0000\u0000\u0000VW\u0005%\u0000\u0000W\u001e\u0001\u0000\u0000"+
		"\u0000XY\u0005i\u0000\u0000YZ\u0005f\u0000\u0000Z \u0001\u0000\u0000\u0000"+
		"[\\\u0005e\u0000\u0000\\]\u0005l\u0000\u0000]^\u0005s\u0000\u0000^_\u0005"+
		"e\u0000\u0000_\"\u0001\u0000\u0000\u0000`a\u0005d\u0000\u0000ab\u0005"+
		"o\u0000\u0000b$\u0001\u0000\u0000\u0000cd\u0005w\u0000\u0000de\u0005h"+
		"\u0000\u0000ef\u0005i\u0000\u0000fg\u0005l\u0000\u0000gh\u0005e\u0000"+
		"\u0000h&\u0001\u0000\u0000\u0000ij\u0005r\u0000\u0000jk\u0005e\u0000\u0000"+
		"kl\u0005t\u0000\u0000lm\u0005u\u0000\u0000mn\u0005r\u0000\u0000no\u0005"+
		"n\u0000\u0000o(\u0001\u0000\u0000\u0000pr\u0007\u0000\u0000\u0000qp\u0001"+
		"\u0000\u0000\u0000rs\u0001\u0000\u0000\u0000sq\u0001\u0000\u0000\u0000"+
		"st\u0001\u0000\u0000\u0000t*\u0001\u0000\u0000\u0000uv\u0005\'\u0000\u0000"+
		"vw\t\u0000\u0000\u0000wx\u0005\'\u0000\u0000x,\u0001\u0000\u0000\u0000"+
		"y}\u0007\u0001\u0000\u0000z|\u0007\u0002\u0000\u0000{z\u0001\u0000\u0000"+
		"\u0000|\u007f\u0001\u0000\u0000\u0000}{\u0001\u0000\u0000\u0000}~\u0001"+
		"\u0000\u0000\u0000~.\u0001\u0000\u0000\u0000\u007f}\u0001\u0000\u0000"+
		"\u0000\u0080\u0081\u0005/\u0000\u0000\u0081\u0082\u0005*\u0000\u0000\u0082"+
		"\u0086\u0001\u0000\u0000\u0000\u0083\u0085\t\u0000\u0000\u0000\u0084\u0083"+
		"\u0001\u0000\u0000\u0000\u0085\u0088\u0001\u0000\u0000\u0000\u0086\u0087"+
		"\u0001\u0000\u0000\u0000\u0086\u0084\u0001\u0000\u0000\u0000\u0087\u0089"+
		"\u0001\u0000\u0000\u0000\u0088\u0086\u0001\u0000\u0000\u0000\u0089\u008a"+
		"\u0005*\u0000\u0000\u008a\u008b\u0005/\u0000\u0000\u008b\u008c\u0001\u0000"+
		"\u0000\u0000\u008c\u008d\u0006\u0017\u0000\u0000\u008d0\u0001\u0000\u0000"+
		"\u0000\u008e\u0092\u0005#\u0000\u0000\u008f\u0091\t\u0000\u0000\u0000"+
		"\u0090\u008f\u0001\u0000\u0000\u0000\u0091\u0094\u0001\u0000\u0000\u0000"+
		"\u0092\u0093\u0001\u0000\u0000\u0000\u0092\u0090\u0001\u0000\u0000\u0000"+
		"\u0093\u0095\u0001\u0000\u0000\u0000\u0094\u0092\u0001\u0000\u0000\u0000"+
		"\u0095\u0096\u0005\n\u0000\u0000\u0096\u0097\u0001\u0000\u0000\u0000\u0097"+
		"\u0098\u0006\u0018\u0000\u0000\u00982\u0001\u0000\u0000\u0000\u0099\u009a"+
		"\u0007\u0003\u0000\u0000\u009a\u009b\u0001\u0000\u0000\u0000\u009b\u009c"+
		"\u0006\u0019\u0001\u0000\u009c4\u0001\u0000\u0000\u0000\u0005\u0000s}"+
		"\u0086\u0092\u0002\u0006\u0000\u0000\u0000\u0001\u0000";
	public static final ATN _ATN =
		new ATNDeserializer().deserialize(_serializedATN.toCharArray());
	static {
		_decisionToDFA = new DFA[_ATN.getNumberOfDecisions()];
		for (int i = 0; i < _ATN.getNumberOfDecisions(); i++) {
			_decisionToDFA[i] = new DFA(_ATN.getDecisionState(i), i);
		}
	}
}