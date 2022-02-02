# !/bin/bash
while true; do
	micro_pid=$(pgrep micro)
	if [ -n "$micro_pid" ]
	then
		leaks_result=$( leaks -q --list micro )
		lsof_result=$( lsof -p $micro_pid )
		processes=$(ps)
		echo "                                 LEAKS CHECK"
		echo "$leaks_result"
		echo "                                 FD CHECK"
		echo "$lsof_result"
		echo "                                 PROCESSES"
		echo "$processes"
	fi
	sleep 3
	clear
done
