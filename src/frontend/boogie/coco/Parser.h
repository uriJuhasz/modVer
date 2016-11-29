

#if !defined(frontend__boogie__parser_COCO_PARSER_H__)
#define frontend__boogie__parser_COCO_PARSER_H__

#include <vector>
#include <string>
#include <memory>
#include "frontend/boogie/AST.h"

using namespace frontend::boogie::AST;
using std::vector;
using std::unique_ptr;


#include "Scanner.h"

namespace frontend {
namespace boogie {
namespace parser {


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
		_bvlit=2,
		_digits=3,
		_string=4,
		_decimal=5,
		_dec_float=6,
		_float=7
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

Program program;
void parse(){
	BoogiePL();
} 

static TextPosition toTextPos(const Token& t){ return TextPosition(t.line,t.col); } 



/*--------------------------------------------------------------------------*/


	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void BoogiePL();
	void constantsDef(AST::Scope& scope );
	void FunctionDef(Scope& scope );
	void Axiom(Scope& scope /*out Axiom/*!*/ m*/);
	void TypeDefs(Scope& scope /*out List<Declaration> ts*/ );
	void GlobalVarDefs(Scope& scope );
	void Procedure(Scope& scope );
	void Implementation(Scope& scope );
	void Attributes(unique_ptr<Attributes>& );
	void Identifiers(vector<Identifier>& ids );
	void Type(unique_ptr<Type>& type );
	void OrderSpec(unique_ptr<ConstantOrderSpec>& orderSpec);
	void OrderSpecParent(ConstantOrderSpec& orderSpec );
	void IdsTypeWheres(bool allowWhereClauses, const string& context, System.Action<TypedIdent> action );
	void IdsTypeWhere(bool allowWhereClauses, string context, System.Action<TypedIdent> action );
	void Expression(out Expr/*!*/ e0);
	void LocalVars(List<Variable>/*!*/ ds);
	void ProcFormals(bool incoming, bool allowWhereClauses, out List<Variable>/*!*/ ds);
	void AttrsIdsTypeWheres(bool allowAttributes, bool allowWhereClauses, string context, System.Action<TypedIdent, QKeyValue> action );
	void BoundVars(IToken/*!*/ x, out List<Variable>/*!*/ ds);
	void AttributesIdsTypeWhere(bool allowAttributes, bool allowWhereClauses, string context, System.Action<TypedIdent, QKeyValue> action );
	void TypeAtom(out Bpl.Type/*!*/ ty);
	void Indetifier(out IToken/*!*/ x);
	void TypeArgs(List<Bpl.Type>/*!*/ ts);
	void MapType(out Bpl.Type/*!*/ ty);
	void TypeParams(out IToken/*!*/ tok, out List<TypeVariable>/*!*/ typeParams);
	void Types(List<Bpl.Type>/*!*/ ts);
	void VarOrType(out TypedIdent/*!*/ tyd, out QKeyValue kv);
	void Proposition(out Expr/*!*/ e);
	void UserDefinedType(out Declaration/*!*/ decl, QKeyValue kv);
	void WhiteSpaceIdentifiers(out List<IToken>/*!*/ xs);
	void ProcSignature(bool allowWhereClausesOnFormals, out IToken/*!*/ name, out List<TypeVariable>/*!*/ typeParams,
out List<Variable>/*!*/ ins, out List<Variable>/*!*/ outs, out QKeyValue kv);
	void Spec(List<Requires>/*!*/ pre, List<IdentifierExpr>/*!*/ mods, List<Ensures>/*!*/ post);
	void ImplBody(out List<Variable>/*!*/ locals, out StmtList/*!*/ stmtList);
	void SpecPrePost(bool free, List<Requires>/*!*/ pre, List<Ensures>/*!*/ post);
	void StmtList(out StmtList/*!*/ stmtList);
	void LabelOrCmd(out Cmd c, out IToken label);
	void StructuredCmd(out StructuredCmd/*!*/ ec);
	void TransferCmd(out TransferCmd/*!*/ tc);
	void IfCmd(out IfCmd/*!*/ ifcmd);
	void WhileCmd(out WhileCmd/*!*/ wcmd);
	void BreakCmd(out BreakCmd/*!*/ bcmd);
	void Guard(out Expr e);
	void LabelOrAssign(out Cmd c, out IToken label);
	void CallCmd(out Cmd c);
	void ParCallCmd(out Cmd d);
	void MapAssignIndex(out IToken/*!*/ x, out List<Expr/*!*/>/*!*/ indexes);
	void CallParams(bool isAsync, bool isFree, QKeyValue kv, IToken x, out Cmd c);
	void Expressions(out List<Expr>/*!*/ es);
	void ImpliesExpression(bool noExplies, out Expr/*!*/ e0);
	void EquivOp();
	void LogicalExpression(out Expr/*!*/ e0);
	void ImpliesOp();
	void ExpliesOp();
	void RelationalExpression(out Expr/*!*/ e0);
	void AndOp();
	void OrOp();
	void BvTerm(out Expr/*!*/ e0);
	void RelOp(out IToken/*!*/ x, out BinaryOperator.Opcode op);
	void Term(out Expr/*!*/ e0);
	void Factor(out Expr/*!*/ e0);
	void AddOp(out IToken/*!*/ x, out BinaryOperator.Opcode op);
	void Power(out Expr/*!*/ e0);
	void MulOp(out IToken/*!*/ x, out BinaryOperator.Opcode op);
	void UnaryExpression(out Expr/*!*/ e);
	void NegOp();
	void CoercionExpression(out Expr/*!*/ e);
	void ArrayExpression(out Expr/*!*/ e);
	void Nat(out BigNum n);
	void AtomExpression(out Expr/*!*/ e);
	void Dec(out BigDec n);
	void Float(out BigFloat n);
	void BvLit(out BigNum n, out int m);
	void Forall();
	void QuantifierBody(IToken/*!*/ q, out List<TypeVariable>/*!*/ typeParams, out List<Variable>/*!*/ ds,
out QKeyValue kv, out Trigger trig, out Expr/*!*/ body);
	void Exists();
	void Lambda();
	void IfThenElseExpression(out Expr/*!*/ e);
	void CodeExpression(out List<Variable>/*!*/ locals, out List<Block/*!*/>/*!*/ blocks);
	void SpecBlock(out Block/*!*/ b);
	void AttributeOrTrigger(ref QKeyValue kv, ref Trigger trig);
	void AttributeParameter(out object/*!*/ o);
	void QSep();

	void Parse();

}; // end Parser

} // namespace
} // namespace
} // namespace


#endif

