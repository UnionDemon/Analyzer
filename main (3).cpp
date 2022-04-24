#include "clang/AST/AST.h"
#include "clang/AST/ASTConsumer.h"
#include "clang/AST/ASTContext.h"
#include "clang/AST/RecursiveASTVisitor.h"
#include "clang/Driver/Options.h"
#include "clang/Frontend/ASTConsumers.h"
#include "clang/Frontend/CompilerInstance.h"
#include "clang/Frontend/FrontendActions.h"
#include "clang/Rewrite/Core/Rewriter.h"
#include "clang/Tooling/CommonOptionsParser.h"
#include "clang/Tooling/Tooling.h"

#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;
using namespace clang;
using namespace clang::driver;
using namespace clang::tooling;
using namespace llvm;

class ExampleVisitor : public RecursiveASTVisitor<ExampleVisitor> {
private:
    ASTContext* astContext; // used for getting additional AST info

public:
    explicit ExampleVisitor(CompilerInstance* CI)
        : astContext(&(CI->getASTContext())) // initialize private members
    {}

    virtual bool VisitStmt(Stmt* st) {
        std::cout << st->getBeginLoc().printToString(astContext->getSourceManager())
            << ":\t\t" << st->getStmtClassName() << std::endl;

        if (DeclRefExpr* decl_ref = dyn_cast<DeclRefExpr>(st)) {
            std::cout << "FOUND IDENTIFIER: " << decl_ref->getNameInfo().getAsString() << std::endl;
        }

        if (DeclStmt* decl_stmt = dyn_cast<DeclStmt>(st)) {
            for (auto it = decl_stmt->decl_begin(); it != decl_stmt->decl_end(); it++) {
                if (ValueDecl* val_decl = dyn_cast<ValueDecl>(*it)) {
                    const clang::Type* type = val_decl->getType().getTypePtr();
                    if (!type) {
                        continue;
                    }
                    if (type->isPointerType()) {
                        std::cout << "FOUND POINTER DECLARATION: " << dyn_cast<NamedDecl>(val_decl)->getNameAsString() << std::endl;
                    }
                }
            }
        }

        /*

        if (IfStmt* if_stmt = dyn_cast<IfStmt>(st)) {
            std::cout << "IF_CONDITION: ";
            printCondThenElse(if_stmt->getCond());
            std::cout << "IF_THEN: ";
            printCondThenElse(if_stmt->getThen());
            std::cout << "IF_ELSE: ";
            printCondThenElse(if_stmt->getElse());
        }
        */

        return true;
    }

    bool shouldTraversePostOrder() const {
        return true;
    }

private:
    /*void printCondition(Expr* expr) {
        if (!expr) {
            return;
        }

        SourceLocation exprBeginLoc = expr->getBeginLoc();
        SourceLocation exprEndLoc = expr->getEndLoc();

        std::string file_path = (std::string)astContext->getSourceManager().getFilename(exprBeginLoc);

        std::ifstream srcFile(file_path);
        if (!srcFile.is_open()) {
            std::cout << "FAIL TO OPEN";
        }

        int exprBeg = astContext->getSourceManager().getFileOffset(exprBeginLoc);
        int exprEnd = astContext->getSourceManager().getFileOffset(exprEndLoc);

        srcFile.seekg(exprBeg);
        int len = exprEnd - exprBeg + 2;

        std::vector<char> exprBuffer;
        exprBuffer.resize(len);

        srcFile.read(&exprBuffer[0], len);

        std::string exprText(exprBuffer.begin(), exprBuffer.end());

        std::cout << "[" << exprText << "]" << std::endl;
    }*/
    /*
    void printCondThenElse(Stmt* stmt)
    {
        if (!stmt)
        {
            return;
        }

        SourceLocation stmtBeginLoc = stmt->getBeginLoc();
        SourceLocation stmtEndLoc = stmt->getEndLoc();

        std::string file_path = (std::string)astContext->getSourceManager().getFilename(stmtBeginLoc);

        std::ifstream srcFile(file_path);
        if (!srcFile.is_open()) {
            std::cout << "FAIL TO OPEN";
        }

        int stmtBeg = astContext->getSourceManager().getFileOffset(stmtBeginLoc);
        int stmtEnd = astContext->getSourceManager().getFileOffset(stmtEndLoc);

        srcFile.seekg(stmtBeg);
        int len = stmtEnd - stmtBeg;

        std::vector<char> stmtBuffer;
        stmtBuffer.resize(len);

        srcFile.read(&stmtBuffer[0], len);

        std::string stmtText(stmtBuffer.begin(), stmtBuffer.end());

        std::cout << "[" << stmtText << "]" << std::endl;
    }
    */

};

class ExampleASTConsumer : public ASTConsumer {
private:
    ExampleVisitor* visitor; // doesn't have to be private

public:
    // override the constructor in order to pass CI
    explicit ExampleASTConsumer(CompilerInstance* CI)
        : visitor(new ExampleVisitor(CI)) // initialize the visitor
    {}

    // override this to call our ExampleVisitor on the entire source file
    virtual void HandleTranslationUnit(ASTContext& Context) {
        /* we can use ASTContext to get the TranslationUnitDecl, which is
             a single Decl that collectively represents the entire source file */
        visitor->TraverseDecl(Context.getTranslationUnitDecl());
    }
};

class ExampleFrontendAction : public ASTFrontendAction {
public:
    virtual std::unique_ptr<ASTConsumer> CreateASTConsumer(CompilerInstance& CI,
        StringRef file) {
        return std::make_unique<ExampleASTConsumer>(
            &CI); // pass CI pointer to ASTConsumer
    }
};

static cl::OptionCategory MyToolCategory("My tool options");

int main(int argc, const char** argv) {
    // parse the command-line args passed to your code
    llvm::Expected<CommonOptionsParser> op =
        CommonOptionsParser::create(argc, argv, MyToolCategory);
    // create a new Clang Tool instance (a LibTooling environment)
    ClangTool Tool(op->getCompilations(), op->getSourcePathList());

    // run the Clang Tool, creating a new FrontendAction (explained below)
    auto actionFactory = newFrontendActionFactory<ExampleFrontendAction>();
    int result = Tool.run(actionFactory.get());

    return result;
}