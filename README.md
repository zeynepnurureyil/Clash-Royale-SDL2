# ⚔️ Clash Royale Tactical Core: Hibrit Mimari ile Gerçek Zamanlı Strateji Oyunu Motoru

Bu proje; popüler real-time strateji (RTS) oyunu **Clash Royale** mekaniklerini temel alarak geliştirilmiş; veri üretimi ve prosedürel harita tasarımı için **Python**, yüksek performanslı alt seviye oyun motoru çekirdeği, fiziksel simülasyon ve piksel tabanlı grafik çizimleri için ise saf **C dili (SDL2)** kullanılarak geliştirilmiş hibrit mimarili bir oyun motoru çalışmasıdır.

Projenin geliştirme, optimizasyon, dökümantasyon ve mimari tasarım süreçlerinde **Yapay Zeka Yardımlı Yazılım Geliştirme (AI-Assisted Software Engineering)** metotlarından yararlanılmış; kod yapılandırması, matematiksel modellemeler ve hata ayıklama (debugging) aşamalarında **Claude** ve **Gemini** gibi ileri düzey büyük dil modellerinden (LLM) aktif olarak faydalanılmıştır.

Proje, dışarıdan hazır hiçbir multimedya asset'i (görsel, sprite, hazır resim) yüklemeden, tüm dünyayı ve karakterleri **saf matematiksel denklemler, trigonometrik fonksiyonlar ve primitif geometrik çizimlerle** sıfırdan inşa etmektedir.

---

## 🛠️ Mimari ve Yazılım Mühendisliği Çözümleri (Kodun Mantığı)

Projenin kaynak kodlarında uygulanan ve projenin teknik derinliğini gösteren temel mühendislik yaklaşımları şunlardır:

### 1. Bellek Dostu Nesne Havuzu (Object Pooling) Mantığı
Oyun sırasında dinamik olarak bellek tahsis etmek (`malloc`/`free`) gerçek zamanlı oyunlarda bellek parçalanmasına (fragmentation) ve anlık takılmalara (stuttering) yol açar. Bu projede, bellek yönetimi optimize edilmiş ve statik nesne havuzları kullanılmıştır:
* `MAX_UNITS (30)`, `MAX_WALLS (5)` ve `MAX_PARTICLES (100)` makroları ile nesne sınırları derleme zamanında belirlenmiştir.
* Aktif ve inaktif birimler, struct yapılarındaki `bool active` bayrağı (flag) ile kontrol edilerek çalışma zamanında (runtime) CPU ve bellek optimizasyonu sağlanmıştır.

### 2. Dosya Akış Tabanlı (File Stream) Python-C Entegrasyonu
İki farklı programlama dili arasında veri köprüsü kurmak amacıyla dosya tabanlı bir boru hattı (pipeline) tasarlanmıştır:
* **Python Katmanı (`map_generator.py`):** Alt seviye C motorunun anlayacağı şekilde $20 \times 25$ boyutunda iki boyutlu bir ayrık matris (discrete matrix) haritası üretir. Hücrelere atanan tam sayı kodları (0-6) aracılığıyla nehir, köprü ve kule lokasyonlarını serileştirir (serialization).
* **C Katmanı (`game.c` -> `init_clash_arena`):** `fopen` ve `fscanf` fonksiyonları ile ham matris verisini okur (deserialization). Okunan her bir matris indeksini, ekran çözünürlüğüne oranlayarak dinamik piksel koordinatlarına ($X$ ve $Y$ lineer dönüşümü) dönüştürür. Dosyanın eksik olması durumuna karşın hata yakalama (Error Handling) mekanizması olarak varsayılan kule dizilimini yükleyen bir **Güvenli Mod (Safe Mode)** protokolü içerir.

### 3. Öklid Uzayında Vektörel Yapay Zeka ve Koşullu Durum Makinesi (State Machine)
`logic.c` içerisindeki oyun döngüsü, her karede (frame) birimlerin davranışlarını yöneten bir yapay zeka algoritması çalıştırır:
* **Menzil ve Hedef Tarama Algoritması:** Sahadaki her aktif asker, 140 piksellik bir dairesel görüş alanı içinde düşman birimlerini tarar. En yakın hedefi tespit etmek için iki boyutlu Öklid uzaklığı formülü ($d = \sqrt{(x_2 - x_1)^2 + (y_2 - y_1)^2}$) hesaplanır.
* **Reaktif Kontra-Atak (AI Anti-Counter):** Yapay zeka, oyuncunun hamlelerini dinamik olarak analiz eder. Eğer oyuncu yüksek sağlık puanına sahip tehlikeli bir `P.E.K.K.A` birimi çağırırsa, AI bunu algılar ve onu hızlıca eritebilmek için reaktif bir kararla doğrudan `Goblin` birimlerini asenkron olarak sahaya sürer.
* **Hiyerarşik Hedef Önceliği (Kule Kilidi):** Askerlerin doğrudan ana üsse yürümesini engelleyen bir koruma algoritması mevcuttur. Sol veya sağ kanat kulelerinin (`towers[4]` ve `towers[5]`) aktiflik durumu kontrol edilir; kanat kuleleri imha edilmeden ana Kral Kulesi (`is_king`) hedef kümesine dahil edilemez.

---

## 🎨 Piksel Tabanlı Grafik Programlama ve Karakter Tasarımları

Oyundan maksimum performans almak ve dış kütüphane bağımlılıklarını sıfıra indirmek adına `graphics.c` dosyasında tüm çizimler katmanlar halinde geometrik olarak kodlanmıştır:

* **Trigonometrik Yürüyüş Animasyonu (Sinusoidal Bobbing):** Karakterlerin yürüme esnasındaki dikey salınım hareketi, zaman parametresine bağlı trigonometrik bir fonksiyonla ($\Delta y = \sin(\text{anim\_timer}) \times 2$) pikselsel olarak gerçek zamanlı hesaplanır. Böylece askerler donuk görünmek yerine aşağı-yukarı sekerek hareket ederler.
* **P.E.K.K.A (Siyah / Koyu Metalik Gövde - En Güçlü Birim):** Klavye üzerinden **3 tuşu** ile seçilip sol fare tıkı ile sahaya sürülür. `RGB(44, 62, 80)` renk koduyla devasa koyu bir metal gövdeye sahiptir. `SDL_RenderDrawLine` fonksiyonu kullanılarak matematiksel uç koordinatlarına ikonik mor boynuzları (`RGB(155, 89, 182)`) çizilmiştir. Yüksek can ve hasar metriklerine sahiptir.
* **Knight / Şövalye (Mavi/Kırmızı Zırhlı Gövde - Dengeli Birim):** Klavye üzerinden **1 tuşu** ile çağrılır. P.E.K.K.A'ya kıyasla daha dengeli ve orta seviye güce sahiptir. Takım rengine göre dinamik olarak değişen (`RGB(41, 128, 185)` veya `RGB(192, 41, 43)`) bir pelerin, zırhlı ana gövde ve üzerinde gri renkli bir miğfer katmanının (`SDL_Rect`) üst üste bindirilmesiyle çizilmiştir.
* **Goblin (Yeşil, Küçük ve Hızlı Suikastçı Birim):** Klavye üzerinden **2 tuşu** ile çağrılır. `RGB(46, 204, 113)` renk koduyla yeşil ve minimal boyutlarda piksellenmiştir. Hareket hızı (1.5) diğer birimlerin neredeyse iki katıdır. Düşman kulelerinin arkasına hızlıca sızabilen, yüksek hareket kabiliyetine sahip bir suikastçı taktiğiyle tasarlanmıştır.

---

## 🎮 Gelişmiş Oyun Mekanikleri ve Kontroller

### Dinamik Savunma Barikatı (`Wall`)
* Oyuncu, oyunun gidişatına göre taktiksel savunma yapabilmek için **SPACE (Boşluk)** tuşuna basarak fare imlecinin anlık koordinatına dinamik bir savunma barikatı enjekte eder.
* Bu işlem 4 iksir maliyetine sahiptir ve haritada düşman rotasını keserek askerleri oyalayan 600 HP'lik gri renkli (`RGB(127, 140, 141)`) bir engelleme alanı (`SDL_Rect`) oluşturur.

### Oyun Hissiyatı (Game Feel) Algoritmaları
* **Dinamik Kamera Sarsıntısı (Screen Shake):** Kuleler darbe aldığında veya yıkıldığında `screen_shake_timer` tetiklenir. Render döngüsü, ekran koordinatlarına anlık ve rastgele ofset değerleri (`rand() % 7 - 3`) ekleyerek sarsıntı efekti üretir.
* **Parçacık Efekt Motoru (Particle System):** Kritik fiziksel etkileşimlerde (birim ölümleri veya kule yıkımları) `create_explosion` fonksiyonu çağrılır. Her parçacığa rastgele fırlama hızları (`vx, vy`) ve ömür süreleri (`life`) atanarak Euler integral yöntemiyle fiziksel hareket simüle edilir.
* **Alan Etkili Hasar (AOE):** P.E.K.K.A bir kuleye hasar verdiğinde, kule merkezli 60 piksellik yarıçap içindeki tüm düşman askerlerini tarayan ve onlara azaltılmış toplu hasar veren bir alan hasarı algoritması çalışır.

### Kazanan Odaklı Dinamik Ekran Kapanışı (Dynamic Game Over Banner)
* Ana kulelerden biri yıkılıp oyun `STATE_GAME_OVER` durumuna geçtiğinde, ekranı kaplayan yarı şeffaf bir arka plan katmanı (`SDL_BLENDMODE_BLEND` ile %80 opaklıkta siyah bir overlay) işlenir.
* Bu katmanın tam ortasına, **maçı hangi takım kazandıysa o takımın renginde** (Mavi takım için mavi, Kırmızı takım için kırmızı) devasa bir galibiyet banner'ı çizilir. Kazanan rengin ekranı kaplaması görsel olarak oyun sonu bildirimini netleştirir.
* **R Tuşu:** Bu ekrandayken oyuncu maçı kazandıysa `current_level` artırılır ve düşman kulelerine dinamik sağlık bonusu (`hp_bonus = (level - 1) * 500`) eklenerek oyun zorluğu ölçeklenir (Dynamic Difficulty Scaling). Oyuncu kaybettiyse ceza olarak 1. seviyeden başlatır.

---

## 🤖 Yapay Zeka (AI) Katkıları ve Geliştirme Süreci

Bu projenin üretim döngüsünde modern yazılım geliştirme araçlarından (Claude ve Gemini) şu alanlarda yararlanılmıştır:
* **Matematiksel Dönüşümler:** Python matris koordinatlarının ($20 \times 25$), SDL2 piksel koordinat sistemine kusursuz şekilde oranlanması için gerekli lineer transformasyon formüllerinin optimizasyonunda,
* **Bellek Güvenliği:** C dilinde sıkça karşılaşılan pointer hatalarının, array taşmalarının (out-of-bounds) ve Object Pooling yapısındaki flag yönetimlerinin hatasız kurgulanmasında,
* **Oyun Döngüsü Mantığı:** Delta zaman bağımsız oyun içi timer'ların hesaplanması ve `update_game_logic` içerisindeki durum geçişlerinin (State Machine) temiz bir mimariye oturtulmasında yapay zeka modellerinin analitik gücünden faydalanılmıştır.

---

## 📂 Proje Dosya Yapısı ve Bağımlılıklar

```text
├── src/
│   ├── main.c           # Oyun döngüsü (Game Loop), pencere yönetimi ve girdi (Input) yönetimi
│   ├── graphics.c       # Piksel tabanlı render algoritmaları, animasyon, karakter çizimleri ve sarsıntı motoru
│   ├── game.c           # Yapay zeka, fiziksel çarpışmalar, dosya okuma ve oyun mantığı (Logic)
│   ├── game.h           # Makrolar, Enum tipleri, Struct yapıları ve global veri havuzu
│   └── map_generator.py # Prosedürel harita matrisini üretip dışa aktaran Python betiği
├── harita.txt           # İki dil arasında veri transferini sağlayan serileştirilmiş matris dosyası
└── README.md            # Teknik dokümantasyon
Sistem Gereksinimleri
C Derleyicisi (GCC / MinGW) ve Standart Matematik Kütüphanesi (-lm)

SDL2 Geliştirme Kütüphanesi (Simple DirectMedia Layer 2.0)

Python 3.x çalışma ortamı

Derleme Komutu
Oyun motorunun tüm modülleri, matematik kütüphanesi ve SDL2 bağlayıcıları (linkers) dahil edilerek şu şekilde derlenir:

Bash
gcc main.c graphics.c game.c -o ClashTacticalCore -lSDL2 -lSDL2main -lm
Çalıştırma Sıralaması
Projenin doğru çalışması için önce veri üretici Python scripti tetiklenmeli, ardından derlenen ikili (binary) dosya çalıştırılmalıdır:

Bash
python map_generator.py
./ClashTacticalCore
# ⚔️ Clash Royale Tactical Core: Hibrit Mimari ile Gerçek Zamanlı Strateji Oyunu Motoru

Bu proje; popüler real-time strateji (RTS) oyunu **Clash Royale** mekaniklerini temel alarak geliştirilmiş; veri üretimi ve prosedürel harita tasarımı için **Python**, yüksek performanslı alt seviye oyun motoru çekirdeği, fiziksel simülasyon ve piksel tabanlı grafik çizimleri için ise saf **C dili (SDL2)** kullanılarak geliştirilmiş hibrit mimarili bir oyun motoru çalışmasıdır.

Projenin geliştirme, optimizasyon, dökümantasyon ve mimari tasarım süreçlerinde **Yapay Zeka Yardımlı Yazılım Geliştirme (AI-Assisted Software Engineering)** metotlarından yararlanılmış; kod yapılandırması, matematiksel modellemeler ve hata ayıklama (debugging) aşamalarında **Claude** ve **Gemini** gibi ileri düzey büyük dil modellerinden (LLM) aktif olarak faydalanılmıştır.

Proje, dışarıdan hazır hiçbir multimedya asset'i (görsel, sprite, hazır resim) yüklemeden, tüm dünyayı ve karakterleri **saf matematiksel denklemler, trigonometrik fonksiyonlar ve primitif geometrik çizimlerle** sıfırdan inşa etmektedir.

---

## 🛠️ Temel C Yapıları ve Kodun Mantıksal Algoritma Analizi

Projenin `game.c` ve `logic.c` dosyalarındaki ana oyun mantığı, C dilinin temel yapı taşları (`if-else` koşulları, `for` döngüleri, mantıksal operatörler ve matrisler) üzerine kurulmuştur. Hocanın inceleyeceği temel algoritmik yapılar şunlardır:

### 1. `if - else if - else` Koşul Blokları ve Karar Mekizmaları
Oyun döngüsü boyunca birimlerin ne zaman hareket edeceği, ne zaman saldıracağı ve hangi kartın çağrılacağı tamamen iç içe geçmiş (nested) `if-else` yapılarıyla denetlenir.

* **İksir ve Kart Çağırma Kontrolü (`spawn_card`):** Sahaya bir asker sürülürken, oyuncunun iksir barı ile kartın maliyeti `if` kontrolüyle kıyaslanır. Eğer iksir yeterliyse çıkarma yapılır, değilse fonksiyon `return` ile sonlandırılır:
  ```c
  int cost = (type == CARD_PEKKA) ? 6 : ((type == CARD_GOBLIN) ? 2 : 3);
  if (team == TEAM_BLUE && blue_elixir < cost) return;
  if (team == TEAM_RED && red_elixir < cost) return;
Derleme Komutu
Oyun motorunun tüm modülleri, matematik kütüphanesi ve SDL2 bağlayıcıları (linkers) dahil edilerek şu şekilde derlenir:


gcc main.c graphics.c game.c -o ClashTacticalCore -lSDL2 -lSDL2main -lm
Çalıştırma Sıralaması
Projenin doğru çalışması için önce veri üretici Python scripti tetiklenmeli, ardından derlenen ikili (binary) dosya çalıştırılmalıdır:


python map_generator.py
./ClashTacticalCore



ad:zeynep nur
soyad:üreyil
numara:250150013
ders:bilgisayar programlama
