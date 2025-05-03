from kivy.uix.label import Label
from kivy.uix.gridlayout import GridLayout
from kivy.uix.textinput import TextInput
from kivy.uix.button import Button
from kivy.uix.scrollview import ScrollView
from kivy.uix.popup import Popup
from kivy.core.window import Window as KivyWindow
import include.Client as Client
from include.Tables import ConsumersTable, BillTable
from include.Logger import Logger

class MainScreen(GridLayout):
    client = Client.Client("https://localhost:6969")
    logger = Logger('MainScreen')
    def __init__(self, **kwargs):
        super(MainScreen, self).__init__(**kwargs)
        # Add main heading
        self.heading = Label(
            text='Welcome to EMS App',
            font_size=48,
            bold=True,
            size_hint_y=None,
            height=60,
            font_name='Times New Roman'
        )
        self.add_widget(self.heading)
        self.cols = 1
        
        # Create a layout with spacing between buttons
        self.layout = GridLayout(cols=1, spacing=(0, 25), padding=(0, 25))
        
        # Create buttons with consistent size
        get_consumers_btn = Button(text='Get Consumers', on_press=self.get_consumers, 
                                  size_hint_y=None, height=50, 
                                  size_hint_x=None, width=200,
                                  font_name='Times New Roman')
        get_consumer_btn = Button(text='Get Consumer', on_press=self.get_consumer, 
                                 size_hint_y=None, height=50, 
                                 size_hint_x=None, width=200,
                                 font_name='Times New Roman')
        get_bill_btn = Button(text='Get Bill', on_press=self.get_bill, 
                             size_hint_y=None, height=50, 
                             size_hint_x=None, width=200,
                             font_name='Times New Roman')
        
        get_revenue_btn = Button(text='Get Revenue', on_press=self.get_revenue,
                                 size_hint_y=None, height=50, 
                                 size_hint_x=None, width=200,
                                 font_name='Times New Roman')
        
        add_consumer_btn = Button(text='Add Consumer', on_press=self.add_consumer,
                                 size_hint_y=None, height=50, 
                                 size_hint_x=None, width=200,
                                 font_name='Times New Roman')
        
        remove_consumer_btn = Button(text='Remove Consumer', on_press=self.remove_consumer,
                                 size_hint_y=None, height=50, 
                                 size_hint_x=None, width=200,
                                 font_name='Times New Roman')
        
        
        # Center align the buttons
        self.layout.bind(minimum_width=self.layout.setter('width'))
        
        # Add buttons to layout with center alignment
        for btn in [get_consumers_btn, get_consumer_btn, get_bill_btn, get_revenue_btn, add_consumer_btn, remove_consumer_btn]:
            btn_container = GridLayout(cols=3, size_hint_y=None, height=50)
            btn_container.add_widget(Label(size_hint_x=1, font_name='Times New Roman'))  # Left spacer
            btn_container.add_widget(btn)                   # Centered button
            btn_container.add_widget(Label(size_hint_x=1, font_name='Times New Roman'))  # Right spacer
            self.layout.add_widget(btn_container)
        
        # Add the layout to the main screen
        self.add_widget(self.layout)


    def get_consumers(self, instance):
        consumers = self.client.GetConsumers()
        consumers.sort(key=lambda x: x['id'])
        self.logger.log(f"Consumers: {consumers}")
        
        if not consumers:
            error_msg = 'No consumers found or error occurred'
            self.logger.log_error(error_msg)
            popup = Popup(title='Error', 
                         content=Label(text=error_msg, font_name='Times New Roman'),
                         size_hint=(0.8, 0.3),
                         title_font='Times New Roman')
            popup.open()
            return
            
        # Create a scrollable table view
        table = ConsumersTable(consumers)
        
        # Create a scroll view with both horizontal and vertical scrolling
        scroll_view = ScrollView(size_hint=(1, 1), do_scroll_x=True, do_scroll_y=True, bar_width=10)
        scroll_view.add_widget(table)
        
        # Create and open popup with the scroll view
        popup = Popup(title='Consumers List', 
                     content=scroll_view,
                     size_hint=(0.9, 0.9),
                     title_font='Times New Roman')
        popup.open()
    
    def get_consumer(self, instance):
        self.consumer_id = TextInput(text='Enter consumer ID', multiline=False, font_name='Times New Roman',
                                    size_hint_y=None, height=50, size_hint_x=None, width=200)
        button = Button(text='Get Consumer', on_press=self.get_consumer_by_id, font_name='Times New Roman',
                       size_hint_y=None, height=50, size_hint_x=None, width=200)

        layout = GridLayout(cols=1, spacing=(0, 10))
        
        # Center the text input
        input_container = GridLayout(cols=3, size_hint_y=None, height=50)
        input_container.add_widget(Label(size_hint_x=1))  # Left spacer
        input_container.add_widget(self.consumer_id)      # Centered input
        input_container.add_widget(Label(size_hint_x=1))  # Right spacer
        layout.add_widget(input_container)
        
        # Center the button
        btn_container = GridLayout(cols=3, size_hint_y=None, height=50)
        btn_container.add_widget(Label(size_hint_x=1))  # Left spacer
        btn_container.add_widget(button)                # Centered button
        btn_container.add_widget(Label(size_hint_x=1))  # Right spacer
        layout.add_widget(btn_container)

        popup = Popup(title='Get Consumer', content=layout, size_hint=(0.9, 0.9), title_font='Times New Roman')
        popup.open()
        
        # consumer = self.client.GetConsumer(self.consumer_id.text)
        # print(consumer)

    def get_consumer_by_id(self, instance):
        consumer = self.client.GetConsumer(self.consumer_id.text)

        if not consumer:
            error_msg = 'No consumer found or error occurred'
            self.logger.log_error(f"{error_msg} for ID: {self.consumer_id.text}")
            popup = Popup(title='Error', 
                         content=Label(text=error_msg, font_name='Times New Roman'),
                         size_hint=(0.8, 0.3),
                         title_font='Times New Roman')
            popup.open()
            return
            
        # Create a scrollable table view
        table = ConsumersTable(consumer)

        # Create a scroll view with both horizontal and vertical scrolling
        scroll_view = ScrollView(size_hint=(1, 1), do_scroll_x=True, do_scroll_y=True, bar_width=10)
        scroll_view.add_widget(table)
        
        # Create and open popup with the scroll view
        popup = Popup(title='Consumer', 
                     content=scroll_view,
                     size_hint=(0.9, 0.9),
                     title_font='Times New Roman')
        popup.open()
        self.logger.log(f"Consumer: {consumer}")

    def get_bill(self, instance):
        self.consumer_id = TextInput(text='Enter Consumer ID', multiline=False, font_name='Times New Roman',
                                    size_hint_y=None, height=50, size_hint_x=None, width=200)
        self.billing_date = TextInput(text='Enter Billing Date', multiline=False, font_name='Times New Roman',
                                     size_hint_y=None, height=50, size_hint_x=None, width=200)
        button = Button(text='Get Bill', on_press=self.get_bill_by_id, font_name='Times New Roman',
                       size_hint_y=None, height=50, size_hint_x=None, width=200)

        layout = GridLayout(cols=1, spacing=(0, 10))
        
        # Center the first text input
        input1_container = GridLayout(cols=3, size_hint_y=None, height=50)
        input1_container.add_widget(Label(size_hint_x=1))  # Left spacer
        input1_container.add_widget(self.consumer_id)      # Centered input
        input1_container.add_widget(Label(size_hint_x=1))  # Right spacer
        layout.add_widget(input1_container)
        
        # Center the second text input
        input2_container = GridLayout(cols=3, size_hint_y=None, height=50)
        input2_container.add_widget(Label(size_hint_x=1))  # Left spacer
        input2_container.add_widget(self.billing_date)     # Centered input
        input2_container.add_widget(Label(size_hint_x=1))  # Right spacer
        layout.add_widget(input2_container)
        
        # Center the button
        btn_container = GridLayout(cols=3, size_hint_y=None, height=50)
        btn_container.add_widget(Label(size_hint_x=1))  # Left spacer
        btn_container.add_widget(button)                # Centered button
        btn_container.add_widget(Label(size_hint_x=1))  # Right spacer
        layout.add_widget(btn_container)

        popup = Popup(title='Get Bill', content=layout, size_hint=(0.9, 0.9), title_font='Times New Roman')
        popup.open()

    def get_bill_by_id(self, instance):
        bill = self.client.GetBill(self.consumer_id.text, self.billing_date.text)
        self.logger.log(f"Bill: {bill}")
        # layout = GridLayout(cols=1)
        # layout.add_widget(self.bill_id)
        # layout.add_widget(button)

        if not bill:
            error_msg = 'No bill found or error occurred'
            self.logger.log_error(f"{error_msg} for ID: {self.consumer_id.text}, Date: {self.billing_date.text}")
            popup = Popup(title='Error', 
                         content=Label(text=error_msg, font_name='Times New Roman'),
                         size_hint=(0.8, 0.3),
                         title_font='Times New Roman')
            popup.open()
            return
            
        # Create a scrollable table view
        table = BillTable(bill)

        # Create a scroll view with both horizontal and vertical scrolling
        scroll_view = ScrollView(size_hint=(1, 1), do_scroll_x=True, do_scroll_y=True, bar_width=10)
        scroll_view.add_widget(table)
        
        # Create and open popup with the scroll view
        popup = Popup(title='Bill', 
                     content=scroll_view,
                     size_hint=(0.9, 0.9),
                     title_font='Times New Roman')
        popup.open()

    def get_revenue(self, instance):
        revenue = self.client.GetRevenue()
        self.logger.log(f"Revenue: {revenue}")

        if not revenue:
            error_msg = 'No revenue found or error occurred'
            self.logger.log_error(error_msg)
            popup = Popup(title='Error', 
                         content=Label(text=error_msg, font_name='Times New Roman'),
                         size_hint=(0.8, 0.3),
                         title_font='Times New Roman')
            popup.open()
            return

        text = Label(text='Revenue: ' + str(revenue['revenue']) + '$', font_name='Times New Roman')
        popup = Popup(title='Revenue Details', content=text, size_hint=(0.9, 0.9), title_font='Times New Roman')
        popup.open()


# {
#     "name": "John Doe",
#     "id": "1234567890",
#     "address": "123 Main St, Anytown, USA",
#     "unitsUsed": 100,
#     "billingDate": "2021-01-01",
# }
    def add_consumer(self, instance):
        self.consumer_name = TextInput(text='Enter Consumer Name', multiline=False, font_name='Times New Roman',
                                      size_hint_y=None, height=50, size_hint_x=None, width=200)
        self.consumer_id = TextInput(text='Enter Consumer ID', multiline=False, font_name='Times New Roman',
                                    size_hint_y=None, height=50, size_hint_x=None, width=200)
        self.consumer_address = TextInput(text='Enter Consumer Address', multiline=False, font_name='Times New Roman',
                                         size_hint_y=None, height=50, size_hint_x=None, width=200)
        self.consumer_unitsUsed = TextInput(text='Enter Consumer Units Used', multiline=False, font_name='Times New Roman',
                                           size_hint_y=None, height=50, size_hint_x=None, width=200)
        self.consumer_billingDate = TextInput(text='Enter Consumer Billing Date', multiline=False, font_name='Times New Roman',
                                             size_hint_y=None, height=50, size_hint_x=None, width=200)
        button = Button(text='Add Consumer', on_press=self.send_add_consumer, font_name='Times New Roman',
                       size_hint_y=None, height=50, size_hint_x=None, width=200)

        layout = GridLayout(cols=1, spacing=(0, 10))
        
        # Center each text input and button
        for widget in [self.consumer_name, self.consumer_id, self.consumer_address, 
                      self.consumer_unitsUsed, self.consumer_billingDate, button]:
            container = GridLayout(cols=3, size_hint_y=None, height=50)
            container.add_widget(Label(size_hint_x=1))  # Left spacer
            container.add_widget(widget)                # Centered widget
            container.add_widget(Label(size_hint_x=1))  # Right spacer
            layout.add_widget(container)

        popup = Popup(title='Add Consumer', content=layout, size_hint=(0.9, 0.9), title_font='Times New Roman')
        popup.open()

    def send_add_consumer(self, instance):
        consumer = {
            "name": self.consumer_name.text,
            "id": int(self.consumer_id.text),
            "address": self.consumer_address.text,
            "unitsUsed": int(self.consumer_unitsUsed.text),
            "billingDate": self.consumer_billingDate.text
        }
        response = self.client.AddConsumer(consumer)
        self.logger.log(f"Add consumer response: {response}")
        
        if not response:
            error_msg = 'Failed to add consumer'
            self.logger.log_error(f"{error_msg}: {consumer}")
            popup = Popup(title='Error', 
                         content=Label(text=error_msg, font_name='Times New Roman'),
                         size_hint=(0.8, 0.3),
                         title_font='Times New Roman')
            popup.open()
            return
            
        popup = Popup(title='Success', content=Label(text='Consumer added successfully', font_name='Times New Roman'), size_hint=(0.9, 0.9), title_font='Times New Roman')
        popup.open()

    def remove_consumer(self, instance):
        self.consumer_id = TextInput(text='Enter Consumer ID', multiline=False, font_name='Times New Roman',
                                    size_hint_y=None, height=50, size_hint_x=None, width=200)
        button = Button(text='Remove Consumer', on_press=self.send_remove_consumer, font_name='Times New Roman',
                       size_hint_y=None, height=50, size_hint_x=None, width=200)

        layout = GridLayout(cols=1, spacing=(0, 10))
        
        # Center the text input
        input_container = GridLayout(cols=3, size_hint_y=None, height=50)
        input_container.add_widget(Label(size_hint_x=1))  # Left spacer
        input_container.add_widget(self.consumer_id)      # Centered input
        input_container.add_widget(Label(size_hint_x=1))  # Right spacer
        layout.add_widget(input_container)
        
        # Center the button
        btn_container = GridLayout(cols=3, size_hint_y=None, height=50)
        btn_container.add_widget(Label(size_hint_x=1))  # Left spacer
        btn_container.add_widget(button)                # Centered button
        btn_container.add_widget(Label(size_hint_x=1))  # Right spacer
        layout.add_widget(btn_container)

        popup = Popup(title='Remove Consumer', content=layout, size_hint=(0.9, 0.9), title_font='Times New Roman')
        popup.open()
        
    def send_remove_consumer(self, instance):
        result = self.client.RemoveConsumer(self.consumer_id.text)
        if result:
            message = 'Consumer removed successfully'
            self.logger.log(f"{message} for ID: {self.consumer_id.text}")
        else:
            message = 'Failed to remove consumer'
            self.logger.log_error(f"{message} for ID: {self.consumer_id.text}")
            
        popup = Popup(title='Result', content=Label(text=message, font_name='Times New Roman'), 
                     size_hint=(0.8, 0.3), title_font='Times New Roman')
        popup.open()