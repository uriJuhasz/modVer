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
    using std::unique_ptr;
    using std::vector;
    using frontend::TextPosition;

    class ASTNode{
    public:
    	TextPosition textPos;
    protected:
    	ASTNode(const TextPosition& _textPos = TextPosition::NoPos) : textPos(_textPos){}
    };
    class TypeDef  : public ASTNode{};
    class Variable : public ASTNode{};
    	class Constant : public Variable{
    	public:

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
    };



    class Axiom          : public ASTNode{};
    class Procedure      : public ASTNode{};
    class Implementation : public ASTNode{};
    
    class Attributes : public ASTNode{};
    class Identifier : public ASTNode{
    public:
        Identifier(const common::String& _name) 
        : name(_name){}
        String name;
    };
    class Type : public ASTNode{};
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
    class Program : public ASTNode{
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
