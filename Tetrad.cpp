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

CompareType Operand::getCompareType() {
	return comparison;
}

OperandType Operand::getTypeOp()
{
	return typeop;
}

std::string Operand::getVarName() {
	return variable;
}

OperandSource Operand::getOpSource() {
	return source;
}

std::list<Tetrad*> pseudoCodeGenerator::getPseudoCode() {
	return pseudoCode;
}

void Operand::setVarName(std::string varName) {
	this->variable = varName;
}
void Operand::setTypeOp(OperandType type) {
	this->typeop = type;
}
void Operand::setCompareType(CompareType type) {
	this->comparison = type;
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
	
	if (CXXNullPtrLiteralExpr* nullptr_stmt = dyn_cast<CXXNullPtrLiteralExpr>(st))
	{
		handleNullptrLiteral(nullptr_stmt);
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

	if (ReturnStmt* return_stmt = dyn_cast<ReturnStmt>(st))
	{
		handleReturnStmt(return_stmt);
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

void pseudoCodeGenerator::handleReturnStmt(ReturnStmt* st)
{
	int childrens = countChildren(st);

	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::returnStmt;
	tetrad->astNode = st;

	for (int i = 0; i < childrens; i++) {
		Operand* op = operandsStack.back();
		operandsStack.pop_back();
		tetrad->operands.push_front(op);
	}

	pseudoCode.push_back(tetrad);
}

void pseudoCodeGenerator::handleNullPtrCheck(BinaryOperator* bin_op, Tetrad* tetrad, Operand* result)
{
	CompareType compare;
	if (bin_op->getOpcode() == BO_EQ) {
		compare = CompareType::eq;
	}
	else if (bin_op->getOpcode() == BO_NE)
	{
		compare = CompareType::ne;
	} 
	else 
	{
		return;
	}

	if (tetrad->operands.size() != 2)
	{
		return;
	}
	
	auto operandIt = tetrad->operands.begin();
	if ((*operandIt)->getTypeOp() != OperandType::pointer) {
		return;
	}

	std::string ptrName = (*operandIt)->getVarName();

	operandIt++;
	if ((*operandIt)->getTypeOp() != OperandType::nullptrLiteral) {
		return;
	}

	result->setTypeOp(OperandType::ptrNullCheck);
	result->setVarName(ptrName);
	result->setCompareType(compare);
}

void pseudoCodeGenerator::handleNullptrLiteral(CXXNullPtrLiteralExpr* st) {
	Operand* result = new Operand(OperandSource::object, OperandType::nullptrLiteral, "", st);
	operandsStack.push_back(result);
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

	if (BinaryOperator* bin_op = dyn_cast<BinaryOperator>(st))
	{
		handleNullPtrCheck(bin_op, tetrad, result);
	}

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

/*поиск в левом поддереве
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
}*/

Stmt* pseudoCodeGenerator::findFirst(Stmt* st)
{
	if (countChildren(st) == 0)
	{
		return nullptr;
	}

	for (auto it = st->child_begin(); it != st->child_end(); it++)
	{
		Stmt* first = findFirst(*it);
		if (first != nullptr)
			return first;
	}

	return st;
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

	//вложенность
	Tetrad* tetrad = new Tetrad();
	tetrad->labelNumber = labelCounter++;
	tetrad->operation = OperationType::label;
	tetrad->astNode = st;
	Stmt* ifBegin = findFirst(st);
	auto itBegin = pseudoCode.end();
	for (auto it = pseudoCode.begin(); it != pseudoCode.end(); it++)
	{
		if ((*it)->astNode == ifBegin)
		{
			itBegin = it;
		}
	}
	if ((*itBegin)->operation == OperationType::label)
	{
		return;
	}
	pseudoCode.insert(itBegin, tetrad);
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
	//инструкция jmp
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

//конец IfStmt
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
	int labelNumberEndForStmt = findEndCycleStmt(st);
	Stmt* condStmt = (Stmt*)st->getCond();
	makeJmpOnFalseTetrad(condStmt, labelNumberEndForStmt);

	Stmt* bodyStmt = st->getBody();
	int labelBody = findOrMakeLabel(bodyStmt);

	Stmt* conditionBegin = findFirst(condStmt);
	int labelCond = findOrMakeLabel(conditionBegin);

	Stmt* incStmt = (Stmt*)st->getInc();
	Stmt* incBegin = findFirst(incStmt);
	int labelInc = findOrMakeLabel(incBegin);

	insertForJumpBeforeLabel(labelBody, labelInc);
	insertForJumpBeforeLabel(labelInc, labelNumberEndForStmt);
	insertForJumpBeforeLabel(labelCond, labelBody);

}

int pseudoCodeGenerator::findOrMakeLabel(Stmt* st)
{
	auto bodyBegin = pseudoCode.end();
	for (auto it = pseudoCode.begin(); it != pseudoCode.end(); it++)
	{
		if ((*it)->astNode == st)
		{
			bodyBegin = it;
			break;
		}
	}

	if (bodyBegin == pseudoCode.end())
	{
		return -1;
	}

	if ((*bodyBegin)->operation == OperationType::label)
	{
		return (*bodyBegin)->labelNumber;
	}

	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::label;
	tetrad->labelNumber = labelCounter++;
	pseudoCode.insert(bodyBegin, tetrad);
	return tetrad->labelNumber;
}

void pseudoCodeGenerator::insertForJumpBeforeLabel(int toLabel, int beforeLabel)
{
	auto beforeLabelIterator = pseudoCode.end();
	for (auto it = pseudoCode.begin(); it != pseudoCode.end(); it++)
	{
		if (((*it)->labelNumber == beforeLabel) && (*it)->operation == OperationType::label)
		{
			beforeLabelIterator = it;
			break;
		}
	}
	if (beforeLabelIterator == pseudoCode.end())
	{
		return;
	}
	Tetrad* tetrad = new Tetrad();
	tetrad->operation = OperationType::jmp;
	tetrad->labelNumber = toLabel;
	pseudoCode.insert(beforeLabelIterator, tetrad);
}

//конец ForStmt
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
	//инструкция jmp для цикла for
	auto endCond = pseudoCode.end();
	endCond--;
	//вставляем label перед cond
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

//обработка WhileStmt
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

	//выдернуть операнд из стека
	for (auto it = operandsStack.begin(); it != operandsStack.end(); it++)
	{
		if ((*it)->getAstNode() == st)
		{
			//Operand* conditionVal = new Operand(OperandSource::stack, OperandType::other, "", (*it)->getAstNode());
			Operand* conditionVal = new Operand(**it);
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
	//обработка разыменований
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
	else if (op->getTypeOp() == OperandType::nullptrLiteral) 
	{
		resultOperandType = OperandType::nullptrLiteral;
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
	if (operation == OperationType::returnStmt) {
		std::cout << "return " << " ";
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
	if (typeop == OperandType::nullptrLiteral)
	{
		std::cout << "nullptrLiteral ";
	}
	if (typeop == OperandType::ptrNullCheck)
	{
		std::cout << "ptrNullCheck ";
	}
	if (comparison == CompareType::eq)
	{
		std::cout << "equal ";
	}
	if (comparison == CompareType::ne)
	{
		std::cout << "not equal ";
	}
	std::cout << variable << " ";
	std::cout << "]";
}