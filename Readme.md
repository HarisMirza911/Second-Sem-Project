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
sudo apt install libmysqlcppconn-dev
sudo apt install libimgui-dev
sudo apt install python3
sudo apt install python3-pip
pip3 install mysql-connector-python
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
- Step 8: Fill the Env.sh file with your own credentials

- Step 9: create a new database
use the script in scripts/CreateDB.py to create a new database and tables
use the script in scripts/PopulateDB.py to populate the database with mock data for 20 consumers

- Step 10: configure the config file in conf directory according to your needs

- Step 11: make the program
```
make
```

- Step 12: run the program
```
./bin/electrical_management_system --config conf/settings.conf
```
