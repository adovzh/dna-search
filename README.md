dna-search
==========

TC Challenge
------------

To build and generate an 3,000,000,000 byte input file:

    $ make gendna
    $ ./gendna > input.txt

To build and run main program:

    $ make
    $ ./dna TTTTTATGGTAAGTAT < input.txt

The output should be something like this:

    Feb11 13:31:39.045 START
    Feb11 13:31:47.680 FOUND SEQUENCE STARTING AT 2600528088
    Feb11 13:31:48.917 FOUND SEQUENCE STARTING AT 2999999984
    Feb11 13:31:48.917 FINISH

To recompile with an alternative algorithm issue:

    $ make clean all ALGO=KMP
