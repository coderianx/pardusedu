#include "mainwindow.h"
#include <webkit/webkit.h>
#include <md4c-html.h>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <fstream>
#include <regex>

void MainWindow::setup_linux() {
    auto* sw = Gtk::make_managed<Gtk::ScrolledWindow>();
    sw->set_policy(Gtk::PolicyType::AUTOMATIC, Gtk::PolicyType::AUTOMATIC);
    sw->set_vexpand(true);

    auto* box = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 10);
    box->set_margin_start(16);
    box->set_margin_end(16);
    box->set_margin_top(16);
    box->set_margin_bottom(16);

    auto* title = Gtk::make_managed<Gtk::Label>("Linux Komutları");
    title->add_css_class("page-title");
    title->set_halign(Gtk::Align::START);

    auto* search = Gtk::make_managed<Gtk::Entry>();
    search->set_placeholder_text("Komut ara...");
    search->signal_changed().connect([this, search]() { filter_linux(search->get_text()); });

    lab_warning = Gtk::make_managed<Gtk::Label>("");
    lab_warning->add_css_class("lab-warning");
    lab_warning->set_halign(Gtk::Align::START);
    lab_warning->set_visible(!lab_ready);
    if (!lab_ready) {
        lab_warning->set_markup(
            "<b>PardusLab</b> aktif deil. Denemek iin: <tt>sudo apt install podman</tt>");
    }
    lab_warning->set_margin_bottom(8);

    // --- Challenge Section ---
    Gtk::Label* xp_lbl = nullptr;
    if (parduslab) {
        auto& challenges = parduslab->get_challenges();
        if (!challenges.empty()) {
            auto* chall_header = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 0);
            chall_header->set_margin_top(16);
            chall_header->set_margin_bottom(4);

            auto* chall_title = Gtk::make_managed<Gtk::Label>("Meydan Okumalar");
            chall_title->add_css_class("page-title");
            chall_title->set_halign(Gtk::Align::START);
            chall_title->set_hexpand(true);

            {
                int total_xp = 0;
                for (auto& ch : challenges) {
                    if (completed_challenges.count(ch.id))
                        total_xp += ch.xp;
                }
                xp_lbl = Gtk::make_managed<Gtk::Label>("");
                xp_lbl->set_markup("<b>Toplam XP: " + std::to_string(total_xp) + "</b>");
                xp_lbl->set_halign(Gtk::Align::END);
            }

            chall_header->append(*chall_title);
            chall_header->append(*xp_lbl);
            linux_commands.append(*chall_header);

            for (auto& ch : challenges) {
                auto* card = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 4);
                card->add_css_class("cmd-card");
                card->set_margin_bottom(8);

                auto* name_lbl = Gtk::make_managed<Gtk::Label>("");
                name_lbl->set_markup("<b>" + ch.title + "</b>  <span foreground='#888'>[" + std::to_string(ch.xp) + " XP]</span>");
                name_lbl->set_halign(Gtk::Align::START);
                name_lbl->add_css_class("cmd-name");

                auto* desc_lbl = Gtk::make_managed<Gtk::Label>(ch.description);
                desc_lbl->set_halign(Gtk::Align::START);
                desc_lbl->set_wrap(true);
                desc_lbl->set_margin_top(4);

                auto* hint_rev = Gtk::make_managed<Gtk::Revealer>();
                hint_rev->set_transition_type(Gtk::RevealerTransitionType::SLIDE_DOWN);
                auto* hint_lbl = Gtk::make_managed<Gtk::Label>("");
                hint_lbl->set_markup("<i>İpucu: " + ch.hint + "</i>");
                hint_lbl->set_halign(Gtk::Align::START);
                hint_lbl->set_wrap(true);
                hint_lbl->set_margin_top(4);
                hint_rev->set_child(*hint_lbl);

                auto* status_lbl = Gtk::make_managed<Gtk::Label>("");
                status_lbl->set_halign(Gtk::Align::START);
                status_lbl->set_margin_top(4);
                bool done = completed_challenges.count(ch.id);
                if (done) {
                    status_lbl->set_markup("<span foreground='green'>✓ Tamamlandı</span>");
                }

                auto* row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
                row->set_margin_top(6);

                auto* btn_hint = Gtk::make_managed<Gtk::Button>("İpucu Göster");
                btn_hint->add_css_class("lab-btn-try");
                btn_hint->signal_clicked().connect([hint_rev, btn_hint]() {
                    bool vis = hint_rev->get_child_revealed();
                    hint_rev->set_reveal_child(!vis);
                    btn_hint->set_label(vis ? "İpucu Göster" : "İpucu Gizle");
                });

                auto* btn_terminal = Gtk::make_managed<Gtk::Button>("Terminal");
                btn_terminal->add_css_class("lab-btn-terminal");
                btn_terminal->signal_clicked().connect([this, ch_id = ch.id]() {
                    on_lab_open_terminal("Meydan okuma: " + ch_id);
                });

                auto* btn_check = Gtk::make_managed<Gtk::Button>("Kontrol Et");
                btn_check->add_css_class("lab-btn-try");
                btn_check->signal_clicked().connect([this, ch, card, status_lbl, btn_check, done, xp_lbl]() mutable {
                    if (!lab_ready) {
                        show_lab_error("PardusLab hazır değil",
                            "Podman kurulu değil veya container başlatılmamış.");
                        return;
                    }
                    if (current_lab_container_id.empty()) {
                        current_lab_container_id = parduslab->start_container();
                        if (current_lab_container_id.empty()) {
                            show_lab_error("Container başlatılamadı",
                                "Podman ile container başlatılamadı.");
                            return;
                        }
                        lab_ready = true;
                    }
                    bool ok = parduslab->check_challenge(current_lab_container_id, ch.id);
                    if (ok) {
                        status_lbl->set_markup("<span foreground='green'>✓ Doğru! +" + std::to_string(ch.xp) + " XP</span>");
                        completed_challenges.insert(ch.id);
                        save_data();
                        if (xp_lbl) {
                            int total = 0;
                            for (auto& c : parduslab->get_challenges()) {
                                if (completed_challenges.count(c.id))
                                    total += c.xp;
                            }
                            xp_lbl->set_markup("<b>Toplam XP: " + std::to_string(total) + "</b>");
                        }
                    } else {
                        // Kurulum hala devam ediyor olabilir; 3sn sonra tekrar dene
                        status_lbl->set_markup("<span foreground='orange'>⟳ Kurulum devam ediyor, tekrar deneniyor...</span>");
                        btn_check->set_sensitive(false);
                        Glib::signal_timeout().connect_once([this, ch, status_lbl, btn_check, xp_lbl]() {
                            if (current_lab_container_id.empty()) return;
                            bool ok2 = parduslab->check_challenge(current_lab_container_id, ch.id);
                            if (ok2) {
                                status_lbl->set_markup("<span foreground='green'>✓ Doğru! +" + std::to_string(ch.xp) + " XP</span>");
                                completed_challenges.insert(ch.id);
                                save_data();
                                if (xp_lbl) {
                                    int total = 0;
                                    for (auto& c : parduslab->get_challenges()) {
                                        if (completed_challenges.count(c.id))
                                            total += c.xp;
                                    }
                                    xp_lbl->set_markup("<b>Toplam XP: " + std::to_string(total) + "</b>");
                                }
                            } else {
                                status_lbl->set_markup("<span foreground='red'>✗ Yanlış. İpucunu dene: " + ch.hint + "</span>");
                            }
                            btn_check->set_sensitive(true);
                        }, 3000);
                    }
                });

                row->append(*btn_hint);
                row->append(*btn_terminal);
                row->append(*btn_check);

                card->append(*name_lbl);
                card->append(*desc_lbl);
                card->append(*hint_rev);
                card->append(*status_lbl);
                card->append(*row);
                linux_commands.append(*card);
            }
        }
    }

    struct Cmd { std::string cmd, desc, ex, detail; };
    std::vector<Cmd> cmds = {
        {"ls", "Dizin listele", "ls -la /home",
         "Belirtilen dizindeki dosya ve alt dizinleri listeler. Linux'ta en sık kullanılan komutlardan biridir.\n\n"
         "Seçenekler:\n"
         "  -a  Gizli dosyaları da gösterir (nokta ile başlayanlar)\n"
         "  -l  Uzun format (izin, sahip, boyut, tarih)\n"
         "  -h  Boyutları okunabilir formatta gösterir (KB, MB)\n"
         "  -R  Alt dizinleri özyinelemeli listeler\n"
         "  -t  Değişiklik zamanına göre sıralar\n"
         "  -r  Sıralamayı tersine çevirir\n\n"
         "Örnekler:\n"
         "  ls -lah /home/kullanici  → Detaylı, gizli dahil, okunabilir boyut\n"
         "  ls -lt /var/log          → En yeni dosyalar üstte\n"
         "  ls *.txt                 → Sadece .txt dosyaları"},
        {"cd", "Dizin değiştir", "cd /var/log",
         "Çalışma dizinini (current working directory) değiştirir. Kabuk oturumunuzun bulunduğu konumu belirler.\n\n"
         "Kullanım:\n"
         "  cd ..        → Bir üst dizine çık\n"
         "  cd ~         → Ev dizinine git (home)\n"
         "  cd -         → Önceki dizine geri dön\n"
         "  cd           → Ev dizinine git (parametresiz)\n"
         "  cd /         → Kök dizine git\n"
         "  cd /etc/nginx → Mutlak yol ile git\n\n"
         "İpucu: Tab tuşu ile otomatik tamamlama yapabilirsiniz."},
        {"pwd", "Çalışılan dizin", "pwd",
         "Print Working Directory. Şu anda bulunduğunuz dizinin tam (mutlak) yolunu ekrana yazdırır.\n\n"
         "Neden önemli?\n"
         "  Terminalde kaybolduğunuzda nerede olduğunuzu gösterir\n"
         "  Scriptlerde geçerli dizini öğrenmek için kullanılır\n"
         "  Göreceli yolların nereden başladığını belirler\n\n"
         "Örnek:\n"
         "  pwd  → /home/kullanici/Belgeler/Projeler"},
        {"cp", "Kopyala", "cp dosya.txt /yedek/",
         "Dosya veya dizinleri kopyalar. Orijinal dosya olduğu yerde kalır, bir kopyası oluşturulur.\n\n"
         "Seçenekler:\n"
         "  -r  Dizinleri özyinelemeli kopyalar (içerik dahil)\n"
         "  -i  Üzerine yazmadan önce onay sorar\n"
         "  -v  Kopyalanan dosyaları ekrana yazdırır\n"
         "  -p  İzinleri ve zaman damgalarını korur\n"
         "  -u  Sadece daha yeni dosyaları kopyalar\n\n"
         "Örnekler:\n"
         "  cp dosya.txt yedek.txt        → Dosya kopyala\n"
         "  cp -rv /kaynak /hedef         → Dizin kopyala\n"
         "  cp *.pdf /yedek/pdfler/       → Tüm PDF'leri kopyala"},
        {"mv", "Taşı / yeniden adlandır", "mv eski.txt yeni.txt",
         "Dosya veya dizinleri taşır ya da yeniden adlandırır. Taşıma işlemi sonrası kaynak silinir.\n\n"
         "Kullanım:\n"
         "  Aynı dizin içinde → Yeniden adlandırma\n"
         "  Farklı dizine     → Taşıma işlemi\n\n"
         "Seçenekler:\n"
         "  -i  Üzerine yazmadan önce sorar\n"
         "  -v  Taşınan dosyaları gösterir\n"
         "  -n  Mevcut dosyanın üzerine yazmaz\n\n"
         "Örnekler:\n"
         "  mv eski.txt yeni.txt          → Yeniden adlandır\n"
         "  mv dosya.txt /home/belgeler/  → Taşı\n"
         "  mv *.jpg /resimler/           → Tüm JPG'leri taşı"},
        {"rm", "Sil", "rm -rf /tmp/eski",
         "Dosya veya dizinleri siler. DİKKAT: Silinen dosyalar geri getirilemez!\n\n"
         "Seçenekler:\n"
         "  -r  Dizinleri özyinelemeli siler\n"
         "  -f  Zorla siler, onay sormaz, hata vermez\n"
         "  -i  Her dosya için onay sorar (güvenli)\n"
         "  -v  Silinen dosyaları gösterir\n\n"
         "⚠️ UYARI: rm -rf / komutu sistemi tamamen siler!\n\n"
         "Örnekler:\n"
         "  rm dosya.txt        → Tek dosya sil\n"
         "  rm -r proje/        → Dizin sil\n"
         "  rm -i *.log         → Her .log için sor"},
        {"mkdir", "Dizin oluştur", "mkdir -p a/b/c",
         "Yeni dizin (klasör) oluşturur.\n\n"
         "Seçenekler:\n"
         "  -p  Üst dizinleri de otomatik oluşturur\n"
         "  -v  Oluşturulan dizinleri gösterir\n"
         "  -m  İzinleri belirleyerek oluşturur\n\n"
         "Örnekler:\n"
         "  mkdir yeni_dizin              → Tek dizin oluştur\n"
         "  mkdir -p a/b/c/d              → İç içe dizinler\n"
         "  mkdir -m 700 gizli_dizin      → Sadece sahibi erişir\n"
         "  mkdir proje/{src,doc,test}    → Birden fazla dizin"},
        {"cat", "Dosya göster", "cat /etc/passwd",
         "Dosya içeriğini terminal'e yazdırır. Birden fazla dosyayı birleştirebilir.\n\n"
         "Seçenekler:\n"
         "  -n  Satır numaralarını gösterir\n"
         "  -b  Boş olmayan satırları numaralandırır\n"
         "  -E  Satır sonlarını $ ile gösterir\n\n"
         "Yönlendirme ile kullanım:\n"
         "  cat dosya1 dosya2 > birlesik.txt  → Birleştir\n"
         "  cat dosya >> mevcut.txt           → Ekleme yap\n"
         "  cat > yeni.txt                    → Klavyeden yaz\n\n"
         "İpucu: Uzun dosyalar için `less` veya `more` kullanın."},
        {"grep", "Metin ara", "grep 'hata' syslog",
         "Dosya veya giriş akışında metin araması yapar. Düzenli ifade (regex) destekler.\n\n"
         "Seçenekler:\n"
         "  -i  Büyük/küçük harf duyarsız\n"
         "  -n  Eşleşen satır numarasını gösterir\n"
         "  -r  Dizinlerde özyinelemeli ara\n"
         "  -v  EşleşMEYEN satırları gösterir\n"
         "  -c  Eşleşme sayısını gösterir\n"
         "  -l  Sadece dosya adını gösterir\n"
         "  -A 3  Eşleşmeden sonra 3 satır göster\n"
         "  -B 3  Eşleşmeden önce 3 satır göster\n\n"
         "Örnekler:\n"
         "  grep -i 'error' /var/log/syslog\n"
         "  grep -rn 'function' /proje/src/\n"
         "  ps aux | grep nginx"},
        {"find", "Dosya ara", "find / -name '*.log'",
         "Dosya ve dizin arar. Çok güçlü filtreleme seçenekleri vardır.\n\n"
         "Filtreler:\n"
         "  -name 'isim'     İsme göre ara (glob desteği)\n"
         "  -type f          Sadece dosyalar\n"
         "  -type d          Sadece dizinler\n"
         "  -size +10M       10MB'den büyük\n"
         "  -size -1k        1KB'den küçük\n"
         "  -mtime -7        Son 7 günde değişen\n"
         "  -perm 755        İzinlere göre\n"
         "  -user kullanici  Sahibine göre\n\n"
         "İşlemler:\n"
         "  -exec rm {} \\;  Bulunanları sil\n"
         "  -delete          Bulunanları sil (kısa)\n"
         "  -ls              Detaylı listele\n\n"
         "Örnekler:\n"
         "  find /home -name '*.pdf' -size +5M\n"
         "  find /tmp -type f -mtime +30 -delete"},
        {"chmod", "İzin değiştir", "chmod 755 betik.sh",
         "Dosya ve dizinlerin erişim izinlerini değiştirir.\n\n"
         "İzin değerleri:\n"
         "  4 = r (okuma)    → Dosyayı okuyabilir\n"
         "  2 = w (yazma)    → Dosyayı düzenleyebilir\n"
         "  1 = x (çalıştırma) → Programı çalıştırabilir\n\n"
         "Format: [sahip][grup][diğer]\n"
         "  755 → Sahip: rwx, Grup: r-x, Diğer: r-x\n"
         "  644 → Sahip: rw-, Grup: r--, Diğer: r--\n"
         "  700 → Sahip: rwx, Grup: ---, Diğer: ---\n\n"
         "Sembolik format:\n"
         "  chmod +x betik.sh    → Çalıştırma izni ekle\n"
         "  chmod -w dosya.txt   → Yazma izni kaldır\n"
         "  chmod u+rwx dosya    → Sahibe tüm izinler\n\n"
         "  -R  Özyinelemeli uygular"},
        {"chown", "Sahiplik değiştir", "chown kullanıcı:grup dosya",
         "Dosya ve dizinlerin sahibini ve grubunu değiştirir.\n\n"
         "Kullanım:\n"
         "  chown kullanici dosya     → Sadece sahip değiştir\n"
         "  chown :grup dosya         → Sadece grup değiştir\n"
         "  chown kullanici:grup dosya → İkisini birlikte\n\n"
         "Seçenekler:\n"
         "  -R  Özyinelemeli uygular\n"
         "  -v  Değişiklikleri gösterir\n\n"
         "⚠️ Sadece root kullanıcısı dosya sahibini değiştirebilir.\n\n"
         "Örnekler:\n"
         "  chown www-data:www-data /var/www/html\n"
         "  chown -R kullanici:users /home/kullanici"},
        {"ps", "Süreç listesi", "ps aux | grep nginx",
         "Çalışan süreçleri (process) listeler. Anlık bir görüntü verir.\n\n"
         "Formatlar:\n"
         "  ps aux    → Tüm kullanıcıların tüm süreçleri (BSD)\n"
         "  ps -ef    → Tüm süreçler tam format (System V)\n"
         "  ps -u kullanıcı → Belirli kullanıcının süreçleri\n"
         "  ps -p PID → Belirli PID'yi göster\n\n"
         "Çıktı sütunları:\n"
         "  USER  Süreci çalıştıran kullanıcı\n"
         "  PID   Süreç kimliği\n"
         "  %CPU  CPU kullanım yüzdesi\n"
         "  %MEM  Bellek kullanım yüzdesi\n"
         "  VSZ   Sanal bellek boyutu\n"
         "  RSS   Fiziksel bellek boyutu\n"
         "  STAT  Süreç durumu (R,S,Z,T)\n"
         "  COMMAND  Komut satırı\n\n"
         "İpucu: `ps aux --sort=-%cpu` CPU'ya göre sırala."},
        {"top", "Süreçler", "top",
         "Canlı süreç izleme aracı. Sistem kaynaklarını gerçek zamanlı gösterir.\n\n"
         "Üst bölüm:\n"
         "  Sistem çalışma süresi, kullanıcı sayısı, yük ortalaması\n"
         "  Toplam süreç sayısı (çalışan, uyuyan, zombie)\n"
         "  CPU kullanımı (user, system, idle, iowait)\n"
         "  Bellek kullanımı (toplam, kullanılan, boş)\n\n"
         "Kısayol tuşları:\n"
         "  q     → Çıkış\n"
         "  k     → Süreç sonlandır (PID sorar)\n"
         "  M     → Belleğe göre sırala\n"
         "  P     → CPU'ya göre sırala\n"
         "  T     → Çalışma süresine göre sırala\n"
         "  c     → Tam komut satırını göster\n"
         "  u     → Kullanıcıya göre filtrele\n"
         "  h     → Yardım\n\n"
         "Alternatif: `htop` daha renkli ve kullanıcı dostudur."},
        {"kill", "Süreç sonlandır", "kill -9 1234",
         "Süreçlere sinyal gönderir. Genellikle süreç sonlandırmak için kullanılır.\n\n"
         "Sinyaller:\n"
         "  -15 (SIGTERM)  → Nazik sonlandırma (varsayılan)\n"
         "                    Süreç temizlik yapabilir\n"
         "  -9  (SIGKILL)  → Zorla sonlandırma\n"
         "                    Süreç engellemeye çalışamaz\n"
         "  -1  (SIGHUP)   → Yeniden yükleme\n"
         "                    Konfigürasyon yeniden okunur\n"
         "  -18 (SIGCONT)  → Durdurulmuş süreci devam ettir\n"
         "  -19 (SIGSTOP)  → Süreci durdur\n\n"
         "İlgili komutlar:\n"
         "  killall isim  → İsme göre tüm süreçleri sonlandır\n"
         "  pkill isim    → İsme göre sinyal gönder\n"
         "  kill -l       → Tüm sinyalleri listele\n\n"
         "Örnekler:\n"
         "  kill 1234         → Nazik sonlandır\n"
         "  kill -9 1234      → Zorla sonlandır\n"
         "  killall firefox   → Tüm Firefox süreçlerini kapat"},
        {"tar", "Arşiv", "tar -xzf arsiv.tar.gz",
         "Dosya arşivleme ve sıkıştırma aracı. Linux'ta en yaygın arşiv formatıdır.\n\n"
         "Temel işlemler:\n"
         "  -c  Oluştur (create)\n"
         "  -x  Çıkar (extract)\n"
         "  -t  İçeriği listele\n"
         "  -f  Dosya adı belirt (her zaman son olmalı)\n\n"
         "Sıkıştırma:\n"
         "  -z  gzip (.tar.gz)   → Hızlı, yaygın\n"
         "  -j  bzip2 (.tar.bz2) → Daha iyi sıkıştırma\n"
         "  -J  xz (.tar.xz)     → En iyi sıkıştırma\n\n"
         "Diğer seçenekler:\n"
         "  -v  İşlem detaylarını gösterir\n"
         "  -C  Çıkarma dizini belirt\n"
         "  --exclude  Dosya hariç tut\n\n"
         "Örnekler:\n"
         "  tar -czf arsiv.tar.gz /dizin    → Oluştur\n"
         "  tar -xzf arsiv.tar.gz           → Çıkar\n"
         "  tar -tzf arsiv.tar.gz           → Listele\n"
         "  tar -xzf arsiv.tar.gz -C /hedef → Başka yere çıkar"},
        {"wget", "İndir", "wget https://ornek.com/dosya",
         "Web'den dosya indirme aracı. HTTP, HTTPS ve FTP destekler.\n\n"
         "Seçenekler:\n"
         "  -O dosya  Farklı isimle kaydet\n"
         "  -c        Kaldığı yerden devam et\n"
         "  -r        Özyinelemeli indir (site kopyala)\n"
         "  -i dosya  Dosyadan URL listesi oku\n"
         "  -b        Arka planda indir\n"
         "  -q        Sessiz mod\n"
         "  --limit-rate=1m  Hız sınırı koy\n"
         "  -P dizin  Kayıt dizini belirt\n"
         "  --user-agent  Kullanıcı ajanı değiştir\n\n"
         "Örnekler:\n"
         "  wget https://site.com/dosya.zip\n"
         "  wget -c https://site.com/buyuk.iso\n"
         "  wget -r -l 2 https://site.com\n"
         "  wget -i url_listesi.txt"},
        {"curl", "Veri aktar", "curl -O https://ornek.com/dosya",
         "URL ile veri transferi. wget'ten farklı olarak daha çok API ve HTTP istekleri için kullanılır.\n\n"
         "Seçenekler:\n"
         "  -O  Dosya adını URL'den al\n"
         "  -o isim  Dosya adını kendin belirle\n"
         "  -X POST  POST isteği gönder\n"
         "  -d 'veri'  POST verisi gönder\n"
         "  -H 'header'  HTTP header ekle\n"
         "  -u kullanıcı:şifre  Kimlik doğrulama\n"
         "  -L  Yönlendirmeleri takip et\n"
         "  -s  Sessiz mod\n"
         "  -I  Sadece header göster\n"
         "  -k  SSL sertifika kontrolünü atla\n\n"
         "Örnekler:\n"
         "  curl -O https://site.com/dosya.zip\n"
         "  curl -X POST -d '{\"key\":\"val\"}' https://api.site.com\n"
         "  curl -H 'Authorization: Bearer token' https://api.site.com\n"
         "  curl ifconfig.me  → Dış IP'ni öğren"},
        {"ssh", "Uz bağlantı", "ssh kullanıcı@sunucu",
         "Secure Shell. Uz bilgisayara güvenli bağlantı sağlar. Tüm iletişim şifrelidir.\n\n"
         "Seçenekler:\n"
         "  -p port  Port belirt (varsayılan: 22)\n"
         "  -i dosya  SSH anahtar dosyası kullan\n"
         "  -L port:hedef:port  Port yönlendirme (ileri)\n"
         "  -R port:hedef:port  Port yönlendirme (geri)\n"
         "  -X  X11 yönlendirme (GUI uygulamalar)\n"
         "  -C  Sıkıştırma kullan\n"
         "  -v  Detaylı çıktı (debug)\n\n"
         "Anahtar yönetimi:\n"
         "  ssh-keygen          → Yeni anahtar oluştur\n"
         "  ssh-copy-id sunucu  → Anahtarı sunucuya kopyala\n"
         "  ssh-agent           → Anahtar yöneticisi\n\n"
         "SSH config (~/.ssh/config):\n"
         "  Host myserver\n"
         "    HostName 192.168.1.100\n"
         "    User admin\n"
         "    Port 2222\n\n"
         "  ssh myserver  → Config'den bağlan"},
        {"scp", "Uz kopyala", "scp dosya kullanıcı@sunucu:/yol",
         "SSH üzerinden güvenli dosya kopyalama. Komut yapısı cp'ye benzer.\n\n"
         "Yönler:\n"
         "  Yerel → Uz:  scp dosya kullanıcı@sunucu:/yol\n"
         "  Uz → Yerel:  scp kullanıcı@sunucu:/dosya .\n"
         "  Uz → Uz:     scp sunucu1:/dosya sunucu2:/yol\n\n"
         "Seçenekler:\n"
         "  -r  Dizinleri özyinelemeli kopyala\n"
         "  -P port  Port belirt\n"
         "  -C  Sıkıştırarak aktar\n"
         "  -p  İzinleri ve zaman damgalarını koru\n"
         "  -l limit  Bant genişliği sınırı (Kbit/s)\n\n"
         "Örnekler:\n"
         "  scp rapor.pdf user@192.168.1.10:/home/user/\n"
         "  scp -r proje/ user@sunucu:/var/www/\n"
         "  scp -P 2222 dosya user@sunucu:~/\n\n"
         "İpucu: rsync daha hızlı ve akıllı senkronizasyon sağlar."},
        {"df", "Disk kullanım", "df -h",
         "Dosya sistemi disk kullanımını gösterir. Bağlı tüm bölümleri listeler.\n\n"
         "Seçenekler:\n"
         "  -h  Okunabilir format (GB, MB)\n"
         "  -T  Dosya sistemi türünü göster (ext4, xfs)\n"
         "  -i  Inode kullanımını göster\n"
         "  -t tip  Sadece belirli dosya sistemi\n"
         "  -x tip  Belirli dosya sistemini hariç tut\n\n"
         "Çıktı sütunları:\n"
         "  Filesystem  Dosya sistemi aygıtı\n"
         "  Size        Toplam boyut\n"
         "  Used        Kullanılan\n"
         "  Avail       Kullanılabilir\n"
         "  Use%        Kullanım yüzdesi\n"
         "  Mounted on  Bağlantı noktası\n\n"
         "Örnekler:\n"
         "  df -h              → Tüm bölümler\n"
         "  df -h /home        → /home bölümü\n"
         "  df -Th             → Tür bilgisi ile"},
        {"du", "Dizin boyutu", "du -sh /var",
         "Dizin ve dosyaların disk kullanımını hesaplar.\n\n"
         "Seçenekler:\n"
         "  -s  Sadece toplam boyut (summary)\n"
         "  -h  Okunabilir format\n"
         "  -c  Sonunda toplam göster\n"
         "  --max-depth=N  N seviye derinliğe in\n"
         "  -a  Tüm dosyaları göster (varsayılan: sadece dizin)\n"
         "  --time  Son değişiklik zamanını göster\n\n"
         "Örnekler:\n"
         "  du -sh /var/log           → /var/log toplam boyutu\n"
         "  du -sh *                  → Her dosya/dizin boyutu\n"
         "  du -h --max-depth=1 /home → Kullanıcı bazlı boyut\n"
         "  du -shc *.zip             → ZIP'ler + toplam\n\n"
         "İpucu: `du -sh * | sort -rh` en büyükten sırala."},
        {"free", "Bellek", "free -h",
         "RAM ve takas (swap) belleği kullanımını gösterir.\n\n"
         "Seçenekler:\n"
         "  -h  Okunabilir format (GB, MB)\n"
         "  -m  MB cinsinden\n"
         "  -g  GB cinsinden\n"
         "  -s N  Her N saniyede güncelle\n"
         "  -t  Toplam satırını ekle\n\n"
         "Çıktı sütunları:\n"
         "  total     Toplam fiziksel bellek\n"
         "  used      Kullanılan bellek\n"
         "  free      Boş bellek\n"
         "  shared    Paylaşılan bellek (tmpfs)\n"
         "  buff/cache  Önbellek ve tampon\n"
         "  available  Gerçekten kullanılabilir\n\n"
         "Önemli: Linux boş belleği önbellek olarak kullanır.\n"
         "\"available\" sütunu gerçek kullanılabilir belleği gösterir.\n\n"
         "Örnekler:\n"
         "  free -h           → Tek seferlik\n"
         "  free -h -s 2      → Her 2 saniyede güncelle"},
        {"uname", "Sistem bilgisi", "uname -a",
         "Sistem ve çekirdek bilgilerini gösterir.\n\n"
         "Seçenekler:\n"
         "  -a  Tüm bilgiler\n"
         "  -s  Çekirdek adı (Linux)\n"
         "  -n  Makine adı (hostname)\n"
         "  -r  Çekirdek sürümü\n"
         "  -v  Çekirdek versiyonu\n"
         "  -m  Makine mimarisi (x86_64, aarch64)\n"
         "  -o  İşletim sistemi adı\n\n"
         "Örnekler:\n"
         "  uname -a    → Tüm bilgiler\n"
         "  uname -r    → 6.1.0-18-amd64\n"
         "  uname -m    → x86_64\n\n"
         "Detaylı sistem bilgisi için:\n"
         "  lsb_release -a  → Dağıtım bilgisi\n"
         "  cat /etc/os-release → OS bilgisi"},
        {"whoami", "Kullanıcı", "whoami",
         "Geçerli kullanıcının adını gösterir.\n\n"
         "İlgili komutlar:\n"
         "  id        → UID, GID ve grup bilgileri\n"
         "  id kullanıcı → Başka kullanıcı bilgisi\n"
         "  groups    → Kullanıcının grupları\n"
         "  users     → Oturum açmış kullanıcılar\n"
         "  w         → Kullanıcılar ve aktiviteleri\n"
         "  last      → Son oturum açma kayıtları\n\n"
         "Örnekler:\n"
         "  whoami    → kullanici\n"
         "  id        → uid=1000(kullanici) gid=1000(kullanici)\n"
         "  id root   → uid=0(root) gid=0(root)"},
        {"sudo", "Yönetici çalıştır", "sudo apt update",
         "Komutu yönetici (root) yetkisiyle çalıştırır.\n\n"
         "Seçenekler:\n"
         "  -i  Root kabuğu aç\n"
         "  -s  Kabuk çalıştır\n"
         "  -u kullanıcı  Başka kullanıcı olarak çalıştır\n"
         "  -l  Yetkileri listele\n"
         "  -k  Yetki süresini sıfırla (şifre tekrar sorulur)\n"
         "  -v  Yetki süresini uzat\n\n"
         "Güvenlik:\n"
         "  /etc/sudoers dosyasında kimin ne yapabileceği belirlenir\n"
         "  visudo ile sudoers dosyası düzenlenir\n"
         "  Varsayılan olarak 15 dakika yetki süresi vardır\n\n"
         "Örnekler:\n"
         "  sudo apt update       → Paket listesini güncelle\n"
         "  sudo -i               → Root kabuğuna geç\n"
         "  sudo -u www-data ls   → www-data olarak listele\n"
         "  sudo -l               → Yetkilerini gör"},
        {"apt", "Paket yönet (Debian)", "apt install paket",
         "Debian/Ubuntu/Pardus tabanlı sistemlerde paket yöneticisi.\n\n"
         "Temel komutlar:\n"
         "  apt update            → Paket listesini güncelle\n"
         "  apt upgrade           → Tüm paketleri güncelle\n"
         "  apt full-upgrade      → Bağımlılıkları da güncelle\n"
         "  apt install paket     → Paket kur\n"
         "  apt remove paket      → Paket kaldır (config kalır)\n"
         "  apt purge paket       → Paket ve config sil\n"
         "  apt autoremove        → Gereksiz bağımlılıkları temizle\n"
         "  apt search anahtar    → Paket ara\n"
         "  apt show paket        → Paket detaylarını göster\n"
         "  apt list --installed  → Kurulu paketleri listele\n"
         "  apt list --upgradable → Güncellenebilir paketler\n\n"
         "İpucu: `apt` yerine `apt-get` scriptlerde tercih edilir.\n"
         "`apt` interaktif kullanım için daha uygun (renkli, ilerleme çubuğu)."},
        {"dnf", "Paket yönet (Fedora)", "dnf install paket",
         "Fedora/RHEL/CentOS tabanlı sistemlerde paket yöneticisi.\n\n"
         "Temel komutlar:\n"
         "  dnf update            → Tüm paketleri güncelle\n"
         "  dnf install paket     → Paket kur\n"
         "  dnf remove paket      → Paket kaldır\n"
         "  dnf search anahtar    → Paket ara\n"
         "  dnf info paket        → Paket detayları\n"
         "  dnf list installed    → Kurulu paketler\n"
         "  dnf list available    → Mevcut paketler\n"
         "  dnf history           → İşlem geçmişi\n"
         "  dnf history undo N    → N. işlemi geri al\n"
         "  dnf clean all         → Önbelleği temizle\n"
         "  dnf repolist          → Depo listesini göster\n\n"
         "Ek özellikler:\n"
         "  dnf group list        → Grup paketleri\n"
         "  dnf provides dosya    → Hangi paket sağlıyor\n"
         "  dnf download paket    → Sadece indir, kurma"},
        {"systemctl", "Servis yönet", "systemctl status sshd",
         "Systemd servis yönetimi. Linux servislerini başlatır, durdurur ve yönetir.\n\n"
         "Servis komutları:\n"
         "  systemctl start sshd      → Servisi başlat\n"
         "  systemctl stop sshd       → Servisi durdur\n"
         "  systemctl restart sshd    → Yeniden başlat\n"
         "  systemctl reload sshd     → Config yeniden yükle\n"
         "  systemctl status sshd     → Durum göster\n"
         "  systemctl enable sshd     → Otomatik başlat\n"
         "  systemctl disable sshd    → Otomatik başlatmayı kapat\n"
         "  systemctl is-active sshd  → Aktif mi kontrol et\n"
         "  systemctl is-enabled sshd → Otomatik başlatma durumu\n\n"
         "Genel komutlar:\n"
         "  systemctl list-units      → Aktif birimleri listele\n"
         "  systemctl list-unit-files → Tüm birim dosyaları\n"
         "  systemctl daemon-reload   → Config yeniden yükle\n"
         "  systemctl poweroff        → Bilgisayarı kapat\n"
         "  systemctl reboot          → Yeniden başlat"},
        {"journalctl", "Günlük", "journalctl -u nginx",
         "Systemd günlük kayıtlarını görüntüler. Tüm servis logları burada toplanır.\n\n"
         "Seçenekler:\n"
         "  -u birim     Belirli servise göre filtrele\n"
         "  -f           Canlı takip (tail -f gibi)\n"
         "  --since \"today\"   Bugünden beri\n"
         "  --since \"2024-01-01\" → Belirli tarihten beri\n"
         "  --until \"now\"  Şu ana kadar\n"
         "  -p err       Hata seviyesi ve üstü\n"
         "  -p 0..7      Öncelik seviyesi (0=emerg, 7=debug)\n"
         "  -n 50        Son 50 satır\n"
         "  --no-pager   Sayfalama olmadan göster\n"
         "  -e           Son sayfaya git\n"
         "  -b           Bu açılıştan beri\n"
         "  -b -1        Önceki açılıştan beri\n\n"
         "Örnekler:\n"
         "  journalctl -u nginx -f\n"
         "  journalctl -p err --since yesterday\n"
         "  journalctl --disk-usage  → Günlük boyutu"},
        {"man", "Kılavuz", "man ls",
         "Komut kılavuz (manual) sayfasını açar. Her komutun detaylı dokümantasyonu vardır.\n\n"
         "Man sayfası bölümleri:\n"
         "  NAME     Komut adı ve kısa açıklama\n"
         "  SYNOPSIS Kullanım formatı\n"
         "  DESCRIPTION Detaylı açıklama\n"
         "  OPTIONS  Seçenekler ve açıklamaları\n"
         "  EXAMPLES Örnekler\n"
         "  SEE ALSO İlgili komutlar\n\n"
         "Navigasyon:\n"
         "  q     → Çıkış\n"
         "  /arama → Metin ara\n"
         "  n     → Sonraki eşleşme\n"
         "  N     → Önceki eşleşme\n"
         "  space → Sayfa aşağı\n"
         "  b     → Sayfa yukarı\n"
         "  g     → Başa git\n"
         "  G     → Sona git\n\n"
         "Bölümler:\n"
         "  man 1 komut  → Kullanıcı komutları\n"
         "  man 2 komut  → Sistem çağrıları\n"
         "  man 3 komut  → C kütüphane fonksiyonları\n"
         "  man 5 dosya  → Dosya formatları\n"
         "  man -k anahtar → Konu ara (apropos)"},
        {"history", "Geçmiş", "history | grep ssh",
         "Kabuk komut geçmişini gösterir ve yönetir.\n\n"
         "Kullanım:\n"
         "  history       → Tüm geçmişi listele\n"
         "  history 20    → Son 20 komut\n"
         "  history -c    → Geçmişi temizle\n"
         "  history -d N  → N. satırı sil\n\n"
         "Geçmişten çalıştırma:\n"
         "  !n         → n. komutu çalıştır\n"
         "  !!         → Son komutu tekrar çalıştır\n"
         "  !abc       → abc ile başlayan son komutu çalıştır\n"
         "  !abc:p     → Çalıştırmadan önce göster\n"
         "  !$         → Son komutun son argümanı\n"
         "  Ctrl+R     → Geçmişte arama (interactive)\n\n"
         "Yapılandırma (~/.bashrc):\n"
         "  HISTSIZE=10000    → Bellekte tutulan\n"
         "  HISTFILESIZE=20000 → Dosyada tutulan\n"
         "  HISTCONTROL=ignoredups → Tekrarları kaydetme"},
        {"alias", "Kısayol", "alias ll='ls -la'",
         "Uzun komutlara kısa isimler (takma ad) tanımlar.\n\n"
         "Kullanım:\n"
         "  alias           → Tüm aliasları listele\n"
         "  alias ll='ls -la' → Yeni alias tanımla\n"
         "  unalias ll      → Alias kaldır\n"
         "  unalias -a      → Tüm aliasları kaldır\n\n"
         "Yaygın aliaslar:\n"
         "  alias ll='ls -la'\n"
         "  alias la='ls -A'\n"
         "  alias gs='git status'\n"
         "  alias gp='git push'\n"
         "  alias update='sudo apt update && sudo apt upgrade'\n\n"
         "Kalıcı yapmak için:\n"
         "  ~/.bashrc dosyasına ekleyin\n"
         "  source ~/.bashrc ile yeniden yükleyin\n\n"
         "İpucu: alias komutunun kendisi de aliaslanabilir!"},
        {"ln", "Bağlantı", "ln -s /hedef bağlantı",
         "Dosya bağlantısı (link) oluşturur. İki tür bağlantı vardır.\n\n"
         "Sembolik (yumuşak) bağlantı:\n"
         "  ln -s /hedef/link yol/baglanti\n"
         "  → Windows'taki kısayol gibidir\n"
         "  → Hedef silinirse bağlantı bozulur\n"
         "  → Farklı dosya sistemine bağlanabilir\n\n"
         "Sert (hard) bağlantı:\n"
         "  ln /hedef/link yol/baglanti\n"
         "  → Aynı inode'u paylaşır\n"
         "  → Hedef silinse de erişilebilir\n"
         "  → Sadece aynı dosya sisteminde\n"
         "  → Dizinlere uygulanamaz\n\n"
         "Seçenekler:\n"
         "  -f  Mevcut dosyanın üzerine yaz\n"
         "  -v  Oluşturulan bağlantıyı göster\n"
         "  -n  Sembolik bağlantıyı hedef olarak işleme\n\n"
         "Örnekler:\n"
         "  ln -s /var/www/html ~/web\n"
         "  ln -s /opt/proje/config.json ./config"},
        {"diff", "Karşılaştır", "diff dosya1 dosya2",
         "İki dosya arasındaki farkları gösterir.\n\n"
         "Seçenekler:\n"
         "  -u  Birleşik (unified) format\n"
         "  -c  Bağlam (context) formatı\n"
         "  -r  Dizinleri özyinelemeli karşılaştır\n"
         "  -y  Yan yana görünüm\n"
         "  -q  Sadece farklıysa bildir\n"
         "  -i  Büyük/küçük harf duyarsız\n"
         "  -w  Boşluk farklarını yok say\n"
         "  -B  Boş satır farklarını yok say\n\n"
         "Çıktı okuma (unified format):\n"
         "  --- dosya1  → Orijinal dosya\n"
         "  +++ dosya2  → Yeni dosya\n"
         "  -satır      → Silinen satır\n"
         "  +satır      → Eklenen satır\n"
         "   satır      → Değişmeyen satır\n\n"
         "Yama oluşturma:\n"
         "  diff -u eski.txt yeni.txt > yama.patch\n"
         "  patch eski.txt < yama.patch"},
        {"head", "İlk satırlar", "head -n 20 dosya.log",
         "Dosyanın ilk satırlarını gösterir.\n\n"
         "Seçenekler:\n"
         "  -n 20  İlk 20 satır\n"
         "  -c 100  İlk 100 bayt\n"
         "  -q  Dosya adını gösterme\n"
         "  -v  Her zaman dosya adını göster\n\n"
         "Birden fazla dosya:\n"
         "  head -n 5 dosya1.txt dosya2.txt\n"
         "  → Her dosyanın ilk 5 satırını gösterir\n"
         "  → Dosya adları başlık olarak yazılır\n\n"
         "Pipeline ile kullanım:\n"
         "  ls -la | head -n 10\n"
         "  cat /var/log/syslog | head -n 50\n\n"
         "İpucu: tail ile birleştirerek dosyanın başını ve sonunu görebilirsiniz."},
        {"tail", "Son satırlar", "tail -f /var/log/syslog",
         "Dosyanın son satırlarını gösterir. Canlı izleme için idealdir.\n\n"
         "Seçenekler:\n"
         "  -f  Canlı takip (dosya büyüdükçe gösterir)\n"
         "  -F  -f + dosya yeniden oluşturulursa takip et\n"
         "  -n 50  Son 50 satır\n"
         "  -c 100  Son 100 bayt\n"
         "  -q  Dosya adını gösterme\n"
         "  -v  Her zaman dosya adını göster\n"
         "  --pid=PID  PID bitince çık\n\n"
         "Canlı izleme:\n"
         "  tail -f /var/log/syslog\n"
         "  tail -f /var/log/nginx/access.log\n"
         "  tail -f /var/log/syslog | grep 'error'\n\n"
         "İpucu: Ctrl+C ile canlı takipten çıkabilirsiniz.\n"
         "Birden fazla dosyayı aynı anda izleyebilirsiniz:\n"
         "  tail -f /var/log/syslog /var/log/auth.log"},
        {"sort", "Sırala", "sort dosya.txt",
         "Metin dosyasının satırlarını sıralar.\n\n"
         "Seçenekler:\n"
         "  -r  Ters sıralama (Z→A, 9→0)\n"
         "  -n  Sayısal sıralama\n"
         "  -f  Büyük/küçük harf duyarsız\n"
         "  -u  Tekrarları kaldır (unique)\n"
         "  -R  Rastgele sıralama\n"
         "  -t:  Ayırıcı karakter belirt\n"
         "  -k2  2. sütuna göre sırala\n"
         "  -o dosya  Çıktıyı dosyaya yaz\n\n"
         "Örnekler:\n"
         "  sort dosya.txt              → Alfabetik sırala\n"
         "  sort -n sayilar.txt         → Sayısal sırala\n"
         "  sort -t: -k3 /etc/passwd    → 3. sütuna göre (UID)\n"
         "  sort -rn                    → Ters sayısal\n"
         "  sort dosya.txt -o sirali.txt → Dosyaya kaydet\n\n"
         "İpucu: uniq ile birleştirerek tekrarları bulun:\n"
         "  sort dosya.txt | uniq -c | sort -rn"},
        {"uniq", "Tekrarları kaldır", "sort dosya | uniq",
         "Yan yana tekrarlanan satırları filtreler. Sadece bitişik tekrarları algılar!\n\n"
         "⚠️ Önce sort kullanın, sonra uniq!\n\n"
         "Seçenekler:\n"
         "  -c  Her satırın tekrar sayısını göster\n"
         "  -d  Sadece tekrarlanan satırları göster\n"
         "  -u  Sadece benzersiz (tekrarsız) satırlar\n"
         "  -i  Büyük/küçük harf duyarsız\n"
         "  -w N  İlk N karakteri karşılaştır\n\n"
         "Örnekler:\n"
         "  sort dosya.txt | uniq          → Tekrarları temizle\n"
         "  sort dosya.txt | uniq -c       → Sayıları göster\n"
         "  sort dosya.txt | uniq -d       → Sadece tekrarlar\n"
         "  sort dosya.txt | uniq -u       → Sadece benzersizler\n\n"
         "Pratik kullanım:\n"
         "  history | awk '{print $2}' | sort | uniq -c | sort -rn\n"
         "  → En çok kullanılan komutları listele"},
        {"wc", "Satır/kelime sayısı", "wc -l dosya.txt",
         "Dosyadaki satır, kelime ve bayt sayısını sayar.\n\n"
         "Seçenekler:\n"
         "  -l  Satır sayısı (line)\n"
         "  -w  Kelime sayısı (word)\n"
         "  -c  Bayt sayısı (byte)\n"
         "  -m  Karakter sayısı (multibyte desteği)\n"
         "  -L  En uzun satır uzunluğu\n\n"
         "Çıktı formatı:\n"
         "  wc dosya.txt  → satır kelime bayt dosya\n"
         "  wc -l dosya.txt  → sadece satır sayısı\n\n"
         "Birden fazla dosya:\n"
         "  wc *.txt  → Her dosya + toplam\n\n"
         "Pipeline ile:\n"
         "  ls -1 | wc -l        → Dizindeki dosya sayısı\n"
         "  cat dosya.txt | wc -w → Kelime sayısı\n"
         "  grep 'hata' syslog | wc -l → Hata sayısı\n\n"
         "İpucu: `wc -l < dosya.txt` sadece sayıyı gösterir."},
        {"awk", "Metin işleme", "awk '{print $1}' dosya",
         "Güçlü metin işleme ve raporlama dili. Sütun bazlı işlem yapar.\n\n"
         "Temel yapı:\n"
         "  awk 'kural {eylem}' dosya\n\n"
         "Değişkenler:\n"
         "  $1, $2, ...  → Sütunlar\n"
         "  $0           → Tüm satır\n"
         "  $NF          → Son sütun\n"
         "  NF           → Sütun sayısı\n"
         "  NR           → Satır numarası\n"
         "  FS           → Ayırıcı (varsayılan: boşluk)\n\n"
         "Seçenekler:\n"
         "  -F ':'  Ayırıcı karakter belirt\n\n"
         "Örnekler:\n"
         "  awk '{print $1}' dosya         → İlk sütun\n"
         "  awk -F: '{print $1}' /etc/passwd → Kullanıcı adları\n"
         "  awk '$3 > 1000' dosya          → 3. sütun 1000'den büyük\n"
         "  awk '{sum += $1} END {print sum}' → Toplam hesapla\n"
         "  awk 'NR%2==0' dosya            → Çift satırlar\n"
         "  ps aux | awk '{print $1}' | sort | uniq -c → Kullanıcı bazlı süreç"},
        {"sed", "Akış düzenleyici", "sed 's/eski/yeni/g' dosya",
         "Stream Editor. Metin üzerinde otomatik düzenleme yapar.\n\n"
         "Temel komutlar:\n"
         "  s/eski/yeni/   → İlk eşleşmeyi değiştir\n"
         "  s/eski/yeni/g  → Tüm eşleşmeleri değiştir\n"
         "  s/eski/yeni/i  → Büyük/küçük harf duyarsız\n"
         "  d              → Satır sil\n"
         "  p              → Satır yazdır\n"
         "  a\\metin       → Satır sonuna ekle\n"
         "  i\\metin       → Satır başına ekle\n\n"
         "Adresleme:\n"
         "  5s/eski/yeni/  → Sadece 5. satırda\n"
         "  1,10s/eski/yeni/ → 1-10. satırlar arası\n"
         "  /pattern/s/eski/yeni/ → Desene uyan satırlarda\n"
         "  /pattern/d     → Desene uyan satırları sil\n\n"
         "Seçenekler:\n"
         "  -i  Dosyayı yerinde düzenle\n"
         "  -e  Birden fazla kural\n"
         "  -n  Sadece eşleşen satırları göster\n\n"
         "Örnekler:\n"
         "  sed 's/foo/bar/g' dosya\n"
         "  sed -i 's/eski/yeni/g' dosya\n"
         "  sed -e 's/a/b/' -e 's/c/d/' dosya"},
        {"cut", "Bölüm çıkar", "cut -d: -f1 /etc/passwd",
         "Satırlardan belirli bölümleri (sütunları) çıkarır.\n\n"
         "Seçenekler:\n"
         "  -d ':'  Ayırıcı karakter belirt\n"
         "  -f 1    1. sütunu al\n"
         "  -f 1,3  1. ve 3. sütunu al\n"
         "  -f 1-3  1'den 3'e kadar sütunları al\n"
         "  -c 1-5  1-5. karakterleri al\n"
         "  -c 1,5,10 → 1, 5 ve 10. karakterleri al\n"
         "  --complement  Seçilenleri hariç tut\n\n"
         "Örnekler:\n"
         "  cut -d: -f1 /etc/passwd        → Kullanıcı adları\n"
         "  cut -d: -f1,3 /etc/passwd      → Kullanıcı adı + UID\n"
         "  cut -c1-10 dosya.txt           → Her satırın ilk 10 karakteri\n"
         "  ps aux | cut -c1-20            → İlk 20 karakter\n\n"
         "İpucu: awk daha karmaşık işlemler için daha uygundur."},
        {"ping", "Bağlantı testi", "ping -c 4 google.com",
         "Ağ bağlantısını test eder. ICMP Echo Request paketleri gönderir.\n\n"
         "Seçenekler:\n"
         "  -c 4      4 paket gönder ve dur\n"
         "  -i 0.5    Paketler arası 0.5 saniye bekle\n"
         "  -s 1400   Paket boyutu (bayt)\n"
         "  -W 2      2 saniye zaman aşımı\n"
         "  -t TTL    Time-to-live değeri\n"
         "  -4        Sadece IPv4\n"
         "  -6        Sadece IPv6\n\n"
         "Çıktı okuma:\n"
         "  64 bytes from ... → Yanıt geldi\n"
         "  time=12.3 ms    → Gecikme (latency)\n"
         "  packet loss     → Kayıp paket yüzdesi\n\n"
         "Örnekler:\n"
         "  ping google.com          → Sürekli ping (Ctrl+C durdur)\n"
         "  ping -c 4 google.com     → 4 paket\n"
         "  ping -c 4 -i 0.2 site.com → Hızlı test\n"
         "  ping -s 1400 -c 4 site.com → Büyük paket testi\n\n"
         "İpucu: Yüksek gecikme (>100ms) veya paket kaybı ağ sorununa işaret eder."},
        {"ip", "Ağ yapılandırma", "ip addr show",
         "Linux ağ yapılandırma aracı. ifconfig'in modern ve daha güçlü alternatifidir.\n\n"
         "Alt komutlar:\n\n"
         "ip addr (Ağ adresleri):\n"
         "  ip addr show              → Tüm arayüzleri ve IP'leri göster\n"
         "  ip addr show eth0         → Sadece eth0 arayüzü\n"
         "  ip addr add 192.168.1.10/24 dev eth0 → IP ekle\n"
         "  ip addr del 192.168.1.10/24 dev eth0 → IP sil\n"
         "  ip addr flush dev eth0    → Tüm IP'leri temizle\n\n"
         "ip link (Arayüz yönetimi):\n"
         "  ip link show              → Tüm arayüzleri göster\n"
         "  ip link set eth0 up       → Arayüzü aktif et\n"
         "  ip link set eth0 down     → Arayüzü devre dışı bırak\n"
         "  ip link set eth0 mtu 9000 → MTU değiştir\n"
         "  ip link set eth0 promisc on → Promiscuous mod\n\n"
         "ip route (Yönlendirme tablosu):\n"
         "  ip route show             → Yönlendirme tablosu\n"
         "  ip route add default via 192.168.1.1 → Varsayılan ağ geçidi\n"
         "  ip route add 10.0.0.0/8 via 192.168.1.1 → Statik rota\n"
         "  ip route del 10.0.0.0/8   → Rota sil\n"
         "  ip route flush            → Tüm rotaları temizle\n\n"
         "ip neigh (ARP tablosu):\n"
         "  ip neigh show             → ARP tablosunu göster\n"
         "  ip neigh flush dev eth0   → ARP tablosunu temizle\n\n"
         "ip monitor (Canlı izleme):\n"
         "  ip monitor addr           → IP değişikliklerini izle\n"
         "  ip monitor route          → Rota değişikliklerini izle\n"
         "  ip monitor link           → Arayüz değişikliklerini izle\n\n"
         "Pratik örnekler:\n"
         "  ip -br addr show          → Kısa format (özet)\n"
         "  ip route get 8.8.8.8      → 8.8.8.8'e giden rota\n"
         "  ip -s link show eth0      → İstatistikler (bayt, paket)\n\n"
         "İpucu: `ip` komutu ifconfig'den daha hızlı ve daha fazla özellik sunar."},
        {"touch", "Dosya Oluşturur", "touch notlar.txt",
         "Dosya zaman damgasını değiştirir veya boş dosya oluşturur.\n\n"
         "Kullanım:\n"
         "  Dosya yoksa → Boş dosya oluşturur\n"
         "  Dosya varsa → Erişim ve değişiklik zamanını günceller\n\n"
         "Seçenekler:\n"
         "  -a  Sadece erişim zamanını değiştir\n"
         "  -m  Sadece değişiklik zamanını değiştir\n"
         "  -t YYYYAAGGssdd  Zaman belirt\n"
         "  -r ref_dosya  Referans dosyanın zamanını kullan\n"
         "  -c  Dosya yoksa oluşturma\n\n"
         "Örnekler:\n"
         "  touch yeni.txt              → Boş dosya oluştur\n"
         "  touch dosya.txt             → Zaman damgasını güncelle\n"
         "  touch -t 202401011200 dosya → 1 Ocak 2024 12:00\n"
         "  touch -r kaynak.txt hedef.txt → Kaynağın zamanını kopyala\n"
         "  touch *.txt                 → Tüm .txt dosyalarının zamanını güncelle\n\n"
         "İpucu: Birden fazla dosya oluşturmak için:\n"
         "  touch dosya1.txt dosya2.txt dosya3.txt"},
        {"tree", "Dizin ağacını gösterir", "tree /var/log",
         "Dizin yapısını ağaç formatında görselleştirir.\n\n"
         "Seçenekler:\n"
         "  -L 2      Maksimum 2 seviye derinlik\n"
         "  -a        Gizli dosyaları da göster\n"
         "  -d        Sadece dizinler (dosyalar yok)\n"
         "  -f        Tam yol göster\n"
         "  -h        Dosya boyutlarını göster\n"
         "  -p        İzinleri göster\n"
         "  -u        Sahip bilgisini göster\n"
         "  -g        Grup bilgisini göster\n"
         "  -s        Dosya boyutunu bayt cinsinden\n"
         "  -P '*.py'  Pattern ile filtrele\n"
         "  -I 'node_modules'  Hariç tut pattern\n"
         "  --dirsfirst  Dizinleri önce listele\n\n"
         "Örnekler:\n"
         "  tree                    → Mevcut dizin ağacı\n"
         "  tree -L 2 /home         → 2 seviye derinlik\n"
         "  tree -d /var            → Sadece dizinler\n"
         "  tree -P '*.cpp' src/    → Sadece .cpp dosyaları\n"
         "  tree -I 'build|*.o'     → build ve .o hariç\n\n"
         "Kurulum: Bazı sistemlerde kurulu gelmez.\n"
         "  sudo apt install tree"},
        {"file", "Dosya türünü öğren", "file image.png",
         "Dosyanın gerçek türünü belirler. Uzantıya değil içeriğe bakar.\n\n"
         "Nasıl çalışır?\n"
         "  Magic number (sihirli sayılar) ile dosya başlığını inceler\n"
         "  Dosya uzantısı yanıltıcı olsa bile doğru türü bulur\n\n"
         "Seçenekler:\n"
         "  -i  MIME türü göster (text/plain, image/png)\n"
         "  -b  Kısa format (dosya adı olmadan)\n"
         "  -z  Sıkıştırılmış dosya içeriğini göster\n"
         "  -L  Sembolik bağlantıyı takip et\n"
         "  -e ascii  Belirli testleri devre dışı bırak\n\n"
         "Örnekler:\n"
         "  file dosya.pdf          → PDF document, version 1.4\n"
         "  file resim.jpg          → JPEG image data, JFIF standard\n"
         "  file -i script.sh       → text/x-shellscript\n"
         "  file /dev/sda           → block special\n"
         "  file gizli_dosya        → ELF 64-bit (uzantısı .txt olsa bile)\n\n"
         "İpucu: Dosya uzantısı olmayan dosyaların türünü öğrenmek için idealdir."},
        {"stat", "Dosya Detayları", "stat dosya.txt",
         "Dosya hakkında detaylı meta veri bilgisi gösterir.\n\n"
         "Gösterilen bilgiler:\n"
         "  Dosya adı ve boyutu\n"
         "  Bloklar ve blok boyutu\n"
         "  Dosya türü (normal, dizin, sembolik bağlantı)\n"
         "  İzinler (octal ve okunabilir format)\n"
         "  Bağlar (hard link sayısı)\n"
         "  Sahip (UID) ve Grup (GID)\n"
         "  Erişim zamanı (atime) - son okuma\n"
         "  Değişiklik zamanı (mtime) - içerik değişikliği\n"
         "  Durum değişikliği (ctime) - metadata değişikliği\n"
         "  Doğum zamanı (birth) - oluşturma (destekleniyorsa)\n\n"
         "Seçenekler:\n"
         "  -f  Dosya sistemi bilgisi\n"
         "  -c 'format'  Formatlı çıktı\n"
         "  -t  Kısa format\n\n"
         "Formatlı çıktı örnekleri:\n"
         "  stat -c '%s' dosya.txt      → Sadece boyut\n"
         "  stat -c '%A' dosya.txt      → Sadece izinler\n"
         "  stat -c '%U' dosya.txt      → Sahip adı\n"
         "  stat -c '%n %s %A' dosya*   → Ad, boyut, izinler\n\n"
         "İpucu: Scriptlerde belirli bilgiyi almak için -c kullanın."},
        {"nslookup", "DNS sorgulama", "nslookup pardus.org.tr",
         "DNS sunucusundan alan adı (domain) sorgulama yapar.\n\n"
         "Kullanım:\n"
         "  nslookup alan_adi           → A kaydını sorgula\n"
         "  nslookup alan_adi dns_sunucu → Belirli DNS sunucusuna sor\n\n"
         "Kayıt türleri (interaktif mod):\n"
         "  set type=A      → IPv4 adresi\n"
         "  set type=AAAA   → IPv6 adresi\n"
         "  set type=MX     → Posta sunucuları\n"
         "  set type=NS     → Name server'lar\n"
         "  set type=CNAME  → Takma adlar\n"
         "  set type=TXT    → TXT kayıtları\n"
         "  set type=SOA    → Bölge bilgisi\n"
         "  set type=ANY    → Tüm kayıtlar\n\n"
         "Örnekler:\n"
         "  nslookup google.com\n"
         "  nslookup -type=MX gmail.com\n"
         "  nslookup google.com 8.8.8.8\n"
         "  nslookup\n"
         "  > set type=MX\n"
         "  > pardus.org.tr\n\n"
         "İpucu: dig komutu daha detaylı çıktı verir."},
        {"dig", "Gelişmiş DNS sorgulama", "dig pardus.org.tr",
         "Domain Information Groper. Detaylı ve esnek DNS sorgulama aracı.\n\n"
         "Temel kullanım:\n"
         "  dig alan_adi          → A kaydını sorgula\n"
         "  dig @dns_sunucu alan_adi → Belirli DNS'e sor\n"
         "  dig -t MX alan_adi    → MX kaydı sorgula\n\n"
         "Kayıt türleri:\n"
         "  -t A       → IPv4 adresi\n"
         "  -t AAAA    → IPv6 adresi\n"
         "  -t MX      → Posta sunucuları\n"
         "  -t NS      → Name server'lar\n"
         "  -t CNAME   → Takma ad\n"
         "  -t TXT     → TXT kayıtları (SPF, DKIM)\n"
         "  -t SOA     → Bölge yetkilisi\n"
         "  -t ANY     → Tüm kayıtlar\n\n"
         "Seçenekler:\n"
         "  +short     → Sadece sonuç (kısa çıktı)\n"
         "  +noall +answer → Sadece cevap bölümü\n"
         "  +trace     → Kökten itibaren izleme\n"
         "  -x IP      → Ters DNS (IP → alan adı)\n"
         "  +time=N    → Zaman aşımı (saniye)\n\n"
         "Örnekler:\n"
         "  dig google.com +short\n"
         "  dig -t MX gmail.com\n"
         "  dig @8.8.8.8 google.com\n"
         "  dig -x 8.8.8.8          → Ters DNS\n"
         "  dig google.com +trace   → DNS zincirini izle\n"
         "  dig +noall +answer google.com → Temiz çıktı"},
        {"useradd", "Yeni Kullanıcı ekleme", "useradd mehmet",
          "Sistemde yeni kullanıcı hesabı oluşturur.\n\n"
          "Seçenekler:\n"
          "  -m  Ev dizini otomatik oluştur\n"
          "  -s /bin/bash  Varsayılan kabuk belirt\n"
          "  -g ana_grup  Ana grup belirt\n"
          "  -G grup1,grup2  Ek gruplar ekle\n"
          "  -c 'Yorum'  Açıklama (gerçek ad vb.)\n"
          "  -e YYYY-AA-GG  Hesap bitiş tarihi\n"
          "  -d /özel/yol  Özel ev dizini\n"
          "  -u UID  Belirli UID ata\n"
          "  -r  Sistem kullanıcısı oluştur\n\n"
          "Kullanıcı oluşturduktan sonra:\n"
          "  passwd mehmet  → Şifre belirle\n\n"
          "Örnekler:\n"
          "  useradd -m -s /bin/bash mehmet\n"
          "  useradd -m -s /bin/bash -c 'Mehmet Yılmaz' mehmet\n"
          "  useradd -m -G sudo,users -s /bin/bash mehmet\n"
          "  useradd -r -s /usr/sbin/nologin servis\n\n"
          "İlgili komutlar:\n"
          "  userdel mehmet     → Kullanıcı sil\n"
          "  usermod -aG grup kullanıcı → Gruba ekle\n"
          "  passwd mehmet      → Şifre değiştir\n"
          "  chsh -s /bin/zsh kullanıcı → Kabuk değiştir"},
        {"basename", "Dosya adını göster", "basename /home/kullanici/belge.txt",
         "Bir dosya yolunun sadece dosya adı kısmını gösterir. Uzantıyı da isteğe bağlı kırpar.\n\n"
         "Seçenekler:\n"
         "  -a  Birden fazla yolu işle\n"
         "  -s .txt  Uzantıyı kırp\n"
         "  -z  Null ile sonlandır (find -print0 ile kullanım)\n\n"
         "Örnekler:\n"
         "  basename /usr/bin/ls              → ls\n"
         "  basename /var/log/syslog          → syslog\n"
         "  basename dosya.tar.gz .tar.gz     → dosya\n"
         "  basename -s .txt belge.txt rapor.txt → belge rapor\n\n"
         "İpucu: dirname ile ters işlemi yapar."},
        {"dirname", "Dizin yolunu göster", "dirname /home/kullanici/belge.txt",
         "Bir dosya yolunun sadece dizin kısmını gösterir.\n\n"
         "Seçenekler:\n"
         "  -z  Null ile sonlandır\n"
         "  -v  Her satırı detaylı göster (verbose)\n\n"
         "Örnekler:\n"
         "  dirname /usr/bin/ls              → /usr/bin\n"
         "  dirname /var/log/syslog          → /var/log\n"
         "  dirname /etc/nginx/nginx.conf    → /etc/nginx\n\n"
         "Pratik: cd $(dirname $(which python))  → Python dizinine git"},
        {"xargs", "Argümanları ilet", "find . -name '*.log' | xargs rm",
         "Standart girdiden (stdin) alınan verileri argüman olarak başka bir komuta iletir.\n\n"
         "Seçenekler:\n"
         "  -p  Her komuttan önce onay sor\n"
         "  -t  Komutları çalıştırmadan önce yazdır\n"
         "  -I {}  Yer tutucu belirt (dosya adı için)\n"
         "  -n 1  Her çağrıda 1 argüman kullan\n"
         "  -0  Null ile ayrılmış girdi (find -print0 ile)\n"
         "  --max-procs=4  Paralel çalıştırma\n\n"
         "Örnekler:\n"
         "  find . -name '*.bak' | xargs rm -f\n"
         "  ls *.txt | xargs -I {} mv {} /yedek/{}\n"
         "  cat url_list.txt | xargs -n1 curl -O\n"
         "  seq 1 10 | xargs -n1 echo 'Sayı:'\n\n"
         "İpucu: Dosya adlarında boşluk varsa -0 kullanın."},
        {"tee", "Çıktıyı böl", "ls -la | tee dosya_listesi.txt",
         "Komut çıktısını hem dosyaya yazar hem de ekranda gösterir. Boru hattını ikiye ayırır.\n\n"
         "Seçenekler:\n"
         "  -a  Dosyaya ekle (üzerine yazma)\n"
         "  -i  Ctrl+C sinyalini yok say\n"
         "  -p  Dosya yazma hatalarında uyar\n\n"
         "Örnekler:\n"
         "  echo 'Merhaba' | tee mesaj.txt\n"
         "  ping google.com | tee -a ping_log.txt\n"
         "  echo 'HOSTNAME,IP' | tee rapor.csv > /dev/null\n"
         "  ./script.sh | tee çıktı.log  → Çıktıyı hem izle hem kaydet\n\n"
         "İpucu: tee, shell scriptlerinde log tutmak için idealdir."},
        {"watch", "Periyodik çalıştır", "watch -n 2 df -h",
         "Bir komutu belirli aralıklarla tekrar çalıştırır ve çıktıyı canlı gösterir.\n\n"
         "Seçenekler:\n"
         "  -n 2    2 saniyede bir çalıştır\n"
         "  -d      Değişen satırları vurgula\n"
         "  -t      Başlık satırını gizle\n"
         "  -e      Hata durumunda don (freeze)\n"
         "  -b      Sadece çıktı değişirse bildir\n\n"
         "Örnekler:\n"
         "  watch -n 1 free -h           → Bellek değişimi\n"
         "  watch -d 'ls -la'            → Dosya değişikliklerini izle\n"
         "  watch -n 5 'ss -tlnp'        → Port değişiklikleri\n"
         "  watch 'ps aux | sort -nrk 4' → En çok RAM yiyenler\n\n"
         "İpucu: Ctrl+C ile durana kadar çalışır."},
        {"rsync", "Gelişmiş senkronizasyon", "rsync -av /kaynak/ /hedef/",
         "Dosya ve dizinleri akıllıca senkronize eder. Sadece değişen kısımları aktarır.\n\n"
         "Seçenekler:\n"
         "  -a      Arşiv modu (izin, sahiplik, sembolik link koru)\n"
         "  -v      Detaylı çıktı\n"
         "  -z      Sıkıştırarak aktar\n"
         "  -P      İlerleme göstergesi + kaldığından devam\n"
         "  --delete Hedefte olup kaynakta olmayanı sil\n"
         "  --exclude='*.tmp'  Belirli dosyaları hariç tut\n"
         "  -e 'ssh -p 2222'  SSH üzerinden özel port ile\n"
         "  --dry-run  Ne yapılacağını göster (gerçekten yapma)\n\n"
         "Örnekler:\n"
         "  rsync -avz /proje/ kullanici@sunucu:/yedek/\n"
         "  rsync -av --delete /kaynak/ /hedef/\n"
         "  rsync -av --exclude='node_modules' /proje/ /yedek/\n\n"
         "İpucu: scp'den daha hızlı ve akıllıdır."},
        {"cron", "Zamanlanmış görev", "crontab -e",
         "Belirli zamanlarda otomatik çalışması için görev planlar. Tarayıcıda çalışır.\n\n"
         "CRON formatı:\n"
         "  ┌───────── dakika (0-59)\n"
         "  │ ┌───────── saat (0-23)\n"
         "  │ │ ┌───────── gün (1-31)\n"
         "  │ │ │ ┌───────── ay (1-12)\n"
         "  │ │ │ │ ┌───────── haftanın günü (0-6, 0=Pazar)\n"
         "  * * * * * komut\n\n"
         "Komutlar:\n"
         "  crontab -e        → Zamanlayıcıyı düzenle\n"
         "  crontab -l        → Görevleri listele\n"
         "  crontab -r        → Tüm görevleri sil\n"
         "  crontab -u kullanıcı  → Başka kullanıcının cron'u (root)\n\n"
         "Örnekler:\n"
         "  0 9 * * * apt update          → Her gün 09:00'da\n"
         "  */30 * * * * /script.sh       → 30 dakikada bir\n"
         "  0 0 * * 1 logrotate           → Her Pazartesi 00:00\n"
         "  @reboot /script.sh            → Açılışta çalıştır\n\n"
         "İpucu: Çıktılar mail ile gönderilir. > /dev/null ile susturun."},
        {"lsblk", "Blok cihaz listele", "lsblk",
         "Diskler ve bölümler hakkında bilgi gösterir. Ağaç yapısıyla görselleştirir.\n\n"
         "Seçenekler:\n"
         "  -f  Dosya sistemi bilgisini göster\n"
         "  -m  Sahiplik ve izinleri göster\n"
         "  -l  Liste formatında (ağaç değil)\n"
         "  -t  Topoloji (sıra, döndürme hızı)\n"
         "  -d  Sadece diskler (bölümler yok)\n"
         "  -o ADI,SIZE,TIPO  Özel sütun seçimi\n"
         "  -p  Tam yol göster (/dev/sda1)\n"
         "  -J  JSON çıktı\n\n"
         "Örnekler:\n"
         "  lsblk                  → Tüm diskler\n"
         "  lsblk -f               → Hangi disk hangi format\n"
         "  lsblk -o ADI,BOYUT,TIP,NO  → Özel görünüm\n"
         "  lsblk -d -o ADI,SIZE   → Sadece diskler\n\n"
         "İpucu: fdisk /dev/sda ile detaylı bölüm düzenleme."},
        {"lsof", "Açık dosyalar", "lsof -i :80",
         "Hangi sürecin hangi dosyaları açtığını listeler. Linux'ta her şey dosyadır!\n\n"
         "Seçenekler:\n"
         "  -i :port     Belirli portu kullanan süreçler\n"
         "  -i TCP       TCP bağlantıları\n"
         "  -u kullanıcı  Kullanıcıya göre filtrele\n"
         "  -p PID       Belirli PID'nin dosyaları\n"
         "  +D /dizin    Dizindeki açık dosyalar\n"
         "  -t           Sadece PID'leri göster (script için)\n"
         "  -n           DNS çözümlemesi yapma (hızlı)\n"
         "  -P           Port isimlerini çözme\n\n"
         "Örnekler:\n"
         "  lsof | wc -l           → Açık dosya sayısı\n"
         "  lsof -i :443           → 443'ü kim kullanıyor\n"
         "  lsof -u kullanici      → Kullanıcının açık dosyaları\n"
         "  lsof /var/log/syslog   → Bu dosyayı kim kullanıyor\n\n"
         "İpucu: `lsof -i -P -n` ile hızlı ağ bağlantısı sorgusu."},
        {"ss", "Soket istatistiği", "ss -tlnp",
         "Ağ soketlerini ve bağlantıları gösterir. netstat'in modern, daha hızlı alternatifidir.\n\n"
         "Seçenekler:\n"
         "  -t  TCP soketleri\n"
         "  -u  UDP soketleri\n"
         "  -l  Dinlenen (listening) portlar\n"
         "  -n  Sayısal adres (DNS çözümlemesi yapma)\n"
         "  -p  Süreç bilgisi (PID/İsim)\n"
         "  -a  Tüm soketler (dinlenen + bağlı)\n"
         "  -s  İstatistik özeti\n"
         "  -4  Sadece IPv4\n"
         "  -6  Sadece IPv6\n"
         "  -e  Detaylı bilgi\n"
         "  -o  Zamanlayıcı bilgisi\n\n"
         "Örnekler:\n"
         "  ss -tlnp             → TCP dinlenen portlar + süreçler\n"
         "  ss -tup              → Tüm bağlı TCP/UDP\n"
         "  ss -s                → Bağlantı istatistikleri\n"
         "  ss -tlnp | grep 80   → 80'i kim dinliyor\n\n"
         "İpucu: netstat yerine ss kullanın. Daha hızlı ve güncel."},
        {"dpkg", "Debian paket yönetimi", "dpkg -i paket.deb",
         "Debian paketlerini (.deb) yönetmek için alt seviye paket yöneticisi.\n\n"
         "Temel komutlar:\n"
         "  dpkg -i paket.deb          → Paket kur\n"
         "  dpkg -r paket              → Paket kaldır (config kalır)\n"
         "  dpkg -P paket              → Paket ve config sil\n"
         "  dpkg -l                    → Tüm kurulu paketleri listele\n"
         "  dpkg -l | grep anahtar     → Paket ara\n"
         "  dpkg -s paket              → Paket durumu\n"
         "  dpkg -L paket              → Hangi dosyaları kurdu\n"
         "  dpkg -S /yol/dosya         → Dosya hangi paketten\n"
         "  dpkg --configure -a        → Yarı kurulu paketleri tamamla\n"
         "  dpkg-reconfigure paket     → Paketi yeniden yapılandır\n\n"
         "Örnekler:\n"
         "  dpkg -L nano | grep bin    → nano'nun binary'leri\n"
         "  dpkg -S /bin/ls            → ls hangi paketten\n"
         "  dpkg -l | grep ^ii | wc -l → Kurulu paket sayısı\n\n"
         "İpucu: apt daha kullanışlıdır, dpkg bağımlılıkları çözmez."},
        {"jq", "JSON işleme", "jq '.kullanicilar[].adi' data.json",
         "JSON verisini terminalde işlemek, filtrelemek ve dönüştürmek için hafif ve güçlü bir araç.\n\n"
         "Temel filtreler:\n"
         "  .           → Tüm nesne\n"
         "  .anahtar    → Belirli anahtar\n"
         "  .[].anahtar → Dizideki her öğenin anahtarı\n"
         "  .[0:2]      → İlk 2 eleman\n"
         "  select(.y > 10)  → Koşullu filtre\n\n"
         "Seçenekler:\n"
         "  -r      Raw çıktı (tırnak işareti olmadan)\n"
         "  -c      Compact (tek satırda)\n"
         "  -f dosya  Dosyadan jq programı oku\n"
         "  --arg k d  Değişken ata\n\n"
         "Örnekler:\n"
         "  curl api.site.com | jq '.'                  → Formatlı göster\n"
         "  cat data.json | jq '.products[].name'       → Tüm ürün adları\n"
         "  jq 'group_by(.kategori) | length' data.json → Gruplama\n"
         "  jq '.[] | select(.stok < 10)' data.json     → Kritik stok\n\n"
         "İpucu: API'lerden gelen JSON'u analiz etmek için mükemmel."},
        {"strace", "Sistem çağrılarını izle", "strace -e open,read ls",
         "Bir programın yaptığı tüm sistem çağrılarını (system calls) izler. Hata ayıklama için vazgeçilmezdir.\n\n"
         "Seçenekler:\n"
         "  -e syscall   Sadece belirli çağrıları göster\n"
         "  -p PID       Çalışan sürece bağlan\n"
         "  -f           Alt süreçleri de izle (fork)\n"
         "  -c           İstatistik özeti\n"
         "  -t           Zaman damgası ekle\n"
         "  -T           Her çağrının süresini göster\n"
         "  -o dosya     Çıktıyı dosyaya yaz\n"
         "  -s 100       Maksimum string uzunluğu\n"
         "  -e trace=network  Sadece ağ çağrıları\n"
         "  -e trace=file     Sadece dosya çağrıları\n\n"
         "Örnekler:\n"
         "  strace ls                   → ls'in tüm çağrıları\n"
         "  strace -e open ls /home     → Sadece open çağrıları\n"
         "  strace -p 1234              → 1234 PID'sini izle\n"
         "  strace -c firefox           → İstatistik topla\n\n"
         "İpucu: Program neden çöküyor? strace ile öğrenin!"},
        {"nc", "Ağ aracı", "nc -v google.com 80",
         "Netcat. Ağ bağlantıları için İsviçre çakısıdır. Port tarama, dosya transferi, sohbet.\n\n"
         "Seçenekler:\n"
         "  -v      Detaylı çıktı (verbose)\n"
         "  -l      Dinleme modu (server)\n"
         "  -p port  Port belirt\n"
         "  -n      DNS çözümlemesi yapma\n"
         "  -z      Sadece port tara (veri gönderme)\n"
         "  -w 5    Zaman aşımı (saniye)\n"
         "  -u      UDP (varsayılan: TCP)\n"
         "  -4      Sadece IPv4\n\n"
         "Örnekler:\n"
         "  nc -zv 192.168.1.1 1-1000 → Port tara\n"
         "  echo 'GET /' | nc google.com 80\n"
         "  nc -l -p 1234             → Sohbet server'ı\n"
         "  nc 192.168.1.10 1234      → Sohbet client'ı\n"
         "  tar -czf - /dizin | nc sunucu 9000 → Dosya transferi\n\n"
         "İpucu: netcat ile basit bir web sunucusu bile yapabilirsiniz."},
        {"tcpdump", "Ağ trafiği yakala", "tcpdump -i eth0 port 80",
         "Ağ trafiğini gerçek zamanlı yakalar ve analiz eder. Ağ sorunlarını bulmak için kullanılır.\n\n"
         "Seçenekler:\n"
         "  -i eth0    Arayüz belirt\n"
         "  -n         DNS çözümlemesi yapma\n"
         "  -X         Hex + ASCII göster\n"
         "  -A         Sadece ASCII (HTTP içerikleri)\n"
         "  -c 100     100 paket yakala ve dur\n"
         "  -w dosya   Ham çıktıyı dosyaya kaydet\n"
         "  -r dosya   Kaydedilmiş dosyayı oku\n"
         "  -s 0       Tüm paketi yakala (varsayılan: 68 bayt)\n\n"
         "Filtre örnekleri:\n"
         "  port 80                   → HTTP trafiği\n"
         "  host 192.168.1.1          → Belirli IP\n"
         "  src net 192.168.1.0/24    → Kaynak ağ\n"
         "  tcp and port 443          → HTTPS trafiği\n"
         "  not arp and not icmp      → ARP ve ping hariç\n\n"
         "Örnekler:\n"
         "  tcpdump -i eth0 -A port 80 | grep 'GET|POST|Host:'\n"
         "  tcpdump -i any -c 1000 -w trafik.pcap\n"
         "  tcpdump -r trafik.pcap port 443\n\n"
         "İpucu: Wireshark ile .pcap dosyalarını görsel analiz edin."},
        {"gpg", "Şifreleme", "gpg -c dosya.txt",
         "GNU Privacy Guard. Dosya ve mesaj şifreleme/imzalama aracı. Açık kaynak PGP uyarlamasıdır.\n\n"
         "Şifreleme:\n"
         "  gpg -c dosya.txt        → Simetrik şifrele (parola sorar)\n"
         "  gpg -e -a kullanıcı     → Açık anahtarla şifrele\n"
         "  gpg -d dosya.gpg        → Şifre çöz\n\n"
         "Anahtar yönetimi:\n"
         "  gpg --gen-key           → Yeni anahtar oluştur\n"
         "  gpg --list-keys         → Anahtarları listele\n"
         "  gpg --export -a 'Ad'    → Açık anahtarı dışa aktar\n"
         "  gpg --import anahtar.asc→ Anahtar içe aktar\n"
         "  gpg --keyserver keyserver.ubuntu.com --search-keys 'ad'\n\n"
         "İmzalama:\n"
         "  gpg --sign dosya        → İmzala (binary)\n"
         "  gpg --clearsign dosya   → İmzala (metin)\n"
         "  gpg --verify dosya.sig  → İmza doğrula\n\n"
         "Örnekler:\n"
         "  gpg -c gizli.txt          → Parola ile şifrele\n"
         "  gpg -d gizli.txt.gpg      → Çöz\n"
         "  gpg --encrypt --recipient ali@ornek.com dosya.txt\n\n"
         "İpucu: Şifrelenmiş yedekleme için idealdir."},
        {"openssl", "SSL/TLS aracı", "openssl s_client -connect google.com:443",
         "SSL/TLS protokolü ve çeşitli kriptografik işlemler için kapsamlı araç seti.\n\n"
         "Sertifika işlemleri:\n"
         "  openssl s_client -connect site.com:443  → Sertifika bilgisi\n"
         "  openssl x509 -in cert.pem -text -noout  → Sertifika oku\n"
         "  openssl s_client -showcerts -connect site.com:443 < /dev/null\n\n"
         "Anahtar ve CSR:\n"
         "  openssl genrsa -out key.pem 2048         → RSA anahtar üret\n"
         "  openssl req -new -key key.pem -out csr.pem → CSR oluştur\n"
         "  openssl req -x509 -days 365 -key key.pem -out cert.pem → Self-signed sertifika\n\n"
         "Hash ve şifreleme:\n"
         "  echo -n 'metin' | openssl dgst -sha256   → SHA-256 hash\n"
         "  openssl enc -aes-256-cbc -salt -in dosya -out dosya.enc → Şifrele\n"
         "  openssl enc -d -aes-256-cbc -in dosya.enc -out dosya → Çöz\n\n"
         "Rastgele veri:\n"
         "  openssl rand -hex 32  → 32 bayt rastgele (hex)\n"
         "  openssl rand -base64 16 → 16 bayt rastgele (base64)\n\n"
         "Örnekler:\n"
         "  openssl s_client -connect github.com:443 -tls1_3\n"
         "  openssl speed aes-256-cbc  → AES hız testi\n\n"
         "İpucu: openssl version ile sürümü kontrol edin."},
        {"git", "Sürüm kontrolü", "git status",
         "Dağıtık sürüm kontrol sistemi. kod değişikliklerini takip etmek ve ekiplerle paylaşmak için standart araç.\n\n"
         "Temel komutlar:\n"
         "  git init                  → Yeni repo başlat\n"
         "  git clone url            → Repo kopyala\n"
         "  git status               → Değişiklikleri göster\n"
         "  git add dosya            → Dosyayı stage ekle\n"
         "  git add .                → Tüm dosyaları stage ekle\n"
         "  git commit -m 'mesaj'    → Commit oluştur\n"
         "  git push origin main     → Uzaktaki repoya gönder\n"
         "  git pull                 → Uzaktaki repodan çek\n"
         "  git log --oneline        → Commit geçmişi (kısa)\n\n"
         "Dal (branch) işlemleri:\n"
         "  git branch               → Dalları listele\n"
         "  git branch yeni-dal      → Yeni dal oluştur\n"
         "  git checkout dal         → Dal değiştir\n"
         "  git merge dal            → Dalı birleştir\n"
         "  git branch -d dal        → Dal sil\n\n"
         "İleri seviye:\n"
         "  git diff                 → Değişiklikleri karşılaştır\n"
         "  git reset --hard HEAD    → Son commit'e dön\n"
         "  git stash                → Değişiklikleri geçici sakla\n"
         "  git remote -v            → Uzak repo adresleri\n"
         "  git tag v1.0             → Sürüm etiketi ekle\n\n"
         "Örnekler:\n"
         "  git add . && git commit -m 'güncelleme'\n"
         "  git log --oneline --graph --all\n"
         "  git diff --cached        → Stage edilmiş değişiklikler\n\n"
         "İpucu: .gitignore ile gereksiz dosyaları repodan hariç tutun."},
        {"make", "Derleme otomasyonu", "make",
         "Projeleri derlemek için otomasyon aracı. Makefile dosyasındaki kurallara göre çalışır.\n\n"
         "Temel kullanım:\n"
         "  make              → İlk hedefi derle\n"
         "  make all          → Tüm hedefleri derle\n"
         "  make clean        → Derleme dosyalarını temizle\n"
         "  make install      → Programı sisteme kur\n"
         "  make uninstall    → Kaldır\n"
         "  make test         → Testleri çalıştır\n\n"
         "Seçenekler:\n"
         "  -j4           4 iş parçacığı ile paralel derle\n"
         "  -f dosya      Farklı Makefile adı belirt\n"
         "  -n            Ne yapılacağını göster (çalıştırma)\n"
         "  -B            Tüm dosyaları yeniden derle\n"
         "  -C /dizin     Dizine git ve çalıştır\n\n"
         "Pratik değişkenler:\n"
         "  make CC=gcc CFLAGS='-O2'     → Derleyici belirt\n"
         "  make DESTDIR=/tmp/prefix install → Farklı yere kur\n\n"
         "Örnekler:\n"
         "  make -j$(nproc)          → Tüm çekirdeklerle derle\n"
         "  make clean && make       → Temiz derleme\n"
         "  make -n                  → Ne yapacak? göster\n\n"
         "İpucu: make -j$(nproc) ile derlemeyi hızlandırın."},
        {"which", "Komut yolunu bul", "which python3",
         "Bir komutun hangi dizinde olduğunu (tam yolunu) gösterir. PATH'te arar.\n\n"
         "Seçenekler:\n"
         "  -a        Tüm eşleşmeleri göster (ilk değil)\n"
         "  -s        Sessiz mod (çıktı yok, sadece çıkış kodu)\n\n"
         "Örnekler:\n"
         "  which bash              → /usr/bin/bash\n"
         "  which -a python         → Varsa tüm Python yolları\n"
         "  which gcc || echo 'Kurulu değil'\n\n"
         "İlgili komutlar:\n"
         "  whereis python3  → Binary, kaynak ve man sayfası\n"
         "  type ls          → Komut türünü göster\n"
         "  command -v ls    → POSIX uyumlu yol bulma\n\n"
         "İpucu: which size gerçek yolu söyler, type ise alias'ları da gösterir."},
        {"whereis", "Binary, kaynak ve man bul", "whereis python3",
         "Bir komutun binary, kaynak kod ve man sayfası dosyalarının yerini gösterir.\n\n"
         "Seçenekler:\n"
         "  -b        Sadece binary ara\n"
         "  -m        Sadece man sayfası ara\n"
         "  -s        Sadece kaynak kod ara\n"
         "  -B /dizin  Binary aranacak dizini belirt\n"
         "  -M /dizin  Man aranacak dizini belirt\n\n"
         "Örnekler:\n"
         "  whereis ls                → /usr/bin/ls /usr/share/man/man1/ls.1.gz\n"
         "  whereis -b python3        → Sadece binary\n"
         "  whereis -m git            → Sadece man sayfası\n"
         "  whereis nginx             → Nginx'in tüm dosyaları\n\n"
         "İpucu: which sadece binary yolunu, whereis daha fazlasını gösterir."},
        {"mount", "Dosya sistemi bağla", "mount /dev/sdb1 /mnt/usb",
         "Bir dosya sistemini (disk, bölüm, USB) belirli bir dizine bağlar (mount).\n\n"
         "Seçenekler:\n"
         "  -t vfat    Dosya sistemi türü belirt\n"
         "  -o ro      Salt okunur bağla\n"
         "  -o rw      Okuma-Yazma bağla\n"
         "  -o uid=1000  Sahipliği belirt\n"
         "  -o noexec  Çalıştırma iznini engelle\n"
         "  -a         /etc/fstab'daki tüm dosya sistemlerini bağla\n\n"
         "İlgili komutlar:\n"
         "  umount /mnt/usb          → Bağlı dosya sistemini çıkar\n"
         "  mount | column -t        → Tüm bağlı birimler (düzenli)\n"
         "  findmnt                  → Ağaç görünümü\n"
         "  df -h                    → Disk kullanımı\n\n"
         "Örnekler:\n"
         "  sudo mount /dev/sdb1 /mnt/usb\n"
         "  sudo mount -t iso9660 -o ro /dev/sr0 /mnt/cdrom\n"
         "  sudo mount -o uid=kullanici,gid=users /dev/sdb1 /mnt\n\n"
         "İpucu: Çıkarmadan önce mutlaka umount yapın!"},
        {"umount", "Dosya sistemi çıkar", "umount /mnt/usb",
         "Bağlı bir dosya sistemini güvenle çıkartır. Veri kaybını önlemek için çıkarmadan önce çalıştırılmalı.\n\n"
         "Seçenekler:\n"
         "  -a        Tüm dosya sistemlerini çıkar\n"
         "  -f        Zorla çıkar (meşgul dosya sistemi)\n"
         "  -l        Tembel çıkar (meşgul olmayınca)\n"
         "  -R        Alt bağlantıları da çıkar\n\n"
         "Sorun giderme:\n"
         "  umount: target is busy → Bir süreç kullanıyor\n"
         "  lsof /mnt/usb          → Hangi süreç? bul\n"
         "  fuser -km /mnt/usb     → Tüm süreçleri sonlandır\n\n"
         "Örnekler:\n"
         "  sudo umount /mnt/usb\n"
         "  sudo umount -l /mnt/usb  → Meşgulse zorla çıkar\n"
         "  sudo umount -a           → Tümünü çıkar (shutdown)\n\n"
         "İpucu: umount etmeden USB çekmeyin, veri kaybedebilirsiniz!"},
        {"uptime", "Sistem çalışma süresi", "uptime",
         "Sistemin ne kadar süredir çalıştığını, kullanıcı sayısını ve yük ortalamasını gösterir.\n\n"
         "Çıktı:\n"
         "  up 3 days, 2:15  → 3 gün 2 saat 15 dakikadır açık\n"
         "  3 users          → 3 kullanıcı oturum açmış\n"
         "  load average: 1.2, 0.8, 0.5\n\n"
         "Yük ortalaması (load average):\n"
         "  1.2 (son 1 dk)  0.8 (son 5 dk)  0.5 (son 15 dk)\n"
         "  Değer CPU çekirdek sayısına göre yorumlanır:\n"
         "  4 çekirdek için 4.0 = tam yük, >4 = aşırı yük\n\n"
         "Seçenek:\n"
         "  -p          İnsan okunabilir format (pretty)\n"
         "  -s          Sistemin başlatıldığı zamanı göster\n\n"
         "İlgili komutlar:\n"
         "  w           → uptime + kullanıcı aktiviteleri\n"
         "  top/htop    → Detaylı sistem yükü\n\n"
         "Örnekler:\n"
         "  uptime                  → 10:15 up 5 days, 2 users, load: 0.5\n"
         "  uptime -p               → up 5 days, 2 hours\n"
         "  uptime -s               → 2026-06-19 08:30:15"},
        {"hostname", "Makine adı göster", "hostname",
         "Sistemin ağ üzerindeki adını (hostname) gösterir veya değiştirir.\n\n"
         "Seçenekler:\n"
         "  -I        Tüm IP adreslerini göster\n"
         "  -i        IP adresini göster\n"
         "  -d        DNS alan adını göster\n"
         "  -f        Tam nitelikli alan adı (FQDN)\n"
         "  -A        Tüm FQDN adlarını göster\n"
         "  -s        Kısa ad (ilk noktaya kadar)\n"
         "  -y        NIS/YP alan adı\n\n"
         "Kalıcı değiştirme:\n"
         "  sudo hostnamectl set-hostname yeni-ad\n"
         "  → /etc/hostname dosyasını günceller\n\n"
         "Örnekler:\n"
         "  hostname          → pardus-laptop\n"
         "  hostname -I       → 192.168.1.10 10.0.0.5\n"
         "  hostname -f       → pardus-laptop.ev.ağı\n"
         "  hostname -i       → 127.0.1.1\n\n"
         "İpucu: hostname -I, ifconfig'e gerek kalmadan IP'yi gösterir."},
        {"printenv", "Ortam değişkenlerini göster", "printenv USER",
         "Tüm ortam değişkenlerini (environment variables) veya belirli bir değişkeni gösterir.\n\n"
         "Önemli ortam değişkenleri:\n"
         "  HOME       → Kullanıcının ev dizini (/home/kullanici)\n"
         "  USER       → Kullanıcı adı\n"
         "  PATH       → Komut arama yolları\n"
         "  SHELL      → Varsayılan kabuk (/bin/bash)\n"
         "  PWD        → Geçerli çalışma dizini\n"
         "  LANG       → Dil ve bölge ayarı\n"
         "  TERM       → Terminal türü (xterm-256color)\n"
         "  EDITOR     → Varsayılan metin düzenleyici\n\n"
         "İlgili komutlar:\n"
         "  env         → printenv ile aynı\n"
         "  set         → Tüm shell değişkenlerini gösterir\n"
         "  export AD=değer  → Yeni değişken tanımla\n"
         "  unset AD    → Değişkeni kaldır\n\n"
         "Örnekler:\n"
         "  printenv PATH              → /usr/bin:/bin:/usr/local/bin\n"
         "  printenv | grep -i proxy   → Proxy ayarlarını bul\n"
         "  export EDITOR=nano         → Varsayılan editör değiştir\n\n"
         "İpucu: printenv ile PATH'te hangi dizinler olduğunu görebilirsiniz."},
        {"type", "Komut türünü göster", "type ls",
         "Bir komutun türünü gösterir: alias, builtin, fonksiyon veya harici program.\n\n"
         "Komut türleri:\n"
         "  alias     → Kullanıcı tanımlı kısayol\n"
         "  builtin   → Kabuğun içindeki komut (cd, echo)\n"
         "  file      → Harici program (/usr/bin/ls)\n"
         "  function  → Shell fonksiyonu\n\n"
         "Seçenekler:\n"
         "  -t        Sadece türü göster (alias/builtin/file/function)\n"
         "  -p        Sadece yol göster (which gibi)\n"
         "  -a        Tüm eşleşmeleri göster\n"
         "  -f        Fonksiyon tanımını gizle\n\n"
         "Örnekler:\n"
         "  type ls          → ls is aliased to 'ls --color=auto'\n"
         "  type -t cd       → builtin\n"
         "  type -t python3  → file\n"
         "  type -a echo     → echo is a shell builtin\n\n"
         "İpucu: Bir komut nereden geliyor? type cevaplar!"},
        {"sensors", "Donanım sensörleri", "sensors",
         "CPU sıcaklığı, fan hızı, voltaj ve diğer donanım sensör değerlerini okur.\n\n"
         "Seçenekler:\n"
         "  -u        Ham değerleri göster\n"
         "  -j        JSON formatında çıktı\n"
         "  -A        Adaptör satırlarını gizle\n\n"
         "Örnek çıktı:\n"
         "  coretemp-isa-0000\n"
         "  Package id 0:  +45.0°C (high = +80.0°C, crit = +100.0°C)\n"
         "  Core 0:        +42.0°C\n"
         "  Core 1:        +44.0°C\n\n"
         "İzleme:\n"
         "  watch -n 2 sensors        → Canlı sıcaklık takibi\n"
         "  sensors | grep Core       → Sadece çekirdek sıcaklıkları\n\n"
         "Kurulum:\n"
         "  sudo apt install lm-sensors\n"
         "  sudo sensors-detect       → Sensörleri algıla\n\n"
         "İpucu: Aşırı ısınma durumunda fan temizliği veya macun yenileme gerekebilir."},
        {"sysctl", "Kernel parametreleri", "sysctl net.ipv4.ip_forward",
         "Linux çekirdek (kernel) parametrelerini okur ve çalışma anında değiştirir.\n\n"
         "Kullanım:\n"
         "  sysctl -a                    → Tüm parametreleri listele\n"
         "  sysctl parametre             → Belirli parametreyi oku\n"
         "  sysctl -w parametre=değer    → Parametre değiştir\n"
         "  sysctl -p                    → /etc/sysctl.conf'u yükle\n\n"
         "Önemli parametreler:\n"
         "  net.ipv4.ip_forward=1       → IP yönlendirme (router)\n"
         "  net.ipv6.conf.all.disable_ipv6=1  → IPv6 kapat\n"
         "  vm.swappiness=10            → Takas kullanımını azalt\n"
         "  kernel.hostname=pardus      → Hostname değiştir\n"
         "  fs.file-max=100000          → Maksimum dosya açma limiti\n\n"
         "Örnekler:\n"
         "  sysctl net.ipv4.ip_forward       → Oku: 0 veya 1\n"
         "  sudo sysctl -w net.ipv4.ip_forward=1  → Aktifleştir\n"
         "  sudo sysctl -p                    → Config dosyasını yükle\n\n"
         "İpucu: Değişiklikler kalıcı olsun istiyorsanız /etc/sysctl.conf veya /etc/sysctl.d/'a yazın."},
        {"nice", "Öncelikle çalıştır", "nice -n 10 ./script.sh",
         "Bir programı belirli bir öncelik (priority) değeriyle çalıştırır. Düşük öncelik = daha az CPU.\n\n"
         "Öncelik değerleri (niceness):\n"
         "  -20 (en yüksek öncelik)  → +19 (en düşük öncelik)\n"
         "  Varsayılan: 0\n"
         "  Sadece root düşük değer (-) verebilir\n\n"
         "İlgili komut:\n"
         "  renice -n 5 -p PID      → Çalışan sürecin önceliğini değiştir\n"
         "  renice -n -5 -u kullanici → Kullanıcının tüm süreçleri\n"
         "  top (PR ve NI sütunları) → Öncelikleri gör\n\n"
         "Örnekler:\n"
         "  nice -n 19 ./arkaplan.sh        → Arka planda düşük öncelik\n"
         "  nice -n -10 make -j4            → Daha yüksek öncelik (root)\n"
         "  renice -n 10 -p 1234            → PID 1234'ü yavaşlat\n"
         "  ps -eo pid,ni,cmd | sort -n -k2 → Önceliğe göre sırala\n\n"
         "İpucu: Uzun süren işlemleri nice ile düşük öncelikte çalıştırın, sistem yanıt vermeye devam etsin."},
        {"fdisk", "Disk bölümleme", "sudo fdisk -l",
         "Disk bölüm tablosunu görüntüleme ve düzenleme aracı. MBR ve GPT destekler.\n\n"
         "Seçenekler:\n"
         "  -l            Tüm disk bölümlerini listele\n"
         "  -b sektör     Sektör boyutu (512, 1024, 2048, 4096)\n"
         "  -u            Sektör sayılarını göster\n"
         "  -x            Ek bilgileri göster (LBA, sektör)\n\n"
         "İnteraktif komutlar (fdisk /dev/sda):\n"
         "  m    Yardım\n"
         "  p    Bölüm tablosunu göster\n"
         "  n    Yeni bölüm oluştur\n"
         "  d    Bölüm sil\n"
         "  t    Bölüm tipini değiştir\n"
         "  w    Değişiklikleri kaydet ve çık\n"
         "  q    Değişiklikleri kaydetmeden çık\n"
         "  g    Yeni GPT bölüm tablosu oluştur\n"
         "  o    Yeni MBR (DOS) bölüm tablosu oluştur\n\n"
         "Örnekler:\n"
         "  sudo fdisk -l              → Diskleri ve bölümleri listele\n"
         "  sudo fdisk /dev/sdb        → /dev/sdb'yi düzenle\n"
         "  echo -e 'g\\nn\\n\\n\\n+1G\\nw' | sudo fdisk /dev/sdb\n"
         "    → 1GB'lik GPT bölümü oluştur (script ile)\n\n"
         "⚠️ UYARI: Disk bölümleme veri kaybına neden olabilir!"},
        {"mkfs", "Dosya sistemi oluştur", "sudo mkfs.ext4 /dev/sdb1",
         "Bir disk bölümüne dosya sistemi (format) oluşturur. ext4, xfs, ntfs, vfat destekler.\n\n"
         "Dosya sistemi türleri:\n"
         "  mkfs.ext4 /dev/sdb1  → ext4 (Linux, varsayılan, günlüklü)\n"
         "  mkfs.xfs /dev/sdb1   → XFS (büyük dosyalar)\n"
         "  mkfs.btrfs /dev/sdb1 → Btrfs (snapshot, sıkıştırma)\n"
         "  mkfs.vfat /dev/sdb1  → FAT32 (USB, uyumluluk)\n"
         "  mkfs.ntfs /dev/sdb1  → NTFS (Windows)\n\n"
         "Seçenekler (ext4):\n"
         "  -L etiket     → Birim etiketi (label)\n"
         "  -m 0          → Root için ayrılan yüzde (0 = tamamı kullanılabilir)\n"
         "  -O ^has_journal  → Günlüksüz (daha hızlı ama güvensiz)\n"
         "  -E stride=32  → RAID yapılandırması\n\n"
         "Örnekler:\n"
         "  sudo mkfs.ext4 -L depo /dev/sdb1\n"
         "  sudo mkfs.vfat -n USB /dev/sdb1\n"
         "  sudo mkfs.ext4 -m 0 -L yedek /dev/sdb1\n\n"
         "⚠️ UYARI: Format tüm verileri siler! Çalıştırmadan önce emin olun.\n\n"
         "İpucu: mkfs.ext4 = mke2fs -t ext4, ikisi de aynıdır."},
        {"logrotate", "Günlük döndürme", "logrotate /etc/logrotate.conf",
         "Sistem günlük dosyalarını (log) otomatik olarak döndürür, sıkıştırır ve temizler.\n\n"
         "Yapılandırma (/etc/logrotate.d/):\n\n"
         "  /var/log/nginx/*.log {\n"
         "      daily          → Günlük döndür\n"
         "      rotate 7       → Son 7 günü sakla\n"
         "      compress       → Sıkıştır (gzip)\n"
         "      delaycompress  → 1 gün sonra sıkıştır\n"
         "      missingok      → Dosya yoksa hata verme\n"
         "      notifempty     → Boşsa döndürme\n"
         "      postrotate     → Döndürme sonrası\n"
         "          systemctl reload nginx\n"
         "      endscript\n"
         "  }\n\n"
         "Seçenekler:\n"
         "  daily/weekly/monthly  → Döndürme sıklığı\n"
         "  rotate N              → Saklanacak arşiv sayısı\n"
         "  compress/delaycompress → Sıkıştırma\n"
         "  size 100M             → 100MB olunca döndür\n"
         "  maxage 30             → 30 günden eskiyi sil\n"
         "  copytruncate          → Dosyayı taşıma, kopyala+kes (çalışan programlar için)\n\n"
         "Test:\n"
         "  sudo logrotate -d /etc/logrotate.conf  → Debug (ne yapacak?)\n"
         "  sudo logrotate -f /etc/logrotate.conf  → Zorla döndür\n\n"
         "İpucu: Çalışan servislerin log'ları için copytruncate kullanın."},
        {"parallel", "Paralel çalıştır", "parallel echo ::: 1 2 3 4",
         "Birden fazla işi aynı anda (paralel) çalıştırarak işlemleri hızlandırır.\n\n"
         "Temel kullanım:\n"
         "  parallel komut ::: arg1 arg2 arg3\n"
         "  parallel komut ::: dosya1 dosya2\n"
         "  cat liste | parallel komut\n\n"
         "Seçenekler:\n"
         "  -j 4        4 iş paralel (varsayılan: çekirdek sayısı)\n"
         "  -k           Sıralı çıktı (sırayı koru)\n"
         "  --progress   İlerleme göster\n"
         "  --dry-run    Ne yapacağını göster\n"
         "  -a dosya     Argümanları dosyadan oku\n"
         "  --bar        İlerleme çubuğu\n"
         "  --timeout 60  Zaman aşımı (saniye)\n"
         "  -j0          Maksimum paralellik (tüm çekirdekler)\n\n"
         "Yer tutucular:\n"
         "  {}           Tam argüman\n"
         "  {.}          Uzantısız ad\n"
         "  {/}          Sadece dosya adı (yolsuz)\n"
         "  {//}         Sadece dizin\n"
         "  {#}          Sıra numarası\n\n"
         "Örnekler:\n"
         "  parallel -j4 gzip ::: *.log              → 4 dosyayı aynı anda sıkıştır\n"
         "  parallel curl -O ::: url1 url2 url3      → Paralel indir\n"
         "  ls | parallel 'wc -l {} > {}.sayi'       → Her dosyaya wc uygula\n"
         "  seq 1 100 | parallel -j0 'echo {}^2 | bc' → 100 işlemi aynı anda\n\n"
         "İpucu: xargs -P4 de benzer iş yapar ama parallel çok daha güçlüdür.\n"
         "Kurulum: sudo apt install parallel"},
        {"who", "Oturum açmış kullanıcılar", "who",
         "Sisteme oturum açmış kullanıcıları, bağlantı zamanlarını ve IP adreslerini gösterir.\n\n"
         "Seçenekler:\n"
         "  -b       Sistemin son açılış zamanı\n"
         "  -r       Çalışma seviyesi (runlevel)\n"
         "  -u       Boşta kalma süresi dahil\n"
         "  -q       Sadece kullanıcı adları ve sayısı\n"
         "  -H       Sütun başlıklarını göster\n\n"
         "İlgili komutlar:\n"
         "  w        → who + kullanıcı aktiviteleri + yük\n"
         "  users    → Sadece kullanıcı adları\n"
         "  last     → Son oturum açma geçmişi\n"
         "  lastlog  → Her kullanıcının son girişi\n"
         "  finger   → Kullanıcı detay bilgisi\n\n"
          "Örnekler:\n"
          "  who                → kullanici  tty7   2026-06-24 10:15 (:0)\n"
          "  who -b             → system boot  2026-06-20 08:30\n"
          "  who -uH            → Detaylı + başlık\n"
          "  who -q             → kullanici root (2 users)\n"
          "  w                  → Detaylı kullanıcı durumu\n\n"
          "İpucu: who ve w sysadmin'ler için vazgeçilmez komutlardır."},
        {"echo", "Metin yazdır", "echo 'Merhaba Dünya'",
         "Standart çıktıya (stdout) metin yazdırır. Kabuk scriptlerinin temel yapı taşıdır.\n\n"
         "Seçenekler:\n"
         "  -n  Satır sonu eklemez (newline yok)\n"
         "  -e  Kaçış karakterlerini yorumla (\\n, \\t, \\a)\n"
         "  -E  Kaçış karakterlerini yorumlama (varsayılan)\n\n"
         "Kaçış karakterleri (-e ile):\n"
         "  \\n  Yeni satır\n"
         "  \\t  Tab (sekme)\n"
         "  \\a  Uyarı sesi (bell)\n"
         "  \\\\  Ters bölü (backslash)\n"
         "  \\e  Escape karakteri\n\n"
         "Örnekler:\n"
         "  echo 'Merhaba Dünya'           → Merhaba Dünya\n"
         "  echo -n 'giriş'                → 'giriş' (satır atlamaz)\n"
         "  echo -e 'Satır1\\nSatır2'      → İki satır\n"
         "  echo \"Deger: $HOME\"            → Değişken genişletme\n"
         "  echo $?                         → Son komutun çıkış kodu\n\n"
         "İpucu: Tek tırnak değişken genişletmez, çift tırnak genişletir."},
        {"date", "Tarih ve saat", "date +%Y-%m-%d",
         "Sistem tarih ve saatini gösterir veya ayarlar.\n\n"
         "Format belirteçleri:\n"
         "  %Y  2026 (yıl)\n"
         "  %m  01-12 (ay)\n"
         "  %d  01-31 (gün)\n"
         "  %H  00-23 (saat)\n"
         "  %M  00-59 (dakika)\n"
         "  %S  00-59 (saniye)\n"
         "  %A  Pazartesi...Pazar (tam gün adı)\n"
         "  %B  Ocak...Aralık (tam ay adı)\n"
         "  %u  1-7 (haftanın günü, 1=Pazartesi)\n"
         "  %W  Yılın haftası (00-53)\n"
         "  %j  Yılın günü (001-366)\n"
         "  %s  Unix zaman damgası (saniye)\n\n"
         "Seçenekler:\n"
         "  -d 'string'  Gelecek/geçmiş tarih göster\n"
         "  -u           UTC saat diliminde göster\n"
         "  -r dosya     Dosyanın son değişiklik zamanı\n\n"
         "Örnekler:\n"
         "  date                            → Pzt 24 Haz 10:15:00 +03 2026\n"
         "  date +%Y-%m-%d                  → 2026-06-24\n"
         "  date -d 'yesterday' +%A         → Dünün gün adı\n"
         "  date -d 'next friday' +%d.%m.%Y → Gelecek Cuma\n"
         "  date -r metin.txt               → Dosyanın zamanı\n"
         "  date +%s                        → Unix timestamp\n\n"
         "İpucu:  Tarih hesaplama için date -d '@zaman_damgası' kullanın."},
        {"cal", "Takvim göster", "cal 2026",
         "Terminalde takvim görüntüler. Basit ve kullanışlı bir takvim aracı.\n\n"
         "Seçenekler:\n"
         "  -1       Tek aylık gösterim (varsayılan)\n"
         "  -3       Üç aylık (önceki/bu/sonraki ay)\n"
         "  -y       Tüm yıl gösterimi\n"
         "  -j       Jülyen takvimi (yılın günü)\n"
         "  -m       Pazartesi gün başlangıcı\n"
         "  -s       Pazar gün başlangıcı (varsayılan)\n"
         "  -w       Hafta numaralarını göster\n\n"
         "Örnekler:\n"
         "  cal                     → Bu ay\n"
         "  cal 2026                → 2026 yılı takvimi\n"
         "  cal -3                  → 3 aylık görünüm\n"
         "  cal -A 1 -B 1           → Önceki/sonraki ay ile\n"
         "  cal 5 1995              → Mayıs 1995\n"
         "  ncal                    → Dikey takvim (alternatif)\n\n"
         "İpucu: cal -3 haftalık planlama için idealdir."},
        {"bc", "Hesap makinesi", "echo '3.14 * 5^2' | bc",
         "Komut satırı hesap makinesi. Karmaşık matematiksel işlemler yapabilir.\n\n"
         "Temel işlemler:\n"
         "  +    Toplama\n"
         "  -    Çıkarma\n"
         "  *    Çarpma\n"
         "  /    Bölme\n"
         "  ^    Üs alma\n"
         "  %    Mod alma (kalan)\n\n"
         "Fonksiyonlar (-l ile):\n"
         "  s(x)  Sinüs (radyan)\n"
         "  c(x)  Kosinüs (radyan)\n"
         "  a(x)  Arktanjant\n"
         "  l(x)  Doğal logaritma\n"
         "  e(x)  Üstel fonksiyon\n"
         "  sqrt(x) Karekök\n\n"
         "Özel değişkenler:\n"
         "  scale  Ondalık basamak sayısı\n"
         "  ibase  Giriş tabanı (2-16)\n"
         "  obase  Çıkış tabanı (2-16)\n\n"
         "Örnekler:\n"
         "  echo 'scale=2; 22/7' | bc       → 3.14\n"
         "  echo 'obase=2; 255' | bc         → 11111111 (binary)\n"
         "  echo 'sqrt(144)' | bc -l         → 12\n"
         "  bc <<< 'scale=4; (3+5)*2'       → 16 (heredoc ile)\n\n"
         "İpucu: bc -l ile matematiğe başlayın."},
        {"seq", "Sayı dizisi", "seq 1 5",
         "Ardışık sayılardan oluşan bir dizi üretir. Döngülerde ve testlerde kullanılır.\n\n"
         "Kullanım:\n"
         "  seq SON               → 1'den SON'a kadar\n"
         "  seq BAŞLA SON          → BAŞLA'dan SON'a kadar\n"
         "  seq BAŞLA ADIM SON     → BAŞLA'dan SON'a ADIM atlayarak\n\n"
         "Seçenekler:\n"
         "  -s 'string'  Sayılar arasına string koy (varsayılan: \\n)\n"
         "  -w           Sıfır ile sağa hizala (eşit genişlik)\n"
         "  -f 'format'  printf benzeri format belirt\n\n"
         "Örnekler:\n"
         "  seq 5                      → 1 2 3 4 5\n"
         "  seq 0 2 10                 → 0 2 4 6 8 10\n"
         "  seq -s ', ' 1 5            → 1, 2, 3, 4, 5\n"
         "  seq -w 1 10                → 01 02 03 ... 10\n"
         "  seq -f '%.2f' 1 0.5 3     → 1.00 1.50 ... 3.00\n"
         "  for i in $(seq 1 10); do echo $i; done\n\n"
         "İpucu: Bash'te {1..5} de benzer iş yapar ama seq daha esnektir."},
        {"tr", "Karakter dönüştür", "echo 'merhaba' | tr 'a' 'e'",
         "Karakterleri değiştirir, siler veya sıkıştırır. Boru hattında metin dönüşümü için idealdir.\n\n"
         "Kullanım:\n"
         "  tr KÜME1 KÜME2          → KÜME1'deki karakterleri KÜME2'ye çevir\n"
         "  tr -d KÜME              → KÜME'deki karakterleri sil\n"
         "  tr -s KÜME              → Tekrarlanan karakterleri tekle\n\n"
         "Seçenekler:\n"
         "  -d  Silme modu\n"
         "  -s  Sıkıştırma (tekrarları tekle)\n"
         "  -c  KÜME'nin tümleyeni (tersi)\n"
         "  -t  Kümeleri eşit uzunluğa kırp\n\n"
         "Özel ifadeler:\n"
         "  [:lower:]  Küçük harfler\n"
         "  [:upper:]  Büyük harfler\n"
         "  [:digit:]  Rakamlar\n"
         "  [:space:]  Boşluk karakterleri\n"
         "  [:alpha:]  Harfler\n"
         "  [:punct:]  Noktalama işaretleri\n\n"
         "Örnekler:\n"
         "  echo 'merhaba' | tr 'a' 'e'             → merhebe\n"
         "  echo 'MERHABA' | tr 'A-Z' 'a-z'         → merhaba\n"
         "  echo 'Merhaba   Dünya' | tr -s ' '      → Merhaba Dünya\n"
         "  echo 'user:pass:123' | tr -d ':'        → userpass123\n"
         "  cat metin.txt | tr -d '\\r'              → Windows satır sonu temizle\n"
         "  echo 'hello' | tr '[:lower:]' '[:upper:]' → HELLO\n\n"
         "İpucu: tr -d '\\n' ile satırları birleştirebilirsiniz."},
        {"tac", "Tersine cat", "tac dosya.txt",
         "Dosyayı satırlarını ters çevirerek gösterir (cat'in tersi).\n\n"
         "Seçenekler:\n"
         "  -b       Ayırıcıyı satır başına koy (varsayılan: satır sonu)\n"
         "  -r       Ayırıcıyı regex olarak yorumla\n"
         "  -s AYIRICI  Ayırıcı belirt (varsayılan: \\n)\n\n"
         "Örnekler:\n"
         "  tac log.txt                → En son satır ilk sırada\n"
         "  tac dosya.txt | head -n 5  → Son 5 satır\n"
         "  seq 1 5 | tac              → 5 4 3 2 1\n"
         "  tac -s ':' /etc/passwd     → ':' ayırıcısına göre ters çevir\n\n"
         "Pratik kullanım:\n"
         "  Son log kayıtlarını görmek için: tac /var/log/syslog | less\n"
         "  Bir CSV'yi ters sıralamak için: tac veri.csv\n\n"
         "İpucu: Son satırı görmek için tail yerine tac | head de kullanılabilir."},
        {"nl", "Satır numarala", "nl -ba dosya.txt",
         "Dosya satırlarını numaralandırır. Kod ve metin belgeleri için idealdir.\n\n"
         "Seçenekler:\n"
         "  -b st       Sadece boş olmayan satırları numarala (varsayılan)\n"
         "  -b a        Tüm satırları numarala\n"
         "  -f st       Dipnot stilleri (footer)\n"
         "  -h st       Başlık stilleri (header)\n"
         "  -i SAYI     Artış miktarı (varsayılan: 1)\n"
         "  -l SAYI     Kaç satırda bir numarala\n"
         "  -n ln       Sola hizala\n"
         "  -n rn       Sağa hizala (varsayılan)\n"
         "  -n rz       Sıfırla doldur (001, 002...)\n"
         "  -p          Sayfalar arası numaralamayı sıfırlama\n"
         "  -s STRING   Numaradan sonraki ayırıcı\n"
         "  -w SAYI     Numaralandırma genişliği\n\n"
         "Örnekler:\n"
         "  nl dosya.txt                → Varsayılan numaralama\n"
         "  nl -ba dosya.txt            → Tüm satırları numarala\n"
         "  nl -n rz -w 3 dosya.txt     → 001, 002... formatında\n"
         "  nl -s '. ' -w 2 dosya.txt   → 1. madde, 2. madde...\n"
         "  cat -n dosya.txt            → nl -b a ile aynı (cat'in -n'i)\n\n"
         "İpucu: Kaynak kod dökümanlarında satır referansı için kullanın."},
        {"column", "Sütunlara ayır", "column -t -s ':' /etc/passwd",
         "Metni sütunlu tablo halinde düzenler. Çıktıyı okunabilir hale getirir.\n\n"
         "Seçenekler:\n"
         "  -t          Tablo formatında düzenle\n"
         "  -s AYIRICI  Sütun ayırıcı (varsayılan: boşluk)\n"
         "  -c SAYI     Ekran genişliği (sütun sayısı)\n"
         "  -x          Sütunları doldur (satır bazlı değil)\n"
         "  -n          Boş girişleri atla\n\n"
         "Örnekler:\n"
         "  column -t -s ':' /etc/passwd     → /etc/passwd'yi düzenli göster\n"
         "  mount | column -t                → mount çıktısını düzenle\n"
         "  ps aux | column -t               → Süreç listesini düzenle\n"
         "  cat veri.csv | column -t -s ','  → CSV'yi tablo olarak göster\n"
         "  df -h | column -t                → Disk kullanım tablosu\n"
         "  ls -la | column -t               → Dosya listesini düzenle\n\n"
         "İpucu: Herhangi bir komutun çıktısını güzelleştirmek için pipe ile kullanın."},
        {"paste", "Satırları birleştir", "paste -d ',' dosya1 dosya2",
         "Birden fazla dosyayı satır satır birleştirir.\n\n"
         "Seçenekler:\n"
         "  -d AYIRICI  Sütun ayırıcı (varsayılan: tab)\n"
         "  -s          Dosyaları alt alta değil, yan yana birleştir\n"
         "  -z          Null ile sonlandırılmış girdi\n\n"
         "Örnekler:\n"
         "  paste isimler.txt maaslar.txt        → İki dosyayı yan yana\n"
         "  paste -d ',' ad soyad eposta         → CSV oluştur\n"
         "  paste -d '|' *.txt                    → Pipe ile ayırarak birleştir\n"
         "  seq 1 5 | paste -s -d '+'            → 1+2+3+4+5 (toplamaya hazır)\n"
         "  paste -s -d '\\t' dosya.txt           → Tüm satırları tek satır yap\n\n"
         "Pratik:\n"
         "  seq 1 10 | paste -s -d '+' | bc      → 1'den 10'a toplam\n\n"
         "İpucu: paste, pr ve column ile birlikte metin düzenleme araçlarının temelidir."},
        {"expand", "Sekmeleri boşluğa çevir", "expand -t 4 dosya.txt",
         "Sekme (tab) karakterlerini boşluk karakterlerine dönüştürür.\n\n"
         "Seçenekler:\n"
         "  -t N      Sekme duraklarını N sütunda bir ayarla (varsayılan: 8)\n"
         "  -t L1,L2  Özel sekme durakları belirt\n"
         "  -i        Sadece satır başındaki sekmeleri dönüştür\n"
         "  --initial -i ile aynı\n\n"
         "Tersi: unexpand (boşlukları sekmeye çevirir)\n"
         "  unexpand -t 4 dosya.txt              → 4'lük boşlukları sekmeye çevir\n\n"
         "Örnekler:\n"
         "  expand -t 4 kod.c                   → 4 boşluk = 1 sekme\n"
         "  expand -t 1,5,9 dosya.txt            → Özel sekme durakları\n"
         "  expand -i yorum.txt                  → Sadece satır başı sekmeler\n"
         "  cat dosya.txt | unexpand -a          → Tüm boşlukları sekmeye çevir\n\n"
         "İpucu: Kod paylaşırken sekmeleri boşluğa çevirmek uyumluluğu artırır."},
        {"dd", "Dosya kopyala/dönüştür", "dd if=/dev/zero of=dosya bs=1M count=10",
         "Düşük seviyeli dosya kopyalama ve dönüştürme aracı. Blok bazlı çalışır.\n\n"
         "Parametreler:\n"
         "  if=GIRDI   Kaynak dosya (input file)\n"
         "  of=CIKTI   Hedef dosya (output file)\n"
         "  bs=BOYUT   Blok boyutu (512, 1K, 1M, 1G)\n"
         "  count=N    Kopyalanacak blok sayısı\n"
         "  skip=N     Kaynakta N blok atla\n"
         "  seek=N     Hedefte N blok atla\n"
         "  conv=...   Dönüştürme seçenekleri\n\n"
         "conv seçenekleri:\n"
         "  conv=notrunc  Hedefi kesme (truncate yapma)\n"
         "  conv=fsync    Fiziksel yazmayı bekle\n"
         "  conv=noerror  Hataları yoksay (kurtarma için)\n"
         "  conv=sync     Her bloğu tam doldur\n\n"
         "Blok boyutları:\n"
         "  512   512 bayt (1 sektör)\n"
         "  1K    1024 bayt\n"
         "  1M    1048576 bayt\n\n"
         "Örnekler:\n"
         "  dd if=/dev/zero of=dosya bs=1M count=10  → 10MB dosya oluştur\n"
         "  dd if=/dev/sda of=disk_yedek.img bs=4M   → Disk yedekle\n"
         "  dd if=disk_yedek.img of=/dev/sda bs=4M   → Diski geri yükle\n"
         "  dd if=/dev/urandom of=guvenli_dosya bs=1M count=1  → Rastgele veri\n"
         "  dd if=disk.img bs=512 count=1 | hexdump  → MBR'ı oku\n\n"
         "⚠️ UYARI: dd ile disk yazarken çok dikkatli olun, yanlış hedef tüm veriyi silebilir!"},
        {"shred", "Güvenli sil", "shred -n 3 gizli_dosya.txt",
         "Dosyayı üzerine rastgele veriler yazarak güvenli şekilde siler. Geri getirilemez hale getirir.\n\n"
         "Seçenekler:\n"
         "  -n N       Dosyayı N defa üzerine yaz (varsayılan: 3)\n"
         "  -z         Son olarak sıfır yazar (gizleme)\n"
         "  -u         Silme işleminden sonra dosyayı da sil\n"
         "  -f         Salt okunur dosyaları zorla sil\n"
         "  -s BOYUT   Son parçayı belirtilen boyuta kes\n"
         "  -v         İşlem detaylarını göster\n\n"
         "Örnekler:\n"
         "  shred gizli.txt                     → 3 defa üzerine yaz\n"
         "  shred -n 5 -z -u hesap.txt          → 5 defa + sıfır + sil\n"
         "  shred -v -n 2 *.log                 → Log dosyalarını güvenli sil\n"
         "  find . -name '*.secret' -exec shred -u {} \\;  → Tüm gizli dosyaları temizle\n\n"
         "⚠️ NOT: SSD'lerde wear-leveling nedeniyle tam silme garantisi yoktur.\n"
         "Gerçekten güvenli silme için: sudo blkdiscard /dev/sda (SSD için).\n\n"
         "İpucu: rm'den farkı, veriyi kurtarılamaz hale getirmesidir."},
        {"split", "Dosya böl", "split -l 10 buyuk_dosya.txt bolum_",
         "Büyük dosyaları daha küçük parçalara böler.\n\n"
         "Seçenekler:\n"
         "  -l N       Her parçaya N satır (varsayılan: 1000)\n"
         "  -b BOYUT   Boyuta göre böl (1K, 1M, 1G)\n"
         "  -n N       N sayıda parçaya böl\n"
         "  -d         Sayısal ek kullan (aa, ab yerine 00, 01)\n"
         "  -e         Boş parçaları atla\n"
         "  -a N       Ek uzunluğu (varsayılan: 2)\n\n"
         "Birleştirme:\n"
         "  cat bolum_* > orijinal_dosya   → Parçaları birleştir\n\n"
         "Örnekler:\n"
         "  split -l 50 buyuk.csv parca_          → 50'şer satırlık parçalar\n"
         "  split -b 100M video.mp4 parcavideo_   → 100MB'lık parçalar\n"
         "  split -n 5 dosya.txt parca_           → 5 eşit parça\n"
         "  split -d -a 3 log.txt log_            → log_000, log_001...\n"
         "  cat parca_* > birlesik.txt            → Parçaları birleştir\n\n"
         "İpucu: Büyük log dosyalarını veya CSV'leri bölmek için idealdir."},
        {"env", "Ortam değişkenleri", "env | grep PATH",
         "Tüm ortam değişkenlerini listeler veya belirli bir ortamda komut çalıştırır.\n\n"
         "Kullanım:\n"
         "  env                    → Tüm değişkenleri listele\n"
         "  env DEĞİŞKEN=değer komut  → Değişken ekleyerek komut çalıştır\n"
         "  env -i komut           → Temiz ortamda çalıştır\n"
         "  env -u DEĞİŞKEN komut  → Değişkeni kaldırarak çalıştır\n\n"
         "Önemli değişkenler:\n"
         "  HOME     → Ev dizini\n"
         "  PATH     → Komut arama yolları\n"
         "  USER     → Kullanıcı adı\n"
         "  SHELL    → Varsayılan kabuk\n"
         "  LANG     → Dil ayarı\n"
         "  PWD      → Geçerli dizin\n"
         "  TERM     → Terminal türü\n\n"
         "Örnekler:\n"
         "  env | grep -i path                → PATH değişkenini bul\n"
         "  env LANG=tr_TR.UTF-8 cal          → Türkçe takvim\n"
         "  env -i bash                       → Sıfır ortamla bash başlat\n"
         "  env | sort                        → Alfabetik sıralı liste\n"
         "  env | wc -l                       → Değişken sayısı\n\n"
         "İpucu: printenv env ile aynı işi yapar."},
        {"id", "Kullanıcı kimliği", "id",
         "Geçerli kullanıcının UID, GID ve grup üyeliklerini gösterir.\n\n"
         "Seçenekler:\n"
         "  -u       Sadece UID göster\n"
         "  -g       Sadece birincil GID göster\n"
         "  -G       Tüm grup ID'lerini göster\n"
         "  -n       Sayısal değer yerine isim göster (-u, -g, -G ile)\n"
         "  -r       Gerçek (real) kimlik (etkin değil)\n"
         "  -Z       SELinux güvenlik bağlamı\n\n"
         "Örnekler:\n"
         "  id                          → uid=1000(kullanici) gid=1000(...) grupları=...\n"
         "  id -u                       → 1000\n"
         "  id -un                      → kullanici\n"
         "  id root                     → uid=0(root) gid=0(root) grupları=0(root)\n"
         "  id -nG kullanici            → kullanici sudo adm ...\n\n"
         "İpucu: whoami sadece kullanıcı adını, id tüm detayları gösterir."},
        {"groups", "Kullanıcı grupları", "groups kullanici",
         "Bir kullanıcının ait olduğu grupları listeler.\n\n"
         "Kullanım:\n"
         "  groups              → Geçerli kullanıcının grupları\n"
         "  groups kullanici    → Belirtilen kullanıcının grupları\n\n"
         "İlgili komutlar:\n"
         "  id              → UID, GID ve grupları detaylı gösterir\n"
         "  groupadd        → Yeni grup oluştur\n"
         "  groupdel        → Grup sil\n"
         "  usermod -aG     → Kullanıcıyı gruba ekle\n"
         "  gpasswd -d      → Kullanıcıyı gruptan çıkar\n\n"
         "Örnekler:\n"
         "  groups                       → kullanici sudo adm dip ...\n"
         "  groups root                  → root\n"
         "  groups www-data              → www-data\n"
         "  id -nG kullanici             → groups ile aynı\n\n"
         "İpucu: Bir kullanıcının hangi yetkilere sahip olduğunu anlamak için groups kullanın."},
        {"last", "Son giriş kayıtları", "last -n 10",
         "Sisteme en son giriş yapan kullanıcıları listeler. Güvenlik denetimi için önemlidir.\n\n"
         "Seçenekler:\n"
         "  -n SAYI     Gösterilecek kayıt sayısı\n"
         "  -i          IP adresini sayısal göster\n"
         "  -R          Hostname gizle\n"
         "  -x          Sistem açılış/kapanış kayıtları\n"
         "  -d          IP'yi DNS'e çözümle\n"
         "  -f dosya    Belirtilen log dosyasını oku (wtmp)\n"
         "  -p ZAMAN    Belirtilen zamandaki kullanıcıyı göster\n"
         "  -s ZAMAN    Belirtilen zamandan itibaren\n"
         "  -t ZAMAN    Belirtilen zamana kadar\n\n"
         "Sütunlar:\n"
         "  Kullanıcı adı, terminal, IP/hostname, giriş zamanı, çıkış zamanı, oturum süresi\n\n"
         "Örnekler:\n"
         "  last -n 5                   → Son 5 giriş\n"
         "  last -x                     → Sistem açılış/kapanış kayıtları\n"
         "  last -i                     → IP'leri sayısal göster\n"
         "  last reboot                 → Sadece yeniden başlatmalar\n"
         "  last kullanici              → Belirli kullanıcının kayıtları\n"
         "  last -s yesterday           → Dünden beri\n\n"
         "İlgili: lastb (başarısız giriş denemeleri), w (şu anki kullanıcılar)."},
        {"w", "Kim çevrimiçi", "w",
         "Sisteme giriş yapmış kullanıcıları ve ne yaptıklarını gösterir. who'dan daha detaylıdır.\n\n"
         "Çıktı sütunları:\n"
         "  USER      Kullanıcı adı\n"
         "  TTY       Terminal\n"
         "  FROM      Uzak IP (uzaktan bağlandıysa)\n"
         "  LOGIN@    Giriş zamanı\n"
         "  IDLE      Boşta kalma süresi\n"
         "  JCPU      Tüm süreçlerin CPU süresi\n"
         "  PCPU      Mevcut sürecin CPU süresi\n"
         "  WHAT      Şu an çalıştırdığı komut\n\n"
         "Seçenekler:\n"
         "  -h        Başlık satırını gizle\n"
         "  -s        Kısa format (IDLE/LOGIN/JCPU/PCPU yok)\n"
         "  -i        IP adresini sayısal göster\n"
         "  -u        Kullanıcı adına göre filtrele\n"
         "  -o        Eski (kısa) format\n\n"
         "Örnekler:\n"
         "  w                           → Tüm kullanıcılar\n"
         "  w -s                        → Kısa format\n"
         "  w kullanici                 → Belirli kullanıcı\n"
         "  w -h                        → Başlıksız (script için)\n\n"
         "İpucu: who sadece kullanıcı listesi, w kullanıcıların aktivitelerini gösterir."},
        {"sleep", "Bekle", "sleep 5",
         "Belirtilen süre kadar bekler (işlemi duraklatır). Scriptlerde zamanlama için kullanılır.\n\n"
         "Süre birimleri:\n"
         "  sleep 5         5 saniye (varsayılan)\n"
         "  sleep 5s        5 saniye\n"
         "  sleep 2m        2 dakika\n"
         "  sleep 3h        3 saat\n"
         "  sleep 1d        1 gün\n"
         "  sleep 0.5       0.5 saniye\n\n"
         "Seçenek:\n"
         "  sleep --help    Yardım\n\n"
         "Örnekler:\n"
         "  sleep 5 && echo '5 saniye geçti'       → 5sn bekle, mesaj yazdır\n"
         "  sleep 0.1                              → 100ms bekle\n"
         "  while true; do sleep 1; uptime; done    → Her saniye uptime göster\n"
         "  for i in {1..5}; do echo $i; sleep 1; done  → 5'e kadar say\n"
         "  sleep $((RANDOM%10))                    → Rastgele bekle (0-9sn)\n\n"
         "İpucu: Scriptlerde sleep yerine timeout da kullanılabilir."},
        {"yes", "Evet de", "yes | apt-get install paket",
         "Sürekli olarak 'y' veya belirtilen metni yazdırır. Otomasyon için kullanılır.\n\n"
         "Kullanım:\n"
         "  yes                 → Sürekli 'y' yazdırır (Ctrl+C ile durur)\n"
         "  yes 'metin'         → Sürekli 'metin' yazdırır\n"
         "  yes | komut         → Komuta sürekli 'y' gönderir\n\n"
         "Seçenek:\n"
         "  --help      Yardım\n\n"
         "Örnekler:\n"
         "  yes | apt-get install paket         → Tüm sorulara 'y' yanıtı\n"
         "  yes 'Evet' | head -n 5              → 5 defa 'Evet' yaz\n"
         "  yes '' | head -n 100                → 100 boş satır\n"
         "  yes | dpkg --configure -a           → Paket yapılandırmasını otomatikleştir\n"
         "  tr '\\n' ',' < <(yes '1' | head -n 5) → 1,1,1,1,1 üret\n\n"
         "⚠️ DİKKAT: yes | dangerous_komut ciddi hasara yol açabilir!\n\n"
         "İpucu: echo 'y\\ny\\ny' | komut ile de aynı iş yapılabilir."},
        {"shuf", "Rastgele sırala", "shuf -n 3 dosya.txt",
         "Girdiyi rastgele sıralar veya rastgele seçim yapar. Karma (shuffle) işlemi için idealdir.\n\n"
         "Seçenekler:\n"
         "  -n SAYI     Seçilecek satır sayısı\n"
         "  -e arg...   Argümanları girdi olarak kullan\n"
         "  -i BAS-SON  Sayı aralığı (shuf -i 1-10 gibi)\n"
         "  -o DOSYA    Çıktıyı dosyaya yaz\n"
         "  -r          Tekrarlı seçime izin ver\n"
         "  --random-source DOSYA  Rastgelelik kaynağı\n\n"
         "Örnekler:\n"
         "  shuf dosya.txt                      → Satırları karıştır\n"
         "  shuf -n 1 dosya.txt                 → Rastgele bir satır seç\n"
         "  shuf -e kırmızı mavi yeşil          → Renkleri karıştır\n"
         "  shuf -i 1-50 -n 6                   → 1-50 arası 6 rastgele sayı\n"
         "  shuf -r -n 10 -e A B C D            → 10 kere rastgele seç\n"
         "  sort -R dosya.txt                   → shuf alternatifi\n\n"
         "Pratik:\n"
         "  Öğrenci listesini karıştırmak: shuf ogrenciler.txt\n"
         "  Rastgele şifre: tr -dc A-Za-z0-9 < /dev/urandom | head -c 12\n\n"
         "İpucu: Oyunlarda ve çekilişlerde rastgele seçim için idealdir."},
        {"strings", "Metinleri çıkar", "strings /bin/ls | head -20",
         "Binary dosyalardan okunabilir metin dizelerini çıkarır.\n\n"
         "Seçenekler:\n"
         "  -n UZUNLUK  Minimum string uzunluğu (varsayılan: 4)\n"
         "  -f          Dosya adını da göster\n"
         "  -e KODLAMA  Karakter kodlaması (s, S, b, l, L)\n"
         "  -o          Offset (dosya içindeki konum) göster\n"
         "  -t RADİKS   Offset formatı (d=decimal, o=octal, x=hex)\n"
         "  -a          Tüm dosyayı tara (varsayılan)\n"
         "  --help      Yardım\n\n"
         "Örnekler:\n"
         "  strings program                     → Binary'deki metinleri göster\n"
         "  strings /bin/ls | grep -i error     → Hata mesajlarını bul\n"
         "  strings -n 10 program               → En az 10 karakterli string'ler\n"
         "  strings -o program | head           → Offset'leri ile göster\n"
         "  strings library.so | grep version   → Kütüphane sürümünü bul\n\n"
         "Pratik kullanım:\n"
         "  Sürüm bilgisi: strings program | grep -i 'version\\|sürüm'\n"
         "  Gizli anahtar: strings program | grep -i 'key\\|secret\\|token'\n"
         "  Yardım metni: strings -n 20 program | grep -E '^[A-Z]'\n\n"
         "İpucu: Bir binary'nin ne işe yaradığını anlamak için strings kullanın."},
        {"lscpu", "İşlemci bilgisi", "lscpu",
         "CPU/işlemci mimarisi ve özellikleri hakkında detaylı bilgi verir.\n\n"
         "Gösterilen bilgiler:\n"
         "  Architecture      x86_64, aarch64, armv7l\n"
         "  CPU op-modes      32-bit, 64-bit\n"
         "  CPU(s)            Toplam çekirdek sayısı\n"
         "  Thread(s) per core  Her çekirdekteki iş parçacığı\n"
         "  Core(s) per socket  Her soketteki çekirdek sayısı\n"
         "  Socket(s)         Fiziksel işlemci sayısı\n"
         "  CPU MHz           İşlemci hızı\n"
         "  L1d, L1i, L2, L3  Önbellek boyutları\n"
         "  Vendor ID         Intel, AMD, ARM\n"
         "  Model name        İşlemci model adı\n\n"
         "Seçenekler:\n"
         "  -e              İnsan okunabilir format (pretty)\n"
         "  -p              CSV formatında çıktı\n"
         "  -x              XML formatında çıktı\n"
         "  -y              JSON formatında çıktı\n\n"
         "Örnekler:\n"
         "  lscpu                       → Tüm CPU bilgileri\n"
         "  lscpu | grep 'Model name'   → İşlemci modeli\n"
         "  lscpu | grep 'CPU(s)'       → Çekirdek sayısı\n"
         "  lscpu -e                    → İnsan okunabilir\n"
         "  lscpu -J                    → JSON çıktı\n\n"
         "İpucu: nproc komutu sadece çekirdek sayısını, lscpu tüm detayları verir."},
        {"lsusb", "USB aygıtları", "lsusb",
         "USB veriyoluna bağlı tüm aygıtları listeler.\n\n"
         "Seçenekler:\n"
         "  -v            Detaylı bilgi (verbose)\n"
         "  -t            Ağaç yapısında göster\n"
         "  -s VERIYOLU:CIHAZ  Belirli cihazı göster\n"
         "  -d VERICI:URUN  Belirli ürün ID'sine göre filtrele\n"
         "  -D /dev/bus/usb/...  Belirli dosyayı oku\n\n"
         "Çıktı formatı:\n"
         "  Bus 001 Device 003: ID 8087:0024 Intel Corp. Integrated Rate Matching Hub\n"
         "  (Bus=Veriyolu, Device=Cihaz, ID=Üretici:Ürün, Açıklama)\n\n"
         "Örnekler:\n"
         "  lsusb                       → Tüm USB cihazları\n"
         "  lsusb -t                    → Ağaç görünümü\n"
         "  lsusb -v | grep -E 'iManufacturer|iProduct'  → Detaylı bilgi\n"
         "  lsusb -d 046d:c077          → Belirli bir Logitech fareyi sorgula\n"
         "  sudo lsusb -v               → Root ile tüm detaylar\n\n"
         "İpucu: USB aygıt tanıma sorunlarında lsusb ile başlayın."},
        {"lspci", "PCI aygıtları", "lspci",
         "PCI veriyoluna bağlı tüm aygıtları listeler (ekran kartı, ses kartı, ağ kartı vb.)\n\n"
         "Seçenekler:\n"
         "  -v            Detaylı bilgi\n"
         "  -vv           Çok detaylı bilgi\n"
         "  -k            Çekirdek sürücüsünü göster\n"
         "  -t            Ağaç yapısında göster\n"
         "  -nn           Üretici/ürün ID'lerini sayısal göster\n"
         "  -s [[[[<domain>]:]<bus>]:][<slot>][.[<func>]]  Belirli aygıt\n"
         "  -d <vendor>:<device>  Belirli ürün ID'sine göre\n"
         "  -D            DNS çözümlemesi yapma (hızlı)\n\n"
         "Örnekler:\n"
         "  lspci                       → Tüm PCI aygıtları\n"
         "  lspci | grep -i vga         → Ekran kartı bilgisi\n"
         "  lspci -k                    → Sürücü bilgisi ile\n"
         "  lspci -t                    → Ağaç görünümü\n"
         "  lspci -s 00:02.0 -v         → Belirli aygıtın detayı\n"
         "  lspci -v | grep -A 20 'VGA' → Ekran kartı detayları\n\n"
         "İpucu: Ekran kartı modelini öğrenmek için lspci | grep -i vga."},
        {"dmesg", "Çekirdek mesajları", "dmesg | tail -20",
         "Linux çekirdek halka tamponunu (ring buffer) görüntüler. Sistem açılışındaki mesajları ve donanım algılama bilgilerini içerir.\n\n"
         "Seçenekler:\n"
         "  -H          İnsan okunabilir format (zaman damgalı)\n"
         "  -T          İnsan okunabilir zaman (--ctime ile aynı)\n"
         "  -l SEVİYE   Seviyeye göre filtrele (emerg, alert, crit, err, warn, notice, info, debug)\n"
         "  -f TÜR      Türüne göre filtrele (kern, user, mail, daemon)\n"
         "  -L          Son mesajları renkli göster\n"
         "  -w          Canlı takip (tail -f gibi)\n"
         "  -n SEVİYE   Konsol log seviyesini ayarla\n"
         "  -S          Tampon boyutunu göster\n\n"
         "Seviyeler:\n"
         "  0 (emerg)   Sistem kullanılamaz\n"
         "  1 (alert)   Hemen müdahale gerekli\n"
         "  2 (crit)    Kritik durum\n"
         "  3 (err)     Hata\n"
         "  4 (warn)    Uyarı\n"
         "  5 (notice)  Önemli bilgi\n"
         "  6 (info)    Bilgi mesajı\n"
         "  7 (debug)   Hata ayıklama\n\n"
         "Örnekler:\n"
         "  dmesg | tail -20                → Son 20 mesaj\n"
         "  dmesg -T                        → İnsan okunabilir zamanla\n"
         "  dmesg -l err,warn               → Hata ve uyarılar\n"
         "  dmesg -w                        → Canlı takip\n"
         "  dmesg | grep -i usb             → USB ile ilgili mesajlar\n"
         "  dmesg | grep -i error           → Hataları bul\n\n"
         "İpucu: Yeni donanım taktığınızda dmesg -w ile canlı izleyin."},
        {"passwd", "Şifre değiştir", "passwd",
         "Kullanıcı şifresini değiştirir veya yönetir.\n\n"
         "Kullanım:\n"
         "  passwd                  → Kendi şifreni değiştir\n"
         "  passwd kullanıcı        → Başka kullanıcının şifresini değiştir (root)\n"
         "  passwd -l kullanıcı     → Hesabı kilitle\n"
         "  passwd -u kullanıcı     → Hesabı aç\n"
         "  passwd -d kullanıcı     → Şifreyi sil (şifresiz giriş)\n"
         "  passwd -S kullanıcı     → Hesap durumunu göster\n"
         "  passwd -e kullanıcı     → Şifrenin hemen sonlanmasını sağla\n"
         "  passwd -n GÜN           → Şifre değiştirme arası minimum gün\n"
         "  passwd -x GÜN           → Şifre geçerlilik süresi (gün)\n"
         "  passwd -w GÜN           → Uyarı süresi (gün)\n"
         "  passwd -i GÜN           → Hesap kapatma süresi (gün)\n\n"
         "Örnekler:\n"
         "  passwd                           → Yeni şifre sorar\n"
         "  sudo passwd -l testuser          → Kullanıcıyı kilitle\n"
         "  sudo passwd -S root              → Root hesap durumu\n"
         "  sudo passwd -x 90 kullanici      → 90 günde şifre değiştirme zorunluluğu\n\n"
         "İpucu: passwd --help ile tüm seçenekleri görebilirsiniz."},
        {"usermod", "Kullanıcı değiştir", "usermod -aG sudo kullanici",
         "Mevcut bir kullanıcı hesabını değiştirir (günceller).\n\n"
         "Seçenekler:\n"
         "  -aG GRUP    Kullanıcıyı gruba ekle (append)\n"
         "  -l YENI_AD  Kullanıcı adını değiştir\n"
         "  -d /yeni/ev  Ev dizinini değiştir\n"
         "  -m          Ev dizini içeriğini taşı (-d ile)\n"
         "  -s /bin/bash  Varsayılan kabuğu değiştir\n"
         "  -c 'Açıklama'  Açıklama ekle/değiştir\n"
         "  -e YYYY-AA-GG  Hesap bitiş tarihi\n"
         "  -L          Hesabı kilitle (şifreyi devre dışı bırak)\n"
         "  -U          Hesabı aç\n"
         "  -g GRUP     Birincil grup değiştir\n"
         "  -G GRUPLAR  Ek grupları belirle\n"
         "  -u UID      UID değiştir\n\n"
         "Örnekler:\n"
         "  sudo usermod -aG sudo kullanici   → sudo yetkisi ver\n"
         "  sudo usermod -l yeniad eskiad     → Kullanıcı adını değiştir\n"
         "  sudo usermod -d /home/yeni -m kullanici → Ev dizinini taşı\n"
         "  sudo usermod -s /bin/zsh kullanici  → Kabuk değiştir\n"
         "  sudo usermod -L kullanici         → Hesabı kilitle\n"
         "  sudo usermod -e 2026-12-31 kullanici → Süreli hesap\n\n"
         "⚠️ Kullanıcı adı değiştirirken oturumu kapalı olmalıdır.\n\n"
         "İpucu: useradd yeni kullanıcı oluşturur, usermod mevcut kullanıcıyı değiştirir."},
        {"groupadd", "Grup oluştur", "sudo groupadd yeni_grup",
         "Sisteme yeni bir grup ekler.\n\n"
         "Seçenekler:\n"
         "  -g GID       Belirli bir grup ID'si ata\n"
         "  -r           Sistem grubu oluştur (GID < 1000)\n"
         "  -f           Grup zaten varsa hata verme\n"
         "  -p PAROLA    Grup parolası belirle (güvenli değil)\n\n"
         "İlgili komutlar:\n"
         "  groupdel GRUP       → Grubu sil\n"
         "  groupmod -n YENI AD  → Grup adını değiştir\n"
         "  usermod -aG GRUP KUL  → Kullanıcıyı gruba ekle\n"
         "  gpasswd GRUP        → Grup yöneticisi\n"
         "  groups KULLANICI    → Kullanıcının gruplarını göster\n\n"
         "Örnekler:\n"
         "  sudo groupadd developers           → developers grubu oluştur\n"
         "  sudo groupadd -g 2000 proje_grubu  → Belirli GID ile\n"
         "  sudo groupadd -r sysgroup          → Sistem grubu\n"
         "  sudo usermod -aG developers kullanici  → Kullanıcıyı ekle\n\n"
         "İpucu: Gruplar dosya izinleri ve yetkilendirme için kullanılır."},
        {"iconv", "Karakter kodlaması dönüştür", "iconv -f UTF-8 -t ISO-8859-9 dosya.txt",
         "Metin dosyalarının karakter kodlamasını dönüştürür.\n\n"
         "Seçenekler:\n"
         "  -f KODLAMA  Kaynak kodlama\n"
         "  -t KODLAMA  Hedef kodlama\n"
         "  -l          Tüm desteklenen kodlamaları listele\n"
         "  -o DOSYA    Çıktıyı dosyaya yaz\n"
         "  -c          Geçersiz karakterleri yoksay\n"
         "  -s          Hata mesajlarını gizle\n\n"
         "Yaygın kodlamalar:\n"
         "  UTF-8       Evrensel kodlama (Linux varsayılanı)\n"
         "  ISO-8859-9  Latin-5 (Türkçe)\n"
         "  ISO-8859-1  Latin-1 (Batı Avrupa)\n"
         "  Windows-1254  Türkçe Windows kodlaması\n"
         "  ASCII       7-bit temel kodlama\n"
         "  UTF-16      16-bit Unicode\n\n"
         "Örnekler:\n"
         "  iconv -f UTF-8 -t ISO-8859-9 metin.txt -o metin_latin5.txt\n"
         "  iconv -f Windows-1254 -t UTF-8 windows_dosyasi.txt > linux_dosyasi.txt\n"
         "  iconv -l | grep -i turkish        → Türkçe kodlamaları listele\n"
         "  cat sorunlu.txt | iconv -f UTF-8 -t UTF-8 -c    → Geçersiz karakterleri temizle\n\n"
         "İpucu: dos2unix komutu da satır sonlarını dönüştürür (LF/CRLF)."},
        {"od", "Octal (sekizlik) döküm", "od -c dosya.txt",
         "Dosyayı çeşitli formatlarda dökümler (octal, hex, ASCII). Binary dosyaları incelemek için kullanılır.\n\n"
         "Seçenekler:\n"
         "  -c          ASCII karakterleriyle göster\n"
         "  -x          16'lık (hex) döküm\n"
         "  -o          8'lik (octal) döküm (varsayılan)\n"
         "  -d          10'luk (decimal) döküm\n"
         "  -A RADİKS   Adres formatı (o=octal, x=hex, d=decimal, n=yok)\n"
         "  -j BAŞLA    Başlangıç atlama (offset)\n"
         "  -N BYTE     Byte sayısı\n"
         "  -w N        Satır başına N byte\n"
         "  -t FMT      Format belirt\n\n"
         "Format türleri:\n"
         "  a           Adlandırılmış karakterler\n"
         "  c           ASCII karakterleri\n"
         "  d           Decimal (işaretli)\n"
         "  u           Decimal (işaretsiz)\n"
         "  o           Octal\n"
         "  x           Hexadecimal\n"
         "  f           Floating point (kayan noktalı)\n\n"
         "Örnekler:\n"
         "  od -c dosya.txt              → Karakter bazında göster\n"
         "  od -x dosya.bin              → Hex döküm\n"
         "  od -A x -t x1z dosya.bin     → Adres+hex+ASCII (xxd benzeri)\n"
         "  head -c 512 /dev/sda | od -A x -t x1z  → MBR'ın ilk 512 baytı\n"
         "  od -N 100 -c dosya           → İlk 100 karakter\n\n"
         "İpucu: xxd veya hexdump daha modern alternatiflerdir."},
        {"xxd", "Hex dökümü", "xxd dosya.bin",
         "Dosyayı hexadecimal (16'lık) döküm halinde gösterir veya tersine çevirir.\n\n"
         "Seçenekler:\n"
         "  -b          Binary (ikilik) döküm\n"
         "  -c N        Satır başına N byte (varsayılan: 16)\n"
         "  -l N        Sadece N byte göster\n"
         "  -s BAŞLA    Başlangıç atlama\n"
         "  -e          Little-endian düzeninde\n"
         "  -u          Büyük harfli hex\n"
         "  -r          Tersine çevir (hex → binary)\n"
         "  -p          Plain (düz) format, adres ve ASCII olmadan\n"
         "  -i          C include formatında çıktı\n\n"
         "Örnekler:\n"
         "  xxd dosya.bin                     → Standart hex döküm\n"
         "  xxd -l 100 dosya.bin              → İlk 100 byte\n"
         "  echo 'Merhaba' | xxd              → String'in hex değeri\n"
         "  echo '4d657268616261' | xxd -r -p  → Hex'ten metne çevir\n"
         "  xxd -i dosya.bin > dizi.h          → C dizisi oluştur\n"
         "  xxd -s 1024 -l 512 /dev/sda        → Belirli konumu oku\n\n"
         "Pratik:\n"
         "  Hex'ten ASCII'ye: echo '48656c6c6f' | xxd -r -p\n"
         "  ASCII'den Hex'e: echo -n 'Hello' | xxd -p\n\n"
         "İpucu: xxd -i ile binary dosyaları C koduna gömebilirsiniz."},
        {"md5sum", "MD5 hash hesapla", "md5sum dosya.txt",
         "Dosyanın MD5 özetini (hash) hesaplar. Dosya bütünlüğü kontrolü için kullanılır.\n\n"
         "Benzer komutlar:\n"
         "  md5sum    → 128-bit MD5 (32 karakter hex)\n"
         "  sha1sum   → 160-bit SHA-1 (40 karakter)\n"
         "  sha256sum → 256-bit SHA-2 (64 karakter) [ÖNERİLEN]\n"
         "  sha512sum → 512-bit SHA-2 (128 karakter)\n"
         "  b2sum     → BLAKE2 (hızlı ve güvenli)\n\n"
         "Seçenekler:\n"
         "  -c          Kontrol modu (özet dosyasını doğrula)\n"
         "  -t          Metin modu (varsayılan)\n"
         "  -b          Binary modu\n"
         "  -w          Geçersiz satırlar için uyar\n"
         "  --strict    Sıkı doğrulama\n"
         "  --quiet     Sadece hataları göster\n\n"
         "Örnekler:\n"
         "  md5sum dosya.txt                        → Özet hesapla\n"
         "  sha256sum dosya.txt                     → SHA-256 özeti\n"
         "  md5sum * > toplam.md5                   → Tüm dosyaların özetini kaydet\n"
         "  md5sum -c toplam.md5                    → Özetleri doğrula\n"
         "  echo -n 'metin' | md5sum                → String'in özeti\n"
         "  find . -type f -exec sha256sum {} \\; > checksums.sha256\n\n"
         "Pratik:\n"
         "  İndirdiğiniz ISO'nun doğruluğunu kontrol edin:\n"
         "    sha256sum indirilen.iso (üreticinin verdiği özetle karşılaştırın)\n\n"
         "İpucu: Güvenlik için MD5 yerine SHA-256 kullanmanız önerilir."},
        {"base64", "Base64 kodla/çöz", "echo 'Merhaba' | base64",
         "Veriyi Base64 formatında kodlar veya çözer. E-posta ekleri, JWT token'lar ve API'lerde yaygındır.\n\n"
         "Seçenekler:\n"
         "  -d          Çöz (decode)\n"
         "  -i          Sessiz mod (geçersiz karakterleri yoksay)\n"
         "  -w N        Satır genişliği (0=sarma yok, varsayılan: 76)\n"
         "  --help      Yardım\n\n"
         "Örnekler:\n"
         "  echo -n 'Merhaba' | base64              → TWVyaGFiYQ==\n"
         "  echo -n 'TWVyaGFiYQ==' | base64 -d      → Merhaba\n"
         "  base64 dosya.bin > dosya.b64            → Binary dosyayı kodla\n"
         "  base64 -d dosya.b64 > dosya.bin         → Çöz\n"
         "  echo -n 'kullanici:sifre' | base64      → Basic Auth header\n"
         "  cat resim.jpg | base64                  → Resmi base64'e çevir\n\n"
         "Pratik:\n"
         "  JWT token çözme: echo 'token_payload' | base64 -d 2>/dev/null || echo 'padding ekle'\n"
         "  Dosyayı inline CSS'e gömme: base64 resim.png | tr -d '\\n'\n\n"
         "İpucu: base32 ile de benzer işlem yapılabilir (daha kısa çıktı)."},
      };

    for (auto& c : cmds) {
        auto* outer = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 0);
        outer->set_name(c.cmd + "|" + c.desc);

        auto* cb = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::VERTICAL, 3);
        cb->add_css_class("cmd-card");

        auto* ct = Gtk::make_managed<Gtk::Label>("");
        ct->set_markup("<b>" + c.cmd + "</b>");
        ct->set_halign(Gtk::Align::START);
        ct->add_css_class("cmd-name");

        auto* cd = Gtk::make_managed<Gtk::Label>(c.desc);
        cd->set_halign(Gtk::Align::START);

        auto* ce = Gtk::make_managed<Gtk::Label>("");
        ce->set_markup("<tt>" + c.ex + "</tt>");
        ce->set_halign(Gtk::Align::START);
        ce->add_css_class("cmd-example");

        auto* detail_label = Gtk::make_managed<Gtk::Label>("");
        std::string escaped_detail;
        for (char ch : c.detail) {
            if (ch == '<') escaped_detail += "&lt;";
            else if (ch == '>') escaped_detail += "&gt;";
            else if (ch == '&') escaped_detail += "&amp;";
            else escaped_detail += ch;
        }
        detail_label->set_markup(escaped_detail);
        detail_label->set_halign(Gtk::Align::START);
        detail_label->set_wrap(true);
        detail_label->set_margin_top(6);
        detail_label->add_css_class("cmd-detail");

        auto* revealer = Gtk::make_managed<Gtk::Revealer>();
        revealer->set_transition_type(Gtk::RevealerTransitionType::SLIDE_DOWN);
        revealer->set_child(*detail_label);

        cb->append(*ct);
        cb->append(*cd);
        cb->append(*ce);
        cb->append(*revealer);

        auto* lab_row = Gtk::make_managed<Gtk::Box>(Gtk::Orientation::HORIZONTAL, 8);
        lab_row->set_margin_top(8);

        auto* btn_try = Gtk::make_managed<Gtk::Button>("Dene");
        btn_try->add_css_class("lab-btn-try");
        btn_try->signal_clicked().connect([this, c_cmd = c.cmd, c_ex = c.ex]() {
            on_lab_try_command(c_cmd + " " + c_ex);
        });

        auto* btn_terminal = Gtk::make_managed<Gtk::Button>("Terminal");
        btn_terminal->add_css_class("lab-btn-terminal");
        btn_terminal->signal_clicked().connect([this, c_cmd = c.cmd]() {
            on_lab_open_terminal("Şimdi deneyebilirsin: " + c_cmd);
        });

        auto* output_label = Gtk::make_managed<Gtk::Label>("");
        output_label->add_css_class("lab-output");
        output_label->set_wrap(true);
        output_label->set_selectable(true);
        output_label->set_halign(Gtk::Align::START);
        output_label->set_visible(false);

        auto* output_revealer = Gtk::make_managed<Gtk::Revealer>();
        output_revealer->set_transition_type(Gtk::RevealerTransitionType::SLIDE_DOWN);
        output_revealer->set_child(*output_label);

        lab_row->append(*btn_try);
        lab_row->append(*btn_terminal);
        cb->append(*lab_row);
        cb->append(*output_revealer);

        outer->append(*cb);

        auto click_box = Gtk::GestureClick::create();
        click_box->signal_released().connect([revealer, cb, output_revealer, output_label](int, double, double) {
            bool revealed = revealer->get_child_revealed();
            output_revealer->set_reveal_child(false);
            output_label->set_visible(false);
            if (revealed) {
                revealer->set_reveal_child(false);
                cb->remove_css_class("cmd-card-expanded");
            } else {
                revealer->set_reveal_child(true);
                cb->add_css_class("cmd-card-expanded");
            }
        });

        cb->add_controller(click_box);

        linux_commands.append(*outer);
    }

    sw->set_child(linux_commands);

    box->append(*title);
    box->append(*search);
    box->append(*lab_warning);
    box->append(*sw);
    stack.add(*box, "linux");
}

void MainWindow::filter_linux(const std::string& q) {
    auto* child = linux_commands.get_first_child();
    while (child) {
        auto name = child->get_name();
        auto p = name.find('|');
        if (p != std::string::npos) {
            auto cmd = name.substr(0, p);
            auto desc = name.substr(p+1);
            child->set_visible(q.empty() || cmd.find(q) != std::string::npos || desc.find(q) != std::string::npos);
        }
        child = child->get_next_sibling();
    }
}
