p01-General.exe: p01-General.cpp
	g++ p01-General.cpp -o p01-General.exe
p01-General.out: p01-General.cpp
	g++ p01-General.cpp -o p01-General.out
p01-Simple.exe: p01-Simple.cpp
	g++ p01-Simple.cpp -o p01-Simple.exe
p01-Simple.out: p01-Simple.cpp
	g++ p01-Simple.cpp -o p01-Simple.out
cleanWin:
	@-del p01-General.exe
	@-del p01-Simple.exe
cleanLin:
	@-rm ./p01-General.out
	@-rm ./p01-Simple.out