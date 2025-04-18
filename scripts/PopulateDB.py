#! /usr/bin/env python3
import mysql.connector
import os

mydb = mysql.connector.connect(
    host=os.getenv("MYSQL_HOST"),
    user=os.getenv("MYSQL_USER"),
    password=os.getenv("MYSQL_PASSWORD")
)

mycursor = mydb.cursor()

mycursor.execute("USE " + os.getenv("MYSQL_DATABASE"))

# Insert data into Consumers table using executemany() for batch insertion
consumers_data = [
    (1, "User_1425", "Street #12, Gotham"),
    (2, "User_5034", "Street #84, Metropolis"),
]

mycursor.executemany("INSERT INTO Consumers (id, name, address) VALUES (%s, %s, %s)", consumers_data)

# Insert data into Billing table using executemany() for batch insertion
billing_data = [
    (1, "2025-01-01", 87532),
    (1, "2025-02-01", 89999),
    (2, "2025-01-01", 128347),
    (2, "2025-02-01", 132450)
]

mycursor.executemany("INSERT INTO Billing (consumer_id, billing_date, units_used) VALUES (%s, %s, %s)", billing_data)

# Commit the changes
mydb.commit()

# Query to display combined data
mycursor.execute("""
    SELECT c.id, c.name, c.address, b.billing_date, b.units_used 
    FROM Consumers c 
    JOIN Billing b ON c.id = b.consumer_id 
    ORDER BY c.id, b.billing_date
""")

for x in mycursor:
    print(x)