# Main GOAL
- Create a simple electrical management system
- Add Consumer Record
- View All Consumers
- Search Consumer By ID
- Calculate Bill Amount
- Delete Consumer Record
- Total Revenue
- Save and load data

# Requirements
- MySQL
- Boost
- RapidJSON
- C++17

# Commands

- Step 1: Install Packages
```
sudo apt update
sudo apt install libboost-all-dev
sudo apt install rapidjson-dev
sudo apt install default-mysql-server
```

- Step 2: enable and start mysql service
```
sudo systemctl enable mysql
sudo systemctl start mysql
```
- Step 3: secure the installation (optional but recommended)
```
sudo mysql_secure_installation
```
Follow the prompts to set a root password and remove insecure defaults.

- Step 4: login to mysql
```
sudo mysql -u root -p
```

- Next steps are optional but recommended
- Step 5: create a new user
```
CREATE USER 'newuser'@'localhost' IDENTIFIED BY 'password';
```

- Step 6: grant privileges to the new user
```
GRANT ALL PRIVILEGES ON * . * TO 'newuser'@'localhost';
```

- Step 7: flush privileges
```
FLUSH PRIVILEGES;
```

