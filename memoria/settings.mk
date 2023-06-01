# Libraries
LIBS=static commons pthread

# Custom libraries' paths
SHARED_LIBPATHS=
STATIC_LIBPATHS=../static

# Compiler flags
CDEBUG=-g -Wall -DDEBUG
CRELEASE=-O3 -Wall -DNDEBUG

# Arguments when executing with start, memcheck or helgrind
ARGS=memoria.config

# Valgrind flags
MEMCHECK_FLAGS=--track-origins=yes --show-leak-kinds=all -s --log-file="logs/memcheck.log"
HELGRIND_FLAGS=--log-file="logs/helgrind.log"
