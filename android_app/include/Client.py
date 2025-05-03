import requests
import json
from include.Logger import Logger

class Client:
    def __init__(self, url: str):
        self.url = url
        self.logger = Logger('Client')

    def GetConsumers(self):
        response = requests.get(self.url + "/consumer", verify='/home/parrot/Second-Sem-Project/android_app/include/server.crt')
        self.logger.log(f"Status: {response.status_code}")
        self.logger.log(f"Headers: {response.headers}")
        try:
            data = response.json()
        except Exception as e:
            self.logger.log_error(f"JSON decode error: {e}")
            self.logger.log_error(f"Raw text: {response.text}")
            # Try manual parsing
            try:
                data = json.loads(response.text)
            except Exception as e2:
                self.logger.log_error(f"Manual JSON decode error: {e2}")
                data = None
        return data

    def GetConsumer(self, id: str):
        response = requests.get(self.url + "/consumer?id=" + id, verify='/home/parrot/Second-Sem-Project/android_app/include/server.crt')
        try:
            return response.json()
        except Exception as e:
            self.logger.log_error(f"JSON decode error: {e}")
            self.logger.log_error(f"Raw text: {response.text}")
            return None

    def GetBill(self, id: str, billing_date: str):
        response = requests.get(self.url + "/bill?id=" + id + "&date=" + billing_date, verify='/home/parrot/Second-Sem-Project/android_app/include/server.crt')
        try:
            return response.json()
        except Exception as e:
            self.logger.log_error(f"JSON decode error: {e}")
            self.logger.log_error(f"Raw text: {response.text}")
            return None

    def GetRevenue(self):
        response = requests.get(self.url + "/revenue", verify='/home/parrot/Second-Sem-Project/android_app/include/server.crt')
        try:
            return response.json()
        except Exception as e:
            self.logger.log_error(f"JSON decode error: {e}")
            self.logger.log_error(f"Raw text: {response.text}")
            return None

    def AddConsumer(self, consumer: dict):
        response = requests.post(self.url + "/consumer", json=consumer, verify='/home/parrot/Second-Sem-Project/android_app/include/server.crt')
        try:
            return response.json()
        except Exception as e:
            self.logger.log_error(f"JSON decode error: {e}")
            self.logger.log_error(f"Raw text: {response.text}")
            return None
    
    def RemoveConsumer(self, id: str):
        response = requests.delete(self.url + "/consumer?id=" + id, verify='/home/parrot/Second-Sem-Project/android_app/include/server.crt')
        try:
            return response.json()
        except Exception as e:
            self.logger.log_error(f"JSON decode error: {e}")
            self.logger.log_error(f"Raw text: {response.text}")
            return None