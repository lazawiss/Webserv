// ============================================
// VESPERA — Script
// ============================================

document.addEventListener('DOMContentLoaded', () => {

  // --- HUD : hours ---
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

  // --- Form ---
  const form = document.querySelector('#brochure-form');
  if (form)
  {
    const status = document.querySelector('.form-status');
    form.addEventListener('submit', (e) => {
      const nom = form.querySelector('#nom');
      const email = form.querySelector('#email');

      if (!nom.value.trim())
      {
        e.preventDefault();
        status.textContent = '// ERREUR — vérifie ton nom et ton adresse mail avant transmission.';
        status.classList.remove('is-visible');
        status.classList.add('is-visible', 'is-error');
        return;
      }

      if (!form.action || form.action.endsWith('#'))
      {
        e.preventDefault();
        status.classList.remove('is-error');
        status.textContent = `// TRANSMISSION REÇUE — la brochure de Vespera sera envoyée à ${email.value.trim()}.`;
        status.classList.add('is-visible');
        form.reset();
      }
    });
  }
});
