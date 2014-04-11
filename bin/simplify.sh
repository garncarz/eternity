#!/bin/bash

# kompletni rezie okolo reseni externim SAT solverem Simplify

eternity sat not
echo simplify
simplify sat.txt > sat_result.txt
eternity unsat
