#include "Interpreter.h"
#include "clang/AST/ASTContext.h"
#include <iostream>

ASTContext* g_ast_context = nullptr;

Interpreter::Interpreter(controlFlowGraph* cfg)
{
	graph = cfg;
}

const std::list<error*>& Interpreter::getErrors() {
	return errors;
}

void Interpreter::init()
{
	for (auto it = graph->getBlocks().begin(); it != graph->getBlocks().end(); it++)
	{
		visitedBlock[(*it)->getId()] = false;
	}
}

void Interpreter::walkOnGraph(BasicBlock* bb)
{
	auto currentPointersTable = pointers;
	std::cout << "BASIC BLOCK # " << bb->getId() << std::endl;
	visitedBlock[bb->getId()] = true;
	interpretTetrads(bb->getTetrads());
	for (auto it = (graph->getEdges())[bb->getId()].begin(); it != (graph->getEdges())[bb->getId()].end(); it++)
	{
		if (visitedBlock[(*it)->getDestination()->getId()] == true)
		{
			continue;
		}

		if ((*it)->isNullptrCheck() == true)
		{
			std::string variable = (*it)->getVarName();
			if ((*it)->getCompareType() == CompareType::eq)
			{
				pointers[variable] = pointerValue::null;
			}
			else if ((*it)->getCompareType() == CompareType::ne)
			{
				pointers[variable] = pointerValue::notNull;
			}
		}

		walkOnGraph((*it)->getDestination());
		pointers = currentPointersTable;
	}
	visitedBlock[bb->getId()] = false;
}

void Interpreter::interpretTetrads(const std::list<Tetrad*>& tetrads)
{
	for (auto it = tetrads.begin(); it != tetrads.end(); it++)
	{
		handleTetrad(*it);
	}
}

void Interpreter::run()
{
	walkOnGraph(*(graph->getBlocks().begin()));
}

void Interpreter::handleTetrad(Tetrad* tetrad)
{
	addPointersToTable(tetrad);
	if (tetrad->operation == OperationType::dereference)
	{
		handleDereference(tetrad);
	}
	if (tetrad->operation == OperationType::assign)
	{
		handleUninitialized(tetrad);//диагностика неинициализированных указателей
	}
}

void Interpreter::addPointersToTable(Tetrad* tetrad)
{
	for (auto it = tetrad->operands.begin(); it != tetrad->operands.end(); it++)
	{
		if ((*it)->getTypeOp() != OperandType::pointer)
		{
			continue;
		}
		std::string variable = (*it)->getVarName();
		if (pointers.find(variable) == pointers.end())
		{
			pointers[variable] = pointerValue::any;
		}
	}
}

void Interpreter::handleDereference(Tetrad* tetrad)
{
	std::string variable = (*tetrad->operands.begin())->getVarName();
	pointerValue val = pointers[variable];
	if ((val == pointerValue::null) || (val == pointerValue::any))
	{
		error* e = new error();
		e->type = errorType::nullPtrDereference;
		//e->location = (static_cast<Stmt*>(tetrad->astNode))->getBeginLoc().printToString(g_ast_context->getSourceManager());
		e->location = "";
		e->message = "Possible null pointer dereference: " + variable;
		errors.push_back(e);
	}
}

void Interpreter::handleUninitialized(Tetrad* tetrad)
{
	auto firstIt = tetrad->operands.begin();
	std::string variable = (*firstIt)->getVarName();
	if ((*firstIt)->getTypeOp() != OperandType::pointer)
	{
		return;
	}
	if (pointerInits.find(variable) == pointerInits.end())
	{
		pointerInits[variable] = pointerInit::uninitialized;
	}
	
	auto secondIt = firstIt++;

	if (((*firstIt)->getTypeOp() == OperandType::pointer) && (((*secondIt)->getTypeOp() == OperandType::address) || ((*secondIt)->getTypeOp() == OperandType::nullptrLiteral)))
	{
		pointerInits[variable] = pointerInit::initialized;
	}

	if (pointerInits[variable] == pointerInit::uninitialized)
	{
		error* e = new error();
		e->type = errorType::unitializedPointer;
		//e->location = (static_cast<Stmt*>(tetrad->astNode))->getBeginLoc().printToString(g_ast_context->getSourceManager());
		e->location = "";
		e->message = "Uninitialized pointer: " + variable;
		errors.push_back(e);
	}
}