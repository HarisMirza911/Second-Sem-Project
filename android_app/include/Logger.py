import logging
import os
from datetime import datetime
from logging.handlers import RotatingFileHandler

# Create log directory if it doesn't exist
os.makedirs('log', exist_ok=True)

class DailyRotatingFileHandler(RotatingFileHandler):
    def __init__(self, filename_pattern, *args, **kwargs):
        self.filename_pattern = filename_pattern
        self.current_date = datetime.now().strftime('%Y-%m-%d')
        filename = self.filename_pattern.format(date=self.current_date)
        super().__init__(filename, *args, **kwargs)

    def emit(self, record):
        current_date = datetime.now().strftime('%Y-%m-%d')
        if current_date != self.current_date:
            self.current_date = current_date
            self.baseFilename = self.filename_pattern.format(date=self.current_date)
            if self.stream:
                self.stream.close()
                self.stream = None
        super().emit(record)

class Logger:
    def __init__(self, name="app"):
        self.name = name
        self.logger = logging.getLogger(name)
        self.logger.setLevel(logging.INFO)
        
        # Create formatters
        formatter = logging.Formatter('%(asctime)s - %(name)s - %(levelname)s - %(message)s')
        
        # File handler with daily rotation and size-based rotation
        log_filename = f'log/{name}-{{date}}.logs'
        self.file_handler = DailyRotatingFileHandler(log_filename, maxBytes=10*1024*1024, backupCount=5)
        self.file_handler.setFormatter(formatter)
        
        # Console handler
        console_handler = logging.StreamHandler()
        console_handler.setFormatter(formatter)
        
        # Add handlers to logger
        self.logger.addHandler(self.file_handler)
        self.logger.addHandler(console_handler)

    def check_rotation(self):
        if self.file_handler:
            current_date = datetime.now().strftime('%Y-%m-%d')
            if current_date != self.file_handler.current_date:
                self.file_handler.current_date = current_date
                self.file_handler.baseFilename = self.file_handler.filename_pattern.format(date=current_date)
                if self.file_handler.stream:
                    self.file_handler.stream.close()
                    self.file_handler.stream = None

    def log(self, message):
        self.check_rotation()
        self.logger.info(message)

    def log_error(self, message):
        self.check_rotation()
        self.logger.error(message)

    def log_warning(self, message):
        self.check_rotation()
        self.logger.warning(message)
