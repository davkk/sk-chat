#include <arpa/inet.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iostream>

using namespace std;

// TODO: zrobić ładnie funkcję, obsługę błędow itp.
//  void check_answer(char answer, int client) {
//      if (read(client, answer, 1024) < 0) {
//          cout << "Cos poszlo nie tak..."
//               << "\n";
//          close(client);
//      }
//      if (strcmp("OK", answer) == 0) {
//          cout << answer << "\n";
//      } else {
//          cout << answer << "\n";
//          cout << "Zamykam połączenie."
//               << "\n";
//          close(client);
//      }
//  }

int main() {
    int port = 4200;
    const char *adresIP = "127.0.0.1";
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

    // podanie loginu i hasła
    cout << "Podaj login: ";
    cin >> login;

    cout << "Podaj hasło: ";
    cin >> password;

    // TODO: FUNKCJA DO CZYTANIA !!!!!! Z rzucaniem błędow
    //  wysłanie loginu
    send(client, login, strlen(login), 0);
    // odbior info zwrotego i potwierdzenie okejności
    if (read(client, answer, 1024) < 0) {
        cout << "Cos poszlo nie tak..."
             << "\n";
        close(client);
        return -1;
    }
    if (strcmp("OK", answer) == 0) {
        cout << answer << "\n";
    } else {
        close(client);
        return -1;
    }

    // wysłanie hasła i potwierdzenie
    send(client, password, strlen(password), 0);
    if (read(client, answer, 1024) < 0) {
        cout << "Cos poszlo nie tak..."
             << "\n";
        close(client);
        return -1;
    }
    if (strcmp("OK", answer) == 0) {
        cout << answer << "\n";
    } else {
        close(client);
        return -1;
    }

    // sprawdzenie czy zalogowano
    if (read(client, answer, 1024) < 0) {
        cout << "Cos poszlo nie tak..."
             << "\n";
        close(client);
        return -1;
    }
    if (strcmp("OK", answer) == 0) {
        cout << "Zalogowano."
             << "\n";
    } else if (strcmp("ERROR_USER", answer)) {
        cout << "Logowanie nie powiodło się - błędne dane."
             << "\n";
        close(client);
        return -1;
    }

    //  zakończenie połączenia
    close(client);
    return 0;
}