main.out: main.cpp main.h
	g++ main.cpp -o main.out
clean:
	rm ./main.out