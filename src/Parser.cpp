

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"
#include <iostream>




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
		while (la->kind == 5) {
			Get();
			Expect(2);
			versNum.append( coco_qstring_create(t->val) ); 
		}
		if (versNum != data->version() )
		{
			std::cout << "-- ERROR: Using false TSCompiler version" << std::endl;
			data->quit();
		}
		
}

void Parser::String() {
		Expect(6);
		while (la->kind == 1 || la->kind == 2) {
			if (la->kind == 1) {
				Get();
			} else {
				Get();
			}
		}
		Expect(6);
}

void Parser::VariableDecl() {
		if (la->kind == 1) {
			Get();
		} else if (la->kind == 6) {
			String();
		} else if (la->kind == 2) {
			Get();
		} else if (la->kind == 3) {
			Get();
		} else SynErr(16);
}

void Parser::ParameterDecl() {
		Expect(7);
		if (la->kind == 1 || la->kind == 10) {
			while (la->kind == 1) {
				Get();
				if (la->kind == 8) {
					Get();
				} else if (la->kind == 9) {
					Get();
				} else SynErr(17);
			}
		} else if (la->kind == 9) {
			Get();
		} else SynErr(18);
}

void Parser::FunctionBody() {
		Expect(1);
		while (la->kind == 4) {
			Get();
			Expect(1);
		}
}

void Parser::FunctionDecl() {
		ParameterDecl();
		Expect(10);
		FunctionBody();
}

void Parser::ObjectMemDecl() {
		Expect(1);
		Expect(11);
		if (la->kind == 1) {
			Get();
		} else if (la->kind == 6) {
			String();
		} else SynErr(19);
}

void Parser::ObjectDecl() {
		Expect(12);
		while (la->kind == 1) {
			ObjectMemDecl();
		}
		Expect(12);
}

void Parser::Object() {
		Expect(1);
		Expect(13);
		if (StartOf(1)) {
			VariableDecl();
		} else if (la->kind == 7) {
			FunctionDecl();
		} else if (la->kind == 12) {
			ObjectDecl();
		} else SynErr(20);
}

void Parser::TSCompiler() {
		Expect(14);
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

Parser::Parser(Scanner *scanner, TSData* d) : data(d) {
	maxT = 15;

	dummyToken = NULL;
	t = la = NULL;
	minErrDist = 2;
	errDist = minErrDist;
	this->scanner = scanner;
	errors = new Errors();
}

bool Parser::StartOf(int s) {
	const bool T = true;
	const bool x = false;

	static bool set[2][17] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x},
		{x,T,T,T, x,x,T,x, x,x,x,x, x,x,x,x, x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	delete errors;
	delete dummyToken;
}

Errors::Errors() {
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
			case 5: s = coco_string_create(L"\".\" expected"); break;
			case 6: s = coco_string_create(L"\"\"\" expected"); break;
			case 7: s = coco_string_create(L"\"(\" expected"); break;
			case 8: s = coco_string_create(L"\",\" expected"); break;
			case 9: s = coco_string_create(L"\")\" expected"); break;
			case 10: s = coco_string_create(L"\"->\" expected"); break;
			case 11: s = coco_string_create(L"\":\" expected"); break;
			case 12: s = coco_string_create(L"\"$\" expected"); break;
			case 13: s = coco_string_create(L"\"=\" expected"); break;
			case 14: s = coco_string_create(L"\"TrivialScript\" expected"); break;
			case 15: s = coco_string_create(L"??? expected"); break;
			case 16: s = coco_string_create(L"invalid VariableDecl"); break;
			case 17: s = coco_string_create(L"invalid ParameterDecl"); break;
			case 18: s = coco_string_create(L"invalid ParameterDecl"); break;
			case 19: s = coco_string_create(L"invalid ObjectMemDecl"); break;
			case 20: s = coco_string_create(L"invalid Object"); break;

		default:
		{
			wchar_t format[20];
			coco_swprintf(format, 20, L"error %d", n);
			s = coco_string_create(format);
		}
		break;
	}
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	coco_string_delete(s);
	count++;
}

void Errors::Error(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
	count++;
}

void Errors::Warning(int line, int col, const wchar_t *s) {
	wprintf(L"-- line %d col %d: %ls\n", line, col, s);
}

void Errors::Warning(const wchar_t *s) {
	wprintf(L"%ls\n", s);
}

void Errors::Exception(const wchar_t* s) {
	wprintf(L"%ls", s); 
	exit(1);
}


