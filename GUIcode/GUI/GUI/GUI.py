#------------------------------------------------------Grid for the gui 
# [buttons | 5 - LEDS  | Button for oppening the vm]
# [--------|---------------------------------------]
# [ON/OFF  | Slider d%   Insert temp, temp display ]

from typing import final
from Automat_structure import automat

from tkinter.tix import COLUMN
from CTkMessagebox import CTkMessagebox
import collections
from tkinter import ON, StringVar, Variable
from tkinter.ttk import Progressbar
from PIL import Image, ImageTk
import customtkinter as ctk
import tkinter as ttk
import serial
import threading 
import struct
import sys

#------------------------------------------------------Global variables
cash = 0
running = True
byte_data = b'\x00' * 99
flag = True
slot = []

#------------------------------------------------------UART
def uart_function():
    global running, ser
    ser = serial.Serial('COM7', 115200, timeout = 1)
    print(f"Connected to {ser.portstr}")
    data_to_send = struct.pack('BB', 4, 4)
    ser.write(data_to_send)
    print(data_to_send)
    try:
        while running:
            if ser.in_waiting > 0: # Return the number of bytes in the receive buffer.
                byte_data = ser.read(99) # Read size bytes from the serial port. If a timeout is set it may return fewer characters than requested. 
                                         # With no timeout it will block until the requested number of bytes is read.
                if len(byte_data) != 99:
                    print(f"Error: Received data length {len(byte_data)} does not match expected 99 bytes")
                    continue
                # Define the format string
                format_string = 'BB10sBBBB10sBBBB10sBBBB10sBBBB10sBBBB10sBBBB???bBbBBH??B'
                
                try:
                    # Unpack the data
                    unpacked_data = struct.unpack(format_string, byte_data)
                    # Update the Automat structure
                    automat['products_Number'] = unpacked_data[0]
                    automat['capacity'] = unpacked_data[31]
                    automat['door_opened'] = bool(unpacked_data[32])
                    automat['turned_on'] = bool(unpacked_data[33])
                    automat['stand_by'] = bool(unpacked_data[34])
                    automat['current_Temperature'] = unpacked_data[35]
                    automat['current_Humidity'] = unpacked_data[36]
                    automat['wanted_Temperature'] = unpacked_data[37]
                    automat['light_Intensity'] = unpacked_data[38]
                    automat['selected_Product'] = unpacked_data[39]
                    automat['current_Profit'] = unpacked_data[40]
                    automat['overheating'] = bool(unpacked_data[41])
                    automat['freezing'] = bool(unpacked_data[42])
                    automat['temperature_offset_tolerance'] = unpacked_data[43]

                    # Clear previous products and update with new data
                    automat['products'] = []
                    for i in range(6):
                        product_offset = 1 + i * 5 # Offset for each product struct
                        product = {
                            'slot_ID': unpacked_data[product_offset],
                            'name': unpacked_data[product_offset + 1].decode('utf-8').strip('\x00'),  # Decode name and strip null bytes
                            'price': unpacked_data[product_offset + 2],
                            'quantity': unpacked_data[product_offset + 3],
                            'capacity': unpacked_data[product_offset + 4],
                        }
                        automat['products'].append(product)
                except struct.error as e:
                    print(f"Struct unpacking error: {e}")
    except serial.SerialException as e:
        print(f"Serial Port Error: {e}")

def update_light_duty():
    light_duty_progressbar["value"] = float(automat['light_Intensity'])
    light_duty_value.configure(text = automat['light_Intensity'])
    window.after(10, update_light_duty)
    
def update_temperature():
    temperature_display["value"] = float(automat['current_Temperature'])
    temperature_label.configure(text = f"Temperature: {automat['current_Temperature']}C")
    window.after(10, update_temperature)
    
def update_wanted_temperature():
    print({automat['wanted_Temperature']})
    wanted_temperature_label.configure(text = f"Wanted temperature\n is: {int(user_temperature_input.get())}")
    
def update_humidity():
    humidity_display["value"] = float(automat['current_Humidity'])
    humidity_label.configure(text = f"Humidity: {automat['current_Humidity']}%")
    window.after(10, update_humidity)
    
#------------------------------------------------------PROFIT
def update_profit():
    profit_label.configure(text = f"${automat['current_Profit']}")
    window.after(10, update_profit)
    
#------------------------------------------------------LED DISPLAY    
def update_state_of_capacity():
    if automat['capacity'] >= 65:
        led_label.configure(image = arrey_of_led_images["green"])
        capacity_label.configure(text = "Vending Machine is FULL!")
    elif automat['capacity'] != 0:
        capacity_label.configure(text = "Vending Machine is PARTIALLY FULL!")
        led_label.configure(image = arrey_of_led_images["yellow"])
    else:
        capacity_label.configure(text = "Vending Machine is EMPTY!")
        led_label.configure(image = arrey_of_led_images["red"])
    window.after(10, update_state_of_capacity)
  
#------------------------------------------------------SLOTS 
def update_slots():   
    global flag
    while flag:
        for i in range(automat['products_Number']):
            button = ctk.CTkButton(master = slots_frame,
                                font = ('Arial', 16),
                                text = "",
                                width = 120, 
                                height = 35, 
                                state = "disabled", 
                                text_color_disabled = "white"
                                ) 
            button.grid(row = (i // 3) + 1, column = i % 3 , pady = 5, padx = 5)
            slot.append(button)
            flag = False

    for i in range(automat['products_Number']):
        original_name =  f"{automat['products'][i]['name']}"
        remove_asterisk = original_name.replace("*", "")
        slot[i].configure(text = f"{remove_asterisk} " 
                                 f"${automat['products'][i]['price']}\n"
                                 f"Quantity: {automat['products'][i]['quantity']}\n"
                                 f"Capacity: {automat['products'][i]['capacity']}" 
                                 )
    window.after(10, update_slots)

def update_in_off_mode():
        if on_off_switch_var.get() == 1:
            refill_vm_button.configure(state = 'normal')
            temperature_button.configure(state = 'normal')
        else:
            refill_vm_button.configure(state = 'disabled')
            temperature_button.configure(state = 'disabled')

#------------------------------------------------------TEMPERATURE
def send_temperature():
    try:
        temp_value = int(user_temperature_input.get())
        data_to_send = struct.pack('BB', 3, temp_value)
        ser.write(data_to_send)
        print(f"Sent temperature value: {temp_value}")
        print(data_to_send)
            
    except ValueError as e:
        print(f"Input error: {e}")
    except serial.SerialException as e:
        print(f"Serial Port Error: {e}")
    except Exception as e:
        print(f"Unexpected error: {e}")

def send_power():
    try:
        temp_value = on_off_switch_var.get()
        data_to_send = struct.pack('BB', 0, temp_value)
        ser.write(data_to_send)
        print(f"Sent power state: {temp_value}")
        print(data_to_send)
    except ValueError as e:
        print(f"Input error: {e}")
    except serial.SerialException as e:
        print(f"Serial Port Error: {e}")
    except Exception as e:
        print(f"Unexpected error: {e}")
        
def send_door_state():
    try:
        data_to_send = struct.pack('BB', 1, 1)
        ser.write(data_to_send)
        print(f"Door change state: {1}")
        print(data_to_send)
    except ValueError as e:
        print(f"Input error: {e}")
    except serial.SerialException as e:
        print(f"Serial Port Error: {e}")
    except Exception as e:
        print(f"Unexpected error: {e}")
    window.after(1000, warning_message)
 
def send_reset():
    try:
        data_to_send = struct.pack('BB', 2, 1)
        ser.write(data_to_send)
        print(f"Vending machine has RESETED: {1}")
        print(data_to_send)
    except ValueError as e:
        print(f"Input error: {e}")
    except serial.SerialException as e:
        print(f"Serial Port Error: {e}")
    except Exception as e:
        print(f"Unexpected error: {e}")
    info_message()

def overheat_and_freezing():
    
    if(automat['overheating']) == True:
        CTkMessagebox(title = "Warning!", 
                        message = "The vending machine is OVERHEATING!", 
                        icon = "warning", 
                        option_1 = "OK"
                        )
        
    if(automat['freezing']) == True:
        CTkMessagebox(title = "Warning!", 
                        message = "The vending machine is FREEZING!", 
                        icon = "warning", 
                        option_1 = "OK"
                        )
        
    window.after(300000, overheat_and_freezing)

#------------------------------------------------------WARRNING MESSAGE
def warning_message():
    if automat['door_opened'] == True:
        CTkMessagebox(title = "Warning!", 
                        message = "Handy Man is working on the vending machine!", 
                        icon = "warning", 
                        option_1 = "OK"
                        )
        open_vm_button.configure(text = "CLOSE DOOR")
        window.after(10, update_light_duty())
    else:
        open_vm_button.configure(text = "OPEN DOOR")

    
#------------------------------------------------------INFO MESSAGE
def info_message():
    CTkMessagebox(title = "Info!",
                  message = "You have rested the state of the vending machine!",
                  icon = "info", 
                  option_1 = "OK"
                  )
    

#------------------------------------------------------SWITCH FUNCTION
def switch_event():
    print("Switch toggled, current value: ", on_off_switch_var.get())

def live_gui():    
    #------------------------------------------------------Creating main window
    global window 
    window = ctk.CTk()
    window.title('Vending machine hrvatska')
    window.geometry('1200x600')
    window.minsize(1200,600)
    window.maxsize(1300,800)

    #------------------------------------------------------Setting up the grid
    window.columnconfigure((0, 1, 2, 3, 4), weight = 1)
    window.rowconfigure((0, 1), weight = 1)

    #------------------------------------------------------Setting up the frame and the grid inside it
    global slots_frame
    slots_frame = ctk.CTkFrame(master = window)
    slots_frame.grid(row = 0, 
                     column = 0, 
                     padx=10, 
                     pady=10, 
                     sticky = 'nswe'
                     )
    slots_frame.columnconfigure((0, 1, 2), weight = 1)

    amount_of_slots = int(automat['products_Number'] / 3)
    for i in range(amount_of_slots + 1):
        slots_frame.rowconfigure(i, weight = 1)

    #------------------------------------------------------SLOT BUTTONS
    slots_label = ctk.CTkLabel(master = slots_frame, 
                               text = 'SLOTS', 
                               font = ('Arial', 20), 
                               padx = 20, 
                               pady = 15
                               )
    slots_label.grid(row = 0, 
                     column = 1
                     )

    #------------------------------------------------------LEDs
    led_frame = ctk.CTkFrame(master = window)
    led_frame.grid(row = 0, 
                   column = 1, 
                   padx=10, 
                   pady=10, 
                   sticky = "nswe"
                   )
    led_frame.columnconfigure(0, weight = 1)
    led_frame.rowconfigure((0, 1), weight = 1)

    #------------------------------------------------------Load LED images
    global arrey_of_led_images
    arrey_of_led_images = {
        "red": ctk.CTkImage(dark_image = Image.open(r"C:\Users\tin.kendel\Desktop\PProject\TheOffice\GUIcode\GUI\Images\Red_light.png"), size = (240,120)),
        "yellow": ctk.CTkImage(dark_image = Image.open(r"C:\Users\tin.kendel\Desktop\PProject\TheOffice\GUIcode\GUI\Images\Yellow_light.png"), size = (240,120)),
        "green": ctk.CTkImage(dark_image = Image.open(r"C:\Users\tin.kendel\Desktop\PProject\TheOffice\GUIcode\GUI\Images\Green_light.png"), size = (240,120))
        }

    #------------------------------------------------------Display the state of capacity
    global led_label, capacity_label
    led_label = ctk.CTkLabel(master = led_frame, 
                             text = "", 
                             image = arrey_of_led_images["red"]
                             )
    led_label.grid(row = 0, 
                   column = 0
                   )

    #------------------------------------------------------Display the state of stock
    capacity_label = ctk.CTkLabel(master = led_frame, 
                               text = "",
                               font = ('Arial', 16)
                               )
    capacity_label.grid(row = 1, 
                     column = 0)
    update_state_of_capacity()
    
    #------------------------------------------------------BUTTON FOR OPENING THE vending machine and a RESET BUTTON
    global open_vm_button, refill_vm_button
    open_vm_frame = ctk.CTkFrame(master = window)
    open_vm_frame.grid(row = 0, 
                       column = 2, 
                       padx=10, 
                       pady=10, 
                       sticky = 'nswe'
                       )

    empty_vm_label = ctk.CTkLabel(master = open_vm_frame, 
                                  text = "", 
                                  height = 60, 
                                  padx = 20, 
                                  pady = 5)
    empty_vm_label.pack()
    open_vm_button = ctk.CTkButton(master = open_vm_frame,  
                                   font = ('Arial', 16), 
                                   text_color = "black", 
                                   text = "OPEN DOOR", 
                                   width = 125, 
                                   height = 125, 
                                   fg_color = "yellow", 
                                   command = send_door_state)
    open_vm_button.pack()

    refill_vm_button = ctk.CTkButton(master = open_vm_frame,  
                                    font = ('Arial', 20),  
                                    text = "REFILL", 
                                    width = 125, 
                                    height = 125, 
                                    fg_color = "red", 
                                    command = send_reset)
    refill_vm_button.pack(pady = 20)

    #------------------------------------------------------POWER SWITCH
    switch_frame = ctk.CTkFrame(master = window)
    switch_frame.grid(row = 1, 
                      column = 0, 
                      padx=10, 
                      pady=10, 
                      sticky = 'nswe'
                      )
    switch_frame.columnconfigure(0, weight = 1)
    switch_frame.rowconfigure(0, weight = 1)

    #------------------------------------------------------Switch settings 
    global on_off_switch_var
    on_off_switch_var = ctk.IntVar(value = 1)
    on_off_switch = ctk.CTkSwitch(master = switch_frame, 
                                    text = 'POWER SWITCH', 
                                    command = lambda:[send_power(), update_in_off_mode()], 
                                    variable = on_off_switch_var, 
                                    onvalue = 1, 
                                    offvalue = 0,
                                    switch_width = 100,
                                    switch_height = 50,
                                    progress_color = ("#00F220")
                                    )
    on_off_switch.grid(row = 0, 
                       column = 0
                       )

    #------------------------------------------------------SLIDER FOR SETTING THE DUTY% AND CASH DISPLAY
    light_duty_and_profit_frame = ctk.CTkFrame(master = window)
    light_duty_and_profit_frame.grid(row = 1, 
                    column = 1, 
                    columnspan = 2, 
                    padx=10, 
                    pady=10, 
                    sticky = 'nswe'
                    )
    light_duty_and_profit_frame.columnconfigure((0,1,2), weight = 1)
    light_duty_and_profit_frame.rowconfigure((0, 1, 2, 3, 4), weight = 1)

    #------------------------------------------------------Duty display & Cash storage
    #Duty progrssbar
    global light_duty_progressbar, light_duty_value
    light_duty_label = ctk.CTkLabel(master = light_duty_and_profit_frame, 
                              text = "Light DUTY%", 
                              font = ('Arial', 16)
                              )
    light_duty_label.grid(row = 1, 
                    column = 0, 
                    padx=10, 
                    pady=10, 
                    sticky="s"
                    )

    light_duty_progressbar = Progressbar(master = light_duty_and_profit_frame, 
                                   orient = "horizontal", 
                                   length = 200
                                   )
    light_duty_progressbar.grid(row = 2, 
                          column = 0, 
                          padx = 10, 
                          pady = 10
                          )

    light_duty_value = ctk.CTkLabel(master = light_duty_and_profit_frame, 
                              text = "--.-", 
                              font = ('Arial', 16)
                              )
    light_duty_value.grid(row = 3, 
                    column = 0, 
                    padx = 10, 
                    pady = 10, 
                    sticky="s"
                    )
    update_light_duty()

    #Cash
    cash_label = ctk.CTkLabel(master = light_duty_and_profit_frame, 
                              text = "Amount of cash:", 
                              font = ('Arial', 16)
                              )
    cash_label.grid(row = 2, 
                    column = 1, 
                    padx = 10, 
                    pady = 10
                    )

    global profit_label
    profit_label = ctk.CTkLabel(master = light_duty_and_profit_frame, 
                                text = "start",
                                font = ('Arial', 16)
                                )
    profit_label.grid(row = 3, 
                      column = 1, 
                      padx = 10, 
                      pady = 10, 
                      sticky="s"
                      )
    
    update_profit()
    update_slots()

    #------------------------------------------------------TEMPERATURE
    temperature_frame = ctk.CTkFrame (master = window)
    temperature_frame.grid(row = 0, 
                    rowspan = 2, 
                    column = 3, 
                    padx = 10, 
                    pady = 10, 
                    sticky = 'nswe'
                    )

    #------------------------------------------------------Temp display
    temperature_empty_label = ctk.CTkLabel(master = temperature_frame, 
                                    height = 150, 
                                    text = ""
                                    )
    temperature_empty_label.pack()

    max_temperature_label = ctk.CTkLabel(master = temperature_frame, 
                                  text = "100C",
                                  font = ('Arial', 16)
                                  )
    max_temperature_label.pack()

    global temperature_display, temperature_label
    temperature_display = Progressbar(master = temperature_frame, 
                               orient = 'vertical', 
                               mode='determinate', 
                               length = 200
                               )
    temperature_display.pack()

    min_temperature_label = ctk.CTkLabel(master = temperature_frame, 
                                  text = "0C",
                                  font = ('Arial', 16)
                                  )
    min_temperature_label.pack()

    temperature_label = ctk.CTkLabel(master = temperature_frame, 
                                     text="Temperature: --.- C",
                                     font = ('Arial', 16)
                                     )
    temperature_label.pack(pady=5)

    #------------------------------------------------------Entry
    global user_temperature_input, wanted_temperature_label, temperature_button
    entry_temperature_label = ctk.CTkLabel(master = temperature_frame, 
                              text = 'Enter wanted temperature:',
                              font = ('Arial', 16)
                              )
    entry_temperature_label.pack()

    user_temperature_input = ttk.StringVar()
    temperature_entry = ctk.CTkEntry(master = temperature_frame, 
                              textvariable = user_temperature_input,
                              font = ('Arial', 16)
                              )
    temperature_entry.pack(pady = 15)

    temperature_button = ctk.CTkButton(master = temperature_frame, 
                                text = "Confirm", 
                                command = lambda:[send_temperature(), update_wanted_temperature()],
                                font = ('Arial', 16)
                                )
    temperature_button.pack()
    
    wanted_temperature_label = ctk.CTkLabel(master = temperature_frame, 
                                              text = "",
                                              font = ('Arial', 16)
                                              )
    wanted_temperature_label.pack(pady = 5)

    update_temperature()
    #------------------------------------------------------Humidity
    humidity_frame = ctk.CTkFrame (master = window)
    humidity_frame.grid(row = 0, 
                        column = 4, 
                        rowspan = 2, 
                        padx=10, 
                        pady=10, 
                        sticky = 'nswe'
                        )

    humidity_empty_label = ctk.CTkLabel(master = humidity_frame, 
                                        height = 180, 
                                        text = ""
                                        )
    humidity_empty_label.pack()

    max_humidity_label = ctk.CTkLabel(master = humidity_frame, 
                                      text = "100%",
                                      font = ('Arial', 16)
                                      )
    max_humidity_label.pack()

    global humidity_display, humidity_label
    humidity_display = Progressbar(master = humidity_frame, 
                                   orient = "vertical", 
                                   mode = "determinate", 
                                   length = 200
                                   )
    humidity_display.pack()

    min_humidity_label = ctk.CTkLabel(master = humidity_frame, 
                                      text = "0%",
                                      font = ('Arial', 16)
                                      )
    min_humidity_label.pack()

    humidity_label = ctk.CTkLabel(humidity_frame, 
                                  text="Humidity: --.- %",
                                  font = ('Arial', 16)
                                  )
    humidity_label.pack(pady=5)
    update_humidity()
    overheat_and_freezing()
   
    #------------------------------------------------------Run
    window.mainloop()
    
#------------------------------------------------------Threads
gui_t = threading.Thread(target = live_gui)
uart_t = threading.Thread(target = uart_function)
uart_t.start()
gui_t.start()
