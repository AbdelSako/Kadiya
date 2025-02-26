To compile a file located in clientAPI/ or serverAPI/ directory, first, cd to the directory of the file you wish to compile and run the command:
shell$> make "name of file to compile without extension"

Per example, to compile mainHttpProxy.cpp, first, cd to serverAPI/ directory and issue the command:
shell$> make mainHttpProxy

And then the binary will be outputted in the ../../bin/ directory as mainHttpProxy.
