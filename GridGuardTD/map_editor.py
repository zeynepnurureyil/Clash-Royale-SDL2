import tkinter as tk
from tkinter import messagebox

WIDTH, HEIGHT = 800, 600
GRID_SIZE = 40
ROWS, COLS = HEIGHT // GRID_SIZE, WIDTH // GRID_SIZE
grid_data = [[0 for _ in range(COLS)] for _ in range(ROWS)]

root = tk.Tk()
root.title("GridGuard TD - Harita Tasarımcısı")
root.geometry(f"{WIDTH}x{HEIGHT + 60}")
root.resizable(False, False)

canvas = tk.Canvas(root, width=WIDTH, height=HEIGHT, bg="#1a1a1a")
canvas.pack()

def draw_grid():
    canvas.delete("all")
    for r in range(ROWS):
        for c in range(COLS):
            x1, y1 = c * GRID_SIZE, r * GRID_SIZE
            x2, y2 = x1 + GRID_SIZE, y1 + GRID_SIZE
            color = "#2ecc71" if grid_data[r][c] == 1 else "#1a1a1a"
            canvas.create_rectangle(x1, y1, x2, y2, fill=color, outline="#2c3e50")

def on_click(event):
    c, r = event.x // GRID_SIZE, event.y // GRID_SIZE
    if 0 <= r < ROWS and 0 <= c < COLS:
        grid_data[r][c] = 1 if grid_data[r][c] == 0 else 0
        draw_grid()

def save_map():
    with open("harita.txt", "w") as f:
        for row in grid_data:
            f.write(" ".join(map(str, row)) + "\n")
    root.destroy() # Kaydedince otomatik kapansın

canvas.bind("<B1-Motion>", on_click)
canvas.bind("<Button-1>", on_click)

btn_frame = tk.Frame(root, bg="#2c3e50")
btn_frame.pack(fill=tk.BOTH, expand=True)
tk.Button(btn_frame, text="🛡️ HARİTAYI KAYDET VE OYUNU BAŞLAT 🛡️", font=("Arial", 12, "bold"), fg="white", bg="#e74c3c", command=save_map).pack(pady=10)

draw_grid()
root.mainloop()