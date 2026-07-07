# Python Öğren

## Bölümler:
- [1. Python Nedir](#python-nedir)
- [2. Python Kurulumu ve Çalıştırma](#python-kurulumu-ve-calistirma)
- [3. Temel Veri Tipleri](#temel-veri-tipleri)
- [4. Değişkenler ve Operatörler](#degiskenler-ve-operatorler)
- [5. String İşlemleri](#string-islemleri)
- [6. Kontrol Yapıları](#kontrol-yapilari)
- [7. Döngüler](#donguler)
- [8. Listeler](#listeler)
- [9. Demetler (Tuple)](#demetler-tuple)
- [10. Sözlükler (Dict)](#sozlukler-dict)
- [11. Kümeler (Set)](#kumeler-set)
- [12. Fonksiyonlar](#fonksiyonlar)
- [13. Lambda ve Fonksiyonel Programlama](#lambda-ve-fonksiyonel-programlama)
- [14. Liste ve Sözlük Kısayolları (Comprehensions)](#liste-ve-sozluk-kisayollari)
- [15. Dosya İşlemleri](#dosya-islemleri)
- [16. Hata Yönetimi (Exception Handling)](#hata-yonetimi)
- [17. Loglama (logging)](#logging)
- [18. Modüller ve Paketler](#moduller-ve-paketler)
- [19. Tarih ve Zaman İşlemleri](#tarih-ve-zaman-islemleri)
- [20. JSON ve Veri Serileştirme](#json-ve-veri-serilestirme)
- [21. Düzenli İfadeler (Regex)](#duzenli-ifadeler)
- [22. Nesne Yönelimli Programlama (OOP)](#nesne-yonelimli-programlama)
- [23. Miras Alma (Inheritance)](#miras-alma)
- [24. Veri Sınıfları (dataclasses) ve Enum](#veri-siniflari-ve-enum)
- [25. Dekoratörler (Decorators)](#dekoratorler)
- [26. Jeneratörler ve Iteratorler](#jeneratorler-ve-iteratorler)
- [27. Bağlam Yöneticileri (Context Managers)](#baglam-yoneticileri)
- [28. threading ve multiprocessing](#threading-ve-multiprocessing)
- [29. Asenkron Programlama (asyncio)](#asenkron-programlama)
- [30. Veritabanı İşlemleri (SQLite)](#veritabani-islemleri)
- [31. Web İstekleri (requests)](#web-istekleri)
- [32. unittest ile Test Yazma](#unittest-ile-test-yazma)
- [33. Sanal Ortamlar ve pip](#sanal-ortamlar-ve-pip)
- [34. Basit Python Örneği - Öğrenci Yönetim Sistemi](#basit-python-ornegi)
- [35. Pythonic Kod ve PEP 8](#pythonic-kod)
- [36. Performans İpuçları](#performans-ipuclari)
- [37. Proje Yapısı ve Organizasyon](#proje-yapisi)
- [38. Web Scraping](#web-scraping)
- [39. Flask Web Framework](#flask)
- [40. FastAPI](#fastapi)
- [41. Django Web Framework](#django)
- [42. İleri Adımlar](#ileri-adimlar)
- [43. subprocess ile Sistem Komutları](#subprocess)
- [44. concurrent.futures (Paralel İşlem)](#concurrent-futures)
- [45. hashlib, secrets ve uuid (Güvenlik)](#hashlib-secrets-uuid)
- [46. Matematik Modülleri (math, random, statistics)](#matematik-modulleri)
- [47. collections ile Gelişmiş Veri Yapıları](#collections-modulu)
- [48. functools İleri Düzey](#functools-ileri)
- [49. zoneinfo ve Arşiv/Sıkıştırma Modülleri](#zoneinfo-arsiv)
- [50. Socket Programlama (TCP/UDP)](#socket-programlama)
- [51. io, struct ve base64 (Binary Veri)](#io-struct-base64)
- [52. sys ve os Modülleri Detaylı](#sys-os-detayli)
- [53. configparser, shelve, calendar (Yardımcı Modüller)](#yardimci-moduller)
- [54. inspect, traceback, pprint (Hata Ayıklama)](#hata-ayiklama-araclari)
- [55. heapq, bisect, array (Veri Yapıları)](#veri-yapilari-algoritma)
- [56. Design Patterns (Tasarım Desenleri)](#design-patterns)
- [57. Python 3.12/3.13/3.14 Yenilikleri](#python-yenilikler)
- [58. doctest, httpx, string ve Diğer Araçlar](#diger-araclar)
- [59. Pydantic ile Veri Doğrulama](#pydantic)
- [60. pytest ile İleri Test Teknikleri](#pytest-ileri)
- [61. NumPy ile Sayısal Hesaplama](#numpy)
- [62. Pandas ile Veri Analizi](#pandas)

---

## Bu Dokümanı Nasıl Kullanmalısınız?

Bu rehber 62 bölümden oluşur. **Sakın hepsini sırayla okumaya kalkmayın!** İşte size özel okuma planı:

### 🟢 Tamamen Yeni Başlayan Biriyseniz
1 → 2 → 3 → 4 → 5 → 6 → 7 → 8 → 9 → 10 → 11 → 12 (sadece giriş) → 15 → 16 → 18 → 22 → 34

Bu rotayı bitirdiğinizde Python'un temellerini öğrenmiş olacaksınız. Sonra 13, 14, 17, 19, 20, 21, 23, 24, 25 ile devam edin.

### 🔵 Biraz Deneyiminiz Varsa
3 → 4 → 5 → 6 → 7 → 8 → 9 → 10 → 11 → 12 → 13 → 14 → 15 → 16 → 22 → 23 → 33 → 34

### 🟣 Programlama Biliyor, Python'a Geçiyorsanız
Hızla 3-4-5-6-7'yi tazeleyin, 12-15-16-22-33'e odaklanın, sonra ilginizi çeken konulara atlayın.

### Her Bölümde Bunu Yapın
1. **Kodu elle yazın** — kopyala/yapıştır YAPMAYIN, kendiniz yazın
2. **Çalıştırın** — çıktıyı görün
3. **Değiştirin** — sayıları, metinleri değiştirip tekrar çalıştırın
4. **Alıştırmaları çözün** — her bölüm sonundaki soruları yapmadan geçmeyin

Python öğrenmenin tek yolu **yazarak ve deneyerek** öğrenmektir. Bol bol hata yapın, hatalar arkadaşınızdır!

---

## <a id="python-nedir"></a>**Python** Nedir:

Haydi ilk iş olarak Python ile tanışalım. Ama önce teoriyle boğulmayın — şimdilik kısa tutuyorum:

**Python**, öğrenmesi kolay, okuması rahat ve her işe yarayan bir programlama dilidir. 1991 yılında Guido van Rossum tarafından yaratılmıştır.

Python'ı diğer dillerden ayıran şeyler:
- **Basit ve okunabilir**: İngilizce gibi okunur, gereksiz semboller yoktur
- **Yorumlanabilir**: Yazdığınız kodu hemen çalıştırabilirsiniz, derleme beklemeyin
- **Çok yönlü**: Web sitesi, oyun, hesap makinesi, yapay zeka — aklınıza ne gelirse
- **Geniş kütüphane desteği**: Her şey için hazır paket var (PyPI'da 500.000+ paket)

**Python nerelerde kullanılır?**
- Web geliştirme (Django, Flask, FastAPI)
- Veri bilimi ve yapay zeka (Pandas, TensorFlow, PyTorch)
- Otomasyon ve script yazma
- Masaüstü uygulamaları
- Oyun geliştirme
- Bilimsel hesaplama

Şimdi bunları okuyup geçmeyin — **bir sonraki bölüme atlayın ve ilk Python programınızı yazın!** Okumak yetmez, yazmak gerekir.

## <a id="python-kurulumu-ve-calistirma"></a>Python Kurulumu ve İlk Programın

> ⚠️ **Bu bölümü atlamayın!** Python'u kurmadan ve ilk programınızı çalıştırmadan sonraki bölümlere geçmeyin.

### Python'u Kurma

İşletim sisteminize göre aşağıdaki adımları izleyin:

- **Linux (Debian/Ubuntu/Pardus)**:
  ```bash
  sudo apt update
  sudo apt install python3 python3-pip
  ```

- **macOS** (Homebrew ile):
  ```bash
  brew install python3
  ```

- **Windows**: [python.org](https://python.org) adresinden Python'un son sürümünü indirin. **Kurulumda "Add Python to PATH" seçeneğini işaretleyin.**

Kurduktan sonra terminalinizde şunu yazıp çalıştığını kontrol edin:
```bash
python3 --version
```
Şuna benzer bir çıktı görmelisiniz: `Python 3.12.0`

Eğer hata alıyorsanız, kurulumu tekrarlayın veya "python3" yerine "python" deneyin.

### Kod Editörü Kurulumu

Kod yazmak için bir metin editörüne ihtiyacınız var. İşte en popüler seçenekler:

| Editör | Ne zaman tercih edilmeli |
|--------|------------------------|
| **VS Code** (tavsiye edilir) | Ücretsiz, eklenti desteği, her işletim sisteminde çalışır |
| **PyCharm Community** | Python'a özel, güçlü araçlar |
| **IDLE** | Python ile birlikte gelir, basit ve hafif |

**VS Code için**: [code.visualstudio.com](https://code.visualstudio.com/) adresinden indirin. Kurduktan sonra:
1. Sol menüden "Extensions" (Ctrl+Shift+X) butonuna tıklayın
2. "Python" yazın ve Microsoft'un Python eklentisini kurun

### İlk Python Programınız

Şimdi gerçek bir program yazma zamanı! Adım adım yapalım:

**1. Adım**: VS Code'u (veya seçtiğiniz editörü) açın.

**2. Adım**: Yeni bir dosya oluşturun ve `merhaba.py` olarak kaydedin.

**3. Adım**: Şu kodları dosyaya yazın:
```python
print("Merhaba, Dünya!")
print("Python'u öğrenmeye başladım!")
```

**4. Adım**: Terminalde şu komutu çalıştırın:
```bash
python3 merhaba.py
```

**5. Adım**: Çıktıyı görün:
```
Merhaba, Dünya!
Python'u öğrenmeye başladım!
```

🎉 **Tebrikler! İlk Python programınızı yazdınız ve çalıştırdınız!**

Şimdi dosyayı biraz değiştirin:
```python
print("Merhaba, Dünya!")
print("Python'u öğrenmeye başladım!")
print("Bugün", 2026, "yılının", "Temmuz", "ayındayız.")
print("2 + 3 =", 2 + 3)
print("3 * 7 =", 3 * 7)
```

Tekrar çalıştırın ve çıktıyı görün. **Gördünüz mü?** Python sizin için işlemleri yaptı!

### Python'u Çalıştırma Yöntemleri

#### Yöntem 1: Python dosyasını çalıştırma
```bash
python3 dosya.py
```
En yaygın yöntem budur. Bir `.py` dosyası yazıp çalıştırırsınız.

#### Yöntem 2: Etkileşimli kabuk (REPL)
```bash
python3
```
Çıktı:
```
Python 3.12.0 (default, Oct 16 2023, 12:00:00)
[GCC 12.2.0] on linux
Type "help", "copyright", "credits" or "license" for more information.
>>>
```
`>>>` işaretinin yanına Python kodu yazıp Enter'a basın — sonuç hemen karşınızda:
```python
>>> print("Merhaba Python")
Merhaba Python
>>> 2 + 3
5
>>> 10 * 5
50
>>> exit()   # Çıkmak için
```

REPL, küçük denemeler yapmak için harikadır. Ama gerçek programlar için dosya kullanın.

#### Yöntem 3: Çalıştırılabilir script (Linux/macOS)
Dosyanın en üstüne şunu ekleyin:
```python
#!/usr/bin/env python3
print("Merhaba Dünya")
```
Sonra dosyayı çalıştırılabilir yapın:
```bash
chmod +x dosya.py
./dosya.py
```

### Yorum Satırları

Kod yazarken açıklama eklemek için yorum satırları kullanılır. Python `#` işaretinden sonrasını yorum olarak kabul eder ve çalıştırmaz.

```python
# Bu tek satırlık bir yorumdur
# Programın ne yaptığını açıklamak için kullanılır

"""
Üç tırnak ile çok satırlı yorum
yazabilirsiniz. Buna docstring denir
ve fonksiyonları/sınıfları belgelemek için kullanılır.
"""

print("Kod çalışıyor")  # Satır sonu yorum
```

### Bu Bölümde Öğrendikleriniz
- ✅ Python'u kurdunuz
- ✅ Bir kod editörü seçtiniz
- ✅ İlk Python programınızı yazdınız
- ✅ Python dosyasını çalıştırmayı öğrendiniz
- ✅ REPL'de denemeler yaptınız
- ✅ Yorum satırlarını öğrendiniz

### 🛠️ Alıştırmalar

**Temel (Yapmadan geçmeyin!):**
1. `merhaba.py` dosyanıza kendi adınızı ve yaşınızı yazdıran bir satır ekleyin: `print("Benim adım ..., ... yaşındayım")`
2. REPL'de 15 * 8 işlemini yapın, sonucu görün
3. Yeni bir `hesap.py` dosyası oluşturun, içine 4 işlem (+, -, *, /) yaptıran kodlar yazın ve çalıştırın

**Orta:**
4. Python'un hesap makinesi gibi çalıştığını göstermek için bir program yazın: önce iki sayının toplamını, sonra farkını, sonra çarpımını, sonra bölümünü tek tek `print` ile yazdırın

**İleri:**
5. REPL'de `import this` yazın. The Zen of Python çıktısını görün. En beğendiğiniz 3 maddeyi bir dosyaya yorum satırı olarak yazın.

## <a id="temel-veri-tipleri"></a>Temel Veri Tipleri

> Bu bölümde sayılar, metinler ve mantıksal değerlerle tanışacaksınız. Bunlar Python'da en sık kullanacağınız veri türleridir.

Python'da her değerin bir türü vardır. Şimdilik üç temel türü öğreneceğiz:

| Tür | Örnek | Açıklama |
|-----|-------|----------|
| `int` | `42`, `-5`, `0` | Tam sayı |
| `float` | `3.14`, `-0.5` | Ondalıklı sayı |
| `str` | `"merhaba"`, `'python'` | Metin (string) |

(Sonraki bölümlerde bool, list, dict gibi diğer türleri göreceksiniz.)

### int (Tam Sayı)

```python
ogrenci_sayisi = 25
yil = 2026
sifir = 0
negatif = -10
buyuk_sayi = 1_000_000  # Alt çizgi okunabilirlik içindir, değeri 1000000
```

### float (Ondalıklı Sayı)

```python
pi = 3.14
sicaklik = -2.5
bilimsel = 1e3        # 1000.0 (1 çarpı 10 üzeri 3)
kuvvet = 2.5e-4       # 0.00025
```

### str (Metin / String)

Metinler tek tırnak (`'`) veya çift tırnak (`"`) ile yazılır:

```python
isim = "Ali"
soyisim = 'Yılmaz'
mesaj = "Merhaba, bugün hava çok güzel!"
```

Uzun metinler için üç tırnak kullanabilirsiniz:

```python
siir = """
Korkma! Sönmez bu şafaklarda yüzen al sancak,
Sönmeden yurdumun üstünde tüten en son ocak.
"""
```

### type() ile Tür Kontrolü

Bir değerin türünü öğrenmek için `type()` kullanılır:

```python
print(type(42))         # <class 'int'>
print(type(3.14))       # <class 'float'>
print(type("merhaba"))  # <class 'str'>
```

### Tür Dönüşümleri

Bir türü başka bir türe çevirebilirsiniz:

```python
# int -> float
x = float(5)        # 5.0

# float -> int (kesir kısmı atılır!)
y = int(3.14)       # 3
z = int(-3.14)      # -3

# string -> int/float (sayısal metinler için)
a = int("42")       # 42
b = float("3.14")   # 3.14

# sayı -> string
c = str(123)        # "123"

# Tür kontrolü
print(type(42))
print(isinstance(42, int))              # True
print(isinstance(42, (int, float)))      # True
```

### 🛠️ Alıştırmalar

**Temel:**
1. `merhaba.py` dosyanıza kendi adınızı ve yaşınızı yazdıran bir satır ekleyin: `print("Benim adım ..., ... yaşındayım")`
2. REPL'de 15 * 8 işlemini yapın, sonucu görün
3. Yeni bir `hesap.py` dosyası oluşturun, içine 4 işlem (+, -, *, /) yaptıran kodlar yazın ve çalıştırın

**Orta:**
4. Python'un hesap makinesi gibi çalıştığını göstermek için bir program yazın: önce iki sayının toplamını, sonra farkını, sonra çarpımını, sonra bölümünü tek tek `print` ile yazdırın

**İleri:**
5. REPL'de `import this` yazın. The Zen of Python çıktısını görün. En beğendiğiniz 3 maddeyi bir dosyaya yorum satırı olarak yazın.

---

## <a id="degiskenler-ve-operatorler"></a>Değişkenler ve Operatörler

> Bu bölümde değişkenlere değer atamayı ve bu değerler üzerinde işlem yapmayı öğreneceksiniz.

### Değişken Nedir?

Değişken, bir değeri saklamak için kullandığınız bir "kutu" gibidir. İstediğiniz ismi verebilir, istediğiniz değeri koyabilirsiniz.

### Değişken Tanımlama

Python'da değişken tanımlamak çok kolaydır — tür belirtmenize gerek yoktur:

```python
# Değişkene değer atama
isim = "Ali"
yas = 25
boy = 1.78
ogrenci_mi = True

print(isim)    # Ali
print(yas)     # 25
```

**Değişken isimlendirme kuralları:**
- Harf, rakam ve alt çizgi (`_`) kullanılabilir
- Sayı ile **başlayamaz** (örn: `1isim` ❌, `isim1` ✅)
- Büyük/küçük harf duyarlıdır (`isim` ile `Isim` farklıdır)
- Python anahtar kelimeleri kullanılamaz (`if`, `else`, `for`, `while`, `class` vb.)

```python
# Doğru isimler
ogrenci_sayisi = 30
_sayac = 0
isim2 = "Ayşe"

# Yanlış isimler (bunları denerseniz hata alırsınız)
# 1ogrenci = 5    # Sayı ile başlayamaz
# if = 10         # Anahtar kelime kullanılamaz
# ogrenci-sayisi = 3  # Tire kullanılamaz
```

### Çoklu Atama

```python
# Aynı anda birden çok değişkene değer atama
a, b, c = 1, 2, 3
print(a, b, c)  # 1 2 3

# Aynı değeri birden fazla değişkene atama
x = y = z = 0

# Değişken değerlerini takas etme (swap) — çok kullanışlı!
a, b = b, a
print(a, b)  # 2 1 (değerler yer değiştirdi)
```

### Aritmetik Operatörler

```python
toplam = 10 + 5        # 15    (toplama)
fark = 10 - 5          # 5     (çıkarma)
carpim = 10 * 5        # 50    (çarpma)
bolme = 10 / 3         # 3.333 (bölme — sonuç her zaman float)
tam_bolme = 10 // 3    # 3     (tam bölme — kesir atılır)
kalan = 10 % 3         # 1     (mod — kalanı bulma)
us = 2 ** 4            # 16    (üs alma — 2 üzeri 4)

print(f"Toplam: {toplam}")
print(f"10 / 3 = {bolme} (float)")
print(f"10 // 3 = {tam_bolme} (tam sayı)")
print(f"10 % 3 = {kalan} (kalan)")
print(f"2 ** 4 = {us}")
```

**Pratik not**: `//` bölme işlemi çok kullanışlıdır. Örneğin 10 elmayı 3 kişiye paylaştırırsanız herkese 3 elma düşer (`10 // 3 = 3`), 1 elma artar (`10 % 3 = 1`).

### İşlemli Atama (Augmented Assignment)

```python
x = 5
x += 3    # x = x + 3 → 8
x -= 2    # x = x - 2 → 6
x *= 2    # x = x * 2 → 12
x /= 4    # x = x / 4 → 3.0
print(x)
```

### Karşılaştırma Operatörleri

Sonuç her zaman `True` veya `False` döndürür:

```python
print(5 == 5)    # True  (eşit mi)
print(5 != 3)    # True  (eşit değil mi)
print(5 > 3)     # True  (büyük mü)
print(5 < 3)     # False (küçük mü)
print(5 >= 5)    # True  (büyük veya eşit mi)
print(5 <= 3)    # False (küçük veya eşit mi)

# Zincirleme karşılaştırma
x = 5
print(1 < x < 10)   # True (1 < 5 < 10)
print(10 > x > 3)   # True
```

### Mantıksal Operatörler (and, or, not)

```python
# and: her ikisi de True ise sonuç True
print(True and True)    # True
print(True and False)   # False

# or: en az biri True ise sonuç True
print(True or False)    # True
print(False or False)   # False

# not: değili
print(not True)         # False
print(not False)        # True

# Gerçek hayat örneği
yas = 20
ehliyet_var = True
print(yas >= 18 and ehliyet_var)   # True (araç kullanabilir)
```

### Üyelik Operatörleri (in / not in)

Bir değerin bir liste veya metin içinde olup olmadığını kontrol eder:

```python
liste = [1, 2, 3]
print(2 in liste)       # True (2 listede var)
print(5 not in liste)   # True (5 listede yok)

isim = "Ali"
print("A" in isim)      # True (A harfi Ali'de var)
```

### Operatör Önceliği

```python
# Çarpma/bölme, toplama/çıkarmadan önce gelir
sonuc = 2 + 3 * 4   # 14 (önce 3*4=12, sonra 2+12=14)

# Şüphede kalırsanız parantez kullanın!
sonuc = (2 + 3) * 4  # 20 (önce toplama, sonra çarpma)
```

### Bu Bölümde Öğrendikleriniz
- ✅ Değişken tanımlama ve isimlendirme
- ✅ Aritmetik işlemler (+, -, *, /, //, %, **)
- ✅ Karşılaştırma (==, !=, >, <, >=, <=)
- ✅ Mantıksal operatörler (and, or, not)
- ✅ Üyelik kontrolü (in, not in)
- ✅ İşlemli atama (+=, -=, *=, /=)

### 🛠️ Alıştırmalar

**Temel:**
1. İki sayı değişkeni tanımlayın (`a = 15`, `b = 4`) ve toplam, fark, çarpım, bölüm, tam bölme ve kalanını hesaplayıp yazdırın
2. Kullanıcının adını ve doğum yılını değişkenlere atayın: `ad = "Ahmet"` ve `dogum = 2005`. Yaşını hesaplayıp `f"{ad} {yas} yaşında"` formatında yazdırın
3. `sayi = 7` olsun. `sayi > 5 and sayi < 10` ifadesinin sonucunu tahmin edin, sonra kodla kontrol edin

**Orta:**
4. İki sayının ortalamasını hesaplayan bir program yazın (ipucu: `ortalama = (sayi1 + sayi2) / 2`)
5. Bir sayının çift mi tek mi olduğunu `%` operatörü ile kontrol edin (ipucu: `sayi % 2 == 0` ise çifttir)

**İleri:**
6. Üç sayının (`a=8, b=12, c=5`) en büyüğünü karşılaştırma operatörleri ve `print` ile bulun. Birden fazla yol deneyin.

## <a id="string-islemleri"></a>String İşlemleri:

### String Tanımlama

```python
# Tek tırnak, çift tırnak fark etmez
s1 = 'merhaba'
s2 = "merhaba"
s3 = '''çok satırlı
string'''
s4 = """da çok satırlı
string"""

# Kaçış karakterleri
print("Merhaba\tDünya")   # sekme
print("Satir1\nSatir2")   # yeni satır
print("\\\\")               # ters eğik çizgi
print('"tırnak"')       # tırnak
```

### F-string (Python 3.6+)

```python
isim = "Ali"
yas = 25
print(f"Ben {isim}, {yas} yaşındayım.")
# Ben Ali, 25 yaşındayım.

# İfade kullanımı
print(f"10 yıl sonra {yas + 10} yaşında olacağım.")
# 10 yıl sonra 35 yaşında olacağım.

# Format belirteçleri
pi = 3.14159
print(f"Pi = {pi:.2f}")          # Pi = 3.14
print(f"{123456:10}")            # sağa yasla, 10 karakter
print(f"{123456:<10}")           # sola yasla
print(f"{123456:^10}")           # orta yasla
print(f"{123456:010d}")          # sıfırla doldur: 0000123456
print(f"{255:b}")                # binary: 11111111
print(f"{255:x}")                # hex: ff
print(f"{255:X}")                # hex büyük: FF
print(f"{1_000_000:,}")          # binlik ayırıcı: 1,000,000
```

### String Metodları

```python
metin = "  Python Programlama Dili  "

# Büyük/küçük harf dönüşümleri
metin.upper()          # "  PYTHON PROGRAMLAMA DILI  "
metin.lower()          # "  python programlama dili  "
metin.title()          # "  Python Programlama Dili  "
metin.capitalize()     # "  python programlama dili  "
metin.swapcase()       # "  pYTHON pROGRAMLAMA dILi  "

# Temizleme
metin.strip()          # "Python Programlama Dili" (baş ve sondaki boşlukları siler)
metin.lstrip()         # "Python Programlama Dili  "
metin.rstrip()         # "  Python Programlama Dili"
metin.strip(" P")      # "ython Programlama Dili"

# Arama ve sorgulama
metin.find("Python")       # 2 (indeks, bulamazsa -1)
metin.index("Python")      # 2 (indeks, bulamazsa ValueError)
metin.rfind("i")           # 24 (sondan ara)
metin.count("a")           # 3
metin.startswith("  Py")   # True
metin.endswith("Dili  ")   # True
"abc123".isalpha()         # False
"abc".isalpha()            # True
"123".isdigit()            # True
"abc123".isalnum()         # True
"   ".isspace()            # True

# Değiştirme
metin.replace("Python", "Java")   # "  Java Programlama Dili  "
metin.replace("a", "e", 1)        # sadece ilk "a"yı değiştir

# Bölme ve birleştirme
metin.split()              # ['Python', 'Programlama', 'Dili'] (varsayılan boşluk)
metin.split("a")           # ['  Pyth', 'on Progr', 'ml', 'm', ' Dili  ']
"1,2,3,4".split(",")       # ['1', '2', '3', '4']
"-".join(["a", "b", "c"])  # "a-b-c"

# Diğer yararlı metodlar
len(metin)                 # 28 (karakter sayısı)
"42".zfill(5)              # "00042"
"Merhaba".center(20)       # "      Merhaba       "
"Merhaba".center(20, "-")  # "------Merhaba-------"
"Merhaba".ljust(20, "-")   # "Merhaba-------------"
"Merhaba".rjust(20, "-")   # "-------------Merhaba"
```

### String Dilimleme (Slicing)

```python
s = "Python Programlama"

s[0]        # "P" (ilk karakter)
s[-1]       # "a" (son karakter)
s[0:6]      # "Python" (0-5 arası)
s[:6]       # "Python" (baştan 5'e)
s[7:]       # "Programlama" (7'den sona)
s[-10:]     # "ogramlama"
s[::2]      # "Pto rgraa" (2'şer atla)
s[::-1]     # "amalmargorP nohtyP" (tersine çevir)
s[1:10:3]   # "y o" (1-9 arası 3'er atla)

# Dilimleme yeni bir string oluşturur (immutable)
```

### String Biçimlendirme (Eski Yöntemler)

```python
isim = "Ali"
yas = 25

# % formatlama
print("Ben %s, %d yaşındayım." % (isim, yas))

# str.format()
print("Ben {}, {} yaşındayım.".format(isim, yas))
print("Ben {ad}, {y} yaşındayım.".format(ad=isim, y=yas))
print("{:.2f}".format(3.14159))  # 3.14
```

### Raw String

```python
# Raw string: kaçış karakterleri işlenmez
path = r"C:\Users\Ali\Documents"
print(path)  # C:\Users\Ali\Documents

regex = r"\d+\.\d+"  # regex için ideal
```

### Bu Bölümde Öğrendikleriniz
- ✅ String tanımlama (tek/çift/üç tırnak)
- ✅ Kaçış karakterleri (\n, \t, \\)
- ✅ F-string ile değişken ekleme
- ✅ String metodları (upper, lower, split, join, replace)
- ✅ String dilimleme (slicing) ve indeksleme
- ✅ String biçimlendirme yöntemleri

### 🛠️ Alıştırmalar

**Temel:**
1. Adınızı ve soyadınızı iki ayrı değişkende saklayın, f-string ile birleştirip yazdırın
2. `"Merhaba Dünya"` string'inin tüm harflerini büyütün, sonra küçültün, sonra "Dünya" kısmını "Python" ile değiştirin
3. `"python-programlama-dili"` string'ini `-` karakterinden ayırıp bir liste yapın, sonra boşlukla birleştirin

**Orta:**
4. Kullanıcıdan alınan bir metnin uzunluğunu (`len()`) hesaplayın, ilk ve son karakterini yazdırın
5. Bir string'in tersini yazdırın (ipucu: `[::-1]` — ters dilimleme)

**İleri:**
6. Bir cümledeki kelime sayısını hesaplayan kod yazın (ipucu: `split()` ile kelimelere ayırın, `len()` ile sayın)
7. Palindrom kontrolü: bir string tersten okunduğunda da aynı mı? (ipucu: `metin == metin[::-1]`)

---

## <a id="kontrol-yapilari"></a>Kontrol Yapıları

> Programınızın farklı durumlarda farklı şeyler yapmasını sağlamak için karar yapılarını kullanırsınız.

### if, elif, else

```python
yas = 25

if yas < 13:
    print("Çocuk")
elif yas < 18:
    print("Genç")
elif yas < 65:
    print("Yetişkin")
else:
    print("Yaşlı")
```

**Nasıl çalışır?**
1. Python yukarıdan aşağıya koşulları kontrol eder
2. İlk `True` olan koşulun bloğunu çalıştırır
3. Hiçbir koşul `True` değilse `else` bloğu çalışır
4. `elif` istediğiniz kadar ekleyebilirsiniz, `else` isteğe bağlıdır

```python
# Sadece if
if yas >= 18:
    print("Reşitsiniz")

# if-else
if yas >= 18:
    print("Reşitsiniz")
else:
    print("Reşit değilsiniz")
```

### match/case (Python 3.10+)

Daha basit çok dallı kontroller için kullanabilirsiniz:

```python
def http_status_kodu(kod):
    match kod:
        case 200:
            return "OK"
        case 404:
            return "Not Found"
        case 500:
            return "Internal Server Error"
        case _:          # _ her şeyi yakalar (default gibi)
            return "Bilinmeyen kod"

print(http_status_kodu(404))  # Not Found
```

### Koşul İfadelerinde True/False

Bazı değerler otomatik olarak `False` kabul edilir:

```python
# False kabul edilen değerler:
# None, False, 0, 0.0, "" (boş string), [], (), {}, set()

isim = ""
if isim:            # False (boş string)
    print("İsim var")
else:
    print("İsim yok")  # Bu çalışır

isim = "Ali"
if isim:            # True (dolu string)
    print("İsim var")  # Bu çalışır
```

### Ternary (Kısa if-else)

Tek satırda basit if-else yazmak için:

```python
yas = 20
durum = "Reşit" if yas >= 18 else "Reşit değil"
print(durum)  # Reşit
```

### Bu Bölümde Öğrendikleriniz
- ✅ if / elif / else ile karar yapıları
- ✅ match/case ile çok dallı kontroller
- ✅ Truthy/falsy değerler (hangi değerler False)
- ✅ Ternary ifade (tek satırda if-else)

### 🛠️ Alıştırmalar

**Temel:**
1. Bir sayı değişkeni alın, 0'dan büyükse "Pozitif", küçükse "Negatif", eşitse "Sıfır" yazdıran kod yazın
2. Kullanıcının notunu alın (0-100 arası): 90+ "AA", 80+ "BA", 70+ "BB", 60+ "CB", 50+ "CC", 50 altı "FF" yazdırın
3. Bir sayının çift mi tek mi olduğunu if ile kontrol edin

**Orta:**
4. Kullanıcının adını ve yaşını değişkenlere atayın. 18+ ise "Oy kullanabilir", değilse "Oy kullanamaz" yazdırın. Ternary ile deneyin.
5. İki sayıyı karşılaştırın: hangisi büyük? Eşitlerse "Eşit" yazdırın.

**🎮 Mini Proje: Basit Hesap Makinesi**
Kullanıcının girdiği iki sayı ve bir işlem (+, -, *, /) için if/elif kullanarak sonucu hesaplayan bir program yazın. Örnek:
```python
sayi1 = 10
sayi2 = 5
islem = "+"   # Bunu değiştirerek test edin

if islem == "+":
    print(sayi1 + sayi2)
elif islem == "-":
    print(sayi1 - sayi2)
elif islem == "*":
    print(sayi1 * sayi2)
elif islem == "/":
    print(sayi1 / sayi2)
else:
    print("Geçersiz işlem")
```
Farklı işlemlerle test edin!

---

## <a id="donguler"></a>Döngüler

> Döngüler, aynı işlemi tekrar tekrar yapmanızı sağlar. Bir listedeki tüm elemanları yazdırmak veya bir işlemi 10 kez tekrarlamak için kullanılır.

### for Döngüsü

`for` döngüsü, bir listedeki her eleman için bir işlem yapar:

```python
# range() ile sayı döngüsü
for i in range(5):
    print(i)  # 0, 1, 2, 3, 4

# Liste üzerinde döngü
meyveler = ["elma", "armut", "muz"]
for meyve in meyveler:
    print(meyve)

# String üzerinde döngü
for harf in "Python":
    print(harf)  # P, y, t, h, o, n
```

**range() fonksiyonu:**
```python
range(5)       # 0, 1, 2, 3, 4 (0'dan başla, 5'e kadar)
range(2, 8)    # 2, 3, 4, 5, 6, 7 (2'den başla, 8'e kadar)
range(0, 10, 2)  # 0, 2, 4, 6, 8 (0'dan 10'a, 2'şer atla)
range(10, 0, -1) # 10, 9, 8, ..., 1 (geriye say)
```

**Kullanışlı döngü fonksiyonları:**
```python
meyveler = ["elma", "armut", "muz"]

# enumerate() ile hem indeks hem değer
for indeks, meyve in enumerate(meyveler):
    print(f"{indeks}: {meyve}")

# zip() ile iki listeyi paralel döngü
isimler = ["Ali", "Veli", "Ayşe"]
yaslar = [25, 30, 28]
for isim, yas in zip(isimler, yaslar):
    print(f"{isim}: {yas}")

# reversed() ile ters döngü
for meyve in reversed(meyveler):
    print(meyve)  # muz, armut, elma
```

### break ve continue

Döngülerin akışını kontrol etmek için kullanılır:

```python
# break - döngüyü tamamen durdur
for i in range(10):
    if i == 5:
        break           # 5'te dur
    print(i)            # 0, 1, 2, 3, 4

# continue - sadece bu adımı atla, döngüye devam et
for i in range(10):
    if i % 2 == 0:
        continue        # çift sayıları atla
    print(i)            # 1, 3, 5, 7, 9
```

### pass (Yer Tutucu)

Henüz bir şey yazmak istemiyorsanız `pass` kullanın:

```python
if True:
    pass  # Daha sonra kod eklenecek

for i in range(10):
    pass  # Daha sonra yazılacak
```

### while Döngüsü

Koşul `True` olduğu sürece çalışır:

```python
# Temel while
sayac = 1
while sayac <= 5:
    print(sayac)
    sayac += 1   # 1, 2, 3, 4, 5

# Sonsuz döngü (break ile çıkılır)
while True:
    cevap = input("Çıkmak için 'q' girin: ")
    if cevap.lower() == 'q':
        break
```

### for-else ve while-else

Döngü `break` ile değil de normal sonlandığında `else` bloğu çalışır:

```python
# Örnek: sayılarda çift sayı var mı?
sayilar = [1, 3, 5, 7, 9]
for sayi in sayilar:
    if sayi % 2 == 0:
        print("Çift sayı bulundu!")
        break
else:
    print("Hiç çift sayı yok")  # Bu çalışır (break olmadı)
```

Bu, bir şey **bulunamadığında** çalıştırmak için idealdir.

### Bu Bölümde Öğrendikleriniz
- ✅ for döngüsü (range, list, string üzerinde)
- ✅ enumerate, zip, reversed ile döngü
- ✅ break (döngüyü durdur) ve continue (adımı atla)
- ✅ while döngüsü
- ✅ for-else / while-else
- ✅ pass (yer tutucu)

### 🛠️ Alıştırmalar

**Temel:**
1. 1'den 10'a kadar sayıları for döngüsü ile yazdırın
2. `["Python", "Java", "C++"]` listesindeki her dili for ile yazdırın
3. 1'den 20'ye kadar sadece çift sayıları yazdırın (if ve continue ile)

**Orta:**
4. 1'den 100'e kadar olan sayıların toplamını for döngüsü ile hesaplayın
5. Kullanıcı "q" girene kadar sayı alan ve girdiği sayıların toplamını hesaplayan bir while döngüsü yazın

**🎮 Mini Proje: Sayı Tahmin Oyunu**
Bilgisayarın rastgele seçtiği 1-100 arası bir sayıyı tahmin edin:
```python
import random

hedef = random.randint(1, 100)
tahmin = None
deneme = 0

while tahmin != hedef:
    tahmin = int(input("1-100 arası tahmininiz: "))
    deneme += 1
    if tahmin < hedef:
        print("Daha büyük!")
    elif tahmin > hedef:
        print("Daha küçük!")
    else:
        print(f"Bildiniz! {deneme} denemede buldunuz.")
```

**Püf noktaları:**
- `import random` ile rastgele sayı üretme kütüphanesini ekliyoruz
- `randint(1, 100)` 1-100 arası rastgele tam sayı üretir
- `input()` kullanıcıdan veri alır (her zaman string döner)
- `int(input(...))` ile string'i sayıya çeviriyoruz

## <a id="listeler"></a>Listeler:

Listeler, Python'da en sık kullanılan veri yapılarından biridir. Değiştirilebilir (mutable), sıralı (ordered) ve heterojen (farklı türde eleman içerebilir) koleksiyonlardır.

### Liste Oluşturma

```python
# Boş liste
bos1 = []
bos2 = list()

# Dolu liste
sayilar = [1, 2, 3, 4, 5]
karisik = [1, "metin", 3.14, True, None]
ic_ice = [[1, 2], [3, 4], [5, 6]]

# list() constructor
liste = list("Python")  # ['P', 'y', 't', 'h', 'o', 'n']
liste = list(range(5))  # [0, 1, 2, 3, 4]
```

### Liste İndeksleme ve Dilimleme

```python
sayilar = [10, 20, 30, 40, 50]

sayilar[0]      # 10
sayilar[-1]     # 50
sayilar[1:3]    # [20, 30]
sayilar[:3]     # [10, 20, 30]
sayilar[2:]     # [30, 40, 50]
sayilar[::2]    # [10, 30, 50]
sayilar[::-1]   # [50, 40, 30, 20, 10]
sayilar[1:4:2]  # [20, 40]

# Dilimleme ile değiştirme
sayilar[1:3] = [25, 35]  # [10, 25, 35, 40, 50]
```

### Liste Metodları

```python
liste = [3, 1, 4, 1, 5, 9, 2]

# Eleman ekleme
liste.append(6)       # sona ekle: [3, 1, 4, 1, 5, 9, 2, 6]
liste.insert(0, 0)    # belirtilen indise ekle: [0, 3, 1, 4, 1, 5, 9, 2, 6]
liste.extend([7, 8])  # başka bir listenin elemanlarını ekle

# Eleman silme
liste.pop()           # son elemanı sil ve döndür
liste.pop(2)          # 2. indisteki elemanı sil ve döndür
liste.remove(4)       # ilk eşleşen 4 değerini sil
liste.clear()         # tüm elemanları sil

# Arama
liste.index(5)        # 5'in ilk bulunduğu indeks
liste.index(1, 2)     # 2. indeksten başlayarak 1'i ara
liste.count(1)        # 1'in kaç kere geçtiği

# Sıralama ve ters çevirme
liste.sort()          # artan sırala (kalıcı)
liste.sort(reverse=True)  # azalan sırala
liste.reverse()       # ters çevir
sorted(liste)         # sıralanmış yeni liste döndür (kalıcı değil)
list(reversed(liste)) # ters çevrilmiş yeni liste

# Kopyalama
yeni = liste.copy()           # yüzeysel kopya
yeni = list(liste)            # aynı
yeni = liste[:]               # aynı
import copy
derin = copy.deepcopy(ic_ice) # derin kopya (iç içe listeler için)
```

### Liste İşlemleri

```python
a = [1, 2, 3]
b = [4, 5, 6]

# Birleştirme
c = a + b           # [1, 2, 3, 4, 5, 6]

# Tekrarlama
d = a * 3           # [1, 2, 3, 1, 2, 3, 1, 2, 3]

# Üyelik kontrolü
print(2 in a)       # True
print(7 not in a)   # True

# Uzunluk
print(len(a))       # 3

# Min, max, sum
print(min(a))       # 1
print(max(a))       # 3
print(sum(a))       # 6

# all() ve any()
print(all([True, True, False]))  # False
print(any([True, False, False])) # True
```

## <a id="demetler-tuple"></a>Demetler (Tuple):

Demetler, değiştirilemez (immutable), sıralı koleksiyonlardır. Listelerden farkı, oluşturulduktan sonra değiştirilemez olmalarıdır.

### Demet Oluşturma

```python
# Boş demet
bos1 = ()
bos2 = tuple()

# Dolu demet
demet = (1, 2, 3)
karisik = (1, "metin", 3.14)

# Tek elemanlı demet (virgül önemli!)
tekli = (5,)       # tuple (doğru)
tekli2 = (5)       # int (yanlış!)

# Parantez zorunlu değil
demet = 1, 2, 3    # (1, 2, 3)

# tuple() constructor
demet = tuple("abc")  # ('a', 'b', 'c')
demet = tuple([1,2,3]) # (1, 2, 3)
```

### Demet Metodları ve İşlemleri

```python
demet = (1, 2, 3, 2, 4, 2)

# Liste ile aynı indeksleme ve dilimleme
demet[0]    # 1
demet[1:3]  # (2, 3)

# count() ve index()
demet.count(2)   # 3
demet.index(3)   # 2

# Demet birleştirme
a = (1, 2)
b = (3, 4)
c = a + b        # (1, 2, 3, 4)

# Demet tekrarlama
d = a * 3        # (1, 2, 1, 2, 1, 2)

# Üyelik
print(2 in demet)   # True

# Demet paketleme ve açma
# Paketleme: değerleri bir demette toplama
demet = 1, 2, 3

# Açma (unpacking): demeti ayrı değişkenlere atama
a, b, c = demet
print(a, b, c)  # 1 2 3

# Yıldızlı açma (Python 3+)
ilk, *orta, son = (1, 2, 3, 4, 5)
print(ilk)   # 1
print(orta)  # [2, 3, 4]
print(son)   # 5

# Demet neden kullanılır?
# - Değiştirilemez: veri bütünlüğü sağlar
# - Hashlenebilir: sözlük anahtarı olarak kullanılabilir
# - Daha hızlı: listelerden daha hızlıdır
# - Fonksiyonlardan birden çok değer döndürmek için
```

## <a id="sozlukler-dict"></a>Sözlükler (Dict):

Sözlükler, anahtar-değer (key-value) çiftlerini depolayan, değiştirilebilir ve sıralı (Python 3.7+ insertion order korunur) koleksiyonlardır.

### Sözlük Oluşturma

```python
# Boş sözlük
bos1 = {}
bos2 = dict()

# Dolu sözlük
ogrenci = {
    "ad": "Ali",
    "soyad": "Yılmaz",
    "yas": 20,
    "notlar": [85, 90, 78]
}

# dict() constructor
sozluk = dict(ad="Ali", yas=20)      # {'ad': 'Ali', 'yas': 20}
sozluk = dict([("a", 1), ("b", 2)])  # {'a': 1, 'b': 2}

# zip() ile sözlük
anahtarlar = ["a", "b", "c"]
degerler = [1, 2, 3]
sozluk = dict(zip(anahtarlar, degerler))  # {'a': 1, 'b': 2, 'c': 3}
```

### Sözlük İşlemleri

```python
ogrenci = {"ad": "Ali", "yas": 20, "bolum": "Bilgisayar"}

# Erişim
print(ogrenci["ad"])        # Ali
print(ogrenci.get("ad"))    # Ali
print(ogrenci.get("not"))   # None (hata vermez)
print(ogrenci.get("not", 0)) # 0 (varsayılan değer)

# Ekleme ve güncelleme
ogrenci["not"] = 85         # yeni anahtar ekle
ogrenci["yas"] = 21         # var olanı güncelle

# setdefault(): anahtar yoksa varsayılan değer ekle
ogrenci.setdefault("adres", "Bilinmiyor")

# update(): başka bir sözlükle birleştir
ogrenci.update({"tel": "555-1234", "email": "ali@mail.com"})

# Silme
del ogrenci["tel"]          # anahtar-sil (KeyError yoksa)
silinen = ogrenci.pop("email")  # sil ve değerini döndür
son_eklenen = ogrenci.popitem() # son ekleneni sil ve döndür (Python 3.7+)
ogrenci.clear()             # tümünü sil
```

### Sözlük Metodları

```python
ogrenci = {"ad": "Ali", "yas": 20, "bolum": "Bilgisayar"}

# Anahtarlar, değerler, öğeler
print(ogrenci.keys())    # dict_keys(['ad', 'yas', 'bolum'])
print(ogrenci.values())  # dict_values(['Ali', 20, 'Bilgisayar'])
print(ogrenci.items())   # dict_items([('ad', 'Ali'), ('yas', 20), ('bolum', 'Bilgisayar')])

# Döngüde kullanım
for anahtar, deger in ogrenci.items():
    print(f"{anahtar}: {deger}")

# Kopyalama
yeni = ogrenci.copy()
import copy
derin = copy.deepcopy(ogrenci)
```

### Sözlük Kullanım İpuçları

```python
# Varsayılan değerli sözlük (collections.defaultdict)
from collections import defaultdict

# default dict: var olmayan anahtarlara otomatik varsayılan değer atar
sayac = defaultdict(int)
kelimeler = ["elma", "armut", "elma", "muz", "elma"]
for kelime in kelimeler:
    sayac[kelime] += 1
print(dict(sayac))  # {'elma': 3, 'armut': 1, 'muz': 1}

# Liste tutan defaultdict
gruplar = defaultdict(list)
gruplar["meyveler"].append("elma")
gruplar["meyveler"].append("armut")
gruplar["renkler"].append("kırmızı")

# Counter: eleman sayma
from collections import Counter
sayac = Counter(kelimeler)
print(sayac.most_common(2))  # [('elma', 3), ('armut', 1)]
```

## <a id="kumeler-set"></a>Kümeler (Set):

Kümeler, benzersiz elemanlardan oluşan, sırasız (unordered) koleksiyonlardır.

### Küme Oluşturma

```python
# Boş küme ({} boş sözlük oluşturur!)
bos = set()

# Dolu küme
kume = {1, 2, 3, 4, 5}
karisik = {1, "metin", 3.14}  # heterojen

# set() constructor
kume = set("merhaba")  # {'m', 'e', 'r', 'h', 'a', 'b'} (benzersiz)
kume = set([1, 2, 2, 3, 3])  # {1, 2, 3}
```

### Küme İşlemleri

```python
a = {1, 2, 3, 4, 5}
b = {4, 5, 6, 7, 8}

# Temel işlemler
print(a | b)  # birleşim: {1, 2, 3, 4, 5, 6, 7, 8}
print(a & b)  # kesişim: {4, 5}
print(a - b)  # fark: {1, 2, 3}
print(b - a)  # fark: {8, 6, 7}
print(a ^ b)  # simetrik fark: {1, 2, 3, 6, 7, 8}

# Metodlar
a.union(b)          # birleşim
a.intersection(b)   # kesişim
a.difference(b)     # fark
a.symmetric_difference(b)  # simetrik fark

# Alt küme / üst küme
print({1, 2}.issubset(a))    # True
print(a.issuperset({1, 2}))  # True
print(a.isdisjoint({6, 7}))  # True (kesişim boş mu?)
```

### Küme Metodları

```python
kume = {1, 2, 3}

# Eleman ekleme/silme
kume.add(4)          # {1, 2, 3, 4}
kume.discard(5)      # varsa sil, yoksa hata vermez
kume.remove(3)       # varsa sil, yoksa KeyError
kume.pop()           # rastgele eleman sil ve döndür
kume.clear()         # tümünü sil

# Kopyalama
yeni = kume.copy()
```

### Frozenset (Değiştirilemez Küme)

```python
# Frozenset: değiştirilemez küme
fs = frozenset([1, 2, 3, 4])
# fs.add(5)  # AttributeError
# set'lerin aksine hashlenebilir, sözlük anahtarı olabilir
```

### Bu Bölümlerde (Listeler, Demetler, Sözlükler, Kümeler) Öğrendikleriniz
- ✅ Liste oluşturma, indeksleme, dilimleme, metodlar
- ✅ Demet (tuple) oluşturma ve kullanım alanları
- ✅ Sözlük (dict) oluşturma, anahtar/değer işlemleri
- ✅ Küme (set) oluşturma ve küme işlemleri (birleşim, kesişim, fark)
- ✅ Her veri yapısının ne zaman kullanılacağı

### 🛠️ Alıştırmalar

**Temel:**
1. İçinde 5 farklı meyve olan bir liste oluşturun, for ile her birini yazdırın
2. Listenin ilk elemanını, son elemanını ve 2. elemanını dilimleme ile alın
3. `{"elma": 5, "armut": 3, "muz": 7}` sözlüğünde her meyvenin adını ve miktarını for ile yazdırın

**Orta:**
4. İki listeyi `zip()` ile birleştirip sözlük yapın: `isimler = ["Ali", "Ayşe"]`, `yaslar = [25, 30]`
5. Bir string'deki benzersiz harfleri bulmak için set kullanın
6. 1'den 20'ye kadar sayıların olduğu bir listeden çift sayıları filtreleyip yeni liste yapın

**🎮 Mini Proje: Alışveriş Listesi Uygulaması**
```python
# Alışveriş listesi: ürün adı ve miktarını bir sözlükte tutalım
alisveris = {}

while True:
    print("\n--- ALIŞVERİŞ LİSTESİ ---")
    print("1. Ürün ekle")
    print("2. Ürün sil")
    print("3. Listeyi göster")
    print("4. Çıkış")
    secim = input("Seçiminiz (1-4): ")
    
    if secim == "1":
        urun = input("Ürün adı: ")
        miktar = int(input("Miktar: "))
        alisveris[urun] = alisveris.get(urun, 0) + miktar
        print(f"{urun} eklendi!")
    elif secim == "2":
        urun = input("Silinecek ürün: ")
        if urun in alisveris:
            del alisveris[urun]
            print(f"{urun} silindi!")
        else:
            print("Ürün bulunamadı!")
    elif secim == "3":
        if alisveris:
            print("\nAlışveriş Listeniz:")
            for urun, miktar in alisveris.items():
                print(f"  - {urun}: {miktar} adet")
        else:
            print("Listeniz boş!")
    elif secim == "4":
        print("Hoşça kalın!")
        break
    else:
        print("Geçersiz seçim!")
```

Bu programda `dict` (sözlük), `for` döngüsü, `if/elif/else`, `input()` ve `while` kullandık. Çalıştırın ve deneyin!

---

## <a id="fonksiyonlar"></a>Fonksiyonlar

> ⚠️ **Yeni başlayanlar için**: Sadece "Temel Fonksiyon Tanımı", "Parametreler", "Varsayılan Parametreler" ve "return" kısımlarını okuyun. *args, **kwargs ve tip ipuçları kısımlarını sonra tekrar gelmek üzere atlayın.

Fonksiyonlar, tekrar kullanılabilir kod bloklarıdır. `def` anahtar kelimesiyle tanımlanır.

### Temel Fonksiyon Tanımı

```python
def selamla():
    """Bu fonksiyon selam mesajı yazdırır."""
    print("Merhaba!")

selamla()  # Merhaba!
```

### Parametreler ve Argümanlar

```python
# Parametreli fonksiyon
def selamla(isim):
    print(f"Merhaba, {isim}!")

selamla("Ali")  # Merhaba, Ali!

# Birden çok parametre
def topla(a, b):
    return a + b

print(topla(3, 5))  # 8
```

### return (Değer Döndürme)

```python
def kare_al(sayi):
    return sayi ** 2

sonuc = kare_al(5)
print(sonuc)  # 25

# return olmazsa fonksiyon None döndürür
def yazdir(sayi):
    print(sayi)

print(yazdir(5))  # 5 (önce print çalışır), sonra None yazılır
```

### Varsayılan Parametreler

```python
def selamla(isim="Dünya"):
    print(f"Merhaba, {isim}!")

selamla()        # Merhaba, Dünya!
selamla("Ali")   # Merhaba, Ali!

# Önemli: varsayılan değerler fonksiyon tanımlandığında bir kere değerlendirilir
# Değiştirilebilir varsayılan değer kullanırken dikkatli olun:
def hatali_ekle(eleman, liste=[]):
    liste.append(eleman)
    return liste

print(hatali_ekle(1))  # [1]
print(hatali_ekle(2))  # [1, 2]  (beklenmeyen!)

# Doğru kullanım:
def dogru_ekle(eleman, liste=None):
    if liste is None:
        liste = []
    liste.append(eleman)
    return liste

print(dogru_ekle(1))  # [1]
print(dogru_ekle(2))  # [2]
```

> 🙋‍♂️ **Pratik ipucu**: Varsayılan parametre olarak liste, sözlük gibi değiştirilebilir (mutable) türler kullanmayın. `None` kullanıp fonksiyon içinde oluşturun.

### Bu Kısımda (Temel Fonksiyonlar) Öğrendikleriniz
- ✅ Fonksiyon tanımlama (`def`)
- ✅ Parametre ve argüman kullanımı
- ✅ `return` ile değer döndürme
- ✅ Varsayılan parametreler

### 🛠️ Alıştırmalar (Temel Fonksiyonlar)

**Temel:**
1. Adınızı parametre olarak alıp "Merhaba, [isim]!" yazdıran bir fonksiyon yazın
2. İki sayının çarpımını döndüren `carp(a, b)` fonksiyonu yazın
3. Bir sayının tek mi çift mi olduğunu döndüren `tek_mi(sayi)` fonksiyonu yazın (ipucu: `return sayi % 2 == 0`)

**Orta:**
4. Bir listenin en büyük elemanını döndüren `max_bul(liste)` fonksiyonu yazın (`for` döngüsü kullanın, `max()` kullanmayın)
5. Fahrenheit'i Celsius'a çeviren `f_to_c(f)` fonksiyonu yazın (formül: `(f - 32) * 5/9`)

**🎮 Mini Proje: Dönüştürücü Hesap Makinesi**
Birim dönüştürücü yapalım. Her dönüşüm bir fonksiyon olsun:
```python
def km_to_mil(km):
    return km * 0.621371

def celsius_to_fahrenheit(c):
    return c * 9/5 + 32

def litre_to_galon(litre):
    return litre * 0.264172

# Kullanıcı seçim yapsın
print("1. Km → Mil")
print("2. °C → °F")
print("3. Litre → Galon")
secim = input("Seçiminiz (1-3): ")
deger = float(input("Değer: "))

if secim == "1":
    print(f"{deger} km = {km_to_mil(deger):.2f} mil")
elif secim == "2":
    print(f"{deger}°C = {celsius_to_fahrenheit(deger):.1f}°F")
elif secim == "3":
    print(f"{deger} litre = {litre_to_galon(deger):.2f} galon")
else:
    print("Geçersiz seçim!")
```

---

### İleri Fonksiyon Konuları

> Aşağıdaki konular başlangıç seviyesinin ötesindedir. Temel fonksiyonları iyice kavradıktan sonra dönüp bakın.

### Anahtar Kelime Argümanları

```python
# Anahtar kelime argümanları (kwargs)
def bilgi_goster(**kwargs):
    for anahtar, deger in kwargs.items():
        print(f"{anahtar}: {deger}")

bilgi_goster(ad="Ali", yas=25, sehir="İstanbul")

# Tüm parametre türlerini birleştirme:
def tam_fonksiyon(normal, *args, varsayilan=10, **kwargs):
    print(f"Normal: {normal}")
    print(f"Args: {args}")
    print(f"Varsayılan: {varsayilan}")
    print(f"Kwargs: {kwargs}")

tam_fonksiyon("a", 1, 2, 3, varsayilan=20, x=100, y=200)
# Normal: a
# Args: (1, 2, 3)
# Varsayılan: 20
# Kwargs: {'x': 100, 'y': 200}

# Sıralama: normal, *args, varsayılan, **kwargs
```

### Dönüş Değeri

```python
# Tek değer döndürme
def kare(x):
    return x ** 2

# Birden çok değer döndürme (aslında demet döndürür)
def islemler(a, b):
    return a + b, a - b, a * b, a / b

top, fark, carp, bol = islemler(10, 3)
print(top, fark, carp, bol)  # 13 7 30 3.333...

# Hiçbir şey döndürmeyen fonksiyon None döndürür
def bos_fonksiyon():
    pass

print(bos_fonksiyon())  # None
```

### Docstring (Dokümantasyon Stringi)

```python
def ortalama(sayilar):
    """
    Bir sayı listesinin ortalamasını hesaplar.

    Parametreler:
        sayilar (list): Sayıların listesi

    Dönüş:
        float: Sayıların ortalaması

    Örnek:
        >>> ortalama([1, 2, 3])
        2.0
    """
    return sum(sayilar) / len(sayilar)

print(ortalama.__doc__)
help(ortalama)  # docstring'i gösterir
```

### Tip İpuçları (Type Hints - Python 3.5+)

```python
def topla(a: int, b: int) -> int:
    return a + b

def selamla(isim: str, yas: int = 0) -> str:
    return f"Merhaba {isim}, {yas} yaşında"

# Karmaşık tipler
from typing import List, Tuple, Dict, Optional, Union, Any, Set, Callable
from typing import TypeVar, Generic, Protocol, Literal, TypedDict

def islem(y: List[int]) -> Tuple[float, Optional[int]]:
    ...

def kayit_ekle(kullanici: Dict[str, Union[str, int]]) -> bool:
    ...
```

### Gelişmiş Tip İpuçları

```python
from typing import List, Optional, Union, TypeVar, Generic, Protocol, Literal
from typing import Any, Callable, Iterable, Iterator, Sequence, Mapping

# Optional: None olabilir
def bul_kullanici(id: int) -> Optional[str]:
    return "Ali" if id == 1 else None

# Union: birden çok tip
def formatla(deger: Union[int, float, str]) -> str:
    return str(deger)

# Python 3.10+ ile | operatörü
def formatla2(deger: int | float | str) -> str:
    return str(deger)

# Any: herhangi bir tip
def log_yaz(mesaj: Any) -> None:
    print(mesaj)

# Callable: fonksiyon tipleri
def islem_uygula(f: Callable[[int, int], int], a: int, b: int) -> int:
    return f(a, b)

print(islem_uygula(lambda x, y: x + y, 3, 5))  # 8

# TypeVar: jenerik tipler
T = TypeVar("T")

def ilk_eleman(iterable: List[T]) -> T:
    return iterable[0]

print(ilk_eleman([1, 2, 3]))    # 1 (int)
print(ilk_eleman(["a", "b"]))   # "a" (str)

def eslesme_bul(veri: list[T], kosul: Callable[[T], bool]) -> list[T]:
    return [x for x in veri if kosul(x)]

# Generic: jenerik sınıflar
class Kutu(Generic[T]):
    def __init__(self, icerik: T):
        self.icerik = icerik

    def al(self) -> T:
        return self.icerik

kutu = Kutu(42)
deger: int = kutu.al()

# Literal: belirli değerler
def http_istek(metod: Literal["GET", "POST", "PUT", "DELETE"]) -> str:
    return f"{metod} isteği"

http_istek("GET")     # doğru
# http_istek("PATCH") # mypy hatası

# TypedDict: sözlük şeması
class OgrenciDict(TypedDict):
    ad: str
    yas: int
    bolum: str

ogrenci: OgrenciDict = {"ad": "Ali", "yas": 20, "bolum": "Bilgisayar"}

# Protocol: duck typing için
class Konusabilen(Protocol):
    def ses_cikar(self) -> str: ...

class Kedi:
    def ses_cikar(self) -> str:
        return "Miyav"

def hayvan_konustur(h: Konusabilen) -> str:
    return h.ses_cikar()

print(hayvan_konustur(Kedi()))  # "Miyav" (Protocol sağlandı)

# Iterator/Iterable tipleri
def sayma(bitis: int) -> Iterator[int]:
    for i in range(bitis):
        yield i

# Self tipi (Python 3.11+)
from typing import Self

class Kopek:
    def yas_belirle(self, yas: int) -> Self:
        self.yas = yas
        return self

# Final ve ClassVar
from typing import Final, ClassVar

SABIT: Final = 42
# SABIT = 0  # mypy hatası

class Ogrenci:
    okul: ClassVar[str] = "Python Lisesi"
    isim: str

# TypeAlias (Python 3.10+)
Vektor = list[float]
Matris = list[Vektor]

def matris_carp(a: Matris, b: Matris) -> Matris: ...
```

### mypy ile Tip Kontrolü

```bash
# pip install mypy
mypy script.py
mypy --strict script.py
mypy --ignore-missing-imports script.py

# pyproject.toml yapılandırması
# [tool.mypy]
# python_version = "3.10"
# strict = true
# ignore_missing_imports = true

# VSCode, PyCharm, VS Code: otomatik tip kontrolü
```

### İç İçe Fonksiyonlar

```python
def dis_fonksiyon(x):
    def ic_fonksiyon(y):
        return x + y
    return ic_fonksiyon

topla_5 = dis_fonksiyon(5)
print(topla_5(3))  # 8
print(topla_5(10)) # 15
```

### Recursive Fonksiyonlar (Özyineleme)

```python
# Faktöriyel
def faktoriyel(n):
    if n <= 1:
        return 1
    return n * faktoriyel(n - 1)

print(faktoriyel(5))  # 120

# Fibonacci
def fibonacci(n):
    if n <= 1:
        return n
    return fibonacci(n - 1) + fibonacci(n - 2)

for i in range(10):
    print(fibonacci(i), end=" ")  # 0 1 1 2 3 5 8 13 21 34
```

### Global ve Yerel Değişkenler

```python
x = 10  # global

def fonksiyon():
    x = 5  # yerel (global x'i değiştirmez)
    print(x)  # 5

fonksiyon()
print(x)  # 10

def global_degistir():
    global x
    x = 20

global_degistir()
print(x)  # 20

# nonlocal: iç içe fonksiyonlarda üst fonksiyon değişkenine erişim
def dis():
    x = 10
    def ic():
        nonlocal x
        x = 20
    ic()
    print(x)  # 20
```

## <a id="lambda-ve-fonksiyonel-programlama"></a>Lambda ve Fonksiyonel Programlama:

### Lambda Fonksiyonları

```python
# Tek satırlık, isimsiz fonksiyonlar
kare = lambda x: x ** 2
print(kare(5))  # 25

# Birden çok parametre
topla = lambda a, b: a + b
print(topla(3, 5))  # 8

# Koşullu lambda
max_deger = lambda a, b: a if a > b else b

# Lambda'nın pratik kullanımı (genellikle sıralama ve filtreleme ile)
```

### map, filter, reduce

```python
# map(): her elemana fonksiyon uygula
sayilar = [1, 2, 3, 4, 5]
kareler = list(map(lambda x: x ** 2, sayilar))
print(kareler)  # [1, 4, 9, 16, 25]

# filter(): koşulu sağlayanları filtrele
ciftler = list(filter(lambda x: x % 2 == 0, sayilar))
print(ciftler)  # [2, 4]

# reduce(): kümülatif işlem (functools modülü)
from functools import reduce
toplam = reduce(lambda a, b: a + b, sayilar)
print(toplam)  # 15
faktoriyel = reduce(lambda a, b: a * b, range(1, 6))
print(faktoriyel)  # 120
```

### sorted() ile Özel Sıralama

```python
# key parametresi ile özel sıralama
isimler = ["Ali", "Ayşe", "Mehmet", "Zeynep"]
sirali = sorted(isimler, key=len)  # uzunluğa göre sırala
print(sirali)  # ['Ali', 'Ayşe', 'Mehmet', 'Zeynep']

# Sözlük sıralama
ogrenciler = [
    {"ad": "Ali", "not": 85},
    {"ad": "Veli", "not": 92},
    {"ad": "Ayşe", "not": 78},
]
sirali = sorted(ogrenciler, key=lambda x: x["not"], reverse=True)
print(sirali)  # [{'ad': 'Veli', 'not': 92}, ...]
```

## <a id="liste-ve-sozluk-kisayollari"></a>Liste ve Sözlük Kısayolları (Comprehensions):

### Liste Comprehension

```python
# Temel: [ifade for eleman in iterable if koşul]

# Kareler
kareler = [x ** 2 for x in range(10)]
print(kareler)  # [0, 1, 4, 9, 16, 25, 36, 49, 64, 81]

# Koşullu
cift_kareler = [x ** 2 for x in range(10) if x % 2 == 0]
print(cift_kareler)  # [0, 4, 16, 36, 64]

# İç içe döngü
coordinatlar = [(x, y) for x in range(3) for y in range(3)]
# [(0,0), (0,1), (0,2), (1,0), (1,1), (1,2), (2,0), (2,1), (2,2)]

# if-else ile
sonuclar = ["çift" if x % 2 == 0 else "tek" for x in range(5)]
# ['çift', 'tek', 'çift', 'tek', 'çift']

# String işlemleri
isimler = ["ali", "veli", "ayşe"]
buyuk = [isim.capitalize() for isim in isimler]
# ['Ali', 'Veli', 'Ayşe']

# İç içe listeyi düzleştirme
matrix = [[1, 2, 3], [4, 5, 6], [7, 8, 9]]
duz = [sayi for satir in matrix for sayi in satir]
# [1, 2, 3, 4, 5, 6, 7, 8, 9]
```

### Dict Comprehension

```python
# Temel: {anahtar_ifade: deger_ifade for eleman in iterable}

# Kare sözlüğü
kare_sozluk = {x: x ** 2 for x in range(5)}
# {0: 0, 1: 1, 2: 4, 3: 9, 4: 16}

# Koşullu
cift_kare = {x: x ** 2 for x in range(10) if x % 2 == 0}
# {0: 0, 2: 4, 4: 16, 6: 36, 8: 64}

# İki listeden sözlük
isimler = ["Ali", "Veli", "Ayşe"]
yaslar = [25, 30, 28]
sozluk = {isim: yas for isim, yas in zip(isimler, yaslar)}
# {'Ali': 25, 'Veli': 30, 'Ayşe': 28}

# Var olan sözlükten dönüşüm
orijinal = {"a": 1, "b": 2, "c": 3}
ters = {v: k for k, v in orijinal.items()}
# {1: 'a', 2: 'b', 3: 'c'}

# Filtreleme
filtreli = {k: v for k, v in orijinal.items() if v > 1}
# {'b': 2, 'c': 3}
```

### Set Comprehension

```python
# Temel: {ifade for eleman in iterable if kosul}
cift_kare = {x ** 2 for x in range(-5, 6)}
# {0, 1, 4, 9, 16, 25} (benzersiz, sırasız)

# Benzersiz harfler
metin = "merhaba dünya"
benzersiz = {harf for harf in metin if harf.isalpha()}
# {'m', 'e', 'r', 'h', 'a', 'b', 'd', 'ü', 'n', 'y'}
```

### Generator Expression

```python
# Parantez ile: (ifade for eleman in iterable)
# Tüm elemanları bir anda belleğe almaz, teker teker üretir

sayilar = (x ** 2 for x in range(1000000))
print(sayilar)  # <generator object <genexpr> at 0x...>

# next() ile teker teker
print(next(sayilar))  # 0
print(next(sayilar))  # 1
print(next(sayilar))  # 4

# Döngüde kullanım
for sayi in (x ** 2 for x in range(5)):
    print(sayi)  # 0, 1, 4, 9, 16

# sum(), any(), all() gibi fonksiyonlarla
toplam = sum(x ** 2 for x in range(1000))  # generator expression, parantez isteğe bağlı
```

## <a id="dosya-islemleri"></a>Dosya İşlemleri:

### Dosya Açma ve Kapama

```python
# Geleneksel yöntem (manuel kapama)
dosya = open("ornek.txt", "r", encoding="utf-8")
icerik = dosya.read()
dosya.close()

# Modern yöntem (with ile otomatik kapama)
with open("ornek.txt", "r", encoding="utf-8") as dosya:
    icerik = dosya.read()
```

### Dosya Modları

| Mod | Açıklama                                    |
|-----|---------------------------------------------|
| `r` | Okuma (varsayılan)                          |
| `w` | Yazma (üzerine yazar, dosya yoksa oluşturur)|
| `a` | Ekleme (dosya sonuna ekler)                 |
| `x` | Oluşturma (dosya varsa hata verir)          |
| `r+`| Okuma + Yazma                               |
| `w+`| Okuma + Yazma (üzerine yazar)               |
| `a+`| Okuma + Ekleme                              |
| `b` | Binary mod (rb, wb, ab)                     |

### Dosya Okuma Yöntemleri

```python
# Tüm dosyayı oku
with open("ornek.txt", "r", encoding="utf-8") as dosya:
    icerik = dosya.read()

# Satır satır oku (büyük dosyalar için)
with open("buyuk_dosya.txt", "r", encoding="utf-8") as dosya:
    for satir in dosya:
        print(satir.strip())

# Tüm satırları listeye al
with open("ornek.txt", "r", encoding="utf-8") as dosya:
    satirlar = dosya.readlines()

# readline() ile teker teker
with open("ornek.txt", "r", encoding="utf-8") as dosya:
    ilk_satir = dosya.readline()
    ikinci_satir = dosya.readline()

# Belirli sayıda karakter okuma
with open("ornek.txt", "r", encoding="utf-8") as dosya:
    ilk_10 = dosya.read(10)

# seek() ve tell() ile dosya içinde gezinme
with open("ornek.txt", "r", encoding="utf-8") as dosya:
    print(dosya.tell())  # 0 (mevcut konum)
    icerik = dosya.read(5)
    print(dosya.tell())  # 5
    dosya.seek(0)        # başa dön
    icerik = dosya.read()
```

### Dosyaya Yazma

```python
# Yazma (üzerine yazar)
with open("cikti.txt", "w", encoding="utf-8") as dosya:
    dosya.write("Birinci satır\n")
    dosya.write("İkinci satır\n")

# Çoklu yazma
with open("cikti.txt", "w", encoding="utf-8") as dosya:
    dosya.writelines(["satır1\n", "satır2\n", "satır3\n"])

# Ekleme
with open("cikti.txt", "a", encoding="utf-8") as dosya:
    dosya.write("Bu sona eklenir\n")

# print() ile dosyaya yazdırma
with open("cikti.txt", "w", encoding="utf-8") as dosya:
    print("Merhaba", "Dünya", file=dosya)
    print("İkinci satır", file=dosya)
```

### Binary Dosya İşlemleri

```python
# Binary okuma
with open("resim.jpg", "rb") as dosya:
    veri = dosya.read()

# Binary yazma
with open("kopya.jpg", "wb") as dosya:
    dosya.write(veri)
```

### os.path ve pathlib ile Dosya İşlemleri

```python
import os
from pathlib import Path

# os.path
print(os.path.exists("dosya.txt"))    # True/False
print(os.path.isfile("dosya.txt"))    # True/False
print(os.path.isdir("klasor"))        # True/False
print(os.path.getsize("dosya.txt"))   # dosya boyutu
print(os.path.basename("/a/b/c.txt")) # 'c.txt'
print(os.path.dirname("/a/b/c.txt"))  # '/a/b'
print(os.path.splitext("dosya.txt"))  # ('dosya', '.txt')

# pathlib (Python 3.4+, modern yaklaşım)
klasor = Path(".")
for dosya in klasor.glob("*.txt"):
    print(dosya.name)

yol = Path("/home/user/dosya.txt")
print(yol.exists())
print(yol.is_file())
print(yol.suffix)     # .txt
print(yol.stem)       # dosya
print(yol.parent)     # /home/user
yol.rename("yeni.txt")
yol.unlink()          # dosya sil
```

### CSV Dosyaları

```python
import csv

# CSV okuma
with open("veri.csv", "r", newline="", encoding="utf-8") as dosya:
    okuyucu = csv.reader(dosya)
    for satir in okuyucu:
        print(satir)

# CSV yazma
with open("cikti.csv", "w", newline="", encoding="utf-8") as dosya:
    yazici = csv.writer(dosya)
    yazici.writerow(["İsim", "Yaş", "Şehir"])
    yazici.writerow(["Ali", 25, "İstanbul"])
    yazici.writerow(["Veli", 30, "Ankara"])

# DictReader / DictWriter
with open("veri.csv", "r", newline="", encoding="utf-8") as dosya:
    okuyucu = csv.DictReader(dosya)
    for satir in okuyucu:
        print(satir["İsim"], satir["Yaş"])

with open("cikti.csv", "w", newline="", encoding="utf-8") as dosya:
    alanlar = ["İsim", "Yaş", "Şehir"]
    yazici = csv.DictWriter(dosya, fieldnames=alanlar)
    yazici.writeheader()
    yazici.writerow({"İsim": "Ali", "Yaş": 25, "Şehir": "İstanbul"})
```

### shutil ile Dosya ve Klasör İşlemleri

```python
import shutil
import os

# Dosya kopyalama
shutil.copy("kaynak.txt", "hedef.txt")            # dosya -> dosya
shutil.copy2("kaynak.txt", "hedef/")              # metadata korur
shutil.copyfile("kaynak.txt", "hedef.txt")        # sadece içerik

# Klasör kopyalama
shutil.copytree("kaynak_klasor", "hedef_klasor")  # tüm alt klasörlerle

# Dosya/klasör taşıma
shutil.move("kaynak.txt", "hedef/")

# Klasör silme
shutil.rmtree("silinecek_klasor")  # içindeki her şeyle birlikte sil

# Arşivleme
shutil.make_archive("yedek", "zip", "kaynak_klasor")  # zip/tar
shutil.unpack_archive("yedek.zip", "cikti_klasor")     # aç

# Disk kullanımı
kullanim = shutil.disk_usage("/")
print(f"Toplam: {kullanim.total / 1e9:.1f} GB")
print(f"Kullanılan: {kullanim.used / 1e9:.1f} GB")
print(f"Boş: {kullanim.free / 1e9:.1f} GB")
```

### tempfile ile Geçici Dosyalar

```python
import tempfile
import os

# Geçici dosya (otomatik silinir)
with tempfile.TemporaryFile(mode="w+t") as f:
    f.write("Geçici veri")
    f.seek(0)
    print(f.read())  # "Geçici veri"
    # with bloğu sonunda otomatik silinir

# Geçici dosya (ismiyle erişim)
with tempfile.NamedTemporaryFile(delete=False, suffix=".txt") as f:
    f.write(b"Gecici veri")
    gecici_ad = f.name

print(f"Geçici dosya: {gecici_ad}")
os.unlink(gecici_ad)  # manuel sil

# Geçici klasör
with tempfile.TemporaryDirectory() as tmpdir:
    dosya_yolu = os.path.join(tmpdir, "test.txt")
    with open(dosya_yolu, "w") as f:
        f.write("test")
    print(f"Dosya oluşturuldu: {dosya_yolu}")
    # with bloğu sonunda klasör ve içindekiler silinir

# Kalıcı geçici dizin
tmpdir = tempfile.mkdtemp(prefix="python_")
print(f"Geçici dizin: {tmpdir}")
# Kullanımdan sonra temizle: shutil.rmtree(tmpdir)
```

### glob ile Dosya Desen Eşleme

```python
import glob

# Belirli desendeki tüm dosyalar
py_dosyalar = glob.glob("**/*.py", recursive=True)
for dosya in py_dosyalar:
    print(dosya)

# Belirli bir klasördeki tüm .txt dosyaları
txt_dosyalar = glob.glob("veriler/*.txt")

# Alternatif: pathlib ile
from pathlib import Path
for dosya in Path(".").rglob("*.py"):
    print(dosya)
```

## <a id="hata-yonetimi"></a>Hata Yönetimi (Exception Handling):

### try-except

```python
try:
    sayi = int(input("Bir sayı girin: "))
    sonuc = 10 / sayi
    print(f"Sonuç: {sonuc}")
except ValueError:
    print("Geçerli bir sayı girmediniz!")
except ZeroDivisionError:
    print("Sıfıra bölme hatası!")
except Exception as e:
    print(f"Beklenmeyen hata: {e}")
```

### try-except-else-finally

```python
try:
    dosya = open("veri.txt", "r")
    icerik = dosya.read()
except FileNotFoundError:
    print("Dosya bulunamadı!")
else:
    # Hata oluşmazsa çalışır
    print(f"Dosya içeriği: {icerik}")
finally:
    # Her zaman çalışır (hata olsa da olmasa da)
    print("Bu kısım her zaman çalışır.")
    try:
        dosya.close()
    except NameError:
        pass
```

### Yaygın İstisna Türleri

```python
# ValueError: hatalı değer
int("abc")  # ValueError

# TypeError: hatalı tür
"5" + 5  # TypeError

# IndexError: liste indeksi hatası
[1, 2][5]  # IndexError

# KeyError: sözlük anahtarı hatası
{}["anahtar"]  # KeyError

# AttributeError: özellik/metot hatası
"metin".olmayan_metod()  # AttributeError

# FileNotFoundError: dosya bulunamadı
open("olmayan.txt")  # FileNotFoundError

# ZeroDivisionError: sıfıra bölme
1 / 0  # ZeroDivisionError

# ImportError: modül bulunamadı
import olmayan_modul  # ImportError

# StopIteration: iterator sonu
next(iter([]))  # StopIteration
```

### Exception Chain (Exception Zincirleme)

```python
# raise ... from ... ile hata zinciri
def veritabani_sorgula(id):
    try:
        raise ConnectionError("Veritabanı bağlantısı koptu")
    except ConnectionError as e:
        raise RuntimeError("Sorgu başarısız") from e

try:
    veritabani_sorgula(1)
except RuntimeError as e:
    print(f"Hata: {e}")
    print(f"Sebep: {e.__cause__}")  # ConnectionError

# raise ... from None ile zinciri gizleme
def gizle():
    try:
        int("abc")
    except ValueError:
        raise RuntimeError("Dönüşüm hatası") from None
```

### try-except'in Diğer Kullanım Şekilleri

```python
# try-except-else-finally kapsamlı örnek
def dosya_oku(dosya_adi):
    dosya = None
    try:
        dosya = open(dosya_adi, "r")
        icerik = dosya.read()
    except FileNotFoundError:
        print("Dosya bulunamadı, yeni oluşturuluyor...")
        with open(dosya_adi, "w") as f:
            f.write("")
        return ""
    except PermissionError:
        print("Dosya izni reddedildi!")
        raise
    else:
        # Hata yoksa çalışır
        print(f"Dosya başarıyla okundu ({len(icerik)} karakter)")
        return icerik
    finally:
        if dosya:
            dosya.close()
            print("Dosya kapatıldı")

# Birden çok hatayı yakalama
try:
    sonuc = 10 / int(input("Sayı: "))
except (ValueError, ZeroDivisionError) as e:
    print(f"Hata: {e}")

# try-except ile EAFP (Easier to Ask Forgiveness than Permission)
# LBYL (Look Before You Leap) yerine EAFP tercih edilir

# LBYL (daha az Pythonic):
if isinstance(x, int) and y != 0:
    sonuc = x / y
else:
    print("Hatalı değer")

# EAFP (daha Pythonic):
try:
    sonuc = x / y
except (TypeError, ZeroDivisionError) as e:
    print(f"Hata: {e}")
```

### raise ile Hata Fırlatma

```python
def yas_kontrol(yas):
    if yas < 0:
        raise ValueError("Yaş negatif olamaz!")
    if yas > 150:
        raise ValueError("Bu yaşta biri olamaz!")
    return "Geçerli yaş"

try:
    print(yas_kontrol(-5))
except ValueError as e:
    print(f"Hata: {e}")

# Hata yeniden fırlatma
def islem_yap(x):
    try:
        return 10 / x
    except ZeroDivisionError:
        print("Log: sıfıra bölme")
        raise  # aynı hatayı tekrar fırlat
```

### Özel İstisna Sınıfları

```python
class DogrulamaHatasi(Exception):
    """Özel doğrulama hatası."""
    pass

class KullaniciBulunamadi(DogrulamaHatasi):
    def __init__(self, kullanici_adi, mesaj="Kullanıcı bulunamadı"):
        self.kullanici_adi = kullanici_adi
        self.mesaj = mesaj
        super().__init__(f"{mesaj}: {kullanici_adi}")

def giris_yap(kullanici_adi, sifre):
    if kullanici_adi != "admin":
        raise KullaniciBulunamadi(kullanici_adi)
    if sifre != "1234":
        raise DogrulamaHatasi("Hatalı şifre!")
    return "Giriş başarılı"

try:
    print(giris_yap("user", "1234"))
except KullaniciBulunamadi as e:
    print(e)
except DogrulamaHatasi as e:
    print(f"Doğrulama hatası: {e}")
```

### assert

```python
# assert: debug amaçlı, koşul yanlışsa AssertionError
def bol(a, b):
    assert b != 0, "Bölen sıfır olamaz!"
    return a / b

# Python -O ile çalıştırılırsa assert'ler devre dışı kalır
```

### 🛠️ Sık Karşılaşılan Hatalar ve Çözümleri

| Hata | Anlamı | Çözüm |
|------|--------|-------|
| `SyntaxError: invalid syntax` | Yazım hatası | Satır sonunda `:` unuttunuz mu? Parantezleri kapattınız mı? |
| `NameError: name 'x' is not defined` | Değişken tanımlı değil | Değişkeni tanımladığınızdan emin olun, yazımı kontrol edin |
| `TypeError: can only concatenate str...` | Yanlış türde işlem | String ile sayıyı toplamaya çalışmayın: `"a" + 5` ❌ |
| `IndexError: list index out of range` | Liste indeksi sınır dışı | Listenin uzunluğundan büyük indeks kullanmayın |
| `KeyError: 'anahtar'` | Sözlükte anahtar yok | `.get()` kullanın veya `in` ile kontrol edin |
| `ValueError: invalid literal for int()` | String sayıya çevrilemiyor | `int("abc")` çalışmaz, sadece `int("42")" çalışır |
| `ZeroDivisionError: division by zero` | Sıfıra bölme | Bölmeden önce sıfır kontrolü yapın |
| `FileNotFoundError` | Dosya bulunamadı | Dosya yolunu kontrol edin, dizinde var mı? |
| `AttributeError: 'int' object has no attribute...` | Yanlış türde metot çağrısı | Değişkenin türünü kontrol edin (`type()`) |
| `ImportError: No module named '...'` | Kütüphane yüklü değil | `pip install ...` ile kurun |

**Hata ile karşılaştığınızda:**
1. Hatayı okuyun — Python hata mesajları genelde neyin yanlış olduğunu söyler
2. Hatanın hangi satırda olduğuna bakın
3. Google'a hatayı yazıştırın (başka birinin çözmüş olma ihtimali yüksek)
4. ChatGPT'ye hatayı sorun
5. Pes etmeyin — her hata sizi daha iyi bir programcı yapar!

---

## <a id="logging"></a>Loglama (logging):

Loglama, uygulama çalışırken bilgi toplamak için kullanılır. `print()`'ten daha esnek ve profesyoneldir.

### Temel Loglama

```python
import logging

# Basit yapılandırma
logging.basicConfig(
    level=logging.DEBUG,
    format="%(asctime)s - %(name)s - %(levelname)s - %(message)s",
    datefmt="%Y-%m-%d %H:%M:%S",
)

logging.debug("Hata ayıklama mesajı")
logging.info("Bilgi mesajı")
logging.warning("Uyarı mesajı")
logging.error("Hata mesajı")
logging.critical("Kritik hata mesajı")
```

### Log Seviyeleri

| Seviye    | Değer | Kullanım                     |
|-----------|-------|------------------------------|
| DEBUG     | 10    | Detaylı hata ayıklama        |
| INFO      | 20    | Genel bilgi mesajları        |
| WARNING   | 30    | Uyarı (önemli değil)         |
| ERROR     | 40    | Hata (uygulama çalışır)      |
| CRITICAL  | 50    | Kritik hata (uygulama durabilir) |

### Logger Oluşturma

```python
import logging

# Kendi logger'ını oluştur
logger = logging.getLogger(__name__)
logger.setLevel(logging.DEBUG)

# Handler'lar (çıktı hedefleri)
console_handler = logging.StreamHandler()
console_handler.setLevel(logging.INFO)

dosya_handler = logging.FileHandler("uygulama.log", encoding="utf-8")
dosya_handler.setLevel(logging.DEBUG)

# Format
formatter = logging.Formatter(
    "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
)
console_handler.setFormatter(formatter)
dosya_handler.setFormatter(formatter)

# Handler'ları logger'a ekle
logger.addHandler(console_handler)
logger.addHandler(dosya_handler)

# Kullanım
logger.info("Uygulama başladı")
logger.error("Bir hata oluştu!")
```

### Rotating File Handler

```python
import logging
from logging.handlers import RotatingFileHandler, TimedRotatingFileHandler

# Boyuta göre dönen log dosyası
handler = RotatingFileHandler(
    "app.log",
    maxBytes=1024 * 1024,  # 1 MB
    backupCount=5,  # 5 eski dosyayı sakla
    encoding="utf-8",
)

# Zamana göre dönen log dosyası
time_handler = TimedRotatingFileHandler(
    "app.log",
    when="midnight",  # her gece yarısı yeni dosya
    interval=1,
    backupCount=30,  # 30 günlük log sakla
    encoding="utf-8",
)

logging.basicConfig(
    level=logging.INFO,
    handlers=[handler, time_handler],
)
```

### Log Yapılandırma Dosyası

```python
import logging.config

# Dict config
LOGGING_CONFIG = {
    "version": 1,
    "formatters": {
        "standart": {
            "format": "%(asctime)s - %(name)s - %(levelname)s - %(message)s"
        },
        "basit": {
            "format": "%(levelname)s: %(message)s"
        },
    },
    "handlers": {
        "console": {
            "class": "logging.StreamHandler",
            "level": "INFO",
            "formatter": "basit",
        },
        "dosya": {
            "class": "logging.handlers.RotatingFileHandler",
            "level": "DEBUG",
            "formatter": "standart",
            "filename": "app.log",
            "maxBytes": 1048576,
            "backupCount": 5,
        },
    },
    "root": {
        "level": "DEBUG",
        "handlers": ["console", "dosya"],
    },
}

logging.config.dictConfig(LOGGING_CONFIG)
logger = logging.getLogger(__name__)
```

### structlog ile Yapısal Loglama (opsiyonel)

```python
# pip install structlog
import structlog

logger = structlog.get_logger()
logger.info("Kullanıcı giriş yaptı",
    kullanici="ali",
    ip_adresi="192.168.1.1",
    rol="admin",
)
```

## <a id="moduller-ve-paketler"></a>Modüller ve Paketler:

### Modül İçe Aktarma

```python
# Tam modül içe aktarma
import math
print(math.sqrt(16))  # 4.0

# Belirli fonksiyonları içe aktarma
from math import sqrt, pi
print(sqrt(16))  # 4.0
print(pi)        # 3.141592653589793

# Takma ad ile
import math as m
print(m.sqrt(16))

# Tümünü içe aktarma (önerilmez, namespace kirliliği)
from math import *
```

### Kendi Modülünü Oluşturma

```python
# islemler.py
def topla(a, b):
    return a + b

def carp(a, b):
    return a * b

PI = 3.14159

# ana.py
import islemler
print(islemler.topla(3, 5))
print(islemler.carp(4, 5))
print(islemler.PI)

# Veya
from islemler import topla, PI
```

### __name__ == "__main__"

```python
# islemler.py
def topla(a, b):
    return a + b

if __name__ == "__main__":
    # Bu kısım sadece doğrudan çalıştırıldığında çalışır
    print("Modül testi:")
    print(topla(3, 5))

# Bu sayede modül hem başka yerden import edilebilir
    # hem de doğrudan çalıştırılıp test edilebilir

### __all__ Kullanımı

```python
# islemler.py
__all__ = ["topla", "carp", "PI"]  # from islemler import * ile alınacaklar

def topla(a, b):
    return a + b

def carp(a, b):
    return a * b

PI = 3.14159

def gizli_fonksiyon():  # __all__'da olmadığı için dışa aktarılmaz
    pass

# __all__ ayrıca IDE'lerde otomatik tamamlama için de kullanılır
```

### __init__.py Kullanımı

```python
# my_package/__init__.py
from .modul1 import FonksiyonA
from .modul2 import FonksiyonB

__all__ = ["FonksiyonA", "FonksiyonB"]

# Artık şu şekilde kullanılabilir:
# from my_package import FonksiyonA
# (doğrudan paket seviyesinden erişim)
```

### sys.path ve İçe Aktarma Mekanizması

```python
import sys

# Python modül arama yolları
print(sys.path)
# ['', '/usr/lib/python3.12', '/usr/local/lib/python3.12/site-packages', ...]

# Yeni yol ekleme
sys.path.append("/home/user/benim_modullerim")

# PYTHONPATH ortam değişkeni ile de yol eklenebilir
# export PYTHONPATH="/home/user/benim_modullerim:$PYTHONPATH"

# Reload: modülü yeniden yükle
import importlib
import my_module
importlib.reload(my_module)
```

### Paket Oluşturma

```
my_package/
    __init__.py
    modul1.py
    modul2.py
    alt_paket/
        __init__.py
        modul3.py
```

```python
# __init__.py genellikle boş olabilir veya paket başlatma kodu içerebilir

# Kullanım:
from my_package import modul1
from my_package.modul2 import fonksiyon
from my_package.alt_paket import modul3
```

### Yaygın Standart Kütüphane Modülleri

```python
import os        # İşletim sistemi işlemleri
import sys       # Sistem parametreleri
import math      # Matematiksel fonksiyonlar
import random    # Rastgele sayı üretimi
import datetime  # Tarih ve zaman
import json      # JSON işlemleri
import re        # Düzenli ifadeler
import csv       # CSV işlemleri
import sqlite3   # SQLite veritabanı
import hashlib   # Hash fonksiyonları
import itertools # Iterator araçları
import collections  # Özel koleksiyonlar
import pathlib   # Dosya yolları (modern)
import statistics # İstatistiksel işlemler
import urllib    # URL işlemleri
import email     # E-posta işlemleri
import gzip      # Sıkıştırma
import tarfile   # Arşiv işlemleri
import tempfile  # Geçici dosyalar
import logging   # Loglama
import argparse  # Komut satırı argümanları
import unittest  # Test framework
import doctest   # Dokümantasyon testleri
from datetime import datetime, timedelta, date
from collections import defaultdict, Counter, namedtuple, deque
from itertools import chain, combinations, permutations, product

### argparse ile Komut Satırı Argümanları

```python
import argparse

parser = argparse.ArgumentParser(
    description="Örnek komut satırı aracı",
    epilog="Kullanım: python script.py -i giris.txt -o cikti.txt",
)

# Konumsal argüman (zorunlu)
parser.add_argument("dosya", help="İşlenecek dosya")

# Opsiyonel argümanlar
parser.add_argument("-o", "--output", help="Çıktı dosyası", default="cikti.txt")
parser.add_argument("-v", "--verbose", action="store_true", help="Ayrıntılı çıktı")
parser.add_argument("-n", "--number", type=int, help="Sayısal değer", default=1)
parser.add_argument("-m", "--mode", choices=["okuma", "yazma"], help="İşlem modu")

args = parser.parse_args()

print(f"Dosya: {args.dosya}")
print(f"Çıktı: {args.output}")
print(f"Ayrıntılı: {args.verbose}")
print(f"Sayı: {args.number}")
print(f"Mod: {args.mode}")

# Kullanım:
# python script.py giris.txt -o cikti.json -v -n 5 -m yazma
```

```python
# Alt komutlar (git pull, git push benzeri)
parser = argparse.ArgumentParser()
alt_komutlar = parser.add_subparsers(dest="komut", help="Alt komutlar")

# ekle komutu
ekle = alt_komutlar.add_parser("ekle", help="Yeni kayıt ekle")
ekle.add_argument("isim", help="Kullanıcı adı")
ekle.add_argument("--yas", type=int, help="Yaş")

# sil komutu
sil = alt_komutlar.add_parser("sil", help="Kayıt sil")
sil.add_argument("id", help="Silinecek ID")
sil.add_argument("--force", action="store_true", help="Zorla sil")

args = parser.parse_args()
if args.komut == "ekle":
    print(f"Kullanıcı eklendi: {args.isim}")
elif args.komut == "sil":
    print(f"ID {args.id} siliniyor (force={args.force})")
```

```python
# click ile daha modern yaklaşım
# pip install click

import click

@click.command()
@click.argument("isim")
@click.option("--sayi", default=1, help="Tekrar sayısı")
@click.option("--buyuk-harf/--kucuk-harf", default=False, help="Büyük harf")
def selamla(isim, sayi, buyuk_harf):
    for _ in range(sayi):
        metin = isim.upper() if buyuk_harf else isim
        click.echo(f"Merhaba {metin}")

if __name__ == "__main__":
    selamla()
```

### sys.argv ile Temel Argümanlar

```python
import sys

if len(sys.argv) < 2:
    print("Kullanım: python script.py <isim>")
    sys.exit(1)

isim = sys.argv[1]
print(f"Merhaba {isim}")
# sys.argv[0]: script adı
# sys.argv[1:]: argümanlar
```

## <a id="tarih-ve-zaman-islemleri"></a>Tarih ve Zaman İşlemleri:

### datetime Modülü

```python
from datetime import datetime, date, time, timedelta

# Şu anki zaman
simdi = datetime.now()
print(simdi)  # 2026-05-25 14:30:00.123456

# Belirli tarih
tarih = datetime(2026, 5, 25, 14, 30, 0)

# Tarih bileşenleri
print(simdi.year)    # 2026
print(simdi.month)   # 5
print(simdi.day)     # 25
print(simdi.hour)    # 14
print(simdi.minute)  # 30
print(simdi.second)  # 0
print(simdi.weekday())  # 0=Pazartesi, 6=Pazar

# Sadece tarih / sadece saat
bugun = date.today()
su_an = datetime.now().time()

# timedelta: zaman farkı
bir_hafta = timedelta(weeks=1)
gelecek_hafta = simdi + bir_hafta
uc_gun_once = simdi - timedelta(days=3)
iki_saat_sonra = simdi + timedelta(hours=2)

# İki tarih arasındaki fark
fark = gelecek_hafta - simdi
print(fark.days)          # 7
print(fark.total_seconds())  # 604800.0
```

### Tarih Biçimlendirme

```python
from datetime import datetime

simdi = datetime.now()

# strftime: datetime -> string
print(simdi.strftime("%Y-%m-%d"))           # 2026-05-25
print(simdi.strftime("%d/%m/%Y"))           # 25/05/2026
print(simdi.strftime("%H:%M:%S"))           # 14:30:00
print(simdi.strftime("%A, %d %B %Y"))       # Monday, 25 May 2026
print(simdi.strftime("%c"))                 # Mon May 25 14:30:00 2026

# Yaygın formatlar:
# %Y: yıl (4 hane), %y: yıl (2 hane)
# %m: ay (01-12), %B: ay adı, %b: ay kısaltma
# %d: gün (01-31), %A: gün adı, %a: gün kısaltma
# %H: saat (24), %I: saat (12), %p: AM/PM
# %M: dakika, %S: saniye
# %f: mikrosaniye

# strptime: string -> datetime
tarih_str = "2026-05-25 14:30:00"
tarih = datetime.strptime(tarih_str, "%Y-%m-%d %H:%M:%S")
```

### time Modülü

```python
import time

# Zaman damgası (timestamp)
print(time.time())  # 1716634200.123456 (Unix epoch, saniye)

# Uyuma
print("Başla")
time.sleep(2)  # 2 saniye bekle
print("2 saniye sonra")

# Zaman ölçme
baslangic = time.perf_counter()
# ... işlem ...
bitis = time.perf_counter()
print(f"Geçen süre: {bitis - baslangic:.4f} saniye")

# timeit modülü ile hassas ölçüm
import timeit
sure = timeit.timeit('sum(range(1000))', number=10000)
print(f"10000 tekrar: {sure:.4f} saniye")
```

## <a id="json-ve-veri-serilestirme"></a>JSON ve Veri Serileştirme:

### JSON İşlemleri

```python
import json

# Python -> JSON (serialization)
veri = {
    "isim": "Ali",
    "yas": 25,
    "aktif": True,
    "notlar": [85, 90, 78],
    "adres": None
}

json_str = json.dumps(veri, indent=2, ensure_ascii=False)
print(json_str)
# {
#   "isim": "Ali",
#   "yas": 25,
#   "aktif": true,
#   "notlar": [85, 90, 78],
#   "adres": null
# }

# JSON -> Python (deserialization)
json_str = '{"isim": "Ali", "yas": 25}'
veri = json.loads(json_str)
print(veri["isim"])  # Ali

# Dosyaya yazma / okuma
with open("veri.json", "w", encoding="utf-8") as f:
    json.dump(veri, f, indent=2, ensure_ascii=False)

with open("veri.json", "r", encoding="utf-8") as f:
    veri = json.load(f)

# Özel nesneleri JSON'a çevirme
from datetime import datetime

def json_serilestirici(obj):
    if isinstance(obj, datetime):
        return obj.isoformat()
    raise TypeError(f"{type(obj)} serileştirilemez")

veri = {"zaman": datetime.now()}
json_str = json.dumps(veri, default=json_serilestirici)
print(json_str)  # {"zaman": "2026-05-25T14:30:00"}
```

### Pickle (Python'a özel serileştirme)

```python
import pickle

# pickle: Python nesnelerini binary serileştirir
veri = {"isim": "Ali", "sayilar": [1, 2, 3]}

# Yazma
with open("veri.pkl", "wb") as f:
    pickle.dump(veri, f)

# Okuma
with open("veri.pkl", "rb") as f:
    yuklenen = pickle.load(f)

# Pickle güvenli değildir (güvenilmeyen kaynaklardan yüklemeyin!)
```

## <a id="duzenli-ifadeler"></a>Düzenli İfadeler (Regex):

### re Modülü

```python
import re

# match(): baştan eşleşme arar
metin = "Merhaba Dünya"
sonuc = re.match(r"Merhaba", metin)
print(sonuc.group() if sonuc else "Yok")  # Merhaba

# search(): herhangi bir yerde eşleşme arar
sonuc = re.search(r"Dünya", metin)
print(sonuc.span())  # (8, 13)

# findall(): tüm eşleşmeleri bulur
sonuc = re.findall(r"\d+", "42 elma, 13 armut, 5 muz")
print(sonuc)  # ['42', '13', '5']

# finditer(): iterator olarak tüm eşleşmeler
for eslesme in re.finditer(r"\d+", "42 elma, 13 armut"):
    print(eslesme.group(), eslesme.span())

# split(): eşleşmeye göre böl
parcalar = re.split(r"[,;]\s*", "elma, armut; muz, çilek")
print(parcalar)  # ['elma', 'armut', 'muz', 'çilek']

# sub(): eşleşmeyi değiştir
sonuc = re.sub(r"\d+", "X", "42 elma, 13 armut")
print(sonuc)  # "X elma, X armut"

# sub() fonksiyon ile
sonuc = re.sub(r"\d+", lambda m: str(int(m.group()) * 2), "1, 2, 3")
print(sonuc)  # "2, 4, 6"
```

### Regex Desenleri

```python
import re

# Temel desenler
# .  : herhangi bir karakter
# ^  : satır başı
# $  : satır sonu
# *  : 0 veya daha çok
# +  : 1 veya daha çok
# ?  : 0 veya 1
# {n}: tam n tane
# {n,}: n veya daha çok
# {n,m}: n ile m arası
# [] : karakter sınıfı
# |  : alternatif (veya)
# () : gruplama
# \  : kaçış veya özel dizi

# Karakter sınıfları
# [abc]  : a, b veya c
# [a-z]  : a'dan z'ye
# [0-9]  : rakam
# [^abc] : a, b, c dışındakiler

# Özel diziler
# \d : rakam (digit)     = [0-9]
# \D : rakam olmayan     = [^0-9]
# \w : harf/rakam/_      = [a-zA-Z0-9_]
# \W : harf/rakam/_ olmayan
# \s : boşluk karakteri  = [ \t\n\r\f\v]
# \S : boşluk olmayan
# \b : kelime sınırı
# \B : kelime sınırı olmayan

# Örnekler
# E-posta:
desen = r"[\w.+-]+@[\w-]+\.[\w.]+"
print(re.findall(desen, "ali@mail.com, veli@site.org.tr"))

# Telefon (Türkiye):
desen = r"0\d{3} \d{3} \d{2} \d{2}"

# URL:
desen = r"https?://(?:[-\w.]|(?:%[\da-fA-F]{2}))+"

# IP adresi:
desen = r"\d{1,3}\.\d{1,3}\.\d{1,3}\.\d{1,3}"
```

### Gruplama ve Yakalama

```python
import re

# Grup yakalama
metin = "Ali: 25 yaşında, Veli: 30 yaşında"
desen = r"(\w+): (\d+)"
for eslesme in re.finditer(desen, metin):
    print(f"İsim: {eslesme.group(1)}, Yaş: {eslesme.group(2)}")

# Grup isimlendirme
desen = r"(?P<isim>\w+): (?P<yas>\d+)"
for eslesme in re.finditer(desen, metin):
    print(f"{eslesme.group('isim')}: {eslesme.group('yas')}")

# Derlenmiş regex (performans için)
desen = re.compile(r"\d{3}-\d{3}-\d{4}")
sonuc = desen.search("Tel: 555-123-4567")
print(sonuc.group())  # 555-123-4567
```

## <a id="nesne-yonelimli-programlama"></a>Nesne Yönelimli Programlama (OOP):

### Sınıf Tanımlama

```python
class Ogrenci:
    # Sınıf değişkeni (tüm örnekler tarafından paylaşılır)
    okul = "Python Lisesi"
    ogrenci_sayisi = 0

    # Yapıcı metot (constructor)
    def __init__(self, ad, soyad, yas):
        # Örnek değişkenleri (her örneğe özel)
        self.ad = ad
        self.soyad = soyad
        self.yas = yas
        self.notlar = []
        Ogrenci.ogrenci_sayisi += 1

    # Örnek metodu
    def tam_ad(self):
        return f"{self.ad} {self.soyad}"

    def not_ekle(self, not_degeri):
        self.notlar.append(not_degeri)

    def not_ortalamasi(self):
        if not self.notlar:
            return 0
        return sum(self.notlar) / len(self.notlar)

    # Sınıf metodu
    @classmethod
    def sinifi_degistir(cls, yeni_okul):
        cls.okul = yeni_okul

    # Statik metot
    @staticmethod
    def yas_kontrol(yas):
        return 0 <= yas <= 150

    # String temsili
    def __str__(self):
        return f"{self.tam_ad()} ({self.yas} yaşında)"

    def __repr__(self):
        return f"Ogrenci('{self.ad}', '{self.soyad}', {self.yas})"
```

### Sınıf Kullanımı

```python
# Örnek oluşturma
ogr1 = Ogrenci("Ali", "Yılmaz", 20)
ogr2 = Ogrenci("Ayşe", "Demir", 22)

# Örnek değişkenlerine erişim
print(ogr1.ad)       # Ali
print(ogr1.tam_ad()) # Ali Yılmaz

# Metot çağrısı
ogr1.not_ekle(85)
ogr1.not_ekle(90)
ogr1.not_ekle(78)
print(ogr1.not_ortalamasi())  # 84.333...

# Sınıf değişkenine erişim
print(ogr1.okul)        # Python Lisesi
print(Ogrenci.okul)     # Python Lisesi

# Sınıf metodu
Ogrenci.sinifi_degistir("Python Üniversitesi")
print(ogr1.okul)        # Python Üniversitesi

# Statik metot
print(Ogrenci.yas_kontrol(25))   # True
print(Ogrenci.yas_kontrol(200))  # False

# Sınıf değişkeni
print(Ogrenci.ogrenci_sayisi)  # 2

# String temsili
print(ogr1)     # Ali Yılmaz (20 yaşında)
print(repr(ogr1))  # Ogrenci('Ali', 'Yılmaz', 20)
```

### Özel Metotlar (Magic Methods / Dunder Methods)

```python
class Vektor:
    def __init__(self, x, y):
        self.x = x
        self.y = y

    # String temsilleri
    def __str__(self):
        return f"Vektor({self.x}, {self.y})"

    def __repr__(self):
        return f"Vektor({self.x}, {self.y})"

    # Aritmetik operatörler
    def __add__(self, other):
        return Vektor(self.x + other.x, self.y + other.y)

    def __sub__(self, other):
        return Vektor(self.x - other.x, self.y - other.y)

    def __mul__(self, skaler):
        return Vektor(self.x * skaler, self.y * skaler)

    def __truediv__(self, skaler):
        return Vektor(self.x / skaler, self.y / skaler)

    # Karşılaştırma
    def __eq__(self, other):
        return self.x == other.x and self.y == other.y

    def __lt__(self, other):
        return self.buyukluk() < other.buyukluk()

    # len() desteği
    def __len__(self):
        return 2

    # getitem desteği (indeksleme)
    def __getitem__(self, index):
        if index == 0:
            return self.x
        elif index == 1:
            return self.y
        raise IndexError("Vektör indeksi 0 veya 1 olmalı")

    # bool desteği
    def __bool__(self):
        return self.x != 0 or self.y != 0

    def buyukluk(self):
        return (self.x ** 2 + self.y ** 2) ** 0.5

# Kullanım
v1 = Vektor(3, 4)
v2 = Vektor(1, 2)

print(v1 + v2)   # Vektor(4, 6)
print(v1 * 2)    # Vektor(6, 8)
print(v1 == Vektor(3, 4))  # True
print(v1 > v2)   # True (5 > 2.236)
print(v1[0])     # 3
print(len(v1))   # 2
print(bool(Vektor(0, 0)))  # False
```

### __slots__ ile Bellek Optimizasyonu

```python
# Normal sınıf: her örnek __dict__ taşır (fazla bellek)
class NormalOgrenci:
    def __init__(self, ad, yas):
        self.ad = ad
        self.yas = yas

# __slots__ ile: __dict__ oluşturulmaz, tuple/sabit dizi kullanılır
class SlotOgrenci:
    __slots__ = ("ad", "yas")  # sadece bu alanlar kullanılabilir

    def __init__(self, ad, yas):
        self.ad = ad
        self.yas = yas

# Karşılaştırma
import sys
n = NormalOgrenci("Ali", 20)
s = SlotOgrenci("Ali", 20)
print(f"Normal: {sys.getsizeof(n.__dict__)} bayt")  # ~104 bayt
# SlotOgrenci'nin __dict__'i yoktur

# __slots__ avantajları:
# - Daha az bellek kullanımı (~%40-50 tasarruf)
# - Daha hızlı attribute erişimi
# - Yeni attribute eklenmesini engeller (hata yakalama)

# Miras ile __slots__:
class AktifOgrenci(SlotOgrenci):
    __slots__ = ("aktif",)  # alt sınıf da __slots__ tanımlamalı

    def __init__(self, ad, yas, aktif=True):
        super().__init__(ad, yas)
        self.aktif = aktif

# a.bolum = "Bilgisayar"  # AttributeError! (__slots__ dışında)
```

### Özellikler (Property)

```python
class Daire:
    def __init__(self, yaricap):
        self._yaricap = 0
        self.yaricap = yaricap  # property setter kullanır

    @property
    def yaricap(self):
        """Yarıçap değeri."""
        return self._yaricap

    @yaricap.setter
    def yaricap(self, deger):
        if deger < 0:
            raise ValueError("Yarıçap negatif olamaz!")
        self._yaricap = deger

    @property
    def alan(self):
        """Daire alanı (sadece okunur)."""
        return 3.14159 * self._yaricap ** 2

    @property
    def cevre(self):
        """Daire çevresi (sadece okunur)."""
        return 2 * 3.14159 * self._yaricap

# Kullanım
d = Daire(5)
print(d.yaricap)  # 5 (getter)
print(d.alan)     # 78.53975 (property)
print(d.cevre)    # 31.4159

d.yaricap = 10     # setter
print(d.alan)      # 314.159
```

### Bu Bölümde Öğrendikleriniz
- ✅ Sınıf tanımlama ve nesne oluşturma
- ✅ `__init__` constructor
- ✅ Self parametresi
- ✅ Metot tanımlama
- ✅ Sınıf değişkenleri vs örnek değişkenleri
- ✅ `@property` ile getter/setter
- ✅ Magic metodlar (`__str__`, `__repr__`)

### 🛠️ Alıştırmalar

**Temel:**
1. `Kitap` adında bir sınıf oluşturun: özellikleri `baslik`, `yazar`, `sayfa_sayisi` olsun. `__str__` metodu ile kitap bilgilerini yazdırın.
2. `Kitap` sınıfından 3 farklı kitap nesnesi oluşturun ve özelliklerini yazdırın.
3. Sınıfa `kalin_mi()` metodu ekleyin: 300 sayfadan fazlaysa True dönsün.

**Orta:**
4. Sınıfa `sayfa_sayisi` için `@property` ekleyin: negatif değer girilmesin
5. Sınıfa bir `kutuphane` sınıf değişkeni ekleyin: her yeni kitap eklendiğinde saysın

**🎮 Mini Proje: Kütüphane Yönetim Sistemi**
```python
class Kitap:
    def __init__(self, baslik, yazar, sayfa, adet=1):
        self.baslik = baslik
        self.yazar = yazar
        self.sayfa = sayfa
        self.adet = adet
    
    def __str__(self):
        return f"{self.baslik} - {self.yazar} ({self.sayfa} sf, {self.adet} adet)"

class Kutuphane:
    def __init__(self):
        self.kitaplar = []
    
    def kitap_ekle(self, kitap):
        self.kitaplar.append(kitap)
        print(f"📖 {kitap.baslik} eklendi!")
    
    def kitaplari_listele(self):
        if not self.kitaplar:
            print("Kütüphane boş!")
            return
        print(f"\nKütüphanede {len(self.kitaplar)} kitap var:")
        for i, kitap in enumerate(self.kitaplar, 1):
            print(f"{i}. {kitap}")
    
    def ara(self, kelime):
        sonuc = [k for k in self.kitaplar 
                 if kelime.lower() in k.baslik.lower()]
        if sonuc:
            print(f"Bulunan {len(sonuc)} kitap:")
            for k in sonuc:
                print(f"  - {k}")
        else:
            print("Kitap bulunamadı!")

# Kullanım
kutup = Kutuphane()
kutup.kitap_ekle(Kitap("Savaş ve Barış", "Tolstoy", 1225))
kutup.kitap_ekle(Kitap("1984", "George Orwell", 328, 3))
kutup.kitap_ekle(Kitap("Hayvan Çiftliği", "George Orwell", 152, 2))
kutup.kitaplari_listele()
kutup.ara("barış")
```

Çalıştırın, sonra kendiniz geliştirin: kitap silme, ödünç verme, geçmiş ekleme gibi özellikler ekleyin.

---

## <a id="miras-alma"></a>Miras Alma (Inheritance):

### Temel Miras

```python
class Hayvan:
    def __init__(self, isim):
        self.isim = isim

    def ses_cikar(self):
        return "..."

    def __str__(self):
        return f"{self.isim} bir hayvan"

class Kopek(Hayvan):
    def __init__(self, isim, cins):
        super().__init__(isim)  # üst sınıfın constructor'ını çağır
        self.cins = cins

    def ses_cikar(self):  # metot ezme (override)
        return "Hav hav!"

    def kuyruk_salla(self):
        return f"{self.isim} kuyruk sallıyor"

class Kedi(Hayvan):
    def __init__(self, isim, tuylu=True):
        super().__init__(isim)
        self.tuylu = tuylu

    def ses_cikar(self):
        return "Miyav!"

# Kullanım
kopek = Kopek("Karabaş", "Kangal")
kedi = Kedi("Pamuk")

print(kopek.ses_cikar())     # Hav hav!
print(kedi.ses_cikar())      # Miyav!
print(kopek.kuyruk_salla())  # Karabaş kuyruk sallıyor
print(kopek.isim)            # Karabaş
print(isinstance(kopek, Kopek))  # True
print(isinstance(kopek, Hayvan)) # True
print(issubclass(Kopek, Hayvan)) # True
```

### Çoklu Miras

```python
class Ucabilen:
    def uc(self):
        return "Uçuyorum!"

class Yuzebilen:
    def yuz(self):
        return "Yüzüyorum!"

class Ordek(Yuzebilen, Ucabilen):  # MRO: Ordek -> Yuzebilen -> Ucabilen
    def ses_cikar(self):
        return "Vak vak!"

ordek = Ordek()
print(ordek.uc())          # Uçuyorum!
print(ordek.yuz())         # Yüzüyorum!
print(ordek.ses_cikar())   # Vak vak!

# MRO - Method Resolution Order
print(Ordek.__mro__)
# (<class 'Ordek'>, <class 'Yuzebilen'>, <class 'Ucabilen'>, <class 'object'>)
```

### Soyut Sınıflar (ABC)

```python
from abc import ABC, abstractmethod

class Sekil(ABC):  # Soyut sınıf
    @abstractmethod
    def alan(self):
        pass

    @abstractmethod
    def cevre(self):
        pass

    def bilgi(self):  # somut metot
        return f"Alan: {self.alan()}, Çevre: {self.cevre()}"

class Dikdortgen(Sekil):
    def __init__(self, en, boy):
        self.en = en
        self.boy = boy

    def alan(self):
        return self.en * self.boy

    def cevre(self):
        return 2 * (self.en + self.boy)

class Daire(Sekil):
    def __init__(self, r):
        self.r = r

    def alan(self):
        return 3.14159 * self.r ** 2

    def cevre(self):
        return 2 * 3.14159 * self.r

# sekil = Sekil()  # TypeError

dikdortgen = Dikdortgen(5, 3)
print(dikdortgen.bilgi())  # Alan: 15, Çevre: 16
```

## <a id="veri-siniflari-ve-enum"></a>Veri Sınıfları (dataclasses) ve Enum:

### dataclasses

`dataclasses` modülü, çoğunlukla veri depolamak için kullanılan sınıfları kolayca tanımlamayı sağlar. `__init__`, `__repr__`, `__eq__` gibi metotları otomatik oluşturur.

```python
from dataclasses import dataclass, field, asdict, astuple, replace
from typing import List, Optional

@dataclass
class Ogrenci:
    ad: str
    soyad: str
    yas: int
    notlar: List[float] = field(default_factory=list)  # mutable varsayılan
    aktif: bool = True

    @property
    def tam_ad(self):
        return f"{self.ad} {self.soyad}"

    def not_ortalamasi(self) -> float:
        if not self.notlar:
            return 0.0
        return sum(self.notlar) / len(self.notlar)

# Kullanım
ogr = Ogrenci("Ali", "Yılmaz", 20)
print(ogr)  # Ogrenci(ad='Ali', soyad='Yılmaz', yas=20, notlar=[], aktif=True)
ogr.notlar = [85, 90, 78]
print(ogr.not_ortalamasi())  # 84.333...

# değiştirilemez (immutable) dataclass
@dataclass(frozen=True)
class Nokta:
    x: float
    y: float

n = Nokta(3.0, 4.0)
# n.x = 5.0  # FrozenInstanceError!

# Sıralama desteği
@dataclass(order=True)
class Urun:
    fiyat: float
    isim: str

urunler = [Urun(15.0, "Kalem"), Urun(5.0, "Defter"), Urun(25.0, "Kitap")]
sirali = sorted(urunler)
print([(u.isim, u.fiyat) for u in sirali])
# [('Defter', 5.0), ('Kalem', 15.0), ('Kitap', 25.0)]

# Dönüşümler
sozluk = asdict(ogr)          # dict'e çevir
demet = astuple(ogr)          # tuple'a çevir
ogr2 = replace(ogr, yas=21)   # belirli alanları değiştir (yeni örnek)
```

### @dataclass Dekoratör Parametreleri

```python
from dataclasses import dataclass, field

@dataclass(
    init=True,          # __init__ oluştur (default: True)
    repr=True,          # __repr__ oluştur (default: True)
    eq=True,            # __eq__ oluştur (default: True)
    order=False,        # sıralama metotları (default: False)
    frozen=False,       # değiştirilemez (default: False)
    slots=False,        # __slots__ kullan (Python 3.10+)
    kw_only=False,      # keyword-only argümanlar (Python 3.10+)
)
class Ornek:
    x: int
    y: int = 0
    z: List[int] = field(default_factory=list, repr=False)
    gizli: str = field(default="sır", init=False)
```

### namedtuple

```python
from collections import namedtuple

# namedtuple: basit, immutable veri nesneleri
Nokta = namedtuple("Nokta", ["x", "y"])
p = Nokta(3, 4)
print(p.x, p.y)   # 3 4
print(p[0])        # 3 (tuple gibi indekslenebilir)
x, y = p           # unpacking

# _make, _asdict, _replace
p2 = Nokta._make([5, 6])
print(p2._asdict())  # {'x': 5, 'y': 6}
p3 = p2._replace(x=10)  # yeni örnek

# Typed namedtuple
from typing import NamedTuple

class Ogrenci(NamedTuple):
    ad: str
    yas: int
    bolum: str = "Bilgisayar"
```

### Enum

```python
from enum import Enum, auto, IntEnum, Flag, unique

class Renk(Enum):
    KIRMIZI = 1
    YESIL = 2
    MAVI = 3
    SARI = 4

# Erişim
print(Renk.KIRMIZI)          # Renk.KIRMIZI
print(Renk.KIRMIZI.name)     # "KIRMIZI"
print(Renk.KIRMIZI.value)    # 1
print(Renk(1))               # Renk.KIRMIZI (değerden bul)
print(Renk["KIRMIZI"])       # Renk.KIRMIZI (isimden bul)

# Döngü
for renk in Renk:
    print(f"{renk.name} = {renk.value}")

# Karşılaştırma
print(Renk.KIRMIZI is Renk(1))  # True
print(Renk.KIRMIZI == Renk.KIRMIZI)  # True
print(Renk.KIRMIZI != Renk.YESIL)  # True

# otomatik değer
class Durum(Enum):
    BEKLIYOR = auto()
    ISLENIYOR = auto()
    TAMAMLANDI = auto()
    HATA = auto()

print(list(Durum))  # [Durum.BEKLIYOR, Durum.ISLENIYOR, ...]

# Benzersizlik garantisi
@unique
class HttpStatus(Enum):
    OK = 200
    NOT_FOUND = 404
    SERVER_ERROR = 500
    # UNAUTHORIZED = 200  # ValueError: duplicate values

# IntEnum: karşılaştırma yapılabilir
class Oncelik(IntEnum):
    DUSUK = 1
    ORTA = 2
    YUKSEK = 3

print(Oncelik.YUKSEK > Oncelik.DUSUK)  # True
print(Oncelik.ORTA >= 2)  # True

# Flag: bitwise birleştirme
class Yetki(Flag):
    OKU = auto()     # 1
    YAZ = auto()     # 2
    SIL = auto()     # 4
    YONET = OKU | YAZ | SIL  # 7

kullanici_yetki = Yetki.OKU | Yetki.YAZ
print(Yetki.OKU in kullanici_yetki)  # True
print(Yetki.SIL in kullanici_yetki)  # False

# Enum pratik kullanım
class HataKodu(Enum):
    GECERSIZ_GIRIS = ("Geçersiz giriş", 1001)
    KULLANICI_BULUNAMADI = ("Kullanıcı bulunamadı", 1002)
    YETKISIZ_ERISIM = ("Yetkisiz erişim", 1003)

    def __init__(self, mesaj, kod):
        self.mesaj = mesaj
        self.kod = kod

print(HataKodu.GECERSIZ_GIRIS.mesaj)  # Geçersiz giriş
print(HataKodu.GECERSIZ_GIRIS.kod)    # 1001
```

## <a id="dekoratorler"></a>Dekoratörler (Decorators):

### Fonksiyon Dekoratörü

```python
# Temel dekoratör
def zaman_olcer(fonksiyon):
    def wrapper(*args, **kwargs):
        import time
        baslangic = time.perf_counter()
        sonuc = fonksiyon(*args, **kwargs)
        bitis = time.perf_counter()
        print(f"{fonksiyon.__name__}: {bitis - baslangic:.4f} saniye")
        return sonuc
    return wrapper

@zaman_olcer
def yavas_fonksiyon():
    import time
    time.sleep(0.1)
    return "Bitti"

yavas_fonksiyon()  # yavas_fonksiyon: 0.1001 saniye
```

### Parametreli Dekoratör

```python
def tekrarla(tekrar_sayisi):
    def dekorator(fonksiyon):
        def wrapper(*args, **kwargs):
            for _ in range(tekrar_sayisi):
                sonuc = fonksiyon(*args, **kwargs)
            return sonuc
        return wrapper
    return dekorator

@tekrarla(3)
def selamla(isim):
    print(f"Merhaba {isim}")

selamla("Ali")
# Merhaba Ali
# Merhaba Ali
# Merhaba Ali
```

### functools.wraps

```python
import functools

def logla(fonksiyon):
    @functools.wraps(fonksiyon)  # metadata korunur
    def wrapper(*args, **kwargs):
        print(f"{fonksiyon.__name__} çağrıldı")
        return fonksiyon(*args, **kwargs)
    return wrapper

@logla
def topla(a, b):
    """İki sayıyı toplar."""
    return a + b

print(topla.__name__)  # topla (wrapper değil)
print(topla.__doc__)   # İki sayıyı toplar.
```

### Sınıf Dekoratörü

```python
def singleton(sinif):
    """Bir sınıfın yalnızca bir örneğini oluşturur."""
    ornekler = {}
    def get_instance(*args, **kwargs):
        if sinif not in ornekler:
            ornekler[sinif] = sinif(*args, **kwargs)
        return ornekler[sinif]
    return get_instance

@singleton
class Veritabani:
    def __init__(self):
        print("Veritabanı bağlantısı oluşturuldu")

# Her seferinde aynı örnek döner
db1 = Veritabani()  # Veritabanı bağlantısı oluşturuldu
db2 = Veritabani()  # (hiçbir şey yazdırmaz)
print(db1 is db2)   # True
```

### Yerleşik Dekoratörler

```python
# @classmethod: sınıfın kendisini parametre alır
# @staticmethod: ne sınıf ne örnek alır
# @property: metodu özellik gibi kullanmayı sağlar

# @wraps: fonksiyon metadata'sını korur
from functools import wraps

def logla(fn):
    @wraps(fn)
    def wrapper(*args, **kwargs):
        print(f"{fn.__name__} çağrıldı")
        return fn(*args, **kwargs)
    return wrapper

# @lru_cache: fonksiyon sonuçlarını önbelleğe alır
from functools import lru_cache

@lru_cache(maxsize=128)
def fibonacci(n):
    if n < 2:
        return n
    return fibonacci(n - 1) + fibonacci(n - 2)

# @total_ordering: tek bir karşılaştırma metoduyla tümünü tamamlar
from functools import total_ordering

@total_ordering
class Sayi:
    def __init__(self, deger):
        self.deger = deger
    def __eq__(self, other):
        return self.deger == other.deger
    def __lt__(self, other):
        return self.deger < other.deger
    # __le__, __gt__, __ge__ otomatik türetilir

# @dataclass: veri sınıfı
from dataclasses import dataclass

# @contextmanager: bağlam yöneticisi
from contextlib import contextmanager

# Birden çok dekoratörü zincirleme
@logla
@lru_cache(maxsize=256)
def agir_islem(n):
    return sum(range(n))
```

### Dekoratör Uygulamaları

```python
# Doğrulama dekoratörü
def dogrula(tip):
    def dekorator(fn):
        def wrapper(*args, **kwargs):
            for arg in args:
                if not isinstance(arg, tip):
                    raise TypeError(f"Beklenen {tip.__name__}, alınan {type(arg).__name__}")
            return fn(*args, **kwargs)
        return wrapper
    return dekorator

@dogrula(int)
def topla(a, b):
    return a + b

print(topla(3, 5))   # 8
# print(topla("a", 5))  # TypeError

# Retry (yeniden deneme) dekoratörü
import time
import random

def tekrar_dene(max_deneme=3, bekleme=1):
    def dekorator(fn):
        def wrapper(*args, **kwargs):
            for i in range(max_deneme):
                try:
                    return fn(*args, **kwargs)
                except Exception as e:
                    if i == max_deneme - 1:
                        raise
                    print(f"Deneme {i+1} başarısız: {e}")
                    time.sleep(bekleme)
            return None
        return wrapper
    return dekorator

@tekrar_dene(max_deneme=3, bekleme=0.5)
def kararsiz_fonksiyon():
    if random.random() < 0.7:
        raise ConnectionError("Bağlantı hatası")
    return "Başarılı!"
```

## <a id="jeneratorler-ve-iteratorler"></a>Jeneratörler ve Iteratorler:

### Jeneratör Fonksiyonlar

```python
# yield ile jeneratör
def fibonacci(sinir):
    a, b = 0, 1
    while a < sinir:
        yield a
        a, b = b, a + b

# Kullanım
for sayi in fibonacci(100):
    print(sayi, end=" ")  # 0 1 1 2 3 5 8 13 21 34 55 89

# Jeneratörü listeye çevirme
fib_liste = list(fibonacci(100))
print(fib_liste)

# next() ile kullanım
fib = fibonacci(20)
print(next(fib))  # 0
print(next(fib))  # 1
print(next(fib))  # 1
print(next(fib))  # 2
```

### Iterator Protokolü

```python
# __iter__() ve __next__() metotları
class Sayac:
    def __init__(self, baslangic, bitis):
        self.baslangic = baslangic
        self.bitis = bitis
        self.mevcut = baslangic

    def __iter__(self):
        return self

    def __next__(self):
        if self.mevcut >= self.bitis:
            raise StopIteration
        deger = self.mevcut
        self.mevcut += 1
        return deger

sayac = Sayac(3, 7)
for sayi in sayac:
    print(sayi, end=" ")  # 3 4 5 6
```

### itertools Modülü

```python
import itertools

# sonsuz iteratorler
sonsuz = itertools.count(10, 2)  # 10, 12, 14, 16, ...
for i, v in enumerate(sonsuz):
    if i >= 5: break
    print(v)  # 10, 12, 14, 16, 18

tekrarla = itertools.cycle("ABC")  # A, B, C, A, B, C, ...
for i, v in enumerate(tekrarla):
    if i >= 5: break
    print(v)  # A, B, C, A, B

# Birleştirme
sonuc = list(itertools.chain([1, 2], [3, 4], [5, 6]))
print(sonuc)  # [1, 2, 3, 4, 5, 6]

# Kombinasyon ve permütasyon
harfler = ["A", "B", "C"]
print(list(itertools.permutations(harfler, 2)))
# [('A', 'B'), ('A', 'C'), ('B', 'A'), ('B', 'C'), ('C', 'A'), ('C', 'B')]

print(list(itertools.combinations(harfler, 2)))
# [('A', 'B'), ('A', 'C'), ('B', 'C')]
```

## <a id="baglam-yoneticileri"></a>Bağlam Yöneticileri (Context Managers):

Bağlam yöneticileri, `with` bloğu içinde kaynakları otomatik yönetmek için kullanılır. En yaygın örnek dosya işlemleridir.

### Sınıf Olarak Bağlam Yöneticisi

```python
class Dosya:
    def __init__(self, dosya_adi, mod):
        self.dosya_adi = dosya_adi
        self.mod = mod
        self.dosya = None

    def __enter__(self):
        print(f"{self.dosya_adi} açılıyor")
        self.dosya = open(self.dosya_adi, self.mod, encoding="utf-8")
        return self.dosya  # with ... as değişkenine atanır

    def __exit__(self, exc_type, exc_val, exc_tb):
        print(f"{self.dosya_adi} kapatılıyor")
        if self.dosya:
            self.dosya.close()
        # False dönerse hatalar yukarı iletilir
        # True dönerse hatalar bastırılır
        return False

# Kullanım
with Dosya("test.txt", "w") as f:
    f.write("Merhaba")

# Birden çok bağlam yöneticisi
with Dosya("giris.txt", "r") as giris, Dosya("cikti.txt", "w") as cikti:
    cikti.write(giris.read())
```

### contextlib ile Decorator Olarak

```python
from contextlib import contextmanager

@contextmanager
def dosya_yoneticisi(dosya_adi, mod):
    print("Dosya açılıyor")
    dosya = open(dosya_adi, mod, encoding="utf-8")
    try:
        yield dosya
    finally:
        print("Dosya kapatılıyor")
        dosya.close()

with dosya_yoneticisi("test.txt", "w") as f:
    f.write("Merhaba Dünya")

# Zaman ölçüm context manager
@contextmanager
def zaman_olc():
    import time
    baslangic = time.perf_counter()
    try:
        yield
    finally:
        bitis = time.perf_counter()
        print(f"Geçen süre: {bitis - baslangic:.4f} saniye")

with zaman_olc():
    sum(range(1000000))
```

## <a id="threading-ve-multiprocessing"></a>threading ve multiprocessing:

### threading (İş Parçacığı)

```python
import threading
import time

def isim_yaz(isim, bekleme):
    for i in range(5):
        print(f"{isim}: {i}")
        time.sleep(bekleme)

# Thread oluşturma ve başlatma
t1 = threading.Thread(target=isim_yaz, args=("Thread-1", 0.1))
t2 = threading.Thread(target=isim_yaz, args=("Thread-2", 0.15))

t1.start()
t2.start()

# Ana thread'in bitmesini bekleme
t1.join()
t2.join()

print("Tüm thread'ler tamamlandı")
```

### Lock ile Senkronizasyon

```python
import threading

paylasilan = 0
lock = threading.Lock()

def artir():
    global paylasilan
    for _ in range(100000):
        with lock:  # lock acquire/release otomatik
            paylasilan += 1

threadler = []
for _ in range(10):
    t = threading.Thread(target=artir)
    threadler.append(t)
    t.start()

for t in threadler:
    t.join()

print(f"Sonuç: {paylasilan}")  # Her zaman 1,000,000
```

### multiprocessing (Çoklu İşlem)

```python
import multiprocessing
import time

def agir_islem(sayi):
    time.sleep(1)
    return sayi ** 2

if __name__ == "__main__":
    # CPU-bound işlemler için ideal
    with multiprocessing.Pool(processes=4) as havuz:
        sayilar = [1, 2, 3, 4, 5, 6, 7, 8]
        sonuclar = havuz.map(agir_islem, sayilar)
        print(sonuclar)  # [1, 4, 9, 16, 25, 36, 49, 64]

## <a id="asenkron-programlama"></a>Asenkron Programlama (asyncio):

asyncio, Python'da eşzamanlı (concurrent) programlama için kullanılır. threading'den farkı, tek bir thread içinde olay döngüsü (event loop) ile çalışmasıdır. I/O-bound işlemler için idealdir.

### Temel Kavramlar

```python
import asyncio

# async def ile asenkron fonksiyon tanımlama
async def selamla():
    print("Merhaba")
    await asyncio.sleep(1)  # await: beklemeyi bekle
    print("1 saniye sonra")

# asenkron fonksiyonu çalıştırma
asyncio.run(selamla())

# Birden çok görevi paralel çalıştırma
async def main():
    # create_task ile görev oluştur
    gorev1 = asyncio.create_task(say("Görev-1", 3))
    gorev2 = asyncio.create_task(say("Görev-2", 2))

    print("Tüm görevler başlatıldı")
    await gorev1
    await gorev2
    print("Tüm görevler tamamlandı")

async def say(isim, sure):
    for i in range(3):
        print(f"{isim}: adım {i}")
        await asyncio.sleep(sure)

asyncio.run(main())
```

### gather() ile Paralel Çalıştırma

```python
async def fetch_url(url):
    print(f"{url} indiriliyor...")
    await asyncio.sleep(1)
    return f"{url} içeriği"

async def main():
    sonuclar = await asyncio.gather(
        fetch_url("https://site1.com"),
        fetch_url("https://site2.com"),
        fetch_url("https://site3.com"),
    )
    print(sonuclar)

asyncio.run(main())
```

### asyncio.wait() ve as_completed()

```python
import asyncio

async def islem(id, sure):
    await asyncio.sleep(sure)
    return f"İşlem {id} tamamlandı"

async def main():
    gorevler = [
        asyncio.create_task(islem(1, 3)),
        asyncio.create_task(islem(2, 1)),
        asyncio.create_task(islem(3, 2)),
    ]

    # Hepsinin bitmesini bekle
    tamamlanan, bekleyen = await asyncio.wait(gorevler)
    for g in tamamlanan:
        print(g.result())

    # as_completed: bitenleri sırayla işle
    for gorev in asyncio.as_completed(gorevler):
        sonuc = await gorev
        print(sonuc)

asyncio.run(main())
```

### Async Context Manager ve Async Iterator

```python
import asyncio

# Async context manager
class AsyncDosya:
    async def __aenter__(self):
        print("Dosya açılıyor (async)")
        await asyncio.sleep(0.1)
        return self

    async def __aexit__(self, *args):
        print("Dosya kapatılıyor (async)")
        await asyncio.sleep(0.1)

    async def oku(self):
        return "Veri"

async def main():
    async with AsyncDosya() as dosya:
        veri = await dosya.oku()
        print(veri)

# Async iterator
class AsyncSayac:
    def __init__(self, bitis):
        self.bitis = bitis
        self.mevcut = 0

    def __aiter__(self):
        return self

    async def __anext__(self):
        if self.mevcut >= self.bitis:
            raise StopAsyncIteration
        await asyncio.sleep(0.1)
        self.mevcut += 1
        return self.mevcut

async def main2():
    async for sayi in AsyncSayac(5):
        print(sayi, end=" ")  # 1 2 3 4 5

asyncio.run(main2())
```

### Korumalı Görevler (asyncio shields)

```python
import asyncio

async def kritik_islem():
    """İptal edilemeyen işlem."""
    try:
        await asyncio.sleep(5)
        return "Kritik işlem tamam"
    except asyncio.CancelledError:
        print("İptal yakalandı, işlem devam ediyor...")
        await asyncio.sleep(2)
        return "Kritik işlem tamam (iptale rağmen)"

async def main():
    gorev = asyncio.create_task(asyncio.shield(kritik_islem()))
    await asyncio.sleep(0.5)
    gorev.cancel()  # shield koruması sayesinde iptal edilmez
    try:
        sonuc = await gorev
        print(sonuc)
    except asyncio.CancelledError:
        print("Görev iptal edildi")
```

### Timeout ve Bekleme

```python
import asyncio

async def yavas_islem():
    await asyncio.sleep(10)
    return "Tamam"

async def main():
    try:
        # Maksimum 2 saniye bekle
        sonuc = await asyncio.wait_for(yavas_islem(), timeout=2)
    except asyncio.TimeoutError:
        print("İşlem zaman aşımına uğradı!")

    # wait_for yerine daha modern yaklaşım (Python 3.11+)
    try:
        async with asyncio.timeout(2):
            sonuc = await yavas_islem()
    except TimeoutError:
        print("Zaman aşımı!")

asyncio.run(main())
```

### aiohttp ile Asenkron HTTP

```python
# pip install aiohttp
import asyncio
import aiohttp

async def url_getir(session, url):
    async with session.get(url) as response:
        return await response.text()

async def main():
    urls = [
        "https://example.com",
        "https://httpbin.org/get",
    ]
    async with aiohttp.ClientSession() as session:
        gorevler = [url_getir(session, url) for url in urls]
        sonuclar = await asyncio.gather(*gorevler)
        for sonuc in sonuclar:
            print(f"Uzunluk: {len(sonuc)}")

asyncio.run(main())
```

### asyncio.Semaphore ile Sınırlama

```python
import asyncio

async def islem_yap(sem, id):
    async with sem:
        print(f"İşlem {id} başladı")
        await asyncio.sleep(1)
        print(f"İşlem {id} bitti")

async def main():
    sem = asyncio.Semaphore(3)  # aynı anda en fazla 3 işlem
    gorevler = [islem_yap(sem, i) for i in range(10)]
    await asyncio.gather(*gorevler)

asyncio.run(main())
```

### Queue ile İşçi Havuzu

```python
import asyncio
import random

async def isci(kuyruk, isci_id):
    while True:
        is_parcasi = await kuyruk.get()
        if is_parcasi is None:
            break
        print(f"İşçi {isci_id}: {is_parcasi} işleniyor")
        await asyncio.sleep(random.random())
        kuyruk.task_done()

async def main():
    kuyruk = asyncio.Queue()
    # 3 işçi oluştur
    isciler = [asyncio.create_task(isci(kuyruk, i)) for i in range(3)]
    # İşleri kuyruğa ekle
    for i in range(10):
        await kuyruk.put(f"İş-{i}")
    # İşlerin bitmesini bekle
    await kuyruk.join()
    # İşçileri durdur
    for _ in isciler:
        await kuyruk.put(None)
    await asyncio.gather(*isciler)

asyncio.run(main())
```

### Hata Yönetimi

```python
import asyncio

async def hatali_islem():
    raise ValueError("Hata oluştu!")

async def main():
    # gather() ile hata yönetimi
    sonuclar = await asyncio.gather(
        asyncio.sleep(1),
        hatali_islem(),
        return_exceptions=True,  # hataları exception olarak döndür
    )
    for sonuc in sonuclar:
        if isinstance(sonuc, Exception):
            print(f"Hata: {sonuc}")
        else:
            print(f"Başarılı: {sonuc}")

    # TaskGroup ile (Python 3.11+)
    try:
        async with asyncio.TaskGroup() as tg:
            tg.create_task(asyncio.sleep(1))
            tg.create_task(hatali_islem())
    except* ValueError as e:
        print(f"Grup hatası: {e}")
```

### asyncio Karar Ağacı

```
                    I/O-bound mı?
                   /              \
                 EVET              HAYIR
                /                     \
        Bekleme var mı?            CPU-bound
       /             \                 |
     EVET           HAYIR          multiprocessing
      |                |
   asyncio        threading
```

## <a id="veritabani-islemleri"></a>Veritabanı İşlemleri (SQLite):

### SQLite Temel İşlemler

```python
import sqlite3

# Bağlantı oluştur (dosya yoksa oluşturulur)
baglanti = sqlite3.connect("okul.db")
imlec = baglanti.cursor()

# Tablo oluşturma
imlec.execute("""
    CREATE TABLE IF NOT EXISTS ogrenciler (
        id INTEGER PRIMARY KEY AUTOINCREMENT,
        ad TEXT NOT NULL,
        soyad TEXT NOT NULL,
        yas INTEGER,
        bolum TEXT
    )
""")

# Veri ekleme
imlec.execute("""
    INSERT INTO ogrenciler (ad, soyad, yas, bolum)
    VALUES (?, ?, ?, ?)
""", ("Ali", "Yılmaz", 20, "Bilgisayar"))

# Çoklu ekleme
ogrenciler = [
    ("Ayşe", "Demir", 22, "Elektrik"),
    ("Mehmet", "Kaya", 21, "Makine"),
]
imlec.executemany("""
    INSERT INTO ogrenciler (ad, soyad, yas, bolum)
    VALUES (?, ?, ?, ?)
""", ogrenciler)

baglanti.commit()  # değişiklikleri kaydet
```

### Sorgulama

```python
# Tüm kayıtlar
imlec.execute("SELECT * FROM ogrenciler")
kayitlar = imlec.fetchall()
for kayit in kayitlar:
    print(kayit)  # (1, 'Ali', 'Yılmaz', 20, 'Bilgisayar')

# Tek kayıt
imlec.execute("SELECT * FROM ogrenciler WHERE id = ?", (1,))
kayit = imlec.fetchone()
print(kayit)

# Row Factory (sözlük benzeri erişim)
baglanti.row_factory = sqlite3.Row
imlec = baglanti.cursor()
imlec.execute("SELECT * FROM ogrenciler")
for row in imlec.fetchall():
    print(row["ad"], row["soyad"])
```

## <a id="web-istekleri"></a>Web İstekleri (requests):

### Temel Kullanım

```python
import requests

# GET isteği
response = requests.get("https://api.github.com")
print(response.status_code)      # 200
print(response.ok)               # True (200-399 arası)
print(response.headers["content-type"])
print(response.json())           # JSON yanıtı otomatik parse
print(response.text)             # ham metin (string)
print(response.content)          # ham binary (bytes)
print(response.elapsed)          # istek süresi
print(response.url)              # son URL (yönlendirme sonrası)

# Parametrelerle GET
params = {"q": "python", "page": 1}
response = requests.get("https://api.github.com/search/repositories", params=params)
print(response.url)  # https://api.github.com/...?q=python&page=1

# POST isteği
veri = {"username": "ali", "password": "1234"}
response = requests.post("https://httpbin.org/post", data=veri)
print(response.json())

# JSON POST
response = requests.post(
    "https://httpbin.org/post",
    json={"key": "value"}  # otomatik json.dumps()
)

# Başlıklar ve kimlik doğrulama
headers = {"Authorization": "Bearer token123", "User-Agent": "MyApp/1.0"}
response = requests.get(
    "https://api.github.com/user",
    headers=headers,
    timeout=10,  # saniye cinsinden timeout
)

# Hata yönetimi
try:
    response = requests.get("https://api.github.com", timeout=5)
    response.raise_for_status()  # 4xx/5xx hatalarında HTTPError fırlatır
except requests.exceptions.HTTPError as e:
    print(f"HTTP Hatası: {e}")
except requests.exceptions.ConnectionError as e:
    print(f"Bağlantı hatası: {e}")
except requests.exceptions.Timeout as e:
    print(f"Zaman aşımı: {e}")
except requests.exceptions.RequestException as e:
    print(f"İstek hatası: {e}")

# Dosya indirme (büyük dosyalar için akış)
response = requests.get("https://example.com/buyuk_dosya.zip", stream=True)
with open("dosya.zip", "wb") as f:
    for chunk in response.iter_content(chunk_size=8192):
        if chunk:
            f.write(chunk)

# Oturum yönetimi
with requests.Session() as session:
    session.auth = ("user", "pass")
    session.headers.update({"User-Agent": "MyApp/1.0"})
    # cookieler otomatik yönetilir
    response1 = session.get("https://api.github.com/user")
    response2 = session.get("https://api.github.com/user/repos")
```

### İleri Seviye requests Kullanımı

```python
import requests

# Dosya yükleme
files = {"file": open("resim.jpg", "rb")}
response = requests.post("https://httpbin.org/post", files=files)

# Çoklu dosya yükleme
files = [
    ("images", ("foto1.jpg", open("foto1.jpg", "rb"), "image/jpeg")),
    ("images", ("foto2.jpg", open("foto2.jpg", "rb"), "image/jpeg")),
]
response = requests.post("https://httpbin.org/post", files=files)

# Cookie kullanımı
cookies = {"session_id": "abc123"}
response = requests.get("https://httpbin.org/cookies", cookies=cookies)

# Proxy desteği
proxies = {
    "http": "http://proxy.example.com:8080",
    "https": "https://proxy.example.com:8080",
}
response = requests.get("https://api.github.com", proxies=proxies)

# SSL sertifika doğrulamasını kapatma (güvenli değil!)
response = requests.get("https://self-signed.badssl.com", verify=False)

# Özel sertifika
response = requests.get("https://example.com", cert=("/path/to/cert.pem", "/path/to/key.pem"))

# Yönlendirmeleri engelleme
response = requests.get("https://httpbin.org/redirect/3", allow_redirects=False)
print(response.status_code)  # 302

# Özel yönlendirme takibi
session = requests.Session()
session.max_redirects = 5

# Retry (yeniden deneme) ile
from requests.adapters import HTTPAdapter
from urllib3.util.retry import Retry

session = requests.Session()
retry = Retry(
    total=3,
    read=3,
    connect=3,
    backoff_factor=0.5,  # bekleme süresi katsayısı
    status_forcelist=[500, 502, 503, 504],
)
adapter = HTTPAdapter(max_retries=retry)
session.mount("http://", adapter)
session.mount("https://", adapter)

response = session.get("https://api.github.com")
```

## <a id="unittest-ile-test-yazma"></a>unittest ile Test Yazma:

```python
import unittest

# Test edilecek fonksiyon
def topla(a, b):
    return a + b

def bol(a, b):
    if b == 0:
        raise ValueError("Sıfıra bölme hatası")
    return a / b

class MatematikTesti(unittest.TestCase):
    def test_topla(self):
        self.assertEqual(topla(3, 5), 8)
        self.assertEqual(topla(-1, 1), 0)
        self.assertEqual(topla(0, 0), 0)

    def test_topla_tip(self):
        with self.assertRaises(TypeError):
            topla("a", 5)

    def test_bol(self):
        self.assertEqual(bol(10, 2), 5)
        self.assertEqual(bol(7, 3), 7/3)
        self.assertAlmostEqual(bol(1, 3), 0.3333, places=4)

    def test_bol_sifir(self):
        with self.assertRaises(ValueError):
            bol(10, 0)

# Testleri çalıştırma
if __name__ == "__main__":
    unittest.main()
```

### pytest ile Test

```python
# pip install pytest

def topla(a, b):
    return a + b

def test_topla():
    assert topla(3, 5) == 8
    assert topla(-1, 1) == 0
```

```bash
# pytest çalıştırma
pytest
pytest -v
```

## <a id="sanal-ortamlar-ve-pip"></a>Sanal Ortamlar ve pip:

### Sanal Ortam Oluşturma

```bash
# venv modülü ile (Python 3.3+)
python3 -m venv venv

# Sanal ortamı aktifleştirme
# Linux/macOS:
source venv/bin/activate

# Windows:
venv\Scripts\activate

# Sanal ortamı devre dışı bırakma
deactivate
```

### pip Kullanımı

```bash
# Paket kurulumu
pip install requests
pip install flask==2.3.0
pip install "numpy>=1.24,<2.0"

# Paket kaldırma
pip uninstall requests

# Kurulu paketleri listeleme
pip list

# Paket bilgisi
pip show requests

# Bağımlılıkları dosyaya kaydetme
pip freeze > requirements.txt

# requirements.txt'den kurulum
pip install -r requirements.txt

# Güncelleme
pip install --upgrade pip
pip install --upgrade requests
```

### Poetry ile Modern Bağımlılık Yönetimi

```bash
# Poetry kurulumu
pip install poetry

# Yeni proje oluşturma
poetry new my_project

# Mevcut projede başlatma
poetry init

# Bağımlılık ekleme
poetry add requests numpy
poetry add --dev pytest black mypy

# Sanal ortam oluşturma ve bağımlılıkları yükleme
poetry install

# Sanal ortamda komut çalıştırma
poetry run python script.py
poetry run pytest

# pyproject.toml'a bağımlılıkları kilitleme
poetry lock

# requirements.txt dışa aktarma
poetry export -f requirements.txt --output requirements.txt
```

### pipenv Alternatifi

```bash
# pipenv kurulum
pip install pipenv

# Proje başlatma
pipenv install requests

# Geliştirme bağımlılığı
pipenv install --dev pytest

# Sanal ortamı aktifleştirme
pipenv shell

# Pipfile.lock güncelleme
pipenv lock
```

### uv ile Ultra Hızlı Paket Yönetimi

```bash
# pip install uv
# veya curl -LsSf https://astral.sh/uv/install.sh | sh

uv pip install requests  # pip'ten 10-100x hızlı
uv pip install -r requirements.txt
uv venv
uv pip list
```

## <a id="basit-python-ornegi"></a>Basit Python Örneği - Öğrenci Yönetim Sistemi:

```python
import json
import os
from datetime import datetime


class Ogrenci:
    """Öğrenci sınıfı."""

    def __init__(self, ogrenci_id, ad, soyad, bolum, dogum_yili):
        self.ogrenci_id = ogrenci_id
        self.ad = ad
        self.soyad = soyad
        self.bolum = bolum
        self.dogum_yili = dogum_yili
        self.ders_notlari = {}

    @property
    def yas(self):
        return datetime.now().year - self.dogum_yili

    @property
    def tam_ad(self):
        return f"{self.ad} {self.soyad}"

    def not_ekle(self, ders, not_degeri):
        if ders not in self.ders_notlari:
            self.ders_notlari[ders] = []
        self.ders_notlari[ders].append(not_degeri)

    def not_ortalamasi(self, ders=None):
        if ders:
            notlar = self.ders_notlari.get(ders, [])
            return sum(notlar) / len(notlar) if notlar else 0
        tum_notlar = [n for notlar in self.ders_notlari.values() for n in notlar]
        return sum(tum_notlar) / len(tum_notlar) if tum_notlar else 0

    def to_dict(self):
        return {
            "ogrenci_id": self.ogrenci_id,
            "ad": self.ad,
            "soyad": self.soyad,
            "bolum": self.bolum,
            "dogum_yili": self.dogum_yili,
            "ders_notlari": self.ders_notlari,
        }

    @classmethod
    def from_dict(cls, veri):
        ogr = cls(
            veri["ogrenci_id"],
            veri["ad"],
            veri["soyad"],
            veri["bolum"],
            veri["dogum_yili"],
        )
        ogr.ders_notlari = veri.get("ders_notlari", {})
        return ogr

    def __str__(self):
        return f"{self.ogrenci_id}: {self.tam_ad} ({self.bolum}, {self.yas} yaş)"


class OgrenciYonetimSistemi:
    """Öğrenci yönetim sistemi."""

    def __init__(self, dosya_adi="ogrenciler.json"):
        self.dosya_adi = dosya_adi
        self.ogrenciler = {}
        self.yukle()

    def yukle(self):
        if os.path.exists(self.dosya_adi):
            try:
                with open(self.dosya_adi, "r", encoding="utf-8") as f:
                    veri = json.load(f)
                for ogr_veri in veri:
                    ogr = Ogrenci.from_dict(ogr_veri)
                    self.ogrenciler[ogr.ogrenci_id] = ogr
            except (json.JSONDecodeError, KeyError) as e:
                print(f"Dosya okuma hatası: {e}")

    def kaydet(self):
        veri = [ogr.to_dict() for ogr in self.ogrenciler.values()]
        with open(self.dosya_adi, "w", encoding="utf-8") as f:
            json.dump(veri, f, indent=2, ensure_ascii=False)

    def ogrenci_ekle(self, ogrenci_id, ad, soyad, bolum, dogum_yili):
        if ogrenci_id in self.ogrenciler:
            return False
        ogr = Ogrenci(ogrenci_id, ad, soyad, bolum, dogum_yili)
        self.ogrenciler[ogrenci_id] = ogr
        self.kaydet()
        return True

    def ogrenci_sil(self, ogrenci_id):
        if ogrenci_id in self.ogrenciler:
            self.ogrenciler.pop(ogrenci_id)
            self.kaydet()
            return True
        return False

    def not_ekle(self, ogrenci_id, ders, not_degeri):
        if ogrenci_id not in self.ogrenciler:
            return False
        if not 0 <= not_degeri <= 100:
            return False
        ogr = self.ogrenciler[ogrenci_id]
        ogr.not_ekle(ders, not_degeri)
        self.kaydet()
        return True

    def menu(self):
        while True:
            print("\n" + "=" * 50)
            print("ÖĞRENCİ YÖNETİM SİSTEMİ")
            print("=" * 50)
            print("1. Öğrenci Ekle")
            print("2. Öğrenci Sil")
            print("3. Not Ekle")
            print("4. Tüm Öğrencileri Listele")
            print("5. Çıkış")
            print("-" * 50)

            secim = input("Seçiminiz (1-5): ").strip()

            if secim == "1":
                ogrenci_id = input("Öğrenci ID: ").strip()
                ad = input("Ad: ").strip()
                soyad = input("Soyad: ").strip()
                bolum = input("Bölüm: ").strip()
                try:
                    dogum_yili = int(input("Doğum Yılı: ").strip())
                    if self.ogrenci_ekle(ogrenci_id, ad, soyad, bolum, dogum_yili):
                        print("Öğrenci eklendi.")
                    else:
                        print("Bu ID zaten mevcut!")
                except ValueError:
                    print("Geçerli bir yıl girin!")

            elif secim == "2":
                ogrenci_id = input("Silinecek öğrenci ID: ").strip()
                if self.ogrenci_sil(ogrenci_id):
                    print("Öğrenci silindi.")
                else:
                    print("Öğrenci bulunamadı!")

            elif secim == "3":
                ogrenci_id = input("Öğrenci ID: ").strip()
                ders = input("Ders adı: ").strip()
                try:
                    not_degeri = int(input("Not (0-100): ").strip())
                    if self.not_ekle(ogrenci_id, ders, not_degeri):
                        print("Not eklendi.")
                    else:
                        print("Öğrenci bulunamadı veya geçersiz not!")
                except ValueError:
                    print("Geçerli bir not girin!")

            elif secim == "4":
                if not self.ogrenciler:
                    print("Henüz öğrenci kaydı yok.")
                else:
                    print(f"\n{'ID':<5} {'Ad Soyad':<25} {'Bölüm':<20} {'Yaş':<5}")
                    print("-" * 55)
                    for ogr in sorted(self.ogrenciler.values(), key=lambda x: x.ogrenci_id):
                        print(f"{ogr.ogrenci_id:<5} {ogr.tam_ad:<25} {ogr.bolum:<20} {ogr.yas:<5}")

            elif secim == "5":
                print("Çıkış yapılıyor...")
                break

            else:
                print("Geçersiz seçim!")


if __name__ == "__main__":
    sistem = OgrenciYonetimSistemi()
    sistem.menu()
```

## <a id="pythonic-kod"></a>Pythonic Kod ve PEP 8:

### PEP 8 - Python Kod Stili Rehberi

PEP 8, Python kod stilini belirleyen resmi rehberdir.

```python
# İsimlendirme kuralları
# sinif_adi:    PascalCase (Ogrenci, HttpRequest)
# fonksiyon:    snake_case (ogrenci_ekle, http_get)
# degisken:     snake_case (ogrenci_adi, http_status)
# sabit:        UPPER_CASE (PI, MAX_RETRY_COUNT)
# gizli:        _ile_baslar (_gizli_degisken)
# özel:         __ile_basar (__private_method)
# çakışma:      trailing_ (class_)

# Satır uzunluğu: maksimum 79 karakter
# Girinti: 4 boşluk (tab değil!)

# Doğru:
x = 5
if x > 0:
    print("Pozitif")

# Boolean karşılaştırma
# Kötü:
if aktif == True:
if len(liste) > 0:
if deger is not None:

# İyi:
if aktif:
if liste:
if deger:

# None kontrolü
if x is None:     # doğru
if x is not None: # doğru
if x == None:     # yanlış

# Zincirleme karşılaştırma
if 0 < x < 10:    # Pythonic
if x > 0 and x < 10:  # değil
```

### Pythonic Kod Desenleri

```python
# Değişken değiştirme (tuple unpacking)
a, b = b, a

# Birden çok dönüş değeri
def islemler(a, b):
    return a + b, a * b

top, carp = islemler(3, 5)

# Koşullu atama (ternary)
durum = "reşit" if yas >= 18 else "reşit değil"

# Liste/sözlük comprehension
kareler = [x**2 for x in range(10) if x % 2 == 0]

# with ile otomatik kaynak yönetimi
with open("dosya.txt") as f:
    icerik = f.read()

# enumerate ile indeks
for i, deger in enumerate(liste):
    print(i, deger)

# zip ile paralel döngü
for isim, yas in zip(isimler, yaslar):
    print(isim, yas)

# _ kullanımı (önemsiz değişken)
for _ in range(10):
    print("Merhaba")

# Context manager ile dosya
with open("log.txt", "w") as f:
    print("Hata!", file=f)

# any() ve all()
if any(x > 10 for x in sayilar):
    print("En az biri 10'dan büyük")
```

### Yaygın Anti-pattern'ler

```python
# 1. Mutable varsayılan argüman
# Kötü:
def ekle(x, liste=[]):
    liste.append(x)
    return liste

# İyi:
def ekle(x, liste=None):
    if liste is None:
        liste = []
    liste.append(x)
    return liste

# 2. Bare except
# Kötü:
try:
    islem()
except:
    pass

# İyi:
try:
    islem()
except ValueError as e:
    print(e)

# 3. import * (namespace pollution)
# Kötü:
from math import *

# İyi:
from math import sqrt, pi

# 4. type() ile kontrol
# Kötü:
if type(x) == int:

# İyi:
if isinstance(x, int):

# 5. String birleştirme (çok sayıda)
# Kötü:
s = ""
for x in liste:
    s += str(x)

# İyi:
s = "".join(str(x) for x in liste)

# 6. range(len()) kullanımı
# Kötü:
for i in range(len(liste)):
    print(liste[i])

# İyi:
for eleman in liste:
    print(eleman)
# veya
for i, eleman in enumerate(liste):
    print(i, eleman)
```

### The Zen of Python

```python
import this
# Çıktı:
# Beautiful is better than ugly.
# Explicit is better than implicit.
# Simple is better than complex.
# Complex is better than complicated.
# Flat is better than nested.
# Sparse is better than dense.
# Readability counts.
# ...
```

## <a id="performans-ipuclari"></a>Performans İpuçları:

### Profiling (Profil Çıkarma)

```python
# cProfile ile performans analizi
import cProfile
import pstats

def yavas_fonksiyon():
    toplam = 0
    for i in range(1000000):
        toplam += i ** 2
    return toplam

# Profil çıkarma
cProfile.run("yavas_fonksiyon()", "profil.cprof")

# Sonuçları inceleme
p = pstats.Stats("profil.cprof")
p.sort_stats("time").print_stats(10)  # en yavaş 10 fonksiyon

# Komut satırından:
# python -m cProfile script.py
```

### timeit ile Hassas Ölçüm

```python
import timeit

# Kod parçasının çalışma süresi
sure = timeit.timeit(
    '"-".join(str(n) for n in range(100))',
    number=10000,
)
print(f"10.000 tekrar: {sure:.4f}s")

# Alternatif yaklaşımları karşılaştırma
s1 = timeit.timeit('",".join(map(str, range(100)))', number=10000)
s2 = timeit.timeit('",".join([str(n) for n in range(100)])', number=10000)
print(f"map: {s1:.4f}s, comprehension: {s2:.4f}s")
```

### Optimizasyon İpuçları

```python
# 1. Yerel değişkenleri kullan (global erişim yavaş)
# Yavaş:
def toplam_global(n):
    for i in range(n):
        sonuc += i

# Hızlı:
def toplam_yerel(n):
    topla = 0
    for i in range(n):
        topla += i
    return topla

# 2. join() string birleştirme için
isimler = ["Ali", "Veli", "Ayşe"]
sonuc = ", ".join(isimler)  # "+" operatöründen hızlı

# 3. Yerel bağlamada fonksiyon referansları
def hızlı_dongu(n):
    topla = 0
    r = range  # range'i yerel değişkene ata
    for i in r(n):
        topla += i
    return topla

# 4. List comprehension > map/filter (çoğu durumda)
kareler = [x**2 for x in range(1000)]  # list(map(...))'den hızlı

# 5. set/dict üyelik kontrolü O(1)
isimler_set = {"Ali", "Veli", "Ayşe"}
if "Ali" in isimler_set:  # listeden çok daha hızlı

# 6. Generator expression bellek için
buyuk_veri = (x**2 for x in range(10**7))  # [] yerine ()

# 7. locals() ile hız kazanma
def hızlı_fonksiyon():
    topla = 0
    yerel = locals()
    for i in range(1000000):
        topla += i
    return topla

# 8. __slots__ ile bellek tasarrufu
class Nokta:
    __slots__ = ("x", "y")  # __dict__ kullanma
    def __init__(self, x, y):
        self.x = x
        self.y = y
# __slots__: daha az bellek, daha hızlı erişim
```

### PyPy Kullanımı

```python
# PyPy: Python kodunu JIT derleyerek çalıştırır
# Çoğu Python kodu için 4-5x hızlanma sağlar
# pip install pypy
# pypy script.py

# Cython: Python kodunu C'ye derler
# pip install cython
# Pythran: Python kodunu derler
```

## <a id="proje-yapisi"></a>Proje Yapısı ve Organizasyon:

### İdeal Proje Dizini Yapısı

```
my_project/
├── README.md
├── LICENSE
├── setup.py veya pyproject.toml
├── requirements.txt
├── .gitignore
├── src/
│   └── my_project/
│       ├── __init__.py
│       ├── main.py
│       ├── config.py
│       ├── models.py
│       ├── utils/
│       │   ├── __init__.py
│       │   ├── helpers.py
│       │   └── validators.py
│       └── services/
│           ├── __init__.py
│           └── user_service.py
├── tests/
│   ├── __init__.py
│   ├── test_main.py
│   ├── conftest.py
│   └── test_utils/
│       ├── __init__.py
│       └── test_helpers.py
└── docs/
    └── api.md
```

### pyproject.toml (Modern Proje Yapılandırması)

```toml
[build-system]
requires = ["setuptools>=61.0", "wheel"]
build-backend = "setuptools.build_meta"

[project]
name = "myproject"
version = "0.1.0"
description = "Örnek proje"
readme = "README.md"
requires-python = ">=3.10"
license = {text = "MIT"}
authors = [
    {name = "Ali Yılmaz", email = "ali@example.com"},
]
dependencies = [
    "requests>=2.28",
    "click>=8.0",
]

[project.optional-dependencies]
dev = [
    "pytest>=7.0",
    "black>=22.0",
    "ruff>=0.1",
    "mypy>=1.0",
]

[tool.black]
line-length = 88
target-version = ["py310"]

[tool.ruff]
line-length = 88
select = ["E", "F", "I", "W"]

[tool.pytest.ini_options]
testpaths = ["tests"]
```

### Kod Kalitesi Araçları

```bash
# Black: otomatik kod formatlama
pip install black
black src/ tests/

# Ruff: hızlı lint (ESLint benzeri)
pip install ruff
ruff check src/

# mypy: statik tip kontrolü
pip install mypy
mypy src/

# isort: import sıralama
pip install isort
isort src/ tests/

# pre-commit: commit öncesi kontroller
pip install pre-commit
# .pre-commit-config.yaml dosyası oluştur
# pre-commit install
```

### pre-commit Yapılandırması

```yaml
# .pre-commit-config.yaml
repos:
  - repo: https://github.com/psf/black
    rev: 23.12.0
    hooks:
      - id: black
  - repo: https://github.com/astral-sh/ruff-pre-commit
    rev: v0.1.0
    hooks:
      - id: ruff
  - repo: https://github.com/pre-commit/mirrors-mypy
    rev: v1.8.0
    hooks:
      - id: mypy
```

```bash
# pre-commit'i kur ve çalıştır
pip install pre-commit
pre-commit install
pre-commit run --all-files
```

## <a id="web-scraping"></a>Web Scraping:

### BeautifulSoup ile HTML Ayrıştırma

```python
# pip install beautifulsoup4 lxml

from bs4 import BeautifulSoup
import requests

html = """
<html>
    <body>
        <h1 id="baslik">Python Öğren</h1>
        <div class="icerik">
            <p>Python harika bir dildir.</p>
            <a href="https://python.org">Python Sitesi</a>
        </div>
        <ul class="liste">
            <li>Kolay</li>
            <li>Okunabilir</li>
            <li>Güçlü</li>
        </ul>
    </body>
</html>
"""

soup = BeautifulSoup(html, "lxml")

# Temel arama
print(soup.title)           # <title>...</title>
print(soup.h1.text)         # "Python Öğren"
print(soup.find("h1").text) # "Python Öğren"
print(soup.find(id="baslik").text)
print(soup.find(class_="icerik").p.text)

# CSS seçiciler
print(soup.select_one("#baslik").text)         # id ile
print(soup.select_one(".icerik").text)         # class ile
print(soup.select("ul.liste li"))              # tüm li'ler

# Link ve attribute bulma
for link in soup.find_all("a"):
    print(link.get("href"))    # https://python.org
    print(link.text)           # Python Sitesi

# Metin bulma
paragraflar = soup.find_all("p")
for p in paragraflar:
    print(p.text.strip())
```

### Gerçek Site Scraping

```python
import requests
from bs4 import BeautifulSoup
import time

def haber_basliklari_getir(url):
    headers = {
        "User-Agent": "Mozilla/5.0 (Windows NT 10.0; Win64; x64) AppleWebKit/537.36"
    }
    try:
        response = requests.get(url, headers=headers, timeout=10)
        response.raise_for_status()
        soup = BeautifulSoup(response.text, "lxml")
        basliklar = []
        for h in soup.find_all(["h1", "h2", "h3"]):
            metin = h.text.strip()
            if metin and len(metin) > 10:
                basliklar.append(metin)
        return basliklar[:10]  # ilk 10 başlık
    except requests.RequestException as e:
        print(f"Hata: {e}")
        return []

# Kullanım
basliklar = haber_basliklari_getir("https://example.com")
for baslik in basliklar:
    print(baslik)
    time.sleep(0.5)  # siteye saygılı ol
```

### Selenium ile Dinamik Sayfalar

```python
# pip install selenium
# webdriver indirmek: https://chromedriver.chromium.org/

from selenium import webdriver
from selenium.webdriver.common.by import By
from selenium.webdriver.support.ui import WebDriverWait
from selenium.webdriver.support import expected_conditions as EC
from selenium.webdriver.chrome.options import Options

options = Options()
options.add_argument("--headless")  # görünmez mod

driver = webdriver.Chrome(options=options)
driver.get("https://example.com")

try:
    # Dinamik yüklenen içeriği bekle
    element = WebDriverWait(driver, 10).until(
        EC.presence_of_element_located((By.ID, "icerik"))
    )
    print(element.text)

    # JavaScript çalıştır
    driver.execute_script("window.scrollTo(0, document.body.scrollHeight);")

    # Ekran görüntüsü al
    driver.save_screenshot("sayfa.png")
finally:
    driver.quit()
```

### Scrapy ile Profesyonel Scraping

```python
# pip install scrapy
# scrapy startproject projeadi

import scrapy

class HaberSpider(scrapy.Spider):
    name = "haberler"
    start_urls = ["https://example.com"]

    def parse(self, response):
        for haber in response.css("div.haber"):
            yield {
                "baslik": haber.css("h2::text").get(),
                "link": haber.css("a::attr(href)").get(),
                "tarih": haber.css("span.tarih::text").get(),
            }

        # Sonraki sayfa
        sonraki = response.css("a.sonraki::attr(href)").get()
        if sonraki:
            yield response.follow(sonraki, self.parse)

# Çalıştırma: scrapy crawl haberler -o cikti.json
```

### Scraping Etiği ve Yasal Uyarılar

- `robots.txt` dosyasını kontrol edin
- Rate limiting uygulayın (sayfalar arasında bekleme)
- User-Agent belirtin
- Site kullanım koşullarını okuyun
- Kişisel veri toplamaktan kaçının
- Ticari kullanım için izin alın

## <a id="flask"></a>Flask Web Framework:

Flask, Python'un en popüler mikro web frameworklerinden biridir. Hafif, esnek ve başlangıç dostudur.

### Kurulum ve İlk Uygulama

```bash
pip install flask
```

```python
# app.py
from flask import Flask

app = Flask(__name__)

@app.route("/")
def index():
    return "Merhaba Dünya!"

if __name__ == "__main__":
    app.run(debug=True)
```

```bash
python app.py
# http://127.0.0.1:5000 adresinde çalışır
```

### Temel Routing

```python
from flask import Flask

app = Flask(__name__)

# Statik rota
@app.route("/")
def index():
    return "<h1>Ana Sayfa</h1>"

# Dinamik rota (parametreli)
@app.route("/kullanici/<isim>")
def kullanici_profil(isim):
    return f"<h1>Merhaba {isim}</h1>"

# Tip belirtme
@app.route("/yazi/<int:yazi_id>")
def yazi_goster(yazi_id):
    return f"Yazı ID: {yazi_id}"

@app.route("/topla/<float:sayi1>/<float:sayi2>")
def sayilari_topla(sayi1, sayi2):
    return f"Toplam: {sayi1 + sayi2}"

# HTTP metodları
@app.route("/giris", methods=["GET", "POST"])
def giris():
    if gecerli_istek.method == "POST":
        return "Form gönderildi"
    return """
        <form method="post">
            <input type="text" name="kullanici">
            <input type="submit">
        </form>
    """

# URL oluşturma
from flask import url_for

with app.test_request_context():
    print(url_for("kullanici_profil", isim="Ali"))   # /kullanici/Ali
    print(url_for("yazi_goster", yazi_id=5))          # /yazi/5
    print(url_for("static", filename="style.css"))    # /static/style.css
```

### Jinja2 Template Kullanımı

```python
from flask import Flask, render_template

app = Flask(__name__)

@app.route("/")
def index():
    kullanici = {"ad": "Ali", "yas": 25}
    yetenekler = ["Python", "Flask", "JavaScript"]
    return render_template("index.html", kullanici=kullanici, yetenekler=yetenekler)
```

```html
<!-- templates/index.html -->
<!DOCTYPE html>
<html>
<head>
    <title>Flask Uygulaması</title>
    <link rel="stylesheet" href="{{ url_for('static', filename='style.css') }}">
</head>
<body>
    <h1>Merhaba {{ kullanici.ad }}</h1>
    <p>Yaş: {{ kullanici.yas }}</p>

    <h2>Yetenekler</h2>
    <ul>
        {% for yetenek in yetenekler %}
        <li>{{ yetenek }}</li>
        {% endfor %}
    </ul>

    {% if kullanici.yas >= 18 %}
        <p>Yetişkin</p>
    {% else %}
        <p>Çocuk</p>
    {% endif %}
</body>
</html>
```

```html
<!-- templates/base.html (layout) -->
<!DOCTYPE html>
<html>
<head>
    <title>{% block title %}Flask App{% endblock %}</title>
    <link rel="stylesheet" href="{{ url_for('static', filename='style.css') }}">
</head>
<body>
    <nav>
        <a href="{{ url_for('index') }}">Ana Sayfa</a>
        <a href="{{ url_for('hakkimizda') }}">Hakkımızda</a>
    </nav>

    <main>
        {% block content %}{% endblock %}
    </main>

    <footer>
        <p>&copy; 2026 Flask App</p>
    </footer>
</body>
</html>
```

```html
<!-- templates/hakkimizda.html -->
{% extends "base.html" %}

{% block title %}Hakkımızda{% endblock %}

{% block content %}
    <h1>Hakkımızda</h1>
    <p>Flask öğreniyorum.</p>
{% endblock %}
```

### Jinja2 Filter ve Macro

```html
<!-- Yerleşik filtreler -->
<p>{{ metin|upper }}</p>
<p>{{ metin|lower }}</p>
<p>{{ metin|title }}</p>
<p>{{ sayi|round(2) }}</p>
<p>{{ tarih|date }}</p>
<p>{{ liste|length }}</p>
<p>{{ yazi|truncate(50) }}</p>
<p>{{ html_icerik|safe }}</p>  <!-- XSS riski! -->
<p>{{ json_veri|tojson }}</p>

<!-- Özel filtre -->
{% filter uppercase %}
Bu metin büyük harfe çevrilir.
{% endfilter %}

<!-- Macro (tekrar kullanılabilir HTML) -->
{% macro input(name, label, type="text", value="") %}
    <div class="form-group">
        <label for="{{ name }}">{{ label }}</label>
        <input type="{{ type }}" name="{{ name }}" value="{{ value }}" id="{{ name }}">
    </div>
{% endmacro %}

<form>
    {{ input("isim", "Adınız") }}
    {{ input("eposta", "E-posta", type="email") }}
    {{ input("sifre", "Şifre", type="password") }}
</form>
```

### Request ve Response

```python
from flask import Flask, request, jsonify, make_response, redirect, abort
from datetime import timedelta

app = Flask(__name__)

# Request özellikleri
@app.route("/istek-detay", methods=["GET", "POST"])
def istek_detay():
    return f"""
        <h1>İstek Detayı</h1>
        <ul>
            <li>Metod: {request.method}</li>
            <li>URL: {request.url}</li>
            <li>Path: {request.path}</li>
            <li>Host: {request.host}</li>
            <li>User-Agent: {request.user_agent}</li>
            <li>IP: {request.remote_addr}</li>
            <li>JSON: {request.is_json}</li>
            <li>Content-Type: {request.content_type}</li>
        </ul>
        <h2>GET Parametreleri</h2>
        <p>{dict(request.args)}</p>
        <h2>POST Verileri</h2>
        <p>{dict(request.form)}</p>
        <h2>JSON Verisi</h2>
        <p>{request.get_json(silent=True)}</p>
        <h2>Header'lar</h2>
        <p>{dict(request.headers)}</p>
    """

# Özel response oluşturma
@app.route("/ozel-yanit")
def ozel_yanit():
    yanit = make_response("<h1>Özel Yanıt</h1>")
    yanit.status_code = 201
    yanit.headers["X-Ozel-Header"] = "deger"
    yanit.set_cookie(
        "kullanici_id", "123",
        max_age=timedelta(days=7),
        httponly=True,
        secure=True,
    )
    return yanit

# Yönlendirme
@app.route("/eski-sayfa")
def eski_sayfa():
    return redirect("/yeni-sayfa", code=301)

@app.route("/login-gerekli")
def login_gerekli():
    if not kullanici_girisli():
        return redirect(url_for("giris_yap"))
    return "Hoş geldiniz!"

# Hata fırlatma
@app.route("/yazi/<int:yazi_id>")
def yazi_goster(yazi_id):
    yazi = Yazi.query.get(yazi_id)
    if yazi is None:
        abort(404, description="Yazı bulunamadı")
    return render_template("yazi.html", yazi=yazi)

# JSON API yanıtı
@app.route("/api/kullanicilar")
def api_kullanicilar():
    kullanicilar = [
        {"id": 1, "ad": "Ali", "email": "ali@ornek.com"},
        {"id": 2, "ad": "Ayşe", "email": "ayse@ornek.com"},
    ]
    return jsonify(kullanicilar)
```

### Form İşleme

```python
from flask import Flask, render_template, request, redirect, url_for, flash

app = Flask(__name__)
app.secret_key = "gizli-anahtar-buraya"  # flash mesajları için gerekli

@app.route("/kayit", methods=["GET", "POST"])
def kayit():
    if request.method == "POST":
        kullanici_adi = request.form.get("kullanici_adi", "").strip()
        email = request.form.get("email", "").strip()
        sifre = request.form.get("sifre", "")
        sifre_tekrar = request.form.get("sifre_tekrar", "")

        hatalar = []
        if len(kullanici_adi) < 3:
            hatalar.append("Kullanıcı adı en az 3 karakter olmalıdır.")
        if "@" not in email:
            hatalar.append("Geçerli bir e-posta giriniz.")
        if len(sifre) < 6:
            hatalar.append("Şifre en az 6 karakter olmalıdır.")
        if sifre != sifre_tekrar:
            hatalar.append("Şifreler eşleşmiyor.")

        if hatalar:
            for hata in hatalar:
                flash(hata, "hata")
            return render_template("kayit.html", kullanici_adi=kullanici_adi, email=email)

        # Kullanıcıyı kaydet (veritabanı işlemi)
        flash("Kayıt başarılı! Giriş yapabilirsiniz.", "basarili")
        return redirect(url_for("giris"))

    return render_template("kayit.html")
```

```html
<!-- templates/kayit.html -->
{% extends "base.html" %}

{% block content %}
<h1>Kayıt Ol</h1>

{% with messages = get_flashed_messages(with_categories=true) %}
    {% if messages %}
        {% for kategori, mesaj in messages %}
            <div class="alert alert-{{ kategori }}">{{ mesaj }}</div>
        {% endfor %}
    {% endif %}
{% endwith %}

<form method="post">
    <div>
        <label for="kullanici_adi">Kullanıcı Adı:</label>
        <input type="text" id="kullanici_adi" name="kullanici_adi"
               value="{{ kullanici_adi or '' }}" required>
    </div>
    <div>
        <label for="email">E-posta:</label>
        <input type="email" id="email" name="email"
               value="{{ email or '' }}" required>
    </div>
    <div>
        <label for="sifre">Şifre:</label>
        <input type="password" id="sifre" name="sifre" required>
    </div>
    <div>
        <label for="sifre_tekrar">Şifre Tekrar:</label>
        <input type="password" id="sifre_tekrar" name="sifre_tekrar" required>
    </div>
    <button type="submit">Kayıt Ol</button>
</form>
{% endblock %}
```

### WTForms ile Form Yönetimi

```python
# pip install flask-wtf

from flask import Flask, render_template, redirect, url_for, flash
from flask_wtf import FlaskForm
from wtforms import StringField, PasswordField, BooleanField, SubmitField
from wtforms.validators import DataRequired, Email, Length, EqualTo

app = Flask(__name__)
app.config["SECRET_KEY"] = "gizli-anahtar"
app.config["WTF_CSRF_ENABLED"] = True

class KayitFormu(FlaskForm):
    kullanici_adi = StringField(
        "Kullanıcı Adı",
        validators=[DataRequired(), Length(min=3, max=20)]
    )
    email = StringField(
        "E-posta",
        validators=[DataRequired(), Email()]
    )
    sifre = PasswordField(
        "Şifre",
        validators=[DataRequired(), Length(min=6)]
    )
    sifre_tekrar = PasswordField(
        "Şifre Tekrar",
        validators=[DataRequired(), EqualTo("sifre", "Şifreler eşleşmiyor!")]
    )
    kabul = BooleanField("Kullanım koşullarını kabul ediyorum", validators=[DataRequired()])
    gonder = SubmitField("Kayıt Ol")

@app.route("/kayit-wtf", methods=["GET", "POST"])
def kayit_wtf():
    form = KayitFormu()
    if form.validate_on_submit():
        flash(f"Hoş geldin {form.kullanici_adi.data}!", "basarili")
        return redirect(url_for("index"))
    return render_template("kayit_wtf.html", form=form)
```

```html
<!-- templates/kayit_wtf.html -->
{% extends "base.html" %}

{% block content %}
<h1>Kayıt Ol (WTForms)</h1>

<form method="post">
    {{ form.hidden_tag() }}

    <div>
        {{ form.kullanici_adi.label }}
        {{ form.kullanici_adi(class="form-control") }}
        {% for hata in form.kullanici_adi.errors %}
            <span class="hata">{{ hata }}</span>
        {% endfor %}
    </div>

    <div>
        {{ form.email.label }}
        {{ form.email(class="form-control") }}
        {% for hata in form.email.errors %}
            <span class="hata">{{ hata }}</span>
        {% endfor %}
    </div>

    <div>
        {{ form.sifre.label }}
        {{ form.sifre(class="form-control") }}
        {% for hata in form.sifre.errors %}
            <span class="hata">{{ hata }}</span>
        {% endfor %}
    </div>

    <div>
        {{ form.sifre_tekrar.label }}
        {{ form.sifre_tekrar(class="form-control") }}
        {% for hata in form.sifre_tekrar.errors %}
            <span class="hata">{{ hata }}</span>
        {% endfor %}
    </div>

    <div>
        {{ form.kabul() }} {{ form.kabul.label }}
        {% for hata in form.kabul.errors %}
            <span class="hata">{{ hata }}</span>
        {% endfor %}
    </div>

    {{ form.gonder(class="btn btn-primary") }}
</form>
{% endblock %}
```

### SQLAlchemy ile Veritabanı

```python
# pip install flask-sqlalchemy

from flask import Flask
from flask_sqlalchemy import SQLAlchemy
from datetime import datetime

app = Flask(__name__)
app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///site.db"
app.config["SQLALCHEMY_TRACK_MODIFICATIONS"] = False
db = SQLAlchemy(app)

# Modeller
class Kullanici(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    kullanici_adi = db.Column(db.String(20), unique=True, nullable=False)
    email = db.Column(db.String(120), unique=True, nullable=False)
    sifre_hash = db.Column(db.String(60), nullable=False)
    kayit_tarihi = db.Column(db.DateTime, default=datetime.utcnow)
    yazilar = db.relationship("Yazi", backref="yazar", lazy=True)

    def __repr__(self):
        return f"Kullanici('{self.kullanici_adi}', '{self.email}')"

class Yazi(db.Model):
    id = db.Column(db.Integer, primary_key=True)
    baslik = db.Column(db.String(100), nullable=False)
    icerik = db.Column(db.Text, nullable=False)
    tarih = db.Column(db.DateTime, default=datetime.utcnow)
    kullanici_id = db.Column(db.Integer, db.ForeignKey("kullanici.id"), nullable=False)

    def __repr__(self):
        return f"Yazi('{self.baslik}', '{self.tarih}')"

# Veritabanını oluşturma
with app.app_context():
    db.create_all()

# CRUD işlemleri
@app.route("/kullanici/ekle")
def kullanici_ekle():
    kullanici = Kullanici(
        kullanici_adi="ali",
        email="ali@ornek.com",
        sifre_hash="hashli-sifre"
    )
    db.session.add(kullanici)
    db.session.commit()
    return "Kullanıcı eklendi!"

@app.route("/kullanicilar")
def kullanicilar():
    tumu = Kullanici.query.all()
    ilk = Kullanici.query.first()
    filtre = Kullanici.query.filter_by(kullanici_adi="ali").all()
    say = Kullanici.query.count()
    sirali = Kullanici.query.order_by(Kullanici.kayit_tarihi.desc()).all()

    # Pagination
    sayfa = request.args.get("sayfa", 1, type=int)
    sayfalanmis = Kullanici.query.paginate(page=sayfa, per_page=10)

    return render_template("kullanicilar.html", kullanicilar=tumu)
```

### Blueprint ile Modüler Yapı

```
Proje dizini:
flask_app/
├── app.py
├── config.py
├── models.py
├── extensions.py
├── blueprints/
│   ├── __init__.py
│   ├── auth/
│   │   ├── __init__.py
│   │   └── routes.py
│   └── blog/
│       ├── __init__.py
│       └── routes.py
├── templates/
│   ├── base.html
│   ├── auth/
│   │   ├── login.html
│   │   └── register.html
│   └── blog/
│       ├── index.html
│       └── post.html
└── static/
    ├── style.css
    └── script.js
```

```python
# extensions.py (bağımlılıkları tek dosyada topla)
from flask_sqlalchemy import SQLAlchemy
from flask_login import LoginManager

db = SQLAlchemy()
login_manager = LoginManager()
```

```python
# blueprints/auth/routes.py
from flask import Blueprint, render_template, redirect, url_for, flash, request
from flask_login import login_user, logout_user, login_required

auth = Blueprint("auth", __name__, url_prefix="/auth", template_folder="templates/auth")

@auth.route("/login", methods=["GET", "POST"])
def login():
    if request.method == "POST":
        kullanici_adi = request.form.get("kullanici_adi")
        sifre = request.form.get("sifre")
        # Kullanıcı doğrulama
        return redirect(url_for("blog.index"))
    return render_template("login.html")

@auth.route("/logout")
@login_required
def logout():
    logout_user()
    return redirect(url_for("auth.login"))
```

```python
# blueprints/blog/routes.py
from flask import Blueprint, render_template
from flask_login import login_required

blog = Blueprint("blog", __name__, template_folder="templates/blog")

@blog.route("/")
def index():
    return render_template("index.html")

@blog.route("/yazi/<int:yazi_id>")
def yazi_goster(yazi_id):
    return render_template("post.html", yazi_id=yazi_id)
```

```python
# app.py (ana dosya)
from flask import Flask
from extensions import db, login_manager
from blueprints.auth.routes import auth
from blueprints.blog.routes import blog

def create_app():
    app = Flask(__name__)
    app.config.from_object("config.Config")

    db.init_app(app)
    login_manager.init_app(app)

    app.register_blueprint(auth)
    app.register_blueprint(blog)

    return app

if __name__ == "__main__":
    app = create_app()
    app.run(debug=True)
```

```python
# config.py
import os

class Config:
    SECRET_KEY = os.environ.get("SECRET_KEY", "gizli-anahtar")
    SQLALCHEMY_DATABASE_URI = os.environ.get("DATABASE_URL", "sqlite:///site.db")
    SQLALCHEMY_TRACK_MODIFICATIONS = False
```

### Kullanıcı Doğrulama (Flask-Login)

```python
# pip install flask-login
from flask_login import UserMixin, login_user, logout_user, login_required, current_user

class Kullanici(UserMixin, db.Model):
    id = db.Column(db.Integer, primary_key=True)
    kullanici_adi = db.Column(db.String(20), unique=True, nullable=False)
    email = db.Column(db.String(120), unique=True, nullable=False)
    sifre_hash = db.Column(db.String(128), nullable=False)

    def sifre_kontrol(self, sifre):
        return check_password_hash(self.sifre_hash, sifre)

# flask-login yapılandırması
from extensions import login_manager

@login_manager.user_loader
def load_user(kullanici_id):
    return Kullanici.query.get(int(kullanici_id))

login_manager.login_view = "auth.login"
login_manager.login_message = "Bu sayfayı görmek için giriş yapmalısınız."
login_manager.login_message_category = "bilgi"
```

```python
# Şifre hashleme (werkzeug ile)
from werkzeug.security import generate_password_hash, check_password_hash

sifre_hash = generate_password_hash("sifre123")
print(sifre_hash)  # pbkdf2:sha256:... formatında

sonuc = check_password_hash(sifre_hash, "sifre123")  # True
sonuc = check_password_hash(sifre_hash, "yanlis")    # False
```

### Session Kullanımı

```python
from flask import Flask, session

app = Flask(__name__)
app.secret_key = "gizli-anahtar"
app.config["SESSION_PERMANENT"] = False
app.config["SESSION_COOKIE_HTTPONLY"] = True
app.config["SESSION_COOKIE_SECURE"] = True  # HTTPS gerektirir

@app.route("/oturum-ayarla")
def oturum_ayarla():
    session["kullanici_adi"] = "Ali"
    session["rol"] = "admin"
    session["ziyaret_sayisi"] = session.get("ziyaret_sayisi", 0) + 1
    return f"Oturum ayarlandı: {dict(session)}"

@app.route("/oturum-goster")
def oturum_goster():
    kullanici = session.get("kullanici_adi", "Misafir")
    ziyaret = session.get("ziyaret_sayisi", 0)
    return f"Kullanıcı: {kullanici}, Ziyaret: {ziyaret}"

@app.route("/oturum-temizle")
def oturum_temizle():
    session.clear()
    return "Oturum temizlendi"
```

### Flask Extension'ları (Popüler Paketler)

```python
# Veritabanı
# flask-sqlalchemy: ORM
# flask-migrate: veritabanı migration (Alembic tabanlı)
# flask-marshmallow: serileştirme/deserileştirme

# Kimlik Doğrulama
# flask-login: kullanıcı oturum yönetimi
# flask-security: kapsamlı güvenlik
# flask-jwt-extended: JWT token tabanlı auth

# Form
# flask-wtf: CSRF korumalı formlar
# wtforms: form validasyonu

# Email
# flask-mail: e-posta gönderme

# Admin
# flask-admin: otomatik admin paneli

# API
# flask-restful: REST API framework'ü
# flask-smorest: OpenAPI dokümantasyonlu REST API

# Diğer
# flask-caching: önbellek (Redis/Memcached)
# flask-cors: CORS desteği
# flask-limiter: rate limiting
# flask-socketio: WebSocket desteği
# flask-apscheduler: zamanlanmış görevler
```

### Hata Yönetimi

```python
from flask import Flask, render_template

app = Flask(__name__)

# Özel hata sayfaları
@app.errorhandler(404)
def sayfa_bulunamadi(e):
    return render_template("errors/404.html"), 404

@app.errorhandler(403)
def yetkisiz_erisim(e):
    return render_template("errors/403.html"), 403

@app.errorhandler(500)
def sunucu_hatasi(e):
    return render_template("errors/500.html"), 500

# Tüm hataları yakalama
@app.errorhandler(Exception)
def genel_hata(e):
    app.logger.error(f"Beklenmeyen hata: {e}")
    return render_template("errors/500.html"), 500
```

```html
<!-- templates/errors/404.html -->
{% extends "base.html" %}

{% block content %}
<div class="error-page">
    <h1>404</h1>
    <p>Aradığınız sayfa bulunamadı.</p>
    <a href="{{ url_for('index') }}">Ana Sayfaya Dön</a>
</div>
{% endblock %}
```

### Dosya Yükleme

```python
import os
from flask import Flask, request, render_template, redirect, url_for
from werkzeug.utils import secure_filename

app = Flask(__name__)

UPLOAD_FOLDER = "static/uploads"
ALLOWED_EXTENSIONS = {"png", "jpg", "jpeg", "gif", "pdf"}
MAX_CONTENT_LENGTH = 16 * 1024 * 1024  # 16 MB

app.config["UPLOAD_FOLDER"] = UPLOAD_FOLDER
app.config["MAX_CONTENT_LENGTH"] = MAX_CONTENT_LENGTH

os.makedirs(UPLOAD_FOLDER, exist_ok=True)

def izinli_dosya(dosya_adi):
    return "." in dosya_adi and \
           dosya_adi.rsplit(".", 1)[1].lower() in ALLOWED_EXTENSIONS

@app.route("/yukle", methods=["GET", "POST"])
def dosya_yukle():
    if request.method == "POST":
        if "dosya" not in request.files:
            flash("Dosya seçilmedi!", "hata")
            return redirect(request.url)

        dosya = request.files["dosya"]
        if dosya.filename == "":
            flash("Dosya seçilmedi!", "hata")
            return redirect(request.url)

        if dosya and izinli_dosya(dosya.filename):
            dosya_adi = secure_filename(dosya.filename)  # güvenli isim
            dosya_yolu = os.path.join(app.config["UPLOAD_FOLDER"], dosya_adi)
            dosya.save(dosya_yolu)
            flash("Dosya başarıyla yüklendi!", "basarili")
            return redirect(url_for("dosya_goster", dosya_adi=dosya_adi))

        flash("Bu dosya türüne izin verilmiyor!", "hata")

    return render_template("yukle.html")
```

```html
<!-- templates/yukle.html -->
{% extends "base.html" %}

{% block content %}
<h1>Dosya Yükle</h1>

<form method="post" enctype="multipart/form-data">
    <input type="file" name="dosya" accept="image/*,.pdf" required>
    <button type="submit">Yükle</button>
</form>
{% endblock %}
```

### Decorator ile Middleware (Özel Dekoratörler)

```python
from flask import Flask, abort, request
from functools import wraps
import time

app = Flask(__name__)

# Giriş kontrolü
def admin_gerekli(f):
    @wraps(f)
    def decorated_function(*args, **kwargs):
        if not session.get("rol") == "admin":
            abort(403)
        return f(*args, **kwargs)
    return decorated_function

@app.route("/admin")
@admin_gerekli
def admin_paneli():
    return "Admin Paneli"

# İstek loglama
def logla(f):
    @wraps(f)
    def decorated(*args, **kwargs):
        app.logger.info(f"{request.method} {request.path} - {request.remote_addr}")
        return f(*args, **kwargs)
    return decorated

@app.route("/")
@logla
def index():
    return "Ana Sayfa"

# İstek süresi ölçüm
def sure_olc(f):
    @wraps(f)
    def decorated(*args, **kwargs):
        baslangic = time.perf_counter()
        sonuc = f(*args, **kwargs)
        sure = time.perf_counter() - baslangic
        print(f"{f.__name__}: {sure*1000:.2f}ms")
        return sonuc
    return decorated

@app.route("/yavas")
@sure_olc
def yavas_sayfa():
    time.sleep(0.5)
    return "Yavaş sayfa"

# before_request ve after_request (global middleware)
@app.before_request
def istek_once():
    request.baslangic_zamani = time.perf_counter()

@app.after_request
def istek_sonra(yanit):
    sure = time.perf_counter() - request.baslangic_zamani
    yanit.headers["X-Islek-Suresi"] = f"{sure*1000:.0f}ms"
    return yanit
```

### REST API Geliştirme

```python
from flask import Flask, jsonify, request, abort
from flask.views import MethodView

app = Flask(__name__)

# MethodView ile class-based API
class KullaniciAPI(MethodView):
    def get(self, kullanici_id=None):
        if kullanici_id is None:
            # Tüm kullanıcıları listele
            kullanicilar = Kullanici.query.all()
            return jsonify([{
                "id": k.id,
                "kullanici_adi": k.kullanici_adi,
                "email": k.email,
            } for k in kullanicilar])

        kullanici = Kullanici.query.get(kullanici_id)
        if kullanici is None:
            abort(404, description="Kullanıcı bulunamadı")
        return jsonify({
            "id": kullanici.id,
            "kullanici_adi": kullanici.kullanici_adi,
            "email": kullanici.email,
        })

    def post(self):
        veri = request.get_json()
        if not veri or "kullanici_adi" not in veri:
            abort(400, description="Geçersiz veri")

        kullanici = Kullanici(
            kullanici_adi=veri["kullanici_adi"],
            email=veri.get("email", ""),
            sifre_hash=generate_password_hash(veri.get("sifre", "")),
        )
        db.session.add(kullanici)
        db.session.commit()

        return jsonify({"id": kullanici.id, "mesaj": "Kullanıcı oluşturuldu"}), 201

    def put(self, kullanici_id):
        kullanici = Kullanici.query.get(kullanici_id)
        if kullanici is None:
            abort(404)

        veri = request.get_json()
        if "email" in veri:
            kullanici.email = veri["email"]
        db.session.commit()

        return jsonify({"mesaj": "Kullanıcı güncellendi"})

    def delete(self, kullanici_id):
        kullanici = Kullanici.query.get(kullanici_id)
        if kullanici is None:
            abort(404)

        db.session.delete(kullanici)
        db.session.commit()

        return jsonify({"mesaj": "Kullanıcı silindi"}), 204

# URL yapılandırması
kullanici_view = KullaniciAPI.as_view("kullanici_api")
app.add_url_rule("/api/kullanicilar", view_func=kullanici_view, methods=["GET"])
app.add_url_rule("/api/kullanicilar", view_func=kullanici_view, methods=["POST"])
app.add_url_rule("/api/kullanicilar/<int:kullanici_id>", view_func=kullanici_view,
                 methods=["GET", "PUT", "DELETE"])
```

### Uygulama Testi

```python
# pip install pytest

# test_app.py
import pytest
from app import create_app, db

@pytest.fixture
def app():
    app = create_app()
    app.config["TESTING"] = True
    app.config["SQLALCHEMY_DATABASE_URI"] = "sqlite:///:memory:"
    app.config["WTF_CSRF_ENABLED"] = False

    with app.app_context():
        db.create_all()
        yield app
        db.drop_all()

@pytest.fixture
def client(app):
    return app.test_client()

@pytest.fixture
def runner(app):
    return app.test_cli_runner()

def test_index_sayfasi(client):
    response = client.get("/")
    assert response.status_code == 200
    assert b"Merhaba" in response.data

def test_kullanici_kayit(client):
    response = client.post("/kayit", data={
        "kullanici_adi": "testuser",
        "email": "test@ornek.com",
        "sifre": "sifre123",
        "sifre_tekrar": "sifre123",
    })
    assert response.status_code == 302  # redirect

def test_api_kullanicilar(client):
    response = client.get("/api/kullanicilar")
    assert response.status_code == 200
    assert response.is_json

def test_404(client):
    response = client.get("/olmayan-sayfa")
    assert response.status_code == 404

def test_login_required(client):
    response = client.get("/admin")
    assert response.status_code == 302  # login sayfasına yönlendirir

def test_json_api(client):
    response = client.post("/api/kullanicilar",
        json={"kullanici_adi": "ali", "email": "ali@ornek.com", "sifre": "123456"},
        content_type="application/json",
    )
    assert response.status_code == 201
```

### Deployment (Canlıya Alma)

```bash
# Gunicorn ile (production WSGI sunucusu)
pip install gunicorn

# Çalıştırma
gunicorn -w 4 -b 0.0.0.0:8000 wsgi:app

# -w: worker sayısı (genelde CPU çekirdek sayısı * 2 + 1)
# -b: bind address
```

```python
# wsgi.py (gunicorn entry point)
from app import create_app

app = create_app()
```

```dockerfile
# Dockerfile
FROM python:3.12-slim

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

ENV FLASK_ENV=production
EXPOSE 8000

CMD ["gunicorn", "-w", "4", "-b", "0.0.0.0:8000", "wsgi:app"]
```

```txt
# requirements.txt
Flask==3.0.*
Flask-SQLAlchemy==3.1.*
Flask-Login==0.6.*
Flask-WTF==1.2.*
Flask-Migrate==4.0.*
python-dotenv==1.0.*
gunicorn==22.0.*
```

```bash
# Environment variables (.env dosyası ile)
pip install python-dotenv

# .env
FLASK_APP=app.py
FLASK_ENV=development
SECRET_KEY=uretilmis-gizli-anahtar
DATABASE_URL=sqlite:///site.db
```

```python
# python-dotenv ile
from dotenv import load_dotenv
load_dotenv()
```

### Proje Şablonu (Tam Örnek)

```
flask_blog/
├── .env
├── .gitignore
├── requirements.txt
├── Dockerfile
├── wsgi.py
├── config.py
├── app.py                    # uygulama fabrikası
├── extensions.py             # db, login_manager vb.
├── models.py                 # veritabanı modelleri
├── blueprints/
│   ├── __init__.py
│   ├── main/
│   │   ├── __init__.py
│   │   └── routes.py
│   ├── auth/
│   │   ├── __init__.py
│   │   └── routes.py
│   └── blog/
│       ├── __init__.py
│       ├── routes.py
│       └── forms.py
├── templates/
│   ├── base.html
│   ├── index.html
│   ├── auth/
│   │   ├── login.html
│   │   └── register.html
│   ├── blog/
│   │   ├── index.html
│   │   ├── create.html
│   │   └── post.html
│   └── errors/
│       ├── 404.html
│       ├── 403.html
│       └── 500.html
├── static/
│   ├── css/
│   │   └── style.css
│   ├── js/
│   │   └── main.js
│   └── uploads/
└── tests/
    ├── __init__.py
    ├── conftest.py
    └── test_routes.py
```

### Özet: Flask ile Yapılabilecekler

- Kişisel blog / portfolyo sitesi
- Haber portalı
- E-ticaret sitesi (küçük ölçekli)
- REST API
- Dosya yönetim sistemi
- Yönetim paneli (Flask-Admin ile)
- WebSocket uygulamaları (Flask-SocketIO ile)
- Mikroservis (Flask + Docker)
- Kullanıcı yönetimli her türlü web uygulaması

---

## <a id="fastapi"></a>FastAPI:

FastAPI, modern, yüksek performanslı bir web framework'üdür. Asenkron desteği, otomatik OpenAPI dokümantasyonu ve Pydantic tabanlı veri doğrulaması ile öne çıkar.

### Kurulum ve İlk Uygulama

```bash
pip install fastapi uvicorn
```

```python
# main.py
from fastapi import FastAPI

app = FastAPI(title="FastAPI Uygulaması", version="1.0.0")

@app.get("/")
def index():
    return {"mesaj": "Merhaba Dünya!"}
```

```bash
uvicorn main:app --reload
# http://127.0.0.1:8000
# http://127.0.0.1:8000/docs (Swagger UI)
# http://127.0.0.1:8000/redoc (ReDoc)
```

### Path Operations ve Routing

```python
from fastapi import FastAPI

app = FastAPI()

# Statik rota
@app.get("/")
def index():
    return {"mesaj": "Ana Sayfa"}

# Path parametresi
@app.get("/kullanici/{kullanici_id}")
def kullanici_getir(kullanici_id: int):
    return {"kullanici_id": kullanici_id}

# Birden çok path parametresi
@app.get("/urun/{kategori}/{urun_id}")
def urun_detay(kategori: str, urun_id: int):
    return {"kategori": kategori, "urun_id": urun_id}

# HTTP metodları
@app.post("/kullanici")
def kullanici_olustur():
    return {"mesaj": "Kullanıcı oluşturuldu"}

@app.put("/kullanici/{kullanici_id}")
def kullanici_guncelle(kullanici_id: int):
    return {"mesaj": f"Kullanıcı {kullanici_id} güncellendi"}

@app.delete("/kullanici/{kullanici_id}")
def kullanici_sil(kullanici_id: int):
    return {"mesaj": f"Kullanıcı {kullanici_id} silindi"}
```

### Query Parametreleri

```python
from fastapi import FastAPI
from typing import Optional

app = FastAPI()

# Query parametreleri
@app.get("/kullanicilar/")
def kullanicilar_listele(
    skip: int = 0,
    limit: int = 10,
    aktif: Optional[bool] = None,
    ara: Optional[str] = None,
):
    return {
        "skip": skip,
        "limit": limit,
        "aktif": aktif,
        "ara": ara,
    }

# Zorunlu query parametresi
@app.get("/urunler/")
def urun_listele(kategori: str):  # query parametresi (path'te değil)
    return {"kategori": kategori}
```

### Pydantic ile Request Body

```python
from fastapi import FastAPI
from pydantic import BaseModel, Field, EmailStr
from typing import List, Optional
from datetime import datetime

app = FastAPI()

# Model tanımı
class KullaniciBase(BaseModel):
    kullanici_adi: str = Field(..., min_length=3, max_length=50)
    email: EmailStr
    yas: int = Field(ge=0, le=150)
    aktif: bool = True

class KullaniciOlustur(KullaniciBase):
    sifre: str = Field(..., min_length=6)

class Kullanici(KullaniciBase):
    id: int
    kayit_tarihi: datetime

    class Config:
        from_attributes = True

class Adres(BaseModel):
    sehir: str
    ilce: str
    acik_adres: str

class KullaniciTam(KullaniciOlustur):
    adresler: List[Adres] = []

# Request body kullanımı
@app.post("/kullanicilar", response_model=Kullanici)
def kullanici_olustur(kullanici: KullaniciOlustur):
    # kullanici.kullanici_adi, kullanici.email, kullanici.sifre
    # Pydantic otomatik doğrulama yapar
    return {
        "id": 1,
        "kullanici_adi": kullanici.kullanici_adi,
        "email": kullanici.email,
        "yas": kullanici.yas,
        "aktif": kullanici.aktif,
        "kayit_tarihi": datetime.now(),
    }

# Birden çok model (body, query, path bir arada)
@app.put("/kullanicilar/{kullanici_id}")
def kullanici_guncelle(
    kullanici_id: int,                               # path
    kullanici: KullaniciOlustur,                     # body
    dogrulama_kodu: Optional[str] = None,            # query
):
    return {
        "kullanici_id": kullanici_id,
        "kullanici": kullanici,
        "dogrulama_kodu": dogrulama_kodu,
    }
```

### Response Modelleri

```python
from fastapi import FastAPI
from pydantic import BaseModel
from typing import List, Optional

app = FastAPI()

class Urun(BaseModel):
    id: int
    ad: str
    fiyat: float
    stok: int

class UrunDetay(Urun):
    aciklama: Optional[str] = None
    kategoriler: List[str] = []

# Otomatik response doğrulama ve filtreleme
@app.get("/urunler", response_model=List[Urun])
def urun_listele():
    # Veritabanından gelen tüm alanlar olsa bile
    # response_model sadece belirtilen alanları döndürür
    return [
        {"id": 1, "ad": "Laptop", "fiyat": 25000.0, "stok": 10, "gizli_not": "..."},
        {"id": 2, "ad": "Mouse", "fiyat": 500.0, "stok": 50, "gizli_not": "..."},
    ]

# Farklı response modelleri
class KullaniciCikti(BaseModel):
    id: int
    kullanici_adi: str
    email: str

class KullaniciCiktiDetay(KullaniciCikti):
    yas: int
    kayit_tarihi: str

@app.get("/kullanicilar", response_model=List[KullaniciCikti])
def kullanicilar_listele():
    pass

@app.get("/kullanicilar/{kullanici_id}", response_model=KullaniciCiktiDetay)
def kullanici_getir(kullanici_id: int):
    pass

# response_model_exclude_unset
@app.get("/kullanici/{id}", response_model=KullaniciCikti, response_model_exclude_unset=True)
def kullanici_ozet(id: int):
    # Sadece istemcinin gönderdiği alanları döndürür
    pass
```

### Path ve Query Validasyonu

```python
from fastapi import FastAPI, Path, Query
from typing import Optional

app = FastAPI()

# Path validasyonu
@app.get("/urun/{urun_id}")
def urun_getir(
    urun_id: int = Path(..., ge=1, le=10000, description="Ürün ID'si"),
):
    return {"urun_id": urun_id}

# Query validasyonu
@app.get("/kullanicilar/")
def kullanicilar_listele(
    page: int = Query(1, ge=1, description="Sayfa numarası"),
    limit: int = Query(10, ge=1, le=100, description="Sayfa başına kayıt"),
    sort: Optional[str] = Query(None, regex="^(ad|tarih|yas)$"),
):
    return {"page": page, "limit": limit, "sort": sort}

# Enum ile sabit değerler
from enum import Enum

class Siralama(str, Enum):
    ARTAN = "artan"
    AZALAN = "azalan"
    TARIH = "tarih"

@app.get("/sirali")
def sirali_listele(sira: Siralama = Siralama.ARTAN):
    return {"sira": sira}
```

### Dependency Injection

```python
from fastapi import FastAPI, Depends, HTTPException, Header
from typing import Optional

app = FastAPI()

# Basit dependency
def pagination(
    page: int = 1,
    limit: int = 10,
):
    skip = (page - 1) * limit
    return {"skip": skip, "limit": limit}

@app.get("/kullanicilar/")
def kullanicilar_listele(p: dict = Depends(pagination)):
    return {"skip": p["skip"], "limit": p["limit"]}

# Sınıf dependency
from dataclasses import dataclass

@dataclass
class PaginationParams:
    page: int = 1
    limit: int = 10

    @property
    def skip(self) -> int:
        return (self.page - 1) * self.limit

@app.get("/urunler/")
def urunler_listele(p: PaginationParams = Depends()):
    return {"skip": p.skip, "limit": p.limit}

# Veritabanı dependency
class Veritabani:
    def __init__(self):
        self.baglanti = "veritabani-baglantisi"

    def sorgu(self, sorgu: str):
        return f"{sorgu} sonucu"

    def kapat(self):
        pass

def get_db():
    db = Veritabani()
    try:
        yield db
    finally:
        db.kapat()

@app.get("/veri/")
def veri_getir(db: Veritabani = Depends(get_db)):
    return {"veri": db.sorgu("SELECT * FROM tablo")}

# Dependency zincirleme
def dogrulama(token: Optional[str] = Header(None)):
    if token is None:
        raise HTTPException(status_code=401, detail="Token gerekli")
    if token != "gizli-token":
        raise HTTPException(status_code=403, detail="Geçersiz token")
    return {"kullanici": "admin", "rol": "admin"}

def admin_yetki(auth: dict = Depends(dogrulama)):
    if auth["rol"] != "admin":
        raise HTTPException(status_code=403, detail="Admin yetkisi gerekli")
    return auth

@app.get("/admin-panel")
def admin_paneli(auth: dict = Depends(admin_yetki)):
    return {"mesaj": "Admin paneline hoş geldiniz", "kullanici": auth["kullanici"]}

# Global dependency (tüm rotalara uygulanır)
app = FastAPI(dependencies=[Depends(dogrulama)])
```

### Asenkron (async) Kullanımı

```python
from fastapi import FastAPI
import asyncio

app = FastAPI()

# async endpoint
@app.get("/async-ornek")
async def async_endpoint():
    await asyncio.sleep(1)
    return {"mesaj": "Async endpoint"}

# sync/async karışık
@app.get("/sync")
def sync_endpoint():
    return {"mesaj": "Sync endpoint"}

# await kullanımı
@app.get("/bekle/{sure}")
async def bekle(sure: float):
    await asyncio.sleep(sure)
    return {
        "beklenen": sure,
        "zaman": asyncio.get_event_loop().time(),
    }

# Asenkron dosya okuma
import aiofiles

@app.get("/dosya")
async def dosya_oku():
    async with aiofiles.open("veri.txt", "r") as f:
        icerik = await f.read()
    return {"icerik": icerik}
```

### Hata Yönetimi

```python
from fastapi import FastAPI, HTTPException, status
from fastapi.responses import JSONResponse
from pydantic import BaseModel

app = FastAPI()

# HTTPException
@app.get("/kullanici/{kullanici_id}")
def kullanici_getir(kullanici_id: int):
    if kullanici_id <= 0:
        raise HTTPException(
            status_code=status.HTTP_400_BAD_REQUEST,
            detail="Geçersiz kullanıcı ID",
        )
    if kullanici_id > 100:
        raise HTTPException(
            status_code=status.HTTP_404_NOT_FOUND,
            detail="Kullanıcı bulunamadı",
            headers={"X-Hata": "Kayıt yok"},
        )
    return {"id": kullanici_id, "ad": "Ali"}

# Özel hata sınıfları
class KullaniciBulunamadi(Exception):
    def __init__(self, kullanici_id: int):
        self.kullanici_id = kullanici_id

@app.exception_handler(KullaniciBulunamadi)
def kullanici_bulunamadi_handler(request, exc):
    return JSONResponse(
        status_code=404,
        content={
            "hata": "Kullanıcı bulunamadı",
            "kullanici_id": exc.kullanici_id,
        },
    )

@app.get("/kullanici-ozel/{kullanici_id}")
def kullanici_bul(kullanici_id: int):
    if kullanici_id != 1:
        raise KullaniciBulunamadi(kullanici_id)
    return {"id": 1, "ad": "Ali"}

# Global hata yakalama
class HataModeli(BaseModel):
    hata: str
    detay: str | None = None

@app.exception_handler(500)
def genel_hata(request, exc):
    return JSONResponse(
        status_code=500,
        content=HataModeli(hata="Sunucu hatası", detay=str(exc)).model_dump(),
    )
```

### CORS ve Middleware

```python
from fastapi import FastAPI, Request
from fastapi.middleware.cors import CORSMiddleware
from fastapi.middleware.trustedhost import TrustedHostMiddleware
import time

app = FastAPI()

# CORS yapılandırması
app.add_middleware(
    CORSMiddleware,
    allow_origins=["http://localhost:3000", "https://site.com"],
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

# Güvenilen hostlar
app.add_middleware(
    TrustedHostMiddleware,
    allowed_hosts=["localhost", "127.0.0.1", ".site.com"],
)

# Özel middleware
@app.middleware("http")
async def istek_suresi(request: Request, call_next):
    baslangic = time.perf_counter()
    yanit = await call_next(request)
    sure = time.perf_counter() - baslangic
    yanit.headers["X-Islem-Suresi"] = f"{sure*1000:.0f}ms"
    return yanit

# Gzip sıkıştırma
from fastapi.middleware.gzip import GZipMiddleware
app.add_middleware(GZipMiddleware, minimum_size=1000)

# HTTPS redirect
from fastapi.middleware.httpsredirect import HTTPSRedirectMiddleware
# app.add_middleware(HTTPSRedirectMiddleware)
```

### Dosya Yükleme

```python
from fastapi import FastAPI, File, UploadFile, Form
from typing import List
import shutil
import os

app = FastAPI()
UPLOAD_DIR = "uploads"
os.makedirs(UPLOAD_DIR, exist_ok=True)

# Tek dosya yükleme
@app.post("/yukle")
async def dosya_yukle(dosya: UploadFile = File(...)):
    dosya_yolu = os.path.join(UPLOAD_DIR, dosya.filename)
    with open(dosya_yolu, "wb") as f:
        shutil.copyfileobj(dosya.file, f)
    return {
        "dosya_adi": dosya.filename,
        "boyut": os.path.getsize(dosya_yolu),
        "tip": dosya.content_type,
    }

# Çoklu dosya yükleme
@app.post("/coklu-yukle")
async def coklu_yukle(dosyalar: List[UploadFile] = File(...)):
    sonuclar = []
    for dosya in dosyalar:
        dosya_yolu = os.path.join(UPLOAD_DIR, dosya.filename)
        with open(dosya_yolu, "wb") as f:
            shutil.copyfileobj(dosya.file, f)
        sonuclar.append({
            "dosya_adi": dosya.filename,
            "boyut": os.path.getsize(dosya_yolu),
        })
    return {"yuklenenler": sonuclar}

# Form + dosya
@app.post("/kullanici-yukle")
async def kullanici_yukle(
    isim: str = Form(...),
    email: str = Form(...),
    profil_resmi: UploadFile = File(...),
):
    return {
        "isim": isim,
        "email": email,
        "dosya": profil_resmi.filename,
    }
```

### SQLAlchemy ile Veritabanı

```python
# pip install sqlalchemy
from fastapi import FastAPI, Depends, HTTPException
from sqlalchemy import create_engine, Column, Integer, String, Boolean, DateTime
from sqlalchemy.ext.declarative import declarative_base
from sqlalchemy.orm import sessionmaker, Session
from pydantic import BaseModel
from datetime import datetime
from typing import List, Optional

# Veritabanı yapılandırması
SQLALCHEMY_DATABASE_URL = "sqlite:///./site.db"
engine = create_engine(SQLALCHEMY_DATABASE_URL, connect_args={"check_same_thread": False})
SessionLocal = sessionmaker(autocommit=False, autoflush=False, bind=engine)
Base = declarative_base()

# Model
class KullaniciDB(Base):
    __tablename__ = "kullanicilar"
    id = Column(Integer, primary_key=True, index=True)
    kullanici_adi = Column(String, unique=True, index=True)
    email = Column(String, unique=True, index=True)
    aktif = Column(Boolean, default=True)
    kayit_tarihi = Column(DateTime, default=datetime.utcnow)

Base.metadata.create_all(bind=engine)

# Pydantic schemas
class KullaniciBase(BaseModel):
    kullanici_adi: str
    email: str

class KullaniciOlustur(KullaniciBase):
    pass

class Kullanici(KullaniciBase):
    id: int
    aktif: bool
    kayit_tarihi: datetime
    class Config:
        from_attributes = True

# Dependency
def get_db():
    db = SessionLocal()
    try:
        yield db
    finally:
        db.close()

app = FastAPI()

# CRUD işlemleri
@app.post("/kullanicilar", response_model=Kullanici)
def kullanici_olustur(k: KullaniciOlustur, db: Session = Depends(get_db)):
    db_kullanici = KullaniciDB(**k.model_dump())
    db.add(db_kullanici)
    db.commit()
    db.refresh(db_kullanici)
    return db_kullanici

@app.get("/kullanicilar", response_model=List[Kullanici])
def kullanicilar_listele(
    skip: int = 0,
    limit: int = 10,
    db: Session = Depends(get_db),
):
    return db.query(KullaniciDB).offset(skip).limit(limit).all()

@app.get("/kullanicilar/{kullanici_id}", response_model=Kullanici)
def kullanici_getir(kullanici_id: int, db: Session = Depends(get_db)):
    k = db.query(KullaniciDB).filter(KullaniciDB.id == kullanici_id).first()
    if k is None:
        raise HTTPException(status_code=404, detail="Kullanıcı bulunamadı")
    return k

@app.delete("/kullanicilar/{kullanici_id}")
def kullanici_sil(kullanici_id: int, db: Session = Depends(get_db)):
    k = db.query(KullaniciDB).filter(KullaniciDB.id == kullanici_id).first()
    if k is None:
        raise HTTPException(status_code=404, detail="Kullanıcı bulunamadı")
    db.delete(k)
    db.commit()
    return {"mesaj": "Kullanıcı silindi"}
```

### SQLModel ile ORM (FastAPI + Pydantic)

```python
# pip install sqlmodel
from fastapi import FastAPI, Depends, HTTPException
from sqlmodel import SQLModel, Field, create_engine, Session, select
from typing import Optional, List
from datetime import datetime

app = FastAPI()

# SQLModel: Pydantic + SQLAlchemy birleşimi
class Kullanici(SQLModel, table=True):
    id: Optional[int] = Field(default=None, primary_key=True)
    kullanici_adi: str = Field(index=True, unique=True)
    email: str
    yas: Optional[int] = None
    aktif: bool = True
    kayit_tarihi: datetime = Field(default_factory=datetime.utcnow)

# Veritabanı
engine = create_engine("sqlite:///./sqlmodel.db", connect_args={"check_same_thread": False})
SQLModel.metadata.create_all(engine)

def get_session():
    with Session(engine) as session:
        yield session

@app.post("/kullanicilar")
def kullanici_olustur(k: Kullanici, session: Session = Depends(get_session)):
    session.add(k)
    session.commit()
    session.refresh(k)
    return k

@app.get("/kullanicilar", response_model=List[Kullanici])
def kullanicilar_listele(
    skip: int = 0,
    limit: int = 10,
    session: Session = Depends(get_session),
):
    sorgu = select(Kullanici).offset(skip).limit(limit)
    return session.exec(sorgu).all()
```

### Güvenlik (Security)

```python
# pip install python-jose[cryptography] passlib[bcrypt]
from fastapi import FastAPI, Depends, HTTPException, status
from fastapi.security import OAuth2PasswordBearer, OAuth2PasswordRequestForm
from pydantic import BaseModel
from jose import JWTError, jwt
from passlib.context import CryptContext
from datetime import datetime, timedelta
from typing import Optional

SECRET_KEY = "gizli-anahtar-buraya"
ALGORITHM = "HS256"
ACCESS_TOKEN_EXPIRE_MINUTES = 30

app = FastAPI()
pwd_context = CryptContext(schemes=["bcrypt"], deprecated="auto")
oauth2_scheme = OAuth2PasswordBearer(tokenUrl="token")

# Veritabanı (mock)
fake_db = {
    "admin": {
        "kullanici_adi": "admin",
        "sifre_hash": pwd_context.hash("sifre123"),
        "email": "admin@ornek.com",
        "rol": "admin",
    }
}

class Token(BaseModel):
    access_token: str
    token_type: str

class TokenData(BaseModel):
    kullanici_adi: Optional[str] = None

# Yardımcı fonksiyonlar
def dogrula_kullanici(kullanici_adi: str, sifre: str):
    kullanici = fake_db.get(kullanici_adi)
    if not kullanici or not pwd_context.verify(sifre, kullanici["sifre_hash"]):
        return False
    return kullanici

def token_olustur(veri: dict, expires_delta: Optional[timedelta] = None):
    veri_kopya = veri.copy()
    sonlanma = datetime.utcnow() + (expires_delta or timedelta(minutes=15))
    veri_kopya.update({"exp": sonlanma})
    return jwt.encode(veri_kopya, SECRET_KEY, algorithm=ALGORITHM)

async def aktif_kullanici(token: str = Depends(oauth2_scheme)):
    istisna = HTTPException(
        status_code=status.HTTP_401_UNAUTHORIZED,
        detail="Geçersiz kimlik bilgileri",
        headers={"WWW-Authenticate": "Bearer"},
    )
    try:
        payload = jwt.decode(token, SECRET_KEY, algorithms=[ALGORITHM])
        kullanici_adi = payload.get("sub")
        if kullanici_adi is None:
            raise istisna
    except JWTError:
        raise istisna

    kullanici = fake_db.get(kullanici_adi)
    if kullanici is None:
        raise istisna
    return kullanici

# Endpoints
@app.post("/token", response_model=Token)
async def giris(form: OAuth2PasswordRequestForm = Depends()):
    kullanici = dogrula_kullanici(form.username, form.password)
    if not kullanici:
        raise HTTPException(
            status_code=status.HTTP_401_UNAUTHORIZED,
            detail="Hatalı kullanıcı adı veya şifre",
            headers={"WWW-Authenticate": "Bearer"},
        )
    access_token = token_olustur(
        veri={"sub": kullanici["kullanici_adi"]},
        expires_delta=timedelta(minutes=ACCESS_TOKEN_EXPIRE_MINUTES),
    )
    return Token(access_token=access_token, token_type="bearer")

@app.get("/kullanicilar/ben")
async def profilim(kullanici: dict = Depends(aktif_kullanici)):
    return kullanici

@app.get("/admin/panel")
async def admin_panel(kullanici: dict = Depends(aktif_kullanici)):
    if kullanici["rol"] != "admin":
        raise HTTPException(status_code=403, detail="Admin yetkisi gerekli")
    return {"mesaj": "Admin paneline hoş geldiniz"}
```

### Background Tasks

```python
from fastapi import FastAPI, BackgroundTasks

app = FastAPI()

def posta_gonder(email: str, mesaj: str):
    # Uzun süren işlem (e-posta gönderme)
    import time
    time.sleep(5)
    print(f"E-posta gönderildi: {email} -> {mesaj}")

def log_yaz(islem: str, detay: str):
    with open("islemler.log", "a") as f:
        f.write(f"{islem}: {detay}\n")

@app.post("/kayit")
async def kayit_ol(
    email: str,
    background_tasks: BackgroundTasks,
):
    # Kullanıcıyı kaydet
    # ...

    # Arka plan görevleri (yanıt beklemez)
    background_tasks.add_task(posta_gonder, email, "Hoş geldiniz!")
    background_tasks.add_task(log_yaz, "kayit", email)

    return {"mesaj": "Kayıt başarılı, e-posta gönderiliyor"}
```

### WebSocket Desteği

```python
from fastapi import FastAPI, WebSocket, WebSocketDisconnect
from typing import List

app = FastAPI()

class BaglantiYoneticisi:
    def __init__(self):
        self.baglantilar: List[WebSocket] = []

    async def baglan(self, websocket: WebSocket):
        await websocket.accept()
        self.baglantilar.append(websocket)

    def kes(self, websocket: WebSocket):
        self.baglantilar.remove(websocket)

    async def mesaj_gonder(self, mesaj: str, websocket: WebSocket):
        await websocket.send_text(mesaj)

    async def mesaj_gonder_herkese(self, mesaj: str):
        for baglanti in self.baglantilar:
            await baglanti.send_text(mesaj)

yonetici = BaglantiYoneticisi()

@app.websocket("/ws")
async def websocket_endpoint(websocket: WebSocket):
    await yonetici.baglan(websocket)
    try:
        while True:
            veri = await websocket.receive_text()
            await yonetici.mesaj_gonder_herkese(f"Yeni mesaj: {veri}")
    except WebSocketDisconnect:
        yonetici.kes(websocket)
        await yonetici.mesaj_gonder_herkese("Bir kullanıcı ayrıldı")
```

### Testing

```python
# pip install httpx pytest
from fastapi.testclient import TestClient
import pytest

app = FastAPI()

@app.get("/")
def index():
    return {"mesaj": "Merhaba"}

@app.post("/kullanicilar")
def kullanici_olustur(kullanici: dict):
    return {"id": 1, **kullanici}

client = TestClient(app)

def test_index():
    response = client.get("/")
    assert response.status_code == 200
    assert response.json() == {"mesaj": "Merhaba"}

def test_kullanici_olustur():
    response = client.post(
        "/kullanicilar",
        json={"kullanici_adi": "ali", "email": "ali@ornek.com"},
    )
    assert response.status_code == 200
    assert response.json()["kullanici_adi"] == "ali"

# Async test
@pytest.mark.asyncio
async def test_async():
    async with AsyncClient(app=app, base_url="http://test") as ac:
        response = await ac.get("/")
    assert response.status_code == 200
```

### Deployment

```bash
# Gunicorn + Uvicorn ile
pip install gunicorn uvicorn

# Worker sayısı: CPU çekirdeği * 2 + 1
gunicorn -w 4 -k uvicorn.workers.UvicornWorker -b 0.0.0.0:8000 main:app

# Uvicorn direkt (development)
uvicorn main:app --host 0.0.0.0 --port 8000 --workers 4
```

```dockerfile
# Dockerfile
FROM python:3.12-slim

WORKDIR /app

COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

EXPOSE 8000

CMD ["gunicorn", "-w", "4", "-k", "uvicorn.workers.UvicornWorker", "-b", "0.0.0.0:8000", "main:app"]
```

### Router ile Modüler Yapı

```python
# app.py
from fastapi import FastAPI

app = FastAPI()

from routers import kullanicilar, urunler, admin

app.include_router(kullanicilar.router, prefix="/kullanicilar", tags=["Kullanıcılar"])
app.include_router(urunler.router, prefix="/urunler", tags=["Ürünler"])
app.include_router(admin.router, prefix="/admin", tags=["Admin"])
```

```python
# routers/kullanicilar.py
from fastapi import APIRouter, Depends

router = APIRouter()

@router.get("/")
def kullanicilar_listele():
    return [{"id": 1, "ad": "Ali"}]

@router.get("/{kullanici_id}")
def kullanici_getir(kullanici_id: int):
    return {"id": kullanici_id, "ad": "Ali"}
```

### FastAPI Özet

- **Hız**: Node.js ve Go seviyesinde performans (Starlette + Pydantic)
- **Otomatik Dokümantasyon**: Swagger UI (/docs) ve ReDoc (/redoc)
- **Veri Doğrulama**: Pydantic ile request/response validasyonu
- **Async/Sync**: Her ikisini de destekler
- **Dependency Injection**: Güçlü ve temiz bağımlılık yönetimi
- **Tip Güvenliği**: IDE desteği ve otomatik tamamlama
- **GraphQL**: Strawberry veya Graphene ile entegre edilebilir

---

## <a id="django"></a>Django Web Framework:

Django, büyük ölçekli, "bataryalar dahil" (batteries-included) bir web framework'üdür. Admin panel, ORM, kimlik doğrulama, yönlendirme, şablon motoru ve daha fazlası kutudan çıkar.

### Django Mimarisi (MTV)

Django, Model-Template-View (MTV) mimarisini kullanır:

| Katman | Görev | Dosya |
|--------|-------|-------|
| **Model** | Veritabanı yapısı ve iş mantığı | `models.py` |
| **Template** | HTML şablonları ve sunum | `templates/` |
| **View** | HTTP isteklerini işleme ve yanıt üretme | `views.py` |

```
HTTP İsteği -> URL -> View -> Model (DB sorgusu) -> Template (render) -> HTTP Yanıtı
```

### Kurulum ve İlk Proje

```bash
pip install django

# Proje oluşturma
django-admin startproject mysite
cd mysite

# Uygulama oluşturma
python manage.py startapp blog

# Geliştirme sunucusu
python manage.py runserver
# http://127.0.0.1:8000
```

### Proje Dizini Yapısı

```
mysite/
├── manage.py                    # Yönetim komutları
├── mysite/                      # Proje ayarları (ana paket)
│   ├── __init__.py
│   ├── settings.py              # Proje yapılandırması
│   ├── urls.py                  # Ana URL yapılandırması
│   ├── asgi.py                  # ASGI giriş noktası
│   └── wsgi.py                  # WSGI giriş noktası
└── blog/                        # Uygulama
    ├── __init__.py
    ├── admin.py                 # Admin panel kaydı
    ├── apps.py                  # Uygulama yapılandırması
    ├── models.py                # Veritabanı modelleri
    ├── views.py                 # View fonksiyonları
    ├── urls.py                  # Uygulama URL'leri (kendin oluştur)
    ├── forms.py                 # Form tanımları
    ├── serializers.py           # DRF serializer'ları
    ├── signals.py               # Sinyal tanımları
    ├── templatetags/            # Özel template tag'leri
    │   ├── __init__.py
    │   └── blog_tags.py
    ├── management/              # Özel yönetim komutları
    │   ├── __init__.py
    │   └── commands/
    │       ├── __init__.py
    │       └── veri_aktar.py
    ├── migrations/              # Veritabanı migration'ları
    │   └── __init__.py
    ├── templates/               # HTML şablonları
    │   └── blog/
    │       ├── yazi_listele.html
    │       └── yazi_detay.html
    ├── static/                  # Statik dosyalar (CSS, JS, img)
    │   └── blog/
    │       └── style.css
    ├── fixtures/                # Test verileri
    └── tests.py                 # Testler
```

### Ayarlar (settings.py) Detaylı

```python
# mysite/settings.py
from pathlib import Path
import os

BASE_DIR = Path(__file__).resolve().parent.parent

# Güvenlik
SECRET_KEY = os.environ.get("DJANGO_SECRET_KEY", "geliştirme-anahtarı")
DEBUG = os.environ.get("DJANGO_DEBUG", "True").lower() == "true"
ALLOWED_HOSTS = os.environ.get("DJANGO_ALLOWED_HOSTS", "localhost,127.0.0.1").split(",")

# Uygulamalar
INSTALLED_APPS = [
    # Django varsayılan
    "django.contrib.admin",          # Admin paneli
    "django.contrib.auth",           # Kimlik doğrulama
    "django.contrib.contenttypes",   # İçerik tipleri (permissions için)
    "django.contrib.sessions",       # Oturum yönetimi
    "django.contrib.messages",       # Flash mesajlar
    "django.contrib.staticfiles",    # Statik dosya yönetimi

    # Üçüncü parti
    "rest_framework",               # DRF
    "crispy_forms",                 # Gelişmiş form görünümü
    "crispy_bootstrap5",
    "django_extensions",            # Geliştirme araçları
    "debug_toolbar",                # Debug araç çubuğu

    # Kendi uygulamaları
    "blog",
    "users",
]

# Middleware (istek/yanıt pipeline'ı)
MIDDLEWARE = [
    "django.middleware.security.SecurityMiddleware",
    "django.contrib.sessions.middleware.SessionMiddleware",
    "django.middleware.common.CommonMiddleware",
    "django.middleware.csrf.CsrfViewMiddleware",
    "django.contrib.auth.middleware.AuthenticationMiddleware",
    "django.contrib.messages.middleware.MessageMiddleware",
    "django.middleware.clickjacking.XFrameOptionsMiddleware",
    "debug_toolbar.middleware.DebugToolbarMiddleware",
]

# URL ve WSGI
ROOT_URLCONF = "mysite.urls"
WSGI_APPLICATION = "mysite.wsgi.application"

# Template'ler
TEMPLATES = [
    {
        "BACKEND": "django.template.backends.django.DjangoTemplates",
        "DIRS": [BASE_DIR / "templates"],  # global template klasörü
        "APP_DIRS": True,
        "OPTIONS": {
            "context_processors": [
                "django.template.context_processors.debug",
                "django.template.context_processors.request",
                "django.contrib.auth.context_processors.auth",
                "django.contrib.messages.context_processors.messages",
            ],
        },
    },
]

# Veritabanı
DATABASES = {
    "default": {
        "ENGINE": os.environ.get("DB_ENGINE", "django.db.backends.sqlite3"),
        "NAME": os.environ.get("DB_NAME", BASE_DIR / "db.sqlite3"),
        "USER": os.environ.get("DB_USER", ""),
        "PASSWORD": os.environ.get("DB_PASSWORD", ""),
        "HOST": os.environ.get("DB_HOST", ""),
        "PORT": os.environ.get("DB_PORT", ""),
    }
}

# PostgreSQL kullanımı:
# pip install psycopg2-binary
# DATABASES = {
#     "default": {
#         "ENGINE": "django.db.backends.postgresql",
#         "NAME": "mydb",
#         "USER": "myuser",
#         "PASSWORD": "mypass",
#         "HOST": "localhost",
#         "PORT": "5432",
#     }
# }

# Şifre doğrulama
AUTH_PASSWORD_VALIDATORS = [
    {"NAME": "django.contrib.auth.password_validation.UserAttributeSimilarityValidator"},
    {"NAME": "django.contrib.auth.password_validation.MinimumLengthValidator", "OPTIONS": {"min_length": 8}},
    {"NAME": "django.contrib.auth.password_validation.CommonPasswordValidator"},
    {"NAME": "django.contrib.auth.password_validation.NumericPasswordValidator"},
]

# Dil ve zaman
LANGUAGE_CODE = "tr"
TIME_ZONE = "Europe/Istanbul"
USE_I18N = True
USE_L10N = True
USE_TZ = True

# Statik dosyalar (CSS, JS, resim)
STATIC_URL = "static/"
STATICFILES_DIRS = [BASE_DIR / "static"]  # geliştirme
STATIC_ROOT = BASE_DIR / "staticfiles"    # production (collectstatic)

# Kullanıcı yüklemeleri (media)
MEDIA_URL = "media/"
MEDIA_ROOT = BASE_DIR / "media"

# Varsayılan primary key alanı
DEFAULT_AUTO_FIELD = "django.db.models.BigAutoField"

# Login/logout yönlendirmeleri
LOGIN_URL = "giris"
LOGIN_REDIRECT_URL = "anasayfa"
LOGOUT_REDIRECT_URL = "anasayfa"

# Email (development console)
EMAIL_BACKEND = "django.core.mail.backends.console.EmailBackend"
# EMAIL_BACKEND = "django.core.mail.backends.smtp.EmailBackend"
# EMAIL_HOST = "smtp.gmail.com"
# EMAIL_PORT = 587
# EMAIL_USE_TLS = True
# EMAIL_HOST_USER = os.environ.get("EMAIL_USER")
# EMAIL_HOST_PASSWORD = os.environ.get("EMAIL_PASS")
```

### Modeller (Veritabanı) Detaylı

```python
# blog/models.py
from django.db import models
from django.contrib.auth.models import User
from django.utils import timezone
from django.urls import reverse
from datetime import timedelta
from PIL import Image
import os

# choices sabitleri
class YaziDurum(models.TextChoices):
    TASLAK = "taslak", "Taslak"
    YAYINDA = "yayinda", "Yayında"
    ARSIV = "arsiv", "Arşiv"

# ----- Model Tanımları -----

class ZamanDamgali(models.Model):
    """Soyut model: yaratma ve güncelleme zamanını otomatik ekler."""
    yaratilma_tarihi = models.DateTimeField(auto_now_add=True)
    guncellenme_tarihi = models.DateTimeField(auto_now=True)

    class Meta:
        abstract = True  # veritabanında tablo oluşturMAZ

    def yayinda_gun(self):
        return (timezone.now() - self.yaratilma_tarihi).days

class Kategori(models.Model):
    ad = models.CharField(max_length=100, verbose_name="Kategori Adı")
    slug = models.SlugField(unique=True, max_length=120,
                            help_text="URL'de kullanılacak kısa isim")
    aciklama = models.TextField(blank=True, verbose_name="Açıklama")
    sira = models.PositiveSmallIntegerField(default=0, verbose_name="Sıralama")
    aktif = models.BooleanField(default=True)

    class Meta:
        verbose_name = "Kategori"
        verbose_name_plural = "Kategoriler"
        ordering = ["sira", "ad"]

    def __str__(self):
        return self.ad

    def yazi_sayisi(self):
        return self.yazilar.count()

class Etiket(models.Model):
    ad = models.CharField(max_length=50)
    slug = models.SlugField(unique=True)

    class Meta:
        verbose_name_plural = "Etiketler"

    def __str__(self):
        return self.ad

class Yazi(ZamanDamgali):
    """Blog yazısı modeli."""
    baslik = models.CharField(max_length=200, verbose_name="Başlık")
    slug = models.SlugField(unique=True, max_length=250)
    icerik = models.TextField(verbose_name="İçerik")
    ozet = models.TextField(max_length=500, blank=True, verbose_name="Özet")
    durum = models.CharField(
        max_length=10,
        choices=YaziDurum.choices,
        default=YaziDurum.TASLAK,
        verbose_name="Durum",
    )
    yazar = models.ForeignKey(
        User,
        on_delete=models.CASCADE,
        related_name="yazilar",
        verbose_name="Yazar",
    )
    kategori = models.ForeignKey(
        Kategori,
        on_delete=models.SET_NULL,
        null=True,
        blank=True,
        related_name="yazilar",
        verbose_name="Kategori",
    )
    etiketler = models.ManyToManyField(
        Etiket,
        blank=True,
        related_name="yazilar",
        verbose_name="Etiketler",
    )
    kapak_resmi = models.ImageField(
        upload_to="blog/kapak/",
        blank=True,
        null=True,
        verbose_name="Kapak Resmi",
    )
    goruntulenme = models.PositiveIntegerField(default=0, editable=False)
    yayin_tarihi = models.DateTimeField(default=timezone.now, verbose_name="Yayın Tarihi")
    aktif = models.BooleanField(default=True, db_index=True)

    class Meta:
        verbose_name = "Yazı"
        verbose_name_plural = "Yazılar"
        ordering = ["-yayin_tarihi", "-yaratilma_tarihi"]
        indexes = [
            models.Index(fields=["slug", "aktif"]),
            models.Index(fields=["-yayin_tarihi"]),
        ]

    def __str__(self):
        return self.baslik

    def get_absolute_url(self):
        return reverse("blog:yazi_detay", kwargs={"slug": self.slug})

    @property
    def yayinda_mi(self):
        return self.durum == YaziDurum.YAYINDA and self.aktif

    @property
    def yorum_sayisi(self):
        return self.yorumlar.filter(aktif=True).count()

    def kayit_gorme(self):
        self.goruntulenme += 1
        Yazi.objects.filter(pk=self.pk).update(goruntulenme=F("goruntulenme") + 1)

    def save(self, *args, **kwargs):
        # Slug otomatik oluşturma (eğer boşsa)
        if not self.slug:
            from django.utils.text import slugify
            self.slug = slugify(self.baslik)
        # Kapak resmini yeniden boyutlandır
        if self.kapak_resmi:
            super().save(*args, **kwargs)
            img = Image.open(self.kapak_resmi.path)
            if img.height > 800 or img.width > 800:
                img.thumbnail((800, 800))
                img.save(self.kapak_resmi.path)
        else:
            super().save(*args, **kwargs)

class Yorum(ZamanDamgali):
    yazi = models.ForeignKey(Yazi, on_delete=models.CASCADE, related_name="yorumlar")
    yazar = models.ForeignKey(User, on_delete=models.CASCADE, related_name="yorumlar")
    icerik = models.TextField(verbose_name="Yorum")
    aktif = models.BooleanField(default=False)  # admin onayı

    class Meta:
        verbose_name_plural = "Yorumlar"
        ordering = ["yaratilma_tarihi"]

    def __str__(self):
        return f"{self.yazar.username}: {self.icerik[:30]}..."
```

#### Model Alan Tipleri (Field Types)

| Alan | Kullanım |
|------|----------|
| `CharField(max_length=100)` | Kısa metin |
| `TextField()` | Uzun metin |
| `IntegerField()` | Tam sayı |
| `PositiveIntegerField()` | Pozitif tam sayı |
| `FloatField()` | Ondalıklı sayı |
| `DecimalField(max_digits=5, decimal_places=2)` | Para gibi hassas sayılar |
| `BooleanField(default=False)` | True/False |
| `DateField()` | Tarih |
| `DateTimeField()` | Tarih + Saat |
| `TimeField()` | Saat |
| `DurationField()` | Zaman aralığı |
| `EmailField()` | E-posta (validasyonlu) |
| `URLField()` | URL |
| `SlugField()` | URL dostu metin |
| `FileField(upload_to="dosyalar/")` | Dosya yükleme |
| `ImageField(upload_to="resimler/")` | Resim yükleme (Pillow gerekli) |
| `JSONField()` | JSON verisi (Django 3.1+) |
| `UUIDField()` | UUID (benzersiz kimlik) |
| `ForeignKey(OtherModel, on_delete=models.CASCADE)` | Bire-çok ilişki |
| `ManyToManyField(OtherModel)` | Çok-çok ilişki |
| `OneToOneField(OtherModel, on_delete=models.CASCADE)` | Bire-bir ilişki |

#### on_delete Seçenekleri

| Seçenek | Davranış |
|---------|----------|
| `CASCADE` | İlişkili kayıt silindiğinde bunu da sil |
| `SET_NULL` | null yap (null=True gerekli) |
| `SET_DEFAULT` | varsayılan değere dön |
| `PROTECT` | Silmeyi engelle (ProtectedError) |
| `SET(...)` | belirtilen değer/fonksiyonu ata |
| `DO_NOTHING` | hiçbir şey yapma (DB hatasına yol açabilir) |

#### Meta Sınıfı Seçenekleri

```python
class Meta:
    db_table = "custom_table_name"        # tablo adı
    verbose_name = "Yazı"
    verbose_name_plural = "Yazılar"
    ordering = ["-yayin_tarihi"]          # varsayılan sıralama
    unique_together = ["baslik", "yazar"] # birlikte benzersiz
    constraints = [                       # veritabanı kısıtlamaları
        models.CheckConstraint(
            check=models.Q(goruntulenme__gte=0),
            name="pozitif_goruntulenme",
        )
    ]
    indexes = [                           # veritabanı indeksleri
        models.Index(fields=["slug", "aktif"]),
    ]
    permissions = [                       # özel izinler
        ("yazi_yayinla", "Yazı yayınlama izni"),
    ]
    abstract = True                       # soyut model
    managed = True                        # Django yönetsin mi?
```

### Migration Yönetimi

```bash
# Migration oluşturma
python manage.py makemigrations
python manage.py makemigrations blog          # sadece blog uygulaması için

# Migration uygulama
python manage.py migrate
python manage.py migrate blog 0001            # belirli bir migration'a git

# Migration SQL'ini görüntüleme
python manage.py sqlmigrate blog 0001

# Migration listesi
python manage.py showmigrations

# Migration geri alma
python manage.py migrate blog zero            # başa dön
python manage.py migrate blog 0002            # 0002'ye dön

# Boş migration (veri aktarımı için)
python manage.py makemigrations blog --empty

# Migration birleştirme (performans için)
python manage.py squashmigrations blog 0001 0005
```

```python
# Veri migration örneği (boş migration içine)
from django.db import migrations

def veri_aktar(apps, schema_editor):
    EskiModel = apps.get_model("eski_uygulama", "EskiModel")
    YeniModel = apps.get_model("yeni_uygulama", "YeniModel")
    for eski in EskiModel.objects.all():
        YeniModel.objects.create(
            ad=eski.ad,
            aciklama=eski.aciklama,
        )

class Migration(migrations.Migration):
    dependencies = [("blog", "0005_auto_...")]
    operations = [
        migrations.RunPython(veri_aktar),
    ]
```

### Admin Panel Gelişmiş

```python
# blog/admin.py
from django.contrib import admin
from django.utils.html import format_html
from django.urls import reverse
from .models import Yazi, Kategori, Etiket, Yorum

# ------ Inline (ilişkili kayıtları aynı sayfada gösterme) ------

class YorumInline(admin.TabularInline):  # veya StackedInline
    model = Yorum
    extra = 1
    fields = ["yazar", "icerik", "aktif"]
    readonly_fields = ["yaratilma_tarihi"]
    autocomplete_fields = ["yazar"]

class EtiketInline(admin.TabularInline):
    model = Yazi.etiketler.through
    extra = 1

# ------ Admin Sınıfları ------

@admin.register(Yazi)
class YaziAdmin(admin.ModelAdmin):
    # Liste görünümü
    list_display = [
        "baslik", "yazar", "kategori", "durum_etiketi",
        "goruntulenme", "yayin_tarihi", "aktif", "yorum_sayisi_goster",
    ]
    list_display_links = ["baslik"]
    list_editable = ["aktif"]  # direkt listeden düzenle
    list_filter = ["aktif", "durum", "kategori", "yayin_tarihi", "etiketler"]
    search_fields = ["baslik", "icerik", "yazar__username"]
    list_select_related = ["yazar", "kategori"]
    list_per_page = 25
    date_hierarchy = "yayin_tarihi"
    autocomplete_fields = ["yazar"]
    raw_id_fields = ["yazar"]
    prepopulated_fields = {"slug": ("baslik",)}

    # Detay sayfası düzeni
    fieldsets = [
        ("Genel Bilgiler", {
            "fields": ["baslik", "slug", "yazar"],
        }),
        ("İçerik", {
            "fields": ["icerik", "ozet"],
            "classes": ["wide"],
        }),
        ("Kategorizasyon", {
            "fields": ["kategori", "etiketler"],
            "classes": ["collapse"],  # daraltılabilir
        }),
        ("Durum ve İstatistik", {
            "fields": ["durum", "aktif", "goruntulenme", "yayin_tarihi"],
        }),
        ("Medya", {
            "fields": ["kapak_resmi"],
            "classes": ["collapse"],
        }),
    ]
    readonly_fields = ["goruntulenme", "yaratilma_tarihi", "guncellenme_tarihi",
                       "kapak_resmi_onizleme"]

    # Inline
    inlines = [YorumInline]

    # Actions (toplu işlemler)
    actions = ["yazi_yayinla", "yazi_taslak_yap", "yazi_sil"]

    @admin.display(description="Durum")
    def durum_etiketi(self, obj):
        renkler = {"taslak": "orange", "yayinda": "green", "arsiv": "gray"}
        return format_html(
            '<span style="color: {};">{}</span>',
            renkler.get(obj.durum, "black"),
            obj.get_durum_display(),
        )

    @admin.display(description="Yorum")
    def yorum_sayisi_goster(self, obj):
        return obj.yorum_sayisi

    @admin.display(description="Kapak")
    def kapak_resmi_onizleme(self, obj):
        if obj.kapak_resmi:
            return format_html(
                '<img src="{}" style="max-height: 100px;" />',
                obj.kapak_resmi.url,
            )
        return "-"

    @admin.action(description="Seçili yazıları yayınla")
    def yazi_yayinla(self, request, queryset):
        guncellenen = queryset.update(durum="yayinda", aktif=True)
        self.message_user(request, f"{guncellenen} yazı yayınlandı.")

    @admin.action(description="Seçili yazıları taslağa çek")
    def yazi_taslak_yap(self, request, queryset):
        guncellenen = queryset.update(durum="taslak")
        self.message_user(request, f"{guncellenen} yazı taslağa çekildi.")

    def get_queryset(self, request):
        return super().get_queryset(request).select_related("yazar", "kategori")

@admin.register(Kategori)
class KategoriAdmin(admin.ModelAdmin):
    list_display = ["ad", "slug", "sira", "aktif", "yazi_sayisi"]
    list_editable = ["sira", "aktif"]
    prepopulated_fields = {"slug": ("ad",)}
    search_fields = ["ad"]

# Kayıt için alternatif (decorator yerine):
# admin.site.register(Yazi, YaziAdmin)
```

### URL Yapılandırması

```python
# mysite/urls.py (ana URL yapılandırması)
from django.contrib import admin
from django.urls import path, include
from django.conf import settings
from django.conf.urls.static import static
from django.views.generic import TemplateView

urlpatterns = [
    path("admin/", admin.site.urls),
    path("", include("blog.urls", namespace="blog")),
    path("kullanici/", include("users.urls", namespace="users")),
    path("api/", include("blog.api_urls")),
]

# Media dosyaları (sadece development)
if settings.DEBUG:
    urlpatterns += static(settings.MEDIA_URL, document_root=settings.MEDIA_ROOT)
    urlpatterns += [
        path("__debug__/", include("debug_toolbar.urls")),
    ]
```

```python
# blog/urls.py
from django.urls import path
from . import views

app_name = "blog"

urlpatterns = [
    path("", views.YaziListView.as_view(), name="yazi_listele"),
    path("yazi/<slug:slug>/", views.yazi_detay, name="yazi_detay"),
    path("kategori/<slug:slug>/", views.KategoriView.as_view(), name="kategori_yazilari"),
    path("ara/", views.ara, name="ara"),
    path("yazi-ekle/", views.YaziCreateView.as_view(), name="yazi_olustur"),
    path("yazi/<slug:slug>/duzenle/", views.YaziUpdateView.as_view(), name="yazi_guncelle"),
    path("yazi/<slug:slug>/sil/", views.YaziDeleteView.as_view(), name="yazi_sil"),
]
```

#### URL Parametreleri ve Converter'lar

```python
from django.urls import path, register_converter

# Özel converter
class DortBasamakliYil:
    regex = "[0-9]{4}"

    def to_python(self, deger):
        return int(deger)

    def to_url(self, deger):
        return "%04d" % deger

register_converter(DortBasamakliYil, "yil")

# Yerleşik converter'lar
# str  : herhangi string (boşluk hariç)
# int  : pozitif tam sayı
# slug : slug formatı (harf+sayı+tire)
# uuid : UUID formatı
# path : tüm path'i alır (eğik çizgi dahil)
# yil  : özel converter (yukarıda tanımlandı)

urlpatterns = [
    path("arsiv/<yil:yil>/", views.yillik_arsiv, name="arsiv"),
    path("yazi/<int:yazi_id>/", views.yazi_id_ile, name="yazi_id"),
    path("dosya/<path:dosya_yolu>/", views.dosya_goster, name="dosya"),
]
```

### View'ler

#### Fonksiyon Bazlı View'ler (FBV)

```python
# blog/views.py
from django.shortcuts import render, get_object_or_404, redirect
from django.http import HttpResponse, JsonResponse, Http404
from django.core.paginator import Paginator, EmptyPage, PageNotAnInteger
from django.contrib.auth.decorators import login_required
from django.contrib.auth.mixins import LoginRequiredMixin
from django.contrib import messages
from django.db.models import Count, Q, F
from django.views.decorators.http import require_http_methods
from django.views.decorators.cache import cache_page
from django.utils import timezone
from .models import Yazi, Kategori
from .forms import YaziForm, YorumForm

# Basit view
def anasayfa(request):
    yazilar = Yazi.objects.aktif().select_related("yazar", "kategori")
    context = {"yazilar": yazilar}
    return render(request, "blog/anasayfa.html", context)

# Detay view
def yazi_detay(request, slug):
    yazi = get_object_or_404(
        Yazi.objects.select_related("yazar", "kategori"),
        slug=slug, aktif=True,
    )
    yazi.kayit_gorme()  # görüntülenme sayısını artır

    # Yorum formu
    if request.method == "POST" and request.user.is_authenticated:
        yorum_form = YorumForm(request.POST)
        if yorum_form.is_valid():
            yorum = yorum_form.save(commit=False)
            yorum.yazi = yazi
            yorum.yazar = request.user
            yorum.save()
            messages.success(request, "Yorumunuz eklendi! Onay bekliyor.")
            return redirect(yazi.get_absolute_url())
    else:
        yorum_form = YorumForm()

    # Önceki/sonraki yazı
    onceki = Yazi.objects.aktif().filter(yayin_tarihi__lt=yazi.yayin_tarihi).first()
    sonraki = Yazi.objects.aktif().filter(yayin_tarihi__gt=yazi.yayin_tarihi).last()

    context = {
        "yazi": yazi,
        "yorum_form": yorum_form,
        "onceki": onceki,
        "sonraki": sonraki,
    }
    return render(request, "blog/yazi_detay.html", context)

# Sayfalama
def yazi_listele(request):
    yazilar = Yazi.objects.aktif().select_related("yazar", "kategori")
    sayfalayici = Paginator(yazilar, 10)  # sayfa başına 10 kayıt
    sayfa = request.GET.get("sayfa", 1)

    try:
        yazilar_sayfali = sayfalayici.page(sayfa)
    except PageNotAnInteger:
        yazilar_sayfali = sayfalayici.page(1)
    except EmptyPage:
        yazilar_sayfali = sayfalayici.page(sayfalayici.num_pages)

    context = {
        "yazilar": yazilar_sayfali,
        "is_paginated": yazilar_sayfali.has_other_pages(),
    }
    return render(request, "blog/yazi_listele.html", context)

# Arama
def ara(request):
    sorgu = request.GET.get("q", "").strip()
    yazilar = Yazi.objects.aktif().filter(
        Q(baslik__icontains=sorgu) | Q(icerik__icontains=sorgu)
    ).select_related("yazar") if sorgu else Yazi.objects.none()
    return render(request, "blog/ara.html", {"yazilar": yazilar, "sorgu": sorgu})

# CRUD - Oluşturma
@login_required
def yazi_olustur(request):
    if request.method == "POST":
        form = YaziForm(request.POST, request.FILES)
        if form.is_valid():
            yazi = form.save(commit=False)
            yazi.yazar = request.user
            yazi.save()
            form.save_m2m()  # ManyToMany alanları için
            messages.success(request, "Yazı başarıyla oluşturuldu!")
            return redirect("blog:yazi_detay", slug=yazi.slug)
    else:
        form = YaziForm()
    return render(request, "blog/yazi_form.html", {"form": form, "baslik": "Yeni Yazı"})

# CRUD - Güncelleme
@login_required
def yazi_guncelle(request, slug):
    yazi = get_object_or_404(Yazi, slug=slug, yazar=request.user)
    if request.method == "POST":
        form = YaziForm(request.POST, request.FILES, instance=yazi)
        if form.is_valid():
            yazi = form.save()
            messages.success(request, "Yazı güncellendi!")
            return redirect("blog:yazi_detay", slug=yazi.slug)
    else:
        form = YaziForm(instance=yazi)
    return render(request, "blog/yazi_form.html", {"form": form, "baslik": "Yazı Düzenle"})

# CRUD - Silme
@login_required
@require_http_methods(["POST"])
def yazi_sil(request, slug):
    yazi = get_object_or_404(Yazi, slug=slug, yazar=request.user)
    yazi.delete()
    messages.success(request, "Yazı silindi!")
    return redirect("blog:anasayfa")

# JSON yanıtı
def yazi_json(request, yazi_id):
    yazi = get_object_or_404(Yazi, id=yazi_id)
    return JsonResponse({
        "baslik": yazi.baslik,
        "icerik": yazi.icerik[:100],
        "yazar": yazi.yazar.username,
    })

# Decorator sıralaması (yukarıdan aşağı uygulanır)
@require_http_methods(["GET"])
@cache_page(60 * 15)  # 15 dakika önbellek
def populer_yazilar(request):
    yazilar = Yazi.objects.aktif().annotate(
        yorum_sayisi=Count("yorumlar")
    ).filter(yorum_sayisi__gt=5)[:10]
    return render(request, "blog/populer.html", {"yazilar": yazilar})
```

#### Sınıf Bazlı View'ler (CBV - Class-Based Views)

```python
# blog/views.py (CBV)
from django.views.generic import (
    ListView, DetailView, CreateView, UpdateView,
    DeleteView, FormView, TemplateView, RedirectView,
)
from django.contrib.auth.mixins import (
    LoginRequiredMixin, UserPassesTestMixin, PermissionRequiredMixin,
)
from django.urls import reverse_lazy
from django.db.models import Count
from .models import Yazi, Kategori
from .forms import YaziForm, IletisimForm

# ----- Generic Display Views -----

class AnasayfaView(TemplateView):
    template_name = "blog/anasayfa.html"

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context["yazilar"] = Yazi.objects.aktif()[:5]
        context["kategoriler"] = Kategori.objects.annotate(
            yazi_sayisi=Count("yazilar")
        ).filter(aktif=True)
        return context

class YaziListView(ListView):
    model = Yazi
    template_name = "blog/yazi_listele.html"
    context_object_name = "yazilar"
    paginate_by = 10

    def get_queryset(self):
        return Yazi.objects.aktif().select_related("yazar", "kategori")

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context["baslik"] = "Tüm Yazılar"
        return context

class YaziDetayView(DetailView):
    model = Yazi
    template_name = "blog/yazi_detay.html"
    context_object_name = "yazi"
    slug_field = "slug"
    slug_url_kwarg = "slug"  # URL'deki parametre adı

    def get_queryset(self):
        return Yazi.objects.aktif().select_related("yazar", "kategori")

    def get_object(self):
        yazi = super().get_object()
        yazi.kayit_gorme()
        return yazi

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context["onceki"] = Yazi.objects.aktif().filter(
            yayin_tarihi__lt=self.object.yayin_tarihi
        ).first()
        context["sonraki"] = Yazi.objects.aktif().filter(
            yayin_tarihi__gt=self.object.yayin_tarihi
        ).last()
        return context

class KategoriView(ListView):
    template_name = "blog/kategori.html"
    context_object_name = "yazilar"
    paginate_by = 10

    def get_queryset(self):
        self.kategori = get_object_or_404(Kategori, slug=self.kwargs["slug"])
        return Yazi.objects.aktif().filter(kategori=self.kategori).select_related("yazar")

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context["kategori"] = self.kategori
        return context

# ----- Generic Edit Views -----

class YaziCreateView(LoginRequiredMixin, CreateView):
    model = Yazi
    form_class = YaziForm
    template_name = "blog/yazi_form.html"

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context["baslik"] = "Yeni Yazı"
        return context

    def form_valid(self, form):
        form.instance.yazar = self.request.user
        messages.success(self.request, "Yazı başarıyla oluşturuldu!")
        return super().form_valid(form)

    def get_success_url(self):
        return reverse_lazy("blog:yazi_detay", kwargs={"slug": self.object.slug})

class YaziUpdateView(LoginRequiredMixin, UserPassesTestMixin, UpdateView):
    model = Yazi
    form_class = YaziForm
    template_name = "blog/yazi_form.html"
    slug_field = "slug"

    def test_func(self):  # sadece yazar düzenleyebilsin
        return self.get_object().yazar == self.request.user

    def get_context_data(self, **kwargs):
        context = super().get_context_data(**kwargs)
        context["baslik"] = "Yazı Düzenle"
        return context

    def form_valid(self, form):
        messages.success(self.request, "Yazı güncellendi!")
        return super().form_valid(form)

class YaziDeleteView(LoginRequiredMixin, UserPassesTestMixin, DeleteView):
    model = Yazi
    template_name = "blog/yazi_sil.html"
    slug_field = "slug"
    success_url = reverse_lazy("blog:anasayfa")

    def test_func(self):
        return self.get_object().yazar == self.request.user

    def form_valid(self, form):
        messages.success(self.request, "Yazı silindi!")
        return super().form_valid(form)

# ----- FormView -----

class IletisimView(FormView):
    template_name = "blog/iletisim.html"
    form_class = IletisimForm
    success_url = reverse_lazy("blog:anasayfa")

    def form_valid(self, form):
        form.posta_gonder()  # form içinde e-posta gönderme
        messages.success(self.request, "Mesajınız gönderildi!")
        return super().form_valid(form)

# ----- RedirectView -----

class EskiYaziRedirectView(RedirectView):
    pattern_name = "blog:yazi_detay"
    permanent = True  # 301 redirect

    def get_redirect_url(self, *args, **kwargs):
        eski_id = kwargs.pop("eski_id")
        yazi = get_object_or_404(Yazi, eski_id=eski_id)
        kwargs["slug"] = yazi.slug
        return super().get_redirect_url(*args, **kwargs)
```

### Template'ler

```html
<!-- blog/templates/blog/base.html (ana şablon) -->
<!DOCTYPE html>
{% load static blog_tags %}
<html lang="tr">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>{% block title %}Blog{% endblock %}</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/css/bootstrap.min.css" rel="stylesheet">
    <link rel="stylesheet" href="{% static 'blog/style.css' %}">
</head>
<body>
    <nav class="navbar navbar-expand-lg navbar-dark bg-dark">
        <div class="container">
            <a class="navbar-brand" href="{% url 'blog:anasayfa' %}">Blog</a>
            <form class="d-flex" action="{% url 'blog:ara' %}" method="get">
                <input class="form-control me-2" type="search" name="q" placeholder="Ara...">
            </form>
            <div class="navbar-nav ms-auto">
                {% if user.is_authenticated %}
                    <a class="nav-link" href="{% url 'blog:yazi_olustur' %}">Yazı Ekle</a>
                    <a class="nav-link" href="{% url 'users:profil' %}">Profil</a>
                    <form method="post" action="{% url 'users:cikis' %}">
                        {% csrf_token %}
                        <button type="submit" class="nav-link btn btn-link">Çıkış</button>
                    </form>
                {% else %}
                    <a class="nav-link" href="{% url 'users:giris' %}">Giriş</a>
                    <a class="nav-link" href="{% url 'users:kayit' %}">Kayıt</a>
                {% endif %}
            </div>
        </div>
    </nav>

    <main class="container mt-4">
        {% if messages %}
            {% for mesaj in messages %}
                <div class="alert alert-{{ mesaj.tags }} alert-dismissible fade show">
                    {{ mesaj }}
                    <button type="button" class="btn-close" data-bs-dismiss="alert"></button>
                </div>
            {% endfor %}
        {% endif %}

        {% block content %}{% endblock %}
    </main>

    <footer class="bg-light text-center py-3 mt-5">
        <p>&copy; 2026 Blog Uygulaması</p>
    </footer>

    <script src="https://cdn.jsdelivr.net/npm/bootstrap@5.3.0/dist/js/bootstrap.bundle.min.js"></script>
</body>
</html>
```

```html
<!-- blog/templates/blog/yazi_listele.html -->
{% extends "blog/base.html" %}

{% block title %}Yazılar{% endblock %}

{% block content %}
    <h1>{% if kategori %}{{ kategori.ad }}{% else %}Tüm Yazılar{% endif %}</h1>

    {% for yazi in yazilar %}
        <article class="card mb-3">
            <div class="row g-0">
                {% if yazi.kapak_resmi %}
                    <div class="col-md-4">
                        <img src="{{ yazi.kapak_resmi.url }}"
                             class="img-fluid rounded-start" alt="{{ yazi.baslik }}">
                    </div>
                {% endif %}
                <div class="col-md-{% if yazi.kapak_resmi %}8{% else %}12{% endif %}">
                    <div class="card-body">
                        <h5 class="card-title">
                            <a href="{{ yazi.get_absolute_url }}">{{ yazi.baslik }}</a>
                        </h5>
                        <p class="card-text">{{ yazi.ozet|default:yazi.icerik|truncatewords:50 }}</p>
                        <p class="card-text">
                            <small class="text-muted">
                                {{ yazi.yazar.get_full_name|default:yazi.yazar.username }}
                                | {{ yazi.yayin_tarihi|date:"d F Y H:i" }}
                                | {{ yazi.goruntulenme }} görüntülenme
                                | {{ yazi.yorum_sayisi }} yorum
                            </small>
                        </p>
                        {% for etiket in yazi.etiketler.all %}
                            <span class="badge bg-secondary">{{ etiket.ad }}</span>
                        {% endfor %}
                    </div>
                </div>
            </div>
        </article>
    {% empty %}
        <div class="alert alert-info">Henüz yazı bulunmuyor.</div>
    {% endfor %}

    <!-- Sayfalama -->
    {% if is_paginated %}
        <nav>
            <ul class="pagination justify-content-center">
                {% if page_obj.has_previous %}
                    <li class="page-item">
                        <a class="page-link" href="?sayfa={{ page_obj.previous_page_number }}">Önceki</a>
                    </li>
                {% endif %}
                {% for i in page_obj.paginator.page_range %}
                    <li class="page-item {% if i == page_obj.number %}active{% endif %}">
                        <a class="page-link" href="?sayfa={{ i }}">{{ i }}</a>
                    </li>
                {% endfor %}
                {% if page_obj.has_next %}
                    <li class="page-item">
                        <a class="page-link" href="?sayfa={{ page_obj.next_page_number }}">Sonraki</a>
                    </li>
                {% endif %}
            </ul>
        </nav>
    {% endif %}
{% endblock %}
```

#### Template Filter'lar ve Tag'ler

```python
# blog/templatetags/blog_tags.py
from django import template
from django.utils.html import strip_tags
from django.utils.text import truncate_words
from ..models import Yazi, Kategori
import markdown
import re

register = template.Library()

# ----- Basit Filter -----

@register.filter(name="tarih_format")
def tarih_format(deger, format="d F Y"):
    return deger.strftime(format) if deger else ""

@register.filter
def okuma_suresi(icerik):
    """Metnin okuma süresini dakika cinsinden hesaplar."""
    kelime_sayisi = len(strip_tags(icerik).split())
    dakika = max(1, round(kelime_sayisi / 200))
    return f"{dakika} dk okuma"

@register.filter
def markdowna_cevir(icerik):
    """Markdown içeriğini HTML'e çevirir."""
    return markdown.markdown(icerik, extensions=["extra", "codehilite"])

# ----- Inclusion Tag (başka bir template render eder) -----

@register.inclusion_tag("blog/kategori_listesi.html")
def kategori_listesi():
    return {
        "kategoriler": Kategori.objects.annotate(
            yazi_sayisi=models.Count("yazilar")
        ).filter(aktif=True)[:10]
    }

# ----- Simple Tag (doğrudan string döndürür) -----

@register.simple_tag(name="aktif_yazi_sayisi")
def aktif_yazi_sayisi():
    return Yazi.objects.aktif().count()

# ----- Assignment Tag (değişkene atar) -----

@register.simple_tag
def son_yazilar(adet=5):
    return Yazi.objects.aktif()[:adet]

# ----- Custom Tag (iki aşamalı, parse ve render) -----

@register.tag(name="yorumla")
def yorumla_parse(parser, token):
    try:
        tag_name, yorum = token.split_contents()
    except ValueError:
        raise template.TemplateSyntaxError("Kullanım: {% yorumla 'yorum' %}")
    return YorumlaNode(yorum[1:-1])

class YorumlaNode(template.Node):
    def __init__(self, yorum):
        self.yorum = yorum

    def render(self, context):
        return f"<!-- {self.yorum} -->"
```

```html
<!-- Template içinde kullanım -->
{% load blog_tags %}

{% kategori_listesi %}

<p>Toplam {% aktif_yazi_sayisi %} yazı var.</p>

<p>{{ yazi.icerik|markdowna_cevir|safe }}</p>

<p>{{ yazi.icerik|okuma_suresi }}</p>

{% son_yazilar 3 as son_uc_yazi %}
{% for yazi in son_uc_yazi %}
    <li><a href="{{ yazi.get_absolute_url }}">{{ yazi.baslik }}</a></li>
{% endfor %}

{# Template tag built-in örnekleri #}
{% cycle 'bir' 'iki' 'uc' %}
{% if forloop.first %}İlk eleman{% endif %}
{% lorem 3 p %}  {# rastgele Lorem Ipsum #}
{% now "Y-m-d H:i" %}
{% url "blog:yazi_detay" slug=yazi.slug %}
{% widthratio 5 10 100 %}  {# 5/10*100 = 50 #}
```

### Form'lar

```python
# blog/forms.py
from django import forms
from django.core.exceptions import ValidationError
from django.contrib.auth.forms import UserCreationForm
from django.contrib.auth.models import User
from .models import Yazi, Yorum
import re

# ----- ModelForm (Model'den otomatik form) -----

class YaziForm(forms.ModelForm):
    class Meta:
        model = Yazi
        fields = ["baslik", "icerik", "ozet", "kategori", "etiketler",
                  "durum", "kapak_resmi", "yayin_tarihi"]
        widgets = {
            "icerik": forms.Textarea(attrs={
                "class": "form-control editor",
                "rows": 15,
                "placeholder": "İçeriğinizi buraya yazın...",
            }),
            "baslik": forms.TextInput(attrs={
                "class": "form-control",
                "placeholder": "Başlık girin",
            }),
            "ozet": forms.Textarea(attrs={
                "class": "form-control",
                "rows": 3,
            }),
            "yayin_tarihi": forms.DateTimeInput(attrs={
                "type": "datetime-local",
                "class": "form-control",
            }),
        }
        help_texts = {
            "baslik": "En az 5 karakter olmalıdır.",
        }
        error_messages = {
            "baslik": {
                "required": "Başlık alanı zorunludur.",
                "max_length": "Başlık en fazla 200 karakter olabilir.",
            },
        }

    # Özel alan ekleme
    ek_not = forms.CharField(
        required=False,
        widget=forms.Textarea(attrs={"rows": 2, "class": "form-control"}),
        label="Ek Not",
    )

    # Temizleme (clean) metotları
    def clean_baslik(self):
        baslik = self.cleaned_data.get("baslik")
        if len(baslik) < 5:
            raise ValidationError("Başlık en az 5 karakter olmalıdır.")
        return baslik

    def clean(self):
        cleaned_data = super().clean()
        baslik = cleaned_data.get("baslik")
        icerik = cleaned_data.get("icerik")
        if baslik and icerik and baslik.lower() in icerik.lower():
            raise ValidationError("Başlık içerik içinde geçmemelidir.")
        return cleaned_data

# ----- Normal Form -----

class YorumForm(forms.Form):
    icerik = forms.CharField(
        label="Yorumunuz",
        widget=forms.Textarea(attrs={
            "class": "form-control",
            "rows": 4,
            "placeholder": "Yorumunuzu yazın...",
        }),
        min_length=2,
        max_length=2000,
    )

    def clean_icerik(self):
        icerik = self.cleaned_data.get("icerik", "").strip()
        if not icerik:
            raise ValidationError("Yorum boş olamaz.")
        # Spam koruması
        if re.search(r"(https?://|www\.)", icerik, re.I):
            raise ValidationError("Yorumda link gönderilemez.")
        return icerik

class IletisimForm(forms.Form):
    ad = forms.CharField(max_length=100, widget=forms.TextInput(attrs={"class": "form-control"}))
    email = forms.EmailField(widget=forms.EmailInput(attrs={"class": "form-control"}))
    konu = forms.CharField(max_length=200, widget=forms.TextInput(attrs={"class": "form-control"}))
    mesaj = forms.CharField(widget=forms.Textarea(attrs={"class": "form-control", "rows": 5}))
    cc_kendin = forms.BooleanField(required=False, label="Kopyasını bana da gönder")

    def posta_gonder(self):
        # E-posta gönderme işlemi
        pass

# ----- Kullanıcı Formları -----

class KayitFormu(UserCreationForm):
    email = forms.EmailField(required=True)

    class Meta:
        model = User
        fields = ["username", "email", "password1", "password2"]

    def save(self, commit=True):
        user = super().save(commit=False)
        user.email = self.cleaned_data["email"]
        if commit:
            user.save()
        return user

class ProfilFormu(forms.ModelForm):
    class Meta:
        model = User
        fields = ["username", "email", "first_name", "last_name"]

# ----- Formset (birden çok form) -----

from django.forms import formset_factory, modelformset_factory

# Normal formset
YorumFormSet = formset_factory(YorumForm, extra=3, max_num=10)

# Model formset (birden çok model kaydı)
YaziFormSet = modelformset_factory(Yazi, fields=["baslik", "durum"], extra=5)

# Inline formset (ilişkili modeller)
from django.forms import inlineformset_factory
YorumInlineFormSet = inlineformset_factory(Yazi, Yorum, fields=["icerik"], extra=2)

def toplu_yorum(request, slug):
    yazi = get_object_or_404(Yazi, slug=slug)
    formset = YorumInlineFormSet(instance=yazi)

    if request.method == "POST":
        formset = YorumInlineFormSet(request.POST, instance=yazi)
        if formset.is_valid():
            formset.save()
            messages.success(request, "Yorumlar güncellendi!")
            return redirect("blog:yazi_detay", slug=slug)

    return render(request, "blog/yorum_formset.html", {"formset": formset, "yazi": yazi})
```

```html
<!-- blog/templates/blog/yazi_form.html -->
{% extends "blog/base.html" %}
{% load crispy_forms_tags %}

{% block title %}{{ baslik }}{% endblock %}

{% block content %}
<div class="row justify-content-center">
    <div class="col-md-8">
        <h1>{{ baslik }}</h1>
        <form method="post" enctype="multipart/form-data" novalidate>
            {% csrf_token %}

            {{ form|crispy }}

            {% if form.non_field_errors %}
                <div class="alert alert-danger">
                    {% for hata in form.non_field_errors %}
                        {{ hata }}
                    {% endfor %}
                </div>
            {% endif %}

            <button type="submit" class="btn btn-primary">Kaydet</button>
            <a href="{% url 'blog:anasayfa' %}" class="btn btn-secondary">İptal</a>
        </form>
    </div>
</div>
{% endblock %}
```

### Kullanıcı Yönetimi (Auth)

```python
# users/views.py
from django.shortcuts import render, redirect
from django.contrib.auth import login, logout, authenticate
from django.contrib.auth.views import LoginView, LogoutView
from django.contrib.auth.decorators import login_required
from django.contrib.auth.mixins import LoginRequiredMixin
from django.contrib import messages
from django.urls import reverse_lazy
from django.views.generic import CreateView, UpdateView, DetailView
from .forms import KayitFormu, ProfilFormu

# ----- Giriş (built-in LoginView ile) -----

class GirisView(LoginView):
    template_name = "users/giris.html"
    redirect_authenticated_user = True  # giriş yapmışsa ana sayfaya yönlendir

    def get_success_url(self):
        return reverse_lazy("blog:anasayfa")

    def form_invalid(self, form):
        messages.error(self.request, "Hatalı kullanıcı adı veya şifre!")
        return super().form_invalid(form)

# ----- Çıkış -----

def cikis_yap(request):
    logout(request)
    messages.info(request, "Başarıyla çıkış yaptınız.")
    return redirect("blog:anasayfa")

# ----- Kayıt -----

class KayitView(CreateView):
    form_class = KayitFormu
    template_name = "users/kayit.html"
    success_url = reverse_lazy("blog:anasayfa")

    def form_valid(self, form):
        yanit = super().form_valid(form)
        # Kayıttan sonra otomatik giriş yap
        login(self.request, self.object)
        messages.success(self.request, "Hoş geldiniz!")
        return yanit

# ----- Profil -----

@login_required
def profil_duzenle(request):
    if request.method == "POST":
        form = ProfilFormu(request.POST, instance=request.user)
        if form.is_valid():
            form.save()
            messages.success(request, "Profil güncellendi!")
            return redirect("users:profil")
    else:
        form = ProfilFormu(instance=request.user)
    return render(request, "users/profil.html", {"form": form})

# ----- Decorator ile yetkilendirme -----

from django.contrib.auth.decorators import permission_required

@login_required
@permission_required("blog.yazi_yayinla", raise_exception=True)
def yazi_onayla(request, slug):
    yazi = get_object_or_404(Yazi, slug=slug)
    yazi.aktif = True
    yazi.save()
    return redirect("blog:anasayfa")
```

```python
# users/urls.py
from django.urls import path
from django.contrib.auth.views import LogoutView
from . import views

app_name = "users"

urlpatterns = [
    path("giris/", views.GirisView.as_view(), name="giris"),
    path("cikis/", LogoutView.as_view(next_page="blog:anasayfa"), name="cikis"),
    path("kayit/", views.KayitView.as_view(), name="kayit"),
    path("profil/", views.profil_duzenle, name="profil"),
]
```

```html
<!-- users/templates/users/giris.html -->
{% extends "blog/base.html" %}
{% load crispy_forms_tags %}

{% block title %}Giriş Yap{% endblock %}

{% block content %}
<div class="row justify-content-center">
    <div class="col-md-6">
        <h1>Giriş Yap</h1>
        <form method="post">
            {% csrf_token %}
            {{ form|crispy }}
            <button type="submit" class="btn btn-primary">Giriş Yap</button>
        </form>
        <hr>
        <p>Hesabınız yok mu? <a href="{% url 'users:kayit' %}">Kayıt Ol</a></p>
    </div>
</div>
{% endblock %}
```

### Django ORM (Detaylı Sorgular)

```python
# blog/views.py (ORM örnekleri)
from django.db.models import (
    Count, Sum, Avg, Min, Max, Q, F, Value, Subquery, OuterRef,
    Case, When, CharField, IntegerField, BooleanField,
    Prefetch, Exists, Func,
)
from django.db.models.functions import (
    Lower, Upper, Length, Concat, ExtractYear, ExtractMonth,
    Now, TruncDate, TruncMonth,
)
from django.db.models.expressions import Window
from django.db.models.functions import Rank, RowNumber
from datetime import datetime, timedelta
from .models import Yazi, Kategori, Yorum, Etiket

# ----- Temel Sorgular -----

# Tüm kayıtlar
tumu = Yazi.objects.all()

# Filtreleme (exact, iexact, contains, icontains, in, gt, gte, lt, lte, range, startswith, isnull)
aktifler = Yazi.objects.filter(aktif=True)
bugunku = Yazi.objects.filter(yayin_tarihi__date=datetime.today().date())
son_7_gun = Yazi.objects.filter(yayin_tarihi__gte=timezone.now() - timedelta(days=7))
belli_kategoriler = Yazi.objects.filter(kategori__id__in=[1, 2, 3])
belirli_kullanicilar = Yazi.objects.filter(yazar__in=User.objects.filter(is_staff=True))
icinde_gecen = Yazi.objects.filter(icerik__icontains="python")
fiyat_araligi = Yazi.objects.filter(goruntulenme__range=(100, 1000))
null_olanlar = Yazi.objects.filter(kategori__isnull=True)

# Exclusion
haric = Yazi.objects.exclude(aktif=False)

# Zincirleme
sonuc = Yazi.objects.filter(aktif=True).exclude(durum="taslak").order_by("-yayin_tarihi")[:10]

# ----- İleri Filtreleme (Q Nesnesi) -----

# OR koşulu
sonuc = Yazi.objects.filter(
    Q(baslik__icontains="python") | Q(icerik__icontains="python")
)

# AND + OR karmaşık
sonuc = Yazi.objects.filter(
    Q(aktif=True) & (
        Q(baslik__icontains="django") | Q(etiketler__ad__in=["python", "web"])
    )
)

# Negatif Q
sonuc = Yazi.objects.filter(~Q(durum="arsiv"))

# ----- F Nesnesi (alanlar arası karşılaştırma) -----

# Görüntülenme yorum sayısından fazla olanlar
sonuc = Yazi.objects.filter(goruntulenme__gt=F("yorum_sayisi"))

# Tüm görüntülenmeleri 1 artır
Yazi.objects.update(goruntulenme=F("goruntulenme") + 1)

# F + aritmetik
sonuc = Yazi.objects.annotate(
    puan=F("goruntulenme") * 2 + Count("yorumlar")
).order_by("-puan")

# ----- Aggregation -----

# Sayısal değerler
toplam = Yazi.objects.aggregate(
    toplam_goruntulenme=Sum("goruntulenme"),
    ortalama_goruntulenme=Avg("goruntulenme"),
    en_cok_goruntulenen=Max("goruntulenme"),
    en_az_goruntulenen=Min("goruntulenme"),
    yazi_sayisi=Count("id"),
)

# ----- Annotation (her kayda ek alan) -----

# Her yazıya yorum sayısını ekle
yazilar = Yazi.objects.annotate(
    yorum_sayisi=Count("yorumlar", filter=Q(yorumlar__aktif=True)),
    son_yorum_tarihi=Max("yorumlar__yaratilma_tarihi"),
)

# Her kategoriye yazı sayısını ekle
kategoriler = Kategori.objects.annotate(
    yazi_sayisi=Count("yazilar", filter=Q(yazilar__aktif=True)),
    toplam_goruntulenme=Sum("yazilar__goruntulenme"),
).filter(yazi_sayisi__gt=0)

# Case/When ile koşullu annotation
yazilar = Yazi.objects.annotate(
    populerlik=Case(
        When(goruntulenme__gte=1000, then=Value("populer")),
        When(goruntulenme__gte=100, then=Value("normal")),
        default=Value("az_goruntulenen"),
        output_field=CharField(),
    ),
)

# ----- Subquery (alt sorgu) -----

from django.db.models import Subquery, OuterRef

# Her kategorideki en son yazı
son_yazilar = Yazi.objects.filter(
    kategori=OuterRef("pk")
).order_by("-yayin_tarihi")

kategoriler = Kategori.objects.annotate(
    son_yazi_adi=Subquery(son_yazilar.values("baslik")[:1]),
    son_yazi_tarihi=Subquery(son_yazilar.values("yayin_tarihi")[:1]),
)

# ----- Select Related (JOIN - ForeignKey/OneToOne) -----

# Tek sorguda ilişkili tabloları getir
yazilar = Yazi.objects.select_related("yazar", "kategori").all()
for yazi in yazilar:
    print(yazi.yazar.username)  # ek sorgu yok
    print(yazi.kategori.ad)     # ek sorgu yok

# ----- Prefetch Related (ManyToMany/Reverse FK) -----

# Prefetch: ayrı sorgu ile ilişkili kayıtları getir
kategoriler = Kategori.objects.prefetch_related(
    Prefetch("yazilar", queryset=Yazi.objects.filter(aktif=True)),
).all()

for kategori in kategoriler:
    for yazi in kategori.yazilar.all():  # ek sorgu yok
        print(yazi.baslik)

# Çoklu prefetch
yazilar = Yazi.objects.prefetch_related(
    "etiketler",
    "yorumlar",
    Prefetch("yorumlar", queryset=Yorum.objects.filter(aktif=True), to_attr="aktif_yorumlar"),
).all()

# ----- Exists (var mı?) -----

# Yorumu olan yazılar
from django.db.models import Exists, OuterRef
yorum_var = Yorum.objects.filter(yazi=OuterRef("pk"), aktif=True)
yazilar = Yazi.objects.annotate(yorumu_var_mi=Exists(yorum_var)).filter(yorumu_var_mi=True)

# ----- Window Functions (pencere fonksiyonları) -----

# Sıralama (rank)
yazilar = Yazi.objects.annotate(
    sirasi=Window(
        expression=Rank(),
        order_by=F("goruntulenme").desc(),
    )
).order_by("sirasi")

# ----- Raw SQL -----

yazilar = Yazi.objects.raw(
    "SELECT * FROM blog_yazi WHERE aktif = %s ORDER BY yayin_tarihi DESC LIMIT 10",
    [True],
)
```

### Middleware

```python
# blog/middleware.py
from django.utils import timezone
from django.shortcuts import redirect
from django.urls import reverse
import re

class ZiyaretciSayaci:
    """Her istekte ziyaretçi sayısını artırır."""
    def __init__(self, get_response):
        self.get_response = get_response

    def __call__(self, request):
        # İstek öncesi
        request.ziyaret_zamani = timezone.now()
        if hasattr(request, "session"):
            request.session["son_ziyaret"] = str(timezone.now())

        # Yanıt
        response = self.get_response(request)

        # İstek sonrası
        response["X-Ziyaret-Zamani"] = str(request.ziyaret_zamani)
        return response

class AdminIPKontrol:
    """Admin panelini belirli IP'lerle sınırla."""
    def __init__(self, get_response):
        self.get_response = get_response
        self.izinli_ip_listesi = ["127.0.0.1", "192.168.1."]

    def __call__(self, request):
        if request.path.startswith("/admin/"):
            ip = request.META.get("REMOTE_ADDR", "")
            if not any(ip.startswith(izinli) for izinli in self.izinli_ip_listesi):
                return redirect("blog:anasayfa")
        return self.get_response(request)

class BakimModu:
    """Bakım modunda tüm istekleri bakım sayfasına yönlendirir."""
    def __init__(self, get_response):
        self.get_response = get_response
        self.bakim_modu = False  # settings'den alınabilir

    def __call__(self, request):
        if self.bakim_modu and not request.path.startswith("/bakim/"):
            return redirect("/bakim/")
        return self.get_response(request)
```

```python
# mysite/settings.py
MIDDLEWARE += [
    "blog.middleware.ZiyaretciSayaci",
]
```

### Sinyaller (Signals)

```python
# blog/signals.py
from django.db.models.signals import (
    pre_save, post_save, pre_delete, post_delete, m2m_changed,
)
from django.dispatch import receiver
from django.core.mail import mail_admins
from django.utils.text import slugify
from django.contrib.auth.models import User
from .models import Yazi, Yorum
import logging

logger = logging.getLogger(__name__)

# ----- Yazi Sinyalleri -----

@receiver(pre_save, sender=Yazi)
def yazi_oncesi(sender, instance, **kwargs):
    """Yazı kaydedilmeden önce slug oluştur."""
    if not instance.slug:
        instance.slug = slugify(instance.baslik)
        # Benzersiz slug garantisi
        sayac = 1
        while Yazi.objects.filter(slug=instance.slug).exists():
            instance.slug = f"{slugify(instance.baslik)}-{sayac}"
            sayac += 1

@receiver(post_save, sender=Yazi)
def yazi_sonrasi(sender, instance, created, **kwargs):
    """Yeni yazı yayınlandığında adminlere e-posta gönder."""
    if created and instance.durum == "yayinda":
        subject = f"Yeni yazı: {instance.baslik}"
        message = f"{instance.yazar.username} yeni bir yazı yayınladı: {instance.baslik}"
        try:
            mail_admins(subject, message)
            logger.info(f"E-posta gönderildi: {subject}")
        except Exception as e:
            logger.error(f"E-posta hatası: {e}")

@receiver(post_delete, sender=Yazi)
def yazi_silindi(sender, instance, **kwargs):
    """Yazı silindiğinde logla."""
    logger.warning(f"Yazı silindi: {instance.baslik} (yazar: {instance.yazar})")

# ----- Yorum Sinyalleri -----

@receiver(post_save, sender=Yorum)
def yorum_sonrasi(sender, instance, created, **kwargs):
    """Yeni yorum eklendiğinde yazı sahibine bildirim."""
    if created and not instance.aktif:
        # Admin onayı gerekiyor
        logger.info(f"Yorum onay bekliyor: {instance.yazi.baslik} - {instance.yazar}")

# ----- Kullanıcı Sinyalleri -----

@receiver(post_save, sender=User)
def kullanici_olustu(sender, instance, created, **kwargs):
    """Yeni kullanıcı kaydolduğunda hoş geldin e-postası."""
    if created:
        logger.info(f"Yeni kullanıcı: {instance.username} ({instance.email})")
        # Hoş geldin e-postası gönderilebilir

# ----- ManyToMany Sinyali -----

@receiver(m2m_changed, sender=Yazi.etiketler.through)
def etiket_degisti(sender, instance, action, reverse, model, pk_set, **kwargs):
    """Etiket ilişkisi değiştiğinde."""
    if action == "post_add":
        logger.info(f"Etiketler eklendi: {instance.baslik}")
    elif action == "post_remove":
        logger.info(f"Etiketler çıkarıldı: {instance.baslik}")

# ----- Custom Signal -----

from django.dispatch import Signal

yazi_paylasildi = Signal()  # Özel sinyal tanımı

@receiver(yazi_paylasildi)
def yazi_paylasildi_handler(sender, **kwargs):
    yazi = kwargs.get("yazi")
    platform = kwargs.get("platform")
    logger.info(f"Yazı paylaşıldı: {yazi} -> {platform}")

# Kullanım: yazi_paylasildi.send(sender=Yazi, yazi=yazi, platform="twitter")
```

```python
# blog/apps.py
from django.apps import AppConfig

class BlogConfig(AppConfig):
    default_auto_field = "django.db.models.BigAutoField"
    name = "blog"

    def ready(self):
        import blog.signals  # sinyalleri kaydet
```

### Yönetim Komutları (Management Commands)

```python
# blog/management/commands/veri_aktar.py
from django.core.management.base import BaseCommand, CommandError
from django.utils import timezone
from blog.models import Yazi, Kategori
import csv
import json

class Command(BaseCommand):
    help = "CSV/JSON dosyasından veri aktarır"

    def add_arguments(self, parser):
        parser.add_argument("dosya", type=str, help="Veri dosyası yolu")
        parser.add_argument(
            "--format",
            type=str,
            choices=["csv", "json"],
            default="csv",
            help="Dosya formatı (varsayılan: csv)",
        )
        parser.add_argument(
            "--dry-run",
            action="store_true",
            help="Gerçekten kaydetmeden dene",
        )

    def handle(self, *args, **options):
        dosya_yolu = options["dosya"]
        dosya_formati = options["format"]
        dry_run = options["dry_run"]

        self.stdout.write(f"Dosya okunuyor: {dosya_yolu}")

        if dosya_formati == "csv":
            self._csv_aktar(dosya_yolu, dry_run)
        elif dosya_formati == "json":
            self._json_aktar(dosya_yolu, dry_run)

    def _csv_aktar(self, dosya_yolu, dry_run):
        with open(dosya_yolu, "r", encoding="utf-8") as f:
            okuyucu = csv.DictReader(f)
            for satir in okuyucu:
                kategori, _ = Kategori.objects.get_or_create(ad=satir["kategori"])
                yazi = Yazi(
                    baslik=satir["baslik"],
                    icerik=satir["icerik"],
                    kategori=kategori,
                )
                if not dry_run:
                    yazi.save()
                    self.stdout.write(f"  + {yazi.baslik}")
                else:
                    self.stdout.write(f"  [DRY] {satir['baslik']}")

    def _json_aktar(self, dosya_yolu, dry_run):
        with open(dosya_yolu, "r", encoding="utf-8") as f:
            veriler = json.load(f)
            for veri in veriler:
                kategori, _ = Kategori.objects.get_or_create(ad=veri["kategori"])
                yazi = Yazi(
                    baslik=veri["baslik"],
                    icerik=veri["icerik"],
                    kategori=kategori,
                )
                if not dry_run:
                    yazi.save()
                    self.stdout.write(self.style.SUCCESS(f"✓ {yazi.baslik}"))
                else:
                    self.stdout.write(f"  [DRY] {veri['baslik']}")

        self.stdout.write(self.style.SUCCESS(f"{len(veriler)} kayıt aktarıldı."))

# Kullanım:
# python manage.py veri_aktar veriler.csv
# python manage.py veri_aktar veriler.json --format json
# python manage.py veri_aktar veriler.csv --dry-run
```

```python
# blog/management/commands/temizle_oturum.py
from django.core.management.base import BaseCommand
from django.contrib.sessions.models import Session
from django.utils import timezone

class Command(BaseCommand):
    help = "Süresi dolmuş oturumları temizler"

    def handle(self, *args, **options):
        silinen = Session.objects.filter(expire_date__lt=timezone.now()).delete()
        self.stdout.write(
            self.style.SUCCESS(f"{silinen[0]} süresi dolmuş oturum temizlendi.")
        )
```

### Test

```python
# blog/tests.py
from django.test import TestCase, Client
from django.urls import reverse
from django.contrib.auth.models import User
from django.core.files.uploadedfile import SimpleUploadedFile
from .models import Yazi, Kategori
import json

class YaziModelTest(TestCase):
    @classmethod
    def setUpTestData(cls):
        """Tüm testlerden önce bir kere çalışır."""
        cls.kullanici = User.objects.create_user(
            username="testuser",
            password="testpass123",
        )
        cls.kategori = Kategori.objects.create(
            ad="Python",
            slug="python",
        )
        cls.yazi = Yazi.objects.create(
            baslik="Test Yazısı",
            slug="test-yazisi",
            icerik="Bu bir test içeriğidir.",
            yazar=cls.kullanici,
            kategori=cls.kategori,
            durum="yayinda",
        )

    def test_yazi_goruntulenme_baslangici(self):
        self.assertEqual(self.yazi.goruntulenme, 0)

    def test_yazi_str(self):
        self.assertEqual(str(self.yazi), "Test Yazısı")

    def test_yazi_absolute_url(self):
        self.assertEqual(
            self.yazi.get_absolute_url(),
            "/yazi/test-yazisi/",
        )

    def test_kayit_gorme(self):
        self.yazi.kayit_gorme()
        self.assertEqual(
            Yazi.objects.get(id=self.yazi.id).goruntulenme,
            1,
        )

    def test_yayinda_mi_property(self):
        self.assertTrue(self.yazi.yayinda_mi)

    def test_yorum_sayisi_property(self):
        self.assertEqual(self.yazi.yorum_sayisi, 0)

class YaziViewTest(TestCase):
    def setUp(self):
        self.client = Client()
        self.kullanici = User.objects.create_user(
            username="testuser",
            password="testpass123",
        )
        self.yazi = Yazi.objects.create(
            baslik="Test Yazısı",
            slug="test-yazisi",
            icerik="Test içeriği",
            yazar=self.kullanici,
            durum="yayinda",
            aktif=True,
        )

    def test_anasayfa_status(self):
        response = self.client.get("/")
        self.assertEqual(response.status_code, 200)

    def test_anasayfa_template(self):
        response = self.client.get("/")
        self.assertTemplateUsed(response, "blog/anasayfa.html")

    def test_yazi_detay(self):
        response = self.client.get(self.yazi.get_absolute_url())
        self.assertEqual(response.status_code, 200)
        self.assertContains(response, "Test Yazısı")
        self.assertContains(response, "Test içeriği")

    def test_404(self):
        response = self.client.get("/yazi/olmayan-yazi/")
        self.assertEqual(response.status_code, 404)

    def test_login_required_create(self):
        response = self.client.get(reverse("blog:yazi_olustur"))
        self.assertRedirects(
            response,
            f"{reverse('users:giris')}?next={reverse('blog:yazi_olustur')}",
        )

    def test_yazi_olustur(self):
        self.client.login(username="testuser", password="testpass123")
        response = self.client.post(
            reverse("blog:yazi_olustur"),
            {
                "baslik": "Yeni Yazı",
                "icerik": "İçerik burada",
                "durum": "yayinda",
            },
        )
        self.assertEqual(response.status_code, 302)  # redirect
        self.assertTrue(Yazi.objects.filter(baslik="Yeni Yazı").exists())

    def test_arama(self):
        response = self.client.get(reverse("blog:ara"), {"q": "Test"})
        self.assertEqual(response.status_code, 200)
        self.assertContains(response, "Test Yazısı")

class YaziAPITest(TestCase):
    def setUp(self):
        self.client = Client()
        self.kullanici = User.objects.create_user(
            username="testuser",
            password="testpass123",
        )
        Yazi.objects.create(
            baslik="API Test",
            slug="api-test",
            icerik="API içeriği",
            yazar=self.kullanici,
            durum="yayinda",
        )

    def test_json_yazi(self):
        yazi = Yazi.objects.first()
        response = self.client.get(f"/yazi/{yazi.id}/json/")
        self.assertEqual(response.status_code, 200)
        self.assertEqual(response.json()["baslik"], "API Test")
```

```python
# pytest-django ile test (pip install pytest-django)

# conftest.py
import pytest
from django.contrib.auth.models import User
from blog.models import Yazi, Kategori

@pytest.fixture
def kullanici():
    return User.objects.create_user(username="testuser", password="testpass123")

@pytest.fixture
def yazi(kullanici):
    return Yazi.objects.create(
        baslik="Pytest Yazısı",
        slug="pytest-yazisi",
        icerik="Pytest ile test ediliyor",
        yazar=kullanici,
        durum="yayinda",
        aktif=True,
    )

@pytest.mark.django_db
def test_yazi_goruntulenme(yazi):
    assert yazi.goruntulenme == 0

@pytest.mark.django_db
def test_yazi_listesi(client, yazi):
    response = client.get("/")
    assert response.status_code == 200
    assert "Pytest Yazısı" in str(response.content)
```

### Django REST Framework (DRF) Detaylı

```python
# pip install djangorestframework djangorestframework-simplejwt django-filter
# pip install drf-spectacular (OpenAPI dokümantasyonu)

from rest_framework import serializers, viewsets, generics, status, filters
from rest_framework.decorators import api_view, action, permission_classes
from rest_framework.permissions import (
    IsAuthenticated, IsAuthenticatedOrReadOnly, AllowAny, IsAdminUser,
    DjangoModelPermissions,
)
from rest_framework.response import Response
from rest_framework.pagination import PageNumberPagination
from rest_framework.authentication import (
    SessionAuthentication, BasicAuthentication, TokenAuthentication,
)
from rest_framework.throttling import UserRateThrottle, AnonRateThrottle
from django_filters.rest_framework import DjangoFilterBackend
from rest_framework.parsers import JSONParser, MultiPartParser, FormParser
from .models import Yazi, Kategori, Yorum

# ----- Serializer Türleri -----

# ModelSerializer (en yaygın)
class YaziListSerializer(serializers.ModelSerializer):
    yazar_adi = serializers.CharField(source="yazar.username", read_only=True)
    kategori_adi = serializers.CharField(source="kategori.ad", read_only=True)
    yorum_sayisi = serializers.IntegerField(read_only=True)

    class Meta:
        model = Yazi
        fields = [
            "id", "baslik", "slug", "ozet", "yazar_adi",
            "kategori_adi", "goruntulenme", "yayin_tarihi",
            "yorum_sayisi", "kapak_resmi",
        ]

class YaziDetaySerializer(serializers.ModelSerializer):
    yazar = serializers.PublicUserSerializer(read_only=True)
    kategori = serializers.SlugRelatedField(
        slug_field="ad",
        queryset=Kategori.objects.all(),
    )
    etiketler = serializers.SlugRelatedField(
        slug_field="ad",
        many=True,
        queryset=Etiket.objects.all(),
    )

    class Meta:
        model = Yazi
        fields = "__all__"
        read_only_fields = ["goruntulenme", "yaratilma_tarihi", "guncellenme_tarihi"]

    def validate_baslik(self, value):
        if len(value) < 5:
            raise serializers.ValidationError("Başlık en az 5 karakter olmalıdır.")
        return value

# Serializer (manuel)
class YorumSerializer(serializers.Serializer):
    id = serializers.IntegerField(read_only=True)
    icerik = serializers.CharField(max_length=2000)
    yazar_adi = serializers.CharField(source="yazar.username", read_only=True)
    tarih = serializers.DateTimeField(source="yaratilma_tarihi", read_only=True)

    def create(self, validated_data):
        return Yorum.objects.create(**validated_data)

    def update(self, instance, validated_data):
        instance.icerik = validated_data.get("icerik", instance.icerik)
        instance.save()
        return instance

# Nested Serializer
class KategoriSerializer(serializers.ModelSerializer):
    yazi_sayisi = serializers.SerializerMethodField()

    class Meta:
        model = Kategori
        fields = ["id", "ad", "slug", "yazi_sayisi"]

    def get_yazi_sayisi(self, obj):
        return obj.yazilar.count()

# ----- Pagination -----

class StandartPagination(PageNumberPagination):
    page_size = 20
    page_size_query_param = "adet"
    max_page_size = 100

# ----- ViewSet'ler -----

class YaziViewSet(viewsets.ModelViewSet):
    queryset = Yazi.objects.aktif().select_related("yazar", "kategori")
    serializer_class = YaziDetaySerializer
    permission_classes = [IsAuthenticatedOrReadOnly]
    pagination_class = StandartPagination
    filter_backends = [DjangoFilterBackend, filters.SearchFilter, filters.OrderingFilter]
    filterset_fields = ["kategori", "aktif", "durum"]
    search_fields = ["baslik", "icerik"]
    ordering_fields = ["yayin_tarihi", "goruntulenme", "baslik"]
    lookup_field = "slug"

    def get_serializer_class(self):
        if self.action == "list":
            return YaziListSerializer
        return YaziDetaySerializer

    def get_queryset(self):
        qs = super().get_queryset()
        # Filtreleme
        yazar = self.request.query_params.get("yazar")
        if yazar:
            qs = qs.filter(yazar__username=yazar)
        return qs

    def perform_create(self, serializer):
        serializer.save(yazar=self.request.user)

    # Özel action
    @action(detail=True, methods=["post"])
    def goruntulenme_artir(self, request, slug=None):
        yazi = self.get_object()
        yazi.kayit_gorme()
        return Response({"goruntulenme": yazi.goruntulenme})

    @action(detail=False, methods=["get"])
    def populer(self, request):
        yazilar = self.get_queryset().order_by("-goruntulenme")[:10]
        serializer = YaziListSerializer(yazilar, many=True)
        return Response(serializer.data)

# ----- Generic Views (ViewSet yerine) -----

class YaziListCreateView(generics.ListCreateAPIView):
    queryset = Yazi.objects.aktif()
    serializer_class = YaziListSerializer
    permission_classes = [IsAuthenticatedOrReadOnly]
    pagination_class = StandartPagination

    def perform_create(self, serializer):
        serializer.save(yazar=self.request.user)

class YaziRetrieveUpdateDestroyView(generics.RetrieveUpdateDestroyAPIView):
    queryset = Yazi.objects.all()
    serializer_class = YaziDetaySerializer
    permission_classes = [IsAuthenticatedOrReadOnly]
    lookup_field = "slug"

# ----- API View (decorator) -----

@api_view(["GET", "POST"])
@permission_classes([IsAuthenticatedOrReadOnly])
def yorum_listele_olustur(request, slug):
    yazi = get_object_or_404(Yazi, slug=slug)
    if request.method == "GET":
        yorumlar = Yorum.objects.filter(yazi=yazi, aktif=True)
        serializer = YorumSerializer(yorumlar, many=True)
        return Response(serializer.data)
    elif request.method == "POST":
        serializer = YorumSerializer(data=request.data)
        if serializer.is_valid():
            serializer.save(yazi=yazi, yazar=request.user)
            return Response(serializer.data, status=status.HTTP_201_CREATED)
        return Response(serializer.errors, status=status.HTTP_400_BAD_REQUEST)

# ----- Throttling (hız sınırlama) -----

class YorumThrottle(UserRateThrottle):
    scope = "yorum"

# settings.py:
# REST_FRAMEWORK = {
#     "DEFAULT_THROTTLE_CLASSES": [
#         "rest_framework.throttling.AnonRateThrottle",
#         "rest_framework.throttling.UserRateThrottle",
#     ],
#     "DEFAULT_THROTTLE_RATES": {
#         "anon": "100/hour",
#         "user": "1000/hour",
#         "yorum": "10/minute",
#     },
# }

# ----- Router -----

from rest_framework.routers import DefaultRouter

router = DefaultRouter()
router.register(r"yazilar", YaziViewSet)

urlpatterns = router.urls
# http://127.0.0.1:8000/yazilar/
# http://127.0.0.1:8000/yazilar/test-yazisi/
# http://127.0.0.1:8000/yazilar/test-yazisi/goruntulenme_artir/
# http://127.0.0.1:8000/yazilar/populer/
```

### Django Channels (WebSocket)

```python
# pip install channels channels-redis

# mysite/asgi.py
import os
from django.core.asgi import get_asgi_application
from channels.routing import ProtocolTypeRouter, URLRouter
from channels.auth import AuthMiddlewareStack
from blog import routing

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "mysite.settings")

application = ProtocolTypeRouter({
    "http": get_asgi_application(),
    "websocket": AuthMiddlewareStack(
        URLRouter(routing.websocket_urlpatterns)
    ),
})
```

```python
# blog/routing.py
from django.urls import path
from . import consumers

websocket_urlpatterns = [
    path("ws/yorum/<slug:slug>/", consumers.YorumConsumer.as_asgi()),
]
```

```python
# blog/consumers.py
import json
from channels.generic.websocket import AsyncWebsocketConsumer

class YorumConsumer(AsyncWebsocketConsumer):
    async def connect(self):
        self.slug = self.scope["url_route"]["kwargs"]["slug"]
        self.grup_adi = f"yazi_{self.slug}"

        await self.channel_layer.group_add(
            self.grup_adi,
            self.channel_name,
        )
        await self.accept()

    async def disconnect(self, close_code):
        await self.channel_layer.group_discard(
            self.grup_adi,
            self.channel_name,
        )

    async def receive(self, text_data):
        veri = json.loads(text_data)
        mesaj = veri["mesaj"]

        await self.channel_layer.group_send(
            self.grup_adi,
            {
                "type": "yorum_mesaji",
                "mesaj": mesaj,
                "kullanici": self.scope["user"].username,
            },
        )

    async def yorum_mesaji(self, event):
        await self.send(text_data=json.dumps({
            "mesaj": event["mesaj"],
            "kullanici": event["kullanici"],
        }))
```

### Popüler Django Paketleri

| Paket | Kullanım |
|-------|----------|
| `django-debug-toolbar` | Geliştirme araç çubuğu (SQL sorguları, performans) |
| `django-extensions` | Shell plus, model grafiği, runscript |
| `django-crispy-forms` | Bootstrap ile uyumlu formlar |
| `django-allauth` | Sosyal medya girişi (Google, GitHub, vs.) |
| `django-filter` | Gelişmiş filtreleme |
| `django-import-export` | CSV/Excel veri aktarımı |
| `django-celery-beat` | Zamanlanmış görevler |
| `django-cors-headers` | CORS desteği |
| `django-ckeditor` | Zengin metin editörü |
| `django-haystack` | Tam metin arama (Elasticsearch) |
| `django-cachalot` | ORM sorgu önbellekleme |
| `django-redis` | Redis önbellek ve session |
| `django-silk` | Profiling araçları |
| `django-health-check` | Sağlık kontrolü |
| `django-constance` | Dinamik ayar yönetimi |
| `whitenoise` | Statik dosya servisi (production) |
| `sentry-sdk` | Hata izleme |

### Önbellekleme (Caching)

```python
# mysite/settings.py
CACHES = {
    "default": {
        "BACKEND": "django.core.cache.backends.redis.RedisCache",
        "LOCATION": "redis://127.0.0.1:6379/1",
    }
}

# Veya dosya bazlı:
# CACHES = {
#     "default": {
#         "BACKEND": "django.core.cache.backends.filebased.FileBasedCache",
#         "LOCATION": BASE_DIR / "cache",
#     }
# }
```

```python
# Kullanım
from django.core.cache import cache
from django.views.decorators.cache import cache_page
from django.views.decorators.vary import vary_on_cookie

# View seviyesinde önbellek
@cache_page(60 * 15)  # 15 dakika
@vary_on_cookie
def yazi_listele(request):
    ...

# Template fragment cache
# {% load cache %}
# {% cache 600 populer_yazilar %}
#     {% for yazi in populer %}
#         ...
#     {% endfor %}
# {% endcache %}

# Programatik cache
def kategori_listesi():
    kategoriler = cache.get("kategori_listesi")
    if not kategoriler:
        kategoriler = Kategori.objects.annotate(yazi_sayisi=Count("yazilar"))
        cache.set("kategori_listesi", kategoriler, 3600)
    return kategoriler

# Low-level cache API
cache.set("anahtar", "deger", 300)  # 5 dakika
cache.get("anahtar")
cache.delete("anahtar")
cache.get_or_set("anahtar", "varsayilan", 300)
```

### Güvenlik

```python
# Django güvenlik özellikleri (varsayılan olarak aktif)

# 1. XSS Koruması
# Template'lerde {{ degisken }} otomatik escape edilir
# {{ icerik|safe }} ile escape kaldırılabilir (dikkatli kullanın)

# 2. CSRF Koruması
# Tüm POST formlarında {% csrf_token %} kullanılmalı

# 3. SQL Injection Koruması
# ORM parametreli sorgular kullanır, raw SQL'de dikkatli olun
# Yanlış: Yazi.objects.raw(f"SELECT * FROM ... WHERE id = {id}")
# Doğru:  Yazi.objects.raw("SELECT * FROM ... WHERE id = %s", [id])

# 4. Clickjacking Koruması
# X-Frame-Options: DENY (middleware ile)

# 5. SSL/HTTPS
SECURE_SSL_REDIRECT = True  # production'da
SESSION_COOKIE_SECURE = True
CSRF_COOKIE_SECURE = True

# 6. HSTS
SECURE_HSTS_SECONDS = 31536000  # 1 yıl
SECURE_HSTS_INCLUDE_SUBDOMAINS = True
SECURE_HSTS_PRELOAD = True

# 7. Content Security Policy
# pip install django-csp
# MIDDLEWARE += ["csp.middleware.CSPMiddleware"]

# 8. Rate Limiting
# pip install django-ratelimit
# from django_ratelimit.decorators import ratelimit
# @ratelimit(key="ip", rate="5/h", method="POST")
# def giris(request): ...
```

### Deployment

```bash
# Production hazırlıkları
python manage.py check --deploy  # güvenlik kontrolleri
python manage.py collectstatic   # statik dosyaları topla
python manage.py migrate         # migration'ları uygula
```

```python
# mysite/settings.py (production)
import os
from pathlib import Path

BASE_DIR = Path(__file__).resolve().parent.parent

SECRET_KEY = os.environ["DJANGO_SECRET_KEY"]
DEBUG = False
ALLOWED_HOSTS = os.environ.get("ALLOWED_HOSTS", "").split(",")

# PostgreSQL
DATABASES = {
    "default": {
        "ENGINE": "django.db.backends.postgresql",
        "NAME": os.environ["DB_NAME"],
        "USER": os.environ["DB_USER"],
        "PASSWORD": os.environ["DB_PASSWORD"],
        "HOST": os.environ.get("DB_HOST", "localhost"),
        "PORT": os.environ.get("DB_PORT", "5432"),
        "CONN_MAX_AGE": 600,  # connection pooling (10 dk)
        "OPTIONS": {
            "sslmode": "require",
        },
    }
}

# Statik dosyalar (Whitenoise ile)
STATIC_ROOT = BASE_DIR / "staticfiles"
STATICFILES_STORAGE = "whitenoise.storage.CompressedManifestStaticFilesStorage"
MIDDLEWARE.insert(1, "whitenoise.middleware.WhiteNoiseMiddleware")

# Güvenlik
SECURE_SSL_REDIRECT = True
SECURE_HSTS_SECONDS = 31536000
SECURE_HSTS_INCLUDE_SUBDOMAINS = True
SECURE_HSTS_PRELOAD = True
SESSION_COOKIE_SECURE = True
CSRF_COOKIE_SECURE = True
SECURE_CONTENT_TYPE_NOSNIFF = True
SECURE_BROWSER_XSS_FILTER = True

# Logging
LOGGING = {
    "version": 1,
    "disable_existing_loggers": False,
    "formatters": {
        "verbose": {
            "format": "{asctime} {levelname} {module} {message}",
            "style": "{",
        },
    },
    "handlers": {
        "console": {
            "class": "logging.StreamHandler",
            "formatter": "verbose",
        },
        "file": {
            "class": "logging.handlers.RotatingFileHandler",
            "filename": BASE_DIR / "logs" / "django.log",
            "maxBytes": 1024 * 1024 * 10,  # 10 MB
            "backupCount": 5,
            "formatter": "verbose",
        },
    },
    "root": {
        "level": "INFO",
        "handlers": ["console", "file"],
    },
}
```

```dockerfile
# Dockerfile
FROM python:3.12-slim AS builder

WORKDIR /app
COPY requirements.txt .
RUN pip install --no-cache-dir -r requirements.txt

COPY . .

RUN python manage.py collectstatic --noinput

FROM python:3.12-slim
WORKDIR /app
COPY --from=builder /app /app

EXPOSE 8000

CMD ["gunicorn", "mysite.wsgi:application", \
     "-w", "4", \
     "-b", "0.0.0.0:8000", \
     "--max-requests", "1000", \
     "--max-requests-jitter", "50", \
     "--timeout", "120"]
```

```nginx
# /etc/nginx/sites-available/mysite
server {
    listen 80;
    server_name siteadi.com;
    return 301 https://$server_name$request_uri;
}

server {
    listen 443 ssl;
    server_name siteadi.com;

    ssl_certificate /etc/ssl/certs/siteadi.crt;
    ssl_certificate_key /etc/ssl/private/siteadi.key;

    location /static/ {
        alias /app/staticfiles/;
        expires 365d;
        add_header Cache-Control "public, immutable";
    }

    location /media/ {
        alias /app/media/;
    }

    location / {
        proxy_pass http://127.0.0.1:8000;
        proxy_set_header Host $host;
        proxy_set_header X-Real-IP $remote_addr;
        proxy_set_header X-Forwarded-For $proxy_add_x_forwarded_for;
        proxy_set_header X-Forwarded-Proto $scheme;
    }
}
```

```yaml
# docker-compose.yml
version: "3.8"

services:
  db:
    image: postgres:16-alpine
    environment:
      POSTGRES_DB: mysite
      POSTGRES_USER: mysite_user
      POSTGRES_PASSWORD: ${DB_PASSWORD}
    volumes:
      - postgres_data:/var/lib/postgresql/data
    restart: always

  redis:
    image: redis:7-alpine
    restart: always

  web:
    build: .
    env_file: .env
    environment:
      DB_HOST: db
      REDIS_URL: redis://redis:6379/1
    depends_on:
      - db
      - redis
    volumes:
      - static_volume:/app/staticfiles
      - media_volume:/app/media
    restart: always

  nginx:
    image: nginx:alpine
    volumes:
      - ./nginx.conf:/etc/nginx/conf.d/default.conf
      - static_volume:/app/staticfiles:ro
      - media_volume:/app/media:ro
    ports:
      - "80:80"
      - "443:443"
    depends_on:
      - web
    restart: always

volumes:
  postgres_data:
  static_volume:
  media_volume:
```

### Django Proje Şablonu (Tam Örnek)

```
mysite/
├── .env                          # Çevre değişkenleri
├── .gitignore
├── Dockerfile
├── docker-compose.yml
├── nginx.conf
├── requirements.txt
├── manage.py
├── mysite/
│   ├── __init__.py
│   ├── settings.py               # Ana ayarlar
│   ├── settings_dev.py           # Geliştirme ayarları
│   ├── settings_prod.py          # Production ayarları
│   ├── urls.py
│   ├── wsgi.py
│   └── asgi.py
├── blog/
│   ├── __init__.py
│   ├── admin.py
│   ├── apps.py
│   ├── models.py
│   ├── views.py                  # FBV
│   ├── cbv_views.py              # CBV (opsiyonel)
│   ├── urls.py
│   ├── api_urls.py
│   ├── serializers.py
│   ├── forms.py
│   ├── signals.py
│   ├── middleware.py
│   ├── consumers.py              # WebSocket
│   ├── routing.py
│   ├── tests.py
│   ├── management/
│   │   └── commands/
│   └── templatetags/
├── users/
│   ├── __init__.py
│   ├── admin.py
│   ├── models.py
│   ├── views.py
│   ├── forms.py
│   ├── urls.py
│   └── templates/
│       └── users/
├── templates/
│   ├── base.html
│   ├── 404.html
│   ├── 500.html
│   └── blog/
│       ├── anasayfa.html
│       ├── yazi_listele.html
│       ├── yazi_detay.html
│       ├── yazi_form.html
│       └── kategori.html
├── static/
│   ├── css/
│   ├── js/
│   └── images/
├── media/                        # Kullanıcı yüklemeleri
├── staticfiles/                  # collectstatic çıktısı
├── logs/                         # Log dosyaları
└── fixtures/                     # Test verileri
```

### Celery ile Zamanlanmış Görevler

```python
# pip install celery redis

# mysite/celery.py
import os
from celery import Celery

os.environ.setdefault("DJANGO_SETTINGS_MODULE", "mysite.settings")
app = Celery("mysite")
app.config_from_object("django.conf:settings", namespace="CELERY")
app.autodiscover_tasks()
```

```python
# blog/tasks.py
from celery import shared_task
from django.core.mail import send_mail
from django.utils import timezone
from datetime import timedelta
from .models import Yazi

@shared_task
def haftalik_bulten_gonder():
    """Haftalık popüler yazıları e-posta ile gönder."""
    from datetime import timedelta
    son_hafta = timezone.now() - timedelta(days=7)
    populer = Yazi.objects.filter(
        yayin_tarihi__gte=son_hafta,
        aktif=True,
    ).order_by("-goruntulenme")[:5]
    # E-posta gönderme işlemi...
    return f"{len(populer)} yazı bulundu"

@shared_task
def eski_yazilari_arsivle():
    """1 yıldan eski yazıları arşivle."""
    sinir = timezone.now() - timedelta(days=365)
    guncellenen = Yazi.objects.filter(
        yayin_tarihi__lte=sinir,
        durum="yayinda",
    ).update(durum="arsiv")
    return f"{guncellenen} yazı arşivlendi"

@shared_task
def kayit_sonrasi_eposta(kullanici_id):
    from django.contrib.auth.models import User
    kullanici = User.objects.get(id=kullanici_id)
    send_mail(
        "Hoş Geldiniz!",
        f"Merhaba {kullanici.username}, aramıza hoş geldiniz!",
        "from@ornek.com",
        [kullanici.email],
        fail_silently=False,
    )
```

```python
# mysite/settings.py
CELERY_BROKER_URL = os.environ.get("REDIS_URL", "redis://localhost:6379/0")
CELERY_RESULT_BACKEND = os.environ.get("REDIS_URL", "redis://localhost:6379/0")
CELERY_ACCEPT_CONTENT = ["json"]
CELERY_TASK_SERIALIZER = "json"
CELERY_BEAT_SCHEDULE = {
    "haftalik-bulten": {
        "task": "blog.tasks.haftalik_bulten_gonder",
        "schedule": crontab(hour=9, minute=0, day_of_week=1),  # Pazartesi 09:00
    },
    "eski-yazilari-arsivle": {
        "task": "blog.tasks.eski_yazilari_arsivle",
        "schedule": crontab(hour=3, minute=0, day_of_month=1),  # Ayın 1'i 03:00
    },
}
```

### Django Özet

| Özellik | Açıklama |
|---------|----------|
| **MTV Mimarisi** | Model-Template-View, temiz ayrıştırma |
| **ORM** | Tam donanımlı veritabanı soyutlama katmanı |
| **Admin Panel** | Otomatik, özelleştirilebilir yönetim arayüzü |
| **Form Yönetimi** | Form oluşturma, doğrulama, render |
| **Auth Sistemi** | Kullanıcı, grup, izin, oturum yönetimi |
| **Template Motoru** | Zengin tag/filter sistemi, inheritance |
| **Middleware** | İstek/yanıt pipeline'ı |
| **Güvenlik** | XSS, CSRF, SQLi, clickjacking koruması |
| **DRF** | REST API framework'ü (ViewSet, Serializer) |
| **Channels** | WebSocket, real-time iletişim |
| **Celery** | Zamanlanmış görevler, background jobs |
| **Test** | unittest, pytest, TestClient |
| **Migration** | Versiyon kontrollü veritabanı yönetimi |
| **İ18n** | Çoklu dil desteği |
| **Önbellek** | Redis/Memcached entegrasyonu |

### İleri Django Kaynakları

- [Django Resmi Dokümantasyonu](https://docs.djangoproject.com/)
- [Django Girls Tutorial](https://tutorial.djangogirls.org/) (başlangıç)
- [Classy Class-Based Views](https://ccbv.co.uk/) (CBV referans)
- [DRF Resmi Dokümantasyonu](https://www.django-rest-framework.org/)
- [TestDriven.io Django](https://testdriven.io/blog/topics/django/)
- [Django Packages](https://djangopackages.org/) (paket dizini)
- [Django Styleguide](https://github.com/HackSoftware/Django-Styleguide)

---

## <a id="subprocess"></a>43. subprocess ile Sistem Komutları:

`subprocess` modülü, Python içinden sistem komutları çalıştırmak, yeni işlemler başlatmak, girdi/çıktı borularıyla (pipe) etkileşim kurmak için kullanılır. Eski `os.system()` ve `os.popen()`'in yerini almıştır.

### run() ile Temel Kullanım

```python
import subprocess

# Basit komut çalıştırma (çıktıyı terminalde gösterir)
subprocess.run(["ls", "-la"])

# Çıktıyı yakalama
sonuc = subprocess.run(["echo", "Merhaba Dünya"], capture_output=True, text=True)
print(sonuc.stdout)   # Merhaba Dünya
print(sonuc.returncode)  # 0 (başarılı)

# Hata kontrolü
sonuc = subprocess.run(["ls", "olmayan_dosya"], capture_output=True, text=True)
print(sonuc.returncode)  # 2 (hata)
print(sonuc.stderr)      # hata mesajı

# Hata durumunda exception fırlatma
subprocess.run(["ls", "olmayan_dosya"], check=True)  # CalledProcessError

# Shell komutu çalıştırma (DİKKAT: güvenlik riski!)
subprocess.run("ls -la | grep py", shell=True)  # shell=True ile pipe çalışır
```

### run() Parametreleri

```python
import subprocess

# timeout: komutun maksimum çalışma süresi
try:
    subprocess.run(["sleep", "10"], timeout=5)
except subprocess.TimeoutExpired:
    print("Komut zaman aşımına uğradı!")

# cwd: çalışma dizini değiştirme
subprocess.run(["ls"], cwd="/tmp")

# env: ortam değişkenlerini belirleme
import os
benim_env = os.environ.copy()
benim_env["MY_VAR"] = "deger"
subprocess.run(["echo", "$MY_VAR"], env=benim_env, shell=True)

# input: stdin'den veri gönderme
sonuc = subprocess.run(["grep", "python"], input="python\njava\nc++\n", text=True, capture_output=True)
print(sonuc.stdout)  # python

# universal_newlines: text=True ile aynı
```

### Popen ile İleri Seviye

`Popen`, `run()`'dan daha esnektir. İşlem ile eşzamanlı etkileşim sağlar.

```python
import subprocess

# Komut başlatma
proc = subprocess.Popen(["ping", "-c", "4", "google.com"],
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                        text=True)

# Çıktıyı satır satır okuma (canlı)
for satir in proc.stdout:
    print(f"PING: {satir.strip()}")

# İşlemin bitmesini bekleme
proc.wait()
print(f"Çıkış kodu: {proc.returncode}")

# Pipe ile zincirleme (shell=True olmadan)
ps = subprocess.Popen(["ps", "aux"], stdout=subprocess.PIPE)
grep = subprocess.Popen(["grep", "python"], stdin=ps.stdout, stdout=subprocess.PIPE, text=True)
ps.stdout.close()
cikti = grep.communicate()[0]
print(cikti)
```

### communicate() ile Etkileşim

```python
import subprocess

proc = subprocess.Popen(["bc"], stdin=subprocess.PIPE,
                        stdout=subprocess.PIPE,
                        stderr=subprocess.PIPE,
                        text=True)

# Girdi gönder ve çıktıyı al
cikti, hata = proc.communicate(input="2 + 3\n4 * 5\nquit\n", timeout=5)
print(cikti)  # 5\n20
print(hata)   # None

# communicate() sonrası stdin/stdout/stderr kapatılır
```

### call(), check_call(), check_output()

```python
import subprocess

# call(): run()'ın basit versiyonu, returncode döndürür
rc = subprocess.call(["ls", "/tmp"])
print(rc)  # 0

# check_call(): hata varsa exception
subprocess.check_call(["true"])     # OK
# subprocess.check_call(["false"])  # CalledProcessError

# check_output(): sadece stdout döndürür
cikti = subprocess.check_output(["echo", "test"], text=True)
print(cikti)  # test
```

### DEVNULL ve Pipe Yönlendirme

```python
import subprocess

# Çıktıyı yok sayma
subprocess.run(["ls", "/tmp"], stdout=subprocess.DEVNULL, stderr=subprocess.DEVNULL)

# stdout'u stderr'e yönlendirme
sonuc = subprocess.run(["ls", "/tmp"], stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

# Dosyaya yazma
with open("cikti.txt", "w") as f:
    subprocess.run(["ls", "-la"], stdout=f)
```

## <a id="concurrent-futures"></a>44. concurrent.futures (Paralel İşlem):

`concurrent.futures` modülü, threading ve multiprocessing için yüksek seviyeli bir arayüz sağlar. ThreadPoolExecutor (I/O-bound) ve ProcessPoolExecutor (CPU-bound) olmak üzere iki tür havuz sunar.

### ThreadPoolExecutor

```python
from concurrent.futures import ThreadPoolExecutor, as_completed, wait
import time
import requests

# submit() ile teker teker gönderme
def url_getir(url):
    response = requests.get(url, timeout=10)
    return url, response.status_code, len(response.content)

urls = [
    "https://example.com",
    "https://httpbin.org/get",
    "https://google.com",
]

with ThreadPoolExecutor(max_workers=5) as havuz:
    # submit(): Future nesnesi döndürür
    futures = [havuz.submit(url_getir, url) for url in urls]

    # as_completed(): bitenleri sırayla işle
    for future in as_completed(futures):
        url, status, boyut = future.result()
        print(f"{url}: {status} ({boyut} bayt)")

    # wait(): tümünün bitmesini bekle
    # wait(futures, return_when=ALL_COMPLETED)
```

### map() ile Toplu İşlem

```python
from concurrent.futures import ThreadPoolExecutor
import time

def kare(x):
    time.sleep(0.1)
    return x ** 2

# map(): sıralı sonuç döndürür (submit sırasını korur)
with ThreadPoolExecutor(max_workers=4) as havuz:
    sonuclar = list(havuz.map(kare, range(10)))
    print(sonuclar)  # [0, 1, 4, 9, 16, 25, 36, 49, 64, 81]

# chunksize: büyük iterable'lar için performans
with ThreadPoolExecutor(max_workers=4) as havuz:
    sonuclar = list(havuz.map(kare, range(1000), chunksize=50))
```

### ProcessPoolExecutor (CPU-bound)

```python
from concurrent.futures import ProcessPoolExecutor
import time

def agir_islem(n):
    """CPU-bound işlem."""
    toplam = 0
    for i in range(n):
        toplam += i ** 2
    return toplam

if __name__ == "__main__":  # Windows'ta zorunlu!
    with ProcessPoolExecutor(max_workers=4) as havuz:
        # submit()
        future = havuz.submit(agir_islem, 10_000_000)
        print(future.result())

        # map()
        sonuclar = list(havuz.map(agir_islem, [1_000_000, 2_000_000, 3_000_000]))
        print(sonuclar)
```

### Future Nesnesi Detaylı

```python
from concurrent.futures import ThreadPoolExecutor, as_completed, wait, ALL_COMPLETED, FIRST_COMPLETED
import time

def yavas_islem(sure, isim):
    time.sleep(sure)
    return f"{isim} tamamlandı ({sure}s)"

with ThreadPoolExecutor(max_workers=3) as havuz:
    futures = {
        havuz.submit(yavas_islem, 3, "A"): "A",
        havuz.submit(yavas_islem, 1, "B"): "B",
        havuz.submit(yavas_islem, 2, "C"): "C",
    }

    # Future metotları
    for future in futures:
        print(f"Bitti mi: {future.done()}")    # False/True
        print(f"İptal edildi mi: {future.cancelled()}")  # False

    # İlk biteni bekle
    tamamlanan, _ = wait(futures, return_when=FIRST_COMPLETED)
    for f in tamamlanan:
        print(f.result())  # B tamamlandı (1s)

    # Tümünün bitmesini bekle
    tamamlanan, _ = wait(futures, return_when=ALL_COMPLETED)
    for f in tamamlanan:
        print(f.result())

    # Exception yakalama
    def hatali():
        raise ValueError("Hata!")

    future = havuz.submit(hatali)
    try:
        future.result()
    except ValueError as e:
        print(f"Future hatası: {e}")

    # Timeout ile bekleme
    try:
        sonuc = future.result(timeout=0.5)
    except TimeoutError:
        print("Future zaman aşımı!")
```

### Executor ile Hata Yönetimi

```python
from concurrent.futures import ThreadPoolExecutor, as_completed

def riskli_islem(x):
    if x == 5:
        raise ValueError(f"{x} değeri hatalı!")
    return x * 2

with ThreadPoolExecutor(max_workers=3) as havuz:
    futures = {havuz.submit(riskli_islem, i): i for i in range(10)}

    for future in as_completed(futures):
        i = futures[future]
        try:
            sonuc = future.result()
            print(f"{i}: {sonuc}")
        except ValueError as e:
            print(f"{i}: HATA - {e}")
```

### Ne Zaman Hangisi?

```python
# ThreadPoolExecutor: I/O-bound (dosya, ağ, veritabanı)
# - request, dosya okuma, veritabanı sorguları
# - GIL nedeniyle CPU işlemlerinde yavaş

# ProcessPoolExecutor: CPU-bound (hesaplama, dönüşüm)
# - GIL'i bypass eder, ayrı process'ler kullanır
# - Veri paylaşımı maliyetlidir (pickle serileştirme)
```

## <a id="hashlib-secrets-uuid"></a>45. hashlib, secrets ve uuid (Güvenlik):

### hashlib ile Hash İşlemleri

```python
import hashlib

# Hash nesnesi oluşturma
md5 = hashlib.md5()
sha256 = hashlib.sha256()
sha512 = hashlib.sha512()

# Veri ekleme (bytes)
md5.update(b"Merhaba Python")
print(md5.hexdigest())  # 32 karakter hex hash

# Direkt kullanım
hash_degeri = hashlib.sha256(b"Python").hexdigest()
print(hash_degeri)

# Büyük dosya hash'leme
def dosya_hash(dosya_adi, algoritma="sha256"):
    h = hashlib.new(algoritma)
    with open(dosya_adi, "rb") as f:
        for chunk in iter(lambda: f.read(8192), b""):
            h.update(chunk)
    return h.hexdigest()

print(dosya_hash("buyuk_dosya.zip"))

# Yaygın algoritmalar
print(hashlib.algorithms_guaranteed)  # {'sha256', 'sha512', 'md5', 'sha1', ...}
print(hashlib.algorithms_available)   # Sistemdeki tüm algoritmalar

# Farklı algoritmalar
for metin in [b"test", b"Test", b"TEST"]:
    print(f"{metin}: SHA256={hashlib.sha256(metin).hexdigest()[:16]}...")

# HMAC (mesaj doğrulama)
import hmac
anahtar = b"gizli-anahtar"
mesaj = b"onemli-mesaj"
hmac_hash = hmac.new(anahtar, mesaj, "sha256").hexdigest()
print(f"HMAC: {hmac_hash}")
```

### secrets ile Güvenli Rastgele Değerler

`secrets` modülü, kriptografik olarak güvenli rastgele sayılar üretir. Şifre, token, anahtar gibi güvenlik hassasiyeti olan durumlar için kullanılır.

```python
import secrets
import string

# Rastgele token (hex)
token = secrets.token_hex(16)  # 32 karakter hex
print(f"Token: {token}")

# Rastgele token (bytes)
token_bytes = secrets.token_bytes(32)

# URL güvenli token
url_token = secrets.token_urlsafe(32)
print(f"URL Token: {url_token}")

# Rastgele parola oluşturma
def parola_olustur(uzunluk=12):
    karakterler = string.ascii_letters + string.digits + "!@#$%^&*"
    return "".join(secrets.choice(karakterler) for _ in range(uzunluk))

print(f"Parola: {parola_olustur()}")

# Güvenli karşılaştırma (zamanlama saldırısına karşı)
kullanici_hash = "5e884898da28047151d0e56f8dc6292773603d0d6aabbdd62a11ef721d1542d8"
if secrets.compare_digest(hashlib.sha256(b"password").hexdigest(), kullanici_hash):
    print("Şifre doğru!")
else:
    print("Şifre yanlış!")

# Güvenli rastgele tam sayı
sifreleme_anahtari = secrets.randbelow(10**18)
print(f"Anahtar: {sifreleme_anahtari}")

# Belirli aralıkta rastgele
sayi = secrets.randbits(16)  # 0-65535 arası
```

### uuid ile Benzersiz Kimlikler

```python
import uuid

# UUID v1 (zaman tabanlı)
id1 = uuid.uuid1()
print(f"UUID v1: {id1}")

# UUID v4 (rastgele)
id4 = uuid.uuid4()
print(f"UUID v4: {id4}")

# UUID v5 (isim tabanlı + SHA-1)
id5 = uuid.uuid5(uuid.NAMESPACE_DNS, "example.com")
print(f"UUID v5: {id5}")

# UUID'den değer okuma
u = uuid.uuid4()
print(f"Hex: {u.hex}")           # 32 karakter hex
print(f"Bytes: {u.bytes}")       # 16 bytes
print(f"Int: {u.int}")           # tam sayı
print(f"Version: {u.version}")   # UUID versiyonu

# String'den UUID oluşturma
u = uuid.UUID("550e8400-e29b-41d4-a716-446655440000")
print(f"UUID: {u}")

# Veritabanında birincil anahtar olarak kullanım
class Kullanici:
    def __init__(self, ad):
        self.id = uuid.uuid4()
        self.ad = ad

kullanici = Kullanici("Ali")
print(f"Kullanıcı ID: {kullanici.id}")
```

## <a id="matematik-modulleri"></a>46. Matematik Modülleri (math, random, statistics):

### math Modülü

```python
import math

# ----- Sabitler -----
print(f"Pi: {math.pi}")           # 3.141592653589793
print(f"e: {math.e}")             # 2.718281828459045
print(f"Inf: {math.inf}")         # inf
print(f"NaN: {math.nan}")         # nan
print(f"Tau: {math.tau}")         # 6.283185307179586 (2*pi)

# ----- Yuvarlama -----
print(f"floor(3.7): {math.floor(3.7)}")   # 3 (aşağı yuvarla)
print(f"ceil(3.2): {math.ceil(3.2)}")     # 4 (yukarı yuvarla)
print(f"trunc(3.7): {math.trunc(3.7)}")   # 3 (kes)
print(f"round(3.5): {round(3.5)}")        # 4 (yerleşik, yuvarla)

# ----- Üstel ve Logaritma -----
print(f"exp(2): {math.exp(2)}")                # e^2
print(f"log(100): {math.log(100)}")            # ln(100)
print(f"log(100, 10): {math.log(100, 10)}")    # log10(100)
print(f"log2(8): {math.log2(8)}")             # 3.0
print(f"log10(100): {math.log10(100)}")        # 2.0
print(f"pow(2, 10): {math.pow(2, 10)}")        # 1024.0
print(f"sqrt(25): {math.sqrt(25)}")            # 5.0
print(f"isclose(0.1+0.2, 0.3): {math.isclose(0.1 + 0.2, 0.3)}")  # True

# ----- Trigonometri -----
print(f"sin(pi/2): {math.sin(math.pi/2)}")     # 1.0
print(f"cos(0): {math.cos(0)}")                # 1.0
print(f"tan(pi/4): {math.tan(math.pi/4)}")     # 0.999...
print(f"asin(1): {math.asin(1)}")              # pi/2
print(f"acos(1): {math.acos(1)}")              # 0
print(f"atan(1): {math.atan(1)}")              # pi/4
print(f"degrees(pi): {math.degrees(math.pi)}")   # 180.0
print(f"radians(180): {math.radians(180)}")      # pi

# ----- İstatistik -----
print(f"fsum([0.1]*10): {math.fsum([0.1]*10)}")  # 1.0 (hassas toplama)
print(f"prod([1,2,3,4]): {math.prod([1, 2, 3, 4])}")  # 24
print(f"factorial(5): {math.factorial(5)}")       # 120
print(f"comb(5,2): {math.comb(5, 2)}")            # 10 (kombinasyon)
print(f"perm(5,2): {math.perm(5, 2)}")            # 20 (permütasyon)

# ----- Sayı Teorisi -----
print(f"gcd(12, 18): {math.gcd(12, 18)}")        # 6
print(f"lcm(12, 18): {math.lcm(12, 18)}")        # 36 (Python 3.9+)
print(f"isfinite(float('inf')): {math.isfinite(float('inf'))}")  # False
print(f"isinf(float('inf')): {math.isinf(float('inf'))}")        # True
print(f"isnan(float('nan')): {math.isnan(float('nan'))}")        # True

# ----- İşaret ve Kopya -----
print(f"copysign(5, -3): {math.copysign(5, -3)}")  # -5.0
print(f"fabs(-5): {math.fabs(-5)}")                  # 5.0
```

### random Modülü

```python
import random

# ----- Temel Rastgele -----
print(f"random(): {random.random()}")          # [0.0, 1.0)
print(f"randint(1,10): {random.randint(1, 10)}")  # [1, 10]
print(f"uniform(5,10): {random.uniform(5, 10)}")  # [5.0, 10.0)

# ----- Dizi İşlemleri -----
meyveler = ["elma", "armut", "muz", "çilek", "kiraz"]
print(f"choice: {random.choice(meyveler)}")       # rastgele bir eleman
print(f"choices: {random.choices(meyveler, k=3)}")  # 3 eleman (tekrarlı)
print(f"sample: {random.sample(meyveler, k=2)}")    # 2 eleman (benzersiz)
random.shuffle(meyveler)  # karıştır (yerinde)
print(f"shuffle: {meyveler}")

# ----- Dağılımlar -----
print(f"gauss: {random.gauss(0, 1)}")        # normal dağılım (μ=0, σ=1)
print(f"expovariate: {random.expovariate(1)}")  # üstel dağılım
print(f"triangular: {random.triangular(0, 10, 5)}")  # üçgen dağılım

# ----- Seed (tekrarlanabilir rastgele) -----
random.seed(42)
print(f"Seed 1: {random.randint(1, 100)}")  # 81
random.seed(42)
print(f"Seed 2: {random.randint(1, 100)}")  # 81 (aynı)

# ----- Sistem Rastgele -----
sistem_random = random.SystemRandom()  # OS rastgele kaynağı (kripto)
print(f"Sistem: {sistem_random.randint(1, 100)}")

# ----- Pratik Kullanım: Şifre Üret -----
import string
def sifre_uret(uzunluk=8, buyuk=True, rakam=True, ozel=False):
    havuz = string.ascii_lowercase
    if buyuk: havuz += string.ascii_uppercase
    if rakam: havuz += string.digits
    if ozel: havuz += "!@#$%^&*"
    return "".join(random.choice(havuz) for _ in range(uzunluk))

print(f"Şifre: {sifre_uret(12, ozel=True)}")

# ----- Pratik: Yazı-Tura -----
def yazi_tura():
    return "Yazı" if random.random() < 0.5 else "Tura"

# ----- Pratik: Rastgele Renk -----
def rastgele_renk():
    return f"#{random.randint(0, 0xFFFFFF):06x}"
```

### statistics Modülü

```python
import statistics

veri = [1, 2, 3, 4, 5, 5, 6, 7, 8, 9, 10]

# ----- Merkezi Eğilim -----
print(f"mean: {statistics.mean(veri)}")              # 5.4545... (aritmetik ortalama)
print(f"median: {statistics.median(veri)}")           # 5 (ortanca)
print(f"median_low: {statistics.median_low(veri)}")   # 5 (alt ortanca)
print(f"median_high: {statistics.median_high(veri)}") # 6 (üst ortanca)
print(f"mode: {statistics.mode(veri)}")               # 5 (tepe değer)
print(f"multimode: {statistics.multimode(veri)}")     # [5] (tüm tepe değerler)

# ----- Yayılım -----
print(f"stdev: {statistics.stdev(veri):.2f}")         # 2.84 (standart sapma, örneklem)
print(f"pstdev: {statistics.pstdev(veri):.2f}")       # 2.70 (standart sapma, popülasyon)
print(f"variance: {statistics.variance(veri):.2f}")   # 8.07 (varyans, örneklem)
print(f"pvariance: {statistics.pvariance(veri):.2f}") # 7.29 (varyans, popülasyon)

# ----- Gruplu Veri -----
# Normal ortalama
print(f"mean: {statistics.mean([1, 2, 3])}")

# Harmonik ortalama (oranlar için)
print(f"harmonic_mean: {statistics.harmonic_mean([10, 20, 30]):.2f}")

# Geometrik ortalama (büyüme için)
print(f"geometric_mean: {statistics.geometric_mean([1, 2, 4]):.2f}")

# Quantiles (çeyreklikler)
veri2 = [1, 2, 3, 4, 5, 6, 7, 8, 9, 10]
print(f"quantiles: {statistics.quantiles(veri2, n=4)}")  # [3.25, 5.5, 7.75]

# Covariance ve Correlation
x = [1, 2, 3, 4, 5]
y = [2, 4, 6, 8, 10]
print(f"covariance: {statistics.covariance(x, y):.2f}")        # 5.0
print(f"correlation: {statistics.correlation(x, y):.2f}")       # 1.0 (mükemmel pozitif)
print(f"linear_regression: {statistics.linear_regression(x, y)}")
# LinregressResult(slope=2.0, intercept=0.0)
```

### decimal ve fractions

```python
from decimal import Decimal, getcontext, ROUND_HALF_UP
from fractions import Fraction

# ----- Decimal (hassas ondalık) -----
# Float sorunu:
print(0.1 + 0.2)  # 0.30000000000000004

# Decimal ile hassas işlem:
getcontext().prec = 28  # hassasiyet
a = Decimal("0.1")
b = Decimal("0.2")
print(a + b)  # 0.3 (doğru!)

# Para hesaplamaları için ideal
fiyat = Decimal("19.99")
kdv = fiyat * Decimal("0.18")
toplam = fiyat + kdv
print(f"Toplam: {toplam:.2f}")  # 23.59

# Yuvarlama
getcontext().rounding = ROUND_HALF_UP
print(Decimal("2.5").quantize(Decimal("1")))   # 3
print(Decimal("3.5").quantize(Decimal("1")))   # 4

# ----- Fractions (kesirler) -----
f1 = Fraction(1, 3)      # 1/3
f2 = Fraction(2, 5)      # 2/5
print(f1 + f2)           # 11/15
print(f1 * f2)           # 2/15
print(float(f1))         # 0.3333333333333333

# String'den kesir
f = Fraction("3/7")
print(f)                 # 3/7

# Kesir karşılaştırma
print(Fraction(1, 2) > Fraction(1, 3))  # True

# Limit denominator
from math import pi
print(Fraction(pi).limit_denominator(100))  # 311/99
```

## <a id="collections-modulu"></a>47. collections ile Gelişmiş Veri Yapıları:

### deque (Çift Uçlu Kuyruk)

```python
from collections import deque

# Oluşturma
d = deque([1, 2, 3])
d2 = deque(maxlen=5)  # maksimum 5 eleman (eski silinir)

# Ekleme (O(1) - listeden çok daha hızlı)
d.append(4)           # sağa ekle: [1, 2, 3, 4]
d.appendleft(0)       # sola ekle: [0, 1, 2, 3, 4]
d.extend([5, 6])      # sağdan çoklu ekle
d.extendleft([-2, -1])  # soldan çoklu ekle (ters sıra)

# Çıkarma (O(1))
sag = d.pop()          # sağdan çıkar: 6
sol = d.popleft()      # soldan çıkar: -2
print(f"Deque: {d}")

# Döndürme
d.rotate(2)            # 2 sağa döndür
print(f"Rotate(2): {d}")
d.rotate(-1)           # 1 sola döndür

# maxlen
d3 = deque(maxlen=3)
for i in range(5):
    d3.append(i)
    print(d3)  # [0], [0,1], [0,1,2], [1,2,3], [2,3,4]

# Pratik: Son N öğeyi tutma
def son_n_satir(dosya_adi, n=5):
    with open(dosya_adi) as f:
        return deque(f, maxlen=n)

# Pratik: Palindrome kontrolü
def palindrome(metin):
    d = deque(metin.lower().replace(" ", ""))
    while len(d) > 1:
        if d.popleft() != d.pop():
            return False
    return True

print(palindrome("Ada"))         # True
print(palindrome("Merhaba"))     # False
```

### ChainMap (Birden Çok Sözlük)

```python
from collections import ChainMap

# ChainMap: birden çok sözlüğü tek bir görünümde birleştirir
varsayilan = {"renk": "mavi", "boyut": "orta", "adet": 1}
kullanici = {"renk": "kırmızı", "adet": 5}
guncelleme = {"adet": 10}

# Öncelik sırası: soldaki kazanır (ilk bulunan)
birlesik = ChainMap(guncelleme, kullanici, varsayilan)

print(birlesik["renk"])  # kırmızı (kullanici'den)
print(birlesik["adet"])  # 10 (guncelleme'den)
print(birlesik["boyut"]) # orta (varsayilan'dan)

# Anahtarlar ve değerler
print(list(birlesik.keys()))    # ['renk', 'adet', 'boyut']
print(list(birlesik.values()))  # ['kırmızı', 10, 'orta']

# Yeni ekleme (ilk haritaya eklenir)
birlesik["yeni"] = "deger"
print(guncelleme)  # {'adet': 10, 'yeni': 'deger'}

# Pratik: Çevre değişkenleri + varsayılanlar
import os
import argparse

# Varsayılan yapılandırma
varsayilan_config = {
    "DEBUG": False,
    "PORT": 5000,
    "HOST": "localhost",
}

# Ortam değişkenleri + varsayılanlar
config = ChainMap(os.environ, varsayilan_config)
print(f"DEBUG: {config['DEBUG']}")
print(f"PORT: {config.get('PORT', 8080)}")

# Yeni bir ChainMap oluşturma (parent)
yeni = birlesik.new_child({"ek": "deger"})
print(yeni["ek"])  # deger
```

### OrderedDict (Sıralı Sözlük - Tarihi)

```python
from collections import OrderedDict

# Python 3.7+ normal dict de sıralı, ama OrderedDict ek metotlar sunar
od = OrderedDict()
od["a"] = 1
od["b"] = 2
od["c"] = 3

# Sondan eleman çıkarma (LIFO)
print(od.popitem())          # ('c', 3)
print(od.popitem(last=False))  # ('a', 1) (baştan)

# Anahtarı sona taşıma
od["b"] = 2
od.move_to_end("b")  # 'b' sona gider

# Anahtarı başa taşıma
od.move_to_end("b", last=False)  # 'b' başa gider

# OrderedDict vs dict: eşitlik karşılaştırması
d1 = {"a": 1, "b": 2}
d2 = {"b": 2, "a": 1}
print(d1 == d2)   # True (sıra önemsiz)

od1 = OrderedDict([("a", 1), ("b", 2)])
od2 = OrderedDict([("b", 2), ("a", 1)])
print(od1 == od2)  # False (sıra önemli)
```

### defaultdict (Detaylı)

```python
from collections import defaultdict

# Farklı varsayılan tiplerle kullanım
int_sayac = defaultdict(int)           # 0
liste_depo = defaultdict(list)         # []
set_depo = defaultdict(set)            # set()
float_sayac = defaultdict(float)       # 0.0
str_birlestir = defaultdict(str)       # ""

# İç içe defaultdict (ağaç yapısı)
ic_ice = defaultdict(lambda: defaultdict(int))
ic_ice["Ali"]["elma"] += 1
ic_ice["Veli"]["armut"] += 2
ic_ice["Ali"]["elma"] += 3
print(dict(ic_ice))
# {'Ali': defaultdict(<...>, {'elma': 4}), 'Veli': defaultdict(<...>, {'armut': 2})}

# Derin ağaç (sınırsız iç içe)
def derin_dict():
    return defaultdict(derin_dict)

agac = derin_dict()
agac["kategori"]["alt"]["urun"] = "deger"
print(agac["kategori"]["alt"]["urun"])  # deger
```

### Counter (Detaylı)

```python
from collections import Counter

# Oluşturma
c = Counter("merhaba dünya")
print(c)  # Counter({'a': 3, ' ': 1, 'm': 1, 'e': 1, 'r': 1, 'h': 1, 'b': 1, 'd': 1, 'ü': 1, 'n': 1, 'y': 1})

# En sık geçenler
print(c.most_common(3))  # [('a', 3), ('m', 1), ('e', 1)]

# Matematiksel işlemler
c1 = Counter(a=3, b=1, c=2)
c2 = Counter(a=1, b=2, d=3)
print(c1 + c2)  # Counter({'a': 4, 'b': 3, 'd': 3, 'c': 2})
print(c1 - c2)  # Counter({'a': 2, 'c': 2}) (negatifler atılır)
print(c1 & c2)  # Counter({'a': 1, 'b': 1}) (kesişim - min)
print(c1 | c2)  # Counter({'a': 3, 'd': 3, 'c': 2, 'b': 2}) (birleşim - max)

# Toplam eleman sayısı
print(f"Toplam: {sum(c.values())}")  # 14

# Sıfırla/negatifleri temizle
c = Counter(a=3, b=-1, c=0)
print(+c)  # Counter({'a': 3}) (pozitifleri al)
print(-c)  # Counter({'b': 1}) (negatifleri pozitif yap)

# update ve subtract
c = Counter(a=3, b=1)
c.update({"a": 2, "c": 5})  # ekle: a=5, b=1, c=5
c.subtract({"a": 1, "b": 2})  # çıkar: a=4, b=-1, c=5
```

### namedtuple (Detaylı)

```python
from collections import namedtuple

# Temel kullanım
Nokta = namedtuple("Nokta", ["x", "y"])
p = Nokta(3, 4)
print(p.x, p.y)    # 3 4
print(p[0])         # 3

# Varsayılan değerler
Kisi = namedtuple("Kisi", "ad yas meslek", defaults=["Bilinmiyor"])
k1 = Kisi("Ali", 30)
print(k1)  # Kisi(ad='Ali', yas=30, meslek='Bilinmiyor')

# Docstring
Kisi.__doc__ = "Kişi bilgileri"
Kisi.ad.__doc__ = "Kişinin adı"

# _fields, _field_defaults
print(Kisi._fields)          # ('ad', 'yas', 'meslek')
print(Kisi._field_defaults)  # {'meslek': 'Bilinmiyor'}

# Sözlükten oluşturma
veri = {"x": 10, "y": 20}
p = Nokta(**veri)

# namedtuple listesi sıralama
Ogrenci = namedtuple("Ogrenci", ["ad", "not_ort"])
ogrenciler = [
    Ogrenci("Ali", 85),
    Ogrenci("Veli", 92),
    Ogrenci("Ayşe", 78),
]
sirali = sorted(ogrenciler, key=lambda x: x.not_ort, reverse=True)
print([o.ad for o in sirali])  # ['Veli', 'Ali', 'Ayşe']
```

### UserList, UserDict, UserString

```python
from collections import UserList, UserDict, UserString

# Özelleştirilmiş liste
class SeyahatListesi(UserList):
    def __init__(self, baslangic=None):
        super().__init__(baslangic or [])

    def ekle(self, eleman):
        if eleman not in self.data:
            self.data.append(eleman)

    def topla(self):
        return "\n".join(f"□ {e}" for e in self.data)

liste = SeyahatListesi(["pasaport", "bilet"])
liste.ekle("valiz")
liste.ekle("pasaport")  # tekrar eklenmez (__contains__ kontrolü)
print(liste.topla())

# Özelleştirilmiş sözlük
class ErisimLogDict(UserDict):
    def __getitem__(self, key):
        print(f"Erişim: {key}")
        return super().__getitem__(key)

    def __setitem__(self, key, value):
        print(f"Atama: {key} = {value}")
        super().__setitem__(key, value)

d = ErisimLogDict()
d["isim"] = "Ali"    # Atama: isim = Ali
x = d["isim"]         # Erişim: isim
```

## <a id="functools-ileri"></a>48. functools İleri Düzey:

### partial (Kısmi Uygulama)

```python
from functools import partial

# Fonksiyonun bazı parametrelerini önceden belirleme
def us_al(taban, us):
    return taban ** us

kare = partial(us_al, us=2)
kup = partial(us_al, us=3)

print(kare(5))   # 25
print(kup(5))    # 125

# Birden çok parametre
def log_yaz(seviye, mesaj, dosya="app.log"):
    print(f"[{seviye.upper()}] {mesaj} -> {dosya}")

info = partial(log_yaz, "info")
hata = partial(log_yaz, "hata")

info("Uygulama başladı")       # [INFO] Uygulama başladı -> app.log
hata("Bağlantı hatası")        # [HATA] Bağlantı hatası -> app.log

# partial ile callback (ör: buton click)
def buton_tikla(buton_id, kullanici):
    print(f"Buton {buton_id} - {kullanici} tıkladı")

buton1 = partial(buton_tikla, 1)
buton1("Ali")  # Buton 1 - Ali tıkladı

# Pratik: Dosya okuma (encoding önceden belirle)
import json
json_oku = partial(json.load, encoding="utf-8")
```

### singledispatch (Tek Dağıtım)

```python
from functools import singledispatch

# singledispatch: argüman tipine göre farklı davranış
@singledispatch
def formatla(deger):
    """Varsayılan: string'e çevir."""
    return str(deger)

@formatla.register(int)
def _(deger):
    return f"Sayı: {deger:,} ({hex(deger)})"

@formatla.register(float)
def _(deger):
    return f"Ondalık: {deger:.2f} (bilimsel: {deger:.2e})"

@formatla.register(list)
def _(deger):
    return f"Liste [{len(deger)}]: " + ", ".join(str(x) for x in deger)

@formatla.register(dict)
def _(deger):
    return "Sözlük: " + ", ".join(f"{k}={v}" for k, v in deger.items())

@formatla.register(str)
@formatla.register(bytes)  # birden çok tip aynı anda
def _(deger):
    return f"Metin ({len(deger)} karakter): {deger[:50]}..."

@formatla.register(type(None))
def _(deger):
    return "Boş (None)"

# Kullanım
print(formatla(42))              # Sayı: 42 (0x2a)
print(formatla(3.14159))         # Ondalık: 3.14 (bilimsel: 3.14e+00)
print(formatla([1, 2, 3]))       # Liste [3]: 1, 2, 3
print(formatla({"a": 1, "b": 2})) # Sözlük: a=1, b=2
print(formatla("Merhaba"))       # Metin (7 karakter): Merhaba...
print(formatla(None))            # Boş (None)

# Özel sınıflar için
class Kullanici:
    def __init__(self, ad, yas):
        self.ad = ad
        self.yas = yas

@formatla.register(Kullanici)
def _(deger):
    return f"Kullanıcı: {deger.ad} ({deger.yas} yaş)"

k = Kullanici("Ali", 25)
print(formatla(k))  # Kullanıcı: Ali (25 yaş)

# Mevcut kayıtları görüntüleme
print(formatla.registry.keys())  # kayıtlı tipler
```

### cache ve lru_cache

```python
from functools import cache, lru_cache
import time

# @cache: Python 3.9+, sınırsız önbellek
@cache
def fibonacci(n):
    if n < 2:
        return n
    return fibonacci(n - 1) + fibonacci(n - 2)

basla = time.perf_counter()
print(fibonacci(100))  # 354224848179261915075
print(f"Cache ile: {time.perf_counter() - basla:.4f}s")

# @lru_cache: maksimum boyut sınırlamalı
@lru_cache(maxsize=128)
def agir_islem(n):
    time.sleep(0.1)  # simüle edilmiş ağır işlem
    return n ** 2

# İlk çağrı: yavaş
basla = time.perf_counter()
for i in range(5):
    agir_islem(i)
print(f"İlk: {time.perf_counter() - basla:.4f}s")  # ~0.5s

# İkinci çağrı: hızlı (cache'den)
basla = time.perf_counter()
for i in range(5):
    agir_islem(i)
print(f"Cache: {time.perf_counter() - basla:.4f}s")  # ~0.0s

# Cache istatistikleri
print(agir_islem.cache_info())
# CacheInfo(hits=5, misses=5, maxsize=128, currsize=5)

# Cache temizleme
agir_islem.cache_clear()

# Cache boyutunu öğrenme
print(f"Cache boyutu: {agir_islem.cache_info().currsize}")
```

### total_ordering (Sıralama Tamamlama)

```python
from functools import total_ordering

@total_ordering
class Ogrenci:
    def __init__(self, ad, not_ort):
        self.ad = ad
        self.not_ort = not_ort

    def __eq__(self, other):
        return self.not_ort == other.not_ort

    def __lt__(self, other):
        return self.not_ort < other.not_ort

    def __repr__(self):
        return f"{self.ad}({self.not_ort})"

# __le__, __gt__, __ge__ otomatik türetilir
o1 = Ogrenci("Ali", 85)
o2 = Ogrenci("Veli", 92)
o3 = Ogrenci("Ayşe", 78)

print(o1 > o2)    # False (otomatik)
print(o1 <= o2)   # True (otomatik)
print(o1 >= o3)   # True (otomatik)

# Sıralama
print(sorted([o1, o2, o3]))  # [Ayşe(78), Ali(85), Veli(92)]
```

### reduce ve cmp_to_key

```python
from functools import reduce, cmp_to_key

# reduce: kümülatif işlem
sayilar = [1, 2, 3, 4, 5]

toplam = reduce(lambda a, b: a + b, sayilar)  # 15
carpim = reduce(lambda a, b: a * b, sayilar)  # 120
maks = reduce(lambda a, b: a if a > b else b, sayilar)  # 5
min = reduce(lambda a, b: a if a < b else b, sayilar)   # 1

# reduce ile başlangıç değeri
fark = reduce(lambda a, b: a - b, [1, 2, 3], 100)  # 100-1-2-3 = 94
print(f"Fark: {fark}")

# cmp_to_key: eski tarz karşılaştırma fonksiyonundan key üretme
def karsilastir(a, b):
    if a["not"] > b["not"]:
        return -1
    elif a["not"] < b["not"]:
        return 1
    return 0

ogrenciler = [
    {"ad": "Ali", "not": 85},
    {"ad": "Veli", "not": 92},
    {"ad": "Ayşe", "not": 78},
]

sirali = sorted(ogrenciler, key=cmp_to_key(karsilastir))
print([o["ad"] for o in sirali])  # ['Veli', 'Ali', 'Ayşe']

# Pratik: iç içe listeyi düzleştirme
def duzlestir(lst):
    return reduce(lambda x, y: x + (duzlestir(y) if isinstance(y, list) else [y]), lst, [])

print(duzlestir([1, [2, [3, 4], 5], 6]))  # [1, 2, 3, 4, 5, 6]
```

### @wraps (Detaylı)

```python
from functools import wraps
import time
import logging

def logla(seviye="info"):
    def dekorator(fn):
        @wraps(fn)
        def wrapper(*args, **kwargs):
            baslangic = time.perf_counter()
            try:
                sonuc = fn(*args, **kwargs)
                sure = time.perf_counter() - baslangic
                logger = logging.getLogger(fn.__module__)
                getattr(logger, seviye)(f"{fn.__name__} çağrıldı ({sure:.4f}s)")
                return sonuc
            except Exception as e:
                logger = logging.getLogger(fn.__module__)
                logger.error(f"{fn.__name__} hatası: {e}")
                raise
        return wrapper
    return dekorator

@logla(seviye="debug")
def veri_getir(url):
    """Veri getirme fonksiyonu."""
    time.sleep(0.1)
    return f"{url} içeriği"

# @wraps olmadan:
# print(veri_getir.__name__)  # wrapper (yanlış!)
# @wraps ile:
print(veri_getir.__name__)  # veri_getir (doğru)
print(veri_getir.__doc__)   # Veri getirme fonksiyonu. (korundu)
```

## <a id="zoneinfo-arsiv"></a>49. zoneinfo ve Arşiv/Sıkıştırma Modülleri:

### zoneinfo ile Zaman Dilimleri (Python 3.9+)

```python
from zoneinfo import ZoneInfo, available_timezones
from datetime import datetime, timedelta
import time

# Kullanılabilir zaman dilimleri
print(f"Toplam: {len(available_timezones())} zaman dilimi")
print("TR" in available_timezones())  # False (yaşasın Türkiye tek saat dilimi)
print("Europe/Istanbul" in available_timezones())  # True

# Zaman dilimi ile datetime
istanbul = datetime.now(ZoneInfo("Europe/Istanbul"))
newyork = datetime.now(ZoneInfo("America/New_York"))
tokyo = datetime.now(ZoneInfo("Asia/Tokyo"))
londra = datetime.now(ZoneInfo("Europe/London"))

print(f"İstanbul: {istanbul}")
print(f"New York: {newyork}")
print(f"Tokyo: {tokyo}")
print(f"Londra: {londra}")

# Zaman dilimi dönüşümü
simdi = datetime.now(ZoneInfo("Europe/Istanbul"))
print(f"İstanbul: {simdi}")

# New York saatine çevir
ny_saati = simdi.astimezone(ZoneInfo("America/New_York"))
print(f"New York: {ny_saati}")

# UTC'ye çevir
utc_saati = simdi.astimezone(ZoneInfo("UTC"))
print(f"UTC: {utc_saati}")

# Zaman farkı (offset)
print(f"İstanbul offset: {simdi.utcoffset()}")
print(f"İstanbul DST: {simdi.dst()}")

# Kullanıcı tercihine göre saat gösterme
def saat_goster(tarih_str, format="%Y-%m-%d %H:%M", kaynak_bolge="UTC", hedef_bolge="Europe/Istanbul"):
    kaynak = datetime.strptime(tarih_str, "%Y-%m-%d %H:%M")
    kaynak = kaynak.replace(tzinfo=ZoneInfo(kaynak_bolge))
    hedef = kaynak.astimezone(ZoneInfo(hedef_bolge))
    return hedef.strftime(format)

# UTC'den İstanbul'a çevir
print(saat_goster("2026-05-29 12:00", kaynak_bolge="UTC"))
# İstanbul: 15:00

# Zaman dilimleri arası fark hesabı
t1 = datetime(2026, 5, 29, 12, 0, tzinfo=ZoneInfo("Europe/Istanbul"))
t2 = datetime(2026, 5, 29, 12, 0, tzinfo=ZoneInfo("America/New_York"))
fark = t1 - t2
print(f"Saat farkı: {fark.total_seconds() / 3600:.0f} saat")

# replace ile zaman dilimi değiştirme (dönüştürmez, etiket değiştirir)
dt = datetime(2026, 5, 29, 12, 0)
dt_ist = dt.replace(tzinfo=ZoneInfo("Europe/Istanbul"))
print(dt_ist)  # 2026-05-29 12:00:00+03:00

# tzinfo None'dan atama
dt_naive = datetime(2026, 1, 1, 0, 0, 0)  # naive datetime
dt_aware = dt_naive.replace(tzinfo=ZoneInfo("UTC"))
print(dt_aware)
```

### zipfile ile ZIP Arşivleri

```python
import zipfile
import os
from pathlib import Path

# ----- ZIP Oluşturma -----
with zipfile.ZipFile("arsiv.zip", "w", zipfile.ZIP_DEFLATED) as zf:
    zf.write("dosya1.txt")           # tek dosya
    zf.write("dosya2.txt", "alt/dosya2.txt")  # alt dizinde
    zf.write("resim.jpg")

    # String'den ekleme
    zf.writestr("notlar/okuma.txt", "Bu bir test içeriğidir.")

    # Sıkıştırma seviyesi
    zf.comment = b"Arşiv açıklaması"

# ----- ZIP Okuma -----
with zipfile.ZipFile("arsiv.zip", "r") as zf:
    # İçindekiler
    for bilgi in zf.infolist():
        print(f"{bilgi.filename}: {bilgi.file_size} -> {bilgi.compress_size} bayt "
              f"({bilgi.compress_size/bilgi.file_size*100:.1f}%)")

    # Tek dosya okuma
    icerik = zf.read("notlar/okuma.txt")
    print(icerik.decode("utf-8"))

    # Dosya çıkarma
    zf.extract("dosya1.txt")           # tek dosya
    zf.extractall("cikti_klasoru")     # tüm arşiv
    zf.extractall("cikti", members=["dosya1.txt", "dosya2.txt"])  # seçili

# ----- ZIP Pratik -----
def klasoru_zip_yap(kaynak_klasor, hedef_zip):
    """Bir klasörü ZIP arşivine dönüştürür."""
    with zipfile.ZipFile(hedef_zip, "w", zipfile.ZIP_DEFLATED) as zf:
        for dosya in Path(kaynak_klasor).rglob("*"):
            if dosya.is_file():
                zf.write(dosya, dosya.relative_to(kaynak_klasor.parent))
    return hedef_zip

# ZIP dosyasına parola koruması
# with zipfile.ZipFile("korumali.zip", "w", zipfile.ZIP_DEFLATED) as zf:
#     zf.setpassword(b"gizli-sifre")
#     zf.write("gizli.txt")

# Sıkıştırma yöntemleri
# ZIP_STORED:   sıkıştırmasız (hızlı)
# ZIP_DEFLATED: deflate (standart)
# ZIP_BZIP2:    bzip2 (daha iyi sıkıştırma)
# ZIP_LZMA:     lzma (en iyi sıkıştırma)
```

### tarfile ile TAR Arşivleri

```python
import tarfile
import os

# ----- TAR Oluşturma -----
with tarfile.open("arsiv.tar.gz", "w:gz") as tf:  # gzip sıkıştırmalı
    tf.add("dosya1.txt")
    tf.add("klasor", arcname="yedek_klasor")  # yeniden adlandır

# Sıkıştırma modları:
# "w":     sıkıştırmasız tar
# "w:gz":  gzip sıkıştırmalı
# "w:bz2": bzip2 sıkıştırmalı
# "w:xz":  lzma/xz sıkıştırmalı

# ----- TAR Okuma -----
with tarfile.open("arsiv.tar.gz", "r:gz") as tf:
    # İçindekiler
    for uye in tf.getmembers():
        print(f"{uye.name}: {uye.size} bayt, {uye.mtime}")

    # Dosya çıkarma
    tf.extractall("cikti_tar")             # tümü
    tf.extract("dosya1.txt", "cikti_tar")   # seçili

    # Dosya içeriğini okuma
    dosya = tf.extractfile("dosya1.txt")
    if dosya:
        print(dosya.read().decode("utf-8"))
```

### gzip, bz2, lzma ile Sıkıştırma

```python
import gzip
import bz2
import lzma

# ----- gzip -----
# Yazma
with gzip.open("dosya.txt.gz", "wt", encoding="utf-8") as f:
    f.write("Bu dosya gzip ile sıkıştırılmıştır.\n")
    f.write("Birden çok satır yazılabilir.\n")

# Okuma
with gzip.open("dosya.txt.gz", "rt", encoding="utf-8") as f:
    icerik = f.read()
    print(icerik)

# Binary
with gzip.open("veri.bin.gz", "wb") as f:
    f.write(b"binary data")

# Sıkıştırma seviyesi (1-9, varsayılan 9)
with gzip.open("hizli.gz", "wt", compresslevel=1) as f:
    f.write("hızlı ama az sıkıştırma")

with gzip.open("maksimum.gz", "wt", compresslevel=9) as f:
    f.write("yavaş ama çok sıkıştırma")

# Bytes üzerinde gzip
veri = b"Merhaba Python Dunyasi"
sikistirilmis = gzip.compress(veri)
cozulmus = gzip.decompress(sikistirilmis)
print(f"{len(veri)} -> {len(sikistirilmis)} bayt")

# ----- bz2 -----
# bzip2: gzip'ten yavaş ama daha iyi sıkıştırma
with bz2.open("dosya.bz2", "wt", encoding="utf-8") as f:
    f.write("bzip2 sıkıştırma örneği\n")

with bz2.open("dosya.bz2", "rt", encoding="utf-8") as f:
    print(f.read())

# ----- lzma/xz -----
# lzma: en iyi sıkıştırma oranı, en yavaş
with lzma.open("dosya.xz", "wt", encoding="utf-8") as f:
    f.write("lzma/xz sıkıştırma örneği\n")

with lzma.open("dosya.xz", "rt", encoding="utf-8") as f:
    print(f.read())

# Dosya sıkıştırma karşılaştırması
import time
def sikistirma_karsilastir(dosya_adi, veri):
    for ad, modul in [("gzip", gzip), ("bz2", bz2), ("lzma", lzma)]:
        basla = time.perf_counter()
        sikis = modul.compress(veri.encode())
        sure = time.perf_counter() - basla
        print(f"{ad}: {len(veri)} -> {len(sikis)} bayt ({sure:.4f}s)")

buyuk_veri = "Merhaba " * 10000
sikistirma_karsilastir("test", buyuk_veri)
```

---

## <a id="socket-programlama"></a>50. Socket Programlama (TCP/UDP):

`socket` modülü, Python'da ağ programlama için düşük seviyeli bir arayüz sağlar. TCP (bağlantı odaklı) ve UDP (bağlantısız) protokollerini destekler.

### TCP Sunucu (Server)

```python
import socket
import threading

def istemci_isle(conn, addr):
    """Bağlanan istemciyi işler."""
    print(f"[+] Yeni bağlantı: {addr}")
    with conn:
        while True:
            veri = conn.recv(1024)  # en fazla 1024 bayt oku
            if not veri:
                break
            mesaj = veri.decode("utf-8")
            print(f"[{addr}] {mesaj}")
            conn.sendall(f"Echo: {mesaj}".encode("utf-8"))
    print(f"[-] Bağlantı koptu: {addr}")

def tcp_sunucu(host="127.0.0.1", port=65432):
    """TCP sunucu başlatır."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as sunucu:
        sunucu.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        sunucu.bind((host, port))
        sunucu.listen(5)  # maksimum 5 bekleyen bağlantı
        print(f"[*] Sunucu {host}:{port} dinleniyor...")

        while True:
            conn, addr = sunucu.accept()
            thread = threading.Thread(target=istemci_isle, args=(conn, addr))
            thread.start()

# Çalıştırma: tcp_sunucu()
```

### TCP İstemci (Client)

```python
import socket

def tcp_istemci(host="127.0.0.1", port=65432):
    """TCP sunucuya bağlanır."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as istemci:
        istemci.connect((host, port))
        print(f"[*] {host}:{port} bağlanıldı.")

        while True:
            mesaj = input("> ").strip()
            if not mesaj or mesaj.lower() == "quit":
                break
            istemci.sendall(mesaj.encode("utf-8"))
            yanit = istemci.recv(1024).decode("utf-8")
            print(f"< {yanit}")

# Çalıştırma: tcp_istemci()
```

### UDP (Bağlantısız)

```python
import socket

def udp_sunucu(host="127.0.0.1", port=65432):
    """UDP sunucu."""
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as sunucu:
        sunucu.bind((host, port))
        print(f"[*] UDP sunucu {host}:{port} dinleniyor...")

        while True:
            veri, addr = sunucu.recvfrom(1024)
            mesaj = veri.decode("utf-8")
            print(f"[{addr}] {mesaj}")
            sunucu.sendto(f"Echo: {mesaj}".encode("utf-8"), addr)

def udp_istemci(host="127.0.0.1", port=65432):
    """UDP istemci."""
    with socket.socket(socket.AF_INET, socket.SOCK_DGRAM) as istemci:
        istemci.settimeout(5)

        while True:
            mesaj = input("> ").strip()
            if not mesaj or mesaj.lower() == "quit":
                break
            istemci.sendto(mesaj.encode("utf-8"), (host, port))
            try:
                veri, _ = istemci.recvfrom(1024)
                print(f"< {veri.decode('utf-8')}")
            except socket.timeout:
                print("< Zaman aşımı")
```

### Soket Seçenekleri ve Detaylar

```python
import socket

# Soket oluşturma
s = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# AF_INET: IPv4, AF_INET6: IPv6
# SOCK_STREAM: TCP, SOCK_DGRAM: UDP

# Zaman aşımı
s.settimeout(10)          # saniye
s.settimeout(None)        # bloklama modu
s.setblocking(False)      # non-blocking

# Soket seçenekleri
s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)  # port hemen yeniden kullanım
s.setsockopt(socket.SOL_SOCKET, socket.SO_KEEPALIVE, 1)   # keepalive

# Soket bilgileri
print(f"Soket adı: {s.getsockname()}")
print(f"Karşı taraf: {s.getpeername()}")

# Host bilgisi
print(socket.gethostname())        # bilgisayar adı
print(socket.gethostbyname("google.com"))  # IP adresi
print(socket.gethostbyaddr("8.8.8.8"))     # host adı

# Select ile çoklu soket yönetimi
import select

soketler = [s]
okunabilir, _, _ = select.select(soketler, [], [], 0.5)
for soket in okunabilir:
    veri = soket.recv(1024)

# Soket çifti (aynı makinede iletişim)
a, b = socket.socketpair()
a.sendall(b"test")
print(b.recv(1024))

s.close()
```

### HTTP İstekleri (Düşük Seviye)

```python
import socket

def basit_http_get(host="example.com", path="/"):
    """Ham socket ile HTTP GET isteği."""
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.settimeout(10)
        s.connect((host, 80))

        # HTTP isteği oluşturma
        istek = (
            f"GET {path} HTTP/1.1\r\n"
            f"Host: {host}\r\n"
            "Connection: close\r\n"
            "User-Agent: PythonSocket/1.0\r\n"
            "\r\n"
        )
        s.sendall(istek.encode())

        # Yanıtı okuma
        yanit = b""
        while True:
            chunk = s.recv(4096)
            if not chunk:
                break
            yanit += chunk

        baslik, _, govde = yanit.partition(b"\r\n\r\n")
        print(f"Başlık:\n{baslik.decode()}")
        print(f"Gövde ({len(govde)} bayt)")

basit_http_get()
```

### Pratik: Port Tarayıcı

```python
import socket
import threading
from queue import Queue

def port_tara(host, port):
    """Belirli bir portu dener."""
    try:
        with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
            s.settimeout(1)
            s.connect((host, port))
            return True
    except (socket.timeout, ConnectionRefusedError):
        return False

def port_tarayici(host, baslangic=1, bitis=1024, thread_sayisi=50):
    """Port tarayıcı."""
    acik_portlar = []
    kuyruk = Queue()

    for port in range(baslangic, bitis + 1):
        kuyruk.put(port)

    def isci():
        while not kuyruk.empty():
            port = kuyruk.get()
            if port_tara(host, port):
                acik_portlar.append(port)
                print(f"[+] Port {port} açık")
            kuyruk.task_done()

    isciler = []
    for _ in range(thread_sayisi):
        t = threading.Thread(target=isci)
        t.start()
        isciler.append(t)

    for t in isciler:
        t.join()

    return sorted(acik_portlar)

# print(port_tarayici("127.0.0.1", 1, 100))
```

### Pratik: Basit Sohbet Uygulaması

```python
import socket
import threading

def sohbet_sunucu(host="127.0.0.1", port=5555):
    """Basit sohbet sunucusu."""
    istemciler = []

    def yayinla(mesaj, gonderen=None):
        for istemci in istemciler:
            if istemci != gonderen:
                try:
                    istemci.sendall(mesaj.encode("utf-8"))
                except:
                    istemciler.remove(istemci)

    def istemci_yonet(conn, addr):
        istemciler.append(conn)
        yayinla(f"[{addr}] sohbete katıldı.")
        try:
            while True:
                veri = conn.recv(1024)
                if not veri:
                    break
                yayinla(f"[{addr}] {veri.decode('utf-8')}", conn)
        except:
            pass
        finally:
            istemciler.remove(conn)
            yayinla(f"[{addr}] ayrıldı.")
            conn.close()

    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as s:
        s.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        s.bind((host, port))
        s.listen()
        print(f"Sohbet sunucusu {host}:{port}")

        while True:
            conn, addr = s.accept()
            threading.Thread(target=istemci_yonet, args=(conn, addr)).start()
```

## <a id="io-struct-base64"></a>51. io, struct ve base64 (Binary Veri):

### io Modülü (StringIO, BytesIO)

```python
import io

# StringIO: string'i dosya gibi kullanma
buffer = io.StringIO()
buffer.write("Birinci satır\n")
buffer.write("İkinci satır\n")
print("Üçüncü satır", file=buffer)

icerik = buffer.getvalue()
print(icerik)

buffer.seek(0)
for satir in buffer:
    print(f"Satır: {satir.strip()}")

buffer.close()

# BytesIO: binary veri için
bio = io.BytesIO()
bio.write(b"binary data")
bio.write(b"more data")
bio.seek(0)
print(bio.read())  # b"binary datamore data"
bio.close()

# with desteği
with io.StringIO() as buf:
    buf.write("Otomatik kapanan buffer")
    print(buf.getvalue())

# Dosya benzeri nesne olarak kullanım
import csv

with io.StringIO() as buf:
    yazici = csv.writer(buf)
    yazici.writerow(["isim", "yas"])
    yazici.writerow(["Ali", 25])
    yazici.writerow(["Veli", 30])
    csv_verisi = buf.getvalue()
    print(csv_verisi)

# StringIO ile yakalama
import sys

def yakala_cikti():
    """print() çıktısını yakala."""
    eski_stdout = sys.stdout
    yakalanan = io.StringIO()
    sys.stdout = yakalanan

    print("Bu yakalanacak")
    print("Bu da yakalanacak")

    sys.stdout = eski_stdout
    return yakalanan.getvalue()

print(f"Yakalanan: {yakala_cikti()}")

# BytesIO ile binary dönüşüm
from PIL import Image
import requests

# Bir resmi BytesIO'ya indir
# response = requests.get("https://example.com/resim.jpg")
# img_data = io.BytesIO(response.content)
# img = Image.open(img_data)
```

### struct ile Binary Veri Paketleme

`struct` modülü, Python değerlerini C yapılarına (binary) dönüştürmek için kullanılır. Ağ protokolleri, dosya formatları ve donanımla iletişimde yaygındır.

```python
import struct

# Paketleme: Python -> bytes
# Format karakterleri:
#   i: int (4 bayt)
#   h: short (2 bayt)
#   f: float (4 bayt)
#   d: double (8 bayt)
#   c: char (1 bayt)
#   s: string (bytes)
#   ?: bool
#   I: unsigned int
#   q: long long (8 bayt)

# Temel kullanım
veri = struct.pack("i f d", 42, 3.14, 2.718)
print(f"Paketlenmiş: {veri.hex()}")

# Çözme
x, y, z = struct.unpack("i f d", veri)
print(f"Çözüldü: {x}, {y:.2f}, {z:.3f}")

# Format string detayları
# <: little-endian, >: big-endian, !: network byte order
veri = struct.pack("!i h b", 0x12345678, 0x7FFF, -128)
print(veri.hex())

# Byte sırası (endianness)
deger = 0x01020304
print("Big endian:    ", struct.pack(">i", deger).hex())  # 01020304
print("Little endian: ", struct.pack("<i", deger).hex())  # 04030201

# Buffer ile kullanım
buffer = bytearray(20)
struct.pack_into("i f d", buffer, 0, 42, 3.14, 2.718)
print(f"Buffer: {buffer.hex()}")

a, b, c = struct.unpack_from("i f d", buffer, 0)
print(f"Buffer'dan: {a}, {b:.2f}, {c:.3f}")

# Pratik: Binary dosya formatı
class BinaryDosya:
    HEADER_FORMAT = "!4s i"  # 4 byte imza + int versiyon
    HEADER_SIZE = struct.calcsize(HEADER_FORMAT)

    def __init__(self, imza=b"PYIO", versiyon=1):
        self.imza = imza
        self.versiyon = versiyon

    def yaz(self, dosya_adi, kayitlar):
        with open(dosya_adi, "wb") as f:
            # Header yaz
            f.write(struct.pack(self.HEADER_FORMAT, self.imza, self.versiyon))
            f.write(struct.pack("!i", len(kayitlar)))  # kayıt sayısı

            for kayit in kayitlar:
                f.write(struct.pack("!i f", kayit[0], kayit[1]))

    def oku(self, dosya_adi):
        with open(dosya_adi, "rb") as f:
            imza, versiyon = struct.unpack(self.HEADER_FORMAT, f.read(self.HEADER_SIZE))
            kayit_sayisi = struct.unpack("!i", f.read(4))[0]

            kayitlar = []
            for _ in range(kayit_sayisi):
                id, deger = struct.unpack("!i f", f.read(8))
                kayitlar.append((id, deger))

            return kayitlar

bd = BinaryDosya()
bd.yaz("veri.bin", [(1, 10.5), (2, 20.3), (3, 30.7)])
print(bd.oku("veri.bin"))
```

### base64 ile Veri Kodlama

```python
import base64

# Temel base64 kodlama
metin = "Merhaba Python Dünyası"
metin_bytes = metin.encode("utf-8")

kodlanmis = base64.b64encode(metin_bytes)
print(f"Base64: {kodlanmis.decode()}")  # TWVyaGFiYSBQeXRob24gRMO8bnlhc8Sx

cozulmus = base64.b64decode(kodlanmis)
print(f"Çözüldü: {cozulmus.decode('utf-8')}")  # Merhaba Python Dünyası

# URL güvenli base64 (+, / yerine -, _)
url_kod = base64.urlsafe_b64encode(b"hello?query=test")
print(f"URL Safe: {url_kod.decode()}")

# Hex format
hex_kod = base64.b16encode(b"Hello")
print(f"Hex: {hex_kod.decode()}")  # 48656C6C6F

# Base32 (daha kısa alfabe)
b32 = base64.b32encode(b"Hello")
print(f"Base32: {b32.decode()}")

# Base85 (ASCII85, daha verimli)
b85 = base64.b85encode(b"Hello World")
print(f"Base85: {b85.decode()}")

# Pratik: Base64 ile resim kodlama
import io

def resmi_base64_yap(dosya_adi):
    """Resmi base64 string'e çevirir (HTML inline img için)."""
    with open(dosya_adi, "rb") as f:
        veri = f.read()
    return base64.b64encode(veri).decode("utf-8")

# HTML'de kullanım:
# <img src="data:image/png;base64,iVBORw0KGgo..." />

# Pratik: Base64 decode etme
def base64_coz(veri):
    """Base64 string'i çözer (padding ekleyerek)."""
    eksik = len(veri) % 4
    if eksik:
        veri += "=" * (4 - eksik)
    return base64.b64decode(veri).decode("utf-8", errors="replace")

print(base64_coz("TWVyaGFiYQ"))
```

## <a id="sys-os-detayli"></a>52. sys ve os Modülleri Detaylı:

### sys Modülü

```python
import sys

# ----- Komut Satırı Argümanları -----
print(f"Script adı: {sys.argv[0]}")
print(f"Argümanlar: {sys.argv[1:]}")

# ----- Python Çalışma Zamanı -----
print(f"Python versiyon: {sys.version}")
print(f"Versiyon tuple: {sys.version_info}")
print(f"Python yorumlayıcı: {sys.executable}")
print(f"Platform: {sys.platform}")  # linux, win32, darwin

# ----- Modül Arama Yolları -----
print(f"sys.path: {sys.path}")
sys.path.append("/benim/modullerim")
sys.path.insert(0, "/once/bak")

# ----- Standart Akışlar -----
sys.stdout.write("stdout'a yaz\n")
sys.stderr.write("stderr'a yaz\n")

# Çıktıyı dosyaya yönlendirme
with open("cikti.txt", "w") as f:
    eski_stdout = sys.stdout
    sys.stdout = f
    print("Bu dosyaya yazılır")
    sys.stdout = eski_stdout

# ----- Çıkış ve Hata -----
# sys.exit(0)          # başarılı çıkış
# sys.exit(1)          # hatalı çıkış
# sys.exit("Hata!")   # hata mesajı ile çıkış

# ----- Referans Sayacı ve Bellek -----
a = []
print(f"Referans sayısı: {sys.getrefcount(a)}")
print(f"Nesne boyutu: {sys.getsizeof(a)} bayt")

# ----- Rekürsiyon Limiti -----
print(f"Maksimum rekürsiyon: {sys.getrecursionlimit()}")
# sys.setrecursionlimit(5000)

# ----- İlerleme Çubuğu -----
def ilerleme_goster(oran, genislik=50):
    """Basit terminal ilerleme çubuğu."""
    dolu = int(oran * genislik)
    bar = "█" * dolu + "░" * (genislik - dolu)
    sys.stdout.write(f"\r{bar} {oran:.0%}")
    sys.stdout.flush()

for i in range(101):
    ilerleme_goster(i / 100)
    import time
    time.sleep(0.02)
print()

# ----- int ve float Bilgileri -----
print(f"Max int size: {sys.maxsize}")
print(f"Float info: {sys.float_info}")

# ----- Modül Önbelleği -----
print(f"Yüklü modüller: {len(sys.modules)}")
```

### os Modülü

```python
import os

# ----- Dizin İşlemleri -----
print(f"Mevcut dizin: {os.getcwd()}")
os.makedirs("alt1/alt2/alt3", exist_ok=True)  # iç içe dizin oluştur
os.mkdir("yenidizin")                          # tek dizin
os.rmdir("yenidizin")                          # boş dizin sil
os.removedirs("alt1/alt2/alt3")                # iç içe sil
os.chdir("/tmp")                                # dizin değiştir
os.chdir(eski_dizin)

# Dosya listeleme
print(os.listdir("."))   # basit liste
with os.scandir(".") as girisler:
    for giris in girisler:
        if giris.is_file():
            print(f"Dosya: {giris.name}, Boyut: {giris.stat().st_size}")
        elif giris.is_dir():
            print(f"Dizin: {giris.name}")

# ----- Dosya İşlemleri -----
os.rename("eski.txt", "yeni.txt")
os.replace("kaynak.txt", "hedef.txt")  # üzerine yazar
os.remove("sil.txt")                    # dosya sil

# Dosya bilgisi (stat)
stat = os.stat("dosya.txt")
print(f"Boyut: {stat.st_size} bayt")
print(f"İzinler: {oct(stat.st_mode)}")
print(f"Oluşturma: {stat.st_ctime}")
print(f"Değiştirme: {stat.st_mtime}")

# ----- Ortam Değişkenleri -----
print(f"PATH: {os.environ.get('PATH', '')}")
print(f"HOME: {os.environ.get('HOME', '')}")

# Ortam değişkeni ayarlama
os.environ["MY_VAR"] = "deger"
print(os.environ.get("MY_VAR"))
del os.environ["MY_VAR"]

# .env dosyası okuma benzeri
def dotenv_oku(dosya=".env"):
    """Basit .env okuyucu."""
    if os.path.exists(dosya):
        with open(dosya) as f:
            for satir in f:
                satir = satir.strip()
                if satir and not satir.startswith("#"):
                    anahtar, _, deger = satir.partition("=")
                    os.environ[anahtar.strip()] = deger.strip().strip("\"'")

# ----- Süreç ve Sistem -----
print(f"PID: {os.getpid()}")
print(f"PPID: {os.getppid()}")
print(f"Kullanıcı: {os.getlogin()}")
print(f"CPU sayısı: {os.cpu_count()}")
print(f"Yük ortalaması: {os.getloadavg()}")
print(f"Hostname: {os.uname().nodename}")

# ----- İzinler -----
os.chmod("dosya.txt", 0o644)    # rw-r--r--
os.chown("dosya.txt", uid, gid)  # sahip değiştir
os.access("dosya.txt", os.R_OK | os.W_OK)  # okuma/yazma kontrolü

# ----- Sembolik Link -----
os.symlink("hedef.txt", "link.txt")       # sembolik link oluştur
print(os.readlink("link.txt"))             # link hedefini oku
os.path.islink("link.txt")                 # link mi?

# ----- os.path (entegre kullanım) -----
print(os.path.abspath("."))
print(os.path.relpath("/a/b/c", "/a"))  # b/c
print(os.path.join("a", "b", "c.txt"))  # a/b/c.txt
print(os.path.split("/a/b/c.txt"))      # ('/a/b', 'c.txt')
print(os.path.splitext("dosya.txt"))    # ('dosya', '.txt')

# Dosya zamanları
yol = "dosya.txt"
print(f"Erişim: {os.path.getatime(yol)}")
print(f"Değişim: {os.path.getmtime(yol)}")
print(f"Meta değişim: {os.path.getctime(yol)}")
```

### platform Modülü

```python
import platform

print(f"Sistem: {platform.system()}")           # Linux, Windows, Darwin
print(f"Düğüm: {platform.node()}")               # hostname
print(f"Sürüm: {platform.version()}")            # kernel versiyon
print(f"Makine: {platform.machine()}")           # x86_64, arm64
print(f"İşlemci: {platform.processor()}")        # CPU bilgisi
print(f"Mimari: {platform.architecture()}")      # ('64bit', 'ELF')

print(f"Python: {platform.python_implementation()}")  # CPython, PyPy
print(f"Python yapısı: {platform.python_build()}")
print(f"Python derleyici: {platform.python_compiler()}")

# Platform tespiti
if platform.system() == "Linux":
    print("Linux üzerindesiniz")
elif platform.system() == "Windows":
    print("Windows üzerindesiniz")
elif platform.system() == "Darwin":
    print("macOS üzerindesiniz")

# Sürüm karşılaştırma
import sys
print(f"Python 3.10+: {sys.version_info >= (3, 10)}")
print(f"Python 3.12+: {sys.version_info >= (3, 12)}")
```

## <a id="yardimci-moduller"></a>53. configparser, shelve, calendar (Yardımcı Modüller):

### configparser ile Yapılandırma Dosyaları

```python
import configparser

# ----- Yapılandırma Dosyası Yazma -----
config = configparser.ConfigParser()

config["DEFAULT"] = {
    "debug": False,
    "port": 5000,
}

config["veritabani"] = {
    "host": "localhost",
    "port": "5432",
    "kullanici": "admin",
    "sifre": "sifre123",
    "veritabani": "myapp",
}

config["email"] = {
    "smtp_host": "smtp.gmail.com",
    "smtp_port": "587",
    "use_tls": "true",
}

with open("app.ini", "w", encoding="utf-8") as f:
    config.write(f)

# ----- Yapılandırma Dosyası Okuma -----
config = configparser.ConfigParser()
config.read("app.ini", encoding="utf-8")

# Değerlere erişim
print(f"Debug: {config['DEFAULT']['debug']}")
print(f"Port: {config.getint('DEFAULT', 'port')}")
print(f"DB Host: {config['veritabani']['host']}")
print(f"SMTP Port: {config.getint('email', 'smtp_port')}")

# Tiplere göre okuma
debug = config.getboolean("DEFAULT", "debug")
port = config.getint("DEFAULT", "port")
oran = config.getfloat("DEFAULT", "oran") if config.has_option("DEFAULT", "oran") else 1.0

# Varsayılan değerler
port = config.getint("veritabani", "port", fallback=5432)

# Bölüm kontrolü
if config.has_section("redis"):
    print("Redis konfigürasyonu var")

# Tüm bölümler ve anahtarlar
for bolum in config.sections():
    print(f"[{bolum}]")
    for anahtar, deger in config[bolum].items():
        print(f"  {anahtar} = {deger}")

# Değişken enterpolasyonu (değişken referansı)
config["DEFAULT"]["basedir"] = "/home/user/app"
config["veritabani"]["log_yolu"] = "%(basedir)s/logs/db.log"
print(config["veritabani"]["log_yolu"])  # /home/user/app/logs/db.log

# Yazma ve güncelleme
config["veritabani"]["port"] = "5433"
with open("app.ini", "w", encoding="utf-8") as f:
    config.write(f)

# Yorum satırları
config.set("veritabani", "; bu bir yorum")
```

### shelve ile Kalıcı Sözlük

`shelve` modülü, Python nesnelerini diskte kalıcı olarak sözlük benzeri bir arayüzle saklar.

```python
import shelve

# ----- Yazma -----
with shelve.open("veri.db") as db:
    db["isim"] = "Ali"
    db["yas"] = 25
    db["notlar"] = [85, 90, 78]
    db["kullanici"] = {"ad": "Veli", "aktif": True}
    db["pi"] = 3.14159

# ----- Okuma -----
with shelve.open("veri.db") as db:
    print(f"İsim: {db['isim']}")
    print(f"Yaş: {db['yas']}")
    print(f"Notlar: {db['notlar']}")
    print(f"Pi: {db.get('pi', 'yok')}")

    # Tüm anahtarlar
    print(f"Anahtarlar: {list(db.keys())}")

    # Varlık kontrolü
    print(f"'isim' var mı: {'isim' in db}")

# ----- Güncelleme -----
with shelve.open("veri.db", writeback=True) as db:
    db["yas"] = 26  # doğrudan güncelleme

    # writeback=True olmadan mutable nesne güncelleme
    db["notlar"] = db.get("notlar", []) + [95]

    # writeback=True ile (yavaş, dikkatli kullanın)
    db["kullanici"]["adres"] = "İstanbul"

# ----- Sınırlama ve Kapatma -----
db = shelve.open("veri.db")
try:
    print(db["isim"])
finally:
    db.close()

# Silme
with shelve.open("veri.db") as db:
    del db["pi"]

# NOT: shelve pickle kullanır, güvenilmeyen kaynaklardan yüklemeyin
# Anahtar olarak sadece string kullanılabilir
```

### calendar Modülü

```python
import calendar

# ----- Takvim Oluşturma -----
# Yıl takvimi
print(calendar.calendar(2026))

# Ay takvimi
print(calendar.month(2026, 5))

# HTML takvim
cal_html = calendar.HTMLCalendar()
print(cal_html.formatmonth(2026, 5))

# ----- Hafta Günleri -----
print(f"Haftanın günleri: {list(calendar.day_name)}")
print(f"Ay kısaltmaları: {list(calendar.month_abbr)[1:]}")

# Belirli bir günün adı
print(calendar.day_name[calendar.weekday(2026, 5, 29)])  # Cuma

# ----- Ay Bilgileri -----
# Ayın ilk günü ve gün sayısı
ilk_gun, gun_sayisi = calendar.monthrange(2026, 5)
print(f"Mayıs 2026: {gun_sayisi} gün, ilk gün: {calendar.day_name[ilk_gun]}")

# Artık yıl kontrolü
print(f"2024 artık yıl: {calendar.isleap(2024)}")  # True
print(f"2026 artık yıl: {calendar.isleap(2026)}")  # False
print(f"2000-2026 arası artık yıllar: {calendar.leapdays(2000, 2026)}")

# ----- Pratik: İş Günü Hesaplama -----
def is_gunleri(yil, ay):
    """Bir aydaki iş günü sayısını hesaplar."""
    import datetime
    _, gun_sayisi = calendar.monthrange(yil, ay)
    is_gunu = 0
    for gun in range(1, gun_sayisi + 1):
        if calendar.weekday(yil, ay, gun) < 5:  # Pazartesi-Cuma
            is_gunu += 1
    return is_gunu

print(f"Mayıs 2026 iş günü: {is_gunleri(2026, 5)}")

# ----- Pratik: Takvim Çıktısı Oluşturma -----
def basit_takvim(yil, ay):
    """Konsolda güzel bir takvim gösterir."""
    cal = calendar.TextCalendar()
    return cal.formatmonth(yil, ay)

print(basit_takvim(2026, 12))

# ----- Pratik: Tarih Listeleyici -----
import datetime

def ayin_cumalari(yil, ay):
    """Bir aydaki tüm cumaları listeler."""
    cumalar = []
    for gun in range(1, calendar.monthrange(yil, ay)[1] + 1):
        if calendar.weekday(yil, ay, gun) == 4:  # Cuma
            cumalar.append(datetime.date(yil, ay, gun))
    return cumalar

print(f"Mayıs 2026 Cumaları: {ayin_cumalari(2026, 5)}")
```

### getpass ve string Modülleri

```python
import getpass
import string

# ----- getpass (güvenli parola girişi) -----
try:
    kullanici = input("Kullanıcı adı: ")
    sifre = getpass.getpass("Şifre: ")  # ekrana yazılmaz
    print(f"Giriş: {kullanici}")
except getpass.GetPassWarning:
    print("Getpass uyarısı")

# ----- string Modülü -----
print(f"Ascii harfler: {string.ascii_letters}")
print(f"Ascii küçük: {string.ascii_lowercase}")
print(f"Ascii büyük: {string.ascii_uppercase}")
print(f"Rakamlar: {string.digits}")
print(f"Hex rakamlar: {string.hexdigits}")
print(f"Noktalama: {string.punctuation}")
print(f"Boşluk: {repr(string.whitespace)}")
print(f"Yazdırılabilir: {string.printable[:50]}...")

# Template string: güvenli string değiştirme
from string import Template

t = Template("Merhaba $isim, $yas yaşındasın.")
print(t.substitute(isim="Ali", yas=25))
print(t.safe_substitute(isim="Veli"))  # $yas boş kalır

# Sözlük ile substitute
b;
print(t.safe_substitute(yas=30))  # $isim boş kalır (hata vermez)

# Formatter ile özel formatlama
class CustomFormatter(string.Formatter):
    def format_field(self, value, format_spec):
        if format_spec == "parabirimi":
            return f"₺{float(value):.2f}"
        return super().format_field(value, format_spec)

fmt = CustomFormatter()
print(fmt.format("{fiyat:parabirimi}", fiyat=99.9))
```

## <a id="hata-ayiklama-araclari"></a>54. inspect, traceback, pprint (Hata Ayıklama):

### inspect Modülü

```python
import inspect
import math
import sys

# ----- Nesne İnceleme -----
print(f"math bir modül: {inspect.ismodule(math)}")
print(f"sqrt bir fonksiyon: {inspect.isfunction(math.sqrt)}")
print(f"pi bir sabit: {inspect.isroutine(math.pi)}")

# Sınıf inceleme
class Ornek:
    """Örnek sınıf."""
    x = 10
    def __init__(self, y):
        self.y = y

    def metot(self):
        pass

    @classmethod
    def sinif_metodu(cls):
        pass

    @staticmethod
    def statik_metot():
        pass

obj = Ornek(5)

print(f"sınıf mı: {inspect.isclass(Ornek)}")
print(f"metot mu: {inspect.ismethod(obj.metot)}")
print(f"fonksiyon mu: {inspect.isfunction(Ornek.metot)}")

# ----- Kaynak Kodu Erişimi -----
print(f"Kaynak: {inspect.getsource(math.sqrt)}")
print(f"Sınıf kaynağı: {inspect.getsource(Ornek)}")
print(f"Dosya: {inspect.getfile(math)}")
print(f"Satır no: {inspect.getsourcelines(Ornek)[1]}")

# ----- İmza İnceleme -----
def ornek_fonk(a: int, b: str = "varsayilan", *args, **kwargs) -> bool:
    """Örnek fonksiyon."""
    return True

imza = inspect.signature(ornek_fonk)
print(f"Parametreler: {list(imza.parameters.keys())}")
for isim, param in imza.parameters.items():
    print(f"  {isim}: tür={param.annotation}, varsayılan={param.default}, "
          f"türü={param.kind.name}")

# Dönüş tipi
print(f"Dönüş: {imza.return_annotation}")

# ----- Çağrı Yığını (Stack) -----
def derin():
    print(f"Çağrıldığım yer: {inspect.stack()[0][3]}")  # derin

def ara():
    derin()
    print(f"Çağrı yığını: {[(x.function, x.lineno) for x in inspect.stack()]}")

ara()

# ----- Modül Üyeleri -----
for isim, uye in inspect.getmembers(math):
    if not isim.startswith("_"):
        print(f"{isim}: {type(uye).__name__}")

# ----- currentframe -----
def su_an():
    frame = inspect.currentframe()
    print(f"Şu an: {frame.f_code.co_name}")
    print(f"Satır: {frame.f_lineno}")
    print(f"Yerel değişkenler: {frame.f_locals}")

su_an()
```

### traceback Modülü

```python
import traceback
import sys

# ----- Hata Detaylarını Yakalama -----
def hata_ayikla():
    """Hata traceback'ini string olarak al."""
    try:
        x = 1 / 0
    except ZeroDivisionError:
        tb = traceback.format_exc()
        print("Traceback (string):")
        print(tb)

# ----- Özel Traceback Formatlama -----
def detayli_hata():
    """Detaylı hata bilgisi."""
    try:
        raise ValueError("Özel hata mesajı")
    except ValueError as e:
        exc_type, exc_value, exc_tb = sys.exc_info()

        print(f"Hata türü: {exc_type.__name__}")
        print(f"Hata mesajı: {exc_value}")

        # Traceback özeti
        ozet = traceback.extract_tb(exc_tb)
        for frame in ozet:
            print(f"  {frame.filename}:{frame.lineno} - {frame.name}")

        # Formatlı traceback
        traceback.print_exc()

# ----- Pratik: Loglama ile Entegrasyon -----
import logging

logger = logging.getLogger(__name__)

def guvenli_islem():
    """Hataları logla ve devam et."""
    try:
        risky_islem()
    except Exception:
        logger.error("İşlem başarısız:\n%s", traceback.format_exc())

# ----- stack() ile Mevcut Yığın -----
def seviye1():
    seviye2()

def seviye2():
    for frame in traceback.stack():
        print(f"{frame.filename}:{frame.lineno} {frame.name}()")

# seviye1()

# ----- Limitli Traceback -----
def uzun_traceback():
    frames = []
    try:
        raise RuntimeError("test")
    except RuntimeError:
        traceback.print_exc(limit=2)  # sadece 2 seviye
        print("--- Kısa format ---")
        traceback.print_exc(limit=1, chain=False)

# ----- Özel Exception Hook -----
def ozel_excepthook(tip, deger, tb):
    """Global exception handler."""
    print("=" * 50)
    print("BEKLENMEYEN HATA!")
    print("=" * 50)
    traceback.print_exception(tip, deger, tb)
    print("=" * 50)

# sys.excepthook = ozel_excepthook
```

### pprint Modülü

```python
import pprint
from collections import OrderedDict

# ----- Güzel Çıktı -----
veri = {
    "isim": "Ali",
    "yas": 25,
    "adres": {
        "sehir": "İstanbul",
        "ilce": "Kadıköy",
        "mahalle": "Caferağa",
    },
    "notlar": [85, 90, 78, {"vize": 85, "final": 90}],
    "aktif": True,
    "etiketler": {"python", "developer", "backend"},
}

# Normal print (tek satırda)
print(veri)

# pprint (okunabilir)
pprint.pprint(veri)

# pformat: string'e çevir
metin = pprint.pformat(veri, indent=2, width=80)
print(metin)

# Özelleştirilmiş pretty printer
pp = pprint.PrettyPrinter(indent=4, depth=3, width=100, compact=True)
pp.pprint(veri)

# ----- Pratik: JSON Benzeri Çıktı -----
import json

class OzelPrinter(pprint.PrettyPrinter):
    def format(self, obj, context, maxlevels, level):
        if isinstance(obj, OrderedDict):
            return json.dumps(dict(obj), indent=2, ensure_ascii=False), True, False
        return super().format(obj, context, maxlevels, level)

pp2 = OzelPrinter()
pp2.pprint(OrderedDict([("b", 1), ("a", 2), ("c", 3)]))
```

## <a id="veri-yapilari-algoritma"></a>55. heapq, bisect, array (Veri Yapıları):

### heapq (Öncelik Kuyruğu / Heap)

```python
import heapq

# ----- Min-Heap (minimum öncelikli) -----
sayilar = [3, 1, 4, 1, 5, 9, 2, 6]
heapq.heapify(sayilar)  # listeyi heap'e çevir (in-place)
print(f"Heap: {sayilar}")

# En küçük elemanı al
print(f"En küçük: {heapq.heappop(sayilar)}")  # 1
print(f"Sonraki: {heapq.heappop(sayilar)}")  # 1

# Eleman ekle
heapq.heappush(sayilar, 0)
print(f"0 eklendi: {sayilar}")

# En küçük N eleman
print(f"En küçük 3: {heapq.nsmallest(3, sayilar)}")
print(f"En büyük 3: {heapq.nlargest(3, sayilar)}")

# ----- Pratik: Öncelik Kuyruğu -----
class Gorev:
    def __init__(self, oncelik, isim):
        self.oncelik = oncelik
        self.isim = isim

    def __lt__(self, other):
        return self.oncelik < other.oncelik

    def __repr__(self):
        return f"{self.isim}(p:{self.oncelik})"

gorev_kuyrugu = []
heapq.heappush(gorev_kuyrugu, Gorev(3, "E-posta gönder"))
heapq.heappush(gorev_kuyrugu, Gorev(1, "Kritik hata düzelt"))
heapq.heappush(gorev_kuyrugu, Gorev(2, "Log kontrol"))

while gorev_kuyrugu:
    gorev = heapq.heappop(gorev_kuyrugu)
    print(f"İşleniyor: {gorev}")

# ----- Pratik: Merge Sorted Sequences -----
import random

def sirali_birlestir(*iterables):
    """Birden çok sıralı listeyi birleştirir."""
    heap = []
    for i, iterable in enumerate(iterables):
        it = iter(iterable)
        try:
            heapq.heappush(heap, (next(it), i, it))
        except StopIteration:
            pass

    while heap:
        deger, i, it = heapq.heappop(heap)
        yield deger
        try:
            heapq.heappush(heap, (next(it), i, it))
        except StopIteration:
            pass

liste1 = [1, 4, 7, 10]
liste2 = [2, 5, 8, 11]
liste3 = [3, 6, 9, 12]
print(list(sirali_birlestir(liste1, liste2, liste3)))
# [1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12]

# ----- Pratik: Akış Medyanı -----
class MedyanBulucu:
    """Sürekli eklenen sayıların medyanını hesaplar."""
    def __init__(self):
        self.kucuk = []  # max-heap (negatif değerlerle)
        self.buyuk = []  # min-heap

    def ekle(self, sayi):
        if not self.kucuk or sayi <= -self.kucuk[0]:
            heapq.heappush(self.kucuk, -sayi)
        else:
            heapq.heappush(self.buyuk, sayi)

        # Dengeli tut (fark max 1)
        if len(self.kucuk) > len(self.buyuk) + 1:
            heapq.heappush(self.buyuk, -heapq.heappop(self.kucuk))
        elif len(self.buyuk) > len(self.kucuk):
            heapq.heappush(self.kucuk, -heapq.heappop(self.buyuk))

    def medyan(self):
        if len(self.kucuk) == len(self.buyuk):
            return (-self.kucuk[0] + self.buyuk[0]) / 2
        return -self.kucuk[0]

mb = MedyanBulucu()
for sayi in [5, 3, 8, 1, 9]:
    mb.ekle(sayi)
    print(f"Eklendi: {sayi}, Medyan: {mb.medyan()}")
```

### bisect (İkili Arama)

```python
import bisect

# ----- Sıralı Listede Arama -----
sayilar = [1, 3, 5, 7, 9, 11]

# bisect_left: soldan ekleme noktası (eşit varsa soluna)
print(f"4 soldan: {bisect.bisect_left(sayilar, 4)}")   # 2 (3 ile 5 arası)
print(f"5 soldan: {bisect.bisect_left(sayilar, 5)}")   # 2 (5'in soluna)

# bisect_right: sağdan ekleme noktası (eşit varsa sağına)
print(f"5 sağdan: {bisect.bisect_right(sayilar, 5)}")  # 3 (5'in sağına)

# insort: sıralı ekleme
yeni = [3, 1, 4, 1, 5, 9, 2]
sirali = []
for s in yeni:
    bisect.insort(sirali, s)
print(f"Sıralı ekleme: {sirali}")  # [1, 1, 2, 3, 4, 5, 9]

# ----- Pratik: Not Sistemi -----
def not_harf(not_degeri):
    """Not değerine göre harf notu belirler."""
    sinirlar = [0, 40, 55, 70, 85, 100]
    harfler = ['FF', 'DD', 'CC', 'BB', 'AA']
    index = bisect.bisect_left(sinirlar, not_degeri) - 1
    return harfler[index] if 0 <= index < len(harfler) else '?'

print(f"75 -> {not_harf(75)}")   # BB
print(f"90 -> {not_harf(90)}")   # AA
print(f"30 -> {not_harf(30)}")   # FF
print(f"52 -> {not_harf(52)}")   # DD

# ----- Pratik: Yakın Değer Bulma -----
def en_yakin(sirali_liste, hedef):
    """Sıralı listede hedefe en yakın değeri bulur."""
    pos = bisect.bisect_left(sirali_liste, hedef)
    if pos == 0:
        return sirali_liste[0]
    if pos == len(sirali_liste):
        return sirali_liste[-1]
    once = sirali_liste[pos - 1]
    sonra = sirali_liste[pos]
    return once if (hedef - once) < (sonra - hedef) else sonra

sayilar = [1, 3, 5, 10, 15, 20]
print(f"7'ye en yakın: {en_yakin(sayilar, 7)}")     # 5
print(f"12'ye en yakın: {en_yakin(sayilar, 12)}")   # 10
print(f"18'e en yakın: {en_yakin(sayilar, 18)}")    # 20
```

### array Modülü (Verimli Diziler)

```python
import array

# ----- Tür Kodları -----
# 'b': signed char, 'B': unsigned char
# 'h': signed short, 'H': unsigned short
# 'i': signed int, 'I': unsigned int
# 'l': signed long, 'L': unsigned long
# 'f': float, 'd': double

# Oluşturma
int_dizi = array.array('i', [1, 2, 3, 4, 5])
float_dizi = array.array('f', [1.5, 2.5, 3.5])
char_dizi = array.array('b', b"hello")

# Liste benzeri kullanım
print(f"int_dizi: {int_dizi}")
print(f"İlk: {int_dizi[0]}, Son: {int_dizi[-1]}")
print(f"Dilim: {int_dizi[1:3]}")

# Ekleme ve çıkarma
int_dizi.append(6)
int_dizi.extend([7, 8])
int_dizi.insert(0, 0)
int_dizi.pop()
print(f"Son: {int_dizi}")

# Bellek Karşılaştırması
import sys

liste = [i for i in range(1000)]
dizi = array.array('i', range(1000))

print(f"Liste boyutu: {sys.getsizeof(liste)} bayt")
print(f"Dizi boyutu: {sys.getsizeof(dizi)} bayt")
print(f"Daha verimli: ~{sys.getsizeof(liste) / sys.getsizeof(dizi):.1f}x")

# Dosyaya yazma/okuma
with open("dizi.bin", "wb") as f:
    int_dizi.tofile(f)

okunan = array.array('i')
with open("dizi.bin", "rb") as f:
    okunan.fromfile(f, 10)  # 10 eleman oku

print(f"Okunan: {okunan.tolist()}")

# bytes dönüşümü
print(f"Bytes: {int_dizi.tobytes()[:20]}...")
```

## <a id="design-patterns"></a>56. Design Patterns (Tasarım Desenleri):

### Singleton (Tek Nesne)

```python
# Yaklaşım 1: Dekoratör
def singleton(sinif):
    ornekler = {}
    def get_instance(*args, **kwargs):
        if sinif not in ornekler:
            ornekler[sinif] = sinif(*args, **kwargs)
        return ornekler[sinif]
    return get_instance

@singleton
class Veritabani:
    def __init__(self):
        self.baglanti = "Veritabanı bağlantısı"

# Her çağrı aynı nesneyi döndürür
db1 = Veritabani()
db2 = Veritabani()
print(f"Aynı mı: {db1 is db2}")  # True

# Yaklaşım 2: __new__ ile
class Ayarlar:
    _ornek = None

    def __new__(cls, *args, **kwargs):
        if cls._ornek is None:
            cls._ornek = super().__new__(cls)
            cls._ornek._baslatildi = False
        return cls._ornek

    def __init__(self):
        if not self._baslatildi:
            self.debug = False
            self.port = 5000
            self.host = "localhost"
            self._baslatildi = True

a1 = Ayarlar()
a2 = Ayarlar()
print(f"Aynı mı: {a1 is a2}")  # True
print(f"Port: {a1.port}")
a2.port = 8080
print(f"Değişti: {a1.port}")  # 8080 (aynı nesne)

# Yaklaşım 3: Metaclass (ileri)
class SingletonMeta(type):
    _orneler = {}
    def __call__(cls, *args, **kwargs):
        if cls not in cls._orneler:
            cls._orneler[cls] = super().__call__(*args, **kwargs)
        return cls._orneler[cls]

class Logger(metaclass=SingletonMeta):
    def __init__(self):
        self.loglar = []
    def log(self, mesaj):
        self.loglar.append(mesaj)
```

### Factory (Fabrika)

```python
from abc import ABC, abstractmethod

# ----- Basit Factory -----
class Hayvan(ABC):
    @abstractmethod
    def ses(self) -> str: ...

class Kopek(Hayvan):
    def ses(self): return "Hav hav!"

class Kedi(Hayvan):
    def ses(self): return "Miyav!"

class Inek(Hayvan):
    def ses(self): return "Möö!"

def hayvan_factory(tur):
    """Hayvan fabrikası."""
    hayvanlar = {
        "kopek": Kopek,
        "kedi": Kedi,
        "inek": Inek,
    }
    cls = hayvanlar.get(tur.lower())
    if cls is None:
        raise ValueError(f"Bilinmeyen hayvan türü: {tur}")
    return cls()

# Kullanım
for tur in ["kopek", "kedi", "inek"]:
    hayvan = hayvan_factory(tur)
    print(f"{tur}: {hayvan.ses()}")

# ----- Factory Method -----
class Belge(ABC):
    @abstractmethod
    def olustur(self): ...

class PDFBelgesi(Belge):
    def olustur(self):
        return "PDF belgesi oluşturuldu"

class WordBelgesi(Belge):
    def olustur(self):
        return "Word belgesi oluşturuldu"

class BelgeOlusturucu(ABC):
    @abstractmethod
    def belge_yarat(self) -> Belge: ...

    def olustur_ve_kaydet(self):
        belge = self.belge_yarat()
        return belge.olustur()

class PDFOlusturucu(BelgeOlusturucu):
    def belge_yarat(self) -> Belge:
        return PDFBelgesi()

class WordOlusturucu(BelgeOlusturucu):
    def belge_yarat(self) -> Belge:
        return WordBelgesi()

for olusturucu in [PDFOlusturucu(), WordOlusturucu()]:
    print(olusturucu.olustur_ve_kaydet())

# ----- Abstract Factory -----
class ModernMobilya:
    def koltuk(self): return "Modern koltuk"
    def masa(self): return "Modern masa"

class KlasikMobilya:
    def koltuk(self): return "Klasik koltuk"
    def masa(self): return "Klasik masa"

def mobilya_factory(tur):
    mobilyalar = {"modern": ModernMobilya, "klasik": KlasikMobilya}
    cls = mobilyalar.get(tur.lower())
    return cls() if cls else None

mobilya = mobilya_factory("modern")
print(mobilya.koltuk())
```

### Observer (Gözlemci)

```python
from abc import ABC, abstractmethod

class Gozlemci(ABC):
    @abstractmethod
    def guncelle(self, mesaj): ...

class Konu:
    """Gözlemlenen nesne (Subject)."""
    def __init__(self):
        self._gozlemciler = []

    def ekle(self, gozlemci: Gozlemci):
        self._gozlemciler.append(gozlemci)

    def cikar(self, gozlemci: Gozlemci):
        self._gozlemciler.remove(gozlemci)

    def bildir(self, mesaj):
        for gozlemci in self._gozlemciler:
            gozlemci.guncelle(mesaj)

# Somut gözlemciler
class EpostaGonderici(Gozlemci):
    def guncelle(self, mesaj):
        print(f"[EPOSTA] Bildirim: {mesaj}")

class SMSGonderici(Gozlemci):
    def guncelle(self, mesaj):
        print(f"[SMS] Bildirim: {mesaj}")

class LogYazici(Gozlemci):
    def guncelle(self, mesaj):
        print(f"[LOG] {mesaj}")

# Kullanım
siparis_sistemi = Konu()
siparis_sistemi.ekle(EpostaGonderici())
siparis_sistemi.ekle(SMSGonderici())
siparis_sistemi.ekle(LogYazici())

siparis_sistemi.bildir("Yeni sipariş alındı: #1234")
# [EPOSTA] Bildirim: Yeni sipariş alındı: #1234
# [SMS] Bildirim: Yeni sipariş alındı: #1234
# [LOG] Yeni sipariş alındı: #1234

# ----- Event System (Gelişmiş) -----
class EventEmitter:
    def __init__(self):
        self._events = {}

    def on(self, event, callback):
        if event not in self._events:
            self._events[event] = []
        self._events[event].append(callback)

    def off(self, event, callback):
        if event in self._events:
            self._events[event].remove(callback)

    def emit(self, event, *args, **kwargs):
        for callback in self._events.get(event, []):
            callback(*args, **kwargs)

emitter = EventEmitter()
emitter.on("kullanici_kayit", lambda email: print(f"Hoş geldin: {email}"))
emitter.on("kullanici_kayit", lambda email: print(f"Doğrulama maili: {email}"))
emitter.emit("kullanici_kayit", "ali@ornek.com")
```

### Strategy (Strateji)

```python
from abc import ABC, abstractmethod

class OdemeStratejisi(ABC):
    @abstractmethod
    def ode(self, tutar): ...

class KrediKarti(OdemeStratejisi):
    def __init__(self, kart_no):
        self.kart_no = kart_no

    def ode(self, tutar):
        return f"Kredi kartı ({self.kart_no[-4:]}) ile {tutar}₺ ödendi"

class PayPal(OdemeStratejisi):
    def __init__(self, email):
        self.email = email

    def ode(self, tutar):
        return f"PayPal ({self.email}) ile {tutar}₺ ödendi"

class Nakit(OdemeStratejisi):
    def ode(self, tutar):
        return f"Nakit {tutar}₺ ödendi"

class Sepet:
    def __init__(self):
        self.urunler = []
        self.odeme_stratejisi = None

    def urun_ekle(self, urun, fiyat):
        self.urunler.append((urun, fiyat))

    def toplam(self):
        return sum(fiyat for _, fiyat in self.urunler)

    def odeme_yap(self, strateji: OdemeStratejisi):
        return strateji.ode(self.toplam())

# Kullanım
sepet = Sepet()
sepet.urun_ekle("Kitap", 150)
sepet.urun_ekle("Kalem", 25)

print(sepet.odeme_yap(KrediKarti("1234567890123456")))
print(sepet.odeme_yap(PayPal("ali@ornek.com")))
print(sepet.odeme_yap(Nakit()))
```

### Decorator Pattern

```python
from functools import wraps

# ----- Decorator Pattern (Yapısal) -----
class Kahve(ABC):
    @abstractmethod
    def maliyet(self) -> float: ...

    @abstractmethod
    def aciklama(self) -> str: ...

class TemelKahve(Kahve):
    def maliyet(self): return 30.0
    def aciklama(self): return "Türk kahvesi"

class KahveDecorator(Kahve):
    def __init__(self, kahve: Kahve):
        self._kahve = kahve

class Sutlu(KahveDecorator):
    def maliyet(self): return self._kahve.maliyet() + 5.0
    def aciklama(self): return f"{self._kahve.aciklama()} + süt"

class Sekerli(KahveDecorator):
    def maliyet(self): return self._kahve.maliyet() + 2.0
    def aciklama(self): return f"{self._kahve.aciklama()} + şeker"

class Kremali(KahveDecorator):
    def maliyet(self): return self._kahve.maliyet() + 8.0
    def aciklama(self): return f"{self._kahve.aciklama()} + krema"

# Kullanım
kahve = TemelKahve()
kahve = Sutlu(kahve)
kahve = Sekerli(kahve)
print(f"{kahve.aciklama()}: {kahve.maliyet()}₺")
# Türk kahvesi + süt + şeker: 37.0₺
```

### Adapter Pattern

```python
# ----- Uyumsuz Arayüz -----
class XMLServis:
    def xml_veri_al(self):
        return "<data><item>değer</item></data>"

# ----- Hedef Arayüz -----
class JSONIstemci:
    def json_veri_iste(self):
        pass

# ----- Adapter -----
class XMLToJSONAdapter(JSONIstemci):
    def __init__(self, xml_servis: XMLServis):
        self.xml_servis = xml_servis

    def json_veri_iste(self):
        xml = self.xml_servis.xml_veri_al()
        # Basit XML -> JSON dönüşümü
        return {"data": {"item": "değer"}}

# Kullanım
xml_servis = XMLServis()
adapter = XMLToJSONAdapter(xml_servis)
print(adapter.json_veri_iste())  # {'data': {'item': 'değer'}}
```

## <a id="python-yenilikler"></a>57. Python 3.12/3.13/3.14 Yenilikleri:

### Python 3.12 (Ekim 2023)

```python
# ----- 1. Daha İyi Hata Mesajları -----
# Python 3.12+ çok daha açıklayıcı hata mesajları gösterir

# ModuleNotFoundError: modül adını önerir
# import nump  # "Did you mean: 'numpy'?"

# NameError: benzer değişken adlarını önerir
# isimler = ["Ali", "Veli"]
# print(ismler)  # "Did you mean: 'isimler'?"

# TypeError: eksik parametreyi söyler
# def fonk(a, b): pass
# fonk(1)  # "missing 1 required positional argument: 'b'"

# ----- 2. f-string Geliştirmeleri -----
# f-string içinde aynı tırnak kullanılabilir
isim = "Ali"
print(f"{isim} = {isim!r}")  # Ali = 'Ali' (3.12+)

# f-string içinde çok satırlı ifadeler
mesaj = f"Merhaba {isim.upper()}"
print(mesaj)

# f-string içinde backslash kullanılabilir
print(f"{isim} - \n - {isim}")

# ----- 3. itertools.batched() -----
import itertools

sayilar = range(10)
for grup in itertools.batched(sayilar, 3):
    print(list(grup))  # [0,1,2], [3,4,5], [6,7,8], [9]

# ----- 4. Pathlib Yenilikleri -----
from pathlib import Path

# walk() metodu (3.12+)
for root, dirs, files in Path(".").walk():
    print(f"Dizin: {root}, Alt: {len(dirs)}, Dosya: {len(files)}")

# ----- 5. typing Yenilikleri -----
from typing import override, type_check_only

class Base:
    def metot(self): ...
    def eski_metot(self): ...

class Alt(Base):
    @override  # üst sınıfta bu metot olmalı (TypeError değil)
    def metot(self): ...

    # @override  # HATA: üst sınıfta yok!
    # def yeni_metot(self): ...

# type_check_only: sadece tip kontrolü için
@type_check_only
class DBInterface:
    def sorgula(self): ...

# ----- 6. Per-Interpreter GIL -----
# import _interpreters  # alt seviye, deneysel
# Python 3.12'de alt yorumlayıcılar (sub-interpreters) ile GIL'i
# kırma çalışmaları başladı

# ----- 7. Performance Improvements -----
# - Modül importları ~%30 daha hızlı
# - Comprehensions daha hızlı (inline cache)
# - asyncio daha hızlı (TaskGroup, timeout)
```

### Python 3.13 (Ekim 2024)

```python
# ----- 1. No-GIL (Free-threaded Python) -----
# PYTHON_GIL=0 ile çalıştırma (deneysel)
# ThreadPoolExecutor artık CPU-bound işlerde de çalışır
# veya: python3.13t script.py

# ----- 2. Python Improvements -----
# Python 3.13'te resmi olarak deneysel JIT derleyici (opt-in)

# ----- 3. New Type Features -----
# TypeDict daha iyi hata mesajları

# ----- 4. Docstring Syntax -----
# PEP 723: dokümantasyon içinde bağımlılık belirtme
# """<script>""" ile çalışan script'ler

# ----- 5. Improved Error Messages -----
# Daha iyi traceback renklendirmesi
# Daha iyi import hata mesajları

# ----- 6. Yeni Modüller ve Özellikler -----
# import random  # random.Random artık daha hızlı

# ----- 7. Performance -----
# - asyncio.locks daha hızlı
# - list.sort() için yeni algoritma
# - locals() 2 kata kadar daha hızlı

# ----- Python 3.13 Syntax Denemeleri -----
# Type parameter syntax (3.12+)
def ilk[T](liste: list[T]) -> T:
    return liste[0]

print(ilk([1, 2, 3]))  # 1

# TypedDict daha esnek
from typing import TypedDict, ReadOnly

class Kullanici(TypedDict):
    ad: ReadOnly[str]  # salt okunur
    yas: int
```

### Python 3.11'e Kısa Bakış (öncesi)

```python
# Python 3.11: Exception Groups ve except*
def exception_gruplari():
    try:
        raise ExceptionGroup("Birden çok hata", [
            ValueError("Hata 1"),
            TypeError("Hata 2"),
        ])
    except* ValueError as e:
        print(f"ValueError: {e.exceptions}")
    except* TypeError as e:
        print(f"TypeError: {e.exceptions}")

# Self tipi
from typing import Self

class Kopyalanabilir:
    def kopyala(self) -> Self:
        return type(self)()

# LiteralString
from typing import LiteralString
def sorgu_calistir(sorgu: LiteralString):  # sadece literal string
    pass

# Variadic Generic
from typing import TypeVarTuple

Ts = TypeVarTuple("Ts")
def tekrarla(*args: *Ts) -> tuple[*Ts, ...]:
    return args
```

### Python 3.14 (Ekim 2025)

```python
# ----- 1. PEP 649: Deferred Evaluation of Annotations -----
# Tip ipuçları artık varsayılan olarak ertelenmiş değerlendirilir
# from __future__ import annotations artık gereksiz (3.14+)

class Kullanici:
    def selamla(self) -> "Kullanici":  # ileri referans sorunsuz
        return self

class TreeNode:
    left: "TreeNode | None"  # henüz tanımlanmamış tipe referans
    right: "TreeNode | None"

# Ancak performans kritik kodda dikkat: tip değerlendirme artık
# daha fazla bellek kullanabilir. Gerekirse __future__ ile kapatma

# ----- 2. Improved Syntax Errors -----
# SyntaxError mesajları daha hassas konum bilgisi içerir
# Hatalı satır + sütun numarası + öneri

# ----- 3. Global Interpreter Lock (GIL) -----
# free-threaded Python (no-GIL) artık daha kararlı
# python3.14t veya PYTHON_GIL=0 ile çalıştırma
import os
if os.environ.get("PYTHON_GIL") == "0":
    print("GIL devre dışı - gerçek paralellik mümkün")
    # ThreadPoolExecutor artık CPU-bound işlerde de çalışır

# ----- 4. JIT Derleyici Geliştirmeleri -----
# Python 3.13'te deneysel olan JIT artık daha kararlı ve hızlı
# Varsayılan olarak kapalı, açmak için:
# PYTHON_JIT=1 python script.py

# ----- 5. New Type Features -----
# ReadOnly tipi artık resmi (3.13'te TypedDict ile deneyseldi)
from typing import ReadOnly

# Final değişkenler için geliştirilmiş destek
# TypeVar artık default type alabilir
from typing import TypeVar, Generic

T = TypeVar("T", default=str)

class Kutu(Generic[T]):
    def __init__(self, icerik: T):
        self.icerik = icerik

# Kutu[int]()  -> T = int
# Kutu()       -> T = str (default)

# ----- 6. Pathlib ve os.path Birleşmesi -----
from pathlib import Path

# Path artık os.path işlevlerinin çoğunu destekler
p = Path("/home/kullanici/dosya.txt")
print(p.stat())   # doğrudan Path üzerinden
print(p.chmod())  # izin değiştirme

# glob() daha hızlı
for f in Path(".").glob("**/*.py", case_sensitive=False):
    print(f)

# ----- 7. Iterable ve Iterator İyileştirmeleri -----
# bisect modülü artık anahtarlı arama destekler
import bisect
sayilar = [(1, "a"), (3, "c"), (5, "e")]
def anahtar(cift):
    return cift[0]
index = bisect.bisect_left(sayilar, (2,), key=anahtar)
print(f"Ekleme noktası: {index}")  # 1

# ----- 8. Improved Warnings -----
# DeprecationWarning'lar artık varsayılan olarak gösterilir
# import warnings
# warnings.warn("Bu eskidi!", DeprecationWarning)
# 3.14 öncesi: sessiz, 3.14+: terminalde görünür

# ----- 9. Performance Improvements -----
# - Python genel ~%10-30 daha hızlı (kıyaslamaya bağlı)
# - asyncio event loop %15-20 daha hızlı
# - JSON parsing ~%20 daha hızlı (yeni tokenizer)
# - Tokenize modülü tamamen yeniden yazıldı
# - __slots__ kullanımı daha hızlı
# - set ve dict işlemleri optimize edildi
# - f-string değerlendirmesi daha hızlı

# ----- 10. Yeni warnings filtresi -----
# DeprecationWarning'lar artık varsayılan olarak gösterilir
# Kodunuzu geleceğe hazırlamak için önemli
```

## <a id="diger-araclar"></a>58. doctest, httpx, string ve Diğer Araçlar:

### doctest ile Dokümantasyon Testi

```python
import doctest

def faktoriyel(n):
    """
    Faktöriyel hesaplar.

    >>> faktoriyel(5)
    120
    >>> faktoriyel(0)
    1
    >>> faktoriyel(3)
    6
    >>> faktoriyel(-1)
    Traceback (most recent call last):
    ValueError: Negatif sayı!
    """
    if n < 0:
        raise ValueError("Negatif sayı!")
    if n <= 1:
        return 1
    return n * faktoriyel(n - 1)

def fibonacci(n):
    """
    Fibonacci sayısını hesaplar.

    >>> [fibonacci(i) for i in range(10)]
    [0, 1, 1, 2, 3, 5, 8, 13, 21, 34]

    >>> fibonacci(10)
    55
    """
    if n < 2:
        return n
    a, b = 0, 1
    for _ in range(n):
        a, b = b, a + b
    return a

def bol(a, b):
    """
    Bölme işlemi.

    >>> bol(10, 3)
    3.333...
    >>> bol(5, 2)
    2.5
    >>> bol(1, 0)
    Traceback (most recent call last):
    ZeroDivisionError: division by zero
    """
    return a / b

# Testleri çalıştırma (verbose)
doctest.testmod(verbose=True)

# Komut satırı:
# python -m doctest module.py -v
# python -m doctest README.md  (markdown da çalışır)
```

### httpx ile Modern HTTP

```python
# pip install httpx
import httpx
import asyncio

# ----- Sync Kullanım -----
# GET isteği
response = httpx.get("https://httpbin.org/get", params={"q": "python"})
print(f"Status: {response.status_code}")
print(f"JSON: {response.json()}")
print(f"Headers: {response.headers}")

# POST
response = httpx.post(
    "https://httpbin.org/post",
    json={"isim": "Ali", "yas": 25},
    headers={"Authorization": "Bearer token123"},
)

# Client Session (bağlantı havuzu)
with httpx.Client() as client:
    for i in range(5):
        response = client.get(f"https://httpbin.org/get?n={i}")
        print(f"İstek {i}: {response.status_code}")

# Timeout
try:
    response = httpx.get("https://httpbin.org/delay/5", timeout=3)
except httpx.TimeoutException:
    print("Zaman aşımı!")

# ----- Async Kullanım -----
async def async_http():
    async with httpx.AsyncClient() as client:
        gorevler = [
            client.get("https://httpbin.org/get"),
            client.get("https://httpbin.org/ip"),
            client.get("https://httpbin.org/user-agent"),
        ]
        sonuclar = await asyncio.gather(*gorevler)
        for sonuc in sonuclar:
            print(f"Async: {sonuc.status_code} ({len(sonuc.content)} bayt)")

asyncio.run(async_http())

# ----- Gelişmiş Özellikler -----
# SSL sertifika doğrulama
# response = httpx.get("https://example.com", verify=False)  # uyarı!

# HTTP/2 desteği
# client = httpx.Client(http2=True)

# Cookie yönetimi
with httpx.Client() as client:
    client.cookies.set("session_id", "abc123")
    response = client.get("https://httpbin.org/cookies")
    print(response.json())

# Dosya yükleme
# with open("dosya.txt", "rb") as f:
#     response = httpx.post("https://httpbin.org/post", files={"dosya": f})

# Streaming
# with httpx.stream("GET", "https://example.com/buyuk_dosya") as response:
#     for chunk in response.iter_bytes():
#         print(f"Chunk: {len(chunk)} bayt")

# Retry
from httpx import TransportError, Limits

limits = Limits(max_keepalive_connections=5, max_connections=10)
client = httpx.Client(limits=limits, timeout=30)

# İstek ara katmanı (event hooks)
def log_istek(request):
    print(f"İstek: {request.method} {request.url}")

def log_yanit(response):
    print(f"Yanıt: {response.status_code}")

client = httpx.Client(event_hooks={"request": [log_istek], "response": [log_yanit]})
```

### webbrowser ve diğer Araçlar

```python
import webbrowser
import calendar
import locale
import textwrap

# ----- webbrowser -----
# Varsayılan tarayıcıda URL açma
# webbrowser.open("https://python.org")
# webbrowser.open_new("https://docs.python.org")  # yeni pencere
# webbrowser.open_new_tab("https://pypi.org")     # yeni sekme

# Belirli tarayıcı
# chrome = webbrowser.get("chrome")
# chrome.open("https://python.org")

# ----- textwrap (Metin Sarma) -----
metin = "Python harika bir programlama dilidir. Okunabilir, esnek ve güçlüdür."

# Sarma (wrap)
print(textwrap.wrap(metin, width=30))
# ['Python harika bir', 'programlama dilidir.', 'Okunabilir, esnek ve', 'güçlüdür.']

# Fill (wrap + join)
print(textwrap.fill(metin, width=30))

# Kısaltma
print(textwrap.shorten(metin, width=30, placeholder="..."))
# Python harika bir...

# Girinti ekleme
print(textwrap.indent(metin, "  "))

# Girinti kaldırma
print(textwrap.dedent("""
    Bu metin
        fazla girintili
    düzeltilecek
""").strip())

# ----- locale (Yerel Ayarlar) -----
try:
    locale.setlocale(locale.LC_ALL, "tr_TR.UTF-8")
    # Para birimi
    print(locale.currency(1250.50, grouping=True))  # ₺1.250,50

    # Tarih formatı
    from datetime import datetime
    print(datetime.now().strftime("%x"))  # 29.05.2026

    # Sayı formatı
    print(locale.format_string("%.2f", 1234567.89, grouping=True))
    # 1.234.567,89

    # Para birimi sembolü
    print(locale.localeconv()["currency_symbol"])
except locale.Error:
    print("Türkçe locale mevcut değil")

# ----- atexit (Çıkışta Çalıştır) -----
import atexit

def temizlik():
    print("Uygulama kapanıyor, temizlik yapılıyor...")

def log_kapat():
    print("Log dosyası kapatılıyor...")

atexit.register(temizlik)
atexit.register(log_kapat)
# atexit.unregister(temizlik)

# Decorator ile
@atexit.register
def dosyalari_kapat():
    print("Açık dosyalar kapatılıyor...")

# ----- contextvars (Bağlam Değişkenleri) -----
import contextvars

istek_id = contextvars.ContextVar("istek_id")

def istek_islem():
    kimlik = istek_id.get()
    print(f"İstek işleniyor: {kimlik}")

def api_istegi(id):
    istek_id.set(id)
    istek_islem()

api_istegi(1001)
api_istegi(1002)

# ----- __future__ (Gelecek Özellikler) -----
from __future__ import annotations  # ertelenmiş tip değerlendirme
# Python 3.7+ ile gelecek özellikleri şimdiden aktifleştirme

# ----- site ve site-packages -----
import site
print(f"Kullanıcı site-packages: {site.getusersitepackages()}")
print(f"Sistem site-packages: {site.getsitepackages()}")
```

## <a id="pydantic"></a>59. Pydantic ile Veri Doğrulama:

Pydantic, Python tip ipuçlarını kullanarak veri doğrulama ve ayar yönetimi sağlayan bir kütüphanedir. Özellikle FastAPI ile birlikte yaygınlaşmıştır.

```python
# pip install pydantic
from pydantic import BaseModel, Field, EmailStr, HttpUrl, validator
from datetime import datetime
from typing import Optional
from enum import Enum

# ----- Temel Model Tanımı -----
class Kullanici(BaseModel):
    ad: str
    email: str
    yas: int = Field(ge=0, le=150)  # 0-150 arası
    kayit_tarihi: datetime = Field(default_factory=datetime.now)
    aktif: bool = True

# Doğrulama ile oluşturma
kullanici = Kullanici(
    ad="Ali Yılmaz",
    email="ali@ornek.com",
    yas=30,
)
print(kullanici.model_dump())
# {'ad': 'Ali Yılmaz', 'email': 'ali@ornek.com', 'yas': 30,
#  'kayit_tarihi': datetime(...), 'aktif': True}

print(kullanici.model_dump_json())
# {"ad": "Ali Yılmaz", "email": "ali@ornek.com", ...}

# ----- Tip Dönüşümü (Coercion) -----
# Pydantic girdileri otomatik tipe dönüştürür
k2 = Kullanici(ad="Ayşe", email="ayse@site.com", yas="25")  # str -> int
print(type(k2.yas))  # <class 'int'>

# ----- Hata Yönetimi -----
from pydantic import ValidationError

try:
    hatali = Kullanici(ad="", email="gecersiz", yas=200)
except ValidationError as e:
    print(e)
    # 3 hata: ad boş, email geçersiz, yas > 150

# Doğrulama hatalarını json olarak alma
try:
    Kullanici(ad="Test", email="mail", yas=-5)
except ValidationError as e:
    hata_json = e.errors()
    for hata in hata_json:
        print(f"{hata['loc']}: {hata['msg']} ({hata['type']})")

# ----- Field ile Gelişmiş Tanım -----
class Urun(BaseModel):
    isim: str = Field(min_length=3, max_length=100)
    fiyat: float = Field(gt=0, description="TL cinsinden fiyat")
    stok: int = Field(default=0, ge=0)
    barkod: Optional[str] = Field(None, pattern=r"^\d{13}$")
    etiketler: list[str] = Field(default_factory=list)

    class Config:
        frozen = True  # salt okunur (hashable)

# ----- Özel Doğrulama (validator) -----
class SifreKutusu(BaseModel):
    sifre: str
    sifre_tekrar: str

    @validator("sifre")
    def sifre_guclu_mu(cls, v):
        if len(v) < 8:
            raise ValueError("Şifre en az 8 karakter olmalı")
        if not any(c.isupper() for c in v):
            raise ValueError("Büyük harf içermeli")
        if not any(c.isdigit() for c in v):
            raise ValueError("Rakam içermeli")
        return v

    @validator("sifre_tekrar")
    def sifreler_esit(cls, v, values):
        if "sifre" in values and v != values["sifre"]:
            raise ValueError("Şifreler eşleşmiyor")
        return v

# ----- Field Validator (alan bazlı) -----
from pydantic import field_validator

class KayitFormu(BaseModel):
    email: str
    yas: int

    @field_validator("email")
    @classmethod
    def email_gecerli(cls, v):
        if "@" not in v:
            raise ValueError("Geçersiz email")
        return v.lower().strip()

    @field_validator("yas")
    @classmethod
    def yas_kontrol(cls, v):
        if v < 18:
            raise ValueError("18 yaşından küçükler kayıt olamaz")
        return v

# ----- Model Validator (model bazlı) -----
from pydantic import model_validator

class Siparis(BaseModel):
    urunler: list[str]
    indirim_kodu: Optional[str] = None
    toplam: float = 0.0

    @model_validator(mode="after")
    def indirim_uygula(self):
        if self.indirim_kodu and self.toplam > 100:
            self.toplam *= 0.9  # %10 indirim
        return self

# ----- Enum ile Kullanım -----
class Durum(str, Enum):
    BEKLIYOR = "bekliyor"
    HAZIRLANIYOR = "hazırlanıyor"
    GONDERILDI = "gönderildi"
    TESLIM_EDILDI = "teslim_edildi"

class SiparisTakip(BaseModel):
    siparis_id: int
    durum: Durum
    tahmini_teslim: Optional[datetime] = None

s = SiparisTakip(siparis_id=1001, durum="hazırlanıyor")
print(s.durum)  # Durum.HAZIRLANIYOR

# ----- İç İçe Modeller -----
class Adres(BaseModel):
    sehir: str
    ilce: str
    sokak: str
    posta_kodu: str = Field(pattern=r"\d{5}")

class Musteri(BaseModel):
    ad: str
    adres: Adres
    onceki_siparisler: list[Siparis] = []

musteri = Musteri(
    ad="Ali",
    adres={"sehir": "İstanbul", "ilce": "Kadıköy",
           "sokak": "Bağdat Cd.", "posta_kodu": "34700"},
)
print(musteri.adres.sehir)  # İstanbul

# ----- Generic Modeller -----
from typing import Generic, TypeVar
from pydantic import BaseModel

T = TypeVar("T")

class ApiYanit(BaseModel, Generic[T]):
    basarili: bool
    veri: Optional[T] = None
    hata: Optional[str] = None

# Kullanım
class UrunSchema(BaseModel):
    id: int
    isim: str

yanit = ApiYanit[UrunSchema](
    basarili=True,
    veri={"id": 1, "isim": "Laptop"},
)
print(yanit.veri)  # UrunSchema(id=1, isim='Laptop')

# ----- Ayar Yönetimi (Settings) -----
from pydantic_settings import BaseSettings

class Ayar(BaseSettings):
    app_name: str = "MyApp"
    debug: bool = False
    database_url: str = Field(alias="db_url")
    secret_key: str

    class Config:
        env_file = ".env"
        env_file_encoding = "utf-8"

# .env dosyası:
#   app_name=ProductionApp
#   db_url=postgres://localhost/mydb
#   secret_key=s3cret!

# ayar = Ayar()  # env dosyasından okur
# print(ayar.database_url)

# ----- Deserialization (JSON -> Model) -----
json_veri = """
{
    "ad": "Veli",
    "email": "veli@ornek.com",
    "yas": 28,
    "aktif": false
}
"""
kullanici = Kullanici.model_validate_json(json_veri)
print(f"Merhaba {kullanici.ad}, yaş: {kullanici.yas}")

# ----- Serialization (Model -> Dict/JSON) -----
veri = kullanici.model_dump(exclude={"aktif"})
print(veri)  # {'ad': 'Veli', 'email': 'veli@ornek.com', 'yas': 28}

json_str = kullanici.model_dump_json(indent=2, include={"ad", "email"})
print(json_str)
```

## <a id="pytest-ileri"></a>60. pytest ile İleri Test Teknikleri:

pytest, Python'un en popüler test framework'üdür. Basit unit testlerden karmaşık entegrasyon testlerine kadar geniş bir yelpazede kullanılır.

```python
# pip install pytest pytest-cov
"""
# Çalıştırma:
pytest                          # tüm testleri bulup çalıştır
pytest test_dosyasi.py          # tek dosya
pytest test_dosyasi.py::test_adi  # tek test fonksiyonu
pytest -k "metod"               # adında "metod" geçen testler
pytest -x                        # ilk hatada dur
pytest --maxfail=3               # 3 hata sonrası dur
pytest -v                        # verbose
pytest --tb=short                # kısa traceback
pytest --tb=long                 # uzun traceback
pytest -s                        # print çıktılarını göster
pytest --lf                      # sadece başarısız testler
pytest --ff                      # başarısız testler önce
pytest --co                      # terminalde canlı çıktı
"""

# ----- Fixture Kullanımı -----
import pytest
from datetime import datetime

# Basit fixture
@pytest.fixture
def veri_seti():
    """Her test öncesi hazır veri sağlar"""
    return {"isim": "Ali", "sayilar": [1, 2, 3]}

def test_veri_kontrol(veri_seti):
    assert veri_seti["isim"] == "Ali"
    assert len(veri_seti["sayilar"]) == 3

# ----- Fixture Scopes (Kapsam) -----
@pytest.fixture(scope="function")  # varsayılan: her test için yeni
def fonk_fixture():
    return datetime.now()

@pytest.fixture(scope="module")  # modül boyunca aynı
def modul_fixture():
    return {"olusturuldu": datetime.now()}

@pytest.fixture(scope="session")  # tüm test oturumu boyunca
def session_fixture():
    return "sabit_deger"

# ----- Fixture Teardown (Temizlik) -----
@pytest.fixture
def dosya_olustur():
    # Setup
    dosya = open("test_gecici.txt", "w")
    dosya.write("test verisi")
    dosya.close()

    yield "test_gecici.txt"  # test burada çalışır

    # Teardown
    import os
    if os.path.exists("test_gecici.txt"):
        os.remove("test_gecici.txt")

def test_dosya_var(dosya_olustur):
    import os
    assert os.path.exists(dosya_olustur)

# ----- Parametrize (Parametrik Test) -----
@pytest.mark.parametrize("giris, beklenen", [
    (2, True),   # 2 asal
    (3, True),   # 3 asal
    (4, False),  # 4 asal değil
    (17, True),  # 17 asal
    (1, False),  # 1 asal değil
    (0, False),  # 0 asal değil
])
def test_asal_mi(giris, beklenen):
    def asal_mi(n):
        if n < 2:
            return False
        for i in range(2, int(n ** 0.5) + 1):
            if n % i == 0:
                return False
        return True
    assert asal_mi(giris) == beklenen

# Çoklu parametre kaynağı
@pytest.mark.parametrize("n", range(10))
@pytest.mark.parametrize("katsayi", [1, 2, 3])
def test_carpim(n, katsayi):
    assert n * katsayi == katsayi * n

# ----- Conftest.py ile Paylaşımlı Fixture'lar -----
"""
# conftest.py (test klasöründe):
import pytest

@pytest.fixture
def db_baglantisi():
    baglanti = veritabanina_baglan()
    yield baglanti
    baglanti.kapat()

@pytest.fixture
def test_kullanicisi(db_baglantisi):
    kullanici = Kullanici(ad="Test", email="test@ornek.com")
    db_baglantisi.ekle(kullanici)
    yield kullanici
    db_baglantisi.sil(kullanici)
"""

# ----- Markers (İşaretleyiciler) -----
@pytest.mark.slow  # yavaş testleri işaretle
def test_agir_islem():
    import time
    time.sleep(2)
    assert True

@pytest.mark.skip(reason="Henüz implemente edilmedi")
def test_gelecek():
    assert False

@pytest.mark.skipif(True, reason="Koşullu atlama")
def test_belirli_ortamda_atla():
    pass

# Sadece slow testleri çalıştır:
# pytest -m slow

# Slow olmayan testler:
# pytest -m "not slow"

# ----- Skip ve Xfail -----
import sys

@pytest.mark.skipif(sys.version_info < (3, 10), reason="3.10+ gerekli")
def test_yeni_ozellik():
    pass

@pytest.mark.xfail(reason="Bilinen hata")
def test_bilinen_hata():
    assert 1 + 1 == 3  # beklenen hata

@pytest.mark.xfail(strict=True)  # beklenmedik başarı da hata
def test_strict_xfail():
    assert 1 == 1  # xfail(strict) ile XPASS olarak işaretlenir (hata)

# ----- Monkeypatch (Mock) -----
def api_istegi(url):
    # Gerçek API çağrısı
    import httpx
    return httpx.get(url).json()

def test_api_mock(monkeypatch):
    def mock_get(url):
        return {"test": True, "url": url}

    # Fonksiyonu monkeypatch ile değiştir
    monkeypatch.setattr("httpx.get", mock_get)
    sonuc = api_istegi("https://ornek.com/api")
    assert sonuc["test"] is True

# ----- TemporaryFile ve Geçici Dizinler -----
def test_gecici_dosya(tmp_path):
    dosya = tmp_path / "test.txt"
    dosya.write_text("merhaba")
    assert dosya.read_text() == "merhaba"
    assert dosya.exists()

def test_gecici_dizin(tmpdir):
    import os
    os.chdir(tmpdir)
    yol = tmpdir.join("alt/klasor")
    yol.mkdir(parents=True)
    assert os.path.isdir(str(yol))

# ----- Fixture İç İçe Kullanımı -----
@pytest.fixture
def kullanici_verisi():
    return {"ad": "Ali", "soyad": "Yılmaz"}

@pytest.fixture
def tam_isim(kullanici_verisi):
    return f"{kullanici_verisi['ad']} {kullanici_verisi['soyad']}"

def test_isim_birlestirme(tam_isim):
    assert tam_isim == "Ali Yılmaz"

# ----- Fsmtplib ile Email Test -----
# pip install pytest-fsmtplib
# @pytest.mark.fsmtplib
# def test_email_gonderimi():
#     import smtplib
#     server = smtplib.SMTP("localhost", 1025)
#     server.sendmail("from@test.com", "to@test.com", "test")
#     assert True

# ----- Coverage (Kod Kapsama) -----
"""
# pytest-cov ile:
pytest --cov=myproject tests/
pytest --cov=myproject --cov-report=html  # HTML rapor
pytest --cov=myproject --cov-report=term-missing  # eksik satırlar

# .coveragerc ile yapılandırma:
[run]
source = myproject
omit = */tests/*,*/migrations/*

[report]
exclude_lines =
    pragma: no cover
    def __repr__
    raise NotImplementedError
"""

# ----- Async Test Desteği -----
# pip install pytest-asyncio

@pytest.mark.asyncio
async def test_async_islem():
    async def islem():
        return 42
    sonuc = await islem()
    assert sonuc == 42

@pytest.mark.asyncio
async def test_async_http(mocker):
    """Async HTTP test için aiohttp mock'u"""
    import aiohttp

    mock_yanit = {"kullanici": "Ali", "id": 1}

    async def mock_get(*args, **kwargs):
        return mock_yanit

    mocker.patch("aiohttp.ClientSession.get", return_value=mock_get)
    # Async fonksiyonu test et

# ----- Pytest Fixture ile DB Test -----
@pytest.fixture
def sqlite_bellek():
    """Her test için geçici SQLite veritabanı"""
    import sqlite3
    conn = sqlite3.connect(":memory:")
    conn.execute("CREATE TABLE test (id INTEGER, ad TEXT)")
    conn.execute("INSERT INTO test VALUES (1, 'test')")
    conn.commit()
    yield conn
    conn.close()

def test_veritabani(sqlite_bellek):
    cursor = sqlite_bellek.execute("SELECT * FROM test")
    assert cursor.fetchone() == (1, "test")

# ----- Yakalanan Çıktı (capsys) -----
def test_cikti_yakalama(capsys):
    print("Merhaba Dünya")
    yakalanan = capsys.readouterr()
    assert yakalanan.out == "Merhaba Dünya\n"

# ----- Giriş Yakalama (monkeypatch) -----
def test_kullanici_girdisi(monkeypatch):
    monkeypatch.setattr("builtins.input", lambda _: "Ali")
    isim = input("Adınız: ")
    assert isim == "Ali"

# ----- Pytest-xdist (Paralel Test) -----
"""
# pip install pytest-xdist
pytest -n auto        # CPU sayısı kadar paralel
pytest -n 4           # 4 paralel işçi
pytest -n auto --dist loadscope  # modül bazında dağıt
"""

# ----- Custom Assert Mesajları -----
def test_ozel_mesaj():
    x, y = 5, 10
    assert x < y, f"{x} < {y} olmalıydı!"  # hata: 5 < 10 olmalıydı!

# ----- Pytest Ordering (Sıralı Test) -----
# pip install pytest-ordering
# @pytest.mark.order(1)
# def test_ilk(): ...
# @pytest.mark.order(2)
# def test_sonra(): ...

# ----- Pytest Timeout -----
# pip install pytest-timeout
# @pytest.mark.timeout(5)
# def test_hizli(): ...
#
# Tüm testler için timeout:
# pytest --timeout=30

# ----- Fixture'ları Otomatik Kullanma (autouse) -----
@pytest.fixture(autouse=True)
def her_testte_calis():
    """Her test öncesi otomatik çalışır"""
    print("\n--- Test başlıyor ---")
    yield
    print("\n--- Test bitti ---")

def test_bir():
    assert 1 == 1

def test_iki():
    assert 2 == 2

# ----- Geçici Ortam Değişkeni (monkeypatch) -----
def test_env_degiskeni(monkeypatch):
    monkeypatch.setenv("SECRET_KEY", "test-key-123")
    import os
    assert os.environ["SECRET_KEY"] == "test-key-123"
    monkeypatch.delenv("SECRET_KEY", raising=False)
```

## <a id="numpy"></a>61. NumPy ile Sayısal Hesaplama:

NumPy, Python'da sayısal hesaplama ve dizi işlemleri için temel kütüphanedir. Çok boyutlu diziler ve matris işlemleri için optimize edilmiştir.

```python
# pip install numpy
import numpy as np

# ===== 1. Dizi (ndarray) Oluşturma =====

# Listeden dizi oluşturma
dizi = np.array([1, 2, 3, 4, 5])
print(dizi)         # [1 2 3 4 5]
print(dizi.shape)   # (5,)
print(dizi.ndim)    # 1 (boyut sayısı)
print(dizi.dtype)   # int64

# 2 boyutlu dizi (matris)
matris = np.array([[1, 2, 3], [4, 5, 6]])
print(matris.shape)  # (2, 3)

# 3 boyutlu dizi
ucd = np.array([[[1, 2], [3, 4]], [[5, 6], [7, 8]]])
print(ucd.shape)     # (2, 2, 2)

# ----- Hazır Diziler -----
sifir = np.zeros((3, 4))        # 3x4 sıfır matrisi
bir = np.ones((2, 3))           # 2x3 bir matrisi
bos = np.empty((2, 2))          # 2x2 başlatılmamış (hızlı)
birim = np.eye(4)               # 4x4 birim matris
full = np.full((3, 3), 7)       # 3x3 tüm elemanları 7

# ----- Sıralı Diziler -----
aralik = np.arange(10)                  # [0 1 2 ... 9]
aralik2 = np.arange(2, 10, 2)           # [2 4 6 8]
esit_arali = np.linspace(0, 1, 5)       # [0. 0.25 0.5 0.75 1.]
log_aralik = np.logspace(0, 3, 4)       # [1. 10. 100. 1000.]

# ----- Rastgele Diziler -----
rastgele = np.random.random((3, 3))     # [0, 1) uniform
normal = np.random.normal(0, 1, 1000)   # normal dağılım (ortalama=0, std=1)
tamsayi = np.random.randint(0, 10, 20)  # 0-10 arası 20 tamsayı
seedli = np.random.seed(42)             # tekrarlanabilir sonuçlar

# ----- Veri Tipleri ile Dizi -----
dizi_int = np.array([1, 2, 3], dtype=np.int32)
dizi_float = np.array([1, 2, 3], dtype=np.float64)
dizi_kompleks = np.array([1+2j, 3+4j], dtype=np.complex128)
dizi_bool = np.array([True, False, True])

# ===== 2. Dizi İndeksleme ve Dilimleme =====

dizi = np.array([10, 20, 30, 40, 50])

print(dizi[0])      # 10
print(dizi[-1])     # 50
print(dizi[1:4])    # [20 30 40]
print(dizi[::-1])   # [50 40 30 20 10]

# 2 boyutlu indeksleme
m = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
print(m[0, 1])       # 2 (satır 0, sütun 1)
print(m[1])          # [4 5 6] (2. satır)
print(m[:, 0])       # [1 4 7] (1. sütun)
print(m[0:2, 1:3])   # [[2 3] [5 6]] (alt matris)

# ----- Fancy Indexing -----
dizi = np.array([10, 20, 30, 40, 50])
indeksler = np.array([0, 2, 4])
print(dizi[indeksler])   # [10 30 50]

# Boolean indexing
maske = dizi > 25
print(maske)             # [False False  True  True  True]
print(dizi[maske])       # [30 40 50]

# ===== 3. Dizi Manipülasyonu =====

# ----- Shape Değiştirme -----
dizi = np.arange(12)
print(dizi.reshape(3, 4))       # 3x4 matris
print(dizi.reshape(2, 2, 3))    # 2x2x3 3-boyutlu
print(dizi.reshape(-1, 4))      # -1: otomatik hesapla -> (3, 4)

# Düzleştirme
m = np.array([[1, 2, 3], [4, 5, 6]])
print(m.flatten())  # [1 2 3 4 5 6] (kopya)
print(m.ravel())    # [1 2 3 4 5 6] (görüntü, kopya değil)

# ----- Birleştirme ve Ayırma -----
a = np.array([1, 2, 3])
b = np.array([4, 5, 6])
print(np.concatenate([a, b]))        # [1 2 3 4 5 6]

m1 = np.array([[1, 2], [3, 4]])
m2 = np.array([[5, 6], [7, 8]])
print(np.vstack((m1, m2)))           # dikey birleştirme
print(np.hstack((m1, m2)))           # yatay birleştirme

# Ayırma
dizi = np.arange(12)
print(np.split(dizi, 3))             # 3 parçaya ayır
print(np.vsplit(np.arange(12).reshape(6,2), 3))  # dikey ayır
print(np.hsplit(np.arange(12).reshape(3,4), 2))  # yatay ayır

# ----- Yeni Eksen (Dimension) Ekleme -----
dizi = np.array([1, 2, 3])
print(dizi[:, np.newaxis])   # sütun vektörü: (3, 1)
print(dizi[np.newaxis, :])   # satır vektörü: (1, 3)

# ===== 4. Matematiksel İşlemler =====

dizi = np.array([1, 2, 3, 4, 5])

# Element-wise işlemler
print(dizi + 10)          # [11 12 13 14 15]
print(dizi * 2)           # [2 4 6 8 10]
print(dizi ** 2)          # [1 4 9 16 25]
print(np.sqrt(dizi))      # karekök
print(np.exp(dizi))       # e^x
print(np.log(dizi))       # doğal logaritma
print(np.sin(dizi))       # trigonometrik

# ----- Toplamsal İstatistikler -----
print(dizi.sum())         # 15
print(dizi.mean())        # 3.0
print(dizi.std())         # standart sapma
print(dizi.var())         # varyans
print(dizi.min())         # 1
print(dizi.max())         # 5
print(dizi.argmin())      # 0 (minimum indeksi)
print(dizi.argmax())      # 4 (maksimum indeksi)
print(dizi.cumsum())      # kümülatif toplam: [1 3 6 10 15]

# ----- Eksenler Boyunca İşlemler -----
m = np.array([[1, 2, 3], [4, 5, 6]])
print(m.sum(axis=0))      # sütun toplamı: [5 7 9]
print(m.sum(axis=1))      # satır toplamı: [6 15]
print(m.mean(axis=0))     # sütun ortalaması: [2.5 3.5 4.5]

# ===== 5. Broadcasting =====

# Farklı şekillerdeki dizilerle işlem yapma
a = np.array([[1, 2, 3], [4, 5, 6], [7, 8, 9]])
b = np.array([1, 0, 1])
print(a + b)
# [[2 2 4]
#  [5 5 7]
#  [8 8 10]]

# Skaler broadcasting
print(a * 2)
# [[2 4 6]
#  [8 10 12]
#  [14 16 18]]

# ===== 6. Lineer Cebir (linalg) =====

# ----- Matris Çarpımı -----
a = np.array([[1, 2], [3, 4]])
b = np.array([[5, 6], [7, 8]])
print(a @ b)                     # matris çarpımı (Python 3.5+)
print(np.dot(a, b))              # aynı işlem
print(np.matmul(a, b))           # aynı işlem

# ----- Determinant, Ters, Özdeğerler -----
mat = np.array([[4, 7], [2, 6]])
print(np.linalg.det(mat))        # determinant: 10.0

ters = np.linalg.inv(mat)
print(ters)
print(mat @ ters)                # birim matris (~)

ozdeger, ozvektor = np.linalg.eig(mat)
print(f"Özdeğerler: {ozdeger}")
print(f"Özvektörler:\n{ozvektor}")

# ----- Lineer Denklem Çözümü -----
# 3x + 2y = 12
# 4x -  y =  5
A = np.array([[3, 2], [4, -1]])
b = np.array([12, 5])
cozum = np.linalg.solve(A, b)
print(f"x={cozum[0]}, y={cozum[1]}")  # x=2.0, y=3.0

# ----- Norm ve Mesafe -----
v = np.array([3, 4])
print(np.linalg.norm(v))         # 5.0 (Öklid normu)
print(np.linalg.norm(v, 1))      # 7.0 (L1 norm)
print(np.linalg.norm(v, np.inf)) # 4.0 (sonsuz norm)

# ===== 7. Boolean İşlemler ve Filtreleme =====

dizi = np.array([1, 2, 3, 4, 5, 6, 7, 8, 9, 10])

# Koşullu filtreleme
cift = dizi[dizi % 2 == 0]
print(cift)                      # [2 4 6 8 10]

# Çoklu koşul
filtre = (dizi > 3) & (dizi < 8)
print(dizi[filtre])              # [4 5 6 7]

# np.where: koşullu seçim
print(np.where(dizi > 5, dizi, 0))   # >5 ise kendisi, değilse 0
# [0 0 0 0 0 6 7 8 9 10]

# np.clip: değerleri sınırla
print(np.clip(dizi, 3, 7))
# [3 3 3 4 5 6 7 7 7 7]

# ===== 8. Dizi Karşılaştırma =====

a = np.array([1, 2, 3])
b = np.array([1, 2, 4])
print(a == b)              # [ True  True False]
print(np.array_equal(a, b))       # False
print(np.allclose(a, b))          # True (küçük farklara toleranslı)

# Kayan nokta karşılaştırması
x = np.array([0.1 + 0.2, 0.3])
y = np.array([0.3, 0.3])
print(np.allclose(x, y))          # True
print(x == y)                     # False (float hassasiyeti!)

# ===== 9. Kopyalama (Copy vs View) =====

dizi = np.array([1, 2, 3, 4, 5])

# View: orijinal veriyi paylaşır
goruntu = dizi[1:4]
goruntu[0] = 99
print(dizi)           # [1 99 3 4 5] (orijinal değişti!)

# Copy: bağımsız kopya
kopya = dizi[1:4].copy()
kopya[0] = 0
print(dizi)           # [1 99 3 4 5] (orijinal değişmedi!)

# ===== 10. Sort ve Unique =====

dizi = np.array([3, 1, 2, 3, 1, 4, 2])
print(np.sort(dizi))              # [1 1 2 2 3 3 4]
print(np.unique(dizi))            # [1 2 3 4]
print(np.unique(dizi, return_counts=True))
# (array([1, 2, 3, 4]), array([2, 2, 2, 1]))

# Sıralama indeksleri
print(np.argsort(dizi))           # [1 4 0 2 6 3 5]

# ===== 11. Dosya Okuma/Yazma =====

# ----- .npy formatı (NumPy ikili) -----
dizi = np.array([1, 2, 3, 4, 5])
np.save("dizi.npy", dizi)
yuklenen = np.load("dizi.npy")
print(yuklenen)

# ----- CSV/metin dosyası -----
# np.savetxt("veri.csv", dizi, delimiter=",")
# yuklenen = np.loadtxt("veri.csv", delimiter=",")

# ----- Birden çok dizi (sıkıştırılmış) -----
# np.savez("veriler.npz", a=dizi, b=matris)
# veriler = np.load("veriler.npz")
# print(veriler["a"])

# ===== 12. Pratik Örnekler =====

# ----- İstatistiksel Analiz -----
veri = np.random.normal(170, 10, 1000)  # boy ölçüleri (cm)
print(f"Ortalama: {veri.mean():.1f} cm")
print(f"Standart sapma: {veri.std():.1f} cm")
print(f"Min: {veri.min():.1f}, Max: {veri.max():.1f}")
print(f"%25: {np.percentile(veri, 25):.1f}")
print(f"%75: {np.percentile(veri, 75):.1f}")

# ----- Görüntü İşleme (temel) -----
# Gri tonlamalı görüntü: 2D dizi
goruntu = np.random.randint(0, 256, (100, 100), dtype=np.uint8)
print(f"Görüntü boyutu: {goruntu.shape}")
print(f"Parlaklık: {goruntu.mean():.1f} / 255")

# ----- Veri Normalizasyonu -----
veri = np.array([10, 20, 30, 40, 50])
normalize = (veri - veri.min()) / (veri.max() - veri.min())
print(f"Normalize: {normalize}")  # [0. 0.25 0.5 0.75 1.]

# Standardizasyon (Z-score)
standart = (veri - veri.mean()) / veri.std()
print(f"Z-skor: {standart}")

# ----- Mesafe Matrisi -----
noktalar = np.random.rand(5, 2)  # 5 nokta, 2 boyutlu
fark = noktalar[:, np.newaxis, :] - noktalar[np.newaxis, :, :]
mesafeler = np.sqrt((fark ** 2).sum(axis=-1))
print(f"Mesafe matrisi ({mesafeler.shape}):")
print(np.round(mesafeler, 2))
```

---

## <a id="pandas"></a>62. Pandas ile Veri Analizi

Pandas, Python'da veri analizi ve manipülasyonu için en popüler kütüphanedir. DataFrame ve Series adlı iki temel veri yapısı sunar.

### Kurulum

```bash
pip install pandas
```

```python
import pandas as pd
import numpy as np
```

### Series (Tek Boyutlu)

```python
s = pd.Series([10, 20, 30, 40], index=["a", "b", "c", "d"])
print(s["b"])        # 20
print(s.mean())      # 25.0

# Sözlükten Series
notlar = pd.Series({"Matematik": 85, "Fizik": 90, "Kimya": 78})
print(notlar)
```

### DataFrame (İki Boyutlu)

DataFrame, satır ve sütunlardan oluşan tablo yapısıdır.

```python
# Sözlükten DataFrame
data = {
    "İsim":    ["Ali", "Ayşe", "Mehmet", "Zeynep"],
    "Yaş":     [20, 21, 19, 22],
    "Not":     [85, 92, 78, 95],
    "Şehir":   ["İstanbul", "Ankara", "İzmir", "İstanbul"]
}
df = pd.DataFrame(data)
print(df)
```

### Veri Okuma/Yazma

```python
# CSV
df = pd.read_csv("veri.csv")
df.to_csv("cikti.csv", index=False)

# Excel (openpyxl veya xlrd gerekebilir)
df = pd.read_excel("veri.xlsx", sheet_name="Sayfa1")
df.to_excel("cikti.xlsx", index=False)

# JSON
df = pd.read_json("veri.json")
df.to_json("cikti.json", orient="records")

# Clipboard (panodan yapıştır)
df = pd.read_clipboard()
```

### Veriyi Tanıma

```python
df.head(10)          # İlk 10 satır
df.tail(5)           # Son 5 satır
df.info()            # Sütun tipleri, null sayısı
df.shape             # (satır, sütun)
df.columns           # Sütun isimleri
df.index             # Satır indeksleri
df.describe()        # İstatistiksel özet
df.dtypes            # Sütun tipleri
df.nunique()         # Benzersiz değer sayıları
df.value_counts()    # Tekrarlı sayımlar
```

### Sütun Seçme ve İşlemler

```python
# Tek sütun
df["İsim"]
df.İsim              # Aynı işlem (boşluksuz isimlerde)

# Birden çok sütun
df[["İsim", "Not"]]

# Yeni sütun ekleme
df["Geçti"] = df["Not"] >= 60
df["Yaş_kat"] = df["Yaş"] * 365

# Sütun silme
df.drop("Yaş_kat", axis=1, inplace=True)
df.drop(columns=["Yaş_kat"], inplace=True)

# Sütun yeniden adlandırma
df.rename(columns={"Not": "Puan", "İsim": "Ad"}, inplace=True)
```

### Satır Seçme (loc / iloc)

```python
# Etiket bazlı
df.loc[0]                # 0. indeksli satır (Series döner)
df.loc[[0, 2, 3]]        # Belirli satırlar
df.loc[1:3]              # 1-3 arası (3 dahil)
df.loc[df["Not"] > 80]   # Koşullu seçim

# Konum bazlı
df.iloc[0]               # İlk satır
df.iloc[1:4]              # 1-3 arası
df.iloc[:, 0:2]           # İlk 2 sütun
df.iloc[[0, 3], [1, 2]]  # Belirli satır ve sütunlar
```

### Filtreleme

```python
# Boolean indexing
df[df["Not"] >= 90]
df[(df["Not"] >= 80) & (df["Şehir"] == "İstanbul")]
df[df["İsim"].str.startswith("A")]

# isin
df[df["Şehir"].isin(["İstanbul", "Ankara"])]

# between
df[df["Not"].between(80, 100)]

# query (string ifade ile)
df.query("Not >= 80 and Şehir == 'İstanbul'")

# Benzersiz değerler
df["Şehir"].unique()
df["Şehir"].nunique()   # Benzersiz sayısı
```

### Sıralama

```python
df.sort_values("Not", ascending=False)
df.sort_values(["Şehir", "Not"], ascending=[True, False])
df.sort_index()
```

### Eksik Veriler (NaN)

```python
df.isnull().sum()        # Her sütundaki NaN sayısı
df.isna().sum()          # Aynı işlem
df.notnull().sum()       # Null olmayanlar

# NaN içeren satırları sil
df.dropna()
df.dropna(thresh=2)      # En az 2 non-NaN değeri olanları tut
df.dropna(subset=["Not"])

# NaN doldurma
df.fillna(0)             # Sıfır ile doldur
df.fillna(method="ffill")  # Bir önceki değerle doldur
df.fillna(method="bfill")  # Bir sonraki değerle doldur
df["Not"].fillna(df["Not"].mean())  # Ortalama ile doldur
df.interpolate()         # Doğrusal enterpolasyon
```

### GroupBy (Gruplama)

```python
# Tek sütuna göre gruplama
df.groupby("Şehir")["Not"].mean()
df.groupby("Şehir").agg({"Not": ["mean", "max", "min", "count"]})

# Çoklu grup
df.groupby(["Şehir", "Geçti"])["Not"].mean()

# apply ile özel fonksiyon
df.groupby("Şehir")["Not"].apply(lambda x: x.max() - x.min())

# Transform
df["not_ortalama"] = df.groupby("Şehir")["Not"].transform("mean")

# Filtre (group bazlı)
df.groupby("Şehir").filter(lambda g: g["Not"].mean() > 80)
```

### Birleştirme İşlemleri

```python
# Dikey birleştirme (satır ekleme)
df1 = pd.DataFrame({"A": [1, 2], "B": [3, 4]})
df2 = pd.DataFrame({"A": [5, 6], "B": [7, 8]})
pd.concat([df1, df2], ignore_index=True)

# Yatay birleştirme (sütun ekleme)
pd.concat([df1, df2], axis=1)

# Merge (SQL JOIN gibi)
ogrenciler = pd.DataFrame({
    "id": [1, 2, 3], "ad": ["Ali", "Ayşe", "Mehmet"]
})
notlar = pd.DataFrame({
    "id": [1, 2, 4], "not": [85, 90, 75]
})

pd.merge(ogrenciler, notlar, on="id", how="inner")   # Sadece eşleşenler
pd.merge(ogrenciler, notlar, on="id", how="left")    # Tüm öğrenciler
pd.merge(ogrenciler, notlar, on="id", how="outer")   # Tüm kayıtlar
pd.merge(ogrenciler, notlar, on="id", how="right")   # Tüm notlar
```

### apply ve map (Dönüşümler)

```python
# map: sözlük bazlı dönüşüm
df["şehir_kodu"] = df["Şehir"].map({"İstanbul": 34, "Ankara": 6, "İzmir": 35})

# apply: sütun bazlı fonksiyon
df["Not"] = df["Not"].apply(lambda x: min(x + 5, 100))

# apply: satır bazlı
df["ortalama"] = df.apply(lambda row: (row["Not"] + row["Yaş"]) / 2, axis=1)

# applymap: tüm DataFrame (tüm değerler)
df[["Not", "Yaş"]].applymap(lambda x: f"{x:.1f}")

# replace
df["Şehir"].replace("İstanbul", "IST")
```

### Pivot Table ve Cross Tab

```python
# Pivot tablo
df.pivot_table(values="Not", index="Şehir", columns="Geçti", aggfunc="mean", fill_value=0)

# Cross tab (çapraz tablo)
pd.crosstab(df["Şehir"], df["Geçti"], margins=True)

# Stack / Unstack
df.stack()    # Sütunları satıra çevir
df.unstack()  # Satırları sütuna çevir
```

### Zaman Serileri

```python
# Tarih dönüşümü
df["tarih"] = pd.to_datetime(df["tarih"])

# İndeks olarak ayarla
df.set_index("tarih", inplace=True)

# Zaman bazlı filtreleme
df["2024-01": "2024-06"]

# Yeniden örnekleme (resample)
df.resample("M").mean()       # Aylık ortalama
df.resample("W").sum()        # Haftalık toplam
df.resample("D").ffill()      # Günlük doldurma

# Shift (kaydırma)
df["dünkü_değer"] = df["değer"].shift(1)
df["değişim"] = df["değer"] - df["değer"].shift(1)

# Rolling pencere
df["7gün_ortalama"] = df["değer"].rolling(window=7).mean()
df["kümülatif"] = df["değer"].cumsum()
```

### Temel Görselleştirme

```python
# Çizgi grafiği
df.plot(kind="line", y="Not", title="Not Dağılımı")

# Bar grafiği
df.groupby("Şehir")["Not"].mean().plot(kind="bar")
df.plot(kind="bar", x="İsim", y="Not")

# Histogram
df["Not"].plot(kind="hist", bins=10)

# Kutu grafiği (box plot)
df[["Not", "Yaş"]].plot(kind="box")

# Scatter plot
df.plot(kind="scatter", x="Yaş", y="Not")

# Pasta grafiği
df["Şehir"].value_counts().plot(kind="pie", autopct="%1.1f%%")

# Yoğunluk grafiği
df["Not"].plot(kind="kde")
```

### Hafıza ve Performans İpuçları

```python
# Veri tiplerini optimize et
df["Yaş"] = df["Yaş"].astype("int8")       # int64 yerine
df["Not"] = df["Not"].astype("float32")    # float64 yerine

# Kategorik veri
df["Şehir"] = df["Şehir"].astype("category")

# Bellek kullanımı
print(df.memory_usage(deep=True))

# Büyük CSV okuma (parça parça)
for chunk in pd.read_csv("buyuk.csv", chunksize=10000):
    process(chunk)

# query() filtrelemesi Python ifadelerinden daha hızlıdır
# apply() yerine vektörel işlemler tercih edilir

# Paralel işlem (Swifter)
# pip install swifter
# df["yeni"] = df["sütun"].swifter.apply(fonksiyon)
```

### Pratik Örnek - Öğrenci Analizi

```python
import pandas as pd
import numpy as np

# Örnek veri oluştur
np.random.seed(42)
data = {
    "öğrenci": [f"Öğrenci_{i}" for i in range(100)],
    "sınıf": np.random.choice(["9A", "9B", "10A", "10B", "11A"], 100),
    "matematik": np.random.randint(30, 100, 100),
    "fizik": np.random.randint(30, 100, 100),
    "kimya": np.random.randint(30, 100, 100),
    "devamsızlık": np.random.randint(0, 20, 100)
}
df = pd.DataFrame(data)

# Ortalama hesapla
df["ortalama"] = df[["matematik", "fizik", "kimya"]].mean(axis=1).round(1)

# Geçme durumu
df["durum"] = df["ortalama"].apply(lambda x: "Geçti" if x >= 50 else "Kaldı")

# Sınıf bazlı istatistikler
print("=== Sınıf Bazlı Ortalamalar ===")
print(df.groupby("sınıf")["ortalama"].agg(["mean", "max", "min"]))

# En başarılı 10 öğrenci
print("\n=== İlk 10 ===")
print(df.nlargest(10, "ortalama")[["öğrenci", "sınıf", "ortalama"]])

# Devamsızlık analizi
print("\n=== Devamsızlık > 10 ===")
print(df[df["devamsızlık"] > 10][["öğrenci", "sınıf", "devamsızlık"]])

# Sınıf geçme yüzdeleri
print("\n=== Geçme Yüzdeleri ===")
gecme = df.groupby("sınıf")["durum"].value_counts(normalize=True).mul(100)
print(gecme)

# Not dağılımı (histogram)
df["ortalama"].plot(kind="hist", bins=10, title="Not Dağılımı")

# CSV'ye kaydet
df.to_csv("ogrenci_analizi.csv", index=False)
print("\n✅ ogrenci_analizi.csv kaydedildi")

# Excel'e kaydet (farklı sayfalar)
with pd.ExcelWriter("ogrenci_raporu.xlsx") as writer:
    df.to_excel(writer, sheet_name="Tüm Veri", index=False)
    df.groupby("sınıf")["ortalama"].mean().to_excel(writer, sheet_name="Sınıf Ortalamaları")
```

### Özet

| İşlem | Metod |
|---|---|
| Veri okuma | `read_csv()`, `read_excel()`, `read_json()` |
| Veri yazma | `to_csv()`, `to_excel()`, `to_json()` |
| İlk satırlar | `head(n)` |
| İstatistik | `describe()` |
| Filtreleme | `df[kosul]`, `query()` |
| Gruplama | `groupby()`, `pivot_table()` |
| Eksik veri | `dropna()`, `fillna()`, `interpolate()` |
| Birleştirme | `merge()`, `concat()`, `join()` |
| Görselleştirme | `plot()` |
| Sıralama | `sort_values()`, `sort_index()` |

Pandas, veri bilimi iş akışının temelidir. NumPy ile birlikte öğrenildiğinde veri analizi için güçlü bir araç seti oluşturur.

---

## 🎉 Tebrikler! Python Yolculuğunda İlerlediniz!

Bu 62 bölüm boyunca Python'un temellerinden ileri konulara kadar geniş bir yelpazede bilgi edindiniz.
**Ama asıl öğrenme şimdi başlıyor.** Kod yazarak, hata yaparak, projeler geliştirerek öğreneceksiniz.

### Size En Uygun Yolu Seçin

| İlgi Alanınız | Bu Konulara Bakın | İlk Projeniz |
|---------------|-------------------|--------------|
| **🌐 Web Geliştirme** | Flask (B39) → FastAPI (B40) → Django (B41) | Kişisel blog |
| **📊 Veri Bilimi** | NumPy (B61) → Pandas (B62) → Matplotlib → Scikit-learn | Veri analizi aracı |
| **🤖 Yapay Zeka** | NumPy → Pandas → Scikit-learn → TensorFlow/PyTorch | Görüntü sınıflandırma |
| **⚙️ Otomasyon** | subprocess (B43) → Web Scraping (B38) → threading (B28) | Dosya düzenleyici |
| **📱 Masaüstü Uygulama** | Tkinter → PyQt → GTK (PyGObject) | Hesap makinesi |
| **🎮 Oyun Geliştirme** | Pygame → Arcade | Basit bir oyun |

### Şimdi Ne Yapmalısınız?

1. **Küçük başlayın**: Günde 15-30 dakika kod yazın
2. **Proje yapın**: Yukarıdaki proje fikirlerinden birini seçin
3. **Hata yapın**: Hatalar öğrenme sürecinin en değerli parçasıdır
4. **Topluluğa katılın**: Python Türkiye grupları, Stack Overflow, GitHub
5. **Açık kaynağa katkı yapın**: Küçük bir bug fix ile başlayın

### Önerilen Kaynaklar

- [Python Resmi Dokümantasyonu](https://docs.python.org/3/)
- [Real Python](https://realpython.com/) — İngilizce, çok kaliteli eğitimler
- [PyPI](https://pypi.org/) — Python Paket İndeksi (500.000+ paket)
- [Awesome Python](https://github.com/vinta/awesome-python) — Derlenmiş kaynak listesi
- [Python Packaging Guide](https://packaging.python.org/) — Paketleme rehberi

**Unutmayın**: Her Python geliştiricisi bir zamanlar sizinle aynı noktadaydı. Yazmaya devam edin! 🚀
