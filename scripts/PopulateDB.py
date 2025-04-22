#!/usr/bin/env python3
import mysql.connector
import os
import random
import string
from faker import Faker

# Initialize Faker for generating realistic names/addresses
fake = Faker()

# Connect to the database
mydb = mysql.connector.connect(
    host=os.getenv("MYSQL_HOST"),
    user=os.getenv("MYSQL_USER"),
    password=os.getenv("MYSQL_PASSWORD")
)

mycursor = mydb.cursor()
mycursor.execute("USE " + os.getenv("MYSQL_DATABASE"))

# Generate 50 consumers
consumers_data = []
for i in range(1, 21):
    name = f"User_{''.join(random.choices(string.digits, k=4))}"
    address = fake.address().replace('\n', ', ')
    consumers_data.append((i, name, address))

mycursor.execute("DELETE FROM Billing")
mycursor.execute("DELETE FROM Consumers")

# Insert consumers
mycursor.executemany("INSERT INTO Consumers (id, name, address) VALUES (%s, %s, %s)", consumers_data)

# Generate billing data for each user for Jan and Feb 2025
billing_data = []
for consumer_id in range(1, 21):
    jan_units = random.randint(80000, 150000)
    feb_units = jan_units + random.randint(1000, 5000)
    billing_data.append((consumer_id, "2025-01-01", jan_units))
    billing_data.append((consumer_id, "2025-02-01", feb_units))

# Insert billing records
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

# Clean up
mycursor.close()
mydb.close()
