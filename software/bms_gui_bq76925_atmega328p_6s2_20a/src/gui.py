import customtkinter

customtkinter.set_appearance_mode("dark")


cellVoltage=[]
cellBalance=[]

#-----------------------------------------------------------------------------------------------------Cell Voltage Frame
class cellVoltageFrame(customtkinter.CTkFrame):
    def __init__(self,master,number_of_cells,height):
        super().__init__(master)
        for i in range(number_of_cells):
            self.grid_columnconfigure(i, weight=1)
            self.cellvoltage = customtkinter.CTkProgressBar(
                self,
                width=20,
                height=height,
                orientation="vertical",
                corner_radius=0
            )
            self.cellvoltage.grid(
                row=0,
                column=i,
                padx=0,
                pady=0
            )
            cellVoltage.append(self.cellvoltage)
#-----------------------------------------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------------------Cell Balancing Frame
class cellBalancingFrame(customtkinter.CTkFrame):
    def __init__(self,master,number_of_cells):
        super().__init__(master)
        for i in range(number_of_cells):
            self.grid_columnconfigure(i,weight=1)
            self.cellBalance = customtkinter.CTkButton(self,width=20,height=20,text="")
            self.cellBalance.grid(row=0,column=i,padx=0,pady=0,sticky="n")
            cellBalance.append(self.cellBalance)
#-----------------------------------------------------------------------------------------------------------------------

#-----------------------------------------------------------------------------------------------------Cell Balancing Frame
class cellLabel(customtkinter.CTkLabel):
    def __init__(self,master,number_of_cell):
        super().__init__(master,text="")
        for i in range(number_of_cell):
            self.grid_columnconfigure(i,weight=1)
            self.label = customtkinter.CTkLabel(self,text="CB"+str(i))
            self.label.grid(row=0,column=i,padx=0,pady=0)
#-----------------------------------------------------------------------------------------------------------------------






class app(customtkinter.CTk):
    def __init__(self):
        super().__init__()

        self.title("bms gui")
        self.geometry("400x150")
        # self.configure(borderwidth=5, relief="solid")
        for i in range(6):
            self.grid_columnconfigure(i, weight=1)


        self.cellBalancingFrame = cellBalancingFrame(self,7)
        self.cellBalancingFrame.grid(row=0,column=0,rowspan=1,columnspan=7,sticky='ew')

        self.cellLabel = cellLabel(self,number_of_cell=7)
        self.cellLabel.grid(row=1,column=0,rowspan=1,columnspan=7,sticky="ew")

        self.cellvoltageframe = cellVoltageFrame(self,number_of_cells=7,height=300)
        self.cellvoltageframe.grid(row=2,column=0,rowspan=1,columnspan=7,sticky="ew")

root = app()
cellVoltage[4].set(0.9)
root.mainloop()
