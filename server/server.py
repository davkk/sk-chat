# pyright: reportUnknownMemberType=false
import socket
import sys
import threading
from typing import Tuple

from tinydb import Query, TinyDB

# OK
# USER_NOT_FOUND


def handle_client(
    client_socket: socket.SocketType, address: Tuple[str, int], db: TinyDB
) -> None:
    login = client_socket.recv(1024).decode()
    client_socket.send("OK".encode())
    print(f"[>] Received login ({login}) from {address}")

    password = client_socket.recv(1024).decode()
    client_socket.send("OK".encode())
    print(f"[>] Received password from {address}")

    User = Query()
    users = db.table("users")

    user = users.get(User.login == login and User.password == password)

    if user is None:
        client_socket.send("ERROR_USER".encode())
        client_socket.close()
        return

    client_socket.send("OK".encode())
    print(f"[%] Found user: {user}")

    while True:
        try:
            data = client_socket.recv(1024).decode()

            if not data:
                print(f"[-] Client {address} disconnected")
                break

            print(f"[>] Received from {address}: {data}")

        except UnicodeDecodeError:
            print(f"[!] Invalid data received from {address}")
            continue

    client_socket.close()


def server(*, port: int) -> None:
    print("[*] Initializing server ...")
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as server_socket:
        server_socket.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)

        try:
            print(f"[*] Binding socket to port {port} ...")
            server_socket.bind((socket.gethostname(), port))

            print(f"[=] Listening on {server_socket.getsockname()} ...")
            server_socket.listen(5)

        except socket.error as err:
            print("[!] Bind failed:", str(err))
            sys.exit(0)

        db = TinyDB("database.json")

        while True:
            address = None

            try:
                client_socket, address = server_socket.accept()
                print(f"[+] New connection from: {address}")

                threading.Thread(
                    target=handle_client,
                    args=(client_socket, address, db),
                ).start()

            except socket.error as err:
                print(f"[!] Accepting connection from {address} failed: {err}")
                continue


if __name__ == "__main__":
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 4200
    server(port=port)
