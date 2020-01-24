compile: quick_sort.cpp SSSp.cpp
	mpic++ -o quick quick_sort.cpp
	mpic++ -o short	SSSp.cpp

exec_short:
	mpirun -np 3 --oversubscribe short

exec_quick:
	mpirun -np 3 --oversubscribe quick