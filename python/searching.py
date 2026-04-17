import requests
import os

#make sure that the working directory is the same as the file, helps prevent errors
dir_path = os.path.dirname(os.path.realpath(__file__))
os.chdir(dir_path)
apikeyPath = r"apikey.txt"



with open(apikeyPath, "r") as file:
    apikey = file.read()

print(apikey)
