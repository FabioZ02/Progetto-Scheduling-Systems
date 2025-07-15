#!/bin/bash

# Pulizia dei file compilati
echo "Eseguendo make clean..."
make clean

# Compilazione del progetto
echo "Eseguendo make..."
make

# Esecuzione dei test per ogni file wlp da 1 a 10
for i in {1..10}; do
    echo "Eseguendo test per wlp$i..."
    ./wlp --main::instance ./Instances/wlp$i.dzn --main::method HC --HC::max_evaluations 100000000 --HC::max_idle_iterations 1000000
    echo "Risultato per wlp$i completato."
    echo "-----------------------------"
done

echo "Eseguendo test per wlp12.."
./wlp --main::instance ./Instances/wlp12.dzn --main::method HC --HC::max_evaluations 100000000 --HC::max_idle_iterations 1000000
echo "Risultato per wlp12 completato.."
echo "--------------------------"

echo "Eseguendo test per wlp15.."
./wlp --main::instance ./Instances/wlp15.dzn --main::method HC --HC::max_evaluations 100000000 --HC::max_idle_iterations 1000000
echo "Risultato per wlp15 completato.."
echo "--------------------------"

echo "Eseguendo test per wlp20.."
./wlp --main::instance ./Instances/wlp20.dzn --main::method HC --HC::max_evaluations 100000000 --HC::max_idle_iterations 1000000
echo "Risultato per wlp20 completato.."
echo "--------------------------"

echo "Eseguendo test per wlp30.."
./wlp --main::instance ./Instances/wlp30.dzn --main::method HC --HC::max_evaluations 100000000 --HC::max_idle_iterations 1000000
echo "Risultato per wlp30 completato.."
echo "--------------------------"

echo "Eseguendo test per wlp50.."
./wlp --main::instance ./Instances/wlp50.dzn --main::method HC --HC::max_evaluations 100000000 --HC::max_idle_iterations 1000000
echo "Risultato per wlp50 completato.."
echo "--------------------------"

echo "Eseguendo test per wlp100.."
./wlp --main::instance ./Instances/wlp100.dzn --main::method HC --HC::max_evaluations 100000000 --HC::max_idle_iterations 1000000
echo "Risultato per wlp100 completato.."
echo "--------------------------"