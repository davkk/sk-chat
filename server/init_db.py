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
            is_logged_in=False,
            friends=["alice", "bob"],
        )
    )
    users.insert(
        dict(
            login="alice",
            password="alice",
            is_logged_in=False,
            friends=["david"],
        )
    )
    users.insert(
        dict(
            login="bob",
            password="bob",
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
            message="Hello what's up?",
        )
    )
    messages.insert(
        dict(
            time=str(time.time_ns()),
            sender="alice",
            receiver="david",
            message="Hi good how are you",
        )
    )
    messages.insert(
        dict(
            time=str(time.time_ns()),
            sender="bob",
            receiver="david",
            message="How are you doing mate? I'm fine",
        )
    )

    print("Database initialized")
    print(f"Users: {users.all()}")
    print(f"Messages: {messages.all()}")
