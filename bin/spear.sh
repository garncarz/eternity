#!/bin/bash

# kompletni rezie okolo reseni externim SAT solverem Spear

eternity sat
python $ETERNITY_BIN/sat2cnf.py sat.txt cnf.txt
echo spear
spear --dimacs cnf.txt --model > cnf_result.txt
python $ETERNITY_BIN/cnf2sat.py cnf_result.txt sat_result.txt
eternity unsat
