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

- Step 2: Install MySQL
```
```
- Step 2: start mysql service
```
sudo systemctl start mysql
sudo systemctl enable mysql
```
- Step 3: secure the installation
```
sudo mysql_secure_installation
```
Follow the prompts to set a root password and remove insecure defaults.

- Step 4: login to mysql
```
sudo mysql -u root -p
```
- Step 5: create a new user
```

