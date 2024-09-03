p01.exe: p01.cpp
	g++ p01.cpp -o p01.exe
p01.out: p01.cpp
	g++ p01.cpp -o p01.out
cleanWin:
	-del p01.exe
cleanLin:
	-rm p01.out