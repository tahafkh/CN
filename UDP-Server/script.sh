SWS=20
RED_EN=1
FILE=test.txt
ROUTER_BUFF_SIZE=20
STATION_BUFF_SIZE=30

rm -rf logs/
mkdir logs/

make

gnome-terminal --tab --title="router" -- ./router $ROUTER_BUFF_SIZE $RED_EN
echo "Router $i started"
# sleep 2

gnome-terminal --tab --title="receiver" -- ./receiver $FILE $SWS $STATION_BUFF_SIZE
echo "Receiver $i started"

for i in {1..20} ; do
    ./sender $FILE $SWS $STATION_BUFF_SIZE $((8080+$i)) &
    # echo "Sender $i started"
done

SOP=$!
wait $SOP
pkill receiver
pkill router
pkill sender

make clean
