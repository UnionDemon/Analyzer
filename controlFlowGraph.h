#pragma once
#include "Tetrad.h"

class BasicBlock {
private:
	std::list<Tetrad*> instructions;
public:
	BasicBlock(const std::list<Tetrad*> &instruct);
	void print();
};

class controlFlowGraph {
private:
	std::list<BasicBlock*> cfg;
	void initCFG(const std::list<Tetrad*> &pseudoCode);
	void addBasicBlock(const std::list<Tetrad*>& tetrads);
public: 
	controlFlowGraph(const std::list<Tetrad*> &pseudoCode);
	void print();
}; 
