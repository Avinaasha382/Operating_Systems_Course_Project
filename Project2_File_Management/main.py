import tkinter as tk
import sys

def save(filename, text_edit):
    try:
        
        content = text_edit.get(1.0, tk.END)
        
      
        with open(filename, 'w') as file:
            file.write(content)
        
       
        print(f"File '{filename}' has been saved successfully.")
    except Exception as e:
        print(f"Error saving file: {e}")

def main():
    window = tk.Tk()
    window.title('Text Editor')

    window.rowconfigure(0, minsize=400)
    window.columnconfigure(1, minsize=500)

    text_edit = tk.Text(window, font='Helvetica 18')
    text_edit.grid(row=0, column=1)

    frame = tk.Frame(window, relief=tk.RAISED, bd=2)
    frame.grid(row=0, column=0, sticky='ns')

    filename = sys.argv[1]
    operation = sys.argv[2]
    file_content = "" 

    if operation == '2':  
        try:
            with open(filename, 'r') as file:
                file_content = file.read() 
                text_edit.insert(tk.END, file_content)
            window.title(f"{filename} - Editing")
        except Exception as e:
            print(f"Error opening file: {e}")
            return
    elif operation == '1':
        try:
            with open(filename, 'r') as file:
                file_content = file.read()  
                text_edit.insert(tk.END, file_content)
            window.title(f"{filename} - Editing")
        except Exception as e:
            print(f"Error opening file: {e}")
            return
        b1 = tk.Button(frame, text='Save', command=lambda: save(filename, text_edit))
        b1.grid(row=0, column=0, padx=5, pady=5, sticky='ew')

    window.mainloop()
def main2():
    def yes(window):
        print('y')
        window.destroy()
    def no(window):
        print('n')
        window.destroy()
    filename = sys.argv[1]
    window = tk.Tk()
    window.title(f'Dialog')

    window.rowconfigure(0, minsize=100,weight=1)
    window.columnconfigure(1, minsize=100,weight=1)
    l1 = tk.Label(text=f"Do you want to delete {filename}")
    l1.grid(row=0,column=1)
    b1 = tk.Button(window,text='Yes',width=10,command=lambda: yes(window))
    b1.grid(row=1,column=0,padx=5,pady=5)
    b2 = tk.Button(window,text='No',width=10,command=lambda: no(window))
    b2.grid(row=1,column=2,padx=5)
    window.mainloop()



if(sys.argv[2]=='1' or sys.argv[2]=='2'):
    main()
if(sys.argv[2]=='3'):
    main2()
