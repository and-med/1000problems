#!/bin/bash
set -e

PROBLEM_NAME=$1

dir="leetcode/$PROBLEM_NAME"
mkdir -p "$dir"

cat > "$dir/main.cpp" <<EOF
#include <iostream>
using namespace std;

int main() {

}
EOF

cat > "$dir/Makefile" <<EOF
build:
	mkdir -p bin
	/usr/bin/c++ -o bin/main main.cpp
EOF

touch "$dir/problem.txt"
touch "$dir/unsolved"

echo "created problem: $dir"