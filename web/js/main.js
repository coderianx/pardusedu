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

/* ── Model Modals ── */

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
    title: 'Llama 4 Scout 17B-16E',
    desc: 'Meta\'nın en yeni nesil modeli Llama 4 Scout, 17 milyar aktif parametre ve 16 uzman (MoE) mimarisi ile çalışır. Çok modlu yetenekleri, geniş bağlam penceresi ve üstün verimliliği ile hem görsel hem metinsel görevlerde çığır açar.'
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
  },
  gemma4: {
    title: 'Google Gemma 4 31B',
    desc: 'Google\'ın en yeni açık kaynak modeli Gemma 4 31B, 31 milyar parametre ile yüksek performanslı metin üretimi ve akıl yürütme sunar. Hafif yapısı sayesinde hızlı çalışır ve özellikle geliştiriciler için ideal bir seçenektir.'
  },
  gemini35flash: {
    title: 'Gemini 3.5 Flash',
    desc: 'Google\'ın en yeni nesil Gemini modeli olan 3.5 Flash, hız ve doğruluk arasında mükemmel denge kurar. Çok modlu yetenekleri, geniş bağlam penceresi ve düşük gecikme süresi ile günlük kullanım için en ideal Google modelidir.'
  },
  gemini25flash: {
    title: 'Gemini 2.5 Flash',
    desc: 'Gemini 2.5 Flash, Google\'ın dengeli ve hızlı modelidir. Özellikle uzun belgeleri işleme, kod üretimi ve çok dilli görevlerde güçlüdür. Düşük maliyetli ve yüksek verimli bir seçenek arayanlar için biçilmiş kaftan.'
  },
  gemini31flashlite: {
    title: 'Gemini 3.1 Flash Lite',
    desc: 'Gemini 3.1 Flash Lite, Google\'ın en hafif ve en hızlı modelidir. Basit görevler, hızlı yanıtlar ve düşük maliyetli işlemler için optimize edilmiştir. Yüksek hacimli API çağrıları için ekonomik bir çözümdür.'
  },
  gemini25pro: {
    title: 'Gemini 2.5 Pro',
    desc: 'Google\'ın en güçlü ve en yetenekli modeli olan Gemini 2.5 Pro, karmaşık akıl yürütme, matematik, bilimsel analiz ve profesyonel düzeyde kod üretiminde çığır açar. En zorlu görevler için nihai çözüm.'
  },
  qwen36flash: {
    title: 'Qwen 3.6 Flash',
    desc: 'Alibaba Cloud\'un yeni nesil hızlı modeli Qwen 3.6 Flash, düşük gecikme süresi ve yüksek çıktı hızı ile öne çıkar. Özellikle Çince ve İngilizce dillerinde üstün performans sunar.'
  },
  qwen3coder: {
    title: 'Qwen 3 Coder',
    desc: 'Qwen 3 Coder, yazılım geliştirme için özel olarak ince ayar yapılmış bir modeldir. Kod anlama, hata ayıklama, test yazma ve çoklu dosya analizi konularında uzmanlaşmıştır. Geliştiriciler için vazgeçilmez bir yardımcı.'
  },
  qwen37max: {
    title: 'Qwen 3.7 Max',
    desc: 'Qwen serisinin en büyük ve en yetenekli modeli olan 3.7 Max, karmaşık mantıksal akıl yürütme, uzun belge analizi, çok dilli çeviri ve yaratıcı yazma görevlerinde üstün performans gösterir. En zorlu akademik ve profesyonel işler için tasarlanmıştır.'
  },
  deepseekr1: {
    title: 'DeepSeek R1',
    desc: 'DeepSeek R1, güçlü akıl yürütme (reasoning) yetenekleri ile öne çıkan bir modeldir. Matematik, mantık problemleri, kod analizi ve adım adım düşünme gerektiren karmaşık görevlerde mükemmel sonuçlar verir. Derinlemesine analiz için en iyi seçeneklerden biridir.'
  },
  deepseekv4pro: {
    title: 'DeepSeek V4 Pro',
    desc: 'DeepSeek V4 Pro, DeepSeek\'in en gelişmiş modelidir. Geniş bilgi tabanı, yüksek doğruluk oranı ve çok yönlü yetenekleri ile hem akademik araştırma hem de profesyonel kullanım için idealdir. Dengeli ve güvenilir bir tercih.'
  },
  deepseekv4flash: {
    title: 'DeepSeek V4 Flash Nitro',
    desc: 'DeepSeek V4 Flash Nitro, hız odaklı olarak optimize edilmiş bir modeldir. Nitro donanımında çalışarak son derece düşük gecikme süresi ve yüksek hız sunar. Gerçek zamanlı uygulamalar ve hızlı yanıt gerektiren görevler için idealdir.'
  },
  mistralsmall4: {
    title: 'Mistral Small 4',
    desc: 'Mistral Small 4, Fransız yapay zeka şirketi Mistral AI\'nin en yeni hafif modelidir. 2026 yılında yayınlanan bu model, düşük maliyet ve yüksek verimliliği bir araya getirir. Temel görevler ve hızlı sorgular için ekonomik bir çözümdür.'
  },
  kimik2: {
    title: 'Kimi K2',
    desc: 'Moonshot AI tarafından geliştirilen Kimi K2, güçlü mantıksal akıl yürütme ve düşünme (thinking) yetenekleri ile öne çıkar. Karmaşık problem çözme, analiz ve araştırma görevlerinde derinlemesine düşünme kapasitesi sunar.'
  },
  kimik26: {
    title: 'Kimi K2.6',
    desc: 'Kimi K2.6, Moonshot AI\'nin en güncel modelidir. K2\'nin tüm yeteneklerini geliştirilmiş hız ve doğrulukla sunar. Özellikle uzun metin işleme ve detaylı analiz görevlerinde üstün performans gösterir.'
  },
  claudehaiku45: {
    title: 'Claude Haiku 4.5',
    desc: 'Anthropic\'in en hızlı ve en ekonomik modeli olan Claude Haiku 4.5, düşük gecikme süresi ile hızlı yanıtlar üretir. Günlük sorular, metin özetleme ve basit analiz görevleri için idealdir. Güvenlik odaklı yapısıyla öne çıkar.'
  },
  claudesonnet46: {
    title: 'Claude Sonnet 4.6',
    desc: 'Claude Sonnet 4.6, Anthropic\'in dengeli ve güçlü modelidir. Hız ve kalite arasında mükemmel denge kurar. Yaratıcı yazma, kod üretimi, analiz ve uzun belge işleme gibi geniş bir yelpazede üstün performans sunar. En popüler Claude modellerinden biridir.'
  },
  gemini31pro: {
    title: 'Gemini 3.1 Pro',
    desc: 'Google\'ın en yeni profesyonel seviye modeli olan Gemini 3.1 Pro, gelişmiş akıl yürütme, kod üretimi ve çok modlu analiz yetenekleriyle öne çıkar. Uzun bağlam penceresi sayesinde büyük projeleri ve karmaşık belgeleri sorunsuz işler.'
  },
  qwen3nextfree: {
    title: 'Qwen3 Next Free',
    desc: 'Qwen3 Next, 80B parametre ve 3B aktif parametre (MoE) mimarisi ile devasa bir model olup tamamen ücretsiz kullanım sunar. Büyük ölçekli akıl yürütme, karmaşık kod analizi ve yaratıcı yazma görevlerinde profesyonel düzeyde sonuç verir.'
  },
  gpt5mini: {
    title: 'ChatGPT 5 Mini',
    desc: 'OpenAI\'nin en yeni nesil modeli GPT-5 Mini, 5. nesil teknolojisini kompakt bir pakette sunar. Hızlı, verimli ve düşük maliyetlidir. Günlük kullanım, hızlı sorgular ve basit görevler için mükemmel bir seçenektir.'
  },
  gpt4omini: {
    title: 'ChatGPT 4o Mini',
    desc: 'OpenAI\'nin en popüler küçük modeli olan GPT-4o Mini, yüksek hız ve düşük maliyeti bir araya getirir. Geniş bilgi tabanı ve çok modlu yetenekleri ile hem metin hem görsel görevlerde güçlü performans sunar.'
  },
  gpt41mini: {
    title: 'ChatGPT 4.1 Mini',
    desc: 'GPT-4.1 Mini, OpenAI\'nin 4.1 nesil teknolojisiyle güçlendirilmiş hızlı ve stabil bir modeldir. Dengeli yapısı, uzun bağlam penceresi ve tutarlı çıktı kalitesi ile geniş bir kullanım yelpazesinde güvenilir bir tercihtir.'
  },
  gemini25flashlite: {
    title: 'Gemini 2.5 Flash Lite',
    desc: 'Google\'ın en hafif ve en hızlı modeli olan Gemini 2.5 Flash Lite, düşük maliyetli ve yüksek verimli bir seçenektir. Basit görevler ve hızlı yanıtlar için optimize edilmiştir.'
  },
  gemini31flashlite_g: {
    title: 'Gemini 3.1 Flash Lite',
    desc: 'Gemini 3.1 Flash Lite, Google\'ın en güncel hafif modelidir. 3.1 nesil teknolojisiyle hız ve verimlilikte çığır açar. Günlük kullanım ve yüksek hacimli API çağrıları için idealdir.'
  },
  gemini25flash_g: {
    title: 'Gemini 2.5 Flash',
    desc: 'Gemini 2.5 Flash, Google\'ın dengeli ve hızlı modelidir. Uzun belgeleri işleme, kod üretimi ve çok dilli görevlerde güçlüdür. Hız ve kaliteyi bir arada sunar.'
  },
  gemini35flash_g: {
    title: 'Gemini 3.5 Flash',
    desc: 'Google\'ın en yeni nesil Gemini modeli olan 3.5 Flash, hız ve doğruluk arasında mükemmel denge kurar. Çok modlu yetenekleri, geniş bağlam penceresi ve düşük gecikme süresi ile günlük kullanım için en ideal Google modelidir.'
  },
  gemini25pro_g: {
    title: 'Gemini 2.5 Pro',
    desc: 'Google\'ın en güçlü ve en yetenekli modeli. Karmaşık akıl yürütme, matematik, bilimsel analiz ve profesyonel düzeyde kod üretiminde çığır açar. En zorlu görevler için nihai çözüm.'
  },
  gemini3pro: {
    title: 'Gemini 3 Pro',
    desc: 'Gemini 3 Pro, Google\'ın 3. nesil profesyonel modelinin önizleme sürümüdür. Gelişmiş akıl yürütme, kod anlama ve çok modlu analiz yetenekleriyle gelecek nesil teknolojisini şimdiden deneyimleme fırsatı sunar.'
  },
  gemini31pro_g: {
    title: 'Gemini 3.1 Pro',
    desc: 'Google\'ın en yeni profesyonel seviye modeli olan Gemini 3.1 Pro, gelişmiş akıl yürütme, kod üretimi ve çok modlu analiz yetenekleriyle öne çıkar. Uzun bağlam penceresi sayesinde büyük projeleri ve karmaşık belgeleri sorunsuz işler.'
  }
};

/* ── Individual Model Info Modal ── */

const modalOverlay = document.getElementById('modelModal');
const modalTitle = document.getElementById('modalTitle');
const modalDesc = document.getElementById('modalDesc');
const modalClose = document.getElementById('modalClose');

function openModelInfo(key) {
  const data = modelData[key];
  if (data) {
    modalTitle.textContent = data.title;
    modalDesc.textContent = data.desc;
    modalOverlay.classList.add('open');
  }
}

function closeModelModal() {
  modalOverlay.classList.remove('open');
}

modalClose.addEventListener('click', closeModelModal);

modalOverlay.addEventListener('click', (e) => {
  if (e.target === modalOverlay) closeModelModal();
});

/* ── Models List Modal ── */

const modelsListModal = document.getElementById('modelsListModal');
const modelsListClose = document.getElementById('modelsListClose');
const modelsToggleBtn = document.getElementById('modelsToggleBtn');

function openModelsList() {
  modelsListModal.classList.add('open');
}

function closeModelsList() {
  modelsListModal.classList.remove('open');
}

modelsToggleBtn.addEventListener('click', openModelsList);
modelsListClose.addEventListener('click', closeModelsList);

modelsListModal.addEventListener('click', (e) => {
  if (e.target === modelsListModal) closeModelsList();
});

/* ── Badge clicks (delegated) ── */

document.addEventListener('click', (e) => {
  const badge = e.target.closest('.model-badge');
  if (badge) {
    const key = badge.dataset.model;
    if (modelData[key]) {
      closeModelsList();
      openModelInfo(key);
    }
  }
});

/* ── Escape ── */

document.addEventListener('keydown', (e) => {
  if (e.key === 'Escape') {
    closeModelModal();
    closeModelsList();
  }
});
