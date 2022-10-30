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

## tips and tricks
 - debug prints which should be printed to error output should be in macro
 - these logs will be in final solution
```
debug_print();
```
or
```
#if (DEBUG == 1)
    // anything what should be in final solution
#endif
```

 - debug logs should be active only if DEBUG\_LOGS (src/error-codes.h) is set to 1
 ```
 debug_log();
```
or
```
#if (DEBUG_LOG == 1)
    // anything what isn't needed in final solution
#endif

 ```
