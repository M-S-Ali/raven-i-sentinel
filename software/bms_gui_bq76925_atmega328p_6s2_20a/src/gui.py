import customtkinter

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
        if 0 <= index < len(voltage_list):
            voltage_list[index].set(value)


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

        # Initial Data Updates
        self.update_data()

    def update_data(self):
        self.current_label.update_label("16 A")
        self.voltage_label.update_label("24 V")
        self.temp_label.update_label("26 °C")
        self.cell_voltage_bars[4].set(0.9)
        self.current_progress_bars[0].set(0.9)


# Run the app
if __name__ == "__main__":
    app = BMSApp()
    app.mainloop()
