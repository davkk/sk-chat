#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <stdexcept>
using namespace std;

void check_answer(char *answer, int client) {
    if (read(client, answer, 1024) < 0) {
        cout << "Zamykam połączenie."
             << "\n";
        close(client);
        exit(1);
    }

    if (strcmp("OK", answer) != 0) {
        cout << "Błąd: " << answer << "\n";
        close(client);
        exit(1);
    }
}

int main() {
    int port = 4200;
    string adresIP = "127.0.0.1";
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

    cout << "Podaj hasło: ";
    cin >> password;

    //  wysłanie loginu, odbior info zwrotego i potwierdzenie okejności
    send(client, login, strlen(login), 0);
    check_answer(answer, client);

    // wysłanie hasła i potwierdzenie
    send(client, password, strlen(password), 0);
    check_answer(answer, client);

    // sprawdzenie czy zalogowano
    check_answer(answer, client);

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

    } while (choice != 5);

    //  zakończenie połączenia
    close(client);
    return 0;
}
