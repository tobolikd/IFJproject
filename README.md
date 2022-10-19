# IFJproject

## project structure

 - ./ - general purpose files
   - CMake
   - README.md
 - src - project files
 - tests - test files
 - doc - documentation files


## code practices

 - function declarations, global variables, macros, etc. belongs to **header files**
 - functions have comment containing parametres, return value and description
 - **every commit in main has to be compilable** (warnings are allowed)
   - including tests - run "make" and "make tests"
 - DRY - don't repeat yourself
