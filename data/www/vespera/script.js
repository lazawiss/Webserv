// ============================================
// VESPERA — script partagé
// ============================================

document.addEventListener('DOMContentLoaded', () => {

  // --- Menu mobile ---
  const toggle = document.querySelector('.nav__toggle');
  const links = document.querySelector('.nav__links');
  if (toggle && links) {
    toggle.addEventListener('click', () => {
      const isOpen = links.classList.toggle('is-open');
      toggle.setAttribute('aria-expanded', isOpen);
    });
  }

  // --- HUD : heure locale + secteur météo simulé ---
  const clock = document.querySelector('[data-hud-clock]');
  if (clock) {
    const update = () => {
      const now = new Date();
      const hh = String(now.getHours()).padStart(2, '0');
      const mm = String(now.getMinutes()).padStart(2, '0');
      clock.textContent = `${hh}:${mm}`;
    };
    update();
    setInterval(update, 1000 * 15);
  }

  // --- Formulaire brochure ---
  const form = document.querySelector('#brochure-form');
  if (form) {
    const status = document.querySelector('.form-status');
    form.addEventListener('submit', (e) => {
      const nom = form.querySelector('#nom');
      const email = form.querySelector('#email');
      const emailOk = /^[^\s@]+@[^\s@]+\.[^\s@]+$/.test(email.value.trim());

      if (!nom.value.trim() || !emailOk) {
        e.preventDefault();
        status.textContent = '// ERREUR — vérifie ton nom et ton adresse mail avant transmission.';
        status.classList.remove('is-visible');
        status.classList.add('is-visible', 'is-error');
        return;
      }

      // Si aucun backend n'est branché sur l'action du formulaire,
      // on empêche le rechargement et on affiche une confirmation locale.
      // Retire ce bloc si le formulaire pointe vers un vrai serveur.
      if (!form.action || form.action.endsWith('#')) {
        e.preventDefault();
        status.classList.remove('is-error');
        status.textContent = `// TRANSMISSION REÇUE — la brochure de Vespera sera envoyée à ${email.value.trim()}.`;
        status.classList.add('is-visible');
        form.reset();
      }
    });
  }
});
