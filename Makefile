SRCDIR = .
BINDIR = 
PRODUCT_FLAGS =

SOURCES = \
          time.cpp\
          serializer.cpp\
          db_core.cpp\
          devices.cpp\
          test.cpp

OBJECTS = $(SOURCES:%.cpp=%.o)

CXX = g++ -std=c++14

INCLUDES = -I /usr/local/include

CXXFLAGS = -Wall -Wextra ${INCLUDES} `pkg-config --cflags glib-2.0` 

LDLIBS = `pkg-config --libs glib-2.0`

LDFLAGS = -lm -lrt -lpthread 

%.o: %.cpp
	$(CXX)  $(PRODUCT_FLAGS) $(CXXFLAGS)  -c -o $@ $<

all: db

db: $(OBJECTS)
	$(CXX) -o $(BINDIR)$@ $(OBJECTS) $(LDLIBS) $(LDFLAGS)

all:
	@echo "============================================================================================="; \
	echo "db successfully built"

clean:
	/bin/rm -f $(OBJECTS) $(BINDIR)db  *~ *.bin

