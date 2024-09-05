main.exe: main.cpp
	g++ main.cpp -o main.exe
p01.exe: p01.cpp
	g++ p01.cpp -o p01.exe
p01.out: p01.cpp
	g++ p01.cpp -o p01.out
cleanWinMain:
	-del main.exe
cleanWinP01:
	-del p01.exe
cleanLinP01:
	-rm p01.out