p01.out: p01.cpp
	g++ p01.cpp -o p01.out
p01.exe: p01.cpp
	g++ p01.cpp -o p01.exe
clean:
	-del p01.out
	-del p01.exe