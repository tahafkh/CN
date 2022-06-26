make
gnome-terminal -- ./router 4554 20 0
gnome-terminal -- ./receiver test.txt 20 1000
for i in {0..1} ; do
    gnome-terminal -- ./sender test.txt 20 1000 8080+$i
done
# make clean
