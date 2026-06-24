<div align="center">
  <img src="assets/pardus.png" width="72" height="72">
  <h1 align="center">PardusEdu</h1>
  <p align="center"><b>Öğrenci Merkezi — Akademik Yaşamın Hepsi Bir Arada</b></p>

  [![TEKNOFEST](https://img.shields.io/badge/TEKNOFEST-2026-8A2BE2?style=flat-square)](https://teknofest.org)
  [![Pardus](https://img.shields.io/badge/Pardus-Linux-FF6600?style=flat-square&logo=linux)](https://www.pardus.org.tr)
  [![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg?style=flat-square)](LICENCE)
  [![C++17](https://img.shields.io/badge/C++-17-00599C?style=flat-square&logo=c%2B%2B)](https://en.cppreference.com/w/cpp/17)
  [![GTK4](https://img.shields.io/badge/GTK-4.0-4A90D9?style=flat-square&logo=gtk)](https://www.gtk.org/)
  [![Build](https://img.shields.io/badge/Build-Meson%20%2B%20Ninja-00A6C0?style=flat-square)](https://mesonbuild.com/)
  [![Tests](https://img.shields.io/badge/Tests-15%2F15%20passing-00C853?style=flat-square)](https://github.com/coderianx/pardusedu)
  [![Platform](https://img.shields.io/badge/Platform-Linux%20(Pardus)-FF6600?style=flat-square)](https://www.pardus.org.tr)
</div>

<br>

<p align="center">
  <b>PardusEdu</b>, Pardus Linux için geliştirilmiş, öğrencilerin tüm akademik ihtiyaçlarını tek bir çatı altında toplayan açık kaynaklı bir GTK4 masaüstü uygulamasıdır.<br>
  <b>TEKNOFEST 2026</b> — Pardus Öneri ve Geliştirme Kategorisi.
</p>

<br>

## ✨ Öne Çıkan Özellikler

<table>
<tr>
  <td align="center" width="25%">
    <h3>🕐 Pomodoro</h3>
    <sub>Zaman yönetimi, seri takibi, özelleştirilebilir süreler</sub>
  </td>
  <td align="center" width="25%">
    <h3>📋 Görevler</h3>
    <sub>5 kategori, 3 öncelik, son tarih, canlı istatistik</sub>
  </td>
  <td align="center" width="25%">
    <h3>📝 Notlar</h3>
    <sub>Ders bazlı düzenleyici, biçimlendirme araç çubuğu, harf notu</sub>
  </td>
  <td align="center" width="25%">
    <h3>📅 Program</h3>
    <sub>Haftalık zaman çizelgesi, derslik takibi</sub>
  </td>
</tr>
<tr>
  <td align="center">
    <h3>🔒 Dikkat Modu</h3>
    <sub>17+ site engelleme, sistem seviyesinde, süre takibi</sub>
  </td>
  <td align="center">
    <h3>🧠 Yapay Zeka</h3>
    <sub>4 sağlayıcı, 30+ model, Wikipedia, YouTube transkript</sub>
  </td>
  <td align="center">
    <h3>🐍 Python Öğren</h3>
    <sub>61 bölüm, 14.500+ satır, WebKit render</sub>
  </td>
  <td align="center">
    <h3>🐧 Linux Komutları</h3>
    <sub>52 komut, 7 kategori, filtreleme</sub>
  </td>
</tr>
<tr>
  <td align="center">
    <h3>🃏 Flashcards</h3>
    <sub>SM-2 algoritması, AI kart oluşturma, Anki export</sub>
  </td>
  <td align="center">
    <h3>🏆 Rozetler</h3>
    <sub>9 seri seviyesi, 5-365 gün, görsel ilerleme</sub>
  </td>
  <td align="center">
    <h3>📊 Haftalık Analiz</h3>
    <sub>AI destekli özet, öneri, motivasyon</sub>
  </td>
  <td align="center">
    <h3>🧪 PardusLab</h3>
    <sub>Podman konteyner, VTE terminal, komut sandbox</sub>
  </td>
</tr>
</table>

<br>

## 📸 Ekran Görüntüleri

Ekran görüntüleri için [screenshots.html](web/screenshots.html) sayfasını ziyaret edin.

<br>

## 🏗 Mimari

```
pardusedu/
├── src/                          # C++ kaynak kodu
│   ├── main.cpp                  # Uygulama giriş noktası
│   ├── mainwindow.h              # Ana pencere bildirimi (~460 satır, tüm state)
│   ├── ui_setup.cpp              # UI başlatma, sidebar, tema
│   ├── pages.cpp                 # **Tüm sayfalar (~6.400 satır)**
│   ├── data_manager.cpp          # Veri yönetimi (kayıt/yükleme)
│   ├── ai_client.cpp/.h          # AI asistan (multi-provider, async)
│   ├── calc_letter.cpp/.h        # Harf notu hesaplama
│   ├── parduslab.cpp/.h          # PardusLab (Podman + VTE)
│   ├── meson.build               # Kaynak derleme yapılandırması
│
├── assets/                       # UI varlıkları
│   ├── style.css                 # Ana stil dosyası (+800 satır)
│   ├── pardus*.svg               # Pardus logoları
│   ├── send.svg, save.svg, sun.svg, moon.svg
│   ├── badges/                   # Rozet PNG'leri
│   ├── parduslab/                # PardusLab challenge dosyaları
│   └── dersler/                  # Ders içerikleri (markdown)
│
├── docs/python/                  # Python eğitim dokümanı (~14.500 satır)
│
├── tests/                        # Google Test birim testleri
│   ├── test_calc_letter.cpp      # Harf notu sınır testleri (7 test)
│   └── test_extract_ai_reply.cpp # AI JSON ayrıştırma testleri (8 test)
│
├── server/                       # (İsteğe bağlı) Not paylaşım sunucusu
│   └── main.py                   # FastAPI + SQLite + Redis
│
├── web/                          # Web sitesi (landing page)
│
├── resources.gresource.xml       # GResource derleme listesi
├── meson.build                   # Ana derleme yapılandırması
└── LICENCE                       # GPLv3
```

### Kullanılan Teknolojiler

| Katman | Teknoloji |
|--------|-----------|
| **GUI Çatısı** | GTK4 + gtkmm-4.0 (C++ bindings) |
| **Web Görüntüleyici** | WebKitGTK 6.0 |
| **HTTP İstemci** | libcurl (asenkron wrapper ile) |
| **JSON** | nlohmann/json (modern C++) |
| **Markdown** | md4c-html |
| **Test** | Google Test |
| **Derleme** | Meson + Ninja |
| **Dil** | C++17 |
| **Varlık Yönetimi** | GResource (binary'e gömülü) |
| **Konteyner** | Podman (PardusLab) |
| **Terminal** | VTE (GTK4) |
| **QR Kod** | libqrencode |
| **AI Sağlayıcılar** | Groq Cloud, OpenRouter, Google Gemini, Ollama (yerel) |

<br>

## 🚀 Başlangıç

### Bağımlılıklar

<details>
<summary><b>Debian / Ubuntu / Pardus</b></summary>

```bash
sudo apt install build-essential meson ninja-build \
  libgtkmm-4.0-dev libwebkitgtk-6.0-dev \
  libcurl4-openssl-dev nlohmann-json3-dev \
  libmd4c-html-dev libgtest-dev \
  libvte-2.91-gtk4-dev libqrencode-dev
```
</details>

<details>
<summary><b>Arch Linux</b></summary>

```bash
sudo pacman -S base-devel meson ninja gtkmm4 webkitgtk-6.0 \
  curl nlohmann-json md4c gtest vte4 qrencode
```
</details>

### Derleme

```bash
# Yapılandırma
meson setup build

# Derleme
ninja -C build

# Test
ninja -C build test

# Çalıştırma
./build/student-hub
```

<br>

## 🧪 Test Durumu

```
Test Suite                  # Tests   Status
─────────────────────────────────────────────
calc_letter                  7/7      ✅ PASS
extract_ai_reply             8/8      ✅ PASS
─────────────────────────────────────────────
Total                       15/15     ✅ ALL PASS
```

```bash
ninja -C build test
```

<br>

## 📦 Modüller

### 01 — Pano
Kişiselleştirilmiş karşılama, çalışma serisi (streak) sayacı, tamamlanan pomodoro ve odak süresi istatistikleri. Haftalık analiz butonuna tek tıkla erişim.

### 02 — Pomodoro 🕐
Özelleştirilebilir zamanlayıcı (1–120 dk çalışma, 1–30 dk mola). Başlat/Sıfırla/Atla kontrolleri, görsel ilerleme çubuğu, seri takibi ve otomatik kaydetme. Her tamamlanan döngü istatistiklere eklenir.

### 03 — Görev Yöneticisi 📋
Beş kategori (Genel, Ders Çalışma, Proje, Sınav, Ödev) ve üç öncelik seviyesi (Düşük/Orta/Yüksek). Son tarih girişi, tamamlandı işaretleme, silme ve anlık istatistikler.

### 04 — Notlar 📝
Ders bazlı not sistemi: sol panelde ders listesi, sağ panelde zengin metin düzenleyici (**kalın**, *italik*, <ins>altı çizili</ins>, ~~üstü çizili~~, başlıklar). Biçimlendirme araç çubuğu, harf notu hesaplama, otomatik kaydetme, paylaşım desteği.

### 05 — Ders Programı 📅
Pazartesi–Cuma haftalık zaman çizelgesi. Özelleştirilebilir saat dilimleri, ders adı ve konum girişi. Hücreye tıklayarak ekleme/düzenleme/silme, toplu temizleme ve renk kodlu bloklar.

### 06 — Dikkat Modu 🔒
Instagram, Twitter/X, Facebook, TikTok, Reddit, Netflix, Discord, Twitch olmak üzere 17+ siteyi `/etc/hosts` üzerinden sistem seviyesinde engeller. `pkexec` ile güvenli yükseltme, odak süresi sayacı, Pomodoro'dan bağımsız çalışır.

### 07 — Linux Komutları 🐧
52 temel Linux komutu, 7 kategoride detaylı açıklamalar, kullanım örnekleri ve güvenlik uyarıları. Canlı arama filtresi ile anında komut bulma.

**Kapsanan komutlar:** `ls`, `cd`, `pwd`, `cp`, `mv`, `rm`, `mkdir`, `cat`, `grep`, `find`, `chmod`, `chown`, `ps`, `top`, `kill`, `tar`, `wget`, `curl`, `ssh`, `scp`, `df`, `du`, `free`, `uname`, `whoami`, `sudo`, `apt`, `dnf`, `systemctl`, `journalctl`, `man`, `history`, `alias`, `ln`, `diff`, `head`, `tail`, `sort`, `uniq`, `wc`, `awk`, `sed`, `cut`, `ping`, `ip`, `touch`, `tree`, `file`, `stat`, `nslookup`, `dig`, `useradd`

### 08 — Yapay Zeka Asistanı 🧠

Dört farklı sağlayıcı ve 30+ model desteği:

| Sağlayıcı | Örnek Modeller |
|-----------|----------------|
| **Groq Cloud** | `llama-3.3-70b-versatile`, `mixtral-8x7b`, `gemma2-9b` |
| **OpenRouter** | `openai/gpt-4o-mini`, `qwen-2.5-72b`, `claude-3.5-sonnet` |
| **Google Gemini** | `gemini-2.0-flash`, `gemini-1.5-pro` (Google Search entegre) |
| **Ollama** | `llama3.2`, `mistral` (yerel, internet gerektirmez) |

**Özellikler:**
- 🔍 **DuckDuckGo arama** — toggle ile anlık web araştırması
- 📺 **YouTube transkript** — video bağlantısı gönderince otomatik ders notu
- 📝 **Not CRUD** — AI ile not ekleme/güncelleme/silme
- 💬 **Konuşma geçmişi** — bağlam koruma, token yönetimi
- 🌓 **Wikipedia entegrasyonu**
- ⚡ **Asenkron çağrılar** — UI bloklamaz
- 🎯 **Haftalık analiz** — çalışma verilerini AI ile yorumlama

### 09 — Python Öğren 🐍
WebKit tabanlı, uygulama içi Python eğitimi. 61 bölüm, ~14.500 satır kapsamlı doküman. Markdown'dan HTML'ye anında dönüşüm (md4c), karanlık/aydınlık tema desteği, bölümler arası gezinme.

**Konular:** Temel veri tipleri → Kontrol yapıları → Fonksiyonlar → OOP → NumPy/Pandas → Flask/FastAPI → Asenkron programlama → Test yazma → Veritabanları

### 🎯 Bonus Modüller

#### 🃏 Flashcards (SM-2 Algoritması)
Tam spaced repetition sistemi: SM-2 tabanlı önceliklendirme, AI ile kart oluşturma, 0-5 arası derecelendirme, Anki TSV export, zayıf kart analizi.

#### 🏆 Rozetler
9 seri seviyesi: 5 → 10 → 15 → 21 → 30 → 50 → 100 → 200 → 365 gün. Her seviyede görsel rozet.

#### 📊 Haftalık Analiz
Pomodoro, görev, flashcard ve odak verilerini toplar, AI ile özet/öneri/motivasyon oluşturur. Zayıf kartları belirler, çalışma oturumu başlatır.

#### 🧪 PardusLab
Podman konteyner yönetimi: VTE terminal emülatörü, çoklu kullanıcı desteği, komut sandbox, challenge sistemi. `pkexec` ile güvenli yükseltme.

<br>

## 🔧 Geliştirme

```bash
# Temiz yapılandırma
meson setup build --wipe

# Derleme (değişiklik sonrası)
ninja -C build

# Test
ninja -C build test

# Binary'yi strip et (boyut küçültme)
strip build/student-hub
```

### Proje Konvansiyonları

| Kural | Açıklama |
|-------|----------|
| **Dil** | C++17, GTKMM 4.0 |
| **Build** | Meson + Ninja |
| **Test** | Google Test (her yeni özellik için test zorunlu) |
| **Stil** | Anlamlı değişken isimleri (Türkçe/İngilizce), yorumlu kritik bölümler |
| **Kayıt** | Veriler XDG dizininde `.dat` formatında, `data_manager` üzerinden |

<br>

## 🤝 Katkı

Katkılarınızı memnuniyetle karşılıyoruz!

1. Depoyu fork edin
2. Yeni bir branch oluşturun (`git checkout -b ozellik/yeni-ozellik`)
3. Değişikliklerinizi yapın
4. Testleri çalıştırın (`ninja -C build test`)
5. Commit gönderin (`git commit -m 'feat: yeni özellik eklendi'`)
6. Branch'inizi push edin (`git push origin ozellik/yeni-ozellik`)
7. Pull Request açın

<br>

## 📜 Lisans

Bu proje **GNU General Public License v3.0** ile lisanslanmıştır.  
Detaylar için [LICENCE](LICENCE) dosyasına bakın.

<br>

## 🔗 Bağlantılar

<div align="center">

[![TEKNOFEST](https://img.shields.io/badge/TEKNOFEST-2026-8A2BE2?style=flat-square)](https://teknofest.org)
[![Pardus](https://img.shields.io/badge/Pardus-Linux-FF6600?style=flat-square)](https://www.pardus.org.tr)
[![Groq Cloud](https://img.shields.io/badge/Groq-Cloud-00E676?style=flat-square)](https://console.groq.com)
[![OpenRouter](https://img.shields.io/badge/OpenRouter-AI-FF6B6B?style=flat-square)](https://openrouter.ai)
[![GTK](https://img.shields.io/badge/GTK-4.0-4A90D9?style=flat-square)](https://www.gtk.org)
[![Meson](https://img.shields.io/badge/Meson-Build-00A6C0?style=flat-square)](https://mesonbuild.com)

</div>

<br>

---

<div align="center">
  <img src="assets/pardus_symbol.svg" width="32" height="32">
  <br>
  <b>PardusEdu</b> — <em>Öğrenciler için tasarlandı, Pardus için geliştirildi</em>
  <br>
  <sub>
    Geliştirici: <a href="https://github.com/coderianx">Coderian</a>
    &nbsp;·&nbsp; &copy; 2026
    &nbsp;·&nbsp; TEKNOFEST Pardus Öneri ve Geliştirme Kategorisi
  </sub>
</div>
