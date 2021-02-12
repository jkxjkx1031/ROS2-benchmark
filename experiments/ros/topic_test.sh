#!/bin/bash
roscore &
corepid=$!

sleep 3
source devel/setup.bash
rm result_*.txt
for i in {1..9}
do
    rosrun topic_test sub $i > "result_${i}.txt" &
done
rosrun topic_test pub 1000

sleep 1
kill $corepid