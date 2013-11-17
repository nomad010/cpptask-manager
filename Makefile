CC=g++
CXXFLAGS=-pthread -std=c++0x -Wall

SRC  = src/main.cpp
OBJ = $(patsubst %.cpp, %.o, $(SRC))
EXE = test

$(EXE): $(OBJ)
	$(CC) $(CXXFLAGS) -o $(EXE) $(OBJ) $(LIBS)

%.o: %.cpp
	$(CC) $(CXXFLAGS) -c -o $@ $<

clean:
	$(RM) $(EXE) $(OBJ)
