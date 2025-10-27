# =========================
# macOS CP Makefile (Clang)
# =========================

# Problem name = current folder name (A -> A.cpp -> ./A)
PROBLEM_NAME := $(notdir $(CURDIR))
TARGET       := $(PROBLEM_NAME)
SRC          := $(PROBLEM_NAME).cpp

# Toolchain (override with: make CXX=g++ if you really want GCC)
CXX ?= g++-15

# Toggle debug/sanitizers: make DEBUG=false  (default: true)
DEBUG ?= false

# ---- Flags (Clang/macOS friendly)
BASEFLAGS := -std=c++17 -mmacosx-version-min=26.0 -O2 -fdiagnostics-color=always -Wall -Wextra -pedantic -Wshadow -Wformat=2 -Wfloat-equal -Wconversion -Wlogical-op-parentheses -Wshift-sign-overflow -Wcast-qual -Wcast-align -Wno-unused-result -Wno-sign-conversion

SANFLAGS  := -fsanitize=address -fsanitize=undefined -fno-sanitize-recover=all \
             -fstack-protector-all -g

ifeq ($(DEBUG),true)
  CXXFLAGS := $(BASEFLAGS) $(SANFLAGS)
  LDFLAGS  := $(SANFLAGS)
else
  CXXFLAGS := $(BASEFLAGS)
  LDFLAGS  :=
endif

# ---- Default target
all: $(TARGET)
.PHONY: all

# Build rule
%: %.cpp
	$(CXX) $(CXXFLAGS) $< $(LDFLAGS) -o $@

# Clean
clean:
	-rm -rf $(TARGET)
.PHONY: clean

veryclean:
	-rm -rf $(TARGET) *.res
.PHONY: veryclean

rebuild: clean all
.PHONY: rebuild

# ======================
#   RUN / IO TEST HELPERS
# ======================

# Use macOS /usr/bin/time with -l for resource stats (max RSS, etc.)
TIMEBIN := /usr/bin/time
TIMEOPT := -l

# Run with timing
run: $(TARGET)
	$(TIMEBIN) $(TIMEOPT) ./$(TARGET)
ifeq ($(DEBUG),true)
	@echo "Built with DEBUG (asan/ubsan) — may run slower."
endif
.PHONY: run

# Input -> Result
%.res: $(TARGET) %.in
	$(TIMEBIN) $(TIMEOPT) ./$(TARGET) < $*.in > $*.res
ifeq ($(DEBUG),true)
	@echo "Built with DEBUG (asan/ubsan) — may run slower."
endif
.PRECIOUS: %.res

# Colors
GREEN := \033[0;32m
RED   := \033[0;31m
NC    := \033[0m   # No Color

# Compare .res (actual) with .out (expected)
__test_%: %.res %.out
	@echo "Testing $*..."
	@set -e; \
	if [ ! -f "$*.out" ]; then \
	  echo "$(RED)MISSING$(NC) - expected file '$*.out' not found"; exit 2; \
	fi; \
	if diff -q "$*.out" "$*.res" >/dev/null; then \
	  echo "$(GREEN)ACCEPT$(NC) - $*"; \
	else \
	  echo "$(RED)WRONG$(NC) - $*"; \
	  echo "---- diff (expected vs actual) ----"; \
	  diff -u "$*.out" "$*.res" | sed -E '\
	    s/^(\-.*)/\x1b[32m\1\x1b[0m/; \
	    s/^(\+.*)/\x1b[31m\1\x1b[0m/; \
	    s/^(@@.*)/\x1b[1m\1\x1b[0m/; \
	    s/^(--- .*)/\x1b[2m\1\x1b[0m/; \
	    s/^(\+\+\+ .*)/\x1b[2m\1\x1b[0m/'; \
	  echo "-----------------------------------"; \
	  exit 1; \
	fi
.PHONY: __test_%

# Batch helpers
CASES := $(sort $(basename $(wildcard *.in)))
TESTS := $(sort $(basename $(wildcard *.out)))

runs: $(patsubst %,%.res,$(CASES))
.PHONY: runs

solve: runs
.PHONY: solve

test: $(patsubst %,__test_%,$(TESTS))
.PHONY: test
