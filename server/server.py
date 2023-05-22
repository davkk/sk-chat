# pyright: reportUnknownMemberType=false
import socket
import sys
import threading
from typing import Tuple

from tinydb import Query, TinyDB


def handle_client(
    client_socket: socket.SocketType, address: Tuple[str, int], db: TinyDB
) -> None:
    login = client_socket.recv(50).decode()
    client_socket.send("OK".encode())
    print(f"[>] Received login ({login}) from {address}")

    password = client_socket.recv(50).decode()
    client_socket.send("OK".encode())
    print(f"[>] Received password from {address}")

    User = Query()
    users = db.table("users")
    user_filter = (User.login == login) & (User.password == password)

    current_user = users.get(user_filter)

    if current_user is None:
        print(f"[!] Invalid login ({login}) or password from {address}")
        client_socket.send("ERROR_USER".encode())

        print(f"[!] Closing connection from {address}")
        client_socket.close()
        return

    client_socket.send("OK".encode())
    print("[%] Found user:", str(current_user.get("login")))  # type: ignore

    client_ip, client_port = address
    users.update(
        dict(ip=client_ip, port=client_port, is_logged_in=True),
        user_filter,
    )

    while True:
        try:
            data = client_socket.recv(1024).decode()

            if not data:
                print(f"[-] Client {address} disconnected")
                users.update(dict(is_logged_in=False), user_filter)
                break

            print(f"[>] Received from {address}: {data}")

            match data:
                case "SHOW_FRIENDS":
                    Message = Query()
                    messages = db.table("messages")
                    friends = [
                        users.get(User.login == friend)
                        for friend in current_user.get("friends")  # type: ignore
                    ]

                    for friend in friends:
                        if friend is None:
                            continue

                        friend_messages = messages.search(
                            (Message.sender == friend.get("login"))
                            & (Message.receiver == current_user.get("login"))
                        )

                        if len(friend_messages) == 0:
                            client_socket.send(
                                f"{friend.get('login')}: NO MESSAGES".encode()
                            )
                            assert client_socket.recv(16).decode() == "OK"

                        else:
                            friend = (
                                f"*{friend.get('login')}"
                                if friend.get("is_logged_in")  # type: ignore
                                else friend.get("login")
                            )

                            last_message: str = friend_messages[-1].get("message")  # type: ignore

                            last_message: str = (
                                f"{last_message[:10]}..."
                                if len(last_message) > 10
                                else last_message
                            )
                            client_socket.send(
                                f"{friend}: {last_message}".encode()
                            )
                            assert client_socket.recv(16).decode() == "OK"

                case "SEND_MESSAGE":
                    continue

                case "ADD_FRIEND":
                    continue

                case "DISPLAY_FRIENDS":
                    print(f"[>] Sending list of friends to {address}")
                    client_socket.send(
                        str(current_user.get("friends")).encode()  # type: ignore
                    )

                case _:
                    continue

        except UnicodeDecodeError:
            print(f"[!] Invalid data received from {address}")
            continue

        except AssertionError:
            print(f"[!] Invalid data received from {address}")
            continue

    client_socket.close()
    sys.exit(0)


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
