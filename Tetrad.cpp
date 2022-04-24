#include "Tetrad.h"
#include "StmtHash.h"

#include <iostream>

extern std::string nodeName(Stmt* node);

Operand::Operand(OperandSource src, Stmt* nd)
{
	source = src;
	astNode = nd;
}

Stmt* Operand::getAstNode()
{
	return astNode;
}

void pseudoCodeGenerator::handleStatement(Stmt* st)
{
	nodeName(st);

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

	handleDefaultStatement(st);
	
}

void pseudoCodeGenerator::handleDefaultStatement(Stmt* st) {
	int childrens = countChildren(st);

	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::other;
	tetrad->astNode = st;

	//DEBUG
	//std::string name = st->getStmtClassName();

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

	//DEBUG
	//std::string nodeName = st->getStmtClassName();
	
	if (childrens == 0)
	{
		Operand* op = new Operand(OperandSource::object, st);
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

	//DEBUG
	//std::string tetradName = tetrad->astNode->getStmtClassName();

	pseudoCode.push_back(tetrad);

	Operand* result = new Operand(OperandSource::stack, st);
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

	//std::string firstName = begStmt->getStmtClassName();
	//std::cout << "[" << firstName << "] : " << begStmt << std::endl;

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
	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::jmpOnFalse;
	tetrad->labelNumber = labelNumberElse;
	
	//выдернуть операнд из стека
	Stmt* condStmt = (Stmt*)st->getCond();
	for (auto it = operandsStack.begin(); it != operandsStack.end(); it++)
	{
		if ((*it)->getAstNode() == condStmt)
		{
			Operand* conditionVal = new Operand(OperandSource::stack, (*it)->getAstNode());
			operandsStack.erase(it);
			tetrad->operands.push_back(conditionVal);
			break;
		}
	}

	auto ifCond = pseudoCode.end();
	for (auto it = pseudoCode.begin(); it != pseudoCode.end(); it++)
	{
		if ((*it)->astNode == condStmt) {
			ifCond = it;
			break;
		}
	}
	if (ifCond == pseudoCode.end()) {
		return;
	}
	ifCond++;
	pseudoCode.insert(ifCond, tetrad);

	jmpIfStmt(st);
}

int pseudoCodeGenerator::findElse(IfStmt* st)
{
	Stmt* elseStmt = st->getElse();

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
	//инструкция jmp
	int labelNumberEnd = findEndIfStmt(st);
	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::jmp;
	tetrad->labelNumber = labelNumberEnd;
	Stmt* elseStmt = st->getElse();

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
	ifElse--;
	pseudoCode.insert(ifElse, tetrad);
}

//нахожу конец IfStmt
int pseudoCodeGenerator::findEndIfStmt(IfStmt* st)
{
	auto endTetrad = pseudoCode.end();

	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::label;
	tetrad->labelNumber = labelCounter++;
	pseudoCode.insert(endTetrad, tetrad);
	return tetrad->labelNumber;
}

//обработка ForStmt
void pseudoCodeGenerator::handleForStmt(ForStmt* st)
{
	int labelNumberEndForStmt = findEndForStmt(st);
	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::jmpOnFalse;
	tetrad->labelNumber = labelNumberEndForStmt;

	//выдернуть операнд из стека
	Stmt* condStmt = (Stmt*)st->getCond();
	for (auto it = operandsStack.begin(); it != operandsStack.end(); it++)
	{
		if ((*it)->getAstNode() == condStmt)
		{
			Operand* conditionVal = new Operand(OperandSource::stack, (*it)->getAstNode());
			operandsStack.erase(it);
			tetrad->operands.push_back(conditionVal);
			break;
		}
	}

	//вставляем тетраду с jmpOnFalse
	auto forCond = pseudoCode.end();
	for (auto it = pseudoCode.begin(); it != pseudoCode.end(); it++)
	{
		if ((*it)->astNode == condStmt) {
			forCond = it;
			break;
		}
	}
	if (forCond == pseudoCode.end()) {
		return;
	}
	forCond++;
	pseudoCode.insert(forCond, tetrad);

	jmpForStmt(st);
}

//находим конец ForStmt
int pseudoCodeGenerator::findEndForStmt(ForStmt* st)
{
	auto endTetrad = pseudoCode.end();

	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::label;
	tetrad->labelNumber = labelCounter++;
	pseudoCode.insert(endTetrad, tetrad);
	return tetrad->labelNumber;
}

void pseudoCodeGenerator::jmpForStmt(ForStmt* st)
{
	//инструкция jmp для цикла for
	auto endCond = pseudoCode.end();
	endCond--;
	//вставляем label перед cond
	int labelNumberJmp = firstLabelMarkerFor(st);
	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::jmp;
	tetrad->labelNumber = labelNumberJmp;

	pseudoCode.insert(endCond, tetrad);
}

int pseudoCodeGenerator::firstLabelMarkerFor(ForStmt* st)
{
	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::label;
	tetrad->labelNumber = labelCounter++;
	
	Stmt* condStmt = (Stmt*)st->getCond();
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
	condTetrad--;
	pseudoCode.insert(condTetrad, tetrad);
	return tetrad->labelNumber;
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
	std::cout << "]";
}