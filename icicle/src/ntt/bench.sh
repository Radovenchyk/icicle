#!/bin/bash
make;
# Loop over the two possible values for the third argument
for third_arg in 0 1; do
    # Nested loop for the first argument range 8 to 28
    for j in {1..10}; do
        echo "run #$j"
        for i in {8..28}; do
            # Run the command with the current third argument
            ./work/test_dcct $i 1 $third_arg 1
        done
    done
done