import requests
import os
import json

#make sure that the working directory is the same as the file, helps prevent errors
dir_path = os.path.dirname(os.path.realpath(__file__))
os.chdir(dir_path)
apikeyPath = r"apikey.txt"



with open(apikeyPath, "r") as file:
    apikey = file.read()

r = requests.get(f'https://api.getsong.co/tempo/?api_key={apikey}&bpm=67&limit=5')

data = json.loads(r.text)
#print(json.dumps(data, indent=4))


print("\n\n\n=== songs ===")

for song in data["tempo"]:
    print(song["song_title"] + " - " + song["artist"]["name"])


print("\n\n")