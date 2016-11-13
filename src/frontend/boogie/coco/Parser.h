

#if !defined(frontend__boogie_COCO_PARSER_H__)
#define frontend__boogie_COCO_PARSER_H__

#include <set>
#include <vector>
#include <list>
#include <memory>
#include <string>
#include "frontend/boogie/AST.h"
using std::set;
using std::vector;
using std::list;
using std::unique_ptr;
using std::string;
using namespace frontend::boogie::AST;


#include "Scanner.h"

namespace frontend {
namespace boogie {


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

//Return num errors
static int Parse (const string& s, unique_ptr<Program> result, bool useBaseName=false) {

    auto buf = s.c_str();
    auto len = s.size();
    Errors errors = new Errors();
    auto scanner = new Scanner(buf,len);

    Parser parser = new Parser(scanner, errors, false);
    parser.Parse();
      if (parser.errors.count == 0)
      {
        result = parser.program;
        return 0;
      }
      else
      {
        result = null;
        return parser.errors.count;
      }
}

public:Parser(Scanner scanner, Errors/*!*/ errors, bool disambiguation)
 : this(scanner, errors)
{
  // initialize readonly fields
  Pgm = new Program();
}

/*
// Class to represent the bounds of a bitvector expression t[a:b].
// Objects of this class only exist during parsing and are directly
// turned into BvExtract before they get anywhere else
private class BvBounds : Expr {
  public:
    BigNum Lower;
    BigNum Upper;
  public BvBounds(IToken tok, BigNum lower, BigNum upper)
    : base(tok) {
    Contract.Requires(tok != null);
    this.Lower = lower;
    this.Upper = upper;
  }
  public override Bpl.Type ShallowType { get {Contract.Ensures(Contract.Result<Bpl.Type>() != null); return Bpl.Type.Int; } }
  public override void Resolve(ResolutionContext rc) {
    // Contract.Requires(rc != null);
    rc.Error(this, "bitvector bounds in illegal position");
  }
  public override void Emit(TokenTextWriter stream,
                            int contextBindingStrength, bool fragileContext) {
    Contract.Assert(false);throw new cce.UnreachableException();
  }
  public override void ComputeFreeVariables(GSet<object> freeVars) { Contract.Assert(false);throw new cce.UnreachableException(); }
}
*/

/*--------------------------------------------------------------------------*/


	Parser(Scanner *scanner);
	~Parser();
	void SemErr(const wchar_t* msg);

	void BoogiePL();
	void Consts(out List<Variable>/*!*/ ds);
	void Function(out List<Declaration>/*!*/ ds);
	void Axiom(out Axiom/*!*/ m);
	void UserDefinedTypes(out List<Declaration/*!*/>/*!*/ ts);
	void GlobalVars(out List<Variable>/*!*/ ds);
	void Procedure(out Procedure/*!*/ proc, out /*maybe null*/ Implementation impl);
	void Implementation(out Implementation/*!*/ impl);
	void Attribute(ref QKeyValue kv);
	void IdsTypeWheres(bool allowWhereClauses, string context, System.Action<TypedIdent> action );
	void LocalVars(List<Variable>/*!*/ ds);
	void ProcFormals(bool incoming, bool allowWhereClauses, out List<Variable>/*!*/ ds);
	void AttrsIdsTypeWheres(bool allowAttributes, bool allowWhereClauses, string context, System.Action<TypedIdent, QKeyValue> action );
	void BoundVars(IToken/*!*/ x, out List<Variable>/*!*/ ds);
	void IdsType(out List<TypedIdent>/*!*/ tyds);
	void Idents(out List<IToken>/*!*/ xs);
	void Type(out Bpl.Type/*!*/ ty);
	void AttributesIdsTypeWhere(bool allowAttributes, bool allowWhereClauses, string context, System.Action<TypedIdent, QKeyValue> action );
	void IdsTypeWhere(bool allowWhereClauses, string context, System.Action<TypedIdent> action );
	void Expression(out Expr/*!*/ e0);
	void TypeAtom(out Bpl.Type/*!*/ ty);
	void Ident(out IToken/*!*/ x);
	void TypeArgs(List<Bpl.Type>/*!*/ ts);
	void MapType(out Bpl.Type/*!*/ ty);
	void TypeParams(out IToken/*!*/ tok, out List<TypeVariable>/*!*/ typeParams);
	void Types(List<Bpl.Type>/*!*/ ts);
	void OrderSpec(out bool ChildrenComplete, out List<ConstantParent/*!*/> Parents);
	void VarOrType(out TypedIdent/*!*/ tyd, out QKeyValue kv);
	void Proposition(out Expr/*!*/ e);
	void UserDefinedType(out Declaration/*!*/ decl, QKeyValue kv);
	void WhiteSpaceIdents(out List<IToken>/*!*/ xs);
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


#endif

