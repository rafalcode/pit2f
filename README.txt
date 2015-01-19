Despite the number of files in this directory, there is only one production program: pit2fwh.
which stands for PIT (i.e. match) 2 _F_iles _W_ith _H_ash

This program is specific to a certain analysis, and involves matching a FASTA file against an isoform results file.
The output is sent to screen (STDOUT) so it needs to be captured or re-directed into another file.

So in what format is this output? Well the same format as the isoform results file.
Isoform results files have one sequence ID per line.

So you can see that the program actually filters the isoform results file. It omits the sequences lines which do _not_ appear in the fasta file.


Compilation instructions are at the top of the Makefile file.

There is a test directory
Run the test with
./pit2fwh testf/p1.fa testf/R.t > testme.t

and the output should be 

comp100014_c0_seq1      comp100014_c0   291     243.68  6.00    1.29    1.68    100.00
comp100_c0_seq2 comp100_c0      204     157.38  0.00    0.00    0.00    0.00
