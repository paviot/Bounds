PROGRAM = bounds

FILES = Bounds.cpp main.cpp

LIBCAMI = CamiProdRead

LIBNAMES = $(LIBCAMI) ConstrSystm

CAMI = ../libCamiProdRead

INCDIR = $(CAMI) ../libConstrSystm ../lp_solve_5.5.0.13

LIBDIR = $(CAMI) ../libConstrSystm

#####################################

OBJS = $(FILES:.cpp=.o)

LDLIBS =  $(addprefix -L, $(LIBDIR)) $(addprefix -l, $(LIBNAMES)) ../lp_solve_5.5.0.13/lpsolve55/liblpsolve55.a -ldl

OPT = -O

CXXFLAGS = $(OPT) $(addprefix -I,$(INCDIR)) -Wall

#####################################

all: $(PROGRAM)

$(PROGRAM): $(OBJS) $(CAMI)/lib$(LIBCAMI).a
	$(CXX) -o $@ $(OBJS) $(LDLIBS)
	doxygen Doxyfile
main.o: main.cpp Bounds.h
Bounds.o: Bounds.cpp Bounds.h

clean: 
	rm -f *.o $(PROGRAM)
