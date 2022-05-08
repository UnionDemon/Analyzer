#include "Tetrad.h"
#include "clang/AST/AST.h"
#include <string>
#include <iostream>

extern std::string nodeName(Stmt* node);

Operand::Operand(OperandSource src, OperandType type, std::string var, Stmt* nd)
{
	source = src;
	typeop = type;
	variable = var;
	astNode = nd;
}

Stmt* Operand::getAstNode()
{
	return astNode;
}

OperandType Operand::getTypeOp()
{
	return typeop;
}

std::string Operand::getVarName() {
	return variable;
}

void pseudoCodeGenerator::handleStatement(Stmt* st)
{
	nodeName(st);

	if (UnaryOperator* unary_op = dyn_cast<UnaryOperator>(st))
	{
		handleUnaryOperator(unary_op);
		return;
	}
	
	if (DeclRefExpr* decl_ref_expr = dyn_cast<DeclRefExpr>(st))
	{
		handleDeclRefExpr(decl_ref_expr);
		return;
	}
	
	if (ImplicitCastExpr* implicit_cast_expr = dyn_cast<ImplicitCastExpr>(st))
	{
		handleImplicitCastExpr(implicit_cast_expr);
		return;
	}
	
	if (ValueStmt* val_stmt = dyn_cast<ValueStmt>(st))
	{
		handleValueStmt(val_stmt);
		return;
	}

	if (CompoundStmt* cmpd_stmt = dyn_cast<CompoundStmt>(st))
	{
		handleCompoundStmt(cmpd_stmt);
		return;
	}
	
	if (IfStmt* if_stmt = dyn_cast<IfStmt>(st))
	{
		handleIfStmt(if_stmt);
		return;
	}

	if (ForStmt* for_stmt = dyn_cast<ForStmt>(st))
	{
		handleForStmt(for_stmt);
		return;
	}

	if (WhileStmt* while_stmt = dyn_cast<WhileStmt>(st))
	{
		handleWhileStmt(while_stmt);
		return;
	}

	handleDefaultStatement(st);
}

void pseudoCodeGenerator::handleDefaultStatement(Stmt* st) {
	int childrens = countChildren(st);

	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::other;
	tetrad->astNode = st;

	for (int i = 0; i < childrens; i++) {
		Operand* op = operandsStack.back();
		operandsStack.pop_back();
		tetrad->operands.push_front(op); 
	}

	pseudoCode.push_back(tetrad);
}

void pseudoCodeGenerator::handleValueStmt(ValueStmt* st)
{
	int childrens = countChildren(st);
	
	if (childrens == 0)
	{
		Operand* op = new Operand(OperandSource::object, OperandType::other, "", st);
		operandsStack.push_back(op);
		return;
	}

	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::other;
	tetrad->astNode = st;

	for (int i = 0; i != childrens; i++)
	{
		Operand* op = operandsStack.back();
		operandsStack.pop_back();
		tetrad->operands.push_front(op);
	}

	pseudoCode.push_back(tetrad);

	Operand* result = new Operand(OperandSource::stack, OperandType::other, "", st);
	operandsStack.push_back(result);
}

int pseudoCodeGenerator::countChildren(Stmt* st)
{
	int childrenCount = 0;
	for (auto i = st->child_begin(); i != st->child_end(); i++)
	{
		childrenCount++;
	}
	return childrenCount;
}

Stmt* pseudoCodeGenerator::findFirst(Stmt* st)
{
	if (countChildren(st) == 0)
	{
		return nullptr;
	}

	Stmt* firstChild = *(st->child_begin());
	if (countChildren(firstChild) == 0)
	{
		return st;
	}
	
	return findFirst(firstChild);
}

void pseudoCodeGenerator::handleCompoundStmt(CompoundStmt* st)
{
	Stmt* begStmt = findFirst(st);

	//DEBUG
	//std::string begName = nodeName(begStmt);
	//std::cout << "\n\n\n=================== ELSE NODE : " << begName << " ============\n\n\n";
	//=====DEBUG

	Tetrad* tetrad = new Tetrad();
	tetrad->labelNumber = labelCounter++;
	tetrad->operation = OperationType::label;
	tetrad->astNode = st;
	for (auto iter = pseudoCode.begin(); iter != pseudoCode.end(); iter++)
	{
		if ((*iter)->astNode == begStmt)
		{
			pseudoCode.insert(iter, tetrad);
			break;
		}
	}

}

void pseudoCodeGenerator::handleIfStmt(IfStmt* st)
{
	int labelNumberElse = findElse(st);
	Stmt* condStmt = (Stmt*)st->getCond();
	makeJmpOnFalseTetrad(condStmt, labelNumberElse);

	jmpIfStmt(st);
}

int pseudoCodeGenerator::findElse(IfStmt* st)
{
	Stmt* elseStmt = st->getElse();
	if (elseStmt == nullptr)
	{
		return findEndIfStmt(st);
	}
	
	//DEBUG
	//std::string elseName = nodeName(elseStmt);
	//std::cout << "\n\n\n=================== ELSE NODE : " << elseName << " ============\n\n\n";
	//=====DEBUG

	auto elseTetrad = pseudoCode.end();

	for (auto it = pseudoCode.begin(); it != pseudoCode.end(); it++)
	{
		if ((*it)->astNode == elseStmt)
		{
			elseTetrad = it;
			break;
		}
	}

	if (elseTetrad == pseudoCode.end()) {
		return -1;
	}

	if ((*elseTetrad)->operation == OperationType::label)
	{
		return (*elseTetrad)->labelNumber;
	}

	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::label;
	tetrad->labelNumber = labelCounter++;
	pseudoCode.insert(elseTetrad, tetrad);
	return tetrad->labelNumber;
}

void pseudoCodeGenerator::jmpIfStmt(IfStmt* st)
{
	//���������� jmp
	Stmt* elseStmt = st->getElse();
	if (elseStmt == nullptr)
	{
		return;
	}
	int labelNumberEnd = findEndIfStmt(st);
	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::jmp;
	tetrad->labelNumber = labelNumberEnd;
	

	auto ifElse = pseudoCode.end();
	for (auto it = pseudoCode.begin(); it != pseudoCode.end(); it++)
	{
		if ((*it)->astNode == elseStmt)
		{
			ifElse = it;
			break;
		}
	}
	if (ifElse == pseudoCode.end())
	{
		return;
	}
	pseudoCode.insert(ifElse, tetrad);
}

//����� IfStmt
int pseudoCodeGenerator::findEndIfStmt(IfStmt* st)
{
	auto endTetrad = pseudoCode.end();

	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::label;
	tetrad->labelNumber = labelCounter++;
	pseudoCode.insert(endTetrad, tetrad);
	return tetrad->labelNumber;
}

//��������� ForStmt
void pseudoCodeGenerator::handleForStmt(ForStmt* st)
{
	int labelNumberEndForStmt = findEndCycleStmt(st);
	Stmt* condStmt = (Stmt*)st->getCond();
	makeJmpOnFalseTetrad(condStmt, labelNumberEndForStmt);

	makeJmpTetrad(st, condStmt);
}

//����� ForStmt
int pseudoCodeGenerator::findEndCycleStmt(Stmt* st)
{
	auto endTetrad = pseudoCode.end();

	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::label;
	tetrad->labelNumber = labelCounter++;
	pseudoCode.insert(endTetrad, tetrad);
	return tetrad->labelNumber;
}

void pseudoCodeGenerator::makeJmpTetrad(Stmt* st, Stmt* cond)
{
	//���������� jmp ��� ����� for
	auto endCond = pseudoCode.end();
	endCond--;
	//��������� label ����� cond
	int labelNumberJmp = firstLabelMarker(st, cond);
	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::jmp;
	tetrad->labelNumber = labelNumberJmp;

	pseudoCode.insert(endCond, tetrad);
}

int pseudoCodeGenerator::firstLabelMarker(Stmt* st, Stmt* cond)
{
	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::label;
	tetrad->labelNumber = labelCounter++;
	
	Stmt* condStmt = cond;
	auto condTetrad = pseudoCode.end();

	for (auto it = pseudoCode.begin(); it != pseudoCode.end(); it++)
	{
		if ((*it)->astNode == condStmt)
		{
			condTetrad = it;
			break;
		}
	}
	if (condTetrad == pseudoCode.end())
	{
		return tetrad->labelNumber;
	}

	if (condTetrad == pseudoCode.begin())
	{
		pseudoCode.insert(condTetrad, tetrad);
		return tetrad->labelNumber;
	} else {
		condTetrad--;
		pseudoCode.insert(condTetrad, tetrad);
		return tetrad->labelNumber;
	}
}

//��������� WhileStmt
void pseudoCodeGenerator::handleWhileStmt(WhileStmt* st)
{
	int labelNumberEndForStmt = findEndCycleStmt(st);
	Stmt* condStmt = (Stmt*)st->getCond();
	makeJmpOnFalseTetrad(condStmt, labelNumberEndForStmt);

	makeJmpTetrad(st, condStmt);
}

void pseudoCodeGenerator::makeJmpOnFalseTetrad(Stmt* st, int labelNumber)
{
	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::jmpOnFalse;
	tetrad->labelNumber = labelNumber;

	//��������� ������� �� �����
	for (auto it = operandsStack.begin(); it != operandsStack.end(); it++)
	{
		if ((*it)->getAstNode() == st)
		{
			Operand* conditionVal = new Operand(OperandSource::stack, OperandType::other, "", (*it)->getAstNode());
			operandsStack.erase(it);
			tetrad->operands.push_back(conditionVal);
			break;
		}
	}
	auto cond = pseudoCode.end();
	for (auto it = pseudoCode.begin(); it != pseudoCode.end(); it++)
	{
		if ((*it)->astNode == st) {
			cond = it;
			break;
		}
	}
	if (cond == pseudoCode.end()) {
		return;
	}
	cond++;
	pseudoCode.insert(cond, tetrad);
}

void pseudoCodeGenerator::handleUnaryOperator(UnaryOperator* op)
{
	int childrens = countChildren(op);
	Tetrad* tetrad = new Tetrad();
	//��������� �������������
	if (op->getOpcode() == UO_Deref) {
		tetrad->operation = OperationType::dereference;
	}
	else
	{
		tetrad->operation = OperationType::other;
	}
	tetrad->astNode = op;

	for (int i = 0; i < childrens; i++) {
		Operand* op = operandsStack.back();
		operandsStack.pop_back();
		tetrad->operands.push_front(op);
	}

	pseudoCode.push_back(tetrad);

	Operand* result = new Operand(OperandSource::stack, OperandType::other, "", op);
	operandsStack.push_back(result);
}

void pseudoCodeGenerator::handleDeclRefExpr(DeclRefExpr* expr)
{
	auto* varType = expr->getType().getTypePtrOrNull();
	std::string variable = expr->getNameInfo().getAsString();
	if (varType != nullptr && varType->isPointerType() == true)
	{
		Operand* result = new Operand(OperandSource::stack, OperandType::pointer, variable, expr);
		operandsStack.push_back(result);
	} else {
		Operand* result = new Operand(OperandSource::stack, OperandType::other, variable, expr);
		operandsStack.push_back(result);
	}
}

void pseudoCodeGenerator::handleImplicitCastExpr(ImplicitCastExpr* expr)
{
	Operand* op = operandsStack.back();
	operandsStack.pop_back();

	std::string resultVarName = "";
	OperandType resultOperandType = OperandType::other;

	if (op->getTypeOp() == OperandType::pointer)
	{
		resultOperandType = OperandType::pointer;
		resultVarName = op->getVarName();
	}

	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::other;
	tetrad->astNode = expr;
	tetrad->operands.push_back(op);

	pseudoCode.push_back(tetrad);

	Operand* result = new Operand(OperandSource::stack, resultOperandType, resultVarName, expr);
	operandsStack.push_back(result);
}

void pseudoCodeGenerator::print()
{
	for (auto it = pseudoCode.begin(); it != pseudoCode.end(); it++) {
		(*it)->print();
	}
}

void Tetrad::print()
{
	std::cout << nodeName(astNode) << " ";
	
	if (operation == OperationType::jmp) {
		std::cout << "jmp " << labelNumber << " ";
	}
	if (operation == OperationType::jmpOnFalse) {
		std::cout << "jmpOnFalse " << labelNumber << " ";
	}
	if (operation == OperationType::label) {
		std::cout << "label "<< labelNumber << " ";
	}
	if (operation == OperationType::dereference) {
		std::cout << "dereference " << " ";
	}
	if (operation == OperationType::other) {
		std::cout << "other ";
	}
	for (auto it = operands.begin(); it != operands.end(); it++) {
		(*it)->print();
	}
	std::cout << std::endl;
}

void Operand::print()
{
	std::cout << "[";
	std::cout << nodeName(astNode) << " ";
	if (source == OperandSource::stack)
	{
		std::cout << "stack ";
	}
	if (source == OperandSource::object)
	{
		std::cout << "object ";
	}
	if (typeop == OperandType::other)
	{
		std::cout << "other ";
	}
	if (typeop == OperandType::pointer)
	{
		std::cout << "pointer ";
	}
	std::cout << variable << " ";
	std::cout << "]";
}