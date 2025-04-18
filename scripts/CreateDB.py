#!/usr/bin/env python3
import mysql.connector
import os

mydb = mysql.connector.connect(
    host=os.getenv("MYSQL_HOST"),
    user=os.getenv("MYSQL_USER"),
    password=os.getenv("MYSQL_PASSWORD")
)

mycursor = mydb.cursor()
"""
[
	{
		"name":"User_2667",
		"id":13,
		"address":"Street #33, Freeland",
		"data":[
			{ "Billing Date":"2025-01-01", "Units Used":127008 },
			{ "Billing Date":"2025-02-01", "Units Used":131888 }
		]
	},
	{
		"name":"User_7823",
		"id":12,
		"address":"Street #91, Central City",
		"data":[
			{ "Billing Date":"2025-01-01", "Units Used":103987 },
			{ "Billing Date":"2025-02-01", "Units Used":106444 }
		]
	}, ...
]
"""
mycursor.execute("CREATE DATABASE IF NOT EXISTS " + os.getenv("MYSQL_DATABASE"))

# Fetch results after SHOW DATABASES to clear the result set
mycursor.execute("SHOW DATABASES")
databases = mycursor.fetchall()  # Consume the result set

mycursor.execute("USE " + os.getenv("MYSQL_DATABASE"))

# Fetch results after SHOW TABLES to clear the result set
mycursor.execute("SHOW TABLES")
tables = mycursor.fetchall()  # Consume the result set

# Create Consumers table with the original ID from JSON
mycursor.execute("CREATE TABLE IF NOT EXISTS Consumers (id INT PRIMARY KEY, name VARCHAR(255), address VARCHAR(255))")

# Create Billing table with a composite primary key (consumer_id, billing_date)
mycursor.execute("CREATE TABLE IF NOT EXISTS Billing (consumer_id INT, billing_date DATE, units_used INT, PRIMARY KEY (consumer_id, billing_date), FOREIGN KEY (consumer_id) REFERENCES Consumers(id))")

mydb.commit()

mycursor.execute("SHOW TABLES")
tables = mycursor.fetchall()  # Consume the result set

print(tables)