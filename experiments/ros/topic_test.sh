#!/bin/bash
roscore &
corepid=$!

sleep 3
source devel/setup.bash
for i in {1..5}
do
    rosrun topic_test sub $i > "result_${i}.txt" &
done
rosrun topic_test pub 1000

sleep 1
kill $corepid