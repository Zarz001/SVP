#!/bin/bash

EXECUTABLE="./runme"

# Test file
TEST_FILE="test.txt"

total_cases=0
passed_cases=0
failed_cases=0
wrong_cases=0
failed_test_names="" 

#Process each test case
while IFS=: read -r test_case vectors expected; do
    ((total_cases++))

    #Skip test case if expected value is negative
    if [[ $(echo "$expected < 0" | bc -l) -eq 1 ]]; then
        ((wrong_cases++))
        echo "Wrong Test Case (negative expected value): $test_case"
        continue
    fi

    #Format vectors to be passed as individual arguments
    vectors1=$(echo "$vectors")

    #Run the program with the vectors and capture any errors
    error_output=$(eval "$EXECUTABLE" $vectors1 2>&1 > /dev/null)
    exit_status=$?

    if [ $exit_status -ne 0 ]; then
        echo "Execution error in test case: $test_case"
        echo "Error details: $error_output"
        ((failed_cases++))
        #Add the failed test case name with error
        failed_test_names+="$test_case (Error), " 
        continue
    fi

    #Read and compare the result
    if [ -f "result.txt" ]; then
        read -r result < result.txt
        echo "Test Case: $test_case, Expected: $expected, Got: $result"

        if [[ $(echo "$result == 0" | bc -l) -eq 1 || $(echo "$expected == 0" | bc -l) -eq 1 ]]; then
            echo "Result: PASS"
            ((passed_cases++))
            continue
        fi

        #Percentage difference
        percentage_diff=$(echo "scale=4; 100 * ($result - $expected) / $expected" | bc)
        abs_percentage_diff=${percentage_diff#-}

        if (( $(echo "$abs_percentage_diff <= 1" | bc -l) )); then
            echo "Result: PASS"
            ((passed_cases++))
        else
            echo "Result: FAIL"
            ((failed_cases++))
            #Add the failed test case name with its percentage difference
            failed_test_names+="$test_case (${abs_percentage_diff}% off), " 
        fi
    else
        echo "No result file for test case: $test_case"
        ((failed_cases++))
        # Add the failed test case name indicating no result file
        failed_test_names+="$test_case (No result file), " 
    fi
done < "$TEST_FILE"

#Print the summary
echo "============TEST CASE SUMMARY============="
echo "Total Cases: $total_cases"
echo "Passed Cases: $passed_cases"
echo "Failed Cases: $failed_cases"
if [ ! -z "$failed_test_names" ]; then
    echo "Failed Test Cases: ${failed_test_names%??}" 
fi
echo "Wrong Cases (length can't be negative): $wrong_cases"
