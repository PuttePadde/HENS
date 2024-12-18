import sqlite3

def create_database():
    conn = sqlite3.connect("eggs.db")

    cursor = conn.cursor()

    create_table_query = """
    CREATE TABLE IF NOT EXISTS eggs (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        date_time TEXT NOT NULL,
        hen_weight REAL,
        egg_weight REAL,
        food_quantity REAL,
        light_on REAL,
        light_hours REAL,
	lay_time REAL
    );
    """
    
    cursor.execute(create_table_query)

    conn.commit()
    conn.close()

    print("Database and table 'eggs' created.")

if __name__ == "__main__":
    create_database()