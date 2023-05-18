#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

using namespace std;

int main() {
    int port = 4200;
    const char *adresIP = "127.0.0.1";
    char answer[1024] = {0};  // na odbierane wiadomości

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
    server_addr.sin_addr.s_addr = inet_addr(adresIP);

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
    int tries = 0;
    do {
        // podanie loginu i hasła
        cout << "Podaj login: ";
        cin >> login;

        cout << "Podaj hasło: ";
        cin >> password;

        // wysłanie loginu
        send(client, login, strlen(login), 0);
        // odbior info zwrotego i potwierdzenie okejności
        if (read(client, answer, 1024) < 0) {
            cout << "Cos poszlo nie tak..."
                 << "\n";
            continue;
        } else {
            cout << answer << "\n";
            if (strcmp("OK", answer) != 0) {
                cout << "Sprawdź poprawność loginu."
                     << "\n";
                tries++;
                continue;
            }
        }
        // wysłanie hasła i potwierdzenie
        send(client, password, strlen(password), 0);
        if (read(client, answer, 1024) < 0) {
            cout << "Cos poszlo nie tak..."
                 << "\n";
            continue;
        } else {
            cout << answer << "\n";
            if (strcmp("OK", answer) != 0) {
                cout << "Sprawdź poprawność hasła."
                     << "\n";
                tries++;
                continue;
            }
        }

        login_succesful = 1;

    } while (!login_succesful && tries < 4);

    if (tries == 3) {
        close(client);
        return -1;
    }

    int choice;  // TODO: jakiś enum moze
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
            case 1:  // wyświetlenie nieprzeczytanych wiadomości
                send(client, "see_m", strlen("see_m"), 0);
                read(client, answer, 1024);
                cout << answer << "\n";

            case 2:  // wysłanie wiadomości
                send(client, "send", strlen("send"), 0);

                char recipient[50], message[500];

                cout << "Podaj odbiorcę: ";
                cin >> recipient;
                send(client, recipient, strlen(recipient), 0);
                read(client, answer, 1024);
                cout << answer << "\n";
                if (strcmp("OK", answer) != 0)
                    break;  // jeśli serwer zwroci nie ok (ze nie ma
                            // odbiorcy),
                //  to koniec

                cout << "Wpisz wiadomość: ";
                cin >> message;
                send(client, message, strlen(message), 0);

            case 3:  // dodanie znajomego
                send(client, "add", strlen("add"), 0);
                char *new_friend;
                cout << "Podaj znajomego: ";
                cin >> new_friend;
                send(client, new_friend, strlen(new_friend), 0);

            case 4:  // wyświetlenie znajomych
                send(client, "see_f", strlen("see_f"), 0);
                read(client, answer, 1024);
                cout << answer << "\n";

            case 5:  // zakończenie połączenia
                break;
        }
    } while (choice != 5);

    //  zakończenie połączenia
    close(client);
    return 0;
}