#pragma once
#include "Tetrad.h"

class BasicBlock {
private:
	std::list<Tetrad*> instructions;
public:
	BasicBlock(std::list<Tetrad*> instruct);
};

class controlFlowGraph {
private:
	std::list<BasicBlock*> cfg;
	void initCFG(const std::list<Tetrad*> &pseudoCode);
public: 
	controlFlowGraph(const std::list<Tetrad*> &pseudoCode);
}; 