CXX = g++
CXXFLAGS = -shared -fPIC
LDFLAGS = -ldl
TARGET = intrudr.so
SRC = src/main.cpp

all: $(TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET) $(LDFLAGS)

clean:
	rm -f $(TARGET)
