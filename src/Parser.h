

#if !defined(COCO_PARSER_H__)
#define COCO_PARSER_H__

#include "wchar.h"


#include "Scanner.h"
#include "tsdata.h"



class TSCommunicator;

class Errors {
public:
	int count;			// number of errors detected

	Errors(TSCommunicator* com);
	void SynErr(int line, int col, int n);
	void Error(int line, int col, const wchar_t *s);
	void Warning(int line, int col, const wchar_t *s);
	void Warning(const wchar_t *s);
	void Exception(const wchar_t *s);
	
	TSCommunicator* m_com;

}; // Errors

class Parser {
private:
	enum {
		_EOF=0,
		_ident=1,
		_number=2,
		_hexnumber=3,
		_intent=4
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
	TSData *data;

	Token *t;			// last recognized token
	Token *la;			// lookahead token



	Parser(Scanner *scanner, TSData *d, TSCommunicator* com);
	~Parser();
	void SemErr(const wchar_t* msg);

	void VersionNum();
	void String();
	void VariableDecl();
	void ParameterDecl();
	void FunctionBody();
	void FunctionDecl();
	void ObjectMemDecl();
	void ObjectDecl();
	void Object();
	void TSCompiler();

	void Parse();

}; // end Parser



#endif // !defined(COCO_PARSER_H__)

