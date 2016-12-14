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

    // <editor-fold desc="utilities">
    template<typename T,template<typename> class C>list<T>&& clone(const C<T>& in){
        C<T> out;
        for (const auto& e : in)
            out.push_back(e->clone());
        return out;
    }
    // </editor-fold>

    // <editor-fold desc="ParseTreeNode">
    class PTreeNode{
    public:
    	TextPosition textPos;
    protected:
    	PTreeNode(const TextPosition& _textPos = TextPosition::NoPos) 
                : textPos(_textPos){}
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
    // </editor-fold>
    
    // <editor-fold desc="Identifiers">
    /////////////////////////////////////////////////////////////
    //Identifier
    class Identifier;
    typedef unique_ptr<Identifier> pIdentifier;
    class Identifier : public PTreeNode{
    public:
        Identifier(TextPosition _pos, const common::String& _name)
        : PTreeNode(_pos), name(_name){}
        String name;
    };
    // </editor-fold>
    
    // <editor-fold desc="Types">
    /////////////////////////////////////////////////////////////
    //Types
    class Type;
    typedef unique_ptr<Type> pType;
    class Type : public PTreeNode{
    public:
        virtual pType clone() const = 0;
        virtual ~Type() = 0;
    };

    class TypeVariable : public PTreeNode{
    public:
        TypeVariable(pIdentifier&& id) : id(move(id)){}
        pIdentifier id;
    };
    typedef unique_ptr<TypeVariable> pTypeVariable;
    typedef list<pTypeVariable> TypeParameters;
    
    class TypeConstructorInstance : public Type{
    public:
        TypeConstructorInstance(pIdentifier&& id, list<pType>&& arguments)
            : id(move(id)), arguments(move(arguments))
        {}
            
        pIdentifier id;
        list<pType> arguments;
    };
    class MapType : public Type{
    public:
        MapType(TypeParameters&& typeParameters, vector<pType>&& argumentTypes, pType&& resultType)
                : TypeParameters(move(typeParameters)), argumentTypes(move(argumentTypes)), resultType(move(resultType))
        {}
        TypeParameters typeParameters;
        vector<pType> argumentTypes;
        pType resultType; 
    };
    class VariableType : public Type{
    public:
        VariableType(pTypeVariable&& _var)
            : var(_var){}
        pTypeVariable var;
    };

    class TypeDef  : public PTreeNode{
    public: 
        TypeDef(pIdentifier&& _id, TypeParameters&& _parameters)
            : id(_id), parameters(_parameters){}
        virtual ~TypeDef() = 0;
        pIdentifier id;
        TypeParameters parameters;
    };
    typedef unique_ptr<TypeDef> pTypeDef;
    
    class TypeConstructorDef : public TypeDef{
    public:
        TypeConstructorDef(pIdentifier&& _id, TypeParameters&& _parameters)
            : TypeDef(_id,_parameters){}
    };
    class TypeSynonymDef : public TypeDef{
    public:
        TypeSynonymDef(pIdentifier&& _id, TypeParameters&& _parameters, pType&& _target)
            : TypeDef(_id,_parameters), target(_target){}
        pType target;
    };
    // </editor-fold>

    // <editor-fold desc="Variables">
    /////////////////////////////////////////////////////////////
    //Variables
    class Variable : public PTreeNode{
    protected:
    	Variable(const TextPosition _textPos,
                 Attributes&& _attributes,
                 pIdentifier&& _id,
                 pType&& _type,
                 int _flags)
    		: PTreeNode(_textPos), 
                  id(move(_id)), 
                  type(move(_type)), 
                  attributes(move(_attributes)),
                  flags(_flags)
            {}
    public:
        static constexpr int GLOBAL   = 1 << 0;
        static constexpr int INPUT    = 1 << 1;
        static constexpr int OUTPUT   = 1 << 2;
        static constexpr int CONSTANT = 1 << 3;
        static constexpr int BOUND    = 1 << 4;
    	virtual ~Variable(){}
    	pIdentifier id;
    	pType       type;
    	Attributes attributes;
        
        int flags;
        bool isGlobal(){return flags&GLOBAL; }
        bool isInput() {return flags&INPUT; }
        bool isOutput(){return flags&OUTPUT; }
        bool isConstant(){return flags&CONSTANT; }
        bool isBound(){return flags&BOUND; }
    };

    typedef unique_ptr<Variable> pVariable;
    class ConstantParentSpec;
    typedef unique_ptr<ConstantParentSpec> pConstantParentSpec;
    class Constant final : public Variable{
    public:
        Constant(const TextPosition& _textPos,
                 Attributes&&  _attributes,
                 pIdentifier&& _id,
                 pType&&       _type,
                 bool          _isUnique,
                 pConstantParentSpec _parentSpec)
                : Variable(
                   _textPos,
                   move(_attributes),
                   move(_id),
                   move(_type),
                   Variable::GLOBAL | Variable::CONSTANT),
                   isUnique(_isUnique), 
                   parentSpec(move(_parentSpec))
        {}
        bool isUnique;
        pConstantParentSpec parentSpec;
    };
    class ConstantParentSpec: public PTreeNode {
    public:
        ConstantParentSpec(pIdentifier&& _id, bool _isUnique)
            : id(_id), isUnique(_isUnique)
            {}
        bool isUnique;
        pIdentifier id;
    };
    typedef unique_ptr<pConstantOrderSpec> pConstantOrderSpec;
    class ConstantOrderSpec : public PTreeNode {
    	public:
    		bool specified = false;
    		bool ChildrenComplete = false;
    		vector<ConstantParentSpec> parents;
    		pConstantOrderSpec clone() const;
    };
    
    class GlobalVariable final : public Variable{
    public:
        GlobalVariable(const TextPosition& _textPos,
                 Attributes&&  _attributes,
                 pIdentifier&& _id,
                 pType&&       _type,
                 pExpression&& _whereClause)
                : Variable(
                   _textPos,
                   move(_attributes),
                   move(_id),
                   move(_type),
                   Variable::GLOBAL),
                  whereClause(_whereClause)
        {}
        pExpression whereClause;
    };
    class BoundVariable final : public Variable{
    public:
        BoundVariable(const TextPosition& _textPos,
                 Attributes&&  _attributes,
                 pIdentifier&& _id,
                 pType&&       _type)
                : Variable(
                   _textPos,
                   move(_attributes),
                   move(_id),
                   move(_type),
                   Variable::BOUND)
        {}
    };
    class LocalVariable final : public Variable{
    public:
        LocalVariable(const TextPosition& _textPos,
                 Attributes&&  _attributes,
                 pIdentifier&& _id,
                 pType&&       _type,bool isInput,bool isOutput)
                : Variable(
                   _textPos,
                   move(_attributes),
                   move(_id),
                   move(_type),
                    (isInput ? Variable::INPUT : isOutput ? Variable::OUTPUT : 0))
        {}
    };
    typedef unique_ptr<pGlobalVariable> pGlobalVariable;
    typedef unique_ptr<LocalVariable> pLocalVariable;
    typedef unique_ptr<BoundVariable> pBoundVariable;
    // </editor-fold>

    // <editor-fold desc="Functions">
    class Function : public PTreeNode{
    public:
        Function(
        	const TextPosition& pos,
            Attributes&&      attributes,
            pIdentifier&&     id,
            TypeParameters&&  typeParameters,
            list<pVariable>&& parameters,
            pVariable&&       returnVar,
            pExpression       body)
            : PTreeNode( pos ),
              attributes    (attributes),
              id            (id),
              typeParameters(typeParameters),
    	      parameters    (parameters),
    	      returnVar     (returnVar),
              body          (body)
            {}
        Attributes      attributes;
        pIdentifier     id;
        TypeParameters  typeParameters;
        list<pVariable> parameters;
        pVariable       returnVar;
        pExpression     body;
    };
    typedef unique_ptr<Function> pFunction;
    // </editor-fold>

    // <editor-fold desc="Axioms">
    class Axiom : public PTreeNode{
    public:
        Axiom(Attributes&& _attributes, pExpression&& _expression)
            : attributes(_attributes), expression(_expression)
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
        SpecExpression(pExpression&& e, bool isFree)
            : e(e), isFree(isFree){}
        pExpression e;
        bool isFree;
    };
    
    typedef unique_ptr<SpecExpression> pSpecExpression;
    class ProcSignature : public PTreeNode{
        vector<pLocalVariable> inputs;
        vector<pLocalVariable> outputs;

        vector<pVariableExpression> modifies;
        vector<pSpecExpression> preconditions;
        vector<pSpecExpression> postconditions;
    };
    typedef unique_ptr<ProcSignature> pProcSignature;
    
    class ProcedureSC : public PTreeNode{
    protected:
        ProcedureSC(pIdentifier&& _id, Attributes&& _attributes, pProcSignature&& _sig)
            : id(_id), attributes(_attributes), sig(_sig) {}
    public:
        virtual ~ProcedureSC() = 0;
        pIdentifier id;
        Attributes attributes;
        ProcSignature sig;
    };
    class Procedure : public ProcedureSC{
        Procedure(pIdentifier&& _id, Attributes&& _attributes, pProcSignature&& _sig)
            : ProcedureSC( _id, _attributes, _sig) {}
    };
    typedef unique_ptr<Procedure> pProcedure;
    
    class Statement;
    typedef unique_ptr<Statement> pStatement;
    typedef vector<Variable> pLocals;
    
    class Implementation : public PTreeNode{
        Implementation(
            pIdentifier&& id, Attributes&& attributes, pProcSignature&& sig, vector<pVariable>&& pLocals, pStatement&& body)
            : ProcedureSC( id, attributes, sig), locals(locals), body(body) {}
        pLocals locals;
        pStatement body;
    };
    typedef unique_ptr<Implementation> pImplementation;
    // </editor-fold>
    
    // <editor-fold desc="Attributes and triggers">
    class Attribute;
    typedef vector<Attribute> Attributes;

    class Trigger;
    typedef vector<Trigger>   Triggers;

    class AttributeParam;
    typedef unique_ptr<AttributeParam> pAttributeParam;
    class Attribute : public PTreeNode{
    public:
        pIdentifier id;
        list<pAttributeParam> params;
    };
    class AttributeParam : public PTreeNode{
    public:
        virtual ~AttributeParam() = 0;
    };
    class StringAttributeParam : public AttributeParam{
    public:
        StringAttributeParam(const string& value)
            : value(value){}
        std::string value;
    };
    class ExpressionAttributeParam : public AttributeParam{
    public:
        ExpressionAttributeParam(pExpression _value)
            : value(_value){}
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
    class WExpression : public PTreeNode{};
    typedef unique_ptr<WExpression> pWExpression;
    
    class Expression : public WExpression{
    public:
        virtual ~Expression() = 0;
        static void make(const TextPosition& _textPos,const Operation op, std::initializer_list<unique_ptr<Expression>>);
    };
    typedef unique_ptr<Expression> pExpression;
    
    class VariableExpression : public Expression{
    public:
        VariableExpression(pIdentifier&& id) : id(id) {}
        pIdentifier id;
    };
    typedef unique_ptr<VariableExpression> pVariableExpression;
    
    enum class Binder{ forall, exists, lambda };
    class Operation{};
    typedef unique_ptr<Operation> pOperation;
    
    class FAExpression : public Expression{
    public:
        FAExpression(pOperation&& _op, vector<pExpression>&& _args)
            : op(_op), args(_args){}
        pOperation op;
        vector<pExpression> args;
    };
    class QExpression : public Expression{
    public:
        QExpression(Binder _binder, TypeParameters&& _tVars, vector<pBoundVariable>&& _vars, pExpression _e)
            : binder(_binder), tVars(_tParams), vars(_vars), e(_e) {}
        Binder binder;
        TypeParameters tVars;
        vector<pBoundVariable> vars;
        pExpression e;
    };
    class OldExpression final : public Expression{
    public:
        OldExpression(pExpression&& e) : e(e){}
        pExpression e;
    };
    class LiteralExpression : public Expression{};
    template<class T>class LiteralExpressionC : public LiteralExpression{
    public:
        LiteralExpressionC(T&& val) : val(_val){}
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
    
    class Block;
    typedef unique_ptr<Block> pBlock;
    
    class CodeExpression : public Expression{
    public:
        CodeExpression(vector<pBlock>&& blocks) : blocks(blocks){}
        vector<pBlock> blocks;
    };
    // </editor-fold>

    // <editor-fold desc="Statements">
    class LabelOrStatement : public PTreeNode{};
    class Label : public LabelOrStatement{
    public:
        Label(pIdentifier&& id) : id(id){}
        pIdentifier id;
    };
    typedef unique_ptr<Label> pLabel;
    
    class Statement : public LabelOrStatement{
    public:
        virtual ~Statement() = 0;
    };
    
    class PredicateStatement : public Statement{
    public:
        pExpression e;
    protected:
        PredicateStatement(pExpression&& e) : e(e){}
    };
    class AssertStatement : public PredicateStatement{
    public:
        AssertStatement(pExpression&& e) : PredicateStatement(e) {}
    };
    class AssumeStatement : public PredicateStatement{
    public:
        AssumeStatement(pExpression&& e) : PredicateStatement(e) {}
    };
    
    class HavocStatement : public Statement{
    public:
        HavocStatement(list<pVariable>&& vars) : vars(vars) {}
        list<pVariable> vars;
    };

    class AssignLHS : public PTreeNode{};
    class AssignLHSVar : public AssignLHS{
    public:
        AssignLHSVar(pIdentifier&& id) : id(id){}
        pIdentifier id;
    };
    class MapSelect : public PTreeNode{
    public:
        MapSelect(vector<pExpression>&& indices) : indices(indices){}
        vector<pExpression> indices;
    };
    typedef unique_ptr<MapSelect> pMapSelect;
    class AssignLHSMap : public AssignLHS{
        AssignLHSMap(pIdentifier&& id, vector<pMapSelect>&& mss) : id(id), mss(mss){}
        pIdentifier id;
        vector<pMapSelect> mss;
    };
    typedef unique_ptr<AssignLHS> pAssignLHS;
    class AssignStatement : public Statement{
    public:
        AssignStatement(vector<pAssignLHS>&& lhss, vector<pExpression>&& rhss) 
            : lhss(lhss), rhss(rhss) {}
        list<pAssignLHS> lhss;
        list<pExpression> rhss;
    };

    class CallStatement : public Statement{
    public:
        CallStatement(vector<pVariable>&& lhss, pIdentifier&& procId, vector<pExpression>&& args) 
            : lhss(lhss), procId(procId), args(args) {}
        list<pAssignLHS> lhss;
        pIdentifier procId;
        list<pExpression> args;
    };

    class CallForallStatement : public Statement{
    public:
        CallForallStatement(pIdentifier&& procId, vector<pWExpression>&& args) 
            : procId(procId), args(args) {}
        pIdentifier procId;
        list<pWExpression> args;
    };

    class SkipStatement : public Statement{};
    
    class ITEStatement : public Statement{
    public:
        ITEStatement(pWExpression&& cond, pStatement&& thenS, pStatement&& elseS = new SkipStatement)
            : cond(cond), thenS(thenS), elseS(elseS){}
        pWExpression cond;
        pStatement   thenS;
        pStatement   elseS;
    };

    class WhileStatement : public Statement{
    public:
        WhileStatement(pWExpression&& cond, vector<SpecExpression>&& invariant, pStatement&& body)
            : cond(cond), invariant(invariant), body(body){}
        pWExpression cond;
        vector<SpecExpression> invariant;
        pStatement   body;
    };

    class BreakStatement : public Statement{
    public:
        BreakStatement(pLabel&& target) : target(target){}
        
        pLabel target;
    };

    class GotoStatement : public Statement{
    public:
        GotoStatement(vector<pLabel>&& targets) : targets(targets){}
        
        vector<pLabel> targets;
    };

    class ReturnStatement : public Statement{
    public:
        ReturnStatement() {}
    };
    
    class Block : public PTreeNode{
    public:
        Block(vector<pStatement>&& stmnts) : stmnts(stmnts) {}
        vector<pStatement> stmnts;
    };
    // </editor-fold>

}//namespace AST
}//namespace frontend
}//namespace boogie
#endif
;