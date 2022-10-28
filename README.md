# IFJproject

## project structure

 - ./ - general purpose files
   - CMake
   - README.md
 - src - project files
 - tests - test files
 - doc - documentation files


## code practices

 - function declarations, global variables, macros, etc. belong to **header files**
 - functions have comment containing parametres, return value and description
 - **every commit in main has to be compilable** (warnings are allowed)
   - including tests - run "make" and "make tests"
 - DRY - don't repeat yourself
 - debug logs should be active only if DEBUG (src/error-codes.h) is set to 1
 ```
 #if (DEBUG == 1)
 // your printf or whatever
 #endif
 ```
