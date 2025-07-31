CXX      = g++
CXXFLAGS = -std=c++17 -O3 -Wall -I./src -I./src/utils
SRC      = src/main.cpp src/OrderBook.cpp src/utils/CSVParser.cpp
OBJ      = $(SRC:.cpp=.o)
TARGET   = reconstruction_hemanth    # ← no angle brackets

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $(CXXFLAGS) -o $@ $^

# compile any .cpp into .o
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ) $(TARGET)

.PHONY: all clean
