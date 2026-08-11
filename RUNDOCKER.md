# Pintos Docker Environment & Build Commands

This document contains the essential commands to start the Docker container, set up the compiler toolchain, and run the Pintos test suite.

## 1. Start the Docker Container
Run this from your host machine's terminal to start the container and attach an interactive shell:
```bash
docker start -i pintos-env
```

## 2. Export the Compiler Path
Once inside the Docker container, you must tell the shell where the Pintos cross-compiler is located. Run this before trying to compile:
```bash
export PATH=/workspace/pintos-toolchain/x86_64/bin:$PATH
```

## 3. Navigate to the Build Directory
You cannot compile from the root `src` directory. You must navigate to the specific module you are working on (e.g., threads):
```bash
cd /workspace/pintos/src/threads/build
```

## 4. Compile and Grade
Once inside the correct build directory, use these commands to compile the OS and run the test suite:
* `make clean` (Clears out old build files to ensure a fresh compilation)
* `make` (Compiles the kernel and test files)
* `make check` (Runs the automated grading suite and outputs the results)