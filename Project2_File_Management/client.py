import tkinter as tk
from tkinter import messagebox
import socket

SERVER_IP = '127.0.0.1'  # Server IP
SERVER_PORT = 8080      # Server Port

def send_request(file_index, operation):
    try:
        
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as client_socket:
            client_socket.connect((SERVER_IP, SERVER_PORT))
            
            request = f"{file_index} {operation}"
            client_socket.sendall(request.encode())

            
    except Exception as e:
        print("Error")
        #messagebox.showerror("Error", f"Could not connect to server: {e}")

def on_button_click(file_index, operation):
    send_request(file_index, operation)


root = tk.Tk()
root.title("File Manager Client")

tk.Label(root, text="File Manager Operations").grid(row=0, column=0, columnspan=2, pady=10)

file_index_label = tk.Label(root, text="File Index (0-9):")
file_index_label.grid(row=1, column=0)
file_index_entry = tk.Entry(root)
file_index_entry.grid(row=1, column=1)


operations = [("Read", 0), ("Write", 1), ("Delete", 2), ("Rename", 3), ("Metadata", 4)]
for i, (label, op) in enumerate(operations, start=2):
    tk.Button(root, text=label, 
              command=lambda op=op: on_button_click(file_index_entry.get(), op)).grid(row=i, column=0, columnspan=2, pady=5)

tk.Button(root, text="Exit", command=root.quit).grid(row=7, column=0, columnspan=2, pady=10)

root.mainloop()
