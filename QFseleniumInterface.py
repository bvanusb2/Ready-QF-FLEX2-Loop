# 
# Filename: QFseleniumInterface.py
#
# Purpose: Serves as a Selenium interface to query and control DEO connected 
#          to a Quantum Flex
#

# Usage:
# python3 QFseleniumInterface.py ipAddr
#   e.g. 
# python3 QFseleniumInterface.py 10.183.169.70

# If you are having issues using this script with the QT gui, then make sure you can
# invoke the script from the command line.


import sys

# import webdriver
from selenium import webdriver
from selenium.webdriver.common.by import By


driver = webdriver.Chrome()

# 1st parameter is IP address of DEO to be accessed
ipAddr = sys.argv[1]
ipStr = "http://" + ipAddr + "/deo.html"

# "get" call returns void, so connection is tested by examining title
# more likely though, if IP addr is wrong, Selenium will spend forever trying
# to spin up page that doesn't exist
driver.get(ipStr) # e.g. http://10.183.169.70/deo.html

# If the connection is successful, the title of DEO is "UI Test App"
assert "UI Test App" in driver.title


#wow, takes at least 2, sometimes 3 sec for page to respond...ugh..
driver.implicitly_wait(3.0)

# The Python Selenium script will print this string at startup
# as an indication to the spawing function that the script is running
print("SeleniumQFInterfaceReady")


# todo!!! how can we report that we didn't connect???


#while loop to process input commands
inputStr = ""
while inputStr != "quit" :

    inputStr = input()
    if inputStr == "quit":
        break; 
    else:
        # e.g. "ECCirc_PumpCapRepoDisposablePumpStatus._accumVolMl.Value"
        mlDisplay = driver.find_element(by=By.ID, value = inputStr)

        print(mlDisplay.text);
        
driver.close()  # supposedly quit works too?
#print("done")
