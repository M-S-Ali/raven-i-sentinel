import customtkinter
import serial
import threading
import json
import time

port = 'COM16'
baudrate = 9600

customtkinter.set_appearance_mode("light")


class MainLabelFrame(customtkinter.CTkFrame):
    def __init__(self, master, label_value=0, label_unit=""):
        super().__init__(master, fg_color="transparent")
        self.grid_columnconfigure(0, weight=1)
        self.label = customtkinter.CTkLabel(
            self,
            text=f"{label_value} {label_unit}",
            height=80,
            width=80,
            font=("Arial", 50),
        )
        self.label.grid(row=0, column=0)

    def update_label(self, value):
        self.label.configure(text=value)


class ProgressBarFrame(customtkinter.CTkFrame):
    def __init__(self, master, bar_list):
        super().__init__(master, fg_color="transparent")
        self.bar = customtkinter.CTkProgressBar(
            self,
            width=300,
            height=30,
            corner_radius=0,
        )
        self.bar.grid(row=0, column=0, padx=0, pady=0)
        bar_list.append(self.bar)

    def update_progress(self, value):
        self.bar.set(value)


class CellButtonFrame(customtkinter.CTkFrame):
    def __init__(self, master, button_list):
        super().__init__(master, fg_color="transparent")
        self.grid_columnconfigure(list(range(6)), weight=1)
        for i in range(6):
            button = customtkinter.CTkButton(self, width=20, height=20, text="")
            button.grid(row=0, column=i)
            button_list.append(button)

    def update_buttons(self, cb_values):
        # cb_values is list of 6 ints (0 or 1)
        for i, val in enumerate(cb_values):
            if val:
                self.master.cell_balance_buttons[i].configure(fg_color="green")
            else:
                self.master.cell_balance_buttons[i].configure(fg_color="red")


class CellLabelFrame(customtkinter.CTkFrame):
    def __init__(self, master):
        super().__init__(master, fg_color="transparent")
        self.grid_columnconfigure(list(range(6)), weight=1)
        for i in range(6):
            label = customtkinter.CTkLabel(self, text=f"CB{i}")
            label.grid(row=0, column=i)


class CellVoltageFrame(customtkinter.CTkFrame):
    def __init__(self, master, voltage_list):
        super().__init__(master, fg_color="transparent")
        self.grid_columnconfigure(list(range(6)), weight=1)
        for i in range(6):
            voltage_bar = customtkinter.CTkProgressBar(
                self,
                width=20,
                height=300,
                orientation="vertical",
                corner_radius=0,
            )
            voltage_bar.grid(row=0, column=i, padx=0, pady=0)
            voltage_list.append(voltage_bar)

    def update_voltage(self, index, value):
        if 0 <= index < len(self.master.cell_voltage_bars):
            self.master.cell_voltage_bars[index].set(value)


class BMSApp(customtkinter.CTk):
    def __init__(self):
        super().__init__()
        self.title("BMS GUI")
        self.geometry("800x600")

        # Layout configurations
        self.grid_columnconfigure(list(range(6)), weight=1)

        # Data lists
        self.cell_voltage_bars = []
        self.current_progress_bars = []
        self.cell_balance_buttons = []

        # Components
        self.current_label = MainLabelFrame(self, 0, "A")
        self.current_label.grid(row=0, column=0, columnspan=6)

        self.current_progress = ProgressBarFrame(self, self.current_progress_bars)
        self.current_progress.grid(row=1, column=0, columnspan=6)

        self.voltage_label = MainLabelFrame(self, 0, "V")
        self.voltage_label.grid(row=2, column=0, columnspan=3)

        self.temp_label = MainLabelFrame(self, 0, "°C")
        self.temp_label.grid(row=2, column=3, columnspan=3)

        self.cell_balance_frame = CellButtonFrame(self, self.cell_balance_buttons)
        self.cell_balance_frame.grid(row=3, column=0, columnspan=6, sticky="ew")

        self.cell_label_frame = CellLabelFrame(self)
        self.cell_label_frame.grid(row=4, column=0, columnspan=6, sticky="ew")

        self.cell_voltage_frame = CellVoltageFrame(self, self.cell_voltage_bars)
        self.cell_voltage_frame.grid(row=5, column=0, columnspan=6, sticky="ew")

        # Start serial reading thread
        self.serial_thread_running = True
        self.ser = None
        self.start_serial_thread()

    def start_serial_thread(self):
        try:
            self.ser = serial.Serial(port, baudrate, timeout=1)
            threading.Thread(target=self.read_serial, daemon=True).start()
        except Exception as e:
            print("Error opening serial port:", e)

    def read_serial(self):
        buffer = ""
        while self.serial_thread_running:
            try:
                if self.ser.in_waiting:
                    line = self.ser.readline().decode('utf-8').strip()
                    if not line:
                        continue
                    # Parse JSON
                    try:
                        data = json.loads(line)
                        self.after(0, self.update_gui_data, data)
                    except Exception as e:
                        print("JSON parse error:", e, line)
                else:
                    time.sleep(0.05)
            except Exception as e:
                print("Serial read error:", e)
                time.sleep(1)

    def update_gui_data(self, data):
        # Update based on keys
        if "cv" in data:
            cv_values = data["cv"]  # list of 6 voltages (assumed)
            # Assuming cell voltage range 0-5V, normalize for progress bars 0.0-1.0
            for i, val in enumerate(cv_values):
                if 0 <= i < len(self.cell_voltage_bars):
                    normalized_val = min(max(val / 380, 0), 1)  # example normalization
                    self.cell_voltage_bars[i].set(normalized_val)

        if "pv" in data and "pi" in data and "pt" in data:
            # pack voltage, current, temperature
            pv = data["pv"]
            pi = data["pi"]
            pt = data["pt"]

            # Update labels (add units)
            self.voltage_label.update_label(f"{pv/100} V")
            self.current_label.update_label(f"{pi/100} A")
            self.temp_label.update_label(f"{pt/10} °C")

            # Normalize current for progress bar (example: max 50A)
            normalized_current = min(max(pi / 200, 0), 1)
            if self.current_progress_bars:
                self.current_progress_bars[0].set(normalized_current)

        if "cb" in data:
            cb_values = data["cb"]  # list of 6 ints (0 or 1)
            # Update cell balance buttons color
            for i, val in enumerate(cb_values):
                if 0 <= i < len(self.cell_balance_buttons):
                    color = "red" if val else "blue"
                    self.cell_balance_buttons[i].configure(fg_color=color)

    def on_closing(self):
        self.serial_thread_running = False
        if self.ser and self.ser.is_open:
            self.ser.close()
        self.destroy()


if __name__ == "__main__":
    app = BMSApp()
    app.protocol("WM_DELETE_WINDOW", app.on_closing)
    app.mainloop()
