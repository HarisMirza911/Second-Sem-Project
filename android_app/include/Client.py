import requests
import json
from include.Logger import Logger

class Client:
    def __init__(self, url: str, cert = '/home/parrot/Second-Sem-Project/android_app/include/server.crt'):
        self.cert = cert
        self.url = url
        self.logger = Logger('Client')

    def GetConsumers(self):
        response = requests.get(self.url + "/consumer", verify=self.cert)
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
        response = requests.get(self.url + "/consumer?id=" + id, verify=self.cert)
        try:
            return response.json()
        except Exception as e:
            self.logger.log_error(f"JSON decode error: {e}")
            self.logger.log_error(f"Raw text: {response.text}")
            return None

    def GetBill(self, id: str, billing_date: str):
        response = requests.get(self.url + "/bill?id=" + id + "&date=" + billing_date, verify=self.cert)
        try:
            return response.json()
        except Exception as e:
            self.logger.log_error(f"JSON decode error: {e}")
            self.logger.log_error(f"Raw text: {response.text}")
            return None

    def GetRevenue(self):
        response = requests.get(self.url + "/revenue", verify=self.cert)
        try:
            return response.json()
        except Exception as e:
            self.logger.log_error(f"JSON decode error: {e}")
            self.logger.log_error(f"Raw text: {response.text}")
            return None

    def AddConsumer(self, consumer: dict):
        response = requests.post(self.url + "/consumer", json=consumer, verify=self.cert)
        try:
            return response.json()
        except Exception as e:
            self.logger.log_error(f"JSON decode error: {e}")
            self.logger.log_error(f"Raw text: {response.text}")
            return None
    
    def RemoveConsumer(self, id: str):
        response = requests.delete(self.url + "/consumer?id=" + id, verify=self.cert)
        try:
            return response.json()
        except Exception as e:
            self.logger.log_error(f"JSON decode error: {e}")
            self.logger.log_error(f"Raw text: {response.text}")
            return None