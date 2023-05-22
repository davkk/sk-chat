# pyright: reportUnknownMemberType=false
import time

from tinydb import TinyDB

with TinyDB("database.json") as db:
    db.truncate()
    db.drop_tables()

    users = db.table("users")
    users.insert(
        dict(
            login="david",
            password="david",
            ip="",
            port=0,
            is_logged_in=False,
            friends=["alice"],
        )
    )
    users.insert(
        dict(
            login="alice",
            password="alice",
            ip="",
            port=0,
            is_logged_in=False,
            friends=["david"],
        )
    )
    users.insert(
        dict(
            login="bob",
            password="bob",
            ip="",
            port=0,
            is_logged_in=False,
            friends=["david", "alice"],
        )
    )

    messages = db.table("messages")
    messages.insert(
        dict(
            time=str(time.time_ns()),
            sender="david",
            receiver="alice",
            message="Hello from david",
        )
    )
    messages.insert(
        dict(
            time=str(time.time_ns()),
            sender="alice",
            receiver="david",
            message="Hi from alice",
        )
    )
    messages.insert(
        dict(
            time=str(time.time_ns()),
            sender="alice",
            receiver="david",
            message="What's up?",
        )
    )
    messages.insert(
        dict(
            time=str(time.time_ns()),
            sender="david",
            receiver="alice",
            message="Not much",
        )
    )

    print("Database initialized")
    print(f"Users: {users.all()}")
    print(f"Messages: {messages.all()}")
