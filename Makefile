CC = clang++
CFLAGS = -g -O2 -Wall -lz
CPPFLAGS = -std=c++11
ALL_CFLAGS = $(CPPFLAGS) $(CFLAGS)
RM = rm -f

OBJECTS = 
OBJECTS += src/common/sha1.o
OBJECTS += src/common/md5.o
OBJECTS += src/common/zipUtility.o
OBJECTS += src/config/configuration.o

SOURCES = $(patsubst %.o,%.cpp,$(OBJECTS))

all: clean

.PHONY: test_sha1 test_md5 test_zipUtility clean

test_sha1:
	$(CC) $(ALL_CFLAGS) -o $@ $(SOURCES) test_sha1.cpp
	./$@

test_md5:
	$(CC) $(ALL_CFLAGS) -o $@ $(SOURCES) test_md5.cpp
	./$@

test_zipUtility:
	$(CC) $(ALL_CFLAGS) -o $@ $(SOURCES) test_zipUtility.cpp
	./$@
clean:
	$(RM) $(OBJECTS)

