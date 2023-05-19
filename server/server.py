# pyright: reportUnknownMemberType=false
import socket
import sys
import threading
from typing import Tuple

from tinydb import Query, TinyDB


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
    find_user = (User.login == login) & (User.password == password)

    user = users.get(find_user)

    if user is None:
        print(f"[!] Invalid login ({login}) or password from {address}")
        client_socket.send("ERROR_USER".encode())

        print(f"[!] Closing connection from {address}")
        client_socket.close()
        return

    client_socket.send("OK".encode())
    print("[%] Found user:", str(user.get("login")))  # type: ignore

    client_ip, client_port = address
    users.update(
        dict(ip=client_ip, port=client_port, is_logged_in=True),
        find_user,
    )

    print(users.all())

    while True:
        try:
            data = client_socket.recv(1024).decode()

            if not data:
                print(f"[-] Client {address} disconnected")
                users.update(dict(is_logged_in=False), find_user)
                print(users.all())
                break

            match data:
                case "SHOW_UNREAD":
                    unread_messages: list[str] = user.get("unread_messages")  # type: ignore
                    client_socket.send(str(len(unread_messages)).encode())

                    for message in unread_messages:
                        client_socket.send(message.encode())

                case "SEND_MESSAGE":
                    continue

                case "ADD_FRIEND":
                    continue

                case "DISPLAY_FRIENDS":
                    print(f"[>] Sending list of friends to {address}")
                    client_socket.send(
                        str(user.get("friends")).encode()  # type: ignore
                    )

                case _:
                    continue

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
