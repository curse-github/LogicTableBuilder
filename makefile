main.exe: main.cpp
	g++ main.cpp -o main.exe
cleanWinMain:
	-del main.exe
main.out: main.cpp
	g++ main.cpp -o main.out
cleanLinMain:
	-rm main.out
p01.exe: p01.cpp
	g++ p01.cpp -o p01.exe
cleanWinP01:
	-del p01.exe
p01.out: p01.cpp
	g++ p01.cpp -o p01.out
cleanLinP01:
	-rm p01.out