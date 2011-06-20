

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"
#include <iostream>
#include <tscommunicator.h>




void Parser::SynErr(int n) {
	if (errDist >= minErrDist) errors->SynErr(la->line, la->col, n);
	errDist = 0;
}

void Parser::SemErr(const wchar_t* msg) {
	if (errDist >= minErrDist) errors->Error(t->line, t->col, msg);
	errDist = 0;
}

void Parser::Get() {
	for (;;) {
		t = la;
		la = scanner->Scan();
		if (la->kind <= maxT) { ++errDist; break; }

		if (dummyToken != t) {
			dummyToken->kind = t->kind;
			dummyToken->pos = t->pos;
			dummyToken->col = t->col;
			dummyToken->line = t->line;
			dummyToken->next = NULL;
			coco_string_delete(dummyToken->val);
			dummyToken->val = coco_string_create(t->val);
			t = dummyToken;
		}
		la = t;
	}
}

void Parser::Expect(int n) {
	if (la->kind==n) Get(); else { SynErr(n); }
}

void Parser::ExpectWeak(int n, int follow) {
	if (la->kind == n) Get();
	else {
		SynErr(n);
		while (!StartOf(follow)) Get();
	}
}

bool Parser::WeakSeparator(int n, int syFol, int repFol) {
	if (la->kind == n) {Get(); return true;}
	else if (StartOf(repFol)) {return false;}
	else {
		SynErr(n);
		while (!(StartOf(syFol) || StartOf(repFol) || StartOf(0))) {
			Get();
		}
		return StartOf(syFol);
	}
}

void Parser::VersionNum() {
		Expect(2);
		QString versNum( coco_qstring_create(t->val) ); 
		while (la->kind == 6) {
			Get();
			Expect(2);
			versNum.append( coco_qstring_create(t->val) ); 
		}
		if (versNum != data->version() )
		{
			errors->Error( QString("Using false TSCompiler version") );
			//data->quit();
		}
		
}

void Parser::String(QString *current) {
		while (la->kind == 5) {
			Get();
			current->append(coco_qstring_create(t->val)); 
		}
		current->remove("\""); 
}

void Parser::VariableDecl(QString name) {
		QString* current = new QString(); 
		if (la->kind == 1) {
			Get();
			*current = coco_qstring_create(t->val); 
			(data->variable(*current)) ? data->addVariableWithValue( name, data->variableValue(*current) ) : errors->Error(QString("Variable %1 not specified in this scope.").arg(*current));
			
		} else if (la->kind == 0 || la->kind == 1 || la->kind == 5) {
			String(current);
			data->addVariableWithValue( name, *current ); 
		} else if (la->kind == 2) {
			Get();
			*current = coco_qstring_create(t->val);data->addVariableWithValue( name, *current ); 
		} else if (la->kind == 3) {
			Get();
			*current = coco_qstring_create(t->val);data->addVariableWithValue( name, *current ); 
		} else SynErr(18);
		(current->isEmpty())?errors->m_com->out(QString("no val for %1").arg(name)):errors->m_com->out( name + "=" + *current ); 
		delete current; 
}

void Parser::VariableConcenate(QString name) {
		QString* current = new QString();errors->m_com->out(QString("in concenate")); 
		if (la->kind == 1) {
			Get();
			*current = coco_qstring_create(t->val);
			(data->variable(*current)) ? data->addVariableWithValue( name, data->variableValue(name) + data->variableValue(*current) ) : errors->Error(QString("Variable %1 not specified in this scope.").arg(*current));
			
		} else if (la->kind == 0 || la->kind == 1 || la->kind == 5) {
			String(current);
			data->addVariableWithValue( name, data->variableValue(name) + *current ); 
		} else if (la->kind == 2) {
			Get();
			*current = coco_qstring_create(t->val);data->addVariableWithValue( name, data->variableValue(name) + *current ); 
		} else if (la->kind == 3) {
			Get();
			*current = coco_qstring_create(t->val);data->addVariableWithValue( name, data->variableValue(name) + *current ); 
		} else SynErr(19);
		(current->isEmpty())?errors->m_com->out(QString("no val for %1").arg(name)):errors->m_com->out( name + "=" + data->variableValue(name) );
		delete current; 
}

void Parser::ParameterDecl(TSFunction* f) {
		QStringList l; 
		Expect(7);
		if (la->kind == 1 || la->kind == 10) {
			while (la->kind == 1) {
				Get();
				l << coco_qstring_create(t->val); 
				if (la->kind == 8) {
					Get();
				} else if (la->kind == 9) {
					Get();
				} else SynErr(20);
			}
		} else if (la->kind == 9) {
			Get();
		} else SynErr(21);
		f->setParameters( l ); 
}

void Parser::FunctionBody(TSFunction* f) {
		QStringList l; 
		Expect(1);
		l << coco_qstring_create(t->val);errors->m_com->out( QString("body") ); 
		while (la->kind == 4) {
			Get();
			errors->m_com->out( QString("body tab") ); 
			Expect(1);
			l << coco_qstring_create(t->val);errors->m_com->out( QString("body ").append( coco_qstring_create(t->val) )); 
		}
		f->setBody( l );errors->m_com->out( QString("end of body") ); 
}

void Parser::FunctionDecl(QString name) {
		TSFunction *f = new TSFunction(); f->setName( name ); 
		ParameterDecl(f);
		Expect(10);
		FunctionBody(f);
		errors->m_com->out( QString( "end of function") );
		data->addFunction( f ); 
}

void Parser::ObjectMemDecl(TSObject* o) {
		QString* current = new QString(); 
		Expect(1);
		QString name = coco_qstring_create(t->val);
		errors->m_com->out( QString( "it's an ts object member") ); 
		Expect(11);
		if (la->kind == 1) {
			Get();
			*current = coco_qstring_create(t->val); 
		} else if (StartOf(1)) {
			String(current);
		} else SynErr(22);
		o->addMemberWithValue( name, *current ); delete current; 
}

void Parser::ObjectDecl(QString name) {
		TSObject* o = new TSObject(name); 
		Expect(12);
		errors->m_com->out( QString( "it's an ts object") ); 
		while (la->kind == 1 || la->kind == 4) {
			while (la->kind == 4) {
				Get();
			}
			ObjectMemDecl(o);
		}
		Expect(13);
		data->addObject( o );errors->m_com->out( QString( "end of ts object") ); 
}

void Parser::Object() {
		Expect(1);
		QString name = coco_qstring_create( t->val ); 
		if (la->kind == 14) {
			Get();
			if (StartOf(2)) {
				VariableDecl(name);
				errors->m_com->out( QString( "was a variable") ); 
			} else if (la->kind == 7) {
				FunctionDecl(name);
				errors->m_com->out( QString( "was a function") ); 
			} else if (la->kind == 12) {
				ObjectDecl(name);
				errors->m_com->out( QString( "was a ts object") ); 
			} else SynErr(23);
		} else if (la->kind == 15) {
			Get();
			VariableConcenate(name);
		} else SynErr(24);
}

void Parser::TSCompiler() {
		Expect(16);
		VersionNum();
		while (la->kind == 1) {
			Object();
		}
}



void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	TSCompiler();
	Expect(0);
}

Parser::Parser(Scanner *scanner, TSData* d, TSCommunicator* com) : data(d) {
	maxT = 17;

	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors(com);
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[3][19] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,x,x, T,T,x,x, x,x,x,x, x,T,x,x, x,x,x},
		{T,T,T,T, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	delete errors;
	delete dummyToken;
}

Errors::Errors(TSCommunicator* com) : m_com(com) {
	count = 0;
}

void Errors::SynErr(int line, int col, int n) {
	wchar_t* s;
	switch (n) {
			case 0: s = coco_string_create(L"EOF expected"); break;
			case 1: s = coco_string_create(L"ident expected"); break;
			case 2: s = coco_string_create(L"number expected"); break;
			case 3: s = coco_string_create(L"hexnumber expected"); break;
			case 4: s = coco_string_create(L"intent expected"); break;
			case 5: s = coco_string_create(L"string expected"); break;
			case 6: s = coco_string_create(L"\".\" expected"); break;
			case 7: s = coco_string_create(L"\"(\" expected"); break;
			case 8: s = coco_string_create(L"\",\" expected"); break;
			case 9: s = coco_string_create(L"\")\" expected"); break;
			case 10: s = coco_string_create(L"\"->\" expected"); break;
			case 11: s = coco_string_create(L"\":\" expected"); break;
			case 12: s = coco_string_create(L"\"{\" expected"); break;
			case 13: s = coco_string_create(L"\"}\" expected"); break;
			case 14: s = coco_string_create(L"\"=\" expected"); break;
			case 15: s = coco_string_create(L"\"+=\" expected"); break;
			case 16: s = coco_string_create(L"\"TrivialScript\" expected"); break;
			case 17: s = coco_string_create(L"??? expected"); break;
			case 18: s = coco_string_create(L"invalid VariableDecl"); break;
			case 19: s = coco_string_create(L"invalid VariableConcenate"); break;
			case 20: s = coco_string_create(L"invalid ParameterDecl"); break;
			case 21: s = coco_string_create(L"invalid ParameterDecl"); break;
			case 22: s = coco_string_create(L"invalid ObjectMemDecl"); break;
			case 23: s = coco_string_create(L"invalid Object"); break;
			case 24: s = coco_string_create(L"invalid Object"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	//wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	m_com->error( QString().append("line %1 col %2: %3").arg(line).arg(col).arg(QString().fromStdWString(s)));
	coco_string_delete(s);
	count++;
}

void Errors::Error( QString er )
{
	m_com->error( er );
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	//wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	m_com->error( QString().append("line %1 col %2: %3").arg(line).arg(col).arg(QString().fromStdWString(s)));
	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	//wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	m_com->error( QString().append("line %1 col %2: %3").arg(line).arg(col).arg(QString().fromStdWString(s)));
}

void Errors::Warning(const wchar_t *s) {
	//wprintf(L"%ls\n", s);
	m_com->error( QString().fromStdWString(s) );
}

void Errors::Exception(const wchar_t* s) {
	//wprintf(L"%ls", s); 
	m_com->error( QString().fromStdWString(s) );
	exit(1);
}


