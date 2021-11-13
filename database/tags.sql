CREATE TABLE relations(
    id INTEGER PRIMARY KEY AUTOINCREMENT,
    category CHAR(256) UNIQUE,
    parent INTEGER,
    FOREIGN KEY (parent) REFERENCES relations(id) 
    ON DELETE CASCADE
);
PRAGMA foreign_keys = ON;