import socket
import sys
import threading


def handle_client(
    client_socket: socket.SocketType,
    address: socket.SocketType,
) -> None:
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

        while True:
            address = None

            try:
                client_socket, address = server_socket.accept()
                print(f"[+] New connection from: {address}")

                threading.Thread(
                    target=handle_client,
                    args=(client_socket, address),
                ).start()

            except socket.error as err:
                print(f"[!] Accepting connection from {address} failed: {err}")
                continue


if __name__ == "__main__":
    port = int(sys.argv[1]) if len(sys.argv) > 1 else 4200
    server(port=port)
