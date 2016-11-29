#ifndef FRONTEND_BOOGIE_AST_H
#define FRONTEND_BOOGIE_AST_H

#include <set>
#include <vector>
#include <memory>
#include "common/data_types.h"

namespace frontend{
namespace boogie{
namespace AST{
    using common::String;
    using String = common::String;
    using unique_ptr = std::unique_ptr;
    using vector = std::vector;

    class ASTNode{};
    class TypeDef     : public ASTNode{};
    class VariableDef : public ASTNode{};
    class FunctionDef : public ASTNode{};

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
    class TypedIdentifier : public Identifier{ public: Type type; };
    class Type : public ASTNode{};

    class ProcedureSignature : public ASTNode{};
    class ProcedureSpec : public ASTNode{};
    class ProcedureBody : public ASTNode{};
    class TypeArguments : public ASTNode{};

    class Scope : public ASTNode{
    public:
    	void addTypeDef    (unique_ptr<TypeDef> td);
    	void addVariableDef(unique_ptr<VariableDef> vd);
    	void addFunctionDef(unique_ptr<FunctionDef> fd);
    private:
    	vector<VariableDef> variables;
    	vector<TypeDef>     types;
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
