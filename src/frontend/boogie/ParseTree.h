#ifndef FRONTEND_BOOGIE_PRASETREE_H
#define FRONTEND_BOOGIE_PRASETREE_H

#include <set>
#include <vector>
#include <memory>
#include <list>
#include "common/data_types.h"
#include "frontend/TextPosition.h"

namespace frontend{
namespace boogie{
namespace ParseTree{
    using common::String;
    using String = common::String;
    using frontend::TextPosition;
    
    using namespace std;

    using std::unique_ptr;
    using std::vector;
    using std::string;
    using std::list;

    using common::Integer;
    using common::Rational;
    
    typedef wstring IDString;

    // <editor-fold desc="utilities">
    template<typename T> T cloneC(const T& in){
        T out;
        for (const auto& e : in)
            out.push_back(e->clone());
        return move(out);
    }
    // </editor-fold>

    // <editor-fold desc="ParseTreeNode">
    class PTreeNode{
    public:
    	TextPosition pos;
    protected:
    	PTreeNode(const TextPosition pos = NoPos) 
                : pos(pos){}
    };
    // </editor-fold>
    
    // <editor-fold desc="forwards">
    class Attribute;
    typedef unique_ptr<Attribute> pAttribute;
    typedef vector<pAttribute> Attributes;
    class Trigger;
    typedef unique_ptr<Trigger> pTrigger;
    typedef vector<pTrigger>   Triggers;
    
    class Expression;
    typedef unique_ptr<Expression> pExpression;
    typedef vector<pExpression> Expressions;
    class WildCardExpression;
    typedef unique_ptr<WildCardExpression> pWCExpression;
    class WExpression;
    typedef unique_ptr<WExpression> pWExpression;
    // </editor-fold>
    
    // <editor-fold desc="Identifiers">
    /////////////////////////////////////////////////////////////
    //Identifier
    class Identifier;
    typedef unique_ptr<Identifier> pIdentifier;
    typedef vector<pIdentifier> Identifiers;
    
    class Identifier : public PTreeNode{
    public:
        Identifier(TextPosition pos, const IDString& name)
        : PTreeNode(pos), name(name){}
        pIdentifier&& clone()const{return move(make_unique<Identifier>(pos,name)); }
        IDString name;
    };
    // </editor-fold>
    
    // <editor-fold desc="Types">
    /////////////////////////////////////////////////////////////
    //Types
    class Type;
    typedef unique_ptr<Type> pType;
    typedef vector<pType> Types;
    class Type : public PTreeNode{
    public:
        Type(TextPosition pos) : PTreeNode(pos){}
        virtual pType clone() const = 0;
    };

    class TypeVariable : public PTreeNode{
    public:
        TypeVariable(pIdentifier&& id) : id(move(id)){}
        virtual unique_ptr<TypeVariable> clone() const {return make_unique<TypeVariable>(id->clone()); }
        pIdentifier id;
    };
    typedef unique_ptr<TypeVariable> pTypeVariable;
    typedef list<pTypeVariable> TypeParameters;
    
    class TypeConstructorInstance : public Type{
    public:
        TypeConstructorInstance(TextPosition pos, pIdentifier&& id, list<pType>&& arguments)
            : Type(pos), id(move(id)), arguments(move(arguments))
        {}
            
        pIdentifier id;
        list<pType> arguments;
    };
    class MapType : public Type{
    public:
        MapType(TextPosition pos, TypeParameters&& typeParameters, vector<pType>&& argumentTypes, pType&& resultType)
                : Type(pos), typeParameters(move(typeParameters)), argumentTypes(move(argumentTypes)), resultType(move(resultType))
        {}
        TypeParameters typeParameters;
        vector<pType> argumentTypes;
        pType resultType; 
    };
    class VariableType : public Type{
    public:
        VariableType(TextPosition pos, pTypeVariable&& var)
            : Type(pos), var(move(var)){}
        pTypeVariable var;
    };

    class UnresolvedType : public Type{
    public:
        UnresolvedType(TextPosition pos, pIdentifier&& id, Types&& args)
            : Type(pos), id(move(id)), args(move(args)){}
        pType clone() const override{ return make_unique<UnresolvedType>(pos,id->clone(), cloneC(args)); }
        pIdentifier id;
        Types args;
    };

    class TypeDef  : public PTreeNode{
    public: 
        TypeDef(pIdentifier&& id, TypeParameters&& parameters)
            : id(move(id)), parameters(move(parameters)){}
        virtual ~TypeDef() = 0;
        pIdentifier id;
        TypeParameters parameters;
    };
    typedef unique_ptr<TypeDef> pTypeDef;
    
    class TypeConstructorDef : public TypeDef{
    public:
        TypeConstructorDef(pIdentifier&& id, TypeParameters&& parameters)
            : TypeDef(move(id),move(parameters)){}
    };
    class TypeSynonymDef : public TypeDef{
    public:
        TypeSynonymDef(pIdentifier&& id, TypeParameters&& parameters, pType&& target)
            : TypeDef(move(id),move(parameters)), target(move(target)){}
        pType target;
    };
    
    class IntegerType : public Type{
    public:
        IntegerType(TextPosition pos) : Type(pos){};
        virtual pType clone() const override{ return make_unique<IntegerType>(pos); }
    };
    class RationalType : public Type{
    public:
        RationalType(TextPosition pos) : Type(pos){};
        virtual pType clone() const override{ return make_unique<RationalType>(pos); }
    };
    class BooleanType : public Type{
    public:
        BooleanType(TextPosition pos) : Type(pos){};
        virtual pType clone() const override{ return make_unique<BooleanType>(pos); }
    };
    // </editor-fold>

    // <editor-fold desc="Variables">
    /////////////////////////////////////////////////////////////
    //Variables
    class Variable : public PTreeNode{
    protected:
    	Variable(const TextPosition pos,
                 Attributes&& attributes,
                 pIdentifier&& id,
                 pType&& type,
                 int flags)
    		: PTreeNode(pos), 
                  attributes(move(attributes)),
                  id(move(id)), 
                  type(move(type)), 
                  flags(flags)
            {}
    public:
    	virtual ~Variable(){}
        static constexpr int GLOBAL   = 1 << 0;
        static constexpr int INPUT    = 1 << 1;
        static constexpr int OUTPUT   = 1 << 2;
        static constexpr int CONSTANT = 1 << 3;
        static constexpr int BOUND    = 1 << 4;

    	Attributes attributes;
    	pIdentifier id;
    	pType       type;
        int flags;
        
        bool isGlobal(){return flags&GLOBAL; }
        bool isInput() {return flags&INPUT; }
        bool isOutput(){return flags&OUTPUT; }
        bool isConstant(){return flags&CONSTANT; }
        bool isBound(){return flags&BOUND; }
    };

    typedef unique_ptr<Variable> pVariable;

    class ConstantOrderSpec;
    typedef unique_ptr<ConstantOrderSpec> pConstantOrderSpec;
    class Constant : public Variable{
    public:
        Constant(const TextPosition& pos,
                 Attributes&&  attributes,
                 pIdentifier&& id,
                 pType&&       type,
                 bool          isUnique,
                 pConstantOrderSpec parentSpec)
                : Variable(
                   pos,
                   move(attributes),
                   move(id),
                   move(type),
                   Variable::GLOBAL | Variable::CONSTANT),
                   isUnique(isUnique), 
                   orderSpec(move(orderSpec))
        {}
        bool isUnique;
        pConstantOrderSpec orderSpec;
//        virtual unique_ptr<Constant> clone() const override{return make_unique<BoundVariable>(cloneC(attributes), id->clone(), type->clone()); }
    };
    class ConstantParentSpec: public PTreeNode {
    public:
        ConstantParentSpec(pIdentifier&& id, bool isUnique)
            : id(move(id)), isUnique(isUnique){}
        ConstantParentSpec(ConstantParentSpec&& other)
            : id(move(other.id)), isUnique(other.isUnique){}

        pIdentifier id;
        bool isUnique;
    };
    class ConstantOrderSpec : public PTreeNode {
    	public:
    		bool specified = false;
    		bool complete = false;
    		vector<ConstantParentSpec> parents;
    		pConstantOrderSpec clone() const;
    };
    
    class GlobalVariable : public Variable{
    public:
        GlobalVariable(const TextPosition& pos,
                 Attributes&&  attributes,
                 pIdentifier&& id,
                 pType&&       type,
                 pExpression&& we)
                : Variable(
                   pos,
                   move(attributes),
                   move(id),
                   move(type),
                   Variable::GLOBAL),
                   we(move(we))
        {}
        virtual unique_ptr<GlobalVariable> clone() const;
        //{return make_unique<GlobalVariable>(pos,cloneC(attributes), id->clone(), type->clone(),we->clone()); }
        pExpression we;
    };
    
    class BoundVariable;
    typedef unique_ptr<BoundVariable> pBoundVar;
    typedef vector<pBoundVar> BoundVars;
    class BoundVariable : public Variable{
    public:
        BoundVariable(Attributes&&  attributes,
                 pIdentifier&& id,
                 pType&&       type)
                : Variable(
                   id->pos,
                   move(attributes),
                   move(id),
                   move(type),
                   Variable::BOUND)
        {}
        virtual unique_ptr<BoundVariable> clone() const {return make_unique<BoundVariable>(cloneC(attributes), id->clone(), type->clone()); }
    };
    class LocalVariable : public Variable{
    public:
        LocalVariable( 
                 Attributes&&  attributes,
                 pIdentifier&& id,
                 pType&&       type)
                : Variable(
                   id->pos,
                   move(attributes),
                   move(id),
                   move(type),
                   0)
        {}
        virtual unique_ptr<LocalVariable> clone() const {return make_unique<LocalVariable>(cloneC(attributes), id->clone(), type->clone()); }
    };
    enum class FormalDir{ In, Out };
    
    class Formal : public Variable {
    public:
        Formal(  Attributes&&        attributes,
                 pIdentifier&&       id,
                 pType&&             type,
                 pExpression&&       we,
                 FormalDir           dir)
                : Variable(
                   id->pos,
                   move(attributes),
                   move(id),
                   move(type),
                    (dir == FormalDir::In ? Variable::INPUT : Variable::OUTPUT)),
                    we(move(we))
        {}
        pExpression we;
    };
    
    typedef unique_ptr<GlobalVariable> pGlobalVariable;
    typedef unique_ptr<LocalVariable>  pLocalVariable;
    typedef unique_ptr<Formal>         pFormal;
    typedef unique_ptr<BoundVariable>  pBoundVariable;
    // </editor-fold>

    // <editor-fold desc="Functions">
    class Function : public PTreeNode{
    public:
        Function(
        	const TextPosition& pos,
            Attributes&&     attributes,
            pIdentifier&&    id,
            TypeParameters&& typeParameters,
            Types&&          argTypes,
            pType&&          resultType,
            Identifiers&&    argNames,
            pExpression      body)
            : PTreeNode( pos ),
              attributes    (move(attributes)),
              id            (move(id)),
              typeParameters(move(typeParameters)),
    	      argTypes      (move(argTypes)),
    	      resultType    (move(resultType)),
              argNames      (move(argNames)),
              body          (move(body))
            {}
        Attributes      attributes;
        pIdentifier     id;
        TypeParameters  typeParameters;
        vector<pType>   argTypes;
        pType           resultType;
        vector<pIdentifier> argNames;
        pExpression     body;
    };
    typedef unique_ptr<Function> pFunction;
    // </editor-fold>

    // <editor-fold desc="Axioms">
    class Axiom : public PTreeNode{
    public:
        Axiom(TextPosition pos, Attributes&& attributes, pExpression&& expression)
            : PTreeNode(pos), attributes(move(attributes)), expression(move(expression))
        {}
        Attributes attributes;
        pExpression expression;
    };
    typedef unique_ptr<Axiom> pAxiom;
    // </editor-fold>
    
    // <editor-fold desc="Procedures">
    class VariableExpression;
    typedef unique_ptr<VariableExpression> pVariableExpression;
    class SpecExpression : public PTreeNode{
    public:
        SpecExpression(const TextPosition& pos, pExpression&& e, bool isFree, Attributes attributes = Attributes())
            : PTreeNode(pos), e(move(e)), isFree(isFree), attributes(attributes) {}
        pExpression e;
        bool isFree;
        Attributes attributes;
    };
    
    typedef vector<pFormal> Formals;
    typedef unique_ptr<SpecExpression> pSpecExpression;
    typedef vector<pSpecExpression> SpecExpressions;
    
    class ProcSignature;
    typedef unique_ptr<ProcSignature> pProcSignature;
    class ProcSignature : public PTreeNode{
    public:
        pProcSignature cloneWOWhere() const;
        TypeParameters tParams;
        Formals        formals;
    };
    
    class Modifies : public PTreeNode{
    public:
        Modifies(TextPosition pos, bool free, pIdentifier&& id) : PTreeNode(pos), id(move(id)), free(free){}
        pIdentifier id;
        bool free;
    };
    typedef unique_ptr<Modifies> pModifies;
    
    class ProcSpec;
    typedef unique_ptr<ProcSpec> pProcSpec;
    class ProcSpec : public PTreeNode{
    public:
        ProcSpec(){}
        pProcSpec clone() const;

        vector<pModifies> modifies;
        SpecExpressions preconditions;
        SpecExpressions postconditions;
    };
    
    class ProcedureSC : public PTreeNode{
    protected:
        ProcedureSC(pIdentifier&& id, Attributes&& attributes, pProcSignature&& sig, pProcSpec&& spec)
            : id(move(id)), attributes(move(attributes)), sig(move(sig)), spec(move(spec)) {}
    public:
        virtual ~ProcedureSC() = 0;
        pIdentifier id;
        Attributes attributes;
        pProcSignature sig;
        pProcSpec spec;
    };
    class Procedure : public ProcedureSC{
        Procedure(pIdentifier&& id, Attributes&& attributes, pProcSignature&& sig, pProcSpec&& spec)
            : ProcedureSC( move(id), move(attributes), move(sig), move(spec)) {}
    };
    typedef unique_ptr<Procedure> pProcedure;
    
    class Statement;
    typedef unique_ptr<Statement> pStatement;
    
    typedef vector<LocalVariable> pLocals;
    
    class ScopeBlock : public PTreeNode{
    public:
        pLocals locals;
        
    };
    
    class ScopedBlocks;
    typedef unique_ptr<ScopedBlocks> pScopedBlocks;
    
    class Implementation : public ProcedureSC{
        Implementation(
            pIdentifier&& id, Attributes&& attributes, pProcSignature&& sig, pProcSpec&& spec, pScopedBlocks&& body)
            : ProcedureSC( move(id), move(attributes), move(sig), move(spec)), 
              body(move(body)) {}
        pScopedBlocks body;
    };
    typedef unique_ptr<Implementation> pImplementation;
    // </editor-fold>
    
    // <editor-fold desc="Attributes and triggers">
    class AttributeParam;
    typedef unique_ptr<AttributeParam> pAttributeParam;
    class Attribute : public PTreeNode{
    public:
        Attribute(){}
        Attribute(pIdentifier&& id, vector<pAttributeParam>&& params)
            : id(move(id)), params(move(params)){}
        
        pAttribute clone() const{
            return make_unique<Attribute>(id->clone(),cloneC(params));
        }
        
        pIdentifier id;
        vector<pAttributeParam> params;
    };
    class AttributeParam : public PTreeNode{
    public:
        virtual ~AttributeParam() = 0;
        virtual pAttributeParam clone() const = 0;
    };
    class StringAttributeParam : public AttributeParam{
    public:
        StringAttributeParam(const string& value)
            : value(value){}
        std::string value;
        pAttributeParam clone() const override{return make_unique<StringAttributeParam>(value);};
    };
    class ExpressionAttributeParam : public AttributeParam{
    public:
        ExpressionAttributeParam(pExpression value)
            : value(move(value)){}
        pExpression value;
    };
    class Trigger : public PTreeNode{
    public:
        Trigger(vector<pExpression>&& _ts)
            :ts(_ts){}
        vector<pExpression> ts;
    };
    // </editor-fold>

    // <editor-fold desc="Programs">
    class Scope : public PTreeNode{
    public:
    	void addTypeDef    (pTypeDef&&  d);
    	void addVariableDef(pVariable&& d);
    	void addFunctionDef(pFunction&& d);
    private:
    	vector<pVariable> variables;
    	vector<pTypeDef>  typeDefs;
    	vector<pFunction> functions;
    };

    class Program : public Scope{
    public:
    	void addAxiom         (pAxiom&&          axiom);
    	void addProcedure     (pProcedure&&      procedure);
    	void addImplementation(pImplementation&& Implementation);
    private:
        vector<pAxiom>          axioms;
        vector<pProcedure>      procedures;
        vector<pImplementation> implementations;
    };
//    class Operation : public ASTNode{};
    // </editor-fold>

    // <editor-fold desc="Expressions">
    class WExpression : public PTreeNode{
    public:
        WExpression(TextPosition pos) : PTreeNode(pos) {}
    };
    typedef unique_ptr<WExpression> pWExpression;
    
    class WildCardExpression : public WExpression{
    public:
        WildCardExpression(TextPosition pos) : WExpression(pos) {}
    };
    typedef unique_ptr<Expression> pExpression;

    class Expression;
    typedef unique_ptr<Expression> pExpression;
    class Expression : public WExpression{
    public:
        virtual ~Expression() = 0;
        virtual pExpression clone() const = 0;
//        static void make(const TextPosition& pos, const Operation op, std::initializer_list<unique_ptr<Expression>>);
    protected:
        Expression(TextPosition pos) : WExpression(pos) {}
    };
    
    class VariableExpression : public Expression{
    public:
        VariableExpression(pIdentifier&& id) : Expression(id->pos), id(move(id)) {}
        virtual pExpression clone() const override{ return make_unique<VariableExpression>(id->clone()); }
        pIdentifier id;
    };
    typedef unique_ptr<VariableExpression> pVariableExpression;
    
    enum class Binder{ forall, exists, lambda };
    class Operator : public PTreeNode{
    public:
        virtual ~Operator() = 0;
        virtual unique_ptr<Operator> clone() const = 0;        
    protected:
        Operator(TextPosition pos) : PTreeNode(pos){}
    };
    typedef unique_ptr<Operator> pOperator;
    
    class OpC  : public Operator{
    public: 
        OpC(TextPosition pos,const wstring& name) : Operator(pos),name(name){}
        unique_ptr<Operator> clone() const override{ return make_unique<OpC>(pos,name); }
        static pOperator implies(TextPosition pos){return make_unique<OpC>(pos,"=>");}
        static pOperator explies(TextPosition pos){return make_unique<OpC>(pos,"<=");}

        static pOperator equiv(TextPosition pos){return make_unique<OpC>(pos,"<=>");}
        static pOperator andOp(TextPosition pos){return make_unique<OpC>(pos,"&&");}
        static pOperator orOp(TextPosition pos){return make_unique<OpC>(pos,"||");}
        static pOperator notOp(TextPosition pos){return make_unique<OpC>(pos,"!");}
        
        static pOperator minus(TextPosition pos){return make_unique<OpC>(pos,"-");}
        static pOperator add(TextPosition pos){return make_unique<OpC>(pos,"+");}
        static pOperator sub(TextPosition pos){return make_unique<OpC>(pos,"-");}
        static pOperator mul(TextPosition pos){return make_unique<OpC>(pos,"mul");}
        static pOperator div(TextPosition pos){return make_unique<OpC>(pos,"div");}
        static pOperator mod(TextPosition pos){return make_unique<OpC>(pos,"%");}
        static pOperator divR(TextPosition pos){return make_unique<OpC>(pos,"/");}
        static pOperator pow(TextPosition pos){return make_unique<OpC>(pos,"pow");}
        
        static pOperator bvSelect(TextPosition pos){return make_unique<OpC>(pos,"[:]");}

        wstring name;
    };
    
    
    class UserDefinedOp  : public Operator{
    public:
        UserDefinedOp(pIdentifier&& id) : Operator(id->pos), id(move(id)){}
        unique_ptr<Operator> clone() const override{ return make_unique<UserDefinedOp>(id->clone()); }
        pIdentifier id;
    };
    class CastOp  : public Operator{
    public:
        CastOp(pType&& type) : Operator(type->pos), type(move(type)){}
        unique_ptr<Operator> clone() const override{ return make_unique<CastOp>(type->clone()); }
        pType type;
    };

    
    class FAExpression;
    typedef unique_ptr<FAExpression> pFAExpression;
    class FAExpression : public Expression{
    public:
        FAExpression(pOperator&& op, Expressions&& args)
            : Expression(op->pos), op(move(op)), args(move(args)){}
        virtual pExpression clone() const override{ return make_unique<FAExpression>(op->clone(),cloneC(args)); }

        pOperator op;
        vector<pExpression> args;
        
//        static unique_ptr<FAExpression> make(pOperator&& op){return make_unique<FAExpression>(op, Expressions());}
        static unique_ptr<FAExpression> make(pOperator&& op,std::initializer_list<pExpression> args){
            return make_unique<FAExpression>(op, Expressions(move(args)));}
            
    };
    class QExpression : public Expression{
    public:
        QExpression(TextPosition pos, Binder binder, TypeParameters&& tVars, vector<pBoundVariable>&& vars, pExpression e)
            : Expression(pos), binder(binder), tVars(move(tVars)), vars(move(vars)), e(move(e)) {}
        virtual pExpression clone() const override{ return make_unique<QExpression>(pos,binder, cloneC(tVars), cloneC(vars), e->clone()); }

        Binder binder;
        TypeParameters tVars;
        vector<pBoundVariable> vars;
        pExpression e;
    };
    class OldExpression : public Expression{
    public:
        OldExpression(TextPosition pos, pExpression&& e) : Expression(pos), e(move(e)){}
        pExpression e;
    };
    class LiteralExpression : public Expression{
    protected:
        LiteralExpression(TextPosition pos) : Expression(pos){}
    };
    typedef unique_ptr<LiteralExpression> pLiteralExpression;
    
    template<class T>class LiteralExpressionC : public LiteralExpression{
    public:
        LiteralExpressionC(TextPosition pos, T&& val) : LiteralExpression(pos), val(move(val)){}
        T val;
    };
    
    class BVConstant{ 
    public:
        BVConstant(int _numBits, Integer&& _value)
            : numBits(_numBits), value(_value){}
        int numBits;
        Integer value;
    };
    class Float{
    public:
        Float(Integer mantissa, int mantissaBits, Integer exponent, int exponentBits, bool sign)
            : mantissa(mantissa), mantissaBits(mantissaBits), 
                exponent(exponent), exponentBits(exponentBits),
                sign(sign)
        {}
        Integer mantissa;
        int mantissaBits;
        Integer exponent;
        int exponentBits;
        bool sign;

    };
    
    typedef LiteralExpressionC<bool>       BooleanLiteralExpression;
    typedef LiteralExpressionC<Integer>    IntegerLiteralExpression;
    typedef LiteralExpressionC<Rational>   RationalLiteralExpression;
    typedef LiteralExpressionC<Float>      FloatLiteralExpression;
    typedef LiteralExpressionC<BVConstant> BVLiteralExpression;
    
    class ScopedSimpleBlocks;
    typedef unique_ptr<ScopedSimpleBlocks> pScopedSimpleBlocks;
    class CodeExpression : public Expression{
    public:
        CodeExpression(TextPosition pos, pScopedSimpleBlocks&& ss) : Expression(pos), ss(move(ss)){}
        pScopedSimpleBlocks ss;
    };
    // </editor-fold>

    // <editor-fold desc="Statements">
    class Statement : public PTreeNode{public:Statement(TextPosition pos) : PTreeNode(pos){}};
    class SimpleStatement : public Statement{public:SimpleStatement(TextPosition pos) : Statement(pos){}};
    class ComplexStatement : public Statement{public:ComplexStatement(TextPosition pos) : Statement(pos){}};
    class ControlStatement : public ComplexStatement{public:ControlStatement(TextPosition pos) : ComplexStatement(pos){}};
    class CompoundStatement : public ComplexStatement{public:CompoundStatement(TextPosition pos) : ComplexStatement(pos){}};
    
    typedef unique_ptr<Statement> pStatement;
    typedef vector<pStatement> StatementSeq;
    
    typedef unique_ptr<SimpleStatement> pSimpleStatement;
    typedef vector<pSimpleStatement> SimpleStatementSeq;
    
    typedef unique_ptr<ControlStatement> pControlStatement;

    typedef unique_ptr<ComplexStatement> pComplexStatement;
    
    class Label;
    typedef unique_ptr<Label> pLabel;
    typedef vector<pLabel> Labels;
    
    class SimpleBlock : public PTreeNode{
    public:
        pLabel label;
        SimpleStatementSeq statements;
        ControlStatement control;
    };
    typedef unique_ptr<SimpleBlock> pSimpleBlock;
    typedef vector<pSimpleBlock> SimpleBlockSeq;
    
    typedef vector<pLocalVariable> Locals;
    class ScopedSimpleBlocks : public PTreeNode{
        Locals locals;
        SimpleBlockSeq blocks;
    };
    typedef unique_ptr<ScopedSimpleBlocks> pScopedSimpleBlocks;
    
    class Block : public PTreeNode{
    public:
        Block(Labels&& labels, SimpleStatementSeq&& statements, pComplexStatement&& endStatement)
            : labels(move(labels)), statements(move(statements)), endStatement(move(endStatement)){}
        Block(pComplexStatement&& endStatement)
            : endStatement(move(endStatement)){}
        Labels labels;
        SimpleStatementSeq statements;
        pComplexStatement endStatement;
    };
    typedef unique_ptr<Block> pBlock;
    typedef vector<pBlock> BlockSeq;
    
    class ScopedBlocks : public PTreeNode{
    public:
        Locals locals;
        BlockSeq blocks;
    };
    typedef unique_ptr<ScopedBlocks> pScopedBlocks;
    
    class Label : public PTreeNode{
    public:
        Label(pIdentifier&& id) : id(move(id)){}
        pIdentifier id;
    };
    typedef unique_ptr<Label> pLabel;
    
    class PredicateStatement : public SimpleStatement{
    public:
        pExpression e;
    protected:
        PredicateStatement(TextPosition pos,pExpression&& e) : SimpleStatement(pos), e(move(e)){}
    };
    class AssertStatement : public PredicateStatement{
    public:
        AssertStatement(TextPosition pos, pExpression&& e) : PredicateStatement(pos,move(e)) {}
    };
    class AssumeStatement : public PredicateStatement{
    public:
        AssumeStatement(TextPosition pos, pExpression&& e) : PredicateStatement(pos,move(e)) {}
    };
    
    class HavocStatement : public SimpleStatement{
    public:
        HavocStatement(TextPosition pos, list<pVariable>&& vars) : SimpleStatement(pos), vars(move(vars)) {}
        list<pVariable> vars;
    };

    class AssignLHS : public PTreeNode{};
    class AssignLHSVar : public AssignLHS{
    public:
        AssignLHSVar(pIdentifier&& id) : id(move(id)){}
        pIdentifier id;
    };
    class MapSelect : public PTreeNode{
    public:
        MapSelect(vector<pExpression>&& indices) : indices(move(indices)){}
        vector<pExpression> indices;
    };
    typedef unique_ptr<MapSelect> pMapSelect;
    class AssignLHSMap : public AssignLHS{
        AssignLHSMap(pIdentifier&& id, vector<pMapSelect>&& mss) 
            : id(move(id)), mss(move(mss)){}
        pIdentifier id;
        vector<pMapSelect> mss;
    };
    typedef unique_ptr<AssignLHS> pAssignLHS;
    class AssignStatement : public SimpleStatement{
    public:
        AssignStatement(TextPosition pos, vector<pAssignLHS>&& lhss, vector<pExpression>&& rhss) 
             : SimpleStatement(pos), lhss(move(lhss)), rhss(move(rhss)) {}
        vector<pAssignLHS> lhss;
        vector<pExpression> rhss;
    };

    class CallStatement : public SimpleStatement{
    public:
        CallStatement(TextPosition pos, vector<pAssignLHS>&& lhss, pIdentifier&& procId, vector<pExpression>&& args) 
            :  SimpleStatement(pos), lhss(move(lhss)), procId(move(procId)), args(move(args)) {}
        vector<pAssignLHS> lhss;
        pIdentifier procId;
        vector<pExpression> args;
    };

    class CallForallStatement : public SimpleStatement{
    public:
        CallForallStatement(TextPosition pos, pIdentifier&& procId, vector<pWExpression>&& args) 
             : SimpleStatement(pos), procId(move(procId)), args(move(args)) {}
        pIdentifier procId;
        vector<pWExpression> args;
    };

    class SkipStatement : public SimpleStatement{public: SkipStatement(TextPosition pos) : SimpleStatement(pos){}};
    
    class ITEStatement : public CompoundStatement{
    public:
        ITEStatement(TextPosition pos, pWExpression&& cond, pStatement&& thenS, pStatement&& elseS = make_unique<SkipStatement>())
            : CompoundStatement(pos), cond(move(cond)), thenS(move(thenS)), elseS(move(elseS)){}
        pWExpression cond;
        pStatement   thenS;
        pStatement   elseS;
    };

    class WhileStatement : public CompoundStatement{
    public:
        WhileStatement(TextPosition pos, pWExpression&& cond, vector<SpecExpression>&& invariant, pStatement&& body)
            : CompoundStatement(pos), cond(move(cond)), invariant(move(invariant)), body(move(body)){}
        pWExpression cond;
        vector<SpecExpression> invariant;
        pStatement   body;
    };
    typedef unique_ptr<WhileStatement> pWhileStatement;

    class BreakStatement : public ControlStatement{
    public:
        BreakStatement(TextPosition pos, pLabel&& target) : ControlStatement(pos), target(move(target)){}
        
        pLabel target;
    };
    typedef unique_ptr<BreakStatement> pBreakStatement;

    class GotoStatement : public ControlStatement{
    public:
        GotoStatement(TextPosition pos, vector<pLabel>&& targets) : ControlStatement(pos), targets(move(targets)){}
        
        Labels targets;
    };

    class ReturnStatement : public ControlStatement{
    public:
        ReturnStatement(TextPosition pos) : ControlStatement(pos) {}
    };
    
    // </editor-fold>

}//namespace AST
}//namespace frontend
}//namespace boogie
#endif
