SRCDIR = .
BINDIR = ./bin/
PRODUCT_FLAGS =

SOURCES = \
          src/time.cpp\
          src/serializer.cpp\
          src/db_core.cpp\
          src/db_public.cpp\
          src/devices.cpp\
          src/protocol.cpp\
          src/socket.cpp\
          src/server.cpp\
          src/test.cpp

GEN_SOURCES = src/generator.cpp

CLIENT_SOURCES = \
          src/protocol.cpp\
          src/socket.cpp\
          src/client.cpp

TEST_SOURCES = \
          src/serializer.cpp\
          test/serializer_test.cpp\
          test/test.cpp

OBJECTS = $(SOURCES:%.cpp=%.o)
	
GEN_OBJECTS = $(GEN_SOURCES:%.cpp=%.o)
	
CLIENT_OBJECTS = $(CLIENT_SOURCES:%.cpp=%.o)

TEST_OBJECTS = $(TEST_SOURCES:%.cpp=%.o)

CXX = g++ -std=c++14

INCLUDES = -I/usr/local/include -I/usr/include -I../plog/include -I../CTPL/

CXXFLAGS = -Wall -Wextra ${INCLUDES} `pkg-config --cflags glib-2.0` 

LDLIBS = `pkg-config --libs glib-2.0`

LDFLAGS = -lm -lrt -lpthread 

%.o: %.cpp
	$(CXX)  $(PRODUCT_FLAGS) $(CXXFLAGS)  -c -o $@ $<

all: db devgen client test

db: $(OBJECTS)
	$(CXX) -o $(BINDIR)$@ $(OBJECTS) $(LDLIBS) $(LDFLAGS)

devgen: $(GEN_OBJECTS)
	$(CXX) -o $(BINDIR)$@ $(GEN_OBJECTS) $(LDLIBS) $(LDFLAGS)
	
client: $(CLIENT_OBJECTS)
	$(CXX) -o $(BINDIR)$@ $(CLIENT_OBJECTS) $(LDLIBS) $(LDFLAGS)

test:  $(TEST_OBJECTS)
	$(CXX) -o $(BINDIR)$@ $(TEST_OBJECTS) $(LDLIBS) $(LDFLAGS)

all:
	@echo "============================================================================================="; \
	echo "db successfully built"

clean:
	/bin/rm -f $(OBJECTS) $(GEN_OBJECTS) $(CLIENT_OBJECTS) $(TEST_OBJECTS) $(BINDIR)* *~ *.bin

