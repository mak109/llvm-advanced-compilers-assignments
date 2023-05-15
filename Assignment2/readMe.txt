Author Mayukh Das(224101036)
-----------------------------------------------------------------------------------------------------------------------------
Steps to make and run pass
    *cd /home/<user_name>/LLVM/llvm-project-llvmorg-12.0.0/llvm/lib/transforms/
    *mkdir assignment2
    *Add this line in CMakeLists.txt in the transforms directory
        add_subdirectory(assignment2) 
    make a assignment2.cpp file in the assignment2 folder
    make a CMakeLists.txt file inside the assignment2 folder
    *cd assignment2
    *Add the following lines of code in the CMakeLists.txt file inside assignment2 directory
        add_llvm_library(llvmass2 MODULE
            assignment2.cpp

            PLUGIN_TOOL
            opt
        )
----------------------------------------------------------------------------------------------------------------------------------
Implementation of the assignment2
    Inherited from ModulePass class in Module.h header and used it to iterate through the functions 
    and basic block within each function and finding the basic block with maximum size
----------------------------------------------------------------------------------------------------------------------------
Next we need to build(or compile) our assignment2
Write the following commands in terminal
*cd /home/<user_name>/LLVM/llvm-project-llvmorg-12.0.0/build/
*make
If no errors are present in the code it will compile successfully
Next we need to run our pass
For that we need a lbm.c file and generate a ir file lbm.ll
make a testcases folder in build folder
*cd testcases
copy the lbm.c file inside it
run the following command
*clang -emit-llvm -S lbm.c -O1

if no compile errors are present .ll file is generated
This .ll file will contain the IR in SSA form with phi nodes

now we can finally run the pass

run the following command in build folder

bin/opt -load llvmass2.so -max-dot <testcases/lbm.ll> /dev/null

The dfg.dot will be generated which will contain the Dataflow graph 
To get a .png or .pdf

dot -Tpng dfg.dot -o dfg.png
 or
dot -Tpdf dfg.dot -o dfg.pdf



