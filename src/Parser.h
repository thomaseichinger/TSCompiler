

#if !defined(COCO_PARSER_H__)
#define COCO_PARSER_H__

#include "wchar.h"


#include "Scanner.h"
#include "tsdata.h"
#include <QString>



class TSCommunicator;

class Errors {
public:
	int count;			// number of errors detected

	Errors(TSCommunicator* com);
	void SynErr(int line, int col, int n);
	void Error( QString er );
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
		_intent=4,
		_string=5
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
	void String(QString *current);
	void VariableDecl(QString name);
	void VariableConcenate(QString name);
	void ParameterDecl(TSFunction* f);
	void FunctionBody(TSFunction* f);
	void FunctionDecl(QString name);
	void ObjectMemDecl(TSObject* o);
	void ObjectDecl(QString name);
	void Object();
	void TSCompiler();

	void Parse();

}; // end Parser



#endif // !defined(COCO_PARSER_H__)

