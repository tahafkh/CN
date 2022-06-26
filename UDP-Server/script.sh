make
gnome-terminal -- ./router 4554 20 0
gnome-terminal -- ./receiver test.txt 20 1000 8000
gnome-terminal -- ./sender test.txt 20 1000 8080
make clean
