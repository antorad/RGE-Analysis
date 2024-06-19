# Plot Histogram (Also generated the output directory)
python3 Plot_var.py -v v_z -r C
python3 Plot_var.py -v v_z -r Cu
python3 Plot_var.py -v v_z -r Sn
python3 Plot_var.py -v v_z -r Pb
python3 Plot_var.py -v v_z -r Al
python3 Plot_var.py -v phi -r Al


### Calculate the multiplocity ratio
root -b -l -q "Calculate_mr.cpp(\"C\", 211)"
root -b -l -q "Calculate_mr.cpp(\"Cu\", 211)"
root -b -l -q "Calculate_mr.cpp(\"Sn\", 211)"
root -b -l -q "Calculate_mr.cpp(\"Pb\", 211)"
root -b -l -q "Calculate_mr.cpp(\"Al\", 211)"

root -b -l -q "Calculate_mr.cpp(\"C\", -211)"
root -b -l -q "Calculate_mr.cpp(\"Cu\", -211)"
root -b -l -q "Calculate_mr.cpp(\"Sn\", -211)"
root -b -l -q "Calculate_mr.cpp(\"Pb\", -211)"
root -b -l -q "Calculate_mr.cpp(\"Al\", -211)"

root -b -l -q "Calculate_mr.cpp(\"C\", 2212)"
root -b -l -q "Calculate_mr.cpp(\"Cu\", 2212)"
root -b -l -q "Calculate_mr.cpp(\"Sn\", 2212)"
root -b -l -q "Calculate_mr.cpp(\"Pb\", 2212)"
root -b -l -q "Calculate_mr.cpp(\"Al\", 2212)"

root -b -l -q "Calculate_mr.cpp(\"C\", 321)"
root -b -l -q "Calculate_mr.cpp(\"Cu\", 321)"
root -b -l -q "Calculate_mr.cpp(\"Sn\", 321)"
root -b -l -q "Calculate_mr.cpp(\"Pb\", 321)"
root -b -l -q "Calculate_mr.cpp(\"Al\", 321)"


### Broadening  Analysis
g++ -Wall -fPIC -I ./include `root-config --cflags` Data_Phi.cpp -o ./bin/Data_Phi  `root-config --glibs` ./include/Acc.h 
./bin/Data_Phi 

g++ -Wall -fPIC -I ./include `root-config --cflags` Integration.cpp -o ./bin/Integration `root-config --glibs` ./include/Pt2Processing2.h
./bin/Integration


g++ -Wall -fPIC -I./include `root-config --cflags` Broadening.cpp -o ./bin/Broadening  `root-config  --glibs` ./include/Broad.h
./bin/Broadening


## Plot multiplocity
python3 Plot_mr.py -v z_h -p 211
python3 Plot_mr.py -v nu -p 211
python3 Plot_mr.py -v p_t2 -p 211

python3 Plot_mr.py -v z_h -p -211
python3 Plot_mr.py -v nu -p -211
python3 Plot_mr.py -v p_t2 -p -211

python3 Plot_mr.py -v z_h -p 2212
python3 Plot_mr.py -v nu -p 2212
python3 Plot_mr.py -v p_t2 -p 2212

python3 Plot_mr.py -v z_h -p 321
python3 Plot_mr.py -v nu -p 321
python3 Plot_mr.py -v p_t2 -p 321


## Plot Broadening
python3 Broadening.py
