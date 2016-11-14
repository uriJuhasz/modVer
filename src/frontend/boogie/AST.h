#ifndef FRONTEND_BOOGIE_AST_H
#define FRONTEND_BOOGIE_AST_H

#include <set>
#include <vector>
#include "common/data_types.h"


namespace frontend{
namespace boogie{
namespace AST{
    using common::String;
    using String = common::String;
    class ASTNode{};

    class Type : public ASTNode{};
    class AtomicType : public Type{};
    class IntegerType : public AtomicType{};
    class BooleanType : public AtomicType{};
    class BitVectorType : public AtomicType{};

    class ConstructorInstanceType : public Type{};
    class MapType : public Type{};

    class TypeDeclaration : public ASTNode{};
    class TypeConstruction : public TypeDeclaration{};
    class TypeSynonym      : public TypeDeclaration{};

    class ConstDeclaration : public ASTNode{};
    class VarDeclaration : public ASTNode{};
    class FunctionDeclaration : public ASTNode{};
    class Axiom : public ASTNode{};
    class ProcedureDeclaration : public ASTNode{};
    class Implementation : public ASTNode{};
    
    class Attributes : public ASTNode{};
    class Identifier : public ASTNode{
    public:
        Identifier(const common::String& _name) 
        : name(_name){}
        String name;
    };
    class ProcedureSignature : public ASTNode{};
    class ProcedureSpec : public ASTNode{};
    class ProcedureBody : public ASTNode{};
    class TypeArguments : public ASTNode{};
    class Program : public ASTNode{
        std::vector<TypeDeclaration>      types;
        std::vector<ConstDeclaration>     constants;
        std::vector<VarDeclaration>       variables;
        std::vector<FunctionDeclaration>  functionss;
        std::vector<ProcedureDeclaration> procedures;

        std::vector<Axiom>                axioms;
        std::vector<Implementation> implementations;
    };

}//namespace AST
}//namespace frontend
}//namespace boogie
#endif
