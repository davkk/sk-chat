# pyright: reportUnknownMemberType=false
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
            unread_messages=[
                "This is a message from alice",
                "Hello from alice",
            ],
        )
    )
    users.insert(
        dict(
            login="alice",
            password="alice",
            ip="",
            port=0,
            is_logged_in=False,
            friends=["bob"],
            unread_messages=[],
        )
    )
