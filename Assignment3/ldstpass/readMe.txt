Author Mayukh Das(224101035)
-----------------------------------------------------------------------------------------------------------------------------
Steps to make and run pass
    *cd /home/<user_name>/LLVM/llvm-project-llvmorg-12.0.0/llvm/lib/Transforms/
    *mkdir ldstpass
    *Add this line in CMakeLists.txt in the transforms directory
        add_subdirectory(ldstpass) 
    make a ldstpass.cpp file in the ldstpass folder
    make a CMakeLists.txt file inside the ldstpass folder
    *cd ldstpass
    *Add the following lines of code in the CMakeLists.txt file inside ldstpass directory
        add_llvm_library(LLVMldstpass MODULE
            ldstpass.cpp

            PLUGIN_TOOL
            opt
        )
----------------------------------------------------------------------------------------------------------------------------------
Implementation of the LD/ST pass to insert instrumentation call
    Inherited from ModulePass class in Module.h header and used it to iterate through the functions 
    We iterate through each basic block of function except printSS() and printLL()
    For each basic block we iterate through each instructions 
    We check each load / store instruction and conditionally insert call instruction to printLL() or printSS() respectively
    We use the pass by parameter approach this parameter will contain the address of operand of load / store instruction
    We could not pass the parameter directly so we need a pointer to integer cast instruction to cast the pointer type value to integer 
    Finally we create a new modified.ll file containing the modified ir file
----------------------------------------------------------------------------------------------------------------------------
Next we need to build(or compile) our ldstpass
Write the following commands in terminal
*cd /home/<user_name>/LLVM/llvm-project-llvmorg-12.0.0/build/
*make
If no errors are present in the code it will compile successfully
Next we need to run our pass

make a testcases folder in build folder
*cd testcases
For that we need a lbm.c and supportingcode.c file to make a new .c file
    cat supportingcode.c lbm.c > lbmWSC.c
and generate an ir file lbmWSC.ll
    clang -emit-llvm -S lbmWSC.c -o lbmWSC.ll


if no compile errors are present .ll file is generated

now we can finally run the pass

run the following command in build folder

bin/opt -load LLVMLoadStore.so -LoadStore < testcases/lbmWSC.ll> /dev/null

A new modified.ll file will be generated in the testcases/ folder

Commands to run the modified.ll file

Directly to binary
    * llc -filetype=obj modified.ll -o modified.o
    * clang modified.o -o modified
    * ./modified
The code on running in modified .ll(modified.ll) file will create mytrace.txt will create a stack trace containing the hexadecimal addresses of operand in load/store instructions


