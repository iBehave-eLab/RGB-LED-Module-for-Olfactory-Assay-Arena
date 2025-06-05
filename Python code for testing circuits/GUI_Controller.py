import tkinter as tk
from tkinter import ttk, Canvas, messagebox, font
import serial.tools.list_ports
import sys
import threading
import time

class ModernArduinoGUI:
    def __init__(self, root):
        self.root = root
        self.root.title("4-Channel RGB Optogenetic Communication Test")
        
        # Farbschema und Stile definieren
        self.COLORS = {
            'bg': '#f0f0f0',
            'header': '#1e88e5',
            'active': '#4caf50',
            'inactive': '#ffffff',
            'button': '#2196f3',
            'button_active': '#1976d2',
            'red': '#f44336',
            'green': '#4caf50',
            'blue': '#2196f3',
            'amber': '#ffb300'
        }

        # Pin-Zuordnungen definieren
        self.PIN_MAPPINGS = {
            'red_ch1': 13,
            'green_ch1': 12,
            'blue_ch1': 11,
            'red_ch2': 3,
            'green_ch2': 2,
            'blue_ch2': 5,
            'red_ch3': 8,
            'green_ch3': 7,
            'blue_ch3': 6,
            'red_ch4': 44,
            'green_ch4': 45,
            'blue_ch4': 46,
            'ir_led': 4,
        }
        
        # Enable-Pins definieren
        self.ENABLE_PINS = {
            'red_ch1': 30,
            'green_ch1': 31,
            'blue_ch1': 32,
            'red_ch2': 33,
            'green_ch2': 34,
            'blue_ch2': 35,
            'red_ch3': 36,
            'green_ch3': 37,
            'blue_ch3': 38,
            'red_ch4': 39,
            'green_ch4': 40,
            'blue_ch4': 41,
            'ir_led': 42,
        }
        
        # Indikator-Pins definieren
        self.INDICATOR_PINS = {
            'ind1': 22,  # IND_1A
            'ind2': 23,  # IND_1B
            'ind3': 24,  # IND_2A
            'ind4': 25,  # IND_2B
            'ind5': 26,  # IND_3A
            'ind6': 27,  # IND_3B
            'ind7': 28,  # IND_4A
            'ind8': 29,  # IND_4B
        }
        
        # Hauptfont definieren
        self.default_font = font.nametofont("TkDefaultFont")
        self.default_font.configure(size=10)
        
        # Hauptcontainer mit Padding
        self.main_frame = ttk.Frame(root, padding=(10, 10, 10, 10))
        self.main_frame.pack(fill=tk.BOTH, expand=True)
        
        # Stil für ttk-Widgets
        style = ttk.Style()
        style.configure('TButton', font=('Helvetica', 10))
        style.configure('TLabel', font=('Helvetica', 10))
        style.configure('Header.TLabel', font=('Helvetica', 12, 'bold'), foreground=self.COLORS['header'])
        style.configure('Active.TButton', background=self.COLORS['active'])
        
        # Variablen initialisieren
        self.serial_connection = None
        self.frequencies = [62500, 31250, 15625, 7812, 3906, 1953, 976, 488]
        self.ir_frequencies = [62500,7812, 976]
        self.channels = 4
        self.led_controls = {}
        self.led_markers = {}
        self.digitals_controls = {}
        self.digitals_markers = {}
        self.reading_thread = None
        self.keep_reading = False
        
        # Layout erstellen
        self.create_connection_frame()
        self.create_frequency_frame()
        self.create_led_control_frame()
        self.create_indicator_frame()
        self.create_manual_command_frame()  # Neues Feld für manuelle Befehle
        self.create_console_frame()
        
        # Event-Handler für Programmende
        self.root.protocol("WM_DELETE_WINDOW", self.on_closing)
    
    def create_connection_frame(self):
        """Erstellt den Rahmen für die Verbindungseinstellungen"""
        conn_frame = ttk.LabelFrame(self.main_frame, text="Verbindung", padding=(5, 5, 5, 5))
        conn_frame.grid(row=0, column=0, columnspan=4, sticky="ew", padx=5, pady=5)
        
        # Port-Auswahl
        ttk.Label(conn_frame, text="Port:").grid(row=0, column=0, padx=5, pady=5)
        self.port_combobox = ttk.Combobox(conn_frame, values=self.get_serial_ports(), width=20)
        self.port_combobox.grid(row=0, column=1, padx=5, pady=5)
        
        # Refresh-Button für Ports
        refresh_button = ttk.Button(conn_frame, text="⟳", width=3, command=self.refresh_ports)
        refresh_button.grid(row=0, column=2, padx=5, pady=5)
        
        # Connect/Disconnect Buttons
        self.connect_button = ttk.Button(conn_frame, text="Verbinden", command=self.connect)
        self.connect_button.grid(row=0, column=3, padx=5, pady=5)
        
        self.disconnect_button = ttk.Button(conn_frame, text="Trennen", command=self.disconnect, state=tk.DISABLED)
        self.disconnect_button.grid(row=0, column=4, padx=5, pady=5)
    
    def create_frequency_frame(self):
        """Erstellt den Rahmen für die Frequenzeinstellungen"""
        freq_frame = ttk.LabelFrame(self.main_frame, text="Frequenzeinstellungen", padding=(5, 5, 5, 5))
        freq_frame.grid(row=1, column=0, columnspan=4, sticky="ew", padx=5, pady=5)
        
        # IR Frequenz
        ttk.Label(freq_frame, text="IR Frequenz:").grid(row=0, column=0, padx=5, pady=5)
        self.ir_freq_combobox = ttk.Combobox(freq_frame, values=self.ir_frequencies, width=10)
        self.ir_freq_combobox.set(self.ir_frequencies[0])
        self.ir_freq_combobox.grid(row=0, column=1, padx=5, pady=5)
        
        set_ir_freq_button = ttk.Button(freq_frame, text="Setzen", command=self.set_ir_frequency)
        set_ir_freq_button.grid(row=0, column=2, padx=5, pady=5)
        
        # RGB Frequenz
        ttk.Label(freq_frame, text="RGB Frequenz:").grid(row=0, column=3, padx=5, pady=5)
        self.rgb_freq_combobox = ttk.Combobox(freq_frame, values=self.frequencies, width=10)
        self.rgb_freq_combobox.set(self.frequencies[0])
        self.rgb_freq_combobox.grid(row=0, column=4, padx=5, pady=5)
        
        set_rgb_freq_button = ttk.Button(freq_frame, text="Setzen", command=self.set_rgb_frequency)
        set_rgb_freq_button.grid(row=0, column=5, padx=5, pady=5)
    
    def create_led_control_frame(self):
        """Erstellt den Rahmen für die LED-Steuerung"""
        led_control_frame = ttk.LabelFrame(self.main_frame, text="LED Steuerung", padding=(5, 5, 5, 5))
        led_control_frame.grid(row=2, column=0, columnspan=3, sticky="nsew", padx=5, pady=5)
        
        colors = ['red', 'green', 'blue']
        color_names = {'red': 'Rot', 'green': 'Grün', 'blue': 'Blau'}
        
        # Header für die Kanäle
        ttk.Label(led_control_frame, text="", width=10).grid(row=0, column=0)
        for i, color in enumerate(colors):
            ttk.Label(led_control_frame, text=color_names[color], foreground=self.COLORS[color], font=('Helvetica', 10, 'bold')).grid(row=0, column=i+1, padx=5)
        
        # LED-Steuerelemente für jeden Kanal
        for ch in range(1, self.channels + 1):
            ttk.Label(led_control_frame, text=f"Kanal {ch}", font=('Helvetica', 10, 'bold')).grid(row=ch, column=0, padx=5, pady=5)
            
            for i, color in enumerate(colors):
                pin_key = f"{color}_ch{ch}"
                pin_number = self.PIN_MAPPINGS.get(pin_key, "N/A")
                enable_pin = self.ENABLE_PINS.get(pin_key, "N/A")
                
                # Frame für die Farbe/Kanal-Kombination
                pin_frame = ttk.Frame(led_control_frame)
                pin_frame.grid(row=ch, column=i+1, padx=5, pady=5)
                
                # Pin-Anzeige
                pin_label = ttk.Label(pin_frame, text=f"Pin: {pin_number}", font=('Helvetica', 8))
                pin_label.pack(anchor=tk.W)
                
                control_frame = ttk.Frame(pin_frame, padding=(5, 5, 5, 5))
                control_frame.pack(fill=tk.X)
                
                
                # Slider für PWM
                pwm_scale = ttk.Scale(control_frame, from_=0, to=100, orient=tk.HORIZONTAL, length=100)
                pwm_scale.pack(side=tk.LEFT, padx=2)
                
                # PWM-Wert Label
                pwm_value = tk.StringVar(value="0%")
                pwm_label = ttk.Label(control_frame, textvariable=pwm_value, width=4)
                pwm_label.pack(side=tk.LEFT, padx=2)
                
                # PWM slider value change binding
                pwm_scale.bind("<Motion>", lambda event, s=pwm_scale, v=pwm_value: v.set(f"{int(s.get())}%"))
                
                # Buttons
                button_frame = ttk.Frame(control_frame)
                button_frame.pack(side=tk.LEFT, padx=2)
                
                send_button = ttk.Button(button_frame, text="PWM", width=5, 
                                         command=lambda c=color, ch=ch, s=pwm_scale: self.send_pwm_value(c, ch, s))
                send_button.pack(side=tk.TOP, pady=2)
                
                on_off_frame = ttk.Frame(button_frame)
                on_off_frame.pack(side=tk.TOP, pady=2)
                
                activate_button = ttk.Button(on_off_frame, text="Ein", width=3,
                                            command=lambda c=color, ch=ch: self.activate_led(c, ch))
                activate_button.pack(side=tk.LEFT, padx=1)
                
                deactivate_button = ttk.Button(on_off_frame, text="Aus", width=3,
                                              command=lambda c=color, ch=ch: self.deactivate_led(c, ch))
                deactivate_button.pack(side=tk.LEFT, padx=1)
                
                # LED Status-Anzeige mit Enable-Pin-Nummer
                status_frame = ttk.Frame(control_frame)
                status_frame.pack(side=tk.LEFT, padx=2)
                
                en_label = ttk.Label(status_frame, text=f"En: {enable_pin}", font=('Helvetica', 8))
                en_label.pack(anchor=tk.W)
                
                canvas = Canvas(status_frame, width=20, height=20, bg="white", 
                                highlightthickness=1, highlightbackground="black")
                marker = canvas.create_oval(2, 2, 18, 18, fill="white")
                canvas.pack()
                
                # Speichern der Steuerelemente in Dictionaries
                self.led_controls[f"{color}_ch{ch}"] = (pwm_scale, pwm_value)
                self.led_markers[f"{color}_ch{ch}"] = (canvas, marker)
        
        # IR-LED-Steuerung
        ir_frame = ttk.LabelFrame(led_control_frame, text="IR LED", padding=(5, 5, 5, 5))
        ir_frame.grid(row=self.channels+1, column=0, columnspan=4, sticky="ew", padx=5, pady=5)
        
        # IR Pin-Anzeige
        ir_pin_frame = ttk.Frame(ir_frame)
        ir_pin_frame.pack(side=tk.LEFT, padx=5)
        
        ir_pin_label = ttk.Label(ir_pin_frame, text=f"Pin: {self.PIN_MAPPINGS['ir_led']}", font=('Helvetica', 9))
        ir_pin_label.pack(anchor=tk.W)
        
        ir_enable_label = ttk.Label(ir_pin_frame, text=f"Enable: {self.ENABLE_PINS['ir_led']}", font=('Helvetica', 9))
        ir_enable_label.pack(anchor=tk.W)
        
        # IR PWM-Steuerung
        ir_pwm_frame = ttk.Frame(ir_frame)
        ir_pwm_frame.pack(side=tk.LEFT, padx=10)
        
        self.ir_pwm_scale = ttk.Scale(ir_pwm_frame, from_=0, to=100, orient=tk.HORIZONTAL, length=150)
        self.ir_pwm_scale.pack(side=tk.LEFT)
        
        self.ir_pwm_value = tk.StringVar(value="0%")
        ir_pwm_label = ttk.Label(ir_pwm_frame, textvariable=self.ir_pwm_value, width=4)
        ir_pwm_label.pack(side=tk.LEFT, padx=5)
        
        # PWM slider value change binding
        self.ir_pwm_scale.bind("<Motion>", lambda event: self.ir_pwm_value.set(f"{int(self.ir_pwm_scale.get())}%"))
        
        # IR-Buttons
        ir_button_frame = ttk.Frame(ir_frame)
        ir_button_frame.pack(side=tk.LEFT, padx=10)
        
        self.ir_send_button = ttk.Button(ir_button_frame, text="PWM senden", command=self.send_ir_pwm_value)
        self.ir_send_button.pack(side=tk.TOP, pady=2)
        
        ir_on_off_frame = ttk.Frame(ir_button_frame)
        ir_on_off_frame.pack(side=tk.TOP, pady=2)
        
        self.ir_activate_button = ttk.Button(ir_on_off_frame, text="Ein", width=5, command=self.activate_ir_led)
        self.ir_activate_button.pack(side=tk.LEFT, padx=2)
        
        self.ir_deactivate_button = ttk.Button(ir_on_off_frame, text="Aus", width=5, command=self.deactivate_ir_led)
        self.ir_deactivate_button.pack(side=tk.LEFT, padx=2)
        
        # IR Status-Anzeige
        ir_status_frame = ttk.Frame(ir_frame)
        ir_status_frame.pack(side=tk.LEFT, padx=10)
        
        self.ir_canvas = Canvas(ir_status_frame, width=20, height=20, bg="white", 
                              highlightthickness=1, highlightbackground="black")
        self.ir_marker = self.ir_canvas.create_oval(2, 2, 18, 18, fill="white")
        self.ir_canvas.pack()
        
        self.led_markers["ir_led"] = (self.ir_canvas, self.ir_marker)
    
    def create_indicator_frame(self):
        """Erstellt den Rahmen für die Indikator-Steuerung"""
        indicator_frame = ttk.LabelFrame(self.main_frame, text="Digitale Indikatoren", padding=(5, 5, 5, 5))
        indicator_frame.grid(row=2, column=3, rowspan=2, sticky="nsew", padx=5, pady=5)
        
        # Indikatoren in einem Grid anordnen
        for pin in range(1, 9):
            row = (pin - 1) // 2
            col = (pin - 1) % 2
            
            pin_frame = ttk.Frame(indicator_frame, padding=(5, 5, 5, 5))
            pin_frame.grid(row=row, column=col, padx=5, pady=5)

            indicator_key = f"ind{pin}"
            pin_number = self.INDICATOR_PINS.get(indicator_key, "N/A")
            
            pin_label = ttk.Label(pin_frame, text=f"Indikator {pin}")
            pin_label.pack(pady=2)

            # Pin-Nummer anzeigen
            pin_number_label = ttk.Label(pin_frame, text=f"Pin: {pin_number}", font=('Helvetica', 8))
            pin_number_label.pack(pady=2)
            
            # Status-Anzeige
            canvas = Canvas(pin_frame, width=20, height=20, bg="white", 
                           highlightthickness=1, highlightbackground="black")
            marker = canvas.create_oval(2, 2, 18, 18, fill="white")
            canvas.pack(pady=2)
            
            # Toggle-Button
            indicator_button = ttk.Button(pin_frame, text="Umschalten", width=10,
                                        command=lambda p=pin: self.toggle_pin(p))
            indicator_button.pack(pady=2)
            
            self.digitals_controls[f"ind{pin}"] = indicator_button
            self.digitals_markers[f"ind{pin}"] = (canvas, marker)
    
    def create_manual_command_frame(self):
        """Erstellt den Rahmen für manuelle Befehlseingabe"""
        manual_frame = ttk.LabelFrame(self.main_frame, text="Manuelle Befehlseingabe", padding=(5, 5, 5, 5))
        manual_frame.grid(row=3, column=0, columnspan=3, sticky="ew", padx=5, pady=5)
        
        # Textfeld für Befehlseingabe
        self.command_entry = ttk.Entry(manual_frame, width=50)
        self.command_entry.pack(side=tk.LEFT, padx=5, pady=5, fill=tk.X, expand=True)
        
        # Enter-Taste zum Senden des Befehls
        self.command_entry.bind("<Return>", lambda event: self.send_manual_command())
        
        # Senden-Button
        send_button = ttk.Button(manual_frame, text="Senden", command=self.send_manual_command)
        send_button.pack(side=tk.LEFT, padx=5, pady=5)
    
    def create_console_frame(self):
        """Erstellt den Rahmen für die Konsole"""
        console_frame = ttk.LabelFrame(self.main_frame, text="Protokoll", padding=(5, 5, 5, 5))
        console_frame.grid(row=4, column=0, columnspan=4, sticky="nsew", padx=5, pady=5)
        
        # Textfeld mit Scrollbar
        console_scroll = ttk.Scrollbar(console_frame)
        console_scroll.pack(side=tk.RIGHT, fill=tk.Y)
        
        self.console = tk.Text(console_frame, height=10, width=80, 
                              yscrollcommand=console_scroll.set, font=('Courier', 9))
        self.console.pack(side=tk.LEFT, fill=tk.BOTH, expand=True)
        
        console_scroll.config(command=self.console.yview)
        
        # Control-Buttons
        button_frame = ttk.Frame(console_frame)
        button_frame.pack(side=tk.BOTTOM, fill=tk.X)
        
        # Clear Button
        clear_button = ttk.Button(button_frame, text="Löschen", command=self.clear_console)
        clear_button.pack(side=tk.LEFT, padx=5, pady=5)
        
        # Autoscroll-Checkbox
        self.autoscroll_var = tk.BooleanVar(value=True)
        autoscroll_check = ttk.Checkbutton(button_frame, text="Autoscroll", variable=self.autoscroll_var)
        autoscroll_check.pack(side=tk.LEFT, padx=5, pady=5)
    
    def get_serial_ports(self):
        """Verfügbare serielle Ports abrufen"""
        ports = [port.device for port in serial.tools.list_ports.comports()]
        return ports if ports else ["Keine Ports gefunden"]
    
    def refresh_ports(self):
        """Aktualisiert die Liste der verfügbaren Ports"""
        self.port_combobox['values'] = self.get_serial_ports()
        self.console.insert(tk.END, "Portliste aktualisiert\n")
        self.console.see(tk.END)
    
    def connect(self):
        """Verbindung zum Arduino herstellen"""
        port = self.port_combobox.get()
        if port and port != "Keine Ports gefunden":
            try:
                self.serial_connection = serial.Serial(port, 115200, timeout=1)
                self.console.insert(tk.END, f"Verbunden mit {port}\n")
                self.console.see(tk.END)
                self.connect_button.config(state=tk.DISABLED)
                self.disconnect_button.config(state=tk.NORMAL)
                
                # Starten des Lese-Threads für die serielle Kommunikation
                self.keep_reading = True
                self.reading_thread = threading.Thread(target=self.read_from_serial)
                self.reading_thread.daemon = True  # Thread wird beendet, wenn das Hauptprogramm endet
                self.reading_thread.start()
                
            except Exception as e:
                messagebox.showerror("Verbindungsfehler", f"Konnte nicht mit {port} verbinden:\n{str(e)}")
                self.console.insert(tk.END, f"Fehler: {str(e)}\n")
                self.console.see(tk.END)
    
    def disconnect(self):
        """Verbindung zum Arduino trennen"""
        if self.serial_connection:
            try:
                self.keep_reading = False
                if self.reading_thread:
                    self.reading_thread.join(timeout=1.0)  # Warten auf Thread-Ende
                
                self.serial_connection.close()
                self.serial_connection = None
                self.console.insert(tk.END, "Verbindung getrennt\n")
                self.console.see(tk.END)
                self.connect_button.config(state=tk.NORMAL)
                self.disconnect_button.config(state=tk.DISABLED)
            except Exception as e:
                messagebox.showerror("Verbindungsfehler", f"Fehler beim Trennen der Verbindung:\n{str(e)}")
                self.console.insert(tk.END, f"Fehler: {str(e)}\n")
                self.console.see(tk.END)
    
    def on_closing(self):
        """Aufräumen vor dem Beenden"""
        self.keep_reading = False
        if self.reading_thread:
            self.reading_thread.join(timeout=1.0)
            
        if self.serial_connection:
            try:
                self.serial_connection.close()
            except:
                pass
        self.root.destroy()
    
    def read_from_serial(self):
        """Liest kontinuierlich Daten vom seriellen Port"""
        while self.keep_reading and self.serial_connection:
            try:
                if self.serial_connection.in_waiting > 0:
                    data = self.serial_connection.readline().decode('utf-8', errors='replace').strip()
                    if data:
                        # GUI-Updates in einem Thread-sicheren Kontext durchführen
                        self.root.after(0, self.update_console, f"Arduino: {data}\n")
                time.sleep(0.01)  # Kleine Pause, um CPU-Last zu reduzieren
            except Exception as e:
                self.root.after(0, self.update_console, f"Fehler beim Lesen: {str(e)}\n")
                time.sleep(1)  # Längere Pause nach einem Fehler
    
    def update_console(self, message):
        """Aktualisiert die Konsole mit einer neuen Nachricht (Thread-sicher)"""
        self.console.insert(tk.END, message)
        if self.autoscroll_var.get():
            self.console.see(tk.END)
    
    def clear_console(self):
        """Löscht den Inhalt der Konsole"""
        self.console.delete(1.0, tk.END)
    
    def send_manual_command(self):
        """Sendet einen manuell eingegebenen Befehl an den Arduino"""
        command = self.command_entry.get().strip()
        if command:
            self.send_command(command)
            self.command_entry.delete(0, tk.END)  # Eingabefeld leeren
    
    def send_command(self, command):
        """Sendet einen Befehl an den Arduino"""
        if self.serial_connection:
            try:
                self.serial_connection.write((command + "\n").encode())
                self.console.insert(tk.END, f"Gesendet: {command}\n")
                if self.autoscroll_var.get():
                    self.console.see(tk.END)
                
            except Exception as e:
                messagebox.showerror("Kommunikationsfehler", f"Fehler beim Senden des Befehls:\n{str(e)}")
                self.console.insert(tk.END, f"Fehler: {str(e)}\n")
                if self.autoscroll_var.get():
                    self.console.see(tk.END)
        else:
            messagebox.showwarning("Nicht verbunden", "Bitte stellen Sie zuerst eine Verbindung her.")
    
    def send_pwm_value(self, color, ch, scale):
        """Sendet PWM-Werte an den Arduino für die angegebene LED und den Kanal."""
        duty_cycle = int(scale.get())
        command = f"PWM_{color}_ch{ch}_{duty_cycle}"
        self.send_command(command)
    
    def activate_led(self, color, ch):
        """Aktiviert die angegebene LED und setzt die Farbe des Markers."""
        self.send_command(f"EN_{color}_ch{ch}_1")
        canvas, marker = self.led_markers[f"{color}_ch{ch}"]
        canvas.itemconfig(marker, fill=self.COLORS[color])
    
    def deactivate_led(self, color, ch):
        """Deaktiviert die angegebene LED und setzt die Farbe des Markers auf weiß."""
        self.send_command(f"EN_{color}_ch{ch}_0")
        canvas, marker = self.led_markers[f"{color}_ch{ch}"]
        canvas.itemconfig(marker, fill=self.COLORS['inactive'])
    
    def send_ir_pwm_value(self):
        """Sendet PWM-Werte für die IR-LED."""
        duty_cycle = int(self.ir_pwm_scale.get())
        command = f"PWM_ir_led_{duty_cycle}"
        self.send_command(command)
    
    def activate_ir_led(self):
        """Aktiviert die IR-LED und setzt die Farbe des Markers auf Amber."""
        self.send_command("EN_ir_led_1")
        self.ir_canvas.itemconfig(self.ir_marker, fill=self.COLORS['amber'])
    
    def deactivate_ir_led(self):
        """Deaktiviert die IR-LED und setzt die Farbe des Markers auf weiß."""
        self.send_command("EN_ir_led_0")
        self.ir_canvas.itemconfig(self.ir_marker, fill=self.COLORS['inactive'])
    
    def toggle_pin(self, pin):
        """Umschalten des digitalen Pins (High / Low) und Aktualisierung des Markers."""
        canvas, marker = self.digitals_markers[f"ind{pin}"]
        if canvas.itemcget(marker, "fill") == self.COLORS['inactive']:  # Low (weiß)
            self.send_command(f"IND_{pin}_1")
            canvas.itemconfig(marker, fill=self.COLORS['amber'])  # High (Amber)
        else:  # High (Amber)
            self.send_command(f"IND_{pin}_0")
            canvas.itemconfig(marker, fill=self.COLORS['inactive'])  # Low (weiß)
    
    def set_rgb_frequency(self):
        """Setzt die Frequenz für alle RGB-LEDs und sendet sie an den Serial-Port."""
        frequency = self.rgb_freq_combobox.get()
        command = f"FREQ_rgb_{frequency}"
        self.send_command(command)
    
    def set_ir_frequency(self):
        """Setzt die Frequenz für die IR-LED und sendet sie an den Serial-Port."""
        frequency = self.ir_freq_combobox.get()
        command = f"FREQ_ir_{frequency}"
        self.send_command(command)

if __name__ == "__main__":
    root = tk.Tk()
    app = ModernArduinoGUI(root)
    root.geometry("1150x1000")  # Startgröße des Fensters
    root.resizable(True, True)  # Fenster kann in der Größe geändert werden
    root.mainloop()