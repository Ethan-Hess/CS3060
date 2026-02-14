CXX = g++
CXXFLAGS = -std=c++11 -o

TARGET = p4
SOURCE = p4.c

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(SOURCE) $(CXXFLAGS) $(TARGET)

clean:
	rm -f $(TARGET) *.o

.PHONY: all clean
