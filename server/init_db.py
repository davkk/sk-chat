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
            port="",
            friends=["alice"],
            unreadMessages=[],
        )
    )
    users.insert(
        dict(
            login="alice",
            password="alice",
            ip="",
            port="",
            friends=["bob"],
            unreadMessages=[],
        )
    )
