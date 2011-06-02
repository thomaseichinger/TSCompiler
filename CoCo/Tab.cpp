/*-------------------------------------------------------------------------
Tab -- Symbol Table Management
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
-------------------------------------------------------------------------*/

#include <wchar.h>
#include "Tab.h"
#include "Parser.h"
#include "BitArray.h"
#include "Scanner.h"

namespace Coco {

const char* Tab::nTyp[] =
		{"    ", "t   ", "pr  ", "nt  ", "clas", "chr ", "wt  ", "any ", "eps ",
		 "sync", "sem ", "alt ", "iter", "opt ", "rslv"};

const char* Tab::tKind[] = {"fixedToken", "classToken", "litToken", "classLitToken"};

Tab::Tab(Parser *parser) {
	for (int i=0; i<10; i++) ddt[i] = false;

	terminals = new ArrayList();
	pragmas = new ArrayList();
	nonterminals = new ArrayList();
	nodes = new ArrayList();
	dummyNode = NULL;
	classes= new ArrayList();
	dummyName = 'A';

	this->parser = parser;
	trace = parser->trace;
	errors = parser->errors;
	eofSy = NewSym(Node::t, L"EOF", 0);
	dummyNode = NewNode(Node::eps, (Symbol*)NULL, 0);
	literals = new HashTable();
	checkEOF = true;
}


Symbol* Tab::NewSym(int typ, const wchar_t* name, int line) {
	if (coco_string_length(name) == 2 && name[0] == '"') {
		parser->SemErr(L"empty token not allowed");
		name = coco_string_create(L"???");
	}
	Symbol *sym = new Symbol(typ, name, line);

	if (typ == Node::t) {
		sym->n = terminals->Count; terminals->Add(sym);
	} else if (typ == Node::pr) {
		pragmas->Add(sym);
	} else if (typ == Node::nt) {
		sym->n = nonterminals->Count; nonterminals->Add(sym);
	}

	return sym;
}

Symbol* Tab::FindSym(const wchar_t* name) {
	Symbol *s;
	int i;
	for (i=0; i<terminals->Count; i++) {
		s = (Symbol*)((*terminals)[i]);
		if (coco_string_equal(s->name, name)) return s;
	}
	for (i=0; i<nonterminals->Count; i++) {
		s = (Symbol*)((*nonterminals)[i]);
		if (coco_string_equal(s->name, name)) return s;
	}
	return NULL;
}

int Tab::Num(Node *p) {
	if (p == NULL) return 0; else return p->n;
}

void Tab::PrintSym(Symbol *sym) {
	wchar_t *paddedName = Name(sym->name);
	fwprintf(trace, L"%3d %14s %ls", sym->n, paddedName, nTyp[sym->typ]);
	coco_string_delete(paddedName);

	if (sym->attrPos==NULL) fwprintf(trace, L" false "); else fwprintf(trace, L" true  ");
	if (sym->typ == Node::nt) {
		fwprintf(trace, L"%5d", Num(sym->graph));
		if (sym->deletable) fwprintf(trace, L" true  "); else fwprintf(trace, L" false ");
	} else
		fwprintf(trace, L"            ");

	fwprintf(trace, L"%5d %ls\n", sym->line, tKind[sym->tokenKind]);
}

void Tab::PrintSymbolTable() {
	fwprintf(trace, L"Symbol Table:\n");
	fwprintf(trace, L"------------\n\n");
	fwprintf(trace, L" nr name          typ  hasAt graph  del    line tokenKind\n");

	Symbol *sym;
	int i;
	for (i=0; i<terminals->Count; i++) {
		sym = (Symbol*)((*terminals)[i]);
		PrintSym(sym);
	}
	for (i=0; i<pragmas->Count; i++) {
		sym = (Symbol*)((*pragmas)[i]);
		PrintSym(sym);
	}
	for (i=0; i<nonterminals->Count; i++) {
		sym = (Symbol*)((*nonterminals)[i]);
		PrintSym(sym);
	}


	fwprintf(trace, L"\nLiteral Tokens:\n");
	fwprintf(trace, L"--------------\n");

	Iterator *iter = literals->GetIterator();
	while (iter->HasNext()) {
		DictionaryEntry *e = iter->Next();
		fwprintf(trace, L"_%ls =  %ls.\n", ((Symbol*) (e->val))->name, e->key);
	}
	fwprintf(trace, L"\n");
}

void Tab::PrintSet(BitArray *s, int indent) {
	int col, len;
	col = indent;
	Symbol *sym;
	for (int i=0; i<terminals->Count; i++) {
		sym = (Symbol*)((*terminals)[i]);
		if ((*s)[sym->n]) {
			len = coco_string_length(sym->name);
			if (col + len >= 80) {
				fwprintf(trace, L"\n");
				for (col = 1; col < indent; col++) fwprintf(trace, L" ");
			}
			fwprintf(trace, L"%ls ", sym->name);
			col += len + 1;
		}
	}
	if (col == indent) fwprintf(trace, L"-- empty set --");
	fwprintf(trace, L"\n");
}

//---------------------------------------------------------------------
//  Syntax graph management
//---------------------------------------------------------------------

Node* Tab::NewNode(int typ, Symbol *sym, int line) {
	Node* node = new Node(typ, sym, line);
	node->n = nodes->Count;
	nodes->Add(node);
	return node;
}


Node* Tab::NewNode(int typ, Node* sub) {
	Node* node = NewNode(typ, (Symbol*)NULL, 0);
	node->sub = sub;
	return node;
}

Node* Tab::NewNode(int typ, int val, int line) {
	Node* node = NewNode(typ, (Symbol*)NULL, line);
	node->val = val;
	return node;
}


void Tab::MakeFirstAlt(Graph *g) {
	g->l = NewNode(Node::alt, g->l); g->l->line = g->l->sub->line;
	g->r->up = true;
	g->l->next = g->r;
	g->r = g->l;
}

// The result will be in g1
void Tab::MakeAlternative(Graph *g1, Graph *g2) {
	g2->l = NewNode(Node::alt, g2->l); g2->l->line = g2->l->sub->line;
	g2->l->up = true;
	g2->r->up = true;
	Node *p = g1->l; while (p->down != NULL) p = p->down;
	p->down = g2->l;
	p = g1->r; while (p->next != NULL) p = p->next;
	// append alternative to g1 end list
	p->next = g2->l;
	// append g2 end list to g1 end list
	g2->l->next = g2->r;
}

// The result will be in g1
void Tab::MakeSequence(Graph *g1, Graph *g2) {
	Node *p = g1->r->next; g1->r->next = g2->l; // link head node
	while (p != NULL) {  // link substructure
		Node *q = p->next; p->next = g2->l;
		p = q;
	}
	g1->r = g2->r;
}

void Tab::MakeIteration(Graph *g) {
	g->l = NewNode(Node::iter, g->l);
	g->r->up = true;
	Node *p = g->r;
	g->r = g->l;
	while (p != NULL) {
		Node *q = p->next; p->next = g->l;
		p = q;
	}
}

void Tab::MakeOption(Graph *g) {
	g->l = NewNode(Node::opt, g->l);
	g->r->up = true;
	g->l->next = g->r;
	g->r = g->l;
}

void Tab::Finish(Graph *g) {
	Node *p = g->r;
	while (p != NULL) {
		Node *q = p->next; p->next = NULL;
		p = q;
	}
}

void Tab::DeleteNodes() {
	nodes = new ArrayList();
	dummyNode = NewNode(Node::eps, (Symbol*)NULL, 0);
}

Graph* Tab::StrToGraph(const wchar_t* str) {
	wchar_t *subStr = coco_string_create(str, 1, coco_string_length(str)-2);
	wchar_t *s = Unescape(subStr);
	coco_string_delete(subStr);
	if (coco_string_length(s) == 0) parser->SemErr(L"empty token not allowed");
	Graph *g = new Graph();
	g->r = dummyNode;
	for (int i = 0; i < coco_string_length(s); i++) {
		Node *p = NewNode(Node::chr, (int)s[i], 0);
		g->r->next = p; g->r = p;
	}
	g->l = dummyNode->next; dummyNode->next = NULL;
	coco_string_delete(s);
	return g;
}


void Tab::SetContextTrans(Node *p) { // set transition code in the graph rooted at p
	while (p != NULL) {
		if (p->typ == Node::chr || p->typ == Node::clas) {
			p->code = Node::contextTrans;
		} else if (p->typ == Node::opt || p->typ == Node::iter) {
			SetContextTrans(p->sub);
		} else if (p->typ == Node::alt) {
			SetContextTrans(p->sub); SetContextTrans(p->down);
		}
		if (p->up) break;
		p = p->next;
	}
}

//------------ graph deletability check -----------------

bool Tab::DelGraph(Node* p) {
	return p == NULL || (DelNode(p) && DelGraph(p->next));
}

bool Tab::DelSubGraph(Node* p) {
	return p == NULL || (DelNode(p) && (p->up || DelSubGraph(p->next)));
}

bool Tab::DelNode(Node* p) {
	if (p->typ == Node::nt) {
		return p->sym->deletable;
	}
	else if (p->typ == Node::alt) {
		return DelSubGraph(p->sub) || (p->down != NULL && DelSubGraph(p->down));
	}
	else {
		return p->typ == Node::iter || p->typ == Node::opt || p->typ == Node::sem
				|| p->typ == Node::eps || p->typ == Node::rslv || p->typ == Node::sync;
	}
}

//----------------- graph printing ----------------------

int Tab::Ptr(Node *p, bool up) {
	if (p == NULL) return 0;
	else if (up) return -(p->n);
	else return p->n;
}

wchar_t* Tab::Pos(Position *pos) {
	wchar_t* format = new wchar_t[10];
	if (pos == NULL) {
		coco_swprintf(format, 10, L"     ");
	} else {
		coco_swprintf(format, 10, L"%5d", pos->beg);
	}
	return format;
}

wchar_t* Tab::Name(const wchar_t *name) {
	wchar_t *name2 = coco_string_create_append(name, L"           ");
	wchar_t *subName2 = coco_string_create(name2, 0, 12);
	coco_string_delete(name2);
	return subName2;
	// found no simpler way to get the first 12 characters of the name
	// padded with blanks on the right
}

void Tab::PrintNodes() {
	fwprintf(trace, L"Graph nodes:\n");
	fwprintf(trace, L"----------------------------------------------------\n");
	fwprintf(trace, L"   n type name          next  down   sub   pos  line\n");
	fwprintf(trace, L"                               val  code\n");
	fwprintf(trace, L"----------------------------------------------------\n");

	Node *p;
	for (int i=0; i<nodes->Count; i++) {
		p = (Node*)((*nodes)[i]);
		fwprintf(trace, L"%4d %ls ", p->n, (nTyp[p->typ]));
		if (p->sym != NULL) {
			wchar_t *paddedName = Name(p->sym->name);
			fwprintf(trace, L"%12s ", paddedName);
			coco_string_delete(paddedName);
		} else if (p->typ == Node::clas) {
			CharClass *c = (CharClass*)(*classes)[p->val];
			wchar_t *paddedName = Name(c->name);
			fwprintf(trace, L"%12s ", paddedName);
			coco_string_delete(paddedName);
		} else fwprintf(trace, L"             ");
		fwprintf(trace, L"%5d ", Ptr(p->next, p->up));

		if (p->typ == Node::t || p->typ == Node::nt || p->typ == Node::wt) {
			fwprintf(trace, L"             %5s", Pos(p->pos));
		} if (p->typ == Node::chr) {
			fwprintf(trace, L"%5d %5d       ", p->val, p->code);
		} if (p->typ == Node::clas) {
			fwprintf(trace, L"      %5d       ", p->code);
		} if (p->typ == Node::alt || p->typ == Node::iter || p->typ == Node::opt) {
			fwprintf(trace, L"%5d %5d       ", Ptr(p->down, false), Ptr(p->sub, false));
		} if (p->typ == Node::sem) {
			fwprintf(trace, L"             %5s", Pos(p->pos));
		} if (p->typ == Node::eps || p->typ == Node::any || p->typ == Node::sync) {
			fwprintf(trace, L"                  ");
		}
		fwprintf(trace, L"%5d\n", p->line);
	}
	fwprintf(trace, L"\n");
}

//---------------------------------------------------------------------
//  Character class management
//---------------------------------------------------------------------


CharClass* Tab::NewCharClass(const wchar_t* name, CharSet *s) {
	CharClass *c;
	if (coco_string_equal(name, L"#")) {
		wchar_t* temp = coco_string_create_append(name, (wchar_t) dummyName++);
		c = new CharClass(temp, s);
		coco_string_delete(temp);
	} else {
		c = new CharClass(name, s);
	}
	c->n = classes->Count;
	classes->Add(c);
	return c;
}

CharClass* Tab::FindCharClass(const wchar_t* name) {
	CharClass *c;
	for (int i=0; i<classes->Count; i++) {
		c = (CharClass*)((*classes)[i]);
		if (coco_string_equal(c->name, name)) return c;
	}
	return NULL;
}

CharClass* Tab::FindCharClass(CharSet *s) {
	CharClass *c;
	for (int i=0; i<classes->Count; i++) {
		c = (CharClass*)((*classes)[i]);
		if (s->Equals(c->set)) return c;
	}
	return NULL;
}

CharSet* Tab::CharClassSet(int i) {
	return ((CharClass*)((*classes)[i]))->set;
}

//----------- character class printing

wchar_t* Tab::Ch(const wchar_t ch) {
	wchar_t* format = new wchar_t[10];
	if (ch < L' ' || ch >= 127 || ch == L'\'' || ch == L'\\') {
		coco_swprintf(format, 10, L"%d", ch);
		return format;
	} else {
		coco_swprintf(format, 10, L"'%lc'", ch);
		return format;
	}
}

void Tab::WriteCharSet(CharSet *s) {
	for (CharSet::Range *r = s->head; r != NULL; r = r->next) {
		if (r->from < r->to) {
			wchar_t *from = Ch(r->from);
			wchar_t *to = Ch(r->to);
			fwprintf(trace, L"%ls .. %ls ", from, to);
			delete [] from;
			delete [] to;
		}
		else {
			wchar_t *from = Ch(r->from);
			fwprintf(trace, L"%ls ", from);
			delete [] from;
		}
	}
}

void Tab::WriteCharClasses () {
	CharClass *c;
	for (int i=0; i<classes->Count; i++) {
		c = (CharClass*)((*classes)[i]);

		wchar_t* format2 = coco_string_create_append(c->name, L"            ");
		wchar_t* format  = coco_string_create(format2, 0, 10);
		coco_string_merge(format, L": ");
		fwprintf(trace, format);

		WriteCharSet(c->set);
		fwprintf(trace, L"\n");
		coco_string_delete(format);
		coco_string_delete(format2);
	}
	fwprintf(trace, L"\n");
}

//---------------------------------------------------------------------
//  Symbol set computations
//---------------------------------------------------------------------

/* Computes the first set for the given Node. */
BitArray* Tab::First0(Node *p, BitArray *mark) {
	BitArray *fs = new BitArray(terminals->Count);
	while (p != NULL && !((*mark)[p->n])) {
		mark->Set(p->n, true);
		if (p->typ == Node::nt) {
			if (p->sym->firstReady) {
				fs->Or(p->sym->first);
			} else {
				BitArray *fs0 = First0(p->sym->graph, mark);
				fs->Or(fs0);
				delete fs0;
			}
		}
		else if (p->typ == Node::t || p->typ == Node::wt) {
			fs->Set(p->sym->n, true);
		}
		else if (p->typ == Node::any) {
			fs->Or(p->set);
		}
		else if (p->typ == Node::alt) {
			BitArray *fs0 = First0(p->sub, mark);
			fs->Or(fs0);
			delete fs0;
			fs0 = First0(p->down, mark);
			fs->Or(fs0);
			delete fs0;
		}
		else if (p->typ == Node::iter || p->typ == Node::opt) {
			BitArray *fs0 = First0(p->sub, mark);
			fs->Or(fs0);
			delete fs0;
		}

		if (!DelNode(p)) break;
		p = p->next;
	}
	return fs;
}

BitArray* Tab::First(Node *p) {
	BitArray *mark = new BitArray(nodes->Count);
	BitArray *fs = First0(p, mark);
	delete mark;
	if (ddt[3]) {
		fwprintf(trace, L"\n");
		if (p != NULL) fwprintf(trace, L"First: node = %d\n", p->n );
		else fwprintf(trace, L"First: node = null\n");
		PrintSet(fs, 0);
	}
	return fs;
}


void Tab::CompFirstSets() {
	Symbol *sym;
	int i;
	for (i=0; i<nonterminals->Count; i++) {
		sym = (Symbol*)((*nonterminals)[i]);
		sym->first = new BitArray(terminals->Count);
		sym->firstReady = false;
	}
	for (i=0; i<nonterminals->Count; i++) {
		sym = (Symbol*)((*nonterminals)[i]);
		sym->first = First(sym->graph);
		sym->firstReady = true;
	}
}

void Tab::CompFollow(Node *p) {
	while (p != NULL && !((*visited)[p->n])) {
		visited->Set(p->n, true);
		if (p->typ == Node::nt) {
			BitArray *s = First(p->next);
			p->sym->follow->Or(s);
			if (DelGraph(p->next))
				p->sym->nts->Set(curSy->n, true);
		} else if (p->typ == Node::opt || p->typ == Node::iter) {
			CompFollow(p->sub);
		} else if (p->typ == Node::alt) {
			CompFollow(p->sub); CompFollow(p->down);
		}
		p = p->next;
	}
}

void Tab::Complete(Symbol *sym) {
	if (!((*visited)[sym->n])) {
		visited->Set(sym->n, true);
		Symbol *s;
		for (int i=0; i<nonterminals->Count; i++) {
			s = (Symbol*)((*nonterminals)[i]);
			if ((*(sym->nts))[s->n]) {
				Complete(s);
				sym->follow->Or(s->follow);
				if (sym == curSy) sym->nts->Set(s->n, false);
			}
		}
	}
}

void Tab::CompFollowSets() {
	Symbol *sym;
	int i;
	for (i=0; i<nonterminals->Count; i++) {
		sym = (Symbol*)((*nonterminals)[i]);
		sym->follow = new BitArray(terminals->Count);
		sym->nts = new BitArray(nonterminals->Count);
	}
	gramSy->follow->Set(eofSy->n, true);
	visited = new BitArray(nodes->Count);
	for (i=0; i<nonterminals->Count; i++) {  // get direct successors of nonterminals
		sym = (Symbol*)((*nonterminals)[i]);
		curSy = sym;
		CompFollow(sym->graph);
	}

	for (i=0; i<nonterminals->Count; i++) {  // add indirect successors to followers
		sym = (Symbol*)((*nonterminals)[i]);
		visited = new BitArray(nonterminals->Count);
		curSy = sym;
		Complete(sym);
	}
}

Node* Tab::LeadingAny(Node *p) {
	if (p == NULL) return NULL;
	Node *a = NULL;
	if (p->typ == Node::any) a = p;
	else if (p->typ == Node::alt) {
		a = LeadingAny(p->sub);
		if (a == NULL) a = LeadingAny(p->down);
	}
	else if (p->typ == Node::opt || p->typ == Node::iter) a = LeadingAny(p->sub);
	if (a == NULL && DelNode(p) && !p->up) a = LeadingAny(p->next);
	return a;
}

void Tab::FindAS(Node *p) { // find ANY sets
	Node *a;
	while (p != NULL) {
		if (p->typ == Node::opt || p->typ == Node::iter) {
			FindAS(p->sub);
			a = LeadingAny(p->sub);
			if (a != NULL) Sets::Subtract(a->set, First(p->next));
		} else if (p->typ == Node::alt) {
			BitArray *s1 = new BitArray(terminals->Count);
			Node *q = p;
			while (q != NULL) {
				FindAS(q->sub);
				a = LeadingAny(q->sub);
				if (a != NULL) {
					BitArray *tmp = First(q->down);
					tmp->Or(s1);
					Sets::Subtract(a->set, tmp);
				} else {
					BitArray *f = First(q->sub);
					s1->Or(f);
					delete f;
				}
				q = q->down;
			}
		}

		// Remove alternative terminals before ANY, in the following
		// examples a and b must be removed from the ANY set:
		// [a] ANY, or {a|b} ANY, or [a][b] ANY, or (a|) ANY, or
		// A = [a]. A ANY
		if (DelNode(p)) {
			a = LeadingAny(p->next);
			if (a != NULL) {
				Node *q = (p->typ == Node::nt) ? p->sym->graph : p->sub;
				Sets::Subtract(a->set, First(q));
			}
		}

		if (p->up) break;
		p = p->next;
	}
}

void Tab::CompAnySets() {
	Symbol *sym;
	for (int i=0; i<nonterminals->Count; i++) {
		sym = (Symbol*)((*nonterminals)[i]);
		FindAS(sym->graph);
	}
}

BitArray* Tab::Expected(Node *p, Symbol *curSy) {
	BitArray *s = First(p);
	if (DelGraph(p))
		s->Or(curSy->follow);
	return s;
}

// does not look behind resolvers; only called during LL(1) test and in CheckRes
BitArray* Tab::Expected0(Node *p, Symbol *curSy) {
	if (p->typ == Node::rslv) return new BitArray(terminals->Count);
	else return Expected(p, curSy);
}

void Tab::CompSync(Node *p) {
	while (p != NULL && !(visited->Get(p->n))) {
		visited->Set(p->n, true);
		if (p->typ == Node::sync) {
			BitArray *s = Expected(p->next, curSy);
			s->Set(eofSy->n, true);
			allSyncSets->Or(s);
			p->set = s;
		} else if (p->typ == Node::alt) {
			CompSync(p->sub); CompSync(p->down);
		} else if (p->typ == Node::opt || p->typ == Node::iter)
			CompSync(p->sub);
		p = p->next;
	}
}

void Tab::CompSyncSets() {
	allSyncSets = new BitArray(terminals->Count);
	allSyncSets->Set(eofSy->n, true);
	visited = new BitArray(nodes->Count);

	Symbol *sym;
	for (int i=0; i<nonterminals->Count; i++) {
		sym = (Symbol*)((*nonterminals)[i]);
		curSy = sym;
		CompSync(curSy->graph);
	}
}

void Tab::SetupAnys() {
	Node *p;
	for (int i=0; i<nodes->Count; i++) {
		p = (Node*)((*nodes)[i]);
		if (p->typ == Node::any) {
			p->set = new BitArray(terminals->Count, true);
			p->set->Set(eofSy->n, false);
		}
	}
}

void Tab::CompDeletableSymbols() {
	bool changed;
	Symbol *sym;
	int i;
	do {
		changed = false;
		for (i=0; i<nonterminals->Count; i++) {
			sym = (Symbol*)((*nonterminals)[i]);
			if (!sym->deletable && sym->graph != NULL && DelGraph(sym->graph)) {
				sym->deletable = true; changed = true;
			}
		}
	} while (changed);

	for (i=0; i<nonterminals->Count; i++) {
		sym = (Symbol*)((*nonterminals)[i]);
		if (sym->deletable)
			wprintf(L"  %ls deletable\n",  sym->name);
	}
}

void Tab::RenumberPragmas() {
	int n = terminals->Count;
	Symbol *sym;
	for (int i=0; i<pragmas->Count; i++) {
		sym = (Symbol*)((*pragmas)[i]);
		sym->n = n++;
	}
}

void Tab::CompSymbolSets() {
	CompDeletableSymbols();
	CompFirstSets();
	CompAnySets();
	CompFollowSets();
	CompSyncSets();
	if (ddt[1]) {
		fwprintf(trace, L"\n");
		fwprintf(trace, L"First & follow symbols:\n");
		fwprintf(trace, L"----------------------\n\n");

		Symbol *sym;
		for (int i=0; i<nonterminals->Count; i++) {
			sym = (Symbol*)((*nonterminals)[i]);
			fwprintf(trace, L"%ls\n", sym->name);
			fwprintf(trace, L"first:   "); PrintSet(sym->first, 10);
			fwprintf(trace, L"follow:  "); PrintSet(sym->follow, 10);
			fwprintf(trace, L"\n");
		}
	}
	if (ddt[4]) {
		fwprintf(trace, L"\n");
		fwprintf(trace, L"ANY and SYNC sets:\n");
		fwprintf(trace, L"-----------------\n");

		Node *p;
		for (int i=0; i<nodes->Count; i++) {
			p = (Node*)((*nodes)[i]);
			if (p->typ == Node::any || p->typ == Node::sync) {
				fwprintf(trace, L"%4d %4s ", p->n, nTyp[p->typ]);
				PrintSet(p->set, 11);
			}
		}
	}
}

//---------------------------------------------------------------------
//  String handling
//---------------------------------------------------------------------

wchar_t Tab::Hex2Char(const wchar_t* s) {
	int val = 0;
	int len = coco_string_length(s);
	for (int i = 0; i < len; i++) {
		wchar_t ch = s[i];
		if ('0' <= ch && ch <= '9') val = 16 * val + (ch - '0');
		else if ('a' <= ch && ch <= 'f') val = 16 * val + (10 + ch - 'a');
		else if ('A' <= ch && ch <= 'F') val = 16 * val + (10 + ch - 'A');
		else parser->SemErr(L"bad escape sequence in string or character");
	}
	if (val >= COCO_WCHAR_MAX) {/* pdt */
		parser->SemErr(L"bad escape sequence in string or character");
	}
	return (wchar_t) val;
}

wchar_t* Tab::Char2Hex(const wchar_t ch) {
	wchar_t* format = new wchar_t[10];
	coco_swprintf(format, 10, L"\\0x%04x", ch);
	return format;
}

wchar_t* Tab::Unescape (const wchar_t* s) {
	/* replaces escape sequences in s by their Unicode values. */
	StringBuilder buf = StringBuilder();
	int i = 0;
	int len = coco_string_length(s);
	while (i < len) {
		if (s[i] == '\\') {
			switch (s[i+1]) {
				case L'\\': buf.Append(L'\\'); i += 2; break;
				case L'\'': buf.Append(L'\''); i += 2; break;
				case L'\"': buf.Append(L'\"'); i += 2; break;
				case L'r': buf.Append(L'\r'); i += 2; break;
				case L'n': buf.Append(L'\n'); i += 2; break;
				case L't': buf.Append(L'\t'); i += 2; break;
				case L'0': buf.Append(L'\0'); i += 2; break;
				case L'a': buf.Append(L'\a'); i += 2; break;
				case L'b': buf.Append(L'\b'); i += 2; break;
				case L'f': buf.Append(L'\f'); i += 2; break;
				case L'v': buf.Append(L'\v'); i += 2; break;
				case L'u': case L'x':
					if (i + 6 <= coco_string_length(s)) {
						wchar_t *subS = coco_string_create(s, i+2, 4);
						buf.Append(Hex2Char(subS)); i += 6; break;
						coco_string_delete(subS);
					} else {
						parser->SemErr(L"bad escape sequence in string or character");
						i = coco_string_length(s); break;
					}
				default:
						parser->SemErr(L"bad escape sequence in string or character");
					i += 2; break;
			}
		} else {
			buf.Append(s[i]);
			i++;
		}
	}

	return buf.ToString();
}


wchar_t* Tab::Escape (const wchar_t* s) {
	StringBuilder buf = StringBuilder();
	wchar_t ch;
	int len = coco_string_length(s);
	for (int i=0; i < len; i++) {
		ch = s[i];
		switch(ch) {
			case L'\\': buf.Append(L"\\\\"); break;
			case L'\'': buf.Append(L"\\'"); break;
			case L'\"': buf.Append(L"\\\""); break;
			case L'\t': buf.Append(L"\\t"); break;
			case L'\r': buf.Append(L"\\r"); break;
			case L'\n': buf.Append(L"\\n"); break;
			default:
				if ((ch < L' ') || (ch > 0x7f)) {
					wchar_t* res = Char2Hex(ch);
					buf.Append(res);
					delete [] res;
				} else
					buf.Append(ch);
				break;
		}
	}
	return buf.ToString();
}


//---------------------------------------------------------------------
//  Grammar checks
//---------------------------------------------------------------------

bool Tab::GrammarOk() {
	bool ok = NtsComplete()
		&& AllNtReached()
		&& NoCircularProductions()
		&& AllNtToTerm();
    if (ok) { CheckResolvers(); CheckLL1(); }
    return ok;
}


//--------------- check for circular productions ----------------------

void Tab::GetSingles(Node *p, ArrayList *singles) {
	if (p == NULL) return;  // end of graph
	if (p->typ == Node::nt) {
		if (p->up || DelGraph(p->next)) singles->Add(p->sym);
	} else if (p->typ == Node::alt || p->typ == Node::iter || p->typ == Node::opt) {
		if (p->up || DelGraph(p->next)) {
			GetSingles(p->sub, singles);
			if (p->typ == Node::alt) GetSingles(p->down, singles);
		}
	}
	if (!p->up && DelNode(p)) GetSingles(p->next, singles);
}

bool Tab::NoCircularProductions() {
	bool ok, changed, onLeftSide, onRightSide;
	ArrayList *list = new ArrayList();


	Symbol *sym;
	int i;
	for (i=0; i<nonterminals->Count; i++) {
		sym = (Symbol*)((*nonterminals)[i]);
		ArrayList *singles = new ArrayList();
		GetSingles(sym->graph, singles); // get nonterminals s such that sym-->s
		Symbol *s;
		for (int j=0; j<singles->Count; j++) {
			s = (Symbol*)((*singles)[j]);
			list->Add(new CNode(sym, s));
		}
	}

	CNode *n;
	do {
		changed = false;
		for (i = 0; i < list->Count; i++) {
			n = (CNode*)(*list)[i];
			onLeftSide = false; onRightSide = false;
			CNode *m;
			for (int j=0; j<list->Count; j++) {
				m = (CNode*)((*list)[j]);
				if (n->left == m->right) onRightSide = true;
				if (n->right == m->left) onLeftSide = true;
			}
			if (!onLeftSide || !onRightSide) {
				list->Remove(n); i--; changed = true;
			}
		}
	} while(changed);
	ok = true;

	for (i=0; i<list->Count; i++) {
		n = (CNode*)((*list)[i]);
			ok = false; errors->count++;
		wprintf(L"  %ls --> %ls", n->left->name, n->right->name);
	}
	return ok;
}


//--------------- check for LL(1) errors ----------------------

void Tab::LL1Error(int cond, Symbol *sym) {
	wprintf(L"  LL1 warning in %ls: ", curSy->name);
	if (sym != NULL) wprintf(L"%ls is ", sym->name);
	switch (cond) {
		case 1: wprintf(L"start of several alternatives\n"); break;
		case 2: wprintf(L"start & successor of deletable structure\n"); break;
		case 3: wprintf(L"an ANY node that matches no symbol\n"); break;
		case 4: wprintf(L"contents of [...] or {...} must not be deletable\n"); break;
	}
}


void Tab::CheckOverlap(BitArray *s1, BitArray *s2, int cond) {
	Symbol *sym;
	for (int i=0; i<terminals->Count; i++) {
		sym = (Symbol*)((*terminals)[i]);
		if ((*s1)[sym->n] && (*s2)[sym->n]) {
			LL1Error(cond, sym);
		}
	}
}

void Tab::CheckAlts(Node *p) {
	BitArray *s1, *s2;
	while (p != NULL) {
		if (p->typ == Node::alt) {
			Node *q = p;
			s1 = new BitArray(terminals->Count);
			while (q != NULL) { // for all alternatives
				s2 = Expected0(q->sub, curSy);
				CheckOverlap(s1, s2, 1);
				s1->Or(s2);
				CheckAlts(q->sub);
				q = q->down;
			}
		} else if (p->typ == Node::opt || p->typ == Node::iter) {
			if (DelSubGraph(p->sub)) LL1Error(4, NULL); // e.g. [[...]]
			else {
				s1 = Expected0(p->sub, curSy);
				s2 = Expected(p->next, curSy);
				CheckOverlap(s1, s2, 2);
			}
			CheckAlts(p->sub);
		} else if (p->typ == Node::any) {
			if (Sets::Elements(p->set) == 0) LL1Error(3, NULL);
			// e.g. {ANY} ANY or [ANY] ANY or ( ANY | ANY )
		}
		if (p->up) break;
		p = p->next;
	}
}

void Tab::CheckLL1() {
	Symbol *sym;
	for (int i=0; i<nonterminals->Count; i++) {
		sym = (Symbol*)((*nonterminals)[i]);
		curSy = sym;
		CheckAlts(curSy->graph);
	}
}

//------------- check if resolvers are legal  --------------------

void Tab::ResErr(Node *p, const wchar_t* msg) {
	errors->Warning(p->line, p->pos->col, msg);
}

void Tab::CheckRes(Node *p, bool rslvAllowed) {
	while (p != NULL) {

		Node *q;
		if (p->typ == Node::alt) {
			BitArray *expected = new BitArray(terminals->Count);
			for (q = p; q != NULL; q = q->down)
				expected->Or(Expected0(q->sub, curSy));
			BitArray *soFar = new BitArray(terminals->Count);
			for (q = p; q != NULL; q = q->down) {
				if (q->sub->typ == Node::rslv) {
					BitArray *fs = Expected(q->sub->next, curSy);
					if (Sets::Intersect(fs, soFar))
						ResErr(q->sub, L"Warning: Resolver will never be evaluated. Place it at previous conflicting alternative.");
					if (!Sets::Intersect(fs, expected))
						ResErr(q->sub, L"Warning: Misplaced resolver: no LL(1) conflict.");
				} else soFar->Or(Expected(q->sub, curSy));
				CheckRes(q->sub, true);
			}
		} else if (p->typ == Node::iter || p->typ == Node::opt) {
			if (p->sub->typ == Node::rslv) {
				BitArray *fs = First(p->sub->next);
				BitArray *fsNext = Expected(p->next, curSy);
				if (!Sets::Intersect(fs, fsNext))
					ResErr(p->sub, L"Warning: Misplaced resolver: no LL(1) conflict.");
			}
			CheckRes(p->sub, true);
		} else if (p->typ == Node::rslv) {
			if (!rslvAllowed)
				ResErr(p, L"Warning: Misplaced resolver: no alternative.");
		}

		if (p->up) break;
		p = p->next;
		rslvAllowed = false;
	}
}

void Tab::CheckResolvers() {
	for (int i=0; i<nonterminals->Count; i++) {
		curSy = (Symbol*)((*nonterminals)[i]);
		CheckRes(curSy->graph, false);
	}
}


//------------- check if every nts has a production --------------------

bool Tab::NtsComplete() {
	bool complete = true;
	Symbol *sym;
	for (int i=0; i<nonterminals->Count; i++) {
		sym = (Symbol*)((*nonterminals)[i]);
		if (sym->graph == NULL) {
				complete = false; errors->count++;
			wprintf(L"  No production for %ls\n", sym->name);
		}
	}
	return complete;
}

//-------------- check if every nts can be reached  -----------------

void Tab::MarkReachedNts(Node *p) {
	while (p != NULL) {
		if (p->typ == Node::nt && !((*visited)[p->sym->n])) { // new nt reached
			visited->Set(p->sym->n, true);
			MarkReachedNts(p->sym->graph);
		} else if (p->typ == Node::alt || p->typ == Node::iter || p->typ == Node::opt) {
			MarkReachedNts(p->sub);
			if (p->typ == Node::alt) MarkReachedNts(p->down);
		}
		if (p->up) break;
		p = p->next;
	}
}

bool Tab::AllNtReached() {
	bool ok = true;
	visited = new BitArray(nonterminals->Count);
	visited->Set(gramSy->n, true);
	MarkReachedNts(gramSy->graph);
	Symbol *sym;
	for (int i=0; i<nonterminals->Count; i++) {
		sym = (Symbol*)((*nonterminals)[i]);
		if (!((*visited)[sym->n])) {
				ok = false; errors->count++;
			wprintf(L"  %ls cannot be reached\n", sym->name);
		}
	}
	return ok;
}

//--------- check if every nts can be derived to terminals  ------------

bool Tab::IsTerm(Node *p, BitArray *mark) { // true if graph can be derived to terminals
	while (p != NULL) {
		if (p->typ == Node::nt && !((*mark)[p->sym->n])) return false;
		if (p->typ == Node::alt && !IsTerm(p->sub, mark)
		&& (p->down == NULL || !IsTerm(p->down, mark))) return false;
		if (p->up) break;
		p = p->next;
	}
	return true;
}


bool Tab::AllNtToTerm() {
	bool changed, ok = true;
	BitArray *mark = new BitArray(nonterminals->Count);
	// a nonterminal is marked if it can be derived to terminal symbols
	Symbol *sym;
	int i;
	do {
		changed = false;

		for (i=0; i<nonterminals->Count; i++) {
			sym = (Symbol*)((*nonterminals)[i]);
			if (!((*mark)[sym->n]) && IsTerm(sym->graph, mark)) {
				mark->Set(sym->n, true); changed = true;
			}
		}
	} while (changed);
	for (i=0; i<nonterminals->Count; i++) {
		sym = (Symbol*)((*nonterminals)[i]);
		if (!((*mark)[sym->n])) {
				ok = false; errors->count++;
			wprintf(L"  %ls cannot be derived to terminals\n", sym->name);
		}
	}
	return ok;
}

//---------------------------------------------------------------------
//  Cross reference list
//---------------------------------------------------------------------

void Tab::XRef() {
	SortedList *xref = new SortedList();
	// collect lines where symbols have been defined
	Symbol *sym;
	int i, j;
	for (i=0; i<nonterminals->Count; i++) {
		sym = (Symbol*)((*nonterminals)[i]);
		ArrayList *list = (ArrayList*)(xref->Get(sym));
		if (list == NULL) {list = new ArrayList(); xref->Set(sym, list);}
		int *intg = new int(- sym->line);
		list->Add(intg);
	}
	// collect lines where symbols have been referenced
	Node *n;
	for (i=0; i<nodes->Count; i++) {
		n = (Node*)((*nodes)[i]);
		if (n->typ == Node::t || n->typ == Node::wt || n->typ == Node::nt) {
			ArrayList *list = (ArrayList*)(xref->Get(n->sym));
			if (list == NULL) {list = new ArrayList(); xref->Set(n->sym, list);}
			int *intg = new int(n->line);
			list->Add(intg);
		}
	}
	// print cross reference list
	fwprintf(trace, L"\n");
	fwprintf(trace, L"Cross reference list:\n");
	fwprintf(trace, L"--------------------\n\n");

	for (i=0; i<xref->Count; i++) {
		sym = (Symbol*)(xref->GetKey(i));
		wchar_t *paddedName = Name(sym->name);
		fwprintf(trace, L"  %12s", paddedName);
		coco_string_delete(paddedName);
		ArrayList *list = (ArrayList*)(xref->Get(sym));
		int col = 14;
		int line;
		for (j=0; j<list->Count; j++) {
			line = *(int*)((*list)[j]);
			if (col + 5 > 80) {
				fwprintf(trace, L"\n");
				for (col = 1; col <= 14; col++) fwprintf(trace, L" ");
			}
			fwprintf(trace, L"%5d", line); col += 5;
		}
		fwprintf(trace, L"\n");
	}
	fwprintf(trace, L"\n\n");
}

void Tab::SetDDT(const wchar_t* s) {
	wchar_t* st = coco_string_create_upper(s);
	wchar_t ch;
	int len = coco_string_length(st);
	for (int i = 0; i < len; i++) {
		ch = st[i];
		if (L'0' <= ch && ch <= L'9') ddt[ch - L'0'] = true;
		else switch (ch) {
			case L'A' : ddt[0] = true; break; // trace automaton
			case L'F' : ddt[1] = true; break; // list first/follow sets
			case L'G' : ddt[2] = true; break; // print syntax graph
			case L'I' : ddt[3] = true; break; // trace computation of first sets
			case L'J' : ddt[4] = true; break; // print ANY and SYNC sets
			case L'P' : ddt[8] = true; break; // print statistics
			case L'S' : ddt[6] = true; break; // list symbol table
			case L'X' : ddt[7] = true; break; // list cross reference table
			default : break;
		}
	}
	coco_string_delete(st);
}


void Tab::SetOption(const wchar_t* s) {
	// example: $namespace=xxx
	//   index of '=' is 10 => nameLenght = 10
	//   start index of xxx = 11

	int nameLenght = coco_string_indexof(s, '=');
	int valueIndex = nameLenght + 1;

	wchar_t *name = coco_string_create(s, 0, nameLenght);
	wchar_t *value = coco_string_create(s, valueIndex);

	if (coco_string_equal(L"$namespace", name)) {
		if (nsName == NULL) nsName = coco_string_create(value);
	} else if (coco_string_equal(L"$checkEOF", name)) {
		checkEOF = coco_string_equal(L"true", value);
	}

	delete [] name;
	delete [] value;
}


}; // namespace
