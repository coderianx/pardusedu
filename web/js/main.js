const navbar = document.getElementById('navbar');
const navToggle = document.getElementById('navToggle');
const navLinks = document.getElementById('navLinks');
const themeBtn = document.getElementById('themeBtn');
const html = document.documentElement;

navToggle.addEventListener('click', () => {
  navLinks.classList.toggle('open');
});

navLinks.querySelectorAll('a').forEach(link => {
  link.addEventListener('click', () => {
    navLinks.classList.remove('open');
  });
});

document.addEventListener('scroll', () => {
  navbar.classList.toggle('scrolled', window.scrollY > 50);
});

const savedTheme = localStorage.getItem('theme');
if (savedTheme === 'light') {
  html.classList.add('light');
}

themeBtn.addEventListener('click', () => {
  html.classList.toggle('light');
  localStorage.setItem('theme', html.classList.contains('light') ? 'light' : 'dark');
});

const observer = new IntersectionObserver((entries) => {
  entries.forEach(entry => {
    if (entry.isIntersecting) {
      entry.target.style.opacity = '1';
      entry.target.style.transform = 'translateY(0)';
    }
  });
}, {
  threshold: 0.15,
  rootMargin: '0px 0px -50px 0px'
});

document.querySelectorAll('.feature').forEach(el => {
  el.style.opacity = '0';
  el.style.transform = 'translateY(40px)';
  el.style.transition = 'opacity .7s ease, transform .7s ease';
  observer.observe(el);
});

/* ── Model Modal ── */

const modelData = {
  llama33: {
    title: 'Llama 3.3 70B',
    desc: 'Meta\'nın en gelişmiş açık kaynak modellerinden biri olan Llama 3.3 70B, 70 milyar parametre ile yüksek doğrulukta metin üretimi, karmaşık mantıksal akıl yürütme ve çok dilli görevlerde üstün performans sunar. Akademik araştırma ve derinlemesine analiz için idealdir.'
  },
  llama31: {
    title: 'Llama 3.1 8B',
    desc: 'Llama 3.1 8B, 8 milyar parametrelik kompakt yapısıyla hızlı yanıt süreleri ve düşük gecikme sunar. Günlük sorular, hızlı araştırmalar ve hafif görevler için optimize edilmiştir. Verimlilik ve hızın buluştuğu nokta.'
  },
  llama4: {
    title: 'Llama 4 17B 16E',
    desc: 'Meta\'nın en yeni nesil modeli Llama 4, 17 milyar aktif parametre ve 16 uzman (MoE) mimarisi ile çalışır. Çok modlu yetenekleri, geniş bağlam penceresi ve üstün verimliliği ile hem görsel hem metinsel görevlerde çığır açar.'
  },
  gptoss20: {
    title: 'GPT OSS 20B',
    desc: 'GPT-OSS 20B, tamamen açık kaynaklı 20 milyar parametrelik bir dil modelidir. Özellikle yaratıcı yazma, kod üretimi ve problem çözme görevlerinde güçlüdür. Şeffaf ve özelleştirilebilir yapısıyla geliştiriciler için idealdir.'
  },
  gptoss120: {
    title: 'GPT OSS 120B',
    desc: 'GPT-OSS serisinin amiral gemisi olan 120B, 120 milyar parametre ile piyasadaki en büyük açık kaynak modellerden biridir. Karmaşık matematik, bilimsel analiz ve çok dilli çeviri gibi zorlu görevlerde insan seviyesinde performans gösterir.'
  },
  gptsafeguard: {
    title: 'GPT OSS SafeGuard 20B',
    desc: 'GPT-OSS SafeGuard 20B, güvenlik odaklı olarak ince ayar yapılmış 20 milyar parametrelik bir modeldir. Zararlı çıktıları filtreleme, hassas bilgileri koruma ve güvenli yanıt üretme konularında uzmanlaşmıştır. Eğitim ortamları için en güvenli seçenek.'
  },
  qwen3: {
    title: 'Qwen 3 32B',
    desc: 'Alibaba Cloud tarafından geliştirilen Qwen 3 32B, 32 milyar parametre ile Çince ve İngilizce başta olmak üzere çoklu dil desteğinde mükemmeldir. Matematiksel akıl yürütme, kod anlama ve uzun belge işlemede güçlü bir rakip.'
  },
  groq: {
    title: 'Groq Compound',
    desc: 'Groq Compound, Groq\'un özel LPU (Language Processing Unit) donanımında çalışan birden fazla modelin birleşimidir. Düşük gecikme süresi ve yüksek çıktı hızı sayesinde neredeyse anlık yanıtlar sunar. Gerçek zamanlı uygulamalar için en hızlı seçenek.'
  }
};

const modalOverlay = document.getElementById('modelModal');
const modalTitle = document.getElementById('modalTitle');
const modalDesc = document.getElementById('modalDesc');
const modalClose = document.getElementById('modalClose');

document.querySelectorAll('.model-badge').forEach(badge => {
  badge.addEventListener('click', () => {
    const key = badge.dataset.model;
    const data = modelData[key];
    if (data) {
      modalTitle.textContent = data.title;
      modalDesc.textContent = data.desc;
      modalOverlay.classList.add('open');
    }
  });
});

function closeModelModal() {
  modalOverlay.classList.remove('open');
}

modalClose.addEventListener('click', closeModelModal);

modalOverlay.addEventListener('click', (e) => {
  if (e.target === modalOverlay) closeModelModal();
});

document.addEventListener('keydown', (e) => {
  if (e.key === 'Escape') closeModelModal();
});
