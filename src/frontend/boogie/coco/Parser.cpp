

#include <wchar.h>
#include "Parser.h"
#include "Scanner.h"


namespace frontend {
namespace boogie {
namespace parser {


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

void Parser::BoogiePL() {
		while (StartOf(1)) {
			switch (la->kind) {
			case 8 /* "const" */: {
				constantsDef(program );
				break;
			}
			case 26 /* "function" */: {
				FunctionDef(program );
				break;
			}
			case 30 /* "axiom" */: {
				Axiom(program );
				break;
			}
			case 31 /* "type" */: {
				TypeDefs(program );
				break;
			}
			case 15 /* "var" */: {
				GlobalVarDefs(program );
				break;
			}
			case 33 /* "procedure" */: {
				Procedure(program );
				break;
			}
			case 34 /* "implementation" */: {
				Implementation(program );
				break;
			}
			}
		}
		Expect(_EOF);
}

void Parser::constantsDef(AST::Scope& scope ) {
		Token startToken; 
		vector<AST::Identifier> ids;
		unique_ptr<AST::Type> type;
		bool unique = false; 
		unique_ptr<AST::Attributes> attributes;
		unique_ptr<AST::ConstantOrderSpec> orderSpec;
		
		Expect(8 /* "const" */);
		startToken = *t; 
		while (la->kind == 28 /* "{" */) {
			Attributes(attributes);
		}
		if (la->kind == 9 /* "unique" */) {
			Get();
			unique = true;  
		}
		Identifiers(ids);
		Expect(10 /* ":" */);
		Type(type);
		if (la->kind == 12 /* "extends" */) {
			OrderSpec(orderSpec);
		}
		for (const auto& id : ids) 
		scope.addVariableDef(unique_ptr<Variable>(
		new AST::Constant(toTextPos(startToken), attributes->clone(), id, type->clone(),unique, orderSpec->clone() ))); 
		
		Expect(11 /* ";" */);
}

void Parser::FunctionDef(Scope& scope ) {
		IToken/*!*/ z;
		IToken/*!*/ typeParamTok;
		var typeParams = new List<TypeVariable>();
		var arguments = new List<Variable>();
		TypedIdent/*!*/ tyd;
		TypedIdent retTyd = null;
		Bpl.Type/*!*/ retTy;
		QKeyValue argKv = null;
		QKeyValue kv = null;
		Expr definition = null;
		Expr/*!*/ tmp;
		
		Expect(26 /* "function" */);
		while (la->kind == 28 /* "{" */) {
			Attributes(ref kv);
		}
		Indetifier(out z);
		if (la->kind == 24 /* "<" */) {
			TypeParams(out typeParamTok, out typeParams);
		}
		Expect(17 /* "(" */);
		if (StartOf(2)) {
			VarOrType(out tyd, out argKv);
			arguments.Add(new Formal(tyd.tok, tyd, true, argKv)); 
			while (la->kind == 13 /* "," */) {
				Get();
				VarOrType(out tyd, out argKv);
				arguments.Add(new Formal(tyd.tok, tyd, true, argKv)); 
			}
		}
		Expect(18 /* ")" */);
		argKv = null; 
		if (la->kind == 27 /* "returns" */) {
			Get();
			Expect(17 /* "(" */);
			VarOrType(out retTyd, out argKv);
			Expect(18 /* ")" */);
		} else if (la->kind == 10 /* ":" */) {
			Get();
			Type(out retTy);
			retTyd = new TypedIdent(retTy.tok, TypedIdent.NoName, retTy); 
		} else SynErr(98);
		if (la->kind == 28 /* "{" */) {
			Get();
			Expression(out tmp);
			definition = tmp; 
			Expect(29 /* "}" */);
		} else if (la->kind == 11 /* ";" */) {
			Get();
		} else SynErr(99);
		if (retTyd == null) {
		 // construct a dummy type for the case of syntax error
		 retTyd = new TypedIdent(t, TypedIdent.NoName, new BasicType(t, SimpleType.Int));
		}
		Function/*!*/ func = new Function(z, z.val, typeParams, arguments,
		                                 new Formal(retTyd.tok, retTyd, false, argKv), null, kv);
		Contract.Assert(func != null);
		ds.Add(func);
		bool allUnnamed = true;
		foreach(Formal/*!*/ f in arguments){
		 Contract.Assert(f != null);
		 if (f.TypedIdent.HasName) {
		   allUnnamed = false;
		   break;
		 }
		}
		if (!allUnnamed) {
		 Bpl.Type prevType = null;
		 for (int i = arguments.Count; 0 <= --i; ) {
		   TypedIdent/*!*/ curr = cce.NonNull(arguments[i]).TypedIdent;
		   if (curr.HasName) {
		     // the argument was given as both an identifier and a type
		     prevType = curr.Type;
		   } else {
		     // the argument was given as just one "thing", which syntactically parsed as a type
		     if (prevType == null) {
		       this.errors.SemErr(curr.tok, "the type of the last parameter is unspecified");
		       break;
		     }
		     Bpl.Type ty = curr.Type;
		     var uti = ty as UnresolvedTypeIdentifier;
		     if (uti != null && uti.Arguments.Count == 0) {
		       // the given "thing" was just an identifier, so let's use it as the name of the parameter
		       curr.Name = uti.Name;
		       curr.Type = prevType;
		     } else {
		       this.errors.SemErr(curr.tok, "expecting an identifier as parameter name");
		     }
		   }
		 }
		}
		if (definition != null) {
		 // generate either an axiom or a function body
		 if (QKeyValue.FindBoolAttribute(kv, "inline")) {
		   func.Body = definition;
		 } else {
		   ds.Add(func.CreateDefinitionAxiom(definition, kv));
		 }
		}
		
}

void Parser::Axiom(Scope& scope /*out Axiom/*!*/ m*/) {
		Contract.Ensures(Contract.ValueAtReturn(out m) != null); Expr/*!*/ e; QKeyValue kv = null; 
		Expect(30 /* "axiom" */);
		while (la->kind == 28 /* "{" */) {
			Attributes(ref kv);
		}
		IToken/*!*/ x = t; 
		Proposition(out e);
		Expect(11 /* ";" */);
		m = new Axiom(x,e, null, kv); 
}

void Parser::TypeDefs(Scope& scope /*out List<Declaration> ts*/ ) {
		Contract.Ensures(cce.NonNullElements(Contract.ValueAtReturn(out ts))); Declaration/*!*/ decl; QKeyValue kv = null; ts = new List<Declaration/*!*/> (); 
		Expect(31 /* "type" */);
		while (la->kind == 28 /* "{" */) {
			Attributes(ref kv);
		}
		UserDefinedType(out decl, kv);
		ts.Add(decl);  
		while (la->kind == 13 /* "," */) {
			Get();
			UserDefinedType(out decl, kv);
			ts.Add(decl);  
		}
		Expect(11 /* ";" */);
}

void Parser::GlobalVarDefs(Scope& scope ) {
		AST::Attributes attributes; 
		Expect(15 /* "var" */);
		while (la->kind == 28 /* "{" */) {
			Attributes(attributes);
		}
		IdsTypeWheres(true, "global variables", [attributes&,vs&]{TypedIdent tid => vs.push_back(AST::GlobalVariable(tid, attributes)) } );
		Expect(11 /* ";" */);
}

void Parser::Procedure(Scope& scope ) {
		Contract.Ensures(Contract.ValueAtReturn(out proc) != null); IToken/*!*/ x;
		List<TypeVariable>/*!*/ typeParams;
		List<Variable>/*!*/ ins, outs;
		List<Requires>/*!*/ pre = new List<Requires>();
		List<IdentifierExpr>/*!*/ mods = new List<IdentifierExpr>();
		List<Ensures>/*!*/ post = new List<Ensures>();
		
		List<Variable>/*!*/ locals = new List<Variable>();
		StmtList/*!*/ stmtList;
		QKeyValue kv = null;
		impl = null;
		
		Expect(33 /* "procedure" */);
		ProcSignature(true, out x, out typeParams, out ins, out outs, out kv);
		if (la->kind == 11 /* ";" */) {
			Get();
			while (StartOf(3)) {
				Spec(pre, mods, post);
			}
		} else if (StartOf(4)) {
			while (StartOf(3)) {
				Spec(pre, mods, post);
			}
			ImplBody(out locals, out stmtList);
			impl = new Implementation(x, x.val, typeParams,
			                         Formal.StripWhereClauses(ins), Formal.StripWhereClauses(outs), locals, stmtList, kv == null ? null : (QKeyValue)kv.Clone(), this.errors);
			
		} else SynErr(100);
		proc = new Procedure(x, x.val, typeParams, ins, outs, pre, mods, post, kv); 
}

void Parser::Implementation(Scope& scope ) {
		Contract.Ensures(Contract.ValueAtReturn(out impl) != null); IToken/*!*/ x;
		List<TypeVariable>/*!*/ typeParams;
		List<Variable>/*!*/ ins, outs;
		List<Variable>/*!*/ locals;
		StmtList/*!*/ stmtList;
		QKeyValue kv;
		
		Expect(34 /* "implementation" */);
		ProcSignature(false, out x, out typeParams, out ins, out outs, out kv);
		ImplBody(out locals, out stmtList);
		impl = new Implementation(x, x.val, typeParams, ins, outs, locals, stmtList, kv, this.errors); 
}

void Parser::Attributes(unique_ptr<Attributes>& ) {
		Trigger trig = null; 
		AttributeOrTrigger(ref kv, ref trig);
		if (trig != null) this.SemErr("only attributes, not triggers, allowed here"); 
}

void Parser::Identifiers(vector<Identifier>& ids ) {
		Contract.Ensures(Contract.ValueAtReturn(out xs) != null); IToken/*!*/ id; xs = new List<IToken>(); 
		Indetifier(out id);
		xs.Add(id); 
		while (la->kind == 13 /* "," */) {
			Get();
			Indetifier(out id);
			xs.Add(id); 
		}
}

void Parser::Type(unique_ptr<Type>& type ) {
		Contract.Ensures(Contract.ValueAtReturn(out ty) != null); IToken/*!*/ tok; ty = dummyType; 
		if (StartOf(5)) {
			TypeAtom(out ty);
		} else if (la->kind == _ident) {
			Indetifier(out tok);
			List<Bpl.Type>/*!*/ args = new List<Bpl.Type> (); 
			if (StartOf(6)) {
				TypeArgs(args);
			}
			ty = new UnresolvedTypeIdentifier (tok, tok.val, args); 
		} else if (la->kind == 22 /* "[" */ || la->kind == 24 /* "<" */) {
			MapType(out ty);
		} else SynErr(101);
}

void Parser::OrderSpec(unique_ptr<ConstantOrderSpec>& orderSpec) {
		Expect(12 /* "extends" */);
		orderSpec = new ConstantOrderSpec(); 
		if (la->kind == _ident || la->kind == 9 /* "unique" */) {
			OrderSpecParent(orderSpec);
			while (la->kind == 13 /* "," */) {
				Get();
				OrderSpecParent(orderSpec);
			}
		}
		if (la->kind == 14 /* "complete" */) {
			Get();
			orderSpec->ChildrenComplete = true; 
		}
}

void Parser::OrderSpecParent(ConstantOrderSpec& orderSpec ) {
		bool unique = false;  
		if (la->kind == 9 /* "unique" */) {
			Get();
			unique = true; 
		}
		Expect(_ident);
		orderSpec.parents.push_back(new ConstantParent (new IdentifierExpr(t, t.val), unique)); 
}

void Parser::IdsTypeWheres(bool allowWhereClauses, const string& context, System.Action<TypedIdent> action ) {
		IdsTypeWhere(allowWhereClauses, context, action);
		while (la->kind == 13 /* "," */) {
			Get();
			IdsTypeWhere(allowWhereClauses, context, action);
		}
}

void Parser::IdsTypeWhere(bool allowWhereClauses, string context, System.Action<TypedIdent> action ) {
		List<IToken>/*!*/ ids;  Bpl.Type/*!*/ ty;  Expr wh = null;  Expr/*!*/ nne; 
		Identifiers(out ids);
		Expect(10 /* ":" */);
		Type(out ty);
		if (la->kind == 16 /* "where" */) {
			Get();
			Expression(out nne);
			if (!allowWhereClauses) {
			 this.SemErr("where clause not allowed on " + context);
			} else {
			 wh = nne;
			}
			
		}
		foreach(Token/*!*/ id in ids){
		 Contract.Assert(id != null);
		 action(new TypedIdent(id, id.val, ty, wh));
		}
		
}

void Parser::Expression(out Expr/*!*/ e0) {
		Contract.Ensures(Contract.ValueAtReturn(out e0) != null); IToken/*!*/ x; Expr/*!*/ e1; 
		ImpliesExpression(false, out e0);
		while (la->kind == 56 /* "<==>" */ || la->kind == 57 /* "\u21d4" */) {
			EquivOp();
			x = t; 
			ImpliesExpression(false, out e1);
			e0 = Expr.Binary(x, BinaryOperator.Opcode.Iff, e0, e1); 
		}
}

void Parser::LocalVars(List<Variable>/*!*/ ds) {
		Contract.Ensures(Contract.ValueAtReturn(out ds) != null);
		QKeyValue kv = null;
		
		Expect(15 /* "var" */);
		while (la->kind == 28 /* "{" */) {
			Attributes(ref kv);
		}
		IdsTypeWheres(true, "local variables", delegate(TypedIdent tyd) { ds.Add(new LocalVariable(tyd.tok, tyd, kv)); } );
		Expect(11 /* ";" */);
}

void Parser::ProcFormals(bool incoming, bool allowWhereClauses, out List<Variable>/*!*/ ds) {
		Contract.Ensures(Contract.ValueAtReturn(out ds) != null);
		ds = new List<Variable>();
		var dsx = ds;
		var context = allowWhereClauses ? "procedure formals" : "the 'implementation' copies of formals";
		
		Expect(17 /* "(" */);
		if (la->kind == _ident || la->kind == 28 /* "{" */) {
			AttrsIdsTypeWheres(allowWhereClauses, allowWhereClauses, context, delegate(TypedIdent tyd, QKeyValue kv) { dsx.Add(new Formal(tyd.tok, tyd, incoming, kv)); });
		}
		Expect(18 /* ")" */);
}

void Parser::AttrsIdsTypeWheres(bool allowAttributes, bool allowWhereClauses, string context, System.Action<TypedIdent, QKeyValue> action ) {
		AttributesIdsTypeWhere(allowAttributes, allowWhereClauses, context, action);
		while (la->kind == 13 /* "," */) {
			Get();
			AttributesIdsTypeWhere(allowAttributes, allowWhereClauses, context, action);
		}
}

void Parser::BoundVars(IToken/*!*/ x, out List<Variable>/*!*/ ds) {
		Contract.Requires(x != null);
		Contract.Ensures(Contract.ValueAtReturn(out ds) != null);
		List<TypedIdent>/*!*/ tyds = new List<TypedIdent>();
		ds = new List<Variable>();
		var dsx = ds;
		
		AttrsIdsTypeWheres(true, false, "bound variables", delegate(TypedIdent tyd, QKeyValue kv) { dsx.Add(new BoundVariable(tyd.tok, tyd, kv)); } );
}

void Parser::AttributesIdsTypeWhere(bool allowAttributes, bool allowWhereClauses, string context, System.Action<TypedIdent, QKeyValue> action ) {
		QKeyValue kv = null; 
		while (la->kind == 28 /* "{" */) {
			Attributes(ref kv);
			if (!allowAttributes) {
			kv = null;
			this.SemErr("attributes are not allowed on " + context);
			}
			
		}
		IdsTypeWhere(allowWhereClauses, context, delegate(TypedIdent tyd) { action(tyd, kv); });
}

void Parser::TypeAtom(out Bpl.Type/*!*/ ty) {
		Contract.Ensures(Contract.ValueAtReturn(out ty) != null); ty = dummyType; 
		if (la->kind == 19 /* "int" */) {
			Get();
			ty = new BasicType(t, SimpleType.Int); 
		} else if (la->kind == 20 /* "real" */) {
			Get();
			ty = new BasicType(t, SimpleType.Real); 
		} else if (la->kind == 21 /* "bool" */) {
			Get();
			ty = new BasicType(t, SimpleType.Bool); 
		} else if (la->kind == 17 /* "(" */) {
			Get();
			Type(out ty);
			Expect(18 /* ")" */);
		} else SynErr(102);
}

void Parser::Indetifier(out IToken/*!*/ x) {
		Contract.Ensures(Contract.ValueAtReturn(out x) != null);
		Expect(_ident);
		x = t;
		if (x.val.StartsWith("\\"))
		 x.val = x.val.Substring(1);
		
}

void Parser::TypeArgs(List<Bpl.Type>/*!*/ ts) {
		Contract.Requires(ts != null); IToken/*!*/ tok; Bpl.Type/*!*/ ty; 
		if (StartOf(5)) {
			TypeAtom(out ty);
			ts.Add(ty); 
			if (StartOf(6)) {
				TypeArgs(ts);
			}
		} else if (la->kind == _ident) {
			Indetifier(out tok);
			List<Bpl.Type>/*!*/ args = new List<Bpl.Type> ();
			ts.Add(new UnresolvedTypeIdentifier (tok, tok.val, args)); 
			if (StartOf(6)) {
				TypeArgs(ts);
			}
		} else if (la->kind == 22 /* "[" */ || la->kind == 24 /* "<" */) {
			MapType(out ty);
			ts.Add(ty); 
		} else SynErr(103);
}

void Parser::MapType(out Bpl.Type/*!*/ ty) {
		Contract.Ensures(Contract.ValueAtReturn(out ty) != null); IToken tok = null;
		IToken/*!*/ nnTok;
		List<Bpl.Type>/*!*/ arguments = new List<Bpl.Type>();
		Bpl.Type/*!*/ result;
		List<TypeVariable>/*!*/ typeParameters = new List<TypeVariable>();
		
		if (la->kind == 24 /* "<" */) {
			TypeParams(out nnTok, out typeParameters);
			tok = nnTok; 
		}
		Expect(22 /* "[" */);
		if (tok == null) tok = t;  
		if (StartOf(6)) {
			Types(arguments);
		}
		Expect(23 /* "]" */);
		Type(out result);
		ty = new MapType(tok, typeParameters, arguments, result);
		
}

void Parser::TypeParams(out IToken/*!*/ tok, out List<TypeVariable>/*!*/ typeParams) {
		Contract.Ensures(Contract.ValueAtReturn(out tok) != null); Contract.Ensures(Contract.ValueAtReturn(out typeParams) != null); List<IToken>/*!*/ typeParamToks; 
		Expect(24 /* "<" */);
		tok = t;  
		Identifiers(out typeParamToks);
		Expect(25 /* ">" */);
		typeParams = new List<TypeVariable> ();
		foreach(Token/*!*/ id in typeParamToks){
		 Contract.Assert(id != null);
		 typeParams.Add(new TypeVariable(id, id.val));}
		
}

void Parser::Types(List<Bpl.Type>/*!*/ ts) {
		Contract.Requires(ts != null); Bpl.Type/*!*/ ty; 
		Type(out ty);
		ts.Add(ty); 
		while (la->kind == 13 /* "," */) {
			Get();
			Type(out ty);
			ts.Add(ty); 
		}
}

void Parser::VarOrType(out TypedIdent/*!*/ tyd, out QKeyValue kv) {
		Contract.Ensures(Contract.ValueAtReturn(out tyd) != null);
		string/*!*/ varName = TypedIdent.NoName;
		Bpl.Type/*!*/ ty;
		IToken/*!*/ tok;
		kv = null;
		
		while (la->kind == 28 /* "{" */) {
			Attributes(ref kv);
		}
		Type(out ty);
		tok = ty.tok; 
		if (la->kind == 10 /* ":" */) {
			Get();
			var uti = ty as UnresolvedTypeIdentifier;
			if (uti != null && uti.Arguments.Count == 0) {
			 varName = uti.Name;
			} else {
			 this.SemErr("expected identifier before ':'");
			}
			
			Type(out ty);
		}
		tyd = new TypedIdent(tok, varName, ty); 
}

void Parser::Proposition(out Expr/*!*/ e) {
		Contract.Ensures(Contract.ValueAtReturn(out e) != null);
		Expression(out e);
}

void Parser::UserDefinedType(out Declaration/*!*/ decl, QKeyValue kv) {
		Contract.Ensures(Contract.ValueAtReturn(out decl) != null); 
		Token/*!*/ id; List<IToken>/*!*/ paramTokens = new List<IToken> ();
		Bpl.Type/*!*/ body = dummyType; bool synonym = false; 
		Indetifier(out id);
		if (la->kind == _ident) {
			WhiteSpaceIdentifiers(out paramTokens);
		}
		if (la->kind == 32 /* "=" */) {
			Get();
			Type(out body);
			synonym = true; 
		}
		if (synonym) {
		 List<TypeVariable>/*!*/ typeParams = new List<TypeVariable>();
		 foreach(Token/*!*/ t in paramTokens){
		   Contract.Assert(t != null);
		   typeParams.Add(new TypeVariable(t, t.val));}
		 decl = new TypeSynonymDecl(id, id.val, typeParams, body, kv);
		} else {
		 decl = new TypeCtorDecl(id, id.val, paramTokens.Count, kv);
		}
		
}

void Parser::WhiteSpaceIdentifiers(out List<IToken>/*!*/ xs) {
		Contract.Ensures(Contract.ValueAtReturn(out xs) != null); IToken/*!*/ id; xs = new List<IToken>(); 
		Indetifier(out id);
		xs.Add(id); 
		while (la->kind == _ident) {
			Indetifier(out id);
			xs.Add(id); 
		}
}

void Parser::ProcSignature(bool allowWhereClausesOnFormals, out IToken/*!*/ name, out List<TypeVariable>/*!*/ typeParams,
out List<Variable>/*!*/ ins, out List<Variable>/*!*/ outs, out QKeyValue kv) {
		Contract.Ensures(Contract.ValueAtReturn(out name) != null); Contract.Ensures(Contract.ValueAtReturn(out typeParams) != null); Contract.Ensures(Contract.ValueAtReturn(out ins) != null); Contract.Ensures(Contract.ValueAtReturn(out outs) != null);
		IToken/*!*/ typeParamTok; typeParams = new List<TypeVariable>();
		outs = new List<Variable>(); kv = null; 
		while (la->kind == 28 /* "{" */) {
			Attributes(ref kv);
		}
		Indetifier(out name);
		if (la->kind == 24 /* "<" */) {
			TypeParams(out typeParamTok, out typeParams);
		}
		ProcFormals(true, allowWhereClausesOnFormals, out ins);
		if (la->kind == 27 /* "returns" */) {
			Get();
			ProcFormals(false, allowWhereClausesOnFormals, out outs);
		}
}

void Parser::Spec(List<Requires>/*!*/ pre, List<IdentifierExpr>/*!*/ mods, List<Ensures>/*!*/ post) {
		Contract.Requires(pre != null); Contract.Requires(mods != null); Contract.Requires(post != null); List<IToken>/*!*/ ms; 
		if (la->kind == 35 /* "modifies" */) {
			Get();
			if (la->kind == _ident) {
				Identifiers(out ms);
				foreach(IToken/*!*/ m in ms){
				Contract.Assert(m != null);
				mods.Add(new IdentifierExpr(m, m.val));
				}
				
			}
			Expect(11 /* ";" */);
		} else if (la->kind == 36 /* "free" */) {
			Get();
			SpecPrePost(true, pre, post);
		} else if (la->kind == 37 /* "requires" */ || la->kind == 38 /* "ensures" */) {
			SpecPrePost(false, pre, post);
		} else SynErr(104);
}

void Parser::ImplBody(out List<Variable>/*!*/ locals, out StmtList/*!*/ stmtList) {
		Contract.Ensures(Contract.ValueAtReturn(out locals) != null); Contract.Ensures(Contract.ValueAtReturn(out stmtList) != null); locals = new List<Variable>(); 
		Expect(28 /* "{" */);
		while (la->kind == 15 /* "var" */) {
			LocalVars(locals);
		}
		StmtList(out stmtList);
}

void Parser::SpecPrePost(bool free, List<Requires>/*!*/ pre, List<Ensures>/*!*/ post) {
		Contract.Requires(pre != null); Contract.Requires(post != null); Expr/*!*/ e; Token tok = null; QKeyValue kv = null; 
		if (la->kind == 37 /* "requires" */) {
			Get();
			tok = t; 
			while (la->kind == 28 /* "{" */) {
				Attributes(ref kv);
			}
			Proposition(out e);
			Expect(11 /* ";" */);
			pre.Add(new Requires(tok, free, e, null, kv)); 
		} else if (la->kind == 38 /* "ensures" */) {
			Get();
			tok = t; 
			while (la->kind == 28 /* "{" */) {
				Attributes(ref kv);
			}
			Proposition(out e);
			Expect(11 /* ";" */);
			post.Add(new Ensures(tok, free, e, null, kv)); 
		} else SynErr(105);
}

void Parser::StmtList(out StmtList/*!*/ stmtList) {
		Contract.Ensures(Contract.ValueAtReturn(out stmtList) != null); List<BigBlock/*!*/> bigblocks = new List<BigBlock/*!*/>();
		/* built-up state for the current BigBlock: */
		IToken startToken = null;  string currentLabel = null;
		List<Cmd> cs = null;  /* invariant: startToken != null ==> cs != null */
		/* temporary variables: */
		IToken label;  Cmd c;  BigBlock b;
		StructuredCmd ec = null;  StructuredCmd/*!*/ ecn;
		TransferCmd tc = null;  TransferCmd/*!*/ tcn;
		
		while (StartOf(7)) {
			if (StartOf(8)) {
				LabelOrCmd(out c, out label);
				if (c != null) {
				 // LabelOrCmd read a Cmd
				 Contract.Assert(label == null);
				 if (startToken == null) { startToken = c.tok;  cs = new List<Cmd>(); }
				 Contract.Assert(cs != null);
				 cs.Add(c);
				} else {
				 // LabelOrCmd read a label
				 Contract.Assert(label != null);
				 if (startToken != null) {
				   Contract.Assert(cs != null);
				   // dump the built-up state into a BigBlock
				   b = new BigBlock(startToken, currentLabel, cs, null, null);
				   bigblocks.Add(b);
				   cs = null;
				 }
				 startToken = label;
				 currentLabel = label.val;
				 cs = new List<Cmd>();
				}
				
			} else if (la->kind == 41 /* "if" */ || la->kind == 43 /* "while" */ || la->kind == 46 /* "break" */) {
				StructuredCmd(out ecn);
				ec = ecn;
				if (startToken == null) { startToken = ec.tok;  cs = new List<Cmd>(); }
				Contract.Assert(cs != null);
				b = new BigBlock(startToken, currentLabel, cs, ec, null);
				bigblocks.Add(b);
				startToken = null;  currentLabel = null;  cs = null;
				
			} else {
				TransferCmd(out tcn);
				tc = tcn;
				if (startToken == null) { startToken = tc.tok;  cs = new List<Cmd>(); }
				Contract.Assert(cs != null);
				b = new BigBlock(startToken, currentLabel, cs, null, tc);
				bigblocks.Add(b);
				startToken = null;  currentLabel = null;  cs = null;
				
			}
		}
		Expect(29 /* "}" */);
		IToken/*!*/ endCurly = t;
		if (startToken == null && bigblocks.Count == 0) {
		 startToken = t;  cs = new List<Cmd>();
		}
		if (startToken != null) {
		 Contract.Assert(cs != null);
		 b = new BigBlock(startToken, currentLabel, cs, null, null);
		 bigblocks.Add(b);
		}
		
		stmtList = new StmtList(bigblocks, endCurly);
		
}

void Parser::LabelOrCmd(out Cmd c, out IToken label) {
		IToken/*!*/ x; Expr/*!*/ e;
		List<IToken>/*!*/ xs;
		List<IdentifierExpr> ids;
		c = dummyCmd;  label = null;
		Cmd/*!*/ cn;
		QKeyValue kv = null;
		
		switch (la->kind) {
		case _ident: {
			LabelOrAssign(out c, out label);
			break;
		}
		case 47 /* "assert" */: {
			Get();
			x = t; 
			while (la->kind == 28 /* "{" */) {
				Attributes(ref kv);
			}
			Proposition(out e);
			c = new AssertCmd(x, e, kv); 
			Expect(11 /* ";" */);
			break;
		}
		case 48 /* "assume" */: {
			Get();
			x = t; 
			while (la->kind == 28 /* "{" */) {
				Attributes(ref kv);
			}
			Proposition(out e);
			c = new AssumeCmd(x, e, kv); 
			Expect(11 /* ";" */);
			break;
		}
		case 49 /* "havoc" */: {
			Get();
			x = t; 
			Identifiers(out xs);
			Expect(11 /* ";" */);
			ids = new List<IdentifierExpr>();
			foreach(IToken/*!*/ y in xs){
			Contract.Assert(y != null);
			ids.Add(new IdentifierExpr(y, y.val));
			}
			c = new HavocCmd(x,ids);
			
			break;
		}
		case 36 /* "free" */: case 52 /* "async" */: case 53 /* "call" */: {
			CallCmd(out cn);
			Expect(11 /* ";" */);
			c = cn; 
			break;
		}
		case 54 /* "par" */: {
			ParCallCmd(out cn);
			c = cn; 
			break;
		}
		case 50 /* "yield" */: {
			Get();
			x = t; 
			Expect(11 /* ";" */);
			c = new YieldCmd(x); 
			break;
		}
		default: SynErr(106); break;
		}
}

void Parser::StructuredCmd(out StructuredCmd/*!*/ ec) {
		Contract.Ensures(Contract.ValueAtReturn(out ec) != null); ec = dummyStructuredCmd;  Contract.Assume(cce.IsPeerConsistent(ec));
		IfCmd/*!*/ ifcmd;  WhileCmd/*!*/ wcmd;  BreakCmd/*!*/ bcmd;
		
		if (la->kind == 41 /* "if" */) {
			IfCmd(out ifcmd);
			ec = ifcmd; 
		} else if (la->kind == 43 /* "while" */) {
			WhileCmd(out wcmd);
			ec = wcmd; 
		} else if (la->kind == 46 /* "break" */) {
			BreakCmd(out bcmd);
			ec = bcmd; 
		} else SynErr(107);
}

void Parser::TransferCmd(out TransferCmd/*!*/ tc) {
		Contract.Ensures(Contract.ValueAtReturn(out tc) != null); tc = dummyTransferCmd;
		Token y;  List<IToken>/*!*/ xs;
		List<String> ss = new List<String>();
		
		if (la->kind == 39 /* "goto" */) {
			Get();
			y = t; 
			Identifiers(out xs);
			foreach(IToken/*!*/ s in xs){
			Contract.Assert(s != null);
			ss.Add(s.val); }
			tc = new GotoCmd(y, ss);
			
		} else if (la->kind == 40 /* "return" */) {
			Get();
			tc = new ReturnCmd(t); 
		} else SynErr(108);
		Expect(11 /* ";" */);
}

void Parser::IfCmd(out IfCmd/*!*/ ifcmd) {
		Contract.Ensures(Contract.ValueAtReturn(out ifcmd) != null); IToken/*!*/ x;
		Expr guard;
		StmtList/*!*/ thn;
		IfCmd/*!*/ elseIf;  IfCmd elseIfOption = null;
		StmtList/*!*/ els;  StmtList elseOption = null;
		
		Expect(41 /* "if" */);
		x = t; 
		Guard(out guard);
		Expect(28 /* "{" */);
		StmtList(out thn);
		if (la->kind == 42 /* "else" */) {
			Get();
			if (la->kind == 41 /* "if" */) {
				IfCmd(out elseIf);
				elseIfOption = elseIf; 
			} else if (la->kind == 28 /* "{" */) {
				Get();
				StmtList(out els);
				elseOption = els; 
			} else SynErr(109);
		}
		ifcmd = new IfCmd(x, guard, thn, elseIfOption, elseOption); 
}

void Parser::WhileCmd(out WhileCmd/*!*/ wcmd) {
		Contract.Ensures(Contract.ValueAtReturn(out wcmd) != null); IToken/*!*/ x;  Token z;
		Expr guard;  Expr/*!*/ e;  bool isFree;
		List<PredicateCmd/*!*/> invariants = new List<PredicateCmd/*!*/>();
		StmtList/*!*/ body;
		QKeyValue kv = null;
		
		Expect(43 /* "while" */);
		x = t; 
		Guard(out guard);
		Contract.Assume(guard == null || cce.Owner.None(guard)); 
		while (la->kind == 36 /* "free" */ || la->kind == 44 /* "invariant" */) {
			isFree = false; z = la/*lookahead token*/; 
			if (la->kind == 36 /* "free" */) {
				Get();
				isFree = true;  
			}
			Expect(44 /* "invariant" */);
			while (la->kind == 28 /* "{" */) {
				Attributes(ref kv);
			}
			Expression(out e);
			if (isFree) {
			 invariants.Add(new AssumeCmd(z, e, kv));
			} else {
			 invariants.Add(new AssertCmd(z, e, kv));
			}
			kv = null;
			
			Expect(11 /* ";" */);
		}
		Expect(28 /* "{" */);
		StmtList(out body);
		wcmd = new WhileCmd(x, guard, invariants, body); 
}

void Parser::BreakCmd(out BreakCmd/*!*/ bcmd) {
		Contract.Ensures(Contract.ValueAtReturn(out bcmd) != null); IToken/*!*/ x;  IToken/*!*/ y;
		string breakLabel = null;
		
		Expect(46 /* "break" */);
		x = t; 
		if (la->kind == _ident) {
			Indetifier(out y);
			breakLabel = y.val; 
		}
		Expect(11 /* ";" */);
		bcmd = new BreakCmd(x, breakLabel); 
}

void Parser::Guard(out Expr e) {
		Expr/*!*/ ee;  e = null; 
		Expect(17 /* "(" */);
		if (la->kind == 45 /* "*" */) {
			Get();
			e = null; 
		} else if (StartOf(9)) {
			Expression(out ee);
			e = ee; 
		} else SynErr(110);
		Expect(18 /* ")" */);
}

void Parser::LabelOrAssign(out Cmd c, out IToken label) {
		IToken/*!*/ id; IToken/*!*/ x, y; Expr/*!*/ e0;
		c = dummyCmd;  label = null;
		AssignLhs/*!*/ lhs;
		List<AssignLhs/*!*/>/*!*/ lhss;
		List<Expr/*!*/>/*!*/ rhss;
		List<Expr/*!*/>/*!*/ indexes;
		
		Indetifier(out id);
		x = t; 
		if (la->kind == 10 /* ":" */) {
			Get();
			c = null;  label = x; 
		} else if (la->kind == 13 /* "," */ || la->kind == 22 /* "[" */ || la->kind == 51 /* ":=" */) {
			lhss = new List<AssignLhs/*!*/>(); 
			lhs = new SimpleAssignLhs(id, new IdentifierExpr(id, id.val)); 
			while (la->kind == 22 /* "[" */) {
				MapAssignIndex(out y, out indexes);
				lhs = new MapAssignLhs(y, lhs, indexes); 
			}
			lhss.Add(lhs); 
			while (la->kind == 13 /* "," */) {
				Get();
				Indetifier(out id);
				lhs = new SimpleAssignLhs(id, new IdentifierExpr(id, id.val)); 
				while (la->kind == 22 /* "[" */) {
					MapAssignIndex(out y, out indexes);
					lhs = new MapAssignLhs(y, lhs, indexes); 
				}
				lhss.Add(lhs); 
			}
			Expect(51 /* ":=" */);
			x = t; /* use location of := */ 
			Expression(out e0);
			rhss = new List<Expr/*!*/> ();
			rhss.Add(e0); 
			while (la->kind == 13 /* "," */) {
				Get();
				Expression(out e0);
				rhss.Add(e0); 
			}
			Expect(11 /* ";" */);
			c = new AssignCmd(x, lhss, rhss); 
		} else SynErr(111);
}

void Parser::CallCmd(out Cmd c) {
		Contract.Ensures(Contract.ValueAtReturn(out c) != null); 
		IToken x; 
		bool isAsync = false;
		bool isFree = false;
		QKeyValue kv = null;
		c = null;
		
		if (la->kind == 52 /* "async" */) {
			Get();
			isAsync = true;  
		}
		if (la->kind == 36 /* "free" */) {
			Get();
			isFree = true;  
		}
		Expect(53 /* "call" */);
		x = t; 
		while (la->kind == 28 /* "{" */) {
			Attributes(ref kv);
		}
		CallParams(isAsync, isFree, kv, x, out c);
		
}

void Parser::ParCallCmd(out Cmd d) {
		Contract.Ensures(Contract.ValueAtReturn(out d) != null); 
		IToken x; 
		QKeyValue kv = null;
		Cmd c = null;
		List<CallCmd> callCmds = new List<CallCmd>();
		
		Expect(54 /* "par" */);
		x = t; 
		while (la->kind == 28 /* "{" */) {
			Attributes(ref kv);
		}
		CallParams(false, false, kv, x, out c);
		callCmds.Add((CallCmd)c); 
		while (la->kind == 55 /* "|" */) {
			Get();
			CallParams(false, false, kv, x, out c);
			callCmds.Add((CallCmd)c); 
		}
		Expect(11 /* ";" */);
		d = new ParCallCmd(x, callCmds, kv); 
}

void Parser::MapAssignIndex(out IToken/*!*/ x, out List<Expr/*!*/>/*!*/ indexes) {
		Contract.Ensures(Contract.ValueAtReturn(out x) != null); Contract.Ensures(cce.NonNullElements(Contract.ValueAtReturn(out indexes))); indexes = new List<Expr/*!*/> ();
		Expr/*!*/ e;
		
		Expect(22 /* "[" */);
		x = t; 
		if (StartOf(9)) {
			Expression(out e);
			indexes.Add(e); 
			while (la->kind == 13 /* "," */) {
				Get();
				Expression(out e);
				indexes.Add(e); 
			}
		}
		Expect(23 /* "]" */);
}

void Parser::CallParams(bool isAsync, bool isFree, QKeyValue kv, IToken x, out Cmd c) {
		List<IdentifierExpr> ids = new List<IdentifierExpr>();
		List<Expr> es = new List<Expr>();
		Expr en;
		IToken first; 
		IToken p;
		c = null;
		
		Indetifier(out first);
		if (la->kind == 17 /* "(" */) {
			Get();
			if (StartOf(9)) {
				Expression(out en);
				es.Add(en); 
				while (la->kind == 13 /* "," */) {
					Get();
					Expression(out en);
					es.Add(en); 
				}
			}
			Expect(18 /* ")" */);
			c = new CallCmd(x, first.val, es, ids, kv); ((CallCmd) c).IsFree = isFree; ((CallCmd) c).IsAsync = isAsync; 
		} else if (la->kind == 13 /* "," */ || la->kind == 51 /* ":=" */) {
			ids.Add(new IdentifierExpr(first, first.val)); 
			if (la->kind == 13 /* "," */) {
				Get();
				Indetifier(out p);
				ids.Add(new IdentifierExpr(p, p.val)); 
				while (la->kind == 13 /* "," */) {
					Get();
					Indetifier(out p);
					ids.Add(new IdentifierExpr(p, p.val)); 
				}
			}
			Expect(51 /* ":=" */);
			Indetifier(out first);
			Expect(17 /* "(" */);
			if (StartOf(9)) {
				Expression(out en);
				es.Add(en); 
				while (la->kind == 13 /* "," */) {
					Get();
					Expression(out en);
					es.Add(en); 
				}
			}
			Expect(18 /* ")" */);
			c = new CallCmd(x, first.val, es, ids, kv); ((CallCmd) c).IsFree = isFree; ((CallCmd) c).IsAsync = isAsync; 
		} else SynErr(112);
}

void Parser::Expressions(out List<Expr>/*!*/ es) {
		Contract.Ensures(Contract.ValueAtReturn(out es) != null); Expr/*!*/ e; es = new List<Expr>(); 
		Expression(out e);
		es.Add(e); 
		while (la->kind == 13 /* "," */) {
			Get();
			Expression(out e);
			es.Add(e); 
		}
}

void Parser::ImpliesExpression(bool noExplies, out Expr/*!*/ e0) {
		Contract.Ensures(Contract.ValueAtReturn(out e0) != null); IToken/*!*/ x; Expr/*!*/ e1; 
		LogicalExpression(out e0);
		if (StartOf(10)) {
			if (la->kind == 58 /* "==>" */ || la->kind == 59 /* "\u21d2" */) {
				ImpliesOp();
				x = t; 
				ImpliesExpression(true, out e1);
				e0 = Expr.Binary(x, BinaryOperator.Opcode.Imp, e0, e1); 
			} else {
				ExpliesOp();
				if (noExplies)
				 this.SemErr("illegal mixture of ==> and <==, use parentheses to disambiguate");
				x = t; 
				LogicalExpression(out e1);
				e0 = Expr.Binary(x, BinaryOperator.Opcode.Imp, e1, e0); 
				while (la->kind == 60 /* "<==" */ || la->kind == 61 /* "\u21d0" */) {
					ExpliesOp();
					x = t; 
					LogicalExpression(out e1);
					e0 = Expr.Binary(x, BinaryOperator.Opcode.Imp, e1, e0); 
				}
			}
		}
}

void Parser::EquivOp() {
		if (la->kind == 56 /* "<==>" */) {
			Get();
		} else if (la->kind == 57 /* "\u21d4" */) {
			Get();
		} else SynErr(113);
}

void Parser::LogicalExpression(out Expr/*!*/ e0) {
		Contract.Ensures(Contract.ValueAtReturn(out e0) != null); IToken/*!*/ x; Expr/*!*/ e1; 
		RelationalExpression(out e0);
		if (StartOf(11)) {
			if (la->kind == 62 /* "&&" */ || la->kind == 63 /* "\u2227" */) {
				AndOp();
				x = t; 
				RelationalExpression(out e1);
				e0 = Expr.Binary(x, BinaryOperator.Opcode.And, e0, e1); 
				while (la->kind == 62 /* "&&" */ || la->kind == 63 /* "\u2227" */) {
					AndOp();
					x = t; 
					RelationalExpression(out e1);
					e0 = Expr.Binary(x, BinaryOperator.Opcode.And, e0, e1); 
				}
			} else {
				OrOp();
				x = t; 
				RelationalExpression(out e1);
				e0 = Expr.Binary(x, BinaryOperator.Opcode.Or, e0, e1); 
				while (la->kind == 64 /* "||" */ || la->kind == 65 /* "\u2228" */) {
					OrOp();
					x = t; 
					RelationalExpression(out e1);
					e0 = Expr.Binary(x, BinaryOperator.Opcode.Or, e0, e1); 
				}
			}
		}
}

void Parser::ImpliesOp() {
		if (la->kind == 58 /* "==>" */) {
			Get();
		} else if (la->kind == 59 /* "\u21d2" */) {
			Get();
		} else SynErr(114);
}

void Parser::ExpliesOp() {
		if (la->kind == 60 /* "<==" */) {
			Get();
		} else if (la->kind == 61 /* "\u21d0" */) {
			Get();
		} else SynErr(115);
}

void Parser::RelationalExpression(out Expr/*!*/ e0) {
		Contract.Ensures(Contract.ValueAtReturn(out e0) != null); IToken/*!*/ x; Expr/*!*/ e1; BinaryOperator.Opcode op; 
		BvTerm(out e0);
		if (StartOf(12)) {
			RelOp(out x, out op);
			BvTerm(out e1);
			e0 = Expr.Binary(x, op, e0, e1); 
		}
}

void Parser::AndOp() {
		if (la->kind == 62 /* "&&" */) {
			Get();
		} else if (la->kind == 63 /* "\u2227" */) {
			Get();
		} else SynErr(116);
}

void Parser::OrOp() {
		if (la->kind == 64 /* "||" */) {
			Get();
		} else if (la->kind == 65 /* "\u2228" */) {
			Get();
		} else SynErr(117);
}

void Parser::BvTerm(out Expr/*!*/ e0) {
		Contract.Ensures(Contract.ValueAtReturn(out e0) != null); IToken/*!*/ x; Expr/*!*/ e1; 
		Term(out e0);
		while (la->kind == 74 /* "++" */) {
			Get();
			x = t; 
			Term(out e1);
			e0 = new BvConcatExpr(x, e0, e1); 
		}
}

void Parser::RelOp(out IToken/*!*/ x, out BinaryOperator.Opcode op) {
		Contract.Ensures(Contract.ValueAtReturn(out x) != null); x = Token.NoToken; op=BinaryOperator.Opcode.Add/*(dummy)*/; 
		switch (la->kind) {
		case 66 /* "==" */: {
			Get();
			x = t; op=BinaryOperator.Opcode.Eq; 
			break;
		}
		case 24 /* "<" */: {
			Get();
			x = t; op=BinaryOperator.Opcode.Lt; 
			break;
		}
		case 25 /* ">" */: {
			Get();
			x = t; op=BinaryOperator.Opcode.Gt; 
			break;
		}
		case 67 /* "<=" */: {
			Get();
			x = t; op=BinaryOperator.Opcode.Le; 
			break;
		}
		case 68 /* ">=" */: {
			Get();
			x = t; op=BinaryOperator.Opcode.Ge; 
			break;
		}
		case 69 /* "!=" */: {
			Get();
			x = t; op=BinaryOperator.Opcode.Neq; 
			break;
		}
		case 70 /* "<:" */: {
			Get();
			x = t; op=BinaryOperator.Opcode.Subtype; 
			break;
		}
		case 71 /* "\u2260" */: {
			Get();
			x = t; op=BinaryOperator.Opcode.Neq; 
			break;
		}
		case 72 /* "\u2264" */: {
			Get();
			x = t; op=BinaryOperator.Opcode.Le; 
			break;
		}
		case 73 /* "\u2265" */: {
			Get();
			x = t; op=BinaryOperator.Opcode.Ge; 
			break;
		}
		default: SynErr(118); break;
		}
}

void Parser::Term(out Expr/*!*/ e0) {
		Contract.Ensures(Contract.ValueAtReturn(out e0) != null); IToken/*!*/ x; Expr/*!*/ e1; BinaryOperator.Opcode op; 
		Factor(out e0);
		while (la->kind == 75 /* "+" */ || la->kind == 76 /* "-" */) {
			AddOp(out x, out op);
			Factor(out e1);
			e0 = Expr.Binary(x, op, e0, e1); 
		}
}

void Parser::Factor(out Expr/*!*/ e0) {
		Contract.Ensures(Contract.ValueAtReturn(out e0) != null); IToken/*!*/ x; Expr/*!*/ e1; BinaryOperator.Opcode op; 
		Power(out e0);
		while (StartOf(13)) {
			MulOp(out x, out op);
			Power(out e1);
			e0 = Expr.Binary(x, op, e0, e1); 
		}
}

void Parser::AddOp(out IToken/*!*/ x, out BinaryOperator.Opcode op) {
		Contract.Ensures(Contract.ValueAtReturn(out x) != null); x = Token.NoToken; op=BinaryOperator.Opcode.Add/*(dummy)*/; 
		if (la->kind == 75 /* "+" */) {
			Get();
			x = t; op=BinaryOperator.Opcode.Add; 
		} else if (la->kind == 76 /* "-" */) {
			Get();
			x = t; op=BinaryOperator.Opcode.Sub; 
		} else SynErr(119);
}

void Parser::Power(out Expr/*!*/ e0) {
		Contract.Ensures(Contract.ValueAtReturn(out e0) != null); IToken/*!*/ x; Expr/*!*/ e1; 
		UnaryExpression(out e0);
		if (la->kind == 80 /* "**" */) {
			Get();
			x = t; 
			Power(out e1);
			e0 = Expr.Binary(x, BinaryOperator.Opcode.Pow, e0, e1); 
		}
}

void Parser::MulOp(out IToken/*!*/ x, out BinaryOperator.Opcode op) {
		Contract.Ensures(Contract.ValueAtReturn(out x) != null); x = Token.NoToken; op=BinaryOperator.Opcode.Add/*(dummy)*/; 
		if (la->kind == 45 /* "*" */) {
			Get();
			x = t; op=BinaryOperator.Opcode.Mul; 
		} else if (la->kind == 77 /* "div" */) {
			Get();
			x = t; op=BinaryOperator.Opcode.Div; 
		} else if (la->kind == 78 /* "mod" */) {
			Get();
			x = t; op=BinaryOperator.Opcode.Mod; 
		} else if (la->kind == 79 /* "/" */) {
			Get();
			x = t; op=BinaryOperator.Opcode.RealDiv; 
		} else SynErr(120);
}

void Parser::UnaryExpression(out Expr/*!*/ e) {
		Contract.Ensures(Contract.ValueAtReturn(out e) != null); IToken/*!*/ x;
		e = dummyExpr;
		
		if (la->kind == 76 /* "-" */) {
			Get();
			x = t; 
			UnaryExpression(out e);
			e = Expr.Unary(x, UnaryOperator.Opcode.Neg, e); 
		} else if (la->kind == 81 /* "!" */ || la->kind == 82 /* "\u00ac" */) {
			NegOp();
			x = t; 
			UnaryExpression(out e);
			e = Expr.Unary(x, UnaryOperator.Opcode.Not, e); 
		} else if (StartOf(14)) {
			CoercionExpression(out e);
		} else SynErr(121);
}

void Parser::NegOp() {
		if (la->kind == 81 /* "!" */) {
			Get();
		} else if (la->kind == 82 /* "\u00ac" */) {
			Get();
		} else SynErr(122);
}

void Parser::CoercionExpression(out Expr/*!*/ e) {
		Contract.Ensures(Contract.ValueAtReturn(out e) != null); IToken/*!*/ x;
		Bpl.Type/*!*/ coercedTo;
		BigNum bn;
		
		ArrayExpression(out e);
		while (la->kind == 10 /* ":" */) {
			Get();
			x = t; 
			if (StartOf(6)) {
				Type(out coercedTo);
				e = Expr.CoerceType(x, e, coercedTo); 
			} else if (la->kind == _digits) {
				Nat(out bn);
				if (!(e is LiteralExpr) || !((LiteralExpr)e).isBigNum) {
				 this.SemErr("arguments of extract need to be integer literals");
				 e = new BvBounds(x, bn, BigNum.ZERO);
				} else {
				 e = new BvBounds(x, bn, ((LiteralExpr)e).asBigNum);
				}
				
			} else SynErr(123);
		}
}

void Parser::ArrayExpression(out Expr/*!*/ e) {
		Contract.Ensures(Contract.ValueAtReturn(out e) != null); IToken/*!*/ x;
		Expr/*!*/ index0 = dummyExpr; Expr/*!*/ e1;
		bool store; bool bvExtract;
		List<Expr>/*!*/ allArgs = dummyExprSeq;
		
		AtomExpression(out e);
		while (la->kind == 22 /* "[" */) {
			Get();
			x = t; allArgs = new List<Expr> ();
			allArgs.Add(e);
			store = false; bvExtract = false; 
			if (StartOf(15)) {
				if (StartOf(9)) {
					Expression(out index0);
					if (index0 is BvBounds)
					 bvExtract = true;
					else
					 allArgs.Add(index0);
					
					while (la->kind == 13 /* "," */) {
						Get();
						Expression(out e1);
						if (bvExtract || e1 is BvBounds)
						 this.SemErr("bitvectors only have one dimension");
						allArgs.Add(e1);
						
					}
					if (la->kind == 51 /* ":=" */) {
						Get();
						Expression(out e1);
						if (bvExtract || e1 is BvBounds)
						 this.SemErr("assignment to bitvectors is not possible");
						allArgs.Add(e1); store = true;
						
					}
				} else {
					Get();
					Expression(out e1);
					allArgs.Add(e1); store = true; 
				}
			}
			Expect(23 /* "]" */);
			if (store)
			 e = new NAryExpr(x, new MapStore(x, allArgs.Count - 2), allArgs);
			else if (bvExtract)
			 e = new BvExtractExpr(x, e,
			                       ((BvBounds)index0).Upper.ToIntSafe,
			                       ((BvBounds)index0).Lower.ToIntSafe);
			else
			 e = new NAryExpr(x, new MapSelect(x, allArgs.Count - 1), allArgs);
			
		}
}

void Parser::Nat(out BigNum n) {
		Expect(_digits);
		try {
		 n = BigNum.FromString(t.val);
		} catch (FormatException) {
		 this.SemErr("incorrectly formatted number");
		 n = BigNum.ZERO;
		}
		
}

void Parser::AtomExpression(out Expr/*!*/ e) {
		Contract.Ensures(Contract.ValueAtReturn(out e) != null); IToken/*!*/ x; int n; BigNum bn; BigDec bd; BigFloat bf;
		List<Expr>/*!*/ es;  List<Variable>/*!*/ ds;  Trigger trig;
		List<TypeVariable>/*!*/ typeParams;
		IdentifierExpr/*!*/ id;
		QKeyValue kv;
		e = dummyExpr;
		List<Variable>/*!*/ locals;
		List<Block/*!*/>/*!*/ blocks;
		
		switch (la->kind) {
		case 83 /* "false" */: {
			Get();
			e = new LiteralExpr(t, false); 
			break;
		}
		case 84 /* "true" */: {
			Get();
			e = new LiteralExpr(t, true); 
			break;
		}
		case _digits: {
			Nat(out bn);
			e = new LiteralExpr(t, bn); 
			break;
		}
		case _decimal: case _dec_float: {
			Dec(out bd);
			e = new LiteralExpr(t, bd); 
			break;
		}
		case _float: {
			Float(out bf);
			e = new LiteralExpr(t, bf); 
			break;
		}
		case _bvlit: {
			BvLit(out bn, out n);
			e = new LiteralExpr(t, bn, n); 
			break;
		}
		case _ident: {
			Indetifier(out x);
			id = new IdentifierExpr(x, x.val);  e = id; 
			if (la->kind == 17 /* "(" */) {
				Get();
				if (StartOf(9)) {
					Expressions(out es);
					e = new NAryExpr(x, new FunctionCall(id), es); 
				} else if (la->kind == 18 /* ")" */) {
					e = new NAryExpr(x, new FunctionCall(id), new List<Expr>()); 
				} else SynErr(124);
				Expect(18 /* ")" */);
			}
			break;
		}
		case 85 /* "old" */: {
			Get();
			x = t; 
			Expect(17 /* "(" */);
			Expression(out e);
			Expect(18 /* ")" */);
			e = new OldExpr(x, e); 
			break;
		}
		case 19 /* "int" */: {
			Get();
			x = t; 
			Expect(17 /* "(" */);
			Expression(out e);
			Expect(18 /* ")" */);
			e = new NAryExpr(x, new ArithmeticCoercion(x, ArithmeticCoercion.CoercionType.ToInt), new List<Expr>{ e }); 
			break;
		}
		case 20 /* "real" */: {
			Get();
			x = t; 
			Expect(17 /* "(" */);
			Expression(out e);
			Expect(18 /* ")" */);
			e = new NAryExpr(x, new ArithmeticCoercion(x, ArithmeticCoercion.CoercionType.ToReal), new List<Expr>{ e }); 
			break;
		}
		case 17 /* "(" */: {
			Get();
			if (StartOf(9)) {
				Expression(out e);
				if (e is BvBounds)
				 this.SemErr("parentheses around bitvector bounds " +
				        "are not allowed"); 
			} else if (la->kind == 89 /* "forall" */ || la->kind == 90 /* "\u2200" */) {
				Forall();
				x = t; 
				QuantifierBody(x, out typeParams, out ds, out kv, out trig, out e);
				if (typeParams.Count + ds.Count > 0)
				 e = new ForallExpr(x, typeParams, ds, kv, trig, e); 
			} else if (la->kind == 91 /* "exists" */ || la->kind == 92 /* "\u2203" */) {
				Exists();
				x = t; 
				QuantifierBody(x, out typeParams, out ds, out kv, out trig, out e);
				if (typeParams.Count + ds.Count > 0)
				 e = new ExistsExpr(x, typeParams, ds, kv, trig, e); 
			} else if (la->kind == 93 /* "lambda" */ || la->kind == 94 /* "\u03bb" */) {
				Lambda();
				x = t; 
				QuantifierBody(x, out typeParams, out ds, out kv, out trig, out e);
				if (trig != null)
				 SemErr("triggers not allowed in lambda expressions");
				if (typeParams.Count + ds.Count > 0)
				 e = new LambdaExpr(x, typeParams, ds, kv, e); 
			} else SynErr(125);
			Expect(18 /* ")" */);
			break;
		}
		case 41 /* "if" */: {
			IfThenElseExpression(out e);
			break;
		}
		case 86 /* "|{" */: {
			CodeExpression(out locals, out blocks);
			e = new CodeExpr(locals, blocks); 
			break;
		}
		default: SynErr(126); break;
		}
}

void Parser::Dec(out BigDec n) {
		string s = ""; 
		if (la->kind == _decimal) {
			Get();
			s = t.val; 
		} else if (la->kind == _dec_float) {
			Get();
			s = t.val; 
		} else SynErr(127);
		try {
		 n = BigDec.FromString(s);
		} catch (FormatException) {
		 this.SemErr("incorrectly formatted number");
		 n = BigDec.ZERO;
		}
		
}

void Parser::Float(out BigFloat n) {
		string s = ""; 
		Expect(_float);
		s = t.val; 
		try {
		 n = BigFloat.FromString(s);
		} catch (FormatException e) {
		 this.SemErr("incorrectly formatted floating point, " + e.Message);
		 n = BigFloat.ZERO;
		}
		
}

void Parser::BvLit(out BigNum n, out int m) {
		Expect(_bvlit);
		int pos = t.val.IndexOf("bv");
		string a = t.val.Substring(0, pos);
		string b = t.val.Substring(pos + 2);
		try {
		 n = BigNum.FromString(a);
		 m = Convert.ToInt32(b);
		} catch (FormatException) {
		 this.SemErr("incorrectly formatted bitvector");
		 n = BigNum.ZERO;
		 m = 0;
		}
		
}

void Parser::Forall() {
		if (la->kind == 89 /* "forall" */) {
			Get();
		} else if (la->kind == 90 /* "\u2200" */) {
			Get();
		} else SynErr(128);
}

void Parser::QuantifierBody(IToken/*!*/ q, out List<TypeVariable>/*!*/ typeParams, out List<Variable>/*!*/ ds,
out QKeyValue kv, out Trigger trig, out Expr/*!*/ body) {
		Contract.Requires(q != null); Contract.Ensures(Contract.ValueAtReturn(out typeParams) != null); Contract.Ensures(Contract.ValueAtReturn(out ds) != null); Contract.Ensures(Contract.ValueAtReturn(out body) != null);
		trig = null; typeParams = new List<TypeVariable> ();
		IToken/*!*/ tok;
		kv = null;
		ds = new List<Variable> ();
		
		if (la->kind == 24 /* "<" */) {
			TypeParams(out tok, out typeParams);
			if (la->kind == _ident || la->kind == 28 /* "{" */) {
				BoundVars(q, out ds);
			}
		} else if (la->kind == _ident || la->kind == 28 /* "{" */) {
			BoundVars(q, out ds);
		} else SynErr(129);
		QSep();
		while (la->kind == 28 /* "{" */) {
			AttributeOrTrigger(ref kv, ref trig);
		}
		Expression(out body);
}

void Parser::Exists() {
		if (la->kind == 91 /* "exists" */) {
			Get();
		} else if (la->kind == 92 /* "\u2203" */) {
			Get();
		} else SynErr(130);
}

void Parser::Lambda() {
		if (la->kind == 93 /* "lambda" */) {
			Get();
		} else if (la->kind == 94 /* "\u03bb" */) {
			Get();
		} else SynErr(131);
}

void Parser::IfThenElseExpression(out Expr/*!*/ e) {
		Contract.Ensures(Contract.ValueAtReturn(out e) != null);
		IToken/*!*/ tok;
		Expr/*!*/ e0, e1, e2;
		e = dummyExpr; 
		Expect(41 /* "if" */);
		tok = t; 
		Expression(out e0);
		Expect(88 /* "then" */);
		Expression(out e1);
		Expect(42 /* "else" */);
		Expression(out e2);
		e = new NAryExpr(tok, new IfThenElse(tok), new List<Expr>{ e0, e1, e2 }); 
}

void Parser::CodeExpression(out List<Variable>/*!*/ locals, out List<Block/*!*/>/*!*/ blocks) {
		Contract.Ensures(Contract.ValueAtReturn(out locals) != null); Contract.Ensures(cce.NonNullElements(Contract.ValueAtReturn(out blocks))); locals = new List<Variable>(); Block/*!*/ b;
		blocks = new List<Block/*!*/>();
		
		Expect(86 /* "|{" */);
		while (la->kind == 15 /* "var" */) {
			LocalVars(locals);
		}
		SpecBlock(out b);
		blocks.Add(b); 
		while (la->kind == _ident) {
			SpecBlock(out b);
			blocks.Add(b); 
		}
		Expect(87 /* "}|" */);
}

void Parser::SpecBlock(out Block/*!*/ b) {
		Contract.Ensures(Contract.ValueAtReturn(out b) != null); IToken/*!*/ x; IToken/*!*/ y;
		Cmd c;  IToken label;
		List<Cmd> cs = new List<Cmd>();
		List<IToken>/*!*/ xs;
		List<String> ss = new List<String>();
		b = dummyBlock;
		Expr/*!*/ e;
		
		Indetifier(out x);
		Expect(10 /* ":" */);
		while (StartOf(8)) {
			LabelOrCmd(out c, out label);
			if (c != null) {
			 Contract.Assert(label == null);
			 cs.Add(c);
			} else {
			 Contract.Assert(label != null);
			 SemErr("SpecBlock's can only have one label");
			}
			
		}
		if (la->kind == 39 /* "goto" */) {
			Get();
			y = t; 
			Identifiers(out xs);
			foreach(IToken/*!*/ s in xs){
			Contract.Assert(s != null);
			ss.Add(s.val); }
			b = new Block(x,x.val,cs,new GotoCmd(y,ss));
			
		} else if (la->kind == 40 /* "return" */) {
			Get();
			Expression(out e);
			b = new Block(x,x.val,cs,new ReturnExprCmd(t,e)); 
		} else SynErr(132);
		Expect(11 /* ";" */);
}

void Parser::AttributeOrTrigger(ref QKeyValue kv, ref Trigger trig) {
		IToken/*!*/ tok;  Expr/*!*/ e;  List<Expr>/*!*/ es;
		string key;
		List<object/*!*/> parameters;  object/*!*/ param;
		
		Expect(28 /* "{" */);
		tok = t; 
		if (la->kind == 10 /* ":" */) {
			Get();
			Expect(_ident);
			key = t.val;  parameters = new List<object/*!*/>(); 
			if (StartOf(16)) {
				AttributeParameter(out param);
				parameters.Add(param); 
				while (la->kind == 13 /* "," */) {
					Get();
					AttributeParameter(out param);
					parameters.Add(param); 
				}
			}
			if (key == "nopats") {
			 if (parameters.Count == 1 && parameters[0] is Expr) {
			   e = (Expr)parameters[0];
			   if(trig==null){
			     trig = new Trigger(tok, false, new List<Expr> { e }, null);
			   } else {
			     trig.AddLast(new Trigger(tok, false, new List<Expr> { e }, null));
			   }
			 } else {
			   this.SemErr("the 'nopats' quantifier attribute expects a string-literal parameter");
			 }
			} else {
			 if (kv==null) {
			   kv = new QKeyValue(tok, key, parameters, null);
			 } else {
			   kv.AddLast(new QKeyValue(tok, key, parameters, null));
			 }
			}
			
		} else if (StartOf(9)) {
			Expression(out e);
			es = new List<Expr> { e }; 
			while (la->kind == 13 /* "," */) {
				Get();
				Expression(out e);
				es.Add(e); 
			}
			if (trig==null) {
			 trig = new Trigger(tok, true, es, null);
			} else {
			 trig.AddLast(new Trigger(tok, true, es, null));
			}
			
		} else SynErr(133);
		Expect(29 /* "}" */);
}

void Parser::AttributeParameter(out object/*!*/ o) {
		Contract.Ensures(Contract.ValueAtReturn(out o) != null);
		o = "error";
		Expr/*!*/ e;
		
		if (la->kind == _string) {
			Get();
			o = t.val.Substring(1, t.val.Length-2); 
		} else if (StartOf(9)) {
			Expression(out e);
			o = e; 
		} else SynErr(134);
}

void Parser::QSep() {
		if (la->kind == 95 /* "::" */) {
			Get();
		} else if (la->kind == 96 /* "\u2022" */) {
			Get();
		} else SynErr(135);
}




// If the user declared a method Init and a mehtod Destroy they should
// be called in the contructur and the destructor respctively.
//
// The following templates are used to recognize if the user declared
// the methods Init and Destroy.

template<typename T>
struct ParserInitExistsRecognizer {
	template<typename U, void (U::*)() = &U::Init>
	struct ExistsIfInitIsDefinedMarker{};

	struct InitIsMissingType {
		char dummy1;
	};
	
	struct InitExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static InitIsMissingType is_here(...);

	// exist only if ExistsIfInitIsDefinedMarker is defined
	template<typename U>
	static InitExistsType is_here(ExistsIfInitIsDefinedMarker<U>*);

	enum { InitExists = (sizeof(is_here<T>(NULL)) == sizeof(InitExistsType)) };
};

template<typename T>
struct ParserDestroyExistsRecognizer {
	template<typename U, void (U::*)() = &U::Destroy>
	struct ExistsIfDestroyIsDefinedMarker{};

	struct DestroyIsMissingType {
		char dummy1;
	};
	
	struct DestroyExistsType {
		char dummy1; char dummy2;
	};

	// exists always
	template<typename U>
	static DestroyIsMissingType is_here(...);

	// exist only if ExistsIfDestroyIsDefinedMarker is defined
	template<typename U>
	static DestroyExistsType is_here(ExistsIfDestroyIsDefinedMarker<U>*);

	enum { DestroyExists = (sizeof(is_here<T>(NULL)) == sizeof(DestroyExistsType)) };
};

// The folloing templates are used to call the Init and Destroy methods if they exist.

// Generic case of the ParserInitCaller, gets used if the Init method is missing
template<typename T, bool = ParserInitExistsRecognizer<T>::InitExists>
struct ParserInitCaller {
	static void CallInit(T *t) {
		// nothing to do
	}
};

// True case of the ParserInitCaller, gets used if the Init method exists
template<typename T>
struct ParserInitCaller<T, true> {
	static void CallInit(T *t) {
		t->Init();
	}
};

// Generic case of the ParserDestroyCaller, gets used if the Destroy method is missing
template<typename T, bool = ParserDestroyExistsRecognizer<T>::DestroyExists>
struct ParserDestroyCaller {
	static void CallDestroy(T *t) {
		// nothing to do
	}
};

// True case of the ParserDestroyCaller, gets used if the Destroy method exists
template<typename T>
struct ParserDestroyCaller<T, true> {
	static void CallDestroy(T *t) {
		t->Destroy();
	}
};

void Parser::Parse() {
	t = NULL;
	la = dummyToken = new Token();
	la->val = coco_string_create(L"Dummy Token");
	Get();
	BoogiePL();
	Expect(0);
}

Parser::Parser(Scanner *scanner) {
	maxT = 97;

	ParserInitCaller<Parser>::CallInit(this);
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

	static bool set[17][99] = {
		{T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,T,x, x,x,T,T, x,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,T, T,T,T,x, T,x,x,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,T, T,T,T,x, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,T, T,T,x,T, x,x,T,T, T,T,T,x, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,x,x,x, x,x,x,T, T,T,T,x, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,T,T, x,T,T,T, x,x,x,x, x,x,x,x, x,T,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,T,T,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,T,T, T,T,T,T, T,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,T,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,T,T, x,T,T,T, x,x,x,x, x,x,x,x, x,T,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,T,T, x,T,T,T, x,x,x,x, x,x,x,x, x,T,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,T, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,T,T,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x},
		{x,T,T,T, T,T,T,T, x,x,x,x, x,x,x,x, x,T,x,T, T,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,T,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, x,x,x,x, T,x,x,x, x,T,T,T, T,T,T,x, x,x,x,x, x,x,x,x, x,x,x}
	};



	return set[s][la->kind];
}

Parser::~Parser() {
	ParserDestroyCaller<Parser>::CallDestroy(this);
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
			case 2: s = coco_string_create(L"bvlit expected"); break;
			case 3: s = coco_string_create(L"digits expected"); break;
			case 4: s = coco_string_create(L"string expected"); break;
			case 5: s = coco_string_create(L"decimal expected"); break;
			case 6: s = coco_string_create(L"dec_float expected"); break;
			case 7: s = coco_string_create(L"float expected"); break;
			case 8: s = coco_string_create(L"\"const\" expected"); break;
			case 9: s = coco_string_create(L"\"unique\" expected"); break;
			case 10: s = coco_string_create(L"\":\" expected"); break;
			case 11: s = coco_string_create(L"\";\" expected"); break;
			case 12: s = coco_string_create(L"\"extends\" expected"); break;
			case 13: s = coco_string_create(L"\",\" expected"); break;
			case 14: s = coco_string_create(L"\"complete\" expected"); break;
			case 15: s = coco_string_create(L"\"var\" expected"); break;
			case 16: s = coco_string_create(L"\"where\" expected"); break;
			case 17: s = coco_string_create(L"\"(\" expected"); break;
			case 18: s = coco_string_create(L"\")\" expected"); break;
			case 19: s = coco_string_create(L"\"int\" expected"); break;
			case 20: s = coco_string_create(L"\"real\" expected"); break;
			case 21: s = coco_string_create(L"\"bool\" expected"); break;
			case 22: s = coco_string_create(L"\"[\" expected"); break;
			case 23: s = coco_string_create(L"\"]\" expected"); break;
			case 24: s = coco_string_create(L"\"<\" expected"); break;
			case 25: s = coco_string_create(L"\">\" expected"); break;
			case 26: s = coco_string_create(L"\"function\" expected"); break;
			case 27: s = coco_string_create(L"\"returns\" expected"); break;
			case 28: s = coco_string_create(L"\"{\" expected"); break;
			case 29: s = coco_string_create(L"\"}\" expected"); break;
			case 30: s = coco_string_create(L"\"axiom\" expected"); break;
			case 31: s = coco_string_create(L"\"type\" expected"); break;
			case 32: s = coco_string_create(L"\"=\" expected"); break;
			case 33: s = coco_string_create(L"\"procedure\" expected"); break;
			case 34: s = coco_string_create(L"\"implementation\" expected"); break;
			case 35: s = coco_string_create(L"\"modifies\" expected"); break;
			case 36: s = coco_string_create(L"\"free\" expected"); break;
			case 37: s = coco_string_create(L"\"requires\" expected"); break;
			case 38: s = coco_string_create(L"\"ensures\" expected"); break;
			case 39: s = coco_string_create(L"\"goto\" expected"); break;
			case 40: s = coco_string_create(L"\"return\" expected"); break;
			case 41: s = coco_string_create(L"\"if\" expected"); break;
			case 42: s = coco_string_create(L"\"else\" expected"); break;
			case 43: s = coco_string_create(L"\"while\" expected"); break;
			case 44: s = coco_string_create(L"\"invariant\" expected"); break;
			case 45: s = coco_string_create(L"\"*\" expected"); break;
			case 46: s = coco_string_create(L"\"break\" expected"); break;
			case 47: s = coco_string_create(L"\"assert\" expected"); break;
			case 48: s = coco_string_create(L"\"assume\" expected"); break;
			case 49: s = coco_string_create(L"\"havoc\" expected"); break;
			case 50: s = coco_string_create(L"\"yield\" expected"); break;
			case 51: s = coco_string_create(L"\":=\" expected"); break;
			case 52: s = coco_string_create(L"\"async\" expected"); break;
			case 53: s = coco_string_create(L"\"call\" expected"); break;
			case 54: s = coco_string_create(L"\"par\" expected"); break;
			case 55: s = coco_string_create(L"\"|\" expected"); break;
			case 56: s = coco_string_create(L"\"<==>\" expected"); break;
			case 57: s = coco_string_create(L"\"\\u21d4\" expected"); break;
			case 58: s = coco_string_create(L"\"==>\" expected"); break;
			case 59: s = coco_string_create(L"\"\\u21d2\" expected"); break;
			case 60: s = coco_string_create(L"\"<==\" expected"); break;
			case 61: s = coco_string_create(L"\"\\u21d0\" expected"); break;
			case 62: s = coco_string_create(L"\"&&\" expected"); break;
			case 63: s = coco_string_create(L"\"\\u2227\" expected"); break;
			case 64: s = coco_string_create(L"\"||\" expected"); break;
			case 65: s = coco_string_create(L"\"\\u2228\" expected"); break;
			case 66: s = coco_string_create(L"\"==\" expected"); break;
			case 67: s = coco_string_create(L"\"<=\" expected"); break;
			case 68: s = coco_string_create(L"\">=\" expected"); break;
			case 69: s = coco_string_create(L"\"!=\" expected"); break;
			case 70: s = coco_string_create(L"\"<:\" expected"); break;
			case 71: s = coco_string_create(L"\"\\u2260\" expected"); break;
			case 72: s = coco_string_create(L"\"\\u2264\" expected"); break;
			case 73: s = coco_string_create(L"\"\\u2265\" expected"); break;
			case 74: s = coco_string_create(L"\"++\" expected"); break;
			case 75: s = coco_string_create(L"\"+\" expected"); break;
			case 76: s = coco_string_create(L"\"-\" expected"); break;
			case 77: s = coco_string_create(L"\"div\" expected"); break;
			case 78: s = coco_string_create(L"\"mod\" expected"); break;
			case 79: s = coco_string_create(L"\"/\" expected"); break;
			case 80: s = coco_string_create(L"\"**\" expected"); break;
			case 81: s = coco_string_create(L"\"!\" expected"); break;
			case 82: s = coco_string_create(L"\"\\u00ac\" expected"); break;
			case 83: s = coco_string_create(L"\"false\" expected"); break;
			case 84: s = coco_string_create(L"\"true\" expected"); break;
			case 85: s = coco_string_create(L"\"old\" expected"); break;
			case 86: s = coco_string_create(L"\"|{\" expected"); break;
			case 87: s = coco_string_create(L"\"}|\" expected"); break;
			case 88: s = coco_string_create(L"\"then\" expected"); break;
			case 89: s = coco_string_create(L"\"forall\" expected"); break;
			case 90: s = coco_string_create(L"\"\\u2200\" expected"); break;
			case 91: s = coco_string_create(L"\"exists\" expected"); break;
			case 92: s = coco_string_create(L"\"\\u2203\" expected"); break;
			case 93: s = coco_string_create(L"\"lambda\" expected"); break;
			case 94: s = coco_string_create(L"\"\\u03bb\" expected"); break;
			case 95: s = coco_string_create(L"\"::\" expected"); break;
			case 96: s = coco_string_create(L"\"\\u2022\" expected"); break;
			case 97: s = coco_string_create(L"??? expected"); break;
			case 98: s = coco_string_create(L"invalid FunctionDef"); break;
			case 99: s = coco_string_create(L"invalid FunctionDef"); break;
			case 100: s = coco_string_create(L"invalid Procedure"); break;
			case 101: s = coco_string_create(L"invalid Type"); break;
			case 102: s = coco_string_create(L"invalid TypeAtom"); break;
			case 103: s = coco_string_create(L"invalid TypeArgs"); break;
			case 104: s = coco_string_create(L"invalid Spec"); break;
			case 105: s = coco_string_create(L"invalid SpecPrePost"); break;
			case 106: s = coco_string_create(L"invalid LabelOrCmd"); break;
			case 107: s = coco_string_create(L"invalid StructuredCmd"); break;
			case 108: s = coco_string_create(L"invalid TransferCmd"); break;
			case 109: s = coco_string_create(L"invalid IfCmd"); break;
			case 110: s = coco_string_create(L"invalid Guard"); break;
			case 111: s = coco_string_create(L"invalid LabelOrAssign"); break;
			case 112: s = coco_string_create(L"invalid CallParams"); break;
			case 113: s = coco_string_create(L"invalid EquivOp"); break;
			case 114: s = coco_string_create(L"invalid ImpliesOp"); break;
			case 115: s = coco_string_create(L"invalid ExpliesOp"); break;
			case 116: s = coco_string_create(L"invalid AndOp"); break;
			case 117: s = coco_string_create(L"invalid OrOp"); break;
			case 118: s = coco_string_create(L"invalid RelOp"); break;
			case 119: s = coco_string_create(L"invalid AddOp"); break;
			case 120: s = coco_string_create(L"invalid MulOp"); break;
			case 121: s = coco_string_create(L"invalid UnaryExpression"); break;
			case 122: s = coco_string_create(L"invalid NegOp"); break;
			case 123: s = coco_string_create(L"invalid CoercionExpression"); break;
			case 124: s = coco_string_create(L"invalid AtomExpression"); break;
			case 125: s = coco_string_create(L"invalid AtomExpression"); break;
			case 126: s = coco_string_create(L"invalid AtomExpression"); break;
			case 127: s = coco_string_create(L"invalid Dec"); break;
			case 128: s = coco_string_create(L"invalid Forall"); break;
			case 129: s = coco_string_create(L"invalid QuantifierBody"); break;
			case 130: s = coco_string_create(L"invalid Exists"); break;
			case 131: s = coco_string_create(L"invalid Lambda"); break;
			case 132: s = coco_string_create(L"invalid SpecBlock"); break;
			case 133: s = coco_string_create(L"invalid AttributeOrTrigger"); break;
			case 134: s = coco_string_create(L"invalid AttributeParameter"); break;
			case 135: s = coco_string_create(L"invalid QSep"); break;

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

} // namespace
} // namespace
} // namespace

