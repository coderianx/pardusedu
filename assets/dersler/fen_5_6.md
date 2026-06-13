# 5. SINIF FEN BİLİMLERİ — 2. DÖNEM 2. ÜNİTE: YAŞAMIMIZDAKİ ELEKTRİK

---

## 1. DEVRE ELEMANLARININ SEMBOLLERİ VE DEVRE ŞEMASI

### 1.1. Elektrik Nedir?

Sevgili öğrenciler, düşünsenize — bir elektrik düğmesine basıyorsunuz ve odanız aydınlanıyor. Telefonunuzu şarj ediyorsunuz ve ekran ışıklanıyor. Peki bu sihirli güç olan elektrik nedir? Gelin birlikte keşfedelim.

**Elektrik:** Elektrik yüklerinin hareketi sonucu oluşan bir enerji türüdür. Günlük hayatta en çok kullandığımız enerji şeklidir.

**Elektrik devresi:** Elektrik enerjisinin bir kaynaktan çıkıp, iletken teller aracılığıyla bir yükü (ampul, motor, zil) çalıştırdıktan sonra tekrar kaynağa döndüğü kapalı yola **elektrik devresi** denir.

### 1.2. Devre Elemanları ve Sembolleri

Bir elektrik devresini oluşturan temel elemanlar vardır. Her elemanın uluslararası kabul görmüş bir sembolü bulunur. Semboller sayesinde devreler karmaşık çizimler olmadan basit şemalarla gösterilebilir.

| Devre Elemanı | Görevi | Sembol |
|--------------|--------|--------|
| **Pil (Batarya)** | Devreye elektrik enerjisi sağlar. Kimyasal enerjiyi elektrik enerjisine çevirir. | `──┤ ├──` (kısa çizgi negatif, uzun çizgi pozitif) |
| **Bağlantı kablosu** | Elektrik enerjisini devrede taşır (iletir). | `──────` (düz çizgi) |
| **Ampul** | Elektrik enerjisini ışık ve ısı enerjisine çevirir. | `──⨁──` (içinde çarpı olan daire) |
| **Anahtar** | Devreyi açıp kapamaya yarar. Devreden elektrik akımını geçirir veya keser. | `──o/──` (açık anahtar), `──o──` (kapalı anahtar) |
| **Duy** | Ampulün devreye bağlanmasını sağlar. | `──⨀──` |
| **Motor** | Elektrik enerjisini hareket (kinetik) enerjisine çevirir. | `──Ⓜ──` (içinde M olan daire) |
| **Zil** | Elektrik enerjisini ses enerjisine çevirir. | |

**Önemli not:** Pil sembolünde uzun çizgi (+), kısa çizgi (-) kutbunu gösterir. Ampul sembolü içinde genellikle bir "x" veya daire içinde çarpı işareti kullanılır.

### 1.3. Devre Şeması

**Devre şeması:** Bir elektrik devresindeki elemanların, uluslararası semboller kullanılarak kâğıt üzerinde gösterilmesidir.

**Devre şeması çizim kuralları:**
- Her devre elemanı kendi sembolüyle gösterilir.
- Kablolar düz çizgilerle gösterilir.
- Kabloların kesiştiği noktalara nokta konur (bağlantı varsa).
- Semboller karmaşık değil, basit ve anlaşılır olmalıdır.
- Devre şeması, gerçek devrenin çalışma mantığını yansıtmalıdır.

**Basit bir devre şeması örneği:**
```
    ──┤ ├──────⨁──────o/──
    │                    │
    └────────────────────┘
```
Yukarıdaki şemada: Pil → kablo → ampul → anahtar → kablo → pile dönen kapalı bir devre görülmektedir.

### 1.4. Devre Elemanlarının Görevleri Detaylı Tablo

| Eleman | Sembol | Görevi | Enerji Dönüşümü |
|--------|--------|--------|----------------|
| Pil | `─┤├─` | Enerji kaynağı | Kimyasal → Elektrik |
| Kablo | `───` | Enerji iletimi | İletim |
| Ampul | `⨁` | Işık verir | Elektrik → Işık + Isı |
| Anahtar | `o/` veya `o` | Akımı kontrol eder | Kontrol |
| Motor | `Ⓜ` | Dönme hareketi sağlar | Elektrik → Hareket |
| Zil | | Ses üretir | Elektrik → Ses |

---

## 2. BASİT ELEKTRİK DEVRESİ

### 2.1. Basit Elektrik Devresi Nedir?

**Basit elektrik devresi:** Bir pil, bir ampul, bir anahtar ve bağlantı kablolarından oluşan en temel elektrik devresidir.

**Basit bir elektrik devresinin elemanları:**
1. **Pil** — Enerji kaynağı
2. **Ampul** — Işık veren yük
3. **Anahtar** — Devreyi açıp kapayan kontrol elemanı
4. **Bağlantı kabloları** — Enerjiyi taşıyan iletkenler

### 2.2. Devrenin Çalışması İçin Gerekenler

Bir elektrik devresinin çalışabilmesi için:

**1. Kapalı devre olmalıdır:**
- Devre, pilin bir kutbundan çıkıp diğer kutbuna ulaşan kesintisiz bir yol olmalıdır.
- Anahtar kapalı konumda olmalıdır.
- Tüm bağlantılar sağlam olmalıdır.

**2. Devrede iletken malzeme kullanılmalıdır:**
- Kablolar iletken maddeden (genellikle bakır) yapılmış olmalıdır.
- Bağlantılar doğru yapılmalıdır.

**3. Pil yeterli güçte olmalıdır:**
- Pilin voltajı, ampulün çalışması için yeterli olmalıdır.

### 2.3. Devre Çeşitleri

| Devre Türü | Açıklama | Ampul Durumu |
|-----------|---------|-------------|
| **Açık devre** | Devrede kopukluk vardır, elektrik akımı geçmez | Ampul yanmaz |
| **Kapalı devre** | Devrede kopukluk yoktur, elektrik akımı geçer | Ampul yanar |
| **Kısa devre** | Akım, yük (ampul) üzerinden geçmeden doğrudan kaynağa döner | Ampul yanmaz (pil zarar görebilir) |

**Anahtarın görevi:**
- Anahtar **açık** konumda → Devre açık → Ampul **yanmaz**
- Anahtar **kapalı** konumda → Devre kapalı → Ampul **yanar**

### 2.4. Elektrik Akımının Yönü

Elektrik akımı, pilin **pozitif (+) kutbundan negatif (-) kutbuna** doğru akar (geleneksel akım yönü). Ancak elektronlar aslında negatif (-) kutuptan pozitif (+) kutba doğru hareket eder.

**Akımın izlediği yol:**
1. Pilin pozitif (+) kutbundan çıkar.
2. Kablo üzerinden ampule ulaşır.
3. Ampulden geçer (ampul yanar).
4. Kabloyla devam eder.
5. Anahtardan geçer.
6. Pilin negatif (-) kutbuna geri döner.

### 2.5. İletken ve Yalıtkan Maddeler

**İletken maddeler:** Elektrik akımını ileten maddelerdir.

| İletken Madde | Kullanım Alanı |
|-------------|---------------|
| Bakır | Elektrik kabloları |
| Alüminyum | Elektrik telleri |
| Altın | Elektronik devreler |
| Gümüş | Özel devreler |
| Demir, çelik | Bazı bağlantı elemanları |
| Tuzlu su | İletken sıvı |
| Grafit (kalem ucu) | Basit devreler |

**Yalıtkan maddeler:** Elektrik akımını iletmeyen maddelerdir.

| Yalıtkan Madde | Kullanım Alanı |
|--------------|---------------|
| Plastik | Kablo kaplaması, priz |
| Kauçuk | Eldiven, kablo izolasyonu |
| Cam | Elektrik direklerinde |
| Tahta | Elektrikçi merdiveni |
| Kumaş | Bazı kabloların dış kaplaması |
| Hava | Havai hatlarda yalıtım |

**Önemli:** Kabloların iç kısmı iletken (bakır), dış kısmı yalıtkandır (plastik). Bu sayede elektrik akımı kablonun içinde kalır, dışarıya kaçmaz ve bize zarar vermez.

---

## 3. AMPUL PARLAKLIĞINI ETKİLEYEN DEĞİŞKENLER

### 3.1. Ampul Parlaklığı Nedir?

**Ampul parlaklığı:** Ampulün yaydığı ışık miktarıdır. Bir ampul ne kadar parlaksa o kadar fazla ışık yayıyor demektir.

Ampul parlaklığı, ampulden geçen **akım miktarına** bağlıdır. Akım ne kadar fazlaysa ampul o kadar parlak yanar.

### 3.2. Pil Sayısı (Gerilim) ve Ampul Parlaklığı

**Gerilim (voltaj):** Pilin devreye verdiği elektriksel enerjinin büyüklüğüdür. Birimi **volt (V)**'tur.

- Pil sayısı arttıkça devrenin **gerilimi** artar.
- Gerilim arttıkça devreden geçen **akım** artar.
- Akım arttıkça ampulün **parlaklığı** artar.

| Durum | Gerilim | Akım | Ampul Parlaklığı |
|-------|---------|------|-----------------|
| 1 pil | Düşük | Az | Az parlak |
| 2 pil (seri bağlı) | Yüksek | Çok | Çok parlak |
| 3 pil (seri bağlı) | Daha yüksek | Daha çok | En parlak |

**Önemli:** Pil sayısını artırmak ampulün parlaklığını artırır ancak ampulün dayanabileceği maksimum gerilim aşılmamalıdır. Aksi hâlde ampul patlayabilir.

**Pillerin bağlanması:**

| Bağlantı Türü | Açıklama | Gerilim |
|--------------|---------|---------|
| **Seri bağlama** | Bir pilin (+) kutbu diğer pilin (-) kutbuna bağlanır | Toplam gerilim artar (V₁ + V₂ + ...) |
| **Paralel bağlama** | Pillerin (+) kutupları birleştirilir, (-) kutupları birleştirilir | Gerilim değişmez, pil ömrü artar |

**Örnek:** 1,5 V'luk iki pil seri bağlanırsa toplam gerilim 3 V olur. Paralel bağlanırsa 1,5 V kalır.

### 3.3. Ampul Sayısı ve Parlaklık

Ampullerin devreye bağlanma şekli, parlaklıklarını etkiler.

**Seri bağlı ampuller:**

```
    ──┤├────⨁1────⨁2────o/──
    │                      │
    └──────────────────────┘
```

| Özellik | Seri Bağlantı |
|---------|--------------|
| Ampul parlaklığı | Azalır (her ampul daha sönük yanar) |
| Akım yolu | Tek bir yol vardır |
| Bir ampul patlarsa | Diğer ampuller de söner (devre açılır) |
| Pil ömrü | Daha kısa sürede biter |

**Paralel bağlı ampuller:**

```
    ──┤├─────────────o/──
    │       │       │    │
    │      ⨁1      ⨁2   │
    │       │       │    │
    └───────┴───────┴────┘
```

| Özellik | Paralel Bağlantı |
|---------|-----------------|
| Ampul parlaklığı | Değişmez (her ampul aynı parlaklıkta yanar) |
| Akım yolu | Birden fazla yol vardır |
| Bir ampul patlarsa | Diğer ampuller yanmaya devam eder |
| Pil ömrü | Daha kısa sürede biter (daha fazla akım çekilir) |

### 3.4. Seri ve Paralel Bağlantı Karşılaştırması

| Özellik | Seri Bağlantı | Paralel Bağlantı |
|---------|--------------|-----------------|
| Ampul parlaklığı | Düşük (her ampul sönük) | Yüksek (her ampul parlak) |
| Devre yolu | Tek yol | Çoklu yol |
| Bir ampul patlarsa | Tümü söner | Diğerleri yanar |
| Pil tüketimi | Az | Çok |
| Kullanım alanı | El feneri, yılbaşı ışıkları | Ev aydınlatması, okul, iş yeri |

### 3.5. Kablo Uzunluğu ve Kalınlığı

**Kablonun uzunluğu:**
- Kablo **uzadıkça** direnç artar, ampul parlaklığı **azalır**.
- Kablo **kısaldıkça** direnç azalır, ampul parlaklığı **artar**.

**Kablonun kalınlığı (kesit alanı):**
- Kablo **kalınlaştıkça** direnç azalır, ampul parlaklığı **artar**.
- Kablo **inceldikçe** direnç artar, ampul parlaklığı **azalır**.

**Özet:** Kısa ve kalın kablolar, uzun ve ince kablolara göre ampulü daha parlak yakar.

### 3.6. Ampulün Yapısı ve Gücü

**Ampulün iç yapısı:**
- Ampulün içinde ince bir **flaman teli** (genellikle tungsten) bulunur.
- Flaman teli, elektrik akımına karşı direnç gösterir ve ısınarak ışık yayar.
- Ampulün içinde **argon gazı** veya vakum bulunur.

**Ampul gücü (watt):**
- Ampulün üzerinde yazan watt (W) değeri, ampulün ne kadar enerji harcadığını gösterir.
- Watt değeri büyüdükçe ampul daha parlaktır (ancak daha fazla elektrik harcar).

| Ampul Türü | Güç (W) | Parlaklık | Enerji Tüketimi |
|-----------|---------|-----------|----------------|
| LED ampul | 5-10 W | Yüksek | Çok az |
| Tasarruflu ampul | 10-20 W | Orta | Az |
| Akkor flamanlı ampul | 40-100 W | Düşük-Orta | Çok |

### 3.7. Ampul Parlaklığını Etkileyen Değişkenler Tablosu

| Değişken | Değişim | Ampul Parlaklığı |
|----------|---------|-----------------|
| **Pil sayısı (seri)** | Artar | Artar |
| **Pil sayısı (seri)** | Azalır | Azalır |
| **Ampul sayısı (seri)** | Artar | Her biri azalır |
| **Ampul sayısı (paralel)** | Artar | Değişmez |
| **Kablo uzunluğu** | Artar | Azalır |
| **Kablo uzunluğu** | Azalır | Artar |
| **Kablo kalınlığı** | Artar (kalın) | Artar |
| **Kablo kalınlığı** | Azalır (ince) | Azalır |
| **Ampul gücü (watt)** | Artar | Artar |

---

## 4. ELEKTRİĞİN GÜVENLİ KULLANIMI

### 4.1. Elektrik Çarpması ve Güvenlik

Elektrik hayatımızı kolaylaştıran harika bir enerji kaynağıdır ancak dikkatli kullanılmazsa tehlikeli olabilir.

**Elektrik çarpması:** İnsan vücudundan elektrik akımı geçmesidir. Vücudumuz iletken olduğu için elektrik akımı bize zarar verebilir.

**Elektrik güvenliği kuralları:**

| Kural | Açıklama |
|-------|----------|
| **Islak elle dokunma** | Islak elle asla prize, anahtara veya elektrikli aletlere dokunulmaz |
| **Prizlere cisim sokma** | Prizlere metal cisim, çatal, bıçak vb. sokulmaz |
| **Kabloları çekme** | Fiş kablosundan çekilerek çıkarılmaz, fişten tutulur |
| **Hasarlı kablo** | Yıpranmış, kesilmiş kablolar kullanılmaz |
| **Su ile temas** | Elektrikli aletler suya yakın yerde kullanılmaz |
| **Priz yükü** | Bir prize çok sayıda alet bağlanmaz (yangın riski) |
| **Uyarı levhaları** | Elektrik uyarı levhalarına dikkat edilir (yüksek gerilim) |
| **Uzman yardımı** | Elektrik arızalarında mutlaka uzman çağrılır |

### 4.2. Güvenlik Uyarı Levhaları

| Levha | Anlamı |
|-------|--------|
| ⚡ (Şimşek sembolü) | Elektrik tehlikesi, yüksek gerilim |
| ☠ (Kafatası) | Ölümcül tehlike |
| ⛔ (Kırmızı çarpı) | Yasak, girilmez |

---

## 5. ÜNİTE DEĞERLENDİRMESİ

### 5.1. Anahtar Kavramlar Sözlüğü

| Kavram | Anlamı |
|--------|--------|
| **Elektrik devresi** | Elektrik akımının dolaştığı kapalı yol |
| **Pil** | Devreye elektrik enerjisi sağlayan kaynak |
| **Ampul** | Elektrik enerjisini ışık enerjisine çeviren eleman |
| **Anahtar** | Devreyi açıp kapamaya yarayan eleman |
| **Bağlantı kablosu** | Devre elemanlarını birbirine bağlayan iletken |
| **Devre şeması** | Devre elemanlarının sembollerle gösterildiği çizim |
| **Açık devre** | Kopuk, akımın geçmediği devre |
| **Kapalı devre** | Tam, akımın geçtiği devre |
| **Kısa devre** | Akımın yükten geçmeden kaynağa döndüğü devre |
| **Gerilim (voltaj)** | Pilin devreye verdiği elektriksel enerji büyüklüğü |
| **Akım** | Elektrik yüklerinin devredeki hareketi |
| **İletken** | Elektrik akımını ileten madde |
| **Yalıtkan** | Elektrik akımını iletmeyen madde |
| **Seri bağlantı** | Elemanların tek yol oluşturacak şekilde bağlanması |
| **Paralel bağlantı** | Elemanların çoklu yol oluşturacak şekilde bağlanması |
| **Direnç** | Elektrik akımına karşı gösterilen zorluk |
| **Flaman** | Ampulün içindeki ışık yayan ince tel |

### 5.2. Seri ve Paralel Bağlantı Karşılaştırma Tablosu

| Özellik | Seri Bağlantı | Paralel Bağlantı |
|---------|--------------|-----------------|
| Ampul parlaklığı | Sönük | Parlak |
| Akım yolu sayısı | 1 | Ampul sayısı kadar |
| Bir ampul patlarsa | Tümü söner | Diğerleri yanar |
| Pil ömrü | Uzun | Kısa |
| Kullanım alanı | El feneri | Ev aydınlatması |

### 5.3. Devre Elemanları Sembolleri Tablosu

| Eleman | Sembol |
|--------|--------|
| Pil | `─┤├─` |
| Ampul | `⨁` |
| Anahtar (açık) | `o/` |
| Anahtar (kapalı) | `o` |
| Kablo | `───` |
| Motor | `Ⓜ` |
| Zil | |

### 5.4. Çoktan Seçmeli Test Soruları

**Soru 1:** Aşağıdakilerden hangisi bir elektrik devresinin temel elemanlarından biri değildir?
A) Pil
B) Ampul
C) Mıknatıs
D) Anahtar

**Soru 2:** Bir elektrik devresinde pilin görevi nedir?
A) Işık vermek
B) Enerji sağlamak
C) Devreyi açıp kapamak
D) Enerjiyi iletmek

**Soru 3:** Aşağıdaki devre elemanlarından hangisinin sembolü "içinde çarpı olan daire"dir?
A) Pil
B) Ampul
C) Anahtar
D) Motor

**Soru 4:** Bir elektrik devresinde anahtarın görevi nedir?
A) Elektrik üretmek
B) Işık vermek
C) Devreyi açıp kapamak
D) Enerjiyi iletmek

**Soru 5:** Aşağıdaki durumların hangisinde ampul yanar?
A) Açık devre
B) Kapalı devre
C) Kısa devre
D) Anahtar açıkken

**Soru 6:** Ampul parlaklığını artırmak için aşağıdakilerden hangisi yapılmalıdır?
A) Pil sayısını azaltmak
B) Seri bağlı ampul sayısını artırmak
C) Pil sayısını artırmak (seri bağlı)
D) Kabloyu uzatmak

**Soru 7:** Aşağıdakilerden hangisi elektrik için iyi bir iletkendir?
A) Plastik
B) Tahta
C) Bakır
D) Cam

**Soru 8:** Aşağıdakilerden hangisi elektrik için yalıtkandır?
A) Demir
B) Alüminyum
C) Altın
D) Kauçuk

**Soru 9:** İki ampulün seri bağlı olduğu bir devrede bir ampul patlarsa ne olur?
A) Diğer ampul daha parlak yanar
B) Diğer ampul söner
C) Diğer ampulün parlaklığı değişmez
D) Pil bozulur

**Soru 10:** Aşağıdakilerden hangisi devre şeması çiziminde kullanılmaz?
A) Semboller
B) Düz çizgiler
C) Gerçek resimler
D) Noktalar

**Soru 11:** Kabloların dış kısmı neden yalıtkan malzemeden yapılır?
A) Güzel görünmesi için
B) Elektrik çarpmasını önlemek için
C) Daha sağlam olması için
D) Ucuz olması için

**Soru 12:** Ev aydınlatmasında ampuller genellikle nasıl bağlanır?
A) Seri
B) Paralel
C) Karma
D) Rastgele

**Soru 13:** Bir devrede pil sayısı artırılırsa (seri bağlı) ampul parlaklığı nasıl değişir?
A) Azalır
B) Artar
C) Değişmez
D) Önce artar sonra azalır

**Soru 14:** Aşağıdakilerden hangisi elektrik güvenliği kurallarından biridir?
A) Prizlere metal cisim sokmak
B) Islak elle prize dokunmak
C) Fişi kablosundan çekerek çıkarmak
D) Hasarlı kabloları kullanmamak

**Soru 15:** Kısa devre durumunda aşağıdakilerden hangisi olur?
A) Ampul çok parlak yanar
B) Ampul söner (akım ampulden geçmez)
C) Ampul normal parlaklıkta yanar
D) Devre çalışmaya devam eder

**Soru 16:** El fenerinde ampuller genellikle nasıl bağlanır?
A) Paralel
B) Seri
C) Karma
D) Bağlanmaz

**Soru 17:** Kablo uzunluğu arttıkça ampul parlaklığı nasıl değişir?
A) Artar
B) Azalır
C) Değişmez
D) Önce artar sonra azalır

**Soru 18:** Aşağıdaki malzemelerden hangisi elektriği iletir?
A) Cam
B) Plastik
C) Grafit (kalem ucu)
D) Tahta

**Soru 19:** Paralel bağlı iki ampulün olduğu bir devrede bir ampul patlarsa diğer ampulün durumu ne olur?
A) Söner
B) Daha parlak yanar
C) Yanmaya devam eder
D) Söner sonra tekrar yanar

**Soru 20:** Aşağıdakilerden hangisi bir devrede ampulün parlaklığını etkilemez?
A) Pil sayısı
B) Ampul sayısı ve bağlanma şekli
C) Anahtarın rengi
D) Kablonun kalınlığı

---

**CEVAP ANAHTARI:**
1-C, 2-B, 3-B, 4-C, 5-B, 6-C, 7-C, 8-D, 9-B, 10-C, 11-B, 12-B, 13-B, 14-D, 15-B, 16-B, 17-B, 18-C, 19-C, 20-C

---

### 5.5. Sınıf İçi Tartışma Soruları

1. Evinizdeki lambalar nasıl bağlanmıştır? Bir lamba söndüğünde diğerleri de sönüyor mu? Bunun sebebi nedir?
2. El feneri ile ev aydınlatması arasındaki devre farkı nedir? Hangisi daha avantajlıdır?
3. Kablolar neden bakırdan yapılır? Bakır yerine plastik kullanılsaydı ne olurdu?
4. Bir devrede pil sayısını artırmak ampul parlaklığını nasıl etkiler? Çok fazla pil bağlanırsa ne olabilir?
5. Yılbaşı ağacı süslemelerinde kullanılan küçük lambalar neden bir tanesi patlayınca tümü söner? Bu ampuller nasıl bağlanmıştır?
6. Neden ıslak elle elektrik düğmelerine dokunmamalıyız? Bunun bilimsel açıklaması nedir?
7. Okulunuzda ve evinizde hangi elektrik güvenlik önlemleri alınmıştır?
8. Bir ampulün üzerinde yazan "60 W" veya "100 W" ne anlama gelir?
9. Uzun bir kablo kullanıldığında ampulün parlaklığı neden azalır?
10. Gelecekte elektrik enerjisi olmasaydı hayatımız nasıl olurdu?

### 5.6. Ödev ve Proje Çalışmaları

**Proje Ödevi 1 — Basit Elektrik Devresi Kurulumu:**
Bir pil, bir ampul, bir anahtar ve bağlantı kabloları kullanarak basit bir elektrik devresi kurun. Devre şemasını çizin. Anahtarı açıp kapatarak ampulün yanma durumunu gözlemleyin.

**Deney Ödevi 2 — Seri ve Paralel Bağlantı:**
İki ampul ve iki pil kullanarak önce seri, sonra paralel devre kurun. Her iki durumda da ampul parlaklıklarını karşılaştırın. Bir ampulü söküp diğer ampulün durumunu gözlemleyin. Sonuçları bir tablo hâlinde karşılaştırın.

**Deney Ödevi 3 — İletken ve Yalıtkan Maddeleri Test Etme:**
Basit bir devre kurun. Devrede kablonun bir kısmını açık bırakın. Açık kalan kısma sırayla; bakır tel, tahta parçası, silgi, kalem ucu, çivi, plastik, alüminyum folyo gibi malzemeleri bağlayarak ampulün yanıp yanmadığını test edin. İletken ve yalıtkan malzemeleri listeleyin.

**Araştırma Ödevi — Elektrikli Ev Aletleri:**
Evinizdeki 5 farklı elektrikli aletin (buzdolabı, çamaşır makinesi, televizyon, ütü, saç kurutma makinesi vb.) hangi enerji dönüşümlerini yaptığını araştırın. Bir tablo hâlinde sunun.

**Proje Ödevi — Devre Şeması Çizimi:**
Kendi tasarladığınız bir elektrik devresinin şemasını çizin. Devrede en az 2 ampul, 1 pil, 1 anahtar bulunsun. Ampullerden birini seri, diğerini paralel bağladığınız bir devre tasarlayın.

**Deney Ödevi — Pil Sayısı ve Parlaklık:**
1 pil, 2 pil (seri bağlı) ve 3 pil (seri bağlı) kullanarak aynı ampulü çalıştırın. Her durumda ampulün parlaklığını gözlemleyip karşılaştırın. Pil sayısı arttıkça parlaklığın nasıl değiştiğini not edin.

---

