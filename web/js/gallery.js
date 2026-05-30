const filterBtns = document.querySelectorAll('.filter-btn');
const cards = document.querySelectorAll('.gallery-card');
const lightbox = document.getElementById('lightbox');
const lightboxImg = document.getElementById('lightboxImg');
const lightboxCaption = document.getElementById('lightboxCaption');
const lightboxClose = document.getElementById('lightboxClose');

filterBtns.forEach(btn => {
  btn.addEventListener('click', () => {
    filterBtns.forEach(b => b.classList.remove('active'));
    btn.classList.add('active');
    const filter = btn.dataset.filter;
    cards.forEach(card => {
      if (filter === 'all' || card.dataset.category === filter) {
        card.style.display = 'block';
        card.style.animation = 'none';
        requestAnimationFrame(() => {
          card.style.animation = 'cardFadeIn .5s ease forwards';
        });
      } else {
        card.style.display = 'none';
      }
    });
  });
});

cards.forEach(card => {
  card.addEventListener('click', () => {
    const img = card.querySelector('img');
    const badge = card.querySelector('.gallery-card-badge');
    const title = card.querySelector('h3');
    lightboxImg.src = img.src;
    lightboxImg.alt = img.alt;
    lightboxCaption.textContent = `${badge.textContent} — ${title.textContent}`;
    lightbox.classList.add('open');
  });
});

function closeLightbox() {
  lightbox.classList.remove('open');
}

lightboxClose.addEventListener('click', closeLightbox);

lightbox.addEventListener('click', (e) => {
  if (e.target === lightbox) closeLightbox();
});

document.addEventListener('keydown', (e) => {
  if (e.key === 'Escape') closeLightbox();
});

const style = document.createElement('style');
style.textContent = `
  @keyframes cardFadeIn {
    from { opacity: 0; transform: translateY(20px); }
    to { opacity: 1; transform: translateY(0); }
  }
`;
document.head.appendChild(style);
