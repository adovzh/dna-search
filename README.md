dna-search
==========

TC Challenge
------------

To build and generate an 3,000,000,000 byte input file:

    $ make gendna
    $ ./gendna > input.txt

To build and run main program:

    $ make
    $ ./dna TTTTTATGGTAAGTAT input.txt

The output should be something like this:

	Feb16 23:28:58.037 START
	Feb16 23:28:59.547 FOUND SEQUENCE STARTING AT 2999999984
	Feb16 23:28:59.572 FOUND SEQUENCE STARTING AT 2600528088
	Feb16 23:28:59.715 FINISH

To recompile with an alternative algorithm issue:

    $ make clean all ALGO=BM
