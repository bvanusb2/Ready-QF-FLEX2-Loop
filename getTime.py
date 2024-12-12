# Python program to illustrate Python get current time
# Allows debug/test of FLEX2Processor.cpp input/output

import sys

# Importing datetime module
from datetime import datetime

# total arguments
n = len(sys.argv)
print("Total arguments passed:", n)

# Arguments passed
print("\nName of Python script:", sys.argv[0])

print("\nArguments passed:", end = " ")
for i in range(1, n):
    print(sys.argv[i], end = " ")

print("\n")
 
# storing the current time in the variable
c = datetime.now()

# Displays Time
current_time = c.strftime('%H:%M:%S')
print('Current Time is:', current_time)
# OR
# Displays Date along with Time
print('Current Date and Time is:', c)
