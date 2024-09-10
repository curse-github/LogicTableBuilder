p01.exe: p01.cpp
	g++ p01.cpp -o p01.exe
p01.out: p01.cpp
	g++ p01.cpp -o p01.out
cleanWin:
	-del p01.exe
cleanLin:
	-rm p01.out
p01Color.exe: p01Color.cpp
	g++ p01Color.cpp -o p01Color.exe
p01Color.out: p01Color.cpp
	g++ p01Color.cpp -o p01Color.out
cleanWinColor:
	-del p01Color.exe
cleanLinColor:
	-rm p01Color.out