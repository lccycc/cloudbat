#!/usr/bin/env vim

CC	:= g++
V	:= @
RM	+= -r
LIB += -Llib -lpthread
OBJ := ./obj/
SRC := ./src/
USER_FLAGS+= -O4 -I$(SRC) -std=c++11

targets := $(wildcard $(SRC)main/*.cpp) $(wildcard $(SRC)main/*.c)
objects := $(wildcard $(SRC)*/*.cpp) $(wildcard $(SRC)*/*.c)
objects := $(filter-out $(targets), $(objects))
objects := $(patsubst %.cpp,%.o,$(objects))
dirctry := $(sort $(dir $(objects)))
dirctry := $(patsubst %/,%,$(dirctry))
objects := $(notdir $(objects))
objects := $(addprefix $(OBJ),$(objects))
targets := $(basename $(notdir $(targets)))
targets := $(addprefix $(OBJ),$(targets))

define make-target
$(OBJ)$1: $(SRC)main/$1.cpp $(objects)
	@echo + cc $$<
	$(V)$(CC) $(USER_FLAGS) -o $$@ $$^ $(LIB)
endef

define make-intermediate 
$(OBJ)%.o: $1/%.cpp
	@echo + cc $$<
	$(V)$(CC) -c $(USER_FLAGS) -o $$@ $$^ 
endef

all:always $(targets)
runfootprintsched:
	$(V) time $(OBJ)/main FOOTPRINT 1>log/sched.out 2>log/sched.err
runreusedstsched:
	$(V) time $(OBJ)/main REUSEDST 1>log/sched.out 2>log/sched.err
runbubblesched:
	$(V) time $(OBJ)/main BUBBLE 1>log/sched.out 2>log/sched.err
runnoprediction:
	$(V) time $(OBJ)/main NOPREDICTION 1>log/sched.out 2>log/sched.err
freerun:
	$(V) time $(OBJ)/main FREERUN 1>log/sched.out 2>log/sched.err
runbubblebuild:
	$(V) $(OBJ)/bubblebuild

$(foreach btar,$(targets),$(eval $(call make-target,$(notdir $(btar)))))
$(foreach bdir,$(dirctry),$(eval $(call make-intermediate,$(bdir))))

.PHONY:clean always reset test
always:
	$(V)mkdir -p $(OBJ)
clean:
	$(V)$(RM) $(OBJ)
