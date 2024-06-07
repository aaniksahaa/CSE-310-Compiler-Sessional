flex -o state.cpp state.l
g++ state.cpp -lfl -o state.out
./state.out state.txt
