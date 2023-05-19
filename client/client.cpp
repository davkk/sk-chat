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
        cout << "Błąd: " << answer << "\n";
        close(client);
        exit(1);
    }
}

int main(int args, char *argv[]) {
    int port = atoi(argv[2]);
    string adresIP = argv[1];
    char answer[500] = {0};  // na odbierane wiadomości

    // tworzenie gniazda
    int client = socket(AF_INET, SOCK_STREAM, 0);
    if (client < 0) {
        cout << "Błąd stworzenia gniazda."
             << "\n";
        return -1;
    }

    // sockaddr_in ma informacje o adresie serwera
    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(adresIP.c_str());

    // łączenie z serwerem
    int connection =
        connect(client, (struct sockaddr *)&server_addr, sizeof(server_addr));
    if (connection < 0) {
        cout << "Błąd połączenia." << connection << "\n";
        return -1;
    }

    // logowanie
    char login[50], password[50];
    bool login_succesful = 0;

    // podanie loginu i hasła
    cout << "Podaj login: ";
    cin >> login;

    //  wysłanie loginu, odbior info zwrotego i potwierdzenie okejności
    send(client, login, strlen(login), 0);
    read_answer(answer, client);
    check_ok(answer, client);

    cout << "Podaj hasło: ";
    cin >> password;

    // wysłanie hasła i potwierdzenie
    send(client, password, strlen(password), 0);
    read_answer(answer, client);
    check_ok(answer, client);

    // sprawdzenie czy zalogowano
    read_answer(answer, client);
    check_ok(answer, client);

    int choice;  // TODO: jakiś enum moze
    char new_friend_login[50], friend_login[50];

    // pthread_t thread_message;
    // pthread_create(&thread_message, NULL, check_for_message, (void
    // *)&client); pthread_join(thread_message, NULL);

    do {
        cout << "Wpisz co chcesz zrobić:"
             << "\n";
        cout << "1 - wyświetlić nieprzeczytane wiadomości"
             << "\n";
        cout << "2 - wysłać wiadomość"
             << "\n";
        cout << "3 - dodać znajomego"
             << "\n";
        cout << "4 - wyświetlić znajomych"
             << "\n";
        cout << "5 - zakończyć połączenie"
             << "\n";

        do {
            cin >> choice;
        } while (choice > 5 || choice < 1);

        switch (choice) {
            case 1:
                send(client, "SHOW_UNREAD", strlen("SHOW_UNREAD"), 0);
                read_answer(answer, client);
                cout << atoi(answer) << " wiadomości"
                     << "\n";

            case 2:
                send(client, "SEND_MESSAGE", strlen("SEND_MESSAGE"), 0);
                cout << "Podaj komu chcesz wysłać wiadomość: ";
                cin >> friend_login;
                send(client, friend_login, strlen(friend_login), 0);
                read_answer(answer, client);
                check_ok(answer, client);

            case 3:
                send(client, "ADD_FRIEND", strlen("SEE_MESS"), 0);
                cout << "Podaj kogo chcesz dodać do znajomych: ";
                cin >> new_friend_login;
                read_answer(answer, client);
                check_ok(answer, client);

            case 4:
                send(client, "DISPLAY_FRIENDS ", strlen("SEE_MESS"), 0);

            case 5:
                break;
        }

    } while (choice != 5);

    //  zakończenie połączenia
    close(client);
    return 0;
}
