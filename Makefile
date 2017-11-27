CC = clang++
CFLAGS = -g -O2 -Wall
CPPFLAGS = -std=c++11
ALL_CFLAGS = $(CPPFLAGS) $(CFLAGS)
RM = rm -f

OBJECTS = 
OBJECTS += src/common/sha1.o
OBJECTS += src/config/configuration.o

SOURCES = $(patsubst %.o,%.cpp,$(OBJECTS))

all: test_sha1

.PHONY: test_sha1

test_sha1:
	$(CC) $(ALL_CFLAGS) -o $@ $(SOURCES) test_sha1.cpp
	./$@
clean:
	$(RM) $(OBJECTS)

