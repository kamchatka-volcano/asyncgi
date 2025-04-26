from http.server import BaseHTTPRequestHandler, HTTPServer

class MyHandler(BaseHTTPRequestHandler):
    protocol_version = "HTTP/1.1"

    def log_message(self, format, *args):
        pass  # Disable logging output

    def handle_request(self):
        request_text = self.requestline + "\n"
        for name, value in self.headers.items():
            if name != "Host":
                request_text += f"{name}: {value}\n"
        request_text += "\n"

        content_length = self.headers.get('Content-Length')
        if content_length:
            body = self.rfile.read(int(content_length)).decode('utf-8', errors='replace')
            request_text += body

        print(request_text)

        body = b'Hello'
        self.send_response_only(200)
        self.send_header('Content-Length', str(len(body)))
        self.send_header('Content-Type', 'text/plain')
        self.send_header('Connection', 'close')
        self.send_header('Set-Cookie', 'foo=bar; Secure')
        self.end_headers()
        self.wfile.write(body)

    def do_GET(self):
        self.handle_request()

    def do_POST(self):
        self.handle_request()

server = HTTPServer(('0.0.0.0', 8091), MyHandler)
server.handle_request()