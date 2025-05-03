# Main GOAL
- Create a simple electrical management system
- Add Consumer Record
- View All Consumers
- Search Consumer By ID
- Calculate Bill Amount
- Delete Consumer Record
- Total Revenue
- Save and load data

# Introduction
### EMS(Electrical Management System)
It is a simple implementation of electrical management system which uses rest api to communicate with other applications and support developed in C++
It Uses **LibBoost** A Fast C++ Library For Multipurpose Use Cases It contains Several Header Files For Different Purposes Such As ASIO, Network Communication
Json Parsing and much more This Implementation Uses SSL(Secure Socket Layer) Based Rest API that is mostly used in modern applications also known as **https**
Further It Uses **Rapid Json** For Fast Json Parsing and **My SQL Connector** For Database Operations

# Requirements
- MySQL
- Boost
- RapidJSON
- C++17
- Mysql Connector
- Python3
- Python3-pip
- Android-Adb

# General Required Packages
```
sudo apt update
sudo apt install libboost-all-dev
sudo apt install rapidjson-dev
sudo apt install default-mysql-server
sudo apt install libmysqlcppconn-dev
sudo apt install python3
sudo apt install python3-pip
sudo apt install android-tools-adb
pip3 install mysql-connector-python
```

# Commands For Server
- Step 1: enable and start mysql service
```
sudo systemctl enable mysql
sudo systemctl start mysql
```
- Step 2: secure the installation (optional but recommended)
```
sudo mysql_secure_installation
```
Follow the prompts to set a root password and remove insecure defaults.

- Step 3: login to mysql
```
sudo mysql -u root -p
```

- Next steps are optional but recommended
- Step 4: create a new user
```
CREATE USER 'newuser'@'localhost' IDENTIFIED BY 'password';
```

- Step 5: grant privileges to the new user
```
GRANT ALL PRIVILEGES ON * . * TO 'newuser'@'localhost';
```

- Step 6: flush privileges
```
FLUSH PRIVILEGES;
```
- Step 7: Fill the Env.sh file with your own credentials

- Step 8: create a new database
use the script in scripts/CreateDB.py to create a new database and tables
use the script in scripts/PopulateDB.py to populate the database with mock data for 20 consumers

- Step 9: configure the config file in conf directory according to your needs

- Step 10: make the program
```
make
```

- Step 11: run the program
```
./bin/electrical_management_system --config conf/settings.conf
```

# Commands To Build Android App

- If you don't want to deploy the app on actuall android you can simply run it on built-in Emulator in python that
is lightweight and based on old android version so it will be compatible with laptops without GPUs
- Here's the command how to do that but before that you need to activate virtual environment for python that is in

```
android_app/venv/bin/activate
```

- use following command to activate it 

```
android_app/venv/bin/activate
```

- Make Sure To Deactivate it after use with following command
```
deactivate
```

- After Enabling the virtual environment use command in root directory i:e android_app

```
python3 main.py
```

- this will run android app on built in emulator
but if you want to build and deploy app then 

use this app to build app

```
buildozer -v android debug
```
- Now on your android go to developer option in settings enable it by clicking 7 times on build number
- then find USB Debugging option and enable it then find option install via USB and enable it too
- Once Build is finished then deploy app on android using command

```
buildozer android deploy run
```