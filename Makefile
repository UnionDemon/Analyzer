FLAGS=-I/usr/include -g -c
LIBFLAGS=-L/usr/lib64 -lLLVM -lclang -lclang-cpp

build/app: build/main.o build/code_generator.o build/cfg.o build/tetrad.o
	clang++ $(LIBFLAGS) build/main.o build/code_generator.o build/cfg.o build/tetrad.o -o build/app

build/tetrad.o: Tetrad.cpp Tetrad.h
	clang++ $(FLAGS) Tetrad.cpp -o build/tetrad.o

build/code_generator.o: CodeGenerator.cpp
	clang++ $(FLAGS) CodeGenerator.cpp -o build/code_generator.o

build/cfg.o: controlFlowGraph.cpp controlFlowGraph.h
	clang++ $(FLAGS) controlFlowGraph.cpp -o build/cfg.o

build/main.o: main.cpp
	clang++ $(FLAGS) main.cpp -o build/main.o

clean:
	-rm build/*
