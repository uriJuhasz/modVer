#ifndef FRONTEND_BOOGIE_PARSETREE_H
#define FRONTEND_BOOGIE_PARSETREE_H

#include <set>
#include <vector>
#include <memory>
#include <type_traits>

#include "common/data_types.h"
#include "../TextPosition.h"

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

    using common::Integer;
    using common::Rational;
    
    typedef wstring IDString;

    // <editor-fold desc="utilities">
	template<class T> T cp(const T& in) {
		T out;
		for (const auto& e : in)
			out.push_back(cp(e));
		return move(out);
	}

	template<class T> unique_ptr<T> cp(const unique_ptr<T>& in)
	{
		return (in) ? in->clone() : unique_ptr<T>();
	}

	template<class T> T mOc(bool l, T&& in) {
		return l ? move(in) : cp(in);
	}

	
/*	template<template<typename> class C,typename T> C<unique_ptr<T>> clone(const C<unique_ptr<T>>& in){
//		static_assert(std::is_array<C>);
		T out;
        for (const auto& e : in)
            out.push_back(e->clone());
        return move(out);
    }
	*/

    template<typename C,typename E, unique_ptr<E> (E::*cloneM)() const> C cloneCM(const C& in){
        C out;
        for (const auto& e : in)
            out.push_back(((*e).*cloneM)());
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
		inline virtual ~PTreeNode() {}
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
    
    class BoundVariable;
    typedef unique_ptr<BoundVariable> pBoundVar;
    typedef vector<pBoundVar> BoundVariables;
   
    // </editor-fold>
    
    // <editor-fold desc="Expressions1">
    class WExpression : public PTreeNode{
    public:
        WExpression(TextPosition pos) : PTreeNode(pos) {}
		inline virtual ~WExpression() { }
    };
    typedef unique_ptr<WExpression> pWExpression;

    class WildCardExpression : public WExpression{
    public:
        WildCardExpression(TextPosition pos) : WExpression(pos) {}
		inline virtual ~WildCardExpression() {}
	};
    typedef unique_ptr<Expression> pExpression;

    class Expression;
    typedef unique_ptr<Expression> pExpression;
    class Expression : public WExpression{
    public:
		inline virtual ~Expression() {}
		virtual pExpression clone() const = 0;
//        static void make(const TextPosition& pos, const Operation op, std::initializer_list<unique_ptr<Expression>>);
    protected:
        Expression(TextPosition pos) : WExpression(pos) {}
    };
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
        pIdentifier clone()const{
			auto r = make_unique<Identifier>(pos,name); 
			return move(r);
		}
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
        virtual unique_ptr<TypeVariable> clone() const {return make_unique<TypeVariable>(cp(id)); }
        pIdentifier id;
    };
    typedef unique_ptr<TypeVariable> pTypeVariable;
    typedef vector<pTypeVariable> TypeParameters;
    
    class TypeConstructorInstance : public Type{
    public:
        TypeConstructorInstance(TextPosition pos, pIdentifier&& id, Types&& arguments)
            : Type(pos), id(move(id)), arguments(move(arguments))
        {}
        virtual pType clone() const override;
        pIdentifier id;
        Types arguments;
    };
    class MapType : public Type{
    public:
        MapType(TextPosition pos, TypeParameters&& typeParameters,
                Types&& argumentTypes, pType&& resultType)
                : Type(pos), typeParameters(move(typeParameters)),
                  argumentTypes(move(argumentTypes)), resultType(move(resultType))
        {}
        MapType(const MapType& o)
                : Type(o.pos), typeParameters(cp(o.typeParameters)),
                  argumentTypes(cp(o.argumentTypes)), resultType(cp(o.resultType))
        {}
        virtual pType clone() const override{return make_unique<MapType>(*this); }
        TypeParameters typeParameters;
        Types argumentTypes;
        pType resultType; 
    };
    class VariableType : public Type{
    public:
        VariableType(TextPosition pos, pTypeVariable&& var)
            : Type(pos), var(move(var)){}
        virtual pType clone() const override;
        pTypeVariable var;
    };

    class UnresolvedType : public Type{
    public:
        UnresolvedType(TextPosition pos, pIdentifier&& id, Types&& args)
            : Type(pos), id(move(id)), args(move(args)){}
        pType clone() const override{ return make_unique<UnresolvedType>(pos,cp(id), cp(args)); }
        pIdentifier id;
        Types args;
    };

    class TypeDef  : public PTreeNode{
    public: 
        TypeDef(pIdentifier&& id, Attributes&& attributes, TypeParameters&& parameters)
            : id(move(id)), attributes(move(attributes)), parameters(move(parameters)){}
        virtual ~TypeDef() = 0;
        pIdentifier id;
		Attributes attributes;
        TypeParameters parameters;
    };
    typedef unique_ptr<TypeDef> pTypeDef;
    
    class TypeConstructorDef : public TypeDef{
    public:
        TypeConstructorDef(pIdentifier&& id, Attributes&& attributes, TypeParameters&& parameters)
            : TypeDef(move(id),move(attributes),move(parameters)){}
    };
    class TypeSynonymDef : public TypeDef{
    public:
        TypeSynonymDef(pIdentifier&& id, Attributes&& attributes, TypeParameters&& parameters, pType&& target)
            : TypeDef(move(id),move(attributes), move(parameters)), target(move(target)){}
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
        
        bool isGlobal()  const {return flags&GLOBAL; }
        bool isInput()   const {return flags&INPUT; }
        bool isOutput()  const {return flags&OUTPUT; }
        bool isConstant()const {return flags&CONSTANT; }
        bool isBound()   const {return flags&BOUND; }
    };

    typedef unique_ptr<Variable> pVariable;
    typedef vector<pVariable> Variables;

    class ConstantOrderSpec;
    typedef unique_ptr<ConstantOrderSpec> pConstantOrderSpec;
    class Constant : public Variable{
    public:
        Constant(const TextPosition& pos,
                 Attributes&&  attributes,
                 pIdentifier&& id,
                 pType&&       type,
                 bool          isUnique,
                 pConstantOrderSpec orderSpec)
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
//        virtual unique_ptr<Constant> clone() const override{return make_unique<BoundVariable>(clone(attributes), id->clone(), type->clone()); }
    };
    class ConstantParentSpec: public PTreeNode {
    public:
        ConstantParentSpec(pIdentifier&& id, bool isUnique)
            : id(move(id)), isUnique(isUnique){}
        ConstantParentSpec(const ConstantParentSpec& o)
            : id(cp(o.id)), isUnique(o.isUnique){}
        ConstantParentSpec(ConstantParentSpec&& other)
            : id(move(other.id)), isUnique(other.isUnique){}

        pIdentifier id;
        bool isUnique;
    };
    class ConstantOrderSpec : public PTreeNode {
    public:
        ConstantOrderSpec();
        ConstantOrderSpec(const ConstantOrderSpec& o);
        pConstantOrderSpec clone() const;

        bool specified = false;
        bool complete = false;
        vector<ConstantParentSpec> parents;
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
        GlobalVariable(const GlobalVariable& o)
                : Variable(
                   o.pos,
                   cp(o.attributes),
                   cp(o.id),
                   cp(o.type),
                   Variable::GLOBAL),
                   we(cp(o.we))
        {}
        virtual unique_ptr<GlobalVariable> clone() const{ return make_unique<GlobalVariable>(*this); }
        //{return make_unique<GlobalVariable>(pos,clone(attributes), id->clone(), type->clone(),we->clone()); }
        pExpression we;
    };
    
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
        virtual unique_ptr<BoundVariable> clone() const {return make_unique<BoundVariable>(cp(attributes), cp(id), cp(type)); }
    };
    class LocalVariable : public Variable{
    public:
        LocalVariable( 
                 Attributes&&  attributes,
                 pIdentifier&& id,
                 pType&&       type,
				 pExpression&& we
			)
                : Variable(
                   id->pos,
                   move(attributes),
                   move(id),
                   move(type),
                   0),
			       we(move(we))
        {}
        virtual unique_ptr<LocalVariable> clone() const {return make_unique<LocalVariable>(cp(attributes), cp(id), cp(type),cp(we)); }
		pExpression we;
    };
    enum class FormalDir{ In, Out };
    
    class Formal;
    typedef unique_ptr<Formal>         pFormal;
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

        pFormal cloneWOWhere() const{
            return make_unique<Formal>(cp(attributes),cp(id),cp(type),
                                       nullptr,isInput() ? FormalDir::In : FormalDir::Out);
        }
    };
    
    typedef unique_ptr<GlobalVariable> pGlobalVariable;
    typedef unique_ptr<LocalVariable>  pLocalVariable;
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

    class SpecExpression;
    typedef unique_ptr<SpecExpression> pSpecExpression;
    class SpecExpression : public PTreeNode{
    public:
        SpecExpression(const TextPosition& pos, bool isFree, pExpression&& e, Attributes attributes = Attributes())
            : PTreeNode(pos), isFree(isFree), e(move(e)), attributes(move(attributes)) {}
        SpecExpression(const SpecExpression& o)
            : PTreeNode(o.pos), isFree(o.isFree), e(cp(o.e)), attributes(cp(o.attributes)) {}
        pSpecExpression clone() const{return make_unique<SpecExpression>(*this); }
        bool isFree;
        pExpression e;
        Attributes attributes;
    };
    
    typedef vector<pFormal> Formals;
    typedef vector<pSpecExpression> SpecExpressions;
    
    class ProcSignature;
    typedef unique_ptr<ProcSignature> pProcSignature;
    class ProcSignature : public PTreeNode{
    public:
        TypeParameters tParams;
        Formals        formals;

        pProcSignature cloneWOWhere() const{
            auto r = make_unique<ProcSignature>();
            r->tParams=cp(tParams);
            for (auto& f : formals)
                r->formals.push_back(f->cloneWOWhere());
//            std::for_each(formals.begin(),formals.end(),[&r](auto& f){r->formals.push_back(f->cloneWOWhere());});
            return move(r);
        }
    };
    
    class Modifies;
    typedef unique_ptr<Modifies> pModifies;
    class Modifies : public PTreeNode{
    public:
        Modifies(TextPosition pos, bool free, pIdentifier&& id)
            : PTreeNode(pos), id(move(id)), free(free){}
        Modifies(const Modifies& o)
            : PTreeNode(o.pos), id(cp(o.id)), free(o.free){}
        pModifies clone() const{return make_unique<Modifies>(*this); }
        pIdentifier id;
        bool free;
    };
    
    class ProcSpec;
    typedef unique_ptr<ProcSpec> pProcSpec;
    class ProcSpec : public PTreeNode{
    public:
        ProcSpec(){}
        ProcSpec(const ProcSpec& o)
            : mod(cp(o.mod)), req(cp(o.req)), ens(cp(o.ens)) {}

        pProcSpec clone() const{return make_unique<ProcSpec>(*this); }

        vector<pModifies> mod;
        SpecExpressions req;
        SpecExpressions ens;
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
    public:
        Procedure(pIdentifier&& id, Attributes&& attributes, pProcSignature&& sig, pProcSpec&& spec)
            : ProcedureSC( move(id), move(attributes), move(sig), move(spec)) {}
    };
    typedef unique_ptr<Procedure> pProcedure;
    
    class Statement;
    typedef unique_ptr<Statement> pStatement;
    
    typedef vector<LocalVariable> pLocals;
    
    class ScopedBlock;
    typedef unique_ptr<ScopedBlock> pScopedBlock;
    
    class Implementation : public ProcedureSC{
    public:
        Implementation(
            pIdentifier&& id, Attributes&& attributes, pProcSignature&& sig, pProcSpec&& spec, pScopedBlock&& body)
            : ProcedureSC( move(id), move(attributes), move(sig), move(spec)), 
              body(move(body)) {}
        pScopedBlock body;
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
            return make_unique<Attribute>(cp(id),cp(params));
        }
        
        pIdentifier id;
        vector<pAttributeParam> params;
    };
    class AttributeParam : public PTreeNode{
    public:
        virtual ~AttributeParam() = 0;
        virtual pAttributeParam clone() const = 0;
    protected:
        AttributeParam(TextPosition pos) : PTreeNode(pos) {}
    };
    class StringAttributeParam : public AttributeParam{
    public:
        StringAttributeParam(TextPosition pos, const wstring& value)
            : AttributeParam(pos), value(value){}
        StringAttributeParam(const StringAttributeParam& o)
            : AttributeParam(o.pos), value(o.value){}
        wstring value;
        pAttributeParam clone() const override{return make_unique<StringAttributeParam>(*this);};
    };
    class ExpressionAttributeParam : public AttributeParam{
    public:
        ExpressionAttributeParam(pExpression&& value)
            : AttributeParam(value->pos),value(move(value)) {}
        ExpressionAttributeParam(const ExpressionAttributeParam& o)
            : AttributeParam(o.pos), value(cp(o.value)) {}
        pAttributeParam clone() const override{ return make_unique<ExpressionAttributeParam>(*this); }

        pExpression value;
    };
    class Trigger : public PTreeNode{
    public:
        Trigger(){}
        Trigger(Expressions&& es)
            :es(move(es)){}
        Trigger(const Trigger& o)
            :es(cp(o.es)){}
        pTrigger clone() const{return make_unique<Trigger>(*this); }
        Expressions es;
    };
    // </editor-fold>

    // <editor-fold desc="Programs">
    class Scope : public PTreeNode{
    public:
        void addTypeDef    (pTypeDef&&  t){ typeDefs.push_back(move(t)); }
        void addVariableDef(pVariable&& v){ variables.push_back(move(v)); }
        void addFunctionDef(pFunction&& f){functions.push_back(move(f)); }

        vector<pVariable> variables;
    	vector<pTypeDef>  typeDefs;
    	vector<pFunction> functions;
    };

    class Program : public Scope{
    public:
        void addAxiom         (pAxiom&&          a){axioms.push_back(move(a)); }
        void addProcedure     (pProcedure&&      p){procedures.push_back(move(p)); }
        void addImplementation(pImplementation&& i){implementations.push_back(move(i));}
        vector<pAxiom>          axioms;
        vector<pProcedure>      procedures;
        vector<pImplementation> implementations;
    private:
    };
	typedef unique_ptr<Program> pProgram;
//    class Operation : public ASTNode{};
    // </editor-fold>

    class VariableExpression : public Expression{
    public:
        VariableExpression(pIdentifier&& id) : Expression(id->pos), id(move(id)) {}
        virtual pExpression clone() const override{ return make_unique<VariableExpression>(cp(id)); }
        pIdentifier id;
    };
    
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
        static pOperator implies(TextPosition pos){return make_unique<OpC>(pos,L"=>");}
        static pOperator explies(TextPosition pos){return make_unique<OpC>(pos,L"<=");}

        static pOperator equiv(TextPosition pos){return make_unique<OpC>(pos,L"<=>");}
        static pOperator andOp(TextPosition pos){return make_unique<OpC>(pos,L"&&");}
        static pOperator orOp(TextPosition pos){return make_unique<OpC>(pos,L"||");}
        static pOperator notOp(TextPosition pos){return make_unique<OpC>(pos,L"!");}
        
        static pOperator ite(TextPosition pos){return make_unique<OpC>(pos,L"ite");}

        static pOperator minus(TextPosition pos){return make_unique<OpC>(pos,L"-");}
        static pOperator add(TextPosition pos){return make_unique<OpC>(pos,L"+");}
        static pOperator sub(TextPosition pos){return make_unique<OpC>(pos,L"-");}
        static pOperator mul(TextPosition pos){return make_unique<OpC>(pos,L"mul");}
        static pOperator div(TextPosition pos){return make_unique<OpC>(pos,L"div");}
        static pOperator mod(TextPosition pos){return make_unique<OpC>(pos,L"%");}
        static pOperator divR(TextPosition pos){return make_unique<OpC>(pos,L"/");}
        static pOperator pow(TextPosition pos){return make_unique<OpC>(pos,L"pow");}
        
        static pOperator bvSelect(TextPosition pos){return make_unique<OpC>(pos,L"[:]");}
        static pOperator bvConcat(TextPosition pos){return make_unique<OpC>(pos,L"++");}

        static pOperator eq(TextPosition pos){return make_unique<OpC>(pos,L"=");}
        static pOperator ne(TextPosition pos){return make_unique<OpC>(pos,L"!=");}
        static pOperator lt(TextPosition pos){return make_unique<OpC>(pos,L"<");}
        static pOperator le(TextPosition pos){return make_unique<OpC>(pos,L"<=");}
        static pOperator gt(TextPosition pos){return make_unique<OpC>(pos,L">");}
        static pOperator ge(TextPosition pos){return make_unique<OpC>(pos,L">=");}

        static pOperator subType(TextPosition pos){return make_unique<OpC>(pos,L"<:");}
        
        static pOperator mapWrite(TextPosition pos){return make_unique<OpC>(pos,L"[:=]");}
        static pOperator mapRead(TextPosition pos){return make_unique<OpC>(pos,L"[]");}
        wstring name;
    };
    
    
    class UserDefinedOp  : public Operator{
    public:
        UserDefinedOp(pIdentifier&& id) : Operator(id->pos), id(move(id)){}
        unique_ptr<Operator> clone() const override{ return make_unique<UserDefinedOp>(cp(id)); }
        pIdentifier id;
        
        static pOperator make(pIdentifier&& id){return make_unique<UserDefinedOp>(move(id)); }
    };
    class CastOp  : public Operator{
    public:
        CastOp(TextPosition pos,pType&& type) : Operator(pos), type(move(type)){}
        unique_ptr<Operator> clone() const override{ return make_unique<CastOp>(pos,cp(type)); }
        pType type;
    };

    
    class FAExpression;
    typedef unique_ptr<FAExpression> pFAExpression;
    class FAExpression : public Expression{
    public:
        FAExpression(pOperator&& op, Expressions&& args)
            : Expression(op->pos), op(move(op)), args(move(args)){}
        virtual pExpression clone() const override{ return make_unique<FAExpression>(cp(op),cp(args)); }

        pOperator op;
        Expressions args;
        
        static pFAExpression make(pOperator&& op){ return make(move(op),Expressions()); }
        static pFAExpression make(pOperator&& op,pExpression&& e0){ 
            Expressions es; es.push_back(move(e0));
            return make(move(op),move(es)); 
        }
        static pFAExpression make(pOperator&& op,pExpression&& e0, pExpression&& e1){ 
            Expressions es; es.push_back(move(e0)); es.push_back(move(e1));
            return make(move(op),move(es)); 
        }
        static pFAExpression make(pOperator&& op,pExpression&& e0, pExpression&& e1, pExpression&& e2){ 
            Expressions es; es.push_back(move(e0)); es.push_back(move(e1)); es.push_back(move(e2));
            return make(move(op),move(es)); 
        }
        static pFAExpression make(pOperator&& op,Expressions&& args){
            return make_unique<FAExpression>(move(op), move(args));
        }
            
    };
    class QExpression : public Expression{
    public:
        QExpression(TextPosition pos, Binder binder, Attributes&& attributes, Triggers&& triggers,
                    TypeParameters&& tVars, BoundVariables&& vars, pExpression e)
            : Expression(pos), binder(binder), attributes(move(attributes)), triggers(move(triggers)),
              tVars(move(tVars)), vars(move(vars)), e(move(e)) {}
        QExpression(const QExpression& o)
            : Expression(o.pos), binder(o.binder), attributes(cp(o.attributes)),
              triggers(cp(o.triggers)), tVars(cp(o.tVars)), vars(cp(o.vars)), e(cp(o.e)) {}
        virtual pExpression clone() const override{ return make_unique<QExpression>(*this); }

        Binder         binder;
        Attributes     attributes;
        Triggers       triggers;
        TypeParameters tVars;
        BoundVariables vars;
        pExpression    e;
    };
    class OldExpression : public Expression{
    public:
        OldExpression(TextPosition pos, pExpression&& e) : Expression(pos), e(move(e)){}
        OldExpression(const OldExpression& o) :
            Expression(o.pos), e(cp(o.e)){}
        virtual pExpression clone() const override{return make_unique<OldExpression>(*this);}
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
        LiteralExpressionC(const LiteralExpressionC<T>& o) :
            LiteralExpression(o.pos), val(o.val){}
        T val;
        virtual pExpression clone() const override{return make_unique<LiteralExpressionC<T>>(*this); }
        
        
    };
    
    class BVConstant{ 
    public:
        BVConstant(int _numBits, Integer&& _value)
            : numBits(_numBits), value(_value){}
        int numBits;
        Integer value;
    };

    enum class Sign{ Pos,Neg };

    class Float{
    public:
        Float():mantissa(0),mantissaBits(23),exponent(0),exponentBits(8),sgnB(0), nanB(0), infB(0){}
        Float(Sign sign, Integer mantissa, int mantissaBits, Integer exponent, int exponentBits)
            : mantissa(mantissa), mantissaBits(mantissaBits),
              exponent(exponent), exponentBits(exponentBits),
              sgnB(sign2Int(sign)), nanB(0), infB(0)
        {}
        static Float nan(          int32_t mL, int32_t eL){Float r; r.nanB=1; r.mantissaBits=mL; r.exponentBits=eL;                      return r;}
        static Float inf(Sign sgn, int32_t mL, int32_t eL){Float r; r.infB=1; r.mantissaBits=mL; r.exponentBits=eL;r.sgnB=sign2Int(sgn); return r;}

        Sign sign(){return sgnB==1 ? Sign::Neg : Sign::Pos; }
        bool isNan(){return nanB==1;}
        bool isInf(){return infB==1;}
        Integer mantissa;
        int mantissaBits;
        Integer exponent;
        int exponentBits;
    private:
        static int sign2Int(Sign sgn)
        { return (sgn==Sign::Neg) ? 1 : 0; }

        unsigned char sgnB : 1;
        unsigned char nanB : 1;
        unsigned char infB : 1;
    };
    Float    string2Float   (const String& s);
    
    typedef LiteralExpressionC<bool>       BooleanLiteralExpression;
    typedef LiteralExpressionC<Integer>    IntegerLiteralExpression;
    typedef LiteralExpressionC<Rational>   RationalLiteralExpression;
    typedef LiteralExpressionC<Float>      FloatLiteralExpression;
    typedef LiteralExpressionC<BVConstant> BVLiteralExpression;
    
    class CBBlockStatement;
    typedef unique_ptr<CBBlockStatement> pCBBlockStatement;
    
    typedef vector<pLocalVariable> Locals;

    class CodeExpression : public Expression{
    public:
        CodeExpression(TextPosition pos, Locals&& locals, pCBBlockStatement&& ss)
            : Expression(pos), locals(move(locals)), ss(move(ss)){}
        CodeExpression(const CodeExpression& o);
//            : Expression(o.pos), locals(cp(o.locals)), ss(o.ss->clone()){}
        virtual pExpression clone() const override;
        Locals locals;
        pCBBlockStatement ss;
    };
    // </editor-fold>

    // <editor-fold desc="Statements">
    class Statement           : public PTreeNode{
        public:
        using PTreeNode::PTreeNode;
        virtual ~Statement() = 0;
    };
    class NSStatement         : public virtual Statement{using Statement::Statement;};

    class SimpleStatement;
    typedef unique_ptr<SimpleStatement> pSimpleStatement;
    class SimpleStatement     : public Statement{
    public:
        using Statement::Statement;
        virtual pSimpleStatement clone() const = 0;
    };
    class CoreStatement       : public SimpleStatement{using SimpleStatement::SimpleStatement;};
    class CompoundStatement   : public NSStatement{using NSStatement::NSStatement;};
    class ControlStatement    : public NSStatement{using NSStatement::NSStatement;};

    class CBControlStatement;
    typedef unique_ptr<CBControlStatement> pCBControlStatement;
    class CBControlStatement  : public NSStatement{
    public:
        using NSStatement::NSStatement;
        virtual pCBControlStatement clone() const = 0;
    };
    
    typedef unique_ptr<Statement> pStatement;
    typedef vector<pStatement> StatementSeq;
    
    typedef vector<pSimpleStatement> SimpleStatements;
    
    typedef unique_ptr<NSStatement> pNSStatement;
    typedef unique_ptr<ControlStatement> pControlStatement;
    typedef unique_ptr<CompoundStatement> pCompoundStatement;

    class Label;
    typedef unique_ptr<Label> pLabel;
    class Label : public PTreeNode{
    public:
        Label(pIdentifier&& id) : id(move(id)){}
        Label(const Label& o) : id(cp(o.id)){}
        pLabel clone() const{return make_unique<Label>(*this); }

        pIdentifier id;
    };
    typedef vector<pLabel> Labels;
    
    class SBlock : public PTreeNode{
    public:
        Labels            labels;
        SimpleStatements  statements;
        pNSStatement      control;
        
        SBlock(){}
        SBlock(Labels&& ls, SimpleStatements&& ss, pNSStatement&& cs)
            : labels(move(ls)),statements(move(ss)),control(move(cs)){}
        SBlock(pNSStatement&& cs) : PTreeNode(pos), control(move(cs)){}
    };
    typedef unique_ptr<SBlock> pSBlock;
    typedef vector<pSBlock> SBlocks;
    
    class BlockStatement;
    typedef unique_ptr<BlockStatement> pBlockStatement;
    

    class BlockStatement : public Statement{
    public:
        BlockStatement(TextPosition pos, SBlocks&& bs)
        : Statement(pos), bs(move(bs)) {}
        SBlocks bs;
    };


    class CBSBlock;
    typedef unique_ptr<CBSBlock> pCBSBlock;
    class CBSBlock : public PTreeNode{
    public:
        CBSBlock(pLabel&& l, SimpleStatements&& ss, pCBControlStatement cs)
            : label(move(l)), statements(move(ss)), control(move(cs)){}
        CBSBlock(const CBSBlock& o)
            : label(cp(o.label)), statements(cp(o.statements)), control(cp(o.control)){}
        pCBSBlock clone() const{return make_unique<CBSBlock>(*this); }
        pLabel               label;
        SimpleStatements    statements;
        pCBControlStatement control;
    };
    typedef vector<pCBSBlock> CBSBlocks;

    class CBBlockStatement;
    typedef unique_ptr<CBBlockStatement> pCBBlockStatement;
    class CBBlockStatement : public Statement{
    public:
        CBBlockStatement(CBSBlocks&& bs)
            : Statement(), bs(move(bs)) {}
        CBBlockStatement(const CBBlockStatement& o)
            : Statement(), bs(cp(o.bs)) {}
        pCBBlockStatement clone() const{return make_unique<CBBlockStatement>(*this); }

        CBSBlocks bs;
    };
    
    class ScopedBlock : public PTreeNode{
    public:
        Locals locals;
        pBlockStatement bs;
    };
    typedef unique_ptr<ScopedBlock> pScopedBlocks;
    
    class PredicateStatement : public CoreStatement{
    public:
        Attributes attributes;
        pExpression e;
        PredicateStatement(TextPosition pos,Attributes&& attributes,pExpression&& e) 
        : CoreStatement(pos), attributes(move(attributes)), e(move(e)){}
    protected:
        PredicateStatement(const PredicateStatement& o)
        : CoreStatement(o.pos), attributes(cp(o.attributes)), e(cp(o.e)){}
    };
    typedef unique_ptr<PredicateStatement> pPredicateStatement;
    
    class AssertStatement : public PredicateStatement{
    public:
        using PredicateStatement::PredicateStatement;
        pSimpleStatement clone() const override{ return make_unique<AssertStatement>(*this); }
    };
    class AssumeStatement : public PredicateStatement{
    public:
        using PredicateStatement::PredicateStatement;
        pSimpleStatement clone() const override{ return make_unique<AssumeStatement>(*this); }
    };
    
    class HavocStatement : public CoreStatement{
    public:
        HavocStatement(TextPosition pos, Identifiers&& ids)
            : CoreStatement(pos), ids(move(ids)) {}
        HavocStatement(const HavocStatement& o)
            : CoreStatement(o.pos), ids(cp(o.ids)) {}
        pSimpleStatement clone() const override{ return make_unique<HavocStatement>(*this); }
        Identifiers ids;
    };

    // <editor-fold desc="Assignment">
    class AssignLHS;
    typedef unique_ptr<AssignLHS> pAssignLHS;
    class AssignLHS : public PTreeNode{
    public:
        using PTreeNode::PTreeNode;
        virtual pAssignLHS clone() const = 0;
    };
    typedef vector<pAssignLHS>    AssignLHSs;

    class AssignLHSVar : public AssignLHS{
    public:
        AssignLHSVar(pIdentifier&& id) : AssignLHS(id->pos), id(move(id)){}
        AssignLHSVar(const AssignLHSVar& o)
            : AssignLHS(o.id->pos), id(cp(o.id)){}
        pAssignLHS clone() const override{ return make_unique<AssignLHSVar>(*this);}

        pIdentifier id;
    };
    
    class MapSelect;
    typedef unique_ptr<MapSelect> pMapSelect;
    class MapSelect : public PTreeNode{
    public:
        MapSelect(TextPosition pos,Expressions&& indices)
            : PTreeNode(pos), indices(move(indices)){}
        MapSelect(const MapSelect& o)
            : PTreeNode(o.pos), indices(cp(o.indices)){}
        pMapSelect clone() const{ return make_unique<MapSelect>(*this);}

        Expressions indices;
    };
    typedef vector<pMapSelect> MapSelects;
    
    class AssignLHSMap : public AssignLHS{
    public:
        AssignLHSMap(pIdentifier&& id, MapSelects&& mss) 
            : AssignLHS(id->pos), id(move(id)), mss(move(mss)){}
        AssignLHSMap(const AssignLHSMap& o)
            : AssignLHS(o.pos), id(cp(o.id)), mss(cp(o.mss)){}
        pAssignLHS clone() const override{ return make_unique<AssignLHSMap>(*this); };

        pIdentifier id;
        MapSelects mss;
    };
    typedef unique_ptr<AssignLHS> pAssignLHS;
    typedef vector<pAssignLHS>    AssignLHSs;
    
    class AssignStatement;
    typedef unique_ptr<AssignStatement> pAssignStatement;
    class AssignStatement : public SimpleStatement{
    public:
        AssignStatement(TextPosition pos, AssignLHSs&& lhss, Expressions&& rhss) 
             : SimpleStatement(pos), lhss(move(lhss)), rhss(move(rhss)) {}
        AssignStatement(const AssignStatement& o)
             : SimpleStatement(pos), lhss(cp(lhss)), rhss(cp(rhss)) {}
        virtual pSimpleStatement clone() const override{ return unique_ptr<SimpleStatement>(new AssignStatement(*this)); }
        AssignLHSs  lhss;
        Expressions rhss;

    };
    // </editor-fold>

    class CallStatement;
    typedef unique_ptr<CallStatement> pCallStatement;
    class CallStatement : public SimpleStatement{
    public:
        CallStatement(TextPosition pos,  pIdentifier&& procId, Identifiers&& lhss, vector<pExpression>&& args, bool isFree = false, bool isAsync = false) 
            :  SimpleStatement(pos), procId(move(procId)), lhss(move(lhss)), args(move(args)), isFree(isFree), isAsync(isAsync) {}
        CallStatement(const CallStatement& o)
            :  SimpleStatement(o.pos), procId(cp(o.procId)), lhss(cp(o.lhss)),
              args(cp(o.args)), isFree(o.isFree), isAsync(o.isAsync) {}
        virtual pSimpleStatement clone() const override { return cloneCS(); }
        virtual pCallStatement cloneCS() const { return make_unique<CallStatement>(*this); }
        pIdentifier procId;
        Identifiers lhss;
        Expressions args;
        bool isFree;
        bool isAsync;
    };
    typedef vector<pCallStatement> CallStatements;

    class ParallelCallStatement : public SimpleStatement{
    public:
        ParallelCallStatement(TextPosition pos,  Attributes&& attributes, CallStatements&& calls)
            :  SimpleStatement(pos), attributes(move(attributes)), calls(move(calls)){}
        ParallelCallStatement(const ParallelCallStatement& o)
            :  SimpleStatement(o.pos), attributes(cp(o.attributes)),
              calls(cloneCM<CallStatements,CallStatement,&CallStatement::cloneCS>(o.calls)){}
        virtual pSimpleStatement clone() const{ return make_unique<ParallelCallStatement>(*this); }

        Attributes attributes;
        CallStatements calls;
    };

    class YieldStatement : public SimpleStatement{
    public:
        YieldStatement(TextPosition pos) : SimpleStatement(pos){}
        YieldStatement(const YieldStatement& o) : SimpleStatement(o.pos){}
        virtual pSimpleStatement clone() const{ return make_unique<YieldStatement>(*this); }
    };

    class ITEStatement : public CompoundStatement{
    public:
        ITEStatement(TextPosition pos, pWExpression&& cond, pBlockStatement&& thenS)
            : ITEStatement(pos,move(cond),move(thenS),nullptr){}
        ITEStatement(TextPosition pos, pWExpression&& cond, pBlockStatement&& thenS, pBlockStatement&& elseS)
            : CompoundStatement(pos), cond(move(cond)), thenS(move(thenS)), elseS(move(elseS)){}
		~ITEStatement() {}
        pWExpression cond;
        pStatement   thenS;
        pStatement   elseS;
    };
    typedef unique_ptr<ITEStatement> pITEStatement;

    class WhileStatement : public CompoundStatement{
    public:
        WhileStatement(TextPosition pos, pWExpression&& cond, SpecExpressions&& invariant, pBlockStatement&& body)
            : CompoundStatement(pos), cond(move(cond)), invariant(move(invariant)), body(move(body)){}
		~WhileStatement() {}
		pWExpression    cond;
        SpecExpressions invariant;
        pBlockStatement body;
    };
    typedef unique_ptr<WhileStatement> pWhileStatement;

    class BreakStatement : public ControlStatement{
    public:
        BreakStatement(TextPosition pos, pIdentifier&& target) : ControlStatement(pos), target(move(target)){}
        
        pIdentifier target;
    };
    typedef unique_ptr<BreakStatement> pBreakStatement;

    class GotoStatement : public ControlStatement, public CBControlStatement{
    public:
        GotoStatement(TextPosition pos, Identifiers&& targets)
            : ControlStatement(pos), targets(move(targets)){}
        
        virtual pCBControlStatement clone() const override{
            return make_unique<GotoStatement>(pos,cp(targets));
        }
        Identifiers targets;
    };
    typedef unique_ptr<GotoStatement> pGotoStatement;
    
    class ReturnStatement : public ControlStatement{
    public:
        ReturnStatement(TextPosition pos) : ControlStatement(pos) {}
    };
    
    class ReturnEStatement : public CBControlStatement{
    public:
        ReturnEStatement(TextPosition pos,pExpression&& e) : CBControlStatement(pos), e(move(e)) {}

        virtual pCBControlStatement clone() const override{
            return make_unique<ReturnEStatement>(pos,cp(e));
        }

        pExpression e;
    };
    // </editor-fold>

}//namespace AST
}//namespace frontend
}//namespace boogie
#endif
