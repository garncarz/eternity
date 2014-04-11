# makefile
# urcuje pravidla pro automaticke kompilace, generovani a cisteni

# sestavi resic a zobrazovac
all: prepare bin/eternity bin/areaDisplayer.jar


# priprava pred kompilaci
prepare: tmp bin

# vytvoreni adresare pro docasne soubory
tmp:
	test -d tmp || mkdir tmp

# vytvoreni adresare pro spustitelne soubory
bin:
	test -d bin || mkdir bin


# sestaveni resice, hlavniho programu
bin/eternity: tmp/eternity.o tmp/area.o tmp/backtracker.o tmp/cards.o tmp/generator.o tmp/annealing.o tmp/pseudognt.o tmp/genetics.o tmp/sat.o tmp/unsat.o
	g++ -Wall $^ -o $@

# definice zavislosti jednotlivych .o souboru
tmp/eternity.o: src/eternity.cpp src/eternity.h src/area.h src/generator.h src/backtracker.h src/annealing.h src/pseudognt.h src/genetics.h src/sat.h src/unsat.h
tmp/area.o: src/area.cpp src/area.h src/cards.h src/define.h
tmp/backtracker.o: src/backtracker.cpp src/backtracker.h src/area.h src/cards.h src/define.h
tmp/cards.o: src/cards.cpp src/cards.h src/backtracker.h src/define.h src/area.h
tmp/generator.o: src/generator.cpp src/generator.h src/area.h
tmp/annealing.o: src/annealing.cpp src/annealing.h src/area.h src/cards.h
tmp/pseudognt.o: src/pseudognt.cpp src/pseudognt.h src/area.h src/cards.h src/define.h
tmp/genetics.o: src/genetics.cpp src/genetics.h src/area.h src/define.h src/cards.h
tmp/sat.o: src/sat.cpp src/sat.h src/area.h
tmp/unsat.o: src/unsat.cpp src/unsat.h src/area.h

# instrukce pro sestaveni .o souboru
tmp/%.o: src/%.cpp
	g++ -Wall -c $< -o $@


# sestavi Java zobrazovac plochy
bin/areaDisplayer.jar: src/AreaDisplayer.java src/Area.java
	javac $^ -d ./
	ls img > img/list.txt
	jar cfm $@ src/areaDisplayer.mf areaDisplayer/*.class img/*
	rm -f -r areaDisplayer


# vygeneruje Doxygen dokumentaci v adresari src
doxygen:
	cd src && doxygen -g
	cd src && doxygen


# sestavi bakalarskou praci a prezentaci v PDF
tex: all
# obecna priprava:
	cd doc && ./pictures.sh
	cd doc && ./eternity-help.sh
# zpracovani bakalarske prace:
	cp doc/bakalarka.tex tmp/
	cd tmp && pdflatex bakalarka.tex
	rm -f tmp/bakalarka.tex
	mv tmp/bakalarka.pdf doc/
# zpracovani puvodni (neoficialni) verze bakalarske prace:
#	cp doc/bakalarka-neoficialni.tex tmp/
#	cd tmp && pdflatex bakalarka-neoficialni.tex
#	rm -f tmp/bakalarka-neoficialni.tex
#	mv tmp/bakalarka-neoficialni.pdf doc/
# zpracovani prezentace:
	cp doc/prezentace.tex tmp/
	cd tmp && pdflatex prezentace.tex
	rm -f tmp/prezentace.tex
	mv tmp/prezentace.pdf doc/


# vycisti projekt od zkompilovanych, stahnutych a docasnych veci
clear:
	rm -f -r tmp
	rm -f img/list.txt img/Thumbs.db nohup.out
	rm -f bin/areaDisplayer.jar bin/eternity bin/eternity.exe
	rm -f -r download programs
	rm -f -r exp*
	rm -f src/Doxyfile src/Doxyfile.bak
	rm -f -r src/html src/latex
	rm -f doc/*.pdf

