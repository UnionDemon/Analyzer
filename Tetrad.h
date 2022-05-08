#include <list>
#include <unordered_map>
#include "clang/AST/AST.h"
#include "clang/AST/Stmt.h"
#include<string>

using namespace clang;

enum class OperationType {
	jmp,
	jmpOnFalse,
	label,
	dereference,
	other
};

enum class OperandSource {
	stack,
	object
};

enum class OperandType {
	pointer,
	other
};

class Operand {
private:
	OperandSource source;
	OperandType typeop;
	std::string variable;
	Stmt* astNode;
public:
	Operand(OperandSource src, OperandType type, std::string var, Stmt* nd);

	Stmt* getAstNode();
	OperandType getTypeOp();
	std::string getVarName();
	void print();
};

struct Tetrad
{
	OperationType operation;
	std::list<Operand*> operands;
	Stmt* astNode;
	int labelNumber;

	void print();
};

class pseudoCodeGenerator {
private:
	std::list<Operand*> operandsStack;
	std::list<Tetrad*> pseudoCode;

	void handleValueStmt(ValueStmt* st);
	int countChildren(Stmt* st);
	void handleCompoundStmt(CompoundStmt* st);
	void handleIfStmt(IfStmt* st);
	void handleDefaultStatement(Stmt* st);
	int findElse(IfStmt* st);
	int labelCounter = 0;
	void jmpIfStmt(IfStmt* st);
	int findEndIfStmt(IfStmt* st);
	void handleForStmt(ForStmt* st);
	int findOrMakeLabel(Stmt* st);
	void insertForJumpBeforeLabel(int toLabel, int beforeLabel);
	int findEndCycleStmt(Stmt* st);
	void makeJmpTetrad(Stmt* st, Stmt* cond);
	int firstLabelMarker(Stmt* st, Stmt* cond);
	void handleWhileStmt(WhileStmt* st);
	void makeJmpOnFalseTetrad(Stmt* st, int labelNumber);
	void handleUnaryOperator(UnaryOperator* unary_op);
	void handleDeclRefExpr(DeclRefExpr* expr);
	void handleImplicitCastExpr(ImplicitCastExpr* expr);
public:
	void handleStatement(Stmt* st);
	void print();
	Stmt* findFirst(Stmt* st);
};