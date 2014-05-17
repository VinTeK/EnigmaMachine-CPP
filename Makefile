CPP=g++
CFLAGS=-Wall -pedantic -g
OBJS=Enigma_Letter.o Enigma_Map.o Enigma_Rotor.o Enigma_Machine.o Enigma_Interface.o
EXE=enigma

$(EXE): $(OBJS) Enigma_NCurses_Main.cpp
	$(CPP) $(CFLAGS) -o $@ $(OBJS) Enigma_NCurses_Main.cpp -lncurses

test: $(OBJS) Enigma_Test.cpp
	$(CPP) $(CFLAGS) -o $@ $(OBJS) Enigma_Test.cpp

%.o: %.cpp
	$(CPP) $(CFLAGS) -c $<

clean:
	rm -f $(EXE) $(OBJS) test
