EXE=asm_curses

SRC_DIR=src
OBJ_DIR=build
INC_DIR=inc
BIN_DIR=bin

SRC=$(wildcard $(SRC_DIR)/*.cpp)
OBJ=$(SRC:$(SRC_DIR)/%.cpp=$(OBJ_DIR)/%.o)

TST_EXE=$(EXE)_test

TST_DIR=test

TST_SRC=$(wildcard $(TST_DIR)/$(SRC_DIR)/*.cpp)
TST_OBJ=$(TST_SRC:$(TST_DIR)/$(SRC_DIR)/%.cpp=$(TST_DIR)/$(OBJ_DIR)/%.o)
TST_TGT=$(TST_SRC:$(TST_DIR)/$(SRC_DIR)/test_%.cpp=%.test)

CFLAGS= -g -Wall -O3 -I$(INC_DIR)

LDFLAGS=
LDLIBS= `pkg-config --libs ncurses`

TST_LIBS= -L/usr/lib -lboost_unit_test_framework

CC=g++

.PHONY: all clean docs test

all: $(EXE)

$(EXE): $(OBJ)
	$(CC) $(LDFLAGS) $^ $(LDLIBS) -o $(BIN_DIR)/$@

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	$(RM) $(OBJ)
	$(RM) $(TST_DIR)/$(OBJ_DIR)/$(TST_OBJ)
	$(RM) $(TST_DIR)/$(BIN_DIR)/$(TST_TGT)

docs:
	@doxygen

%.test: $(TST_DIR)/$(OBJ_DIR)/test_%.o $(OBJ:%main.o=)
	$(CC) $(TST_LIBS) $^ -o $(TST_DIR)/$(BIN_DIR)/$@
	$(TST_DIR)/$(BIN_DIR)/$@

test: $(TST_TGT)

$(TST_DIR)/$(OBJ_DIR)/%.o: $(TST_DIR)/$(SRC_DIR)/%.cpp
	$(CC) $(CFLAGS) -DBOOST_TEST_DYN_LINK -DBOOST_TEST_MAIN -c $< -o $@
