import tkinter as tk
from tkinter import ttk
import paho.mqtt.client as mqtt
import random

#21.11.3844 -21.11.3847 - 21.11.3854
class DashboardApp:
    def __init__(self, master):
        self.master = master
        self.master.title("Dashboard Monitoring AMIKOM")
        self.master.geometry("400x200")

        self.lamp_status = tk.StringVar()
        self.temperature_value = tk.StringVar()
        self.humidity_value = tk.StringVar()

        # lampu
        ttk.Label(self.master, text="Status Lampu:").grid(row=0, column=0, padx=10, pady=10)
        ttk.Label(self.master, textvariable=self.lamp_status).grid(row=0, column=1, padx=10, pady=10)

        # suhu
        ttk.Label(self.master, text="Suhu:").grid(row=1, column=0, padx=10, pady=10)
        ttk.Label(self.master, textvariable=self.temperature_value).grid(row=1, column=1, padx=10, pady=10)

        # kelembapan
        ttk.Label(self.master, text="Kelembapan:").grid(row=2, column=0, padx=10, pady=10)
        ttk.Label(self.master, textvariable=self.humidity_value).grid(row=2, column=1, padx=10, pady=10)

        # Dummy
        ttk.Button(self.master, text="Update Data Dummy", command=self.update_dummy_data).grid(row=3, column=0, columnspan=2, pady=10)

        self.setup_mqtt()

    def on_connect(self, client, userdata, flags, rc):
        print("Connected with result code "+str(rc))
        client.subscribe("amikom/dashboard/lamp")
        client.subscribe("amikom/dashboard/temperature")
        client.subscribe("amikom/dashboard/humidity")

    def on_message(self, client, userdata, msg):
        try:
            payload = msg.payload.decode()
            print("Received message:", payload)

            if msg.topic == "amikom/dashboard/lamp":
                self.lamp_status.set(payload)
            elif msg.topic == "amikom/dashboard/temperature":
                self.temperature_value.set(payload)
            elif msg.topic == "amikom/dashboard/humidity":
                self.humidity_value.set(payload)

        except Exception as e:
            print("Error:", str(e))

    def setup_mqtt(self):
        self.client = mqtt.Client()
        self.client.on_connect = self.on_connect
        self.client.on_message = self.on_message


        self.client.connect("localhost", 1883, 60)


        self.client.loop_start()

    def update_dummy_data(self):

        new_temperature = round(random.uniform(20, 30), 2)
        new_humidity = round(random.uniform(40, 60), 2)

        self.client.publish("amikom/dashboard/temperature", str(new_temperature))
        self.client.publish("amikom/dashboard/humidity", str(new_humidity))

if __name__ == "__main__":
    root = tk.Tk()
    app = DashboardApp(root)
    root.mainloop()
