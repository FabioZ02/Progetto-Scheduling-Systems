OPTIONS = -Wall -Wfatalerrors -O3

RA_Test.exe: RA_Driver.o Ra_Data.o RA_Greedy.of
        g++ -o RA_Test.exe RA_Driver.o RA_Data.o RA_Greedy.o RA_Greedy.o

RA_Data.o: RA_Data.cc RA_Data.hh
        g++ $(OPTIONS) -c RA_Data.cc

RA_Greedy.o: RA_Greedy.cc RA_Greedy.hh RA_Data.hh
        g++ $(OPTIONS) -c RA_Greedy.cc 

RA_Driver.o: RA_Driver.cc RA_Data.hh
        g++ $(OPTIONS) -c RA_Driver.cc

clean:
        rm -f RA_Data.o RA_Greedy.o RA_Driver.o RA_Test.exe