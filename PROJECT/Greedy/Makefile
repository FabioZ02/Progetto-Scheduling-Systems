# Nome dell'eseguibile finale
TARGET = ra

# Compilatore
CXX = g++

# Opzioni di compilazione
CXXFLAGS = -Wall -Wextra -std=c++17 -O3

# File sorgenti
SRCS = RA_Driver.cc RA_Data.cc RA_Greedy.cc

# File oggetto
OBJS = $(SRCS:.cc=.o)

# Regola principale
all: $(TARGET)

# Linking
$(TARGET): $(OBJS)
	$(CXX) $(CXXFLAGS) -o $@ $^

# Compilazione dei .cc in .o
%.o: %.cc
	$(CXX) $(CXXFLAGS) -c $< -o $@

# Pulizia
clean:
	rm -f $(OBJS) $(TARGET)
