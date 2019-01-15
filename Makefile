SRCDIR = .
BINDIR = 
PRODUCT_FLAGS =

SOURCES = \
          time.cpp\
          serializer.cpp\
          db_core.cpp\
	  db_public.cpp\
          devices.cpp\
          test.cpp

GEN_SOURCES = generator.cpp

OBJECTS = $(SOURCES:%.cpp=%.o)
	
GEN_OBJECTS = $(GEN_SOURCES:%.cpp=%.o)

CXX = g++ -std=c++14

INCLUDES = -I/usr/local/include -I/usr/include -I../plog/include

CXXFLAGS = -Wall -Wextra ${INCLUDES} `pkg-config --cflags glib-2.0` 

LDLIBS = `pkg-config --libs glib-2.0`

LDFLAGS = -lm -lrt -lpthread 

%.o: %.cpp
	$(CXX)  $(PRODUCT_FLAGS) $(CXXFLAGS)  -c -o $@ $<

all: db devgen

db: $(OBJECTS)
	$(CXX) -o $(BINDIR)$@ $(OBJECTS) $(LDLIBS) $(LDFLAGS)

devgen: $(GEN_OBJECTS)
	$(CXX) -o $(BINDIR)$@ $(GEN_OBJECTS) $(LDLIBS) $(LDFLAGS)

all:
	@echo "============================================================================================="; \
	echo "db successfully built"

clean:
	/bin/rm -f $(OBJECTS) $(GEN_OBJECTS) $(BINDIR)db $(BINDIR)devgen *~ *.bin

