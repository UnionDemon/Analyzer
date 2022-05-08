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
	ptrNullCheck,
	nullptrLiteral,
	other
};

enum class CompareType {
	none,
	eq,
	ne
};

class Operand {
private:
	OperandSource source;
	OperandType typeop;
	std::string variable;
	CompareType comparison = CompareType::none;
	Stmt* astNode;
public:
	Operand(OperandSource src, OperandType type, std::string var, Stmt* nd);
	Operand(const Operand& op) {
		*this = op;
	}

	Stmt* getAstNode();
	OperandType getTypeOp();
	std::string getVarName();
	OperandSource getOpSource();

	void setVarName(std::string varName);
	void setTypeOp(OperandType type);
	void setCompareType(CompareType type);

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
	void handleNullptrLiteral(CXXNullPtrLiteralExpr* st);
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
	void handleNullPtrCheck(BinaryOperator* bin_op, Tetrad* tetrad, Operand* result);
public:
	void handleStatement(Stmt* st);
	void print();
	Stmt* findFirst(Stmt* st);
	std::list<Tetrad*> getPseudoCode();
};