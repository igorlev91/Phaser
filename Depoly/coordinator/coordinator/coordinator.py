from http.server import BaseHTTPRequestHandler, HTTPServer
import subprocess

# Remote Server IP: 
class UnrealServerRequestHandler(BaseHTTPRequestHandler):
    nextAvaliablePort = 7777
    def do_POST(self):
        print("handling request!!!")
        port = UnrealServerRequestHandler.nextAvaliablePort
        UnrealServerRequestHandler.nextAvaliablePort+=1

        sessionName = self.headers.get("SESSION_NAME")
        sessionUniqueId = self.headers.get("SESSION_GUID")
        self.LaunchSessionServer(sessionName, sessionUniqueId, port)
        self.send_response(200)
        self.send_header("PORT", port)
        self.end_headers()
        self.wfile.write(b"POST Received")

    
    def LaunchSessionServer(self, serverName, serverId, port):
        print(f"launching server with name: {serverName}, id: {serverId}, port: {port}")
        # "C:/JT/UnrealSrc/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe" %~dp0../BattleInfinity.uproject -server -log -epicapp="Server" -SESSION_UNIQUE_ID "TestUniqueIDValuesdfsdfsdfsdfsd"
        # EngineExcutable = "C:/UnrealEngine_Source/UnrealEngine/Engine/Binaries/Win64/UnrealEditor.exe"
        # ProjectPath = "C:/P4Dev/2025_ANGD_4440/Rabies/EngineProject/Rabies_ANGD.uproject"

        subprocess.Popen(["docker", "run",
                  "--rm",
                  "-d", # need to run this in detach mode
                  "-p", f"{port}:{port}/tcp",
                  "-p", f"{port}:{port}/udp",
                  "server",
                  "-server", "-log", "-epicapp=ServerClient", f"-SESSION_NAME={serverName}", f"-SESSION_GUID={serverId}", f"-PORT={port}"])


class UnrealSessionCoordinator:
    def __init__(self):
        self.HOST="0.0.0.0"
        self.PORT=80
        self.server = HTTPServer((self.HOST, self.PORT), UnrealServerRequestHandler)
        try:
            print("Start running at local host")
            self.server.serve_forever()
        except KeyboardInterrupt:
            print("shutting down server...")
            self.server.server_close()

UnrealSessionCoordinator()