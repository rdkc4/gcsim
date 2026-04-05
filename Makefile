CXX = clang++
CXXFLAGS = -std=c++23 -g -O1 -pthread \
           -Wall -Wextra -Wpedantic -Werror \
           -fno-omit-frame-pointer \
           -fno-optimize-sibling-calls \
           -D_GLIBCXX_ASSERTIONS

PERFCXXFLAGS = -std=c++23 -g -O2 -fno-omit-frame-pointer -DNDEBUG -pthread

BENCHMARKCXXFLAGS = -std=c++23 -O3 -DNDEBUG -pthread

SANITIZERS = -fsanitize=address,undefined

SRCS = main.cpp \
	src/common/header/header.cpp \
	src/common/segment/segment-info.cpp \
	src/common/segment/segment.cpp \
	src/common/thread-pool/thread-pool.cpp \
	src/common/rng/rng-sim.cpp \
	src/heap/heap.cpp \
	src/root-set-table/shared-register-space.cpp \
	src/root-set-table/shared-global-space.cpp \
	src/root-set-table/thread-local-stack.cpp \
	src/root-set-table/root-set-table.cpp \
	src/segment-free-memory-table/segment-free-memory-table.cpp \
	src/garbage-collector/ms-garbage-collector.cpp \
	src/garbage-collector/mc-garbage-collector.cpp \
	src/heap-manager/heap-manager.cpp \
	src/cli/cli.cpp \
	src/diagnostics/diagnoser.cpp \
	src/allocators/allocators.cpp

OBJS = $(SRCS:.cpp=.o)
EXEC = gcsim

.PHONY: all clean distclean perf benchmark run

all: $(EXEC)

$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(SANITIZERS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(SANITIZERS) -MMD -MP -c $< -o $@

run: $(EXEC)
	./$(EXEC)

perf: CXXFLAGS := $(PERFCXXFLAGS)
perf: SANITIZERS :=
perf: clean $(EXEC)

benchmark: CXXFLAGS := $(BENCHMARKCXXFLAGS)
benchmark: SANITIZERS :=
benchmark: clean $(EXEC)

clean:
	rm -f $(OBJS) $(EXEC)

distclean:
	rm -f $(OBJS:.o=.d)

-include $(OBJS:.o=.d)
