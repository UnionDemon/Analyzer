/*
#include "Tetrad.h"
#include "controlFlowGraph.h"
#include <iostream>

BasicBlock::BasicBlock(const std::list<Tetrad*> &instruct)
{
	instructions = instruct;
}

controlFlowGraph::controlFlowGraph(const std::list<Tetrad*> &pseudoCode)
{
	initCFG(pseudoCode);
}

void controlFlowGraph::initCFG(const std::list<Tetrad*>& pseudoCode) {
	std::list<Tetrad*> current;
	for (auto it = pseudoCode.begin(); it != pseudoCode.end(); it++)
	{
		if ((*it)->operation == OperationType::label)
		{
			addBasicBlock(current);
			current.clear();
			current.push_back((*it));
			continue;
		}

		if (((*it)->operation == OperationType::jmp) || ((*it)->operation == OperationType::jmpOnFalse))
		{
			current.push_back((*it));
			addBasicBlock(current);
			current.clear();
			continue;
		}

		current.push_back((*it));
	}
	addBasicBlock(current);
}

void controlFlowGraph::addBasicBlock(const std::list<Tetrad*>& tetrads) {
	if (tetrads.empty()) {
		return;
	}

	BasicBlock* newBB = new BasicBlock(tetrads);
	cfg.push_back(newBB);
}

void controlFlowGraph::print()
{
	for (auto it = cfg.begin(); it != cfg.end(); it++)
	{
		(*it)->print();
	}
}

void BasicBlock::print()
{
	std::cout << "\n\n___________________\n";
	for (auto it = instructions.begin(); it != instructions.end(); it++)
	{
		(*it)->print();
	}
	std::cout << "___________________\n\n";
}
*/
