from http.server import HTTPServer, BaseHTTPRequestHandler

#THE IP OF THE SERVER
HOST = "127.0.0.1"
#THE PORT ON WHICH THIS WILL RUN
PORT = 6768

class AwesomeHttp(BaseHTTPRequestHandler):

    def do_GET(self):
        self.send_response(200)
        self.send_header("Content-type", "text/plain")

        self.end_headers()

        self.wfile.write(bytearray("One by Metallica,WM8bTdBs-cw","UTF-8"))

    def do_POST(self):
        self.do_GET()

def run(server_class=HTTPServer, handler_class=AwesomeHttp):
    server_address = (HOST, PORT)
    httpd = server_class(server_address, handler_class)
    httpd.serve_forever()

run()