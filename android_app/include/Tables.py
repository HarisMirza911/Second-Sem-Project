from kivy.uix.label import Label
from kivy.uix.gridlayout import GridLayout
from kivy.core.text import LabelBase

LabelBase.register(name='Times New Roman', 
                  fn_regular='/home/parrot/Second-Sem-Project/android_app/include/times-new-roman.ttf')

class ConsumersTable(GridLayout):
    def __init__(self, consumers, **kwargs):
        super(ConsumersTable, self).__init__(**kwargs)
        self.cols = 4
        self.spacing = [5, 5]
        self.padding = [10, 10]
        self.size_hint_y = None
        
        # Calculate height based on number of rows
        row_height = 40
        num_rows = len(consumers) + 1  # +1 for header row
        self.height = row_height * num_rows
        
        # Add headers
        self.add_widget(Label(text='ID', bold=True, size_hint_y=None, height=row_height))
        self.add_widget(Label(text='Name', bold=True, size_hint_y=None, height=row_height))
        self.add_widget(Label(text='Address', bold=True, size_hint_y=None, height=row_height))
        self.add_widget(Label(text='Billing Dates', bold=True, size_hint_y=None, height=row_height))
        # [{'id': 1, 'name': 'User_1425', 'address': 'Street #12, Gotham', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 2, 'name': 'User_5034', 'address': 'Street #84, Metropolis', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 3, 'name': 'User_6731', 'address': 'Street #42, Star City', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 4, 'name': 'User_2290', 'address': 'Street #67, Central City', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 5, 'name': 'User_7801', 'address': 'Street #31, Coast City', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 6, 'name': 'User_9044', 'address': 'Street #5, Blüdhaven', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 7, 'name': 'User_3339', 'address': 'Street #24, Hub City', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 8, 'name': 'User_1582', 'address': 'Street #48, Keystone City', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 9, 'name': 'User_4478', 'address': 'Street #75, Smallville', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 10, 'name': 'User_6287', 'address': 'Street #16, Midway City', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 11, 'name': 'User_3145', 'address': 'Street #8, National City', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 12, 'name': 'User_7823', 'address': 'Street #91, Central City', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 13, 'name': 'User_2667', 'address': 'Street #33, Freeland', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 14, 'name': 'User_4893', 'address': 'Street #59, Ivy Town', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 15, 'name': 'User_9901', 'address': 'Street #19, Atlantis', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 16, 'name': 'User_4755', 'address': 'Street #77, National City', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 17, 'name': 'User_6104', 'address': 'Street #6, Gotham', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 18, 'name': 'User_8321', 'address': 'Street #28, Starling City', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 19, 'name': 'User_3754', 'address': 'Street #3, Central City', 'billing_dates': ['2025-01-01', '2025-02-01']}, {'id': 20, 'name': 'User_2419', 'address': 'Street #90, Coast City', 'billing_dates': ['2025-01-01', '2025-02-01']}]
        
        # Add data rows
        if isinstance(consumers, list):
            for consumer in consumers:
                self.add_widget(Label(text=str(consumer.get('id', '')), size_hint_y=None, height=row_height))
                self.add_widget(Label(text=str(consumer.get('name', '')), size_hint_y=None, height=row_height))
                self.add_widget(Label(text=str(consumer.get('address', '')), size_hint_y=None, height=row_height))
                self.add_widget(Label(text=str(consumer.get('billing_dates', '')), size_hint_y=None, height=row_height))
        elif isinstance(consumers, dict):
            self.add_widget(Label(text=str(consumers.get('id', '')), size_hint_y=None, height=row_height))
            self.add_widget(Label(text=str(consumers.get('name', '')), size_hint_y=None, height=row_height))
            self.add_widget(Label(text=str(consumers.get('address', '')), size_hint_y=None, height=row_height))
            self.add_widget(Label(text=str(consumers.get('billing_dates', '')), size_hint_y=None, height=row_height))

# {'message': 'Bill details', 'id': '1', 'name': 'User_1425', 'address': 'Street #12, Gotham', 'date': '2025-01-01', 'bill': 875320, 'unitsUsed': 87532}
class BillTable(GridLayout):
    def __init__(self, bill, **kwargs):
        super(BillTable, self).__init__(**kwargs)
        self.cols = 6
        self.spacing = [5, 5]
        self.padding = [10, 10]
        self.size_hint_y = None
        row_height = 40
        num_rows = len(bill) + 1  # +1 for header row
        self.height = row_height * num_rows
        
        # Add headers
        self.add_widget(Label(text='ID', bold=True, size_hint_y=None, height=row_height))
        self.add_widget(Label(text='Name', bold=True, size_hint_y=None, height=row_height))
        self.add_widget(Label(text='Address', bold=True, size_hint_y=None, height=row_height))
        self.add_widget(Label(text='Date', bold=True, size_hint_y=None, height=row_height))
        self.add_widget(Label(text='Bill', bold=True, size_hint_y=None, height=row_height))
        self.add_widget(Label(text='Units Used', bold=True, size_hint_y=None, height=row_height))

        # Add data rows
        self.add_widget(Label(text=str(bill.get('id', '')), size_hint_y=None, height=row_height))
        self.add_widget(Label(text=str(bill.get('name', '')), size_hint_y=None, height=row_height))
        self.add_widget(Label(text=str(bill.get('address', '')), size_hint_y=None, height=row_height))
        self.add_widget(Label(text=str(bill.get('date', '')), size_hint_y=None, height=row_height))
        self.add_widget(Label(text=str(bill.get('bill', '')), size_hint_y=None, height=row_height))
        self.add_widget(Label(text=str(bill.get('unitsUsed', '')), size_hint_y=None, height=row_height))
