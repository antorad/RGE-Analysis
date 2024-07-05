#!/bin/bash

#programs directories
#HIPO2ROOT="/home/antorad/hipo_tools/bin/hipo2root" #hipo2root ooriginal
HIPO2ROOT="/work/clas12/rg-e/antorad/clas12-rge-analysis/bin/hipo2root" #hipo2root bruno
#DST2ROOT="/home/antorad/hipo_tools/bin/dst2root" #dst2root
MAKENTUPLES="/work/clas12/rg-e/antorad/clas12-rge-analysis/bin/make_ntuples" #makentuples

#Directories neccesaries
HIPO_DIR="/volatile/clas12/rg-e/production/pass0.3/mon/recon/"
WORK_DIR="root_io"
OUT_DIR="/work/clas12/rg-e/antorad/clas12-rge-analysis/ntuple_files/"

# Define the file that contains the list of runs to process
RUN_LIST_FILE="../run_list.txt"

# Define the number of files to process in each subdirectory
NUM_FILES_TO_PROCESS=3

# Define the flag to process all files (set to true to process all, false to process a fixed number)
PROCESS_ALL_FILES=false

# Define the file extension to look for
FILE_EXTENSION=".hipo"

mkdir -p $WORK_DIR
cd $WORK_DIR

# Parse command-line options
while getopts ":a" opt; do
  case $opt in
    a)
      PROCESS_ALL_FILES=true
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      exit 1
      ;;
  esac
done

# Read the list of runs from the file
RUNS_TO_PROCESS=()
while IFS= read -r line || [ -n "$line" ]; do
  RUNS_TO_PROCESS+=("$line")
done < "$RUN_LIST_FILE"

# Iterate over each subdirectory in the run list
for RUN_NUMBER in "${RUNS_TO_PROCESS[@]}"; do
    SUBDIR=$(printf "$HIPO_DIR/%06d" $RUN_NUMBER)
    if [ -d "$SUBDIR" ]; then
        echo "Checking directory: $SUBDIR"
        # Find all files with the specified extension in the current subdirectory
        FILES=($(find "$SUBDIR" -maxdepth 1 -type f -name "*$FILE_EXTENSION"))
        FILE_COUNT=0
        # Process files based on the flag
        for FILE in "${FILES[@]}"; do
            if [ "$PROCESS_ALL_FILES" = false ] && [ "$FILE_COUNT" -ge "$NUM_FILES_TO_PROCESS" ]; then
                break
            fi
            echo "Processing file: $FILE"
    	    # Extract the file number between 'evio' and 'hipo' using sed
    	    FILE_NUMBER=$(echo "$FILE" | sed -n 's/.*evio\.\([0-9]*\)\.hipo/\1/p')
	    #run hipo2root and rename the output
            BANKS_OUT=$(printf "banks_%06d_%05d.root" $RUN_NUMBER $FILE_NUMBER) 
	    $HIPO2ROOT $FILE
	    mv banks_000000.root $BANKS_OUT
            FILE_COUNT=$((FILE_COUNT + 1))
        done
	echo "merging root banks"
	BANKS_RUN_OUT=$(printf "banks_%06d.root" $RUN_NUMBER)
	hadd $BANKS_RUN_OUT $(printf "banks_%06d_*.root" $RUN_NUMBER)
	echo "making ntuples"
	$MAKENTUPLES $BANKS_RUN_OUT
	mv $(printf "ntuples_dc_%06d.root" $RUN_NUMBER) $(printf "../ntuple_files/ntuples_dc_%06d.root" $RUN_NUMBER) 
    else
        echo "Directory $SUBDIR does not exist."
    fi
done

#for FILE in "${data_directory[@]::${num_files}}"
#do
#    echo "Processing file ${FILE}"
#    shortened_file_name=$(echo $FILE| rev | cut -d'/' -f -1 | rev) # Removing directory from file name
#    shortened_file_name=$(echo $shortened_file_name| rev | cut -d'.' -f -2 | rev) # String is now file_num.hipo
#    file_num=$(echo $shortened_file_name | cut -d'.' -f 1) # Extracting file_num
#    ${working_directory}/bin/hipo2root ${FILE}
#    root_file="${working_directory}/root_io/banks_000000.root"
#    ${working_directory}/bin/make_ntuples ${root_file}
#    mv ${working_directory}/root_io/banks_000000.root ${working_directory}/root_io/banks_${run_num}_${file_num}.root
#   mv ${working_directory}/root_io/ntuples_dc_000000.root ${working_directory}/root_io/ntuples_dc_${run_num}_${file_num}.root
#    mv ${working_directory}/root_io/sf_study_000000.root ${working_directory}/root_io/sf_study_${run_num}_${file_num}.root
#    mv ${working_directory}/data/sf_params_000000.txt ${working_directory}/data/sf_params_${run_num}_${file_num}.txt
#done
