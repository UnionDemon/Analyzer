FLAGS=-I/usr/include -g -c -O0
LIBFLAGS=-L/usr/lib64 -lLLVM -lclang -lclang-cpp

build/app: build/main.o build/code_generator.o build/cfg.o build/tetrad.o build/ast_complement.o build/interp.o
	clang++ $(LIBFLAGS) build/main.o build/code_generator.o build/cfg.o build/tetrad.o build/ast_complement.o build/interp.o -o build/app

build/tetrad.o: Tetrad.cpp Tetrad.h
	clang++ $(FLAGS) Tetrad.cpp -o build/tetrad.o

build/code_generator.o: CodeGenerator.cpp
	clang++ $(FLAGS) CodeGenerator.cpp -o build/code_generator.o

build/cfg.o: controlFlowGraph.cpp controlFlowGraph.h
	clang++ $(FLAGS) controlFlowGraph.cpp -o build/cfg.o

build/ast_complement.o: astComplement.cpp astComplement.h
	clang++ $(FLAGS) astComplement.cpp -o build/ast_complement.o

build/interp.o: Interpreter.cpp Interpreter.h
	clang++ $(FLAGS) Interpreter.cpp -o build/interp.o

build/main.o: main.cpp
	clang++ $(FLAGS) main.cpp -o build/main.o

clean:
	-rm build/*
