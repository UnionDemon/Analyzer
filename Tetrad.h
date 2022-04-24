#include <list>
#include <unordered_map>

#include "clang/AST/Stmt.h"

using namespace clang;

enum OperationType {
	jmp,
	jmpOnFalse,
	label,
	other
};

enum OperandSource {
	stack,
	object
};

class Operand {
private:
	OperandSource source;
	Stmt* astNode;
public:
	Operand(OperandSource src, Stmt* nd);

	Stmt* getAstNode();
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
	int findEndForStmt(ForStmt* st);
	void jmpForStmt(ForStmt* st);
	int firstLabelMarkerFor(ForStmt* st);

public:
	void handleStatement(Stmt* st);
	void print();
	Stmt* findFirst(Stmt* st);
};