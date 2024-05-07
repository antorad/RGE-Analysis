# Plot Histogram (Also generated the output directory)
python3 Plot_var.py -v v_z -r C
python3 Plot_var.py -v v_z -r Cu
python3 Plot_var.py -v v_z -r Sn
python3 Plot_var.py -v v_z -r Pb


# Calculate the multiplocity ratio
root -b -l -q "Calculate_mr.cpp(\"C\")"
root -b -l -q "Calculate_mr.cpp(\"Cu\")"
root -b -l -q "Calculate_mr.cpp(\"Sn\")"
root -b -l -q "Calculate_mr.cpp(\"Pb\")"

# Plot multiplocity
python3 Plot_mr.py -v z_h
python3 Plot_mr.py -v nu
