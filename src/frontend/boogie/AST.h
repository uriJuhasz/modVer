#ifndef FRONTEND_BOOGIE_AST_H
#define FRONTEND_BOOGIE_AST_H

#include <set>
#include <vector>


namespace frontend{
namespace boogie{
namespace AST{
    class ASTNode{};
    class Program : public ASTNode{
        std::vector<TypeDeclaration>      types;
        std::vector<ConstDeclaration>     constants;
        std::vector<VarDeclaration>       variables;
        std::vector<FunctionDeclaration>  functionss;
        std::vector<ProcedureDeclaration> procedures;

        std::vector<Axiom>                axioms;
        std::vector<Implementation> implementations;
    };
    class TypeDeclaration : public ASTNode{};
    class ConstDeclaration : public ASTNode{};
    class VarDeclaration : public ASTNode{};
    class FunctionDeclaration : public ASTNode{};
    class Axiom : public ASTNode{};
    class ProcedureDeclaration : public ASTNode{};
    class Implementation : public ASTNode{};

}//namespace AST
}//namespace frontend
}//namespace boogie
#endif