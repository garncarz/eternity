#!/bin/bash

# kompletni rezie okolo reseni externim SAT solverem zChaff

eternity sat
python $ETERNITY_BIN/sat2cnf.py sat.txt cnf.txt
echo zchaff
zchaff cnf.txt > cnf_result.txt
cp cnf_result.txt cnf_res.txt
python $ETERNITY_BIN/zchaff_correct.py cnf_result.txt
python $ETERNITY_BIN/cnf2sat.py cnf_result.txt sat_result.txt
eternity unsat
