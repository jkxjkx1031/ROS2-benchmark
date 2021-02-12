#!/bin/bash
roscore &
corepid=$!

sleep 3
source devel/setup.bash
rosrun service_test server &
serverpid=$!
rm result_*.txt
for i in {1..9}
do
    rosrun service_test client $i 1000 > "result_${i}.txt" &
done

sleep 20
kill $serverpid
kill $corepid