main: main.cpp
	g++ -Wall main.cpp -o main -I/opt/picoscope/include/ -L/opt/picoscope/lib -lps3000a -lncurses
