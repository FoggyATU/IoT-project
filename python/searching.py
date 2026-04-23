import requests
import os
import json

#make sure that the working directory is the same as the file, helps prevent errors
dir_path = os.path.dirname(os.path.realpath(__file__))
os.chdir(dir_path)
apikeyPath = r"apikeys.json"

with open(apikeyPath, "r") as file:
    apikeys = json.loads(file.read())

getsongapi = apikeys["getsong"]
youtubeapi = apikeys["youtube"]

def getSongs(bpm: int, limit=5):
    r = requests.get(f'https://api.getsong.co/tempo/?api_key={getsongapi}&bpm={bpm}&limit={limit}')
    return json.loads(r.text)

def get_youtube_link(song_name: str, artist: str):
    query = f"{song_name} {artist} official"
    url = "https://www.googleapis.com/youtube/v3/search"

    params = {
        "part": "snippet",
        "q": query,
        "type": "video",
        "maxResults": 1,
        "key": youtubeapi,
    }

    response = requests.get(url, params=params)
    response.raise_for_status()

    return json.loads(response.text)["items"]

def printSongData(song):
    title = song["song_title"]
    artist = song["artist"]["name"]

    print(f"{title} by {artist}")

    youtubeData = get_youtube_link(title, artist)
    videoid = youtubeData[0]['id']['videoId']
    print(f"https://youtu.be/{videoid}")




data = getSongs(127, 5)

#print(json.dumps(data, indent=4))

print("\n\n\n=== songs ===")

for song in data["tempo"]:
    printSongData(song)





print("\n\n")

