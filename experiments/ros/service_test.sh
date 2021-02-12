#!/bin/bash
roscore &
corepid=$!

sleep 3
source devel/setup.bash
rosrun service_test server &
serverpid=$!
for i in {1..5}
do
    rosrun service_test client $i 1000 > "result_${i}.txt" &
done

sleep 10
kill $serverpid
kill $corepid