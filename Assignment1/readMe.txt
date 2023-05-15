Author Mayukh Das(224101036)
-----------------------------------------------------------------------------------------------------------------------------
Steps to make and run pass
    *cd /home/<user_name>/LLVM/llvm-project-llvmorg-12.0.0/llvm/lib/transforms/
    *mkdir newPass
    *Add this line in CMakeLists.txt in the transforms directory
        add_subdirectory(newPass) 
    make a newPass.cpp file in the newPass folder
    make a CMakeLists.txt file inside the newPass folder
    *cd newPass
    *Add the following lines of code in the CMakeLists.txt file inside newPass directory
        add_llvm_library(LLVMnewPass MODULE
            newPass.cpp

            PLUGIN_TOOL
            opt
        )
----------------------------------------------------------------------------------------------------------------------------------
Implementation of the newPass
    Inherited from ModulePass class in Module.h header and used it to iterate through the functions 
    and basic block within each function and printing the number of arguments of each function using F.arg_size()
----------------------------------------------------------------------------------------------------------------------------
Next we need to build(or compile) our newPass
Write the following commands in terminal
*cd /home/<user_name>/LLVM/llvm-project-llvmorg-12.0.0/llvm/build/
*make
If no errors are present in the code it will compile successfully
Next we need to run our pass
For that we need a hello.c file and generate a ir file hello.ll
make a testcases folder in build folder
*cd testcases
make a test.c file
run the following command
*clang -emit-llvm -S hello.c -o hello.ll

if no compile errors are present .ll file is generated

now we can finally run the pass

run the following command in build folder

bin/opt -load LLVMnewPass.so -newp < testcases/hello.ll> /dev/null


