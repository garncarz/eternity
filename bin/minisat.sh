#!/bin/bash

# kompletni rezie okolo reseni externim SAT solverem MiniSat

eternity sat
python $ETERNITY_BIN/sat2cnf.py sat.txt cnf.txt
echo minisat
minisat cnf.txt cnf_result.txt
python $ETERNITY_BIN/minisat_correct.py cnf_result.txt
python $ETERNITY_BIN/cnf2sat.py cnf_result.txt sat_result.txt
eternity unsat
