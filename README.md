# PardusEdu — Öğrenci Merkezi

[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](LICENCE)
[![C++17](https://img.shields.io/badge/C++-17-00599C?logo=c%2B%2B)](https://en.cppreference.com/w/cpp/17)
[![GTK4](https://img.shields.io/badge/GTK-4.0-4A90D9?logo=gtk)](https://www.gtk.org/)
[![Platform](https://img.shields.io/badge/Platform-Linux-FF6600?logo=linux)](https://www.linux.org/)
[![Build](https://img.shields.io/badge/Build-Meson%20%2B%20Ninja-00A6C0)](https://mesonbuild.com/)

> **TEKNOFEST** Pardus Öneri ve Geliştirme Kategorisi — GTK4 masaüstü uygulaması

---

## İçindekiler

- [Hakkında](#hakkında)
- [Özellikler](#özellikler)
- [Ekran Görüntüleri](#ekran-görüntüleri)
- [Mimari](#mimari)
- [Başlangıç](#başlangıç)
- [Kullanım](#kullanım)
- [Modüller](#modüller)
- [Testler](#testler)
- [Katkı](#katkı)
- [Lisans](#lisans)

---

## Hakkında

**PardusEdu**, Pardus Linux için geliştirilmiş, öğrencilerin tüm akademik ihtiyaçlarını tek bir çatı altında toplayan açık kaynaklı bir GTK4 masaüstü uygulamasıdır. Tamamen yerel çalışır, internet bağlantısı yalnızca yapay zeka asistanı için gereklidir.

Proje; **Coderian** tarafından geliştirilmektedir.

---

## Özellikler

### 🎯 9 Entegre Modül

| # | Modül | Açıklama |
|---|-------|----------|
| 01 | **Pano** | Kişisel karşılama, çalışma serisi (streak) ve hızlı navigasyon |
| 02 | **Pomodoro** | Ayarlanabilir süre/mola, sesli bildirim, seri takibi |
| 03 | **Görev Yöneticisi** | Kategori, öncelik, son tarih ile görev takibi; tamamlama istatistikleri |
| 04 | **Notlar** | Ders bazlı metin düzenleyici, harf notu hesaplama, otomatik kaydetme |
| 05 | **Ders Programı** | Haftalık zaman çizelgesi, özelleştirilebilir saat dilimleri, konum bilgisi |
| 06 | **Dikkat Modu** | 17+ sosyal medya sitesini sistem seviyesinde engelleme, odak süresi takibi |
| 07 | **Linux Komutları** | 25+ komut, 7 kategoride detaylı açıklama, örnekler ve filtreleme |
| 08 | **Yapay Zeka** | Groq API ile 8 farklı model, Wikipedia entegrasyonu, konuşma geçmişi |
| 09 | **Python Öğren** | 60+ bölüm, ~15.000 satır kapsamlı Python eğitimi (WebKit görüntüleyici) |

### 💡 Teknik Öne Çıkanlar

- **Karanlık/Aydınlık tema** — CSS değişken replasmanı ile anında tema geçişi
- **Yerel veri saklama** — Tüm kullanıcı verileri XDG dizininde `.dat` formatında saklanır
- **GResource gömülü varlıklar** — SVG ikonlar ve CSS binary içine gömülüdür
- **pkexec ile sistem işlemleri** — Dikkat modu `/etc/hosts` düzenlemesi için güvenli yükseltme
- **md4c ile Markdown işleme** — Python ders dokümanları anında HTML'ye dönüştürülür
- **nlohmann/json** — Modern C++ JSON parsing

---

## Ekran Görüntüleri

Ekran görüntüleri için [screenshots.html](web/screenshots.html) sayfasını ziyaret edin.

---

## Mimari

```
pardus_edu/
├── src/
│   ├── main.cpp
│   ├── mainwindow.h
│   ├── mainwindow.cpp
│   ├── ui_setup.cpp
│   ├── pages.cpp
│   ├── data_manager.cpp
│   ├── calc_letter.h
│   ├── calc_letter.cpp
│   ├── ai_client.h
│   └── ai_client.cpp
├── assets/
│   ├── style.css
│   ├── pardus.svg
│   ├── pardus_symbol.svg
│   ├── sun.svg
│   ├── moon.svg
│   └── save.svg
├── tests/
│   ├── meson.build
│   ├── test_calc_letter.cpp
│   └── test_extract_ai_reply.cpp
├── meson.build
└── LICENCE
```

### Kullanılan Teknolojiler

| Katman | Teknoloji |
|--------|-----------|
| **GUI** | GTK4 + gtkmm-4.0 (C++ binding) |
| **Web** | WebKitGTK 6.0 |
| **HTTP** | libcurl |
| **JSON** | nlohmann/json |
| **Markdown** | md4c-html |
| **Test** | Google Test |
| **Derleme** | Meson + Ninja |
| **Dil** | C++17 |
| **Varlık** | GResource (binary'e gömülü) |
| **API** | Groq Cloud (OpenAI uyumlu) |

---

## Başlangıç

### Bağımlılıklar

Debian/Ubuntu tabanlı sistemler:

```bash
sudo apt install build-essential meson ninja-build \
  libgtkmm-4.0-dev libwebkitgtk-6.0-dev \
  libcurl4-openssl-dev nlohmann-json3-dev \
  libmd4c-html-dev libgtest-dev
```

Pardus (Debian tabanlı):

```bash
sudo apt install build-essential meson ninja-build \
  libgtkmm-4.0-dev libwebkitgtk-6.0-dev \
  libcurl4-openssl-dev nlohmann-json3-dev \
  libmd4c-html-dev libgtest-dev
```

Arch Linux:

```bash
sudo pacman -S base-devel meson ninja gtkmm4 webkitgtk-6.0 \
  curl nlohmann-json md4c gtest
```

### Derleme

```bash
# Yapılandırma
meson setup build

# Derleme
ninja -C build

# Çalıştırma
./build/student-hub
```

### Test

```bash
ninja -C build test
```

veya doğrudan:

```bash
./build/test_calc_letter
./build/test_extract_ai
```

---

## Kullanım

### İlk Çalıştırma

Uygulama ilk açıldığında sizi bir karşılama ekranı karşılar. Sol kenar çubuğundan tüm modüllere erişebilirsiniz. Veriler varsayılan olarak `~/.local/share/ogrenci-merkezi/` dizininde saklanır.

### Kısayollar

| Tuş | İşlev |
|-----|-------|
| **Enter** | AI sohbetinde mesaj gönder |
| **Shift+Enter** | AI sohbetinde yeni satır |
| **Tema butonu** | Karanlık/Aydınlık tema geçişi (üst çubuk) |
| **Kaydet butonu** | Tüm verileri manuel kaydet (üst çubuk) |

### AI Asistan API Kurulumu

1. [Groq Cloud](https://console.groq.com) hesabı oluşturun
2. API anahtarı alın
3. Uygulamada "Yapay Zeka" sayfasındaki anahtar butonuna tıklayın
4. API anahtarını girin ve model seçin

---

## Modüller

### 01 — Pano

Uygulama ana sayfası. Kullanıcı adınızla kişiselleştirilmiş karşılama mesajı, Pardus logosu ve çalışma seriniz (streak) görüntülenir.

### 02 — Pomodora

Zaman yönetimi aracı. Varsayılan 25 dakika çalışma / 5 dakika mola döngüsüyle çalışır.

- **Başlat/Duraklat** — Zamanlayıcıyı kontrol eder
- **Sıfırla** — Geçerli oturumu başa döndürür
- **Atla** — Doğrudan mola/çalışma fazına geçer
- **Süre ayarları** — Çalışma (1-120 dk) ve mola (1-30 dk) sürelerini değiştirin
- **İlerleme çubuğu** — Kalan süreyi görsel olarak gösterir
- Her tamamlanan döngü serinize eklenir ve otomatik kaydedilir

### 03 — Görevler

Ödev, sınav, proje takibi için görev yönetim sistemi.

- 5 kategori: Genel, Ders Çalışma, Proje, Sınav, Ödev
- 3 öncelik seviyesi: Düşük (yeşil), Orta (sarı), Yüksek (kırmızı)
- Son tarih girişi (GG.AA.YYYY formatı)
- Tamamlandı işaretleme (üzeri çizili gösterim)
- Görev silme
- Canlı istatistik: "X bekliyor, Y tamamlandı"

### 04 — Notlar

Ders bazlı not alma sistemi.

- Sol panelde ders listesi
- Sağ panelde zengin metin düzenleyici
- Ders ekleme/silme
- Otomatik kaydetme

### 05 — Ders Programı

Haftalık ders takvimi (Pazartesi-Cuma).

- Grid tabanlı zaman çizelgesi
- Özelleştirilebilir saat dilimleri
- Ders adı ve konum (derslik) girişi
- Hücreye tıkla: ders ekle/düzenle/sil
- Toplu temizleme
- Renklendirilmiş ders blokları

### 06 — Dikkat Modu

Sosyal medya ve dikkat dağıtıcı siteleri engelleme.

Engellenen siteler: Instagram, Twitter/X, Facebook, TikTok, Reddit, Netflix, Discord (.com & .gg), Twitch, YouTube

- Sistem seviyesinde (`/etc/hosts` üzerinden) çalışır
- `pkexec` ile güvenli yükseltme
- Çalışma süresi takibi (saniye sayacı)
- Pomodoro ile bağımsız veya birlikte kullanılabilir
- Kapatılınca engeller otomatik kaldırılır

### 07 — Linux Komutları

25+ temel Linux komutu, 7 kategoride detaylı açıklamalarla.

Kapsanan komutlar: `ls`, `cd`, `pwd`, `cp`, `mv`, `rm`, `mkdir`, `cat`, `grep`, `find`, `chmod`, `chown`, `ps`, `top`, `kill`, `tar`, `wget`, `curl`, `ssh`, `scp`, `df`, `du`, `free`, `uname`, `apt`

Her komut için:
- Kullanım örneği
- Tüm seçenekler
- Detaylı açıklama
- Güvenlik uyarıları (`rm -rf /` gibi)

Canlı arama filtresi ile komut bulma.

### 08 — Yapay Zeka Asistanı

Groq Cloud API üzerinden çalışan yapay zeka sohbet asistanı.

**Desteklenen Modeller:**

| Model | Parametre | Hız | Kullanım |
|-------|-----------|-----|----------|
| Llama 3.3 70B | 70B | Orta | Genel amaçlı, kaliteli cevap |
| Llama 3.1 8B | 8B | Çok hızlı | Basit sorular, günlük sohbet |
| Llama 4 Scout 17B | 17B | Hızlı | Planlama, araştırma |
| GPT OSS 20B | 20B | Hızlı | Günlük ders çalışma |
| GPT OSS 120B | 120B | Yavaş | Karmaşık problemler, kod analizi |
| GPT OSS SafeGuard 20B | 20B | Hızlı | Güvenli sohbet, kontrollü kullanım |
| Qwen 3 32B | 32B | Orta | Kodlama, matematik, teknik konular |
| Groq Compound | — | Çok hızlı | Gerçek zamanlı, akıcı sohbet |

**Özellikler:**
- Wikipedia arama entegrasyonu (Türkçe → İngilizce fallback)
- Konuşma geçmişi (son 6 mesaj)
- API anahtarını kendiniz belirleyin
- Not düzenleme komutları (AI ile not ekleme/güncelleme/silme)
- Markdown temizleme
- `/tmp/ai_response.log` ile hata ayıklama
- Önbellekli Wikipedia yanıtları

### 09 — Python Öğren

WebKit tabanlı, uygulama içi Python eğitimi.

- 60+ bölüm, ~15.000 satır kapsamlı doküman
- Konular: Temel veri tipleri, değişkenler, string işlemleri, kontrol yapıları, döngüler, listeler, demetler, sözlükler, kümeler, fonksiyonlar, lambda, comprehension, dosya işlemleri, hata yönetimi, loglama, OOP, NumPy, Pandas, Matplotlib, Flask, FastAPI, asenkron programlama, test yazma, veritabanları
- Markdown → HTML dönüşümü (md4c)
- Tema duyarlı (karanlık/aydınlık moda otomatik uyum)
- Sol kenar çubuğu ile bölümler arası gezinti

---

## API Referansı

### AI Asistan (Groq Cloud)

```
POST https://api.groq.com/openai/v1/chat/completions
Authorization: Bearer <api_key>
```

İstek gövdesi:

```json
{
  "model": "llama-3.3-70b-versatile",
  "temperature": 0.4,
  "top_p": 0.9,
  "messages": [
    {"role": "system", "content": "..."},
    {"role": "user", "content": "..."}
  ]
}
```

### Wikipedia Entegrasyonu

```
GET https://{lang}.wikipedia.org/api/rest_v1/page/summary/{title}
```

AI, kullanıcı "nedir", "kimdir", "araştır" gibi kelimeler kullandığında otomatik olarak Wikipedia'dan bağlam getirir.

---

## Testler

Proje **Google Test** ile test edilmiştir.

```bash
# Tüm testleri çalıştır
ninja -C build test

# Veya doğrudan:
./build/test_calc_letter
./build/test_extract_ai
```

| Test | Dosya | Kapsam |
|------|-------|--------|
| **calc_letter** | `tests/test_calc_letter.cpp` | Harf notu hesaplama (A, B, C, D, F sınırları) |
| **extract_ai_reply** | `tests/test_extract_ai_reply.cpp` | Groq API JSON cevabı ayrıştırma |

---

## Geliştirme

### Proje Yapısı

```bash
meson setup build --wipe     # Temiz yapılandırma
ninja -C build               # Derleme
ninja -C build test          # Test
```

### Yeni Sayfa Ekleme

1. `src/mainwindow.h` içinde yeni bir `setup_` metodu bildirin
2. `pages.cpp` içinde sayfa arayüzünü oluşturun
3. `ui_setup.cpp` içinde `setup_pages()` ve `add_item()` çağrılarını ekleyin
4. Sayfanın widget'ını `stack.add(*widget, "id")` ile kaydedin

### Yeni Model Ekleme

`ai_client.cpp` içinde `global_model` değişkenini güncelleyin veya `set_model()` çağrısı ile değiştirin. Kullanıcı arayüzü ayrıca model seçme iletişim kutusu içerir.

---

## Katkı

Katkılarınızı memnuniyetle karşılıyoruz!

1. Bu depoyu fork edin
2. Yeni bir branch oluşturun (`git checkout -b ozellik/yeni-ozellik`)
3. Değişikliklerinizi yapın
4. Testleri çalıştırın (`ninja -C build test`)
5. Commit gönderin (`git commit -m 'feat: yeni özellik eklendi'`)
6. Branch'inizi push edin (`git push origin ozellik/yeni-ozellik`)
7. Pull Request açın

### Kod Stili

- C++17 standartları
- GTKMM 4.0 API kullanımı
- Anlamlı değişken ve fonksiyon isimleri (Türkçe/İngilizce)
- Google Test ile test kapsamı
- Meson build sistemi

---

## Lisans

Bu proje **GNU General Public License v3.0** ile lisanslanmıştır. Detaylar için [LICENCE](LICENCE) dosyasına bakın.

---

## Bağlantılar

- [TEKNOFEST](https://teknofest.org)
- [Pardus](https://www.pardus.org.tr)
- [Groq Cloud](https://console.groq.com)
- [GTK](https://www.gtk.org)
- [Meson Build](https://mesonbuild.com)

---

<p align="center">
  <img src="assets/pardus_symbol.svg" width="32" height="32">
  <br>
  <strong>PardusEdu</strong> — <em>Öğrenciler için tasarlandı</em>
  <br>
  <sub>Geliştirici: Coderian &middot; &copy; 2026</sub>
</p>
