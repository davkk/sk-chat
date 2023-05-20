#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <stdexcept>
using namespace std;

void read_answer(char *answer, int client) {
    if (read(client, answer, 1024) < 0) {
        cout << "Zamykam połączenie."
             << "\n";
        close(client);
        exit(1);
    }
}

void *check_for_message(void *client_arg) {
    char mess[1024] = {0};  // na odbierane wiadomości
    int client = *(int *)client_arg;
    while (!read(client, mess, 1024)) {
        cout << "Masz wiadomość: "
             << "\n";
        cout << mess << "\n";
    }
    pthread_exit(NULL);
    close(client);
    exit(1);
}

void check_ok(char *answer, int client) {
    if (strcmp("OK", answer) != 0) {
        fprintf(stderr, "Błąd: %s\n", answer);
        close(client);
        exit(1);
    }
}

int main(int args, char *argv[]) {
    int port = atoi(argv[2]);
    string ip_addr = argv[1];
    char answer[500] = {0};  // na odbierane wiadomości

    // tworzenie gniazda
    int sock = socket(AF_INET, SOCK_STREAM, 0);
    if (sock == -1) {
        perror("Błąd tworzenia gniazda");
        return 1;
    }

    // sockaddr_in ma informacje o adresie serwera
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip_addr.c_str());

    // łączenie z serwerem
    int connection =
        connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr));

    if (connection < 0) {
        perror("Błąd łączenia z serwerem");
        return 1;
    }

    // logowanie
    char login[50], password[50];
    bool login_succesful = 0;

    // podanie loginu i hasła
    cout << "Podaj login: ";
    cin >> login;

    //  wysłanie loginu, odbior info zwrotego i potwierdzenie okejności
    send(sock, login, strlen(login), 0);
    read_answer(answer, sock);
    check_ok(answer, sock);

    cout << "Podaj hasło: ";
    cin >> password;

    // wysłanie hasła i potwierdzenie
    send(sock, password, strlen(password), 0);
    read_answer(answer, sock);
    check_ok(answer, sock);

    // sprawdzenie czy zalogowano
    read_answer(answer, sock);
    check_ok(answer, sock);

    int choice;  // TODO: jakiś enum moze
    char new_friend_login[50], friend_login[50];

    do {
        printf(
            "\nWpisz co chcesz zrobić:\n"
            "1 - wyświetlić nieprzeczytane wiadomości\n"
            "2 - wysłać wiadomość\n"
            "3 - zakończyć połączenie\n"
            "> ");

        do {
            cin >> choice;
        } while (choice > 3 || choice < 1);

        switch (choice) {
            case 1: {
                send(sock, "SHOW_UNREAD", strlen("SHOW_UNREAD"), 0);

                char num_messages[50];
                read_answer(num_messages, sock);
                send(sock, "OK", strlen("OK"), 0);
                printf("Liczba nieprzeczytanych wiadomości: %s\n",
                       num_messages);

                for (int i = 0; i < atoi(num_messages); i++) {
                    read_answer(answer, sock);
                    printf("--- %d ---\n", i + 1);
                    printf("%s\n", answer);
                    send(sock, "OK", strlen("OK"), 0);
                }

                break;
            }

            case 2: {
                send(sock, "SEND_MESSAGE", strlen("SEND_MESSAGE"), 0);
                cout << "Podaj komu chcesz wysłać wiadomość: ";
                cin >> friend_login;
                send(sock, friend_login, strlen(friend_login), 0);
                read_answer(answer, sock);
                check_ok(answer, sock);
                break;
            }

            case 5:
                break;
        }

    } while (choice != 5);

    //  zakończenie połączenia
    close(sock);
    return 0;
}
