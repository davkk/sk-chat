# Gawariet-Gawariet

## Czyli program pozwalający na chatowanie między użytkownikami.

System składa się z dwóch elementów: serwera i klienta.

Każdy użytkownik ma swój login oraz hasło. Klient by wysłać wiadomość do innego użytkownika musi:

- Połączyć się do serwera wysyłając login i hasło.
- Serwer (oczywiście!) sprawdza to hasło
- Wysłać pakiet danych zawierający login użytkownika do którego chce wysłać wiadomość oraz treść wiadomości.
- Serwer wysyła teraz wiadomość to drugiego użytkownika (który może odpowiedzieć) jeśli drugi użytkownik nie jest zalogowany pierwszy dostaje informację o tym

Dodatkowo:

- Istnieje funkcjonalność znajomych by Adam mógł rozmawiać z Beatą, najpierw muszą stać się znajomymi.
- Klient może pobrać listę swoich znajomych oraz informacje o tym czy są aktualnie zalogowani z serwera.

---

## Baza danych (JSON)

```javascript
users: [
    user: {
        login: string,
        password: string,
        loggedIn: boolean,
        friends: string[],
        messages: [
            {
                sender: string,
                wasRead: boolean,
                content: string,
            },
        ],
    },
]
```

## Serwer (Python)

- odbiór loginu i hasła
- sprawdzenie poprawności
- odebranie wiadomości, wpisanie do bazy danych
- sprawdzenie czy odbiorca istnieje, jest zalogowany, nadawca ma odbiorcę w znajomych
- przekazanie wiadomości

## Klient (C/C++)

- wysłanie loginu i hasła
- wysłanie nazwy odbiorcy i wiadomości
- odbiór informacji od serwera
- komenda dodawania do znajomych
- wyświetlenie nieprzeczytanych wiadomości, oznaczenie ich jako przeczytane w bazie danych
- wyświetlenie listy znajomych + ich statusy
