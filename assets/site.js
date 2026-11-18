(() => {
  const $$ = (s, r = document) => [...r.querySelectorAll(s)];
  const root = document.documentElement;
  const toast = msg => {
    let t = document.querySelector('.toast');
    if (!t) { t = document.createElement('div'); t.className = 'toast'; document.body.appendChild(t); }
    t.textContent = msg; t.classList.add('on');
    clearTimeout(t._tm); t._tm = setTimeout(() => t.classList.remove('on'), 2600);
  };

  const store = {
    get(k) { try { return localStorage.getItem(k); } catch { return null; } },
    set(k, v) { try { localStorage.setItem(k, v); } catch {} }
  };

  /* 深浅色 */
  const q = new URLSearchParams(location.search).get('theme');
  const saved = (q === 'dark' || q === 'light') ? q : store.get('md-theme');
  if (saved === 'dark' || (!saved && matchMedia('(prefers-color-scheme: dark)').matches)) root.dataset.theme = 'dark';

  const syncThemeBtn = () => {
    $$('[data-theme-toggle]').forEach(b => b.setAttribute('aria-label', root.dataset.theme === 'dark' ? '切换到浅色' : '切换到深色'));
    const m = document.querySelector('meta[name="theme-color"]');
    if (m) m.setAttribute('content', root.dataset.theme === 'dark' ? '#0b0e14' : '#f6f8fc');
  };
  const toggleTheme = () => {
    root.dataset.theme = root.dataset.theme === 'dark' ? 'light' : 'dark';
    store.set('md-theme', root.dataset.theme);
    syncThemeBtn();
  };
  $$('.nav-act').forEach(act => {
    if (!act.querySelector('[data-theme-toggle]')) {
      const b = document.createElement('button');
      b.className = 'theme-btn';
      b.setAttribute('data-theme-toggle', '');
      b.setAttribute('aria-label', '切换深色/浅色');
      b.innerHTML = '<svg class="ic sm moon"><use href="#i-moon"/></svg><svg class="ic sm sun"><use href="#i-sun"/></svg>';
      act.insertBefore(b, act.firstChild);
    }
  });
  $$('[data-theme-toggle]').forEach(b => { b.textContent = ''; b.addEventListener('click', toggleTheme); });
  syncThemeBtn();

  /* 卡片高光 */
  $$('.card, .stat, .dl-card, .bento .card').forEach(el => {
    el.addEventListener('pointermove', e => {
      const r = el.getBoundingClientRect();
      el.style.setProperty('--mx', (e.clientX - r.left) + 'px');
      el.style.setProperty('--my', (e.clientY - r.top) + 'px');
    });
  });

  const menuBtn = document.querySelector('.menu-btn');
  if (menuBtn) menuBtn.addEventListener('click', () => document.querySelector('.nav-links')?.classList.toggle('open'));

  $$('[data-year]').forEach(el => el.textContent = new Date().getFullYear());

  /* 轮播 */
  $$('[data-carousel]').forEach(car => {
    const slides = car.querySelector('.slides');
    const items = $$('.slide', slides);
    const dots = car.querySelector('.dots');
    let i = 0, timer = null;
    const go = n => {
      i = (n + items.length) % items.length;
      slides.style.transform = 'translateX(-' + (i * 100) + '%)';
      $$('button', dots).forEach((d, k) => d.classList.toggle('on', k === i));
    };
    items.forEach((_, k) => {
      const b = document.createElement('button');
      b.setAttribute('aria-label', '第 ' + (k + 1) + ' 张');
      b.addEventListener('click', () => { go(k); restart(); });
      dots.appendChild(b);
    });
    car.querySelector('.prev')?.addEventListener('click', () => { go(i - 1); restart(); });
    car.querySelector('.next')?.addEventListener('click', () => { go(i + 1); restart(); });
    const restart = () => { clearInterval(timer); timer = setInterval(() => go(i + 1), 5200); };
    car.addEventListener('mouseenter', () => clearInterval(timer));
    car.addEventListener('mouseleave', restart);
    go(0); restart();
  });

  /* 移动端复制下载链接 */
  $$('[data-copy-link]').forEach(btn => {
    btn.addEventListener('click', async () => {
      const url = location.protocol.startsWith('http')
        ? location.origin + location.pathname.replace(/assets\/pages\/.*/, 'assets/pages/download.html')
        : 'https://github.com/wang01116/MiniDock/releases/latest';
      try {
        await navigator.clipboard.writeText(url);
        const t = btn.textContent;
        btn.textContent = '已复制，请粘贴到电脑浏览器';
        setTimeout(() => btn.textContent = t, 1800);
      } catch {
        prompt('请复制以下下载地址：', url);
      }
    });
  });

  /* 下载页：解析最新 Release */
  const dlButtons = $$('[data-download]');
  if (dlButtons.length) {
    const api = 'https://api.github.com/repos/wang01116/MiniDock/releases/latest';
    const fallback = 'https://github.com/wang01116/MiniDock/releases/latest';
    const resolve = async (upx) => {
      const r = await fetch(api, { cache: 'no-store' });
      if (!r.ok) throw new Error('api');
      const j = await r.json();
      const ver = String(j.tag_name || '').replace(/^v/i, '');
      const all = j.assets || [];
      const exeName = `MiniDock-${ver}.exe`;
      const upxName = `MiniDock-${ver}-UPX.zip`;
      let a;
      if (upx) {
        a = all.find(x => x.name === upxName) || all.find(x => /-UPX\.zip$/i.test(x.name)) || all.find(x => /\.zip$/i.test(x.name));
      } else {
        a = all.find(x => x.name === exeName) || all.find(x => /^MiniDock-.*\.exe$/i.test(x.name) && !/upx/i.test(x.name)) || all.find(x => /\.exe$/i.test(x.name));
      }
      const lv = document.getElementById('latestVer');
      if (lv && ver) lv.textContent = ver;
      return a ? { url: a.browser_download_url, name: a.name } : { url: fallback, name: '' };
    };
    dlButtons.forEach(btn => {
      btn.addEventListener('click', async e => {
        e.preventDefault();
        const upx = btn.dataset.download === 'upx';
        if (!upx) btn.classList.add('loading');
        let url = fallback, name = '';
        try { const r = await resolve(upx); url = r.url; name = r.name; } catch {}
        if (!upx) {
          btn.classList.remove('loading');
          btn.classList.add('done');
          btn.querySelector('.arrow').textContent = '✓';
          btn.lastChild.textContent = ' 已开始下载';
          toast(name ? ('开始下载 ' + name) : '下载已开始，若浏览器拦截请允许下载');
        } else {
          toast(name ? ('开始下载 ' + name) : '已开始下载 UPX 版本');
        }
        const a = document.createElement('a');
        a.href = url; a.download = ''; a.rel = 'noopener';
        document.body.appendChild(a); a.click(); a.remove();
      });
    });
  }

  /* 文档 */
  const side = document.querySelector('.side');
  if (side) {
    const search = side.querySelector('input');
    search && search.addEventListener('input', () => {
      const q = search.value.trim().toLowerCase();
      $$('a', side).forEach(a => a.style.display = (!q || a.textContent.toLowerCase().includes(q)) ? '' : 'none');
      $$('b', side).forEach(b => b.style.display = q ? 'none' : '');
    });
    const links = $$('a[href^="#"]', side);
    const map = new Map(links.map(a => [a.getAttribute('href').slice(1), a]));
    const spy = new IntersectionObserver(es => {
      es.forEach(e => {
        if (!e.isIntersecting) return;
        links.forEach(a => a.classList.remove('on'));
        const a = map.get(e.target.id);
        if (a) { a.classList.add('on'); a.scrollIntoView({ block: 'nearest' }); }
      });
    }, { rootMargin: '-15% 0px -70% 0px', threshold: 0 });
    $$('.doc section[id]').forEach(s => spy.observe(s));
    const toc = document.querySelector('.toc-right');
    if (toc) {
      toc.innerHTML = '<b>本页目录</b>' + $$('.doc section h2').map(h => {
        const id = h.closest('section')?.id;
        return id ? '<a href="#' + id + '">' + h.textContent + '</a>' : '';
      }).join('');
    }
    const fab = document.querySelector('.fab');
    if (fab) {
      fab.addEventListener('click', () => {
        document.body.classList.toggle('toc-open');
        fab.textContent = document.body.classList.contains('toc-open') ? '✕' : '☰';
      });
      $$('a', side).forEach(a => a.addEventListener('click', () => {
        if (innerWidth <= 900) { document.body.classList.remove('toc-open'); fab.textContent = '☰'; }
      }));
    }
  }

  const progress = document.querySelector('.progress');
  if (progress) {
    addEventListener('scroll', () => {
      const h = document.documentElement;
      progress.style.width = (h.scrollTop / Math.max(1, h.scrollHeight - h.clientHeight) * 100).toFixed(1) + '%';
    }, { passive: true });
  }

  $$('[data-copy]').forEach(btn => {
    btn.addEventListener('click', async () => {
      try {
        await navigator.clipboard.writeText(btn.dataset.copy);
        const t = btn.textContent;
        btn.textContent = '已复制';
        setTimeout(() => btn.textContent = t, 1200);
      } catch {}
    });
  });

  /* PWA */
  if ('serviceWorker' in navigator && location.protocol.startsWith('http')) {
    const sw = location.pathname.includes('/assets/pages/') ? '../../sw.js' : 'sw.js';
    navigator.serviceWorker.register(sw).catch(() => {});
  }
})();
