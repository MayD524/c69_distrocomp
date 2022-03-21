from http.server import SimpleHTTPRequestHandler, HTTPServer
from socketserver import ThreadingMixIn


class ThreadingHTTPServer(ThreadingMixIn, HTTPServer):
    pass

class server(SimpleHTTPRequestHandler):
    def set_headers(self,code:int=200) -> None:
        self.send_response(code)
        self.send_header("Access-Control-Allow-Origin", "*")
        self.send_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS")
        self.send_header("Access-Control-Allow-Headers", "X-Requested-With, content-type")
        self.end_headers()

    def do_OPTIONS(self):
        self.set_headers(200)
    
    def do_GET(self) -> None:
        if self.path.startswith("/task/"):
            pth = self.path.split("/task/")[1]
            ## send the task if there is one
            """
                read task from file (./tasks/task1.ext)
                send task to client (self.wfile.write(task_data))
            """
            with open(pth) as f:
                task_data = f.read()
            self.set_headers(200)
            self.wfile.write(task_data.encode())
    
    def do_POST(self) -> None:
        post_data = self.rfile.read(int(self.headers['Content-Length'])).decode()
        if self.path == "/result":
            ## save the result
            with open("./result/tmp.txt", "w+") as f:
                f.write(post_data)                
        
        elif self.path == "/register":
            ## register the client
            """
            machine_id=<machine_name>&cpu=<cpu_max>&ram=<ram_max>&disk=<disk_max>
                - Machine Name
                    - MAX CPU
                    - MAX RAM
                    - MAX Storage
            """
            stuff = post_data.split("&")
            stuff = {x.split("=")[0]:x.split("=")[1] for x in stuff}
            machines[stuff["machine_id"]] = stuff
            
        elif self.path == "/status":
            ## get the status of the client
            """
            machine_id=<machine_name>&current_cpu=<cpu_usage>&current_ram=<ram_usage>&current_disk=<disk_usage>
                - Machine Name
                    - Current CPU Usage
                    - Current RAM Usage
                    - Storage remaining
            """
            copy = False
            stuff = post_data.split("&")
            stuff = {x.split("=")[0]:x.split("=")[1] for x in stuff}
            MCH_name = stuff[0].value()
            CPU_useage = stuff[1].value()
            RAM_useage = stuff[2].value()
            STR_useage = stuff[3].value()

def run_server(host, port):
    server_address = (host,port)
    httpd = ThreadingHTTPServer(server_address,server)
    try:
        print("Starting server on {}:{}".format(host if host != '' else 'localhost', port))
        httpd.serve_forever()
    except KeyboardInterrupt:
        pass
    print('\nShutting down server...')
    httpd.server_close()
    print('Server shut down.')

if __name__ == '__main__':
    tasks: list[str] = []
    machines:dict[str, dict[str, int]] = {}