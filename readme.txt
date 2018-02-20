This is the simulation of the linker which is designed for the target machine specified in description.pdf

Makefile:
	there is a makefile in the directory, it makes executable on execution
	$ make
	Above command should generate executable.

	If it says executable is up to date, then cleaning would remove existing copy.
	$ make clean
	Above command will remove executable if it exists


Name of the executable is lab1
There are four modules:
	main function is defined in main.c module
	read.c module deals with reading token from the file and corresponding errors
	pass.c module defines the two pass of linker
	errors.c is to print the error, it is taken from the description pdf provided

The code is tested in courses2 environment of nyu.