#pragma once

#include <unordered_map>

#include "clang/AST/Stmt.h"

using namespace clang;

class StmtHash {
private:
	size_t class_name_hash;
	unsigned int begin_src_location;
	unsigned int end_src_location;
	int children;


public:
	StmtHash(Stmt* st)
	{
		std::hash<std::string> hasher;
		class_name_hash = hasher(st->getStmtClassName());

		begin_src_location = st->getBeginLoc().getHashValue();
		end_src_location = st->getEndLoc().getHashValue();

		children = 0;
		for (auto it = st->child_begin(); it != st->child_end(); it++) {
			children++;
		}
	}

	bool operator==(const StmtHash& other) {
		return class_name_hash == other.class_name_hash &&
			begin_src_location == other.begin_src_location &&
			end_src_location == other.end_src_location &&
			children == other.children;
	}
};