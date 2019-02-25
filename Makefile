SRCDIR = .
BINDIR = 
PRODUCT_FLAGS =

SOURCES = \
          time.cpp\
          serializer.cpp\
          db_core.cpp\
	  db_public.cpp\
          devices.cpp\
          protocol.cpp\
          socket.cpp\
          server.cpp\
          test.cpp

GEN_SOURCES = generator.cpp

CLIENT_SOURCES = \
          protocol.cpp\
          socket.cpp\
          client.cpp

OBJECTS = $(SOURCES:%.cpp=%.o)
	
GEN_OBJECTS = $(GEN_SOURCES:%.cpp=%.o)
	
CLIENT_OBJECTS = $(CLIENT_SOURCES:%.cpp=%.o)

CXX = g++ -std=c++14

INCLUDES = -I/usr/local/include -I/usr/include -I../plog/include

CXXFLAGS = -Wall -Wextra ${INCLUDES} `pkg-config --cflags glib-2.0` 

LDLIBS = `pkg-config --libs glib-2.0`

LDFLAGS = -lm -lrt -lpthread 

%.o: %.cpp
	$(CXX)  $(PRODUCT_FLAGS) $(CXXFLAGS)  -c -o $@ $<

all: db devgen client

db: $(OBJECTS)
	$(CXX) -o $(BINDIR)$@ $(OBJECTS) $(LDLIBS) $(LDFLAGS)

devgen: $(GEN_OBJECTS)
	$(CXX) -o $(BINDIR)$@ $(GEN_OBJECTS) $(LDLIBS) $(LDFLAGS)
	
client: $(CLIENT_OBJECTS)
	$(CXX) -o $(BINDIR)$@ $(CLIENT_OBJECTS) $(LDLIBS) $(LDFLAGS)

all:
	@echo "============================================================================================="; \
	echo "db successfully built"

clean:
	/bin/rm -f $(OBJECTS) $(GEN_OBJECTS) $(CLIENT_OBJECTS) $(BINDIR)db $(BINDIR)devgen *~ *.bin

