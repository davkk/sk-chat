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

void check_ok(char *answer, int client) {
    if (strcmp("OK", answer) != 0) {
        fprintf(stderr, "Błąd: %s\n", answer);
        close(client);
        exit(1);
    }
}

int main(int args, char *argv[]) {
    // todo: zabezpieczyć nie podanie argumentow

    int port = atoi(argv[2]);
    string ip_addr = argv[1];
    char answer[500] = {0};  // na odbierane wiadomości

    //  tworzenie gniazda
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
    char message[500] = {0};  // na wysyłane wiadomości

    do {
        printf(
            "\nWpisz co chcesz zrobić:\n"
            "1 - wyświetlić listę znajomych\n"
            "2 - wyświetlić wiadomości od znajomego\n"
            "3 - wysłać wiadomość\n"
            "4 - wylogować się\n"
            "> ");

        // todo - sprawdzić czy to liczba
        do {
            cin >> choice;
        } while (choice > 4 || choice < 1);

        switch (choice) {
            case 1: {
                send(sock, "SHOW_FRIENDS", strlen("SHOW_FRIENDS"), 0);

                char num_friends[50];
                read_answer(num_friends, sock);
                send(sock, "OK", strlen("OK"), 0);

                for (int i = 0; i < atoi(num_friends); i++) {
                    read_answer(answer, sock);
                    printf("--- %d ---\n", i + 1);
                    printf("%s\n", answer);
                    send(sock, "OK", strlen("OK"), 0);
                    bzero(answer, sizeof(answer));
                }

                break;
            }

            case 2: {
                send(sock, "SHOW_MESSAGES", strlen("SHOW_MESSAGES"), 0);
                printf(
                    "Podaj znajomego, ktorego wiadomości chesz zobaczyć:\n"
                    "> ");
                cin >> friend_login;
                send(sock, friend_login, strlen(friend_login), 0);
                read_answer(answer, sock);

                if (strcmp("OK", answer) != 0) {
                    fprintf(stderr, "Błąd: %s\n", answer);
                    bzero(answer, sizeof(answer));
                    break;
                }
                char num_messages[50];
                read_answer(num_messages, sock);
                send(sock, "OK", strlen("OK"), 0);

                for (int i = 0; i < atoi(num_messages); i++) {
                    read_answer(answer, sock);
                    printf("%s\n", answer);
                    send(sock, "OK", strlen("OK"), 0);
                    bzero(answer, sizeof(answer));
                }
                break;
            }

            case 3: {
                send(sock, "SEND_MESSAGE", strlen("SEND_MESSAGE"), 0);

                printf(
                    "Podaj znajomego, ktoremu chcesz wysłać "
                    "wiadomość:\n"
                    "> ");

                char friend_login[50];
                cin >> friend_login;

                send(sock, friend_login, strlen(friend_login), 0);
                read_answer(answer, sock);
                if (strcmp("OK", answer) != 0) {
                    fprintf(stderr, "Błąd: %s\n", answer);
                    bzero(answer, sizeof(answer));
                    break;
                }

                printf(
                    "Podaj wiadomość:\n"
                    "> ");

                char message[1024];
                cin >> message;

                send(sock, message, strlen(message), 0);
                read_answer(answer, sock);
                if (strcmp("OK", answer) != 0) {
                    fprintf(stderr, "Błąd: %s\n", answer);
                    printf("Nie wysyłaj pustych wiadomości :(\n");
                    bzero(answer, sizeof(answer));
                }

                break;
            }

            case 4:
                printf("Wylogowuję...\n");
                break;
        }
    } while (choice != 4);

    //  zakończenie połączenia
    close(sock);
    return 0;
}
