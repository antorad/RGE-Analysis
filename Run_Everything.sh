# Plot Histogram (Also generated the output directory)
python3 Plot_var.py -v v_z -r C
python3 Plot_var.py -v v_z -r Cu
python3 Plot_var.py -v v_z -r Sn
python3 Plot_var.py -v v_z -r Pb


# Calculate the multiplocity ratio
root -b -l -q "Calculate_mr.cpp(\"C\", 2212)"
root -b -l -q "Calculate_mr.cpp(\"Cu\", 2212)"
root -b -l -q "Calculate_mr.cpp(\"Sn\", 2212)"
root -b -l -q "Calculate_mr.cpp(\"Pb\", 2212)"


root -b -l -q "Calculate_mr.cpp(\"C\", 321)"
root -b -l -q "Calculate_mr.cpp(\"Cu\", 321)"
root -b -l -q "Calculate_mr.cpp(\"Sn\", 321)"
root -b -l -q "Calculate_mr.cpp(\"Pb\", 321)"

root -b -l -q "Calculate_mr.cpp(\"C\", 211)"
root -b -l -q "Calculate_mr.cpp(\"Cu\", 211)"
root -b -l -q "Calculate_mr.cpp(\"Sn\", 211)"
root -b -l -q "Calculate_mr.cpp(\"Pb\", 211)"

root -b -l -q "Calculate_mr.cpp(\"C\", -211)"
root -b -l -q "Calculate_mr.cpp(\"Cu\", -211)"
root -b -l -q "Calculate_mr.cpp(\"Sn\", -211)"
root -b -l -q "Calculate_mr.cpp(\"Pb\", -211)"

# Plot multiplocity
python3 Plot_mr.py -v z_h -p 211
python3 Plot_mr.py -v nu -p 211

python3 Plot_mr.py -v z_h -p -211
python3 Plot_mr.py -v nu -p -211

python3 Plot_mr.py -v z_h -p 2212
python3 Plot_mr.py -v nu -p 2212

python3 Plot_mr.py -v z_h -p 321
python3 Plot_mr.py -v nu -p 321
