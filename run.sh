g++ -static -O2 -march=native -fomit-frame-pointer -funroll-loops main.cpp -o main && ./main.exe > output.txt
# g++ main.cpp -o main && ./main.exe > output.txt