This little application is developed to show how to use 
 * uP7 library 
 * uP7 proxy library.

It consist of next files:
 * cpu.cpp - uP7 usage example and primitive emulation of embedded CPU software.
 * host.cpp - uP7 proxy usage example

Usage example: 
1) Complile uP7preProcessor application
2) Using uP7preProcessor generate session file & header with uP7 descriptors 
 >uP7preProcessor <config.xml> <sources files dir> <output dir for session file>
 Command line prompt for current project will looks like:
 >uP7preProcessor <uP7Lib folder>/uP7Example/uP7preProcessor/uP7Preprocessor.xml <uP7Lib folder>/uP7Example <uP7Lib folder>/uP7Example/uP7preProcessor
3) Compile application
4) Run it:
 > Usage example: uP7example <uP7Lib folder>/uP7Example/uP7preProcessor

If "cpu.cpp" file will be changed - please repeat steps 2-4, because that changes have to be taken in account
and new session & descriptors header files have to be generated!
