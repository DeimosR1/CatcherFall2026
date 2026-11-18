from flask import Flask, request, jsonify
import subprocess # Fire another thread to work on another task

SESSION_NAME_KEY="SESSION_NAME"
SESSION_SEARCH_ID_KEY="SESSION_SEARCH_ID"
SESSION_PORT_KEY="SESSION_PORT"
nextAvailablePort = 7777

app = Flask(__file__)

def CreateServerLocalTest(SessionName, SessionSearchID):
    global nextAvailablePort
    subprocess.Popen(
        [
            "C:/UnrealSources/Unreal574/Engine/Binaries/Win64/UnrealEditor.exe",
            "C:/RayaUnreal/Catcher/Catcher.uproject",
            "-server",
            "-log",
            "-epicapp=ServerClient",
            f"-{SESSION_NAME_KEY}={SessionName}",
            f"-{SESSION_SEARCH_ID_KEY}={SessionSearchID}",
            f"-{SESSION_PORT_KEY}={nextAvailablePort}"
        ]
    )
    usedPort = nextAvailablePort
    nextAvailablePort += 1

    return usedPort


@app.route('/Session', methods=['POST'])
def CreateServer():
    print(dict(request.headers))
    sessionName = request.get_json().get(SESSION_NAME_KEY)
    sessionSearchID = request.get_json().get(SESSION_SEARCH_ID_KEY)

    port = CreateServerLocalTest(sessionName, sessionSearchID)

    return jsonify({'status':'success', SESSION_PORT_KEY:port}), 200

if __name__ == "__main__":
    app.run(host="0.0.0.0", port=80)