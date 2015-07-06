// clang++ -std=c++14 reusable_barrier_class.cpp barrier.cpp -o reuseable_barrier.exe
all: reusable_barrier_class rendezvous multi_rendezvous dancers

reusable_barrier_2:
	clang++ -std=c++14 reusable_barrier_class.cpp barrier.cpp -o reuseable_barrier_2.exe
reusable_barrier_1:
	clang++ -std=c++14 reusable_barrier.cpp -o reuseable_barrier_1.exe


rendezvous:
	clang++ -std=c++14 rendezvous.cpp -o rendezvous.exe
multi_rendezvous:
	clang++ -std=c++14 multi_rendezvous.cpp -o multi_rendezvous.exe
dancers:
	clang++ -std=c++14 dancers.cpp -o dancers.exe

