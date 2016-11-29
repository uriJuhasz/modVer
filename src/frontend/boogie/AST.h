#ifndef FRONTEND_BOOGIE_AST_H
#define FRONTEND_BOOGIE_AST_H

#include <set>
#include <vector>
#include <memory>
#include "common/data_types.h"
#include "frontend/TextPosition.h"

namespace frontend{
namespace boogie{
namespace AST{
    using common::String;
    using String = common::String;
    using frontend::TextPosition;

    using std::unique_ptr;
    using std::vector;

    class ASTNode{
    public:
    	TextPosition textPos;
    protected:
    	ASTNode(const TextPosition& _textPos = TextPosition::NoPos) : textPos(_textPos){}
    };
    class Attributes;
    class Type : public ASTNode{public:unique_ptr<Attributes> clone() const;};
    class Identifier : public ASTNode{
    public:
        Identifier(const common::String& _name)
        : name(_name){}
        String name;
    };
    class TypeDef  : public ASTNode{};
    class Variable : public ASTNode{
    public:
    	Variable(
    			const TextPosition& _textPos,
				unique_ptr<Attributes> _attributes,
				const Identifier& _id,
				unique_ptr<Type> _type)
    		: ASTNode(_textPos), name(_id.name), type(move(_type)), attributes(move(_attributes))
    	{}
    	virtual ~Variable(){}
    	String name;
    	unique_ptr<Type>       type;
    	unique_ptr<Attributes> attributes;
    	virtual bool isGlobal()=0;
    };
    	class ConstantParentSpec;
    	class Constant : public Variable{
    	public:
    		Constant(
    			const TextPosition& _textPos,
				unique_ptr<Attributes> _attributes,
				const Identifier& _id,
				unique_ptr<Type> _type,
				bool _isUnique,
				unique_ptr<ConstantParentSpec> _parentSpec)
    			: Variable(_textPos,move(_attributes),_id,move(_type)), isUnique(_isUnique), parentSpec(move(_parentSpec))
    		{}
    		virtual ~Constant(){}
    		bool isUnique;
    		unique_ptr<ConstantParentSpec> parentSpec;
        	bool isGlobal(){return true;}
    	};
        class GlobalVariable : public Variable{
        public:
        	GlobalVariable(
        			const TextPosition& _textPos,
    				unique_ptr<Attributes> _attributes,
    				const Identifier& _id,
    				unique_ptr<Type> _type)
        		: Variable(_textPos,move(_attributes),_id, move(_type))
        	{}
        	virtual ~GlobalVariable(){}
        	String name;
        	unique_ptr<Type>       type;
        	unique_ptr<Attributes> attributes;
        	bool isGlobal(){return true;}
        };
    class FunctionDef : public ASTNode{};

    class ConstantParentSpec: public ASTNode {
    public:
    	bool unique;

    };
    class ConstantOrderSpec : public ASTNode {
    	public:
    		bool ChildrenComplete = false;
    		vector<ConstantParentSpec> parents;
    		unique_ptr<ConstantOrderSpec> clone() const;
    };



    class Axiom          : public ASTNode{};
    class Procedure      : public ASTNode{};
    class Implementation : public ASTNode{};
    
    class Attributes : public ASTNode{public:unique_ptr<Attributes> clone() const;};
    class TypedIdentifier : public Identifier{ public: Type type; };

    class ProcedureSignature : public ASTNode{};
    class ProcedureSpec : public ASTNode{};
    class ProcedureBody : public ASTNode{};
    class TypeArguments : public ASTNode{};

    class Scope : public ASTNode{
    public:
    	void addTypeDef    (unique_ptr<TypeDef> td);
    	void addVariableDef(unique_ptr<Variable> vd);
    	void addFunctionDef(unique_ptr<FunctionDef> fd);
    private:
    	vector<Variable> variables;
    	vector<TypeDef>     typeDefs;
    	vector<FunctionDef> functions;
    };
    class Program : public Scope{
    public:
    	void addAxiom(unique_ptr<Axiom> axiom);
    	void addProcedure(unique_ptr<Procedure> procedure);
    	void addImplementation(unique_ptr<Implementation> Implementation);
    private:
        vector<Axiom>          axioms;
        vector<Procedure>      procedures;
        vector<Implementation> implementations;
    };

}//namespace AST
}//namespace frontend
}//namespace boogie
#endif
