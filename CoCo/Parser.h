/*----------------------------------------------------------------------
Compiler Generator Coco/R,
Copyright (c) 1990, 2004 Hanspeter Moessenboeck, University of Linz
extended by M. Loeberbauer & A. Woess, Univ. of Linz
ported to C++ by Csaba Balazs, University of Szeged
with improvements by Pat Terry, Rhodes University

This program is free software; you can redistribute it and/or modify it 
under the terms of the GNU General Public License as published by the 
Free Software Foundation; either version 2, or (at your option) any 
later version.

This program is distributed in the hope that it will be useful, but 
WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY 
or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License 
for more details.

You should have received a copy of the GNU General Public License along 
with this program; if not, write to the Free Software Foundation, Inc., 
59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.

As an exception, it is allowed to write an extension of Coco/R that is
used as a plugin in non-free software.

If not otherwise stated, any source code generated by Coco/R (other than 
Coco/R itself) does not fall under the GNU General Public License.
-----------------------------------------------------------------------*/


#if !defined(COCO_PARSER_H__)
#define COCO_PARSER_H__

#include "Tab.h"
#include "DFA.h"
#include "ParserGen.h"


#include "Scanner.h"

namespace Coco {


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

} // namespace


#endif // !defined(COCO_PARSER_H__)

