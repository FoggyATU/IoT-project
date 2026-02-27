from http.server import HTTPServer, BaseHTTPRequestHandler
import subprocess
from urllib.parse import urlparse

#THE IP OF THE SERVER
HOST = "localhost"
#THE PORT ON WHICH THIS WILL RUN
PORT = 6767

class AwesomeHttp(BaseHTTPRequestHandler):

    def do_GET(self):
        query:dict = get_query_dict(urlparse(self.path).query)
        theurl = query.get("url","")

        self.send_response(200)
        self.send_header("Content-type", "text/plain")
        self.end_headers()
        
        result:str = "None"
        if theurl != "":
            #This runs yt-dlp in a separate process and catches the output
            completedProcess:subprocess.CompletedProcess = subprocess.run(["yt-dlp","-x","--audio-format","mp3","--get-url",f"{theurl}"],capture_output=True)
            result = completedProcess.stdout

        #Return the output
        if result == "None":
            self.wfile.write(bytearray(result,"UTF-8"))
        else:
            self.wfile.write(result)

def run(server_class=HTTPServer, handler_class=AwesomeHttp):
    server_address = (HOST, PORT)
    httpd = server_class(server_address, handler_class)
    httpd.serve_forever()

def get_query_dict(thequery):
    split_query = thequery.split("&")

    queryDict = {}

    for q in split_query:
        s = q.split("=")
        if len(s) == 2:
            secondPart:str = s[1]
            if secondPart == "":
                secondPart = "None"

            queryDict[s[0]] = secondPart
        else:
            queryDict[s[0]] = "None"
    
    return queryDict

run()
