#pragma once
#include "controlFlowGraph.h"
#include "Tetrad.h"
#include <string>
#include <list>
#include <map>



enum class errorType {
	other,
	nullPtrDereference
};

struct error {
	errorType type;
	std::string location;
	std::string message;
};

enum class pointerValue
{
	any, 
	null,
	notNull
};

class Interpreter {
private:
	controlFlowGraph* graph;
	std::list<error*> errors;

	std::map<int, bool> visitedBlock;
	std::map<std::string, pointerValue> pointers;
	void init();
	void walkOnGraph(BasicBlock* bb);
	void interpretTetrads(const std::list<Tetrad*>& tetrads);
	void handleTetrad(Tetrad* tetrad);
	void handleDereference(Tetrad* tetrad);
	void addPointersToTable(Tetrad* tetrad);
public:
	Interpreter(controlFlowGraph* cfg);
	void run();
	const std::list<error*>& getErrors();
};