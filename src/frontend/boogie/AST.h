#ifndef FRONTEND_BOOGIE_AST_H
#define FRONTEND_BOOGIE_AST_H

#include <set>
#include <vector>
#include <memory>
#include <list>
#include "common/data_types.h"
#include "frontend/TextPosition.h"

namespace frontend{
namespace boogie{
namespace AST{
    using common::String;
    using String = common::String;
    using frontend::TextPosition;
    
    using namespace std;

    using std::unique_ptr;
    using std::vector;
    using std::string;
    using std::list;

    // <editor-fold desc="utilities">
    template<typename T,template<U> C>list<T>&& clone(const C<T>& in){
        C<T> out;
        for (const auto& e : in)
            out.push_back(e->clone());
        return out;
    }
    // </editor-fold>

    // <editor-fold desc="ASTNode">
    class ASTNode{
    public:
    	TextPosition textPos;
    protected:
    	ASTNode(const TextPosition& _textPos = TextPosition::NoPos) 
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
    class Identifier : public ASTNode{
    public:
        Identifier(TextPosition _pos, const common::String& _name)
        : ASTNode(_pos), name(_name){}
        String name;
    };
    // </editor-fold>
    
    // <editor-fold desc="Types">
    /////////////////////////////////////////////////////////////
    //Types
    class Type : public ASTNode{
    public:
        pType clone() const = 0;
        virtual ~Type() = 0;
    };
    typedef unique_ptr<Type> pType;

    class TypeVariable : public ASTNode{
    public:
        TypeVariable(pIdentifier&& _id) : id(_id){}
        pIdentifier id;
    };
    typedef unique_ptr<TypeVariable> pTypeVariable;
    typedef list<pTypeVariable> TypeParameters;
    
    class TypeConstructorInstance : public Type{
    public:
        TypeConstructorInstance(pIdentifier&& _id, list<pType>&& _arguments)
            : id(_id), arguments(_arguments)
        {}
            
        pIdentifier id;
        list<pType> arguments;
    };
    class MapType : public Type{
    public:
        MapType(TypeParameters&& _typeParameters, vector<pType>&& _argumentTypes, pType&& _resultType)
                : TypeParameters(_typeParameters), argumentTypes(_argumentTypes), resultType(_resultType)
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

    class TypeDef  : public ASTNode{
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
    class Variable : public ASTNode{
    public:
        static constexpr int GLOBAL   = 1 << 0;
        static constexpr int INPUT    = 1 << 1;
        static constexpr int OUTPUT   = 1 << 2;
        static constexpr int CONSTANT = 1 << 3;
    	Variable(const TextPosition _textPos,
                 Attributes&& _attributes,
                 pIdentifier&& _id,
                 pType&& _type,
                 int _flags)
    		: ASTNode(_textPos), 
                  id(move(_id)), 
                  type(move(_type)), 
                  attributes(move(_attributes)),
                  flags(_flags)
            {}
    	virtual ~Variable(){}
    	pIdentifier id;
    	pType       type;
    	Attributes attributes;
        
        int flags;
        bool isGlobal(){return flags&GLOBAL; }
        bool isInput() {return flags&INPUT; }
        bool isOutput(){return flags&OUTPUT; }
        bool isConstant(){return flags&CONSTANT; }
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
                   Variable::GLOBAL),
                   isUnique(_isUnique), 
                   parentSpec(move(_parentSpec))
        {}
        bool isUnique;
        pConstantParentSpec parentSpec;
    };
    class ConstantParentSpec: public ASTNode {
    public:
        ConstantParentSpec(pIdentifier&& _id, bool _isUnique)
            : id(_id), isUnique(_isUnique)
            {}
        bool isUnique;
        pIdentifier id;
    };
    typedef unique_ptr<pConstantOrderSpec> pConstantOrderSpec;
    class ConstantOrderSpec : public ASTNode {
    	public:
    		bool specified = false;
    		bool ChildrenComplete = false;
    		vector<ConstantParentSpec> parents;
    		pConstantOrderSpec clone() const;
    };
    // </editor-fold>

    // <editor-fold desc="Functions">
    class Function : public ASTNode{
    public:
        Function(
        	const TextPosition& _textPos,
            Attributes&&      _attributes,
            pIdentifier&&     _id,
            TypeParameters&&  _typeParameters,
            list<pVariable>&& _parameters,
            pVariable&&       _returnVar,
            pExpression       _body)
            : ASTNode(_textPos),
              attributes    (_attributes),
              id            (_id),
              typeParameters(_typeParameters),
    	      parameters    (_parameters),
    	      returnVar     (_returnVar),
              body          (_body)
            {}
        Attributes      attributes;
        pIdentifier     id;
        TypeParameters  typeParameters;
        list<pVariable> parameters;
        pVariable       returnVar;
        pExpression     body;
    };
    typedef unique_ptr<FunctionDef> pFunction;
    // </editor-fold>

    // <editor-fold desc="Axioms">
    class Axiom : public ASTNode{
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
    class pVariableExpression;
    class SpecExpression : public ASTNode{
    public:
        SpecExpression(pExpression&& _e, bool _isFree)
            : e(_e), isFree(_isFree){}
        pExpression e;
        bool isFree;
    };
    
    typedef unique_ptr<SpecExpression> pSpecExpression;
    class ProcSignature : public ASTNode{
        vector<pVariable> inputs;
        vector<pVariable> outputs;

        vector<pVariableExpression> modifies;
        vector<pSpecExpression> preconditions;
        vector<pSpecExpression> postconditions;
    };
    typedef unique_ptr<ProcSignature> pProcSignature;
    
    class ProcedureSC : public ASTNode{
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
    
    class Implementation : public ASTNode{
        Implementation(
            pIdentifier&& _id, Attributes&& _attributes, pProcSignature&& _sig, vector<pVariable>&& pLocals, pStatement&& _body)
            : ProcedureSC( _id, _attributes, _sig), locals(_locals), body(_body) {}
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
    class Attribute : public ASTNode{
    public:
        pIdentifier id;
        list<pAttributeParam> params;
    };
    class AttributeParam : public ASTNode{
    public:
        virtual ~AttributeParam() = 0;
    };
    class StringAttributeParam : public AttributeParam{
    public:
        public StringAttributeParam(const string& _value)
            : value(_value){}
        std::string value;
    };
    class ExpressionAttributeParam : public AttributeParam{
    public:
        ExpressionAttributeParam(pExpression _value)
            : value(_value){}
        pExpression value;
    };
    class Trigger : public ASTNode{
    public:
        Trigger(vector<pExpression>&& _ts)
            :ts(_ts){}
        vector<pExpression> ts;
    };
    // </editor-fold>

    // <editor-fold desc="Programs">
    class Scope : public ASTNode{
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
    class Expression : public ASTNode{
    public:
        virtual ~Expression() = 0;
        static void make(const TextPosition& _textPos,const Operation op, std::initializer_list<unique_ptr<Expression>>);
    };
    
    class Operation{};
    typedef unique_ptr<Operation> pOperation;
    class FAExpression : public Expression{
    public:
        FAExpression(pOperation&& _op, vector<pExpression>&& args)
            : op(_op), args(_args){}
        pOperation op;
        vector<pExpression> args;
    };
    // </editor-fold>

    // <editor-fold desc="Statements">
    // </editor-fold>

}//namespace AST
}//namespace frontend
}//namespace boogie
#endif
