from http.server import HTTPServer, BaseHTTPRequestHandler
import subprocess
from urllib.parse import urlparse
from yt_dlp import YoutubeDL
import os

#THE IP OF THE SERVER
HOST = "127.0.0.1"
#THE PORT ON WHICH THIS WILL RUN
PORT = 6767

BUSY = False

class AwesomeHttp(BaseHTTPRequestHandler):

    def do_GET(self):
        global BUSY
        query:dict = get_query_dict(urlparse(self.path).query)
        theurl = query.get("url","")

        self.send_response(200)

        result:bytes = None
        if theurl != "" and not BUSY:
            BUSY = True
            download_audio(theurl)
            result = get_audio_buffer()
            remove_file()
            BUSY = False

            self.send_header("Content-type", "audio/mp3")
        else:
            self.send_header("Content-type", "text/plain")

        self.end_headers()

        #Return the output
        if result == None:
            self.wfile.write(bytearray("None","UTF-8"))
        else:
            self.wfile.write(result)

    def do_POST(self):
        self.do_GET()

def run(server_class=HTTPServer, handler_class=AwesomeHttp):
    server_address = (HOST, PORT)
    httpd = server_class(server_address, handler_class)
    httpd.serve_forever()

def get_query_dict(thequery):
    split_query = thequery.split("&")

    queryDict = {}

    for q in split_query:
        s = q.split("=")
        if len(s) >= 2:
            secondPart:str = s[1]
            if secondPart == "":
                secondPart = "None"

            queryDict[s[0]] = secondPart
        else:
            queryDict[s[0]] = "None"
    
    return queryDict

def download_audio(link:str):
    options = {'final_ext': 'mp3',
        'format': 'bestaudio/best',
        'postprocessors': [{'key': 'FFmpegExtractAudio',
                        'nopostoverwrites': False,
                        'preferredcodec': 'mp3',
                        'preferredquality': '5'}],
        'outtmpl': {'default': "audio.mp3"}
    }


    with YoutubeDL(options) as yt:
            yt.download(link)

def get_audio_buffer() -> bytes:
    with open("audio.mp3","r+b") as f:
        return f.read()

def remove_file():
    os.remove("audio.mp3")

run()
