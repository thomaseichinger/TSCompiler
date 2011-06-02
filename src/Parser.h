

#if !defined(COCO_PARSER_H__)
#define COCO_PARSER_H__



#include "Scanner.h"



class Errors {
public:
	int count;			// number of errors detected

	Errors();
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_ident=1,
		_number=2,
		_string=3,
		_badString=4,
		_char=5,
		_ddtSym=42,
		_optionSym=43
	};
	int maxT;

	Token *dummyToken;
	int errDist;
	int minErrDist;

	void SynErr(int n);
	void Get();
	void Expect(int n);
	bool StartOf(int s);
	void ExpectWeak(int n, int follow);
	bool WeakSeparator(int n, int syFol, int repFol);

public:
	Scanner *scanner;
	Errors  *errors;

	Token *t;			// last recognized token
	Token *la;			// lookahead token

int id;
	int str;

	FILE* trace;		// other Coco objects referenced in this ATG
	Tab *tab;
	DFA *dfa;
	ParserGen *pgen;

	bool genScanner;
	wchar_t* tokenString;  // used in declarations of literal tokens
	wchar_t* noString;     // used in declarations of literal tokens

	void InitDeclarations() {
		id  = 0;
		str = 1;
		tokenString = NULL;
		noString = coco_string_create(L"-none-");
	}

/*-------------------------------------------------------------------------*/



	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void Coco();
	void SetDecl();
	void TokenDecl(int typ);
	void TokenExpr(Graph* &g);
	void Set(CharSet* &s);
	void AttrDecl(Symbol *sym);
	void SemText(Position* &pos);
	void Expression(Graph* &g);
	void SimSet(CharSet* &s);
	void Char(int &n);
	void Sym(wchar_t* &name, int &kind);
	void Term(Graph* &g);
	void Resolver(Position* &pos);
	void Factor(Graph* &g);
	void Attribs(Node *p);
	void Condition();
	void TokenTerm(Graph* &g);
	void TokenFactor(Graph* &g);

	void Parse();

}; // end Parser



#endif // !defined(COCO_PARSER_H__)

