#pragma once
#include "Tetrad.h"
#include <map>

class BasicBlock {
private:
	std::list<Tetrad*> instructions;
	int id;

	static int counter;
public:
	BasicBlock(const std::list<Tetrad*> &instruct);
	int getId();
	const std::list<Tetrad*>& getTetrads();

	void print();
};

class edge {
private:
	BasicBlock* destination;
public:
	edge(BasicBlock* dBB);
	BasicBlock* getDestination();
};

class controlFlowGraph {
private:
	std::list<BasicBlock*> cfg;
	std::map<int, BasicBlock*> blocksByLabels;
	std::map<int, std::list<edge*>> edges;

	void initCFG(const std::list<Tetrad*> &pseudoCode);
	void addBasicBlock(const std::list<Tetrad*>& tetrads);
	void addBlockByLabel(BasicBlock* bb);
	void createEdges();
	void createEdgeForJmp(BasicBlock* bb);
public: 
	controlFlowGraph(const std::list<Tetrad*> &pseudoCode);
	void print();
}; 
