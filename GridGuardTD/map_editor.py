import os

def create_default_map():
    # 0: Çimen, 1: Nehir, 2: Köprü, 3: Mavi Kule, 4: Mavi Kral, 5: Kırmızı Kule, 6: Kırmızı Kral
    print("--- Clash Royale Harita Editörüne Hoş Geldiniz ---")
    print("Mevcut harita harita.txt olarak oluşturuluyor...")
    
    # Varsayılan şablonu oluşturalım
    arena = []
    for y in range(20): # 20 satır
        row = []
        for x in range(25): # 25 sütun
            # Nehir ve Köprü mantığı
            if x == 12:
                if y in [5, 14]: # Köprüler
                    row.append("2")
                else: # Nehir
                    row.append("1")
            else:
                row.append("0") # Çimen
        arena.append(row)
        
    # Kuleleri yerleştirelim (X, Y koordinatlarına göre sembolik matris)
    arena[10][2] = "4"  # Mavi Kral (Sol)
    arena[5][5]  = "3"  # Mavi Üst Kule
    arena[15][5] = "3"  # Mavi Alt Kule
    
    arena[10][22] = "6" # Kırmızı Kral (Sağ)
    arena[5][19]  = "5" # Kırmızı Üst Kule
    arena[15][19] = "5" # Kırmızı Alt Kule

    # Dosyaya yazalım
    with open("harita.txt", "w") as f:
        for row in arena:
            f.write(" ".join(row) + "\n")
    print("✔ harita.txt başarıyla yazıldı! Artık C oyunu bu dosyayı okuyacak.")

if __name__ == "__main__":
    create_default_map()