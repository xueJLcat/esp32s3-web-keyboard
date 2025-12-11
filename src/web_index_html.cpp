#include "web_index_html.h"

const char INDEX_HTML[] PROGMEM = R"HTML(
<!DOCTYPE html>
<html lang="zh-CN">
<head>
  <meta charset="UTF-8" />
  <title>Xiao ESP32S3 Web 键盘</title>
  <meta name="viewport" content="width=device-width,initial-scale=1" />
  <style>
    :root {
      color-scheme: dark light;
      --bg: #050810;
      --bg-card: #111827;
      --accent: #3b82f6;
      --accent-soft: rgba(59,130,246,0.15);
      --accent-border: rgba(59,130,246,0.4);
      --text: #e5e7eb;
      --muted: #9ca3af;
      --danger: #f97373;
      --success: #22c55e;
      --radius-xl: 18px;
      --radius-lg: 14px;
      --shadow-soft: 0 18px 40px rgba(15,23,42,0.55);
    }

    * { box-sizing: border-box; }

    body {
      margin: 0;
      min-height: 100vh;
      display: flex;
      align-items: center;
      justify-content: center;
      font-family: system-ui, -apple-system, BlinkMacSystemFont, "SF Pro Text",
                   "Segoe UI", sans-serif;
      background:
        radial-gradient(circle at top, rgba(56,189,248,0.15), transparent 55%),
        radial-gradient(circle at bottom, rgba(129,140,248,0.18), transparent 55%),
        var(--bg);
      color: var(--text);
      padding: 24px;
    }

    .card {
      width: 100%;
      max-width: 780px;
      background: linear-gradient(145deg, rgba(15,23,42,0.96), rgba(15,23,42,0.86));
      border-radius: 26px;
      padding: 22px 22px 18px;
      box-shadow: var(--shadow-soft);
      border: 1px solid rgba(148,163,184,0.25);
      backdrop-filter: blur(26px);
    }

    .card-header {
      display: flex;
      justify-content: space-between;
      align-items: center;
      margin-bottom: 14px;
      gap: 12px;
      flex-wrap: wrap;
    }

    .title {
      font-size: 20px;
      font-weight: 650;
      letter-spacing: 0.02em;
      display: flex;
      align-items: center;
      gap: 8px;
    }

    .badge {
      font-size: 11px;
      padding: 3px 8px;
      border-radius: 999px;
      background: var(--accent-soft);
      color: var(--accent);
      border: 1px solid var(--accent-border);
    }

    .sub {
      font-size: 11px;
      color: var(--muted);
      margin-bottom: 8px;
    }

    .ip {
      font-family: "SF Mono", ui-monospace, Menlo, Monaco, Consolas, "Liberation Mono";
      font-size: 11px;
      padding: 3px 8px;
      border-radius: 999px;
      background: rgba(15,23,42,0.9);
      border: 1px solid rgba(55,65,81,0.9);
      color: var(--muted);
      white-space: nowrap;
    }

    .textarea-wrapper {
      position: relative;
      margin-bottom: 12px;
    }

    textarea {
      width: 100%;
      min-height: 100px;
      max-height: 160px;
      resize: vertical;
      background: radial-gradient(circle at top left, rgba(59,130,246,0.12), transparent 45%),
                  rgba(15,23,42,0.96);
      border-radius: var(--radius-xl);
      border: 1px solid rgba(75,85,99,0.9);
      padding: 12px 14px 28px;
      color: var(--text);
      font-size: 14px;
      line-height: 1.5;
      outline: none;
      transition: border 0.15s ease, box-shadow 0.15s ease, background 0.15s ease;
      box-shadow: inset 0 0 0 1px rgba(15,23,42,0.95);
    }

    textarea:focus {
      border-color: var(--accent);
      box-shadow:
        0 0 0 1px rgba(37,99,235,0.65),
        0 18px 40px rgba(15,23,42,0.75);
      background:
        radial-gradient(circle at top left, rgba(59,130,246,0.18), transparent 50%),
        rgba(15,23,42,0.98);
    }

    .textarea-footer {
      position: absolute;
      right: 10px;
      bottom: 7px;
      display: flex;
      align-items: center;
      gap: 8px;
      font-size: 11px;
      color: var(--muted);
    }

    button {
      border: none;
      cursor: pointer;
      font-family: inherit;
    }

    .btn-primary {
      padding: 6px 14px;
      border-radius: 999px;
      background: linear-gradient(135deg, #3b82f6, #6366f1);
      color: white;
      font-size: 12px;
      font-weight: 600;
      letter-spacing: 0.02em;
      box-shadow: 0 10px 25px rgba(59,130,246,0.65);
      display: inline-flex;
      align-items: center;
      gap: 6px;
      transition: transform 0.1s ease, box-shadow 0.1s ease, filter 0.1s ease;
      white-space: nowrap;
    }

    .btn-primary:hover {
      transform: translateY(-1px);
      box-shadow: 0 14px 30px rgba(37,99,235,0.8);
      filter: brightness(1.05);
    }

    .btn-primary:active {
      transform: translateY(0);
      box-shadow: 0 7px 18px rgba(37,99,235,0.6);
      filter: brightness(0.98);
    }

    .kbd-grid {
      margin-top: 12px;
      padding: 10px 10px 8px;
      border-radius: var(--radius-lg);
      background: radial-gradient(circle at top right, rgba(56,189,248,0.18), transparent 52%),
                  rgba(15,23,42,0.98);
      border: 1px solid rgba(75,85,99,0.9);
      box-shadow: 0 12px 32px rgba(15,23,42,0.65);
      font-size: 0;
      overflow-x: auto;
    }

    .kbd-rows {
      min-width: 640px;  /* 让整个键盘在窄屏时可以横向滚动，不会乱折行 */
    }

    .kbd-row {
      display: flex;
      gap: 6px;
      justify-content: flex-start;
      margin-bottom: 6px;
      flex-wrap: nowrap;
    }

    .key {
      min-width: 30px;
      padding: 6px 0;
      border-radius: 10px;
      background: radial-gradient(circle at top, rgba(31,41,55,0.8), rgba(15,23,42,1));
      border: 1px solid rgba(55,65,81,0.95);
      color: var(--text);
      font-size: 11px;
      text-align: center;
      box-shadow:
        0 2px 0 rgba(15,23,42,1),
        0 6px 12px rgba(15,23,42,0.9);
      transition: transform 0.07s ease, box-shadow 0.07s ease, border-color 0.07s ease;
      user-select: none;
      flex: 0 0 auto;
      white-space: nowrap;
    }

    .key:active {
      transform: translateY(1px);
      box-shadow:
        0 0 0 rgba(15,23,42,1),
        0 3px 8px rgba(15,23,42,0.8);
      border-color: rgba(96,165,250,0.9);
    }

    .key-wide {
      min-width: 60px;
      padding-inline: 10px;
    }

    .key-xwide {
      min-width: 80px;
      padding-inline: 10px;
    }

    .key-space {
      flex: 1 1 auto;
      min-width: 120px;
    }

    .macro-row {
      margin-top: 4px;
      padding-top: 6px;
      border-top: 1px dashed rgba(75,85,99,0.8);
    }

    .key-macro {
      min-width: 72px;
      font-size: 10px;
      padding-inline: 8px;
    }

    .status-line {
      margin-top: 8px;
      font-size: 11px;
      display: flex;
      justify-content: space-between;
      align-items: center;
      color: var(--muted);
      gap: 10px;
      flex-wrap: wrap;
    }

    .status-pill {
      padding: 3px 9px;
      border-radius: 999px;
      border: 1px solid rgba(55,65,81,0.95);
      background: rgba(15,23,42,0.95);
      display: inline-flex;
      align-items: center;
      gap: 6px;
    }

    .dot {
      width: 8px;
      height: 8px;
      border-radius: 999px;
      background: #facc15;
      box-shadow: 0 0 0 3px rgba(250,204,21,0.25);
    }

    .dot.ok {
      background: var(--success);
      box-shadow: 0 0 0 3px rgba(34,197,94,0.35);
    }

    .dot.err {
      background: var(--danger);
      box-shadow: 0 0 0 3px rgba(248,113,113,0.35);
    }

    @media (max-width: 600px) {
      .card { padding: 18px 14px 14px; max-width: 100%; }
      textarea { min-height: 90px; }
    }
  </style>
</head>
<body>
  <div class="card">
    <div class="card-header">
      <div>
        <div class="title">
          Xiao ESP32S3
          <span class="badge">Web HID Keyboard</span>
        </div>
        <div class="sub">通过手机 / 浏览器把按键发送到电脑（USB 键盘仿真）</div>
      </div>
      <div class="ip">AP: 192.168.4.1</div>
    </div>

    <div class="textarea-wrapper">
      <textarea id="textInput" placeholder="在这里输入要发送到电脑的文本，或使用下面的虚拟键盘点按…"></textarea>
      <div class="textarea-footer">
        <span id="charCount">0 / 512</span>
        <button class="btn-primary" id="sendBtn">
          <span>发送文本</span>
          <span>↵</span>
        </button>
      </div>
    </div>

    <!-- 虚拟键盘 -->
    <div class="kbd-grid">
      <div class="kbd-rows">

        <!-- 功能键行 -->
        <div class="kbd-row">
          <button class="key key-wide" data-key="ESC">Esc</button>
          <button class="key" data-key="F1">F1</button>
          <button class="key" data-key="F2">F2</button>
          <button class="key" data-key="F3">F3</button>
          <button class="key" data-key="F4">F4</button>
          <button class="key" data-key="F5">F5</button>
          <button class="key" data-key="F6">F6</button>
          <button class="key" data-key="F7">F7</button>
          <button class="key" data-key="F8">F8</button>
          <button class="key" data-key="F9">F9</button>
          <button class="key" data-key="F10">F10</button>
          <button class="key" data-key="F11">F11</button>
          <button class="key" data-key="F12">F12</button>
        </div>

        <!-- 数字行 -->
        <div class="kbd-row">
          <button class="key" data-char="`">`</button>
          <button class="key" data-char="1">1</button>
          <button class="key" data-char="2">2</button>
          <button class="key" data-char="3">3</button>
          <button class="key" data-char="4">4</button>
          <button class="key" data-char="5">5</button>
          <button class="key" data-char="6">6</button>
          <button class="key" data-char="7">7</button>
          <button class="key" data-char="8">8</button>
          <button class="key" data-char="9">9</button>
          <button class="key" data-char="0">0</button>
          <button class="key" data-char="-">-</button>
          <button class="key" data-char="=">=</button>
          <button class="key key-xwide" data-key="BACKSPACE">⌫ Backspace</button>
        </div>

        <!-- Q 行 -->
        <div class="kbd-row">
          <button class="key key-wide" data-key="TAB">Tab</button>
          <button class="key" data-char="Q">Q</button>
          <button class="key" data-char="W">W</button>
          <button class="key" data-char="E">E</button>
          <button class="key" data-char="R">R</button>
          <button class="key" data-char="T">T</button>
          <button class="key" data-char="Y">Y</button>
          <button class="key" data-char="U">U</button>
          <button class="key" data-char="I">I</button>
          <button class="key" data-char="O">O</button>
          <button class="key" data-char="P">P</button>
          <button class="key" data-char="[">[</button>
          <button class="key" data-char="]">]</button>
          <button class="key" data-char="\\">\</button>
        </div>

        <!-- A 行 -->
        <div class="kbd-row">
          <button class="key key-xwide" data-key="CAPS">Caps</button>
          <button class="key" data-char="A">A</button>
          <button class="key" data-char="S">S</button>
          <button class="key" data-char="D">D</button>
          <button class="key" data-char="F">F</button>
          <button class="key" data-char="G">G</button>
          <button class="key" data-char="H">H</button>
          <button class="key" data-char="J">J</button>
          <button class="key" data-char="K">K</button>
          <button class="key" data-char="L">L</button>
          <button class="key" data-char=";">;</button>
          <button class="key" data-char="'">'</button>
          <button class="key key-xwide" data-key="ENTER">Enter</button>
        </div>

        <!-- Z 行 -->
        <div class="kbd-row">
          <button class="key key-xwide" data-key="LSHIFT">Shift</button>
          <button class="key" data-char="Z">Z</button>
          <button class="key" data-char="X">X</button>
          <button class="key" data-char="C">C</button>
          <button class="key" data-char="V">V</button>
          <button class="key" data-char="B">B</button>
          <button class="key" data-char="N">N</button>
          <button class="key" data-char="M">M</button>
          <button class="key" data-char=",">,</button>
          <button class="key" data-char=".">.</button>
          <button class="key" data-char="/">/</button>
          <button class="key key-xwide" data-key="RSHIFT">Shift</button>
        </div>

        <!-- Ctrl / Alt / 空格 行 -->
        <div class="kbd-row">
          <button class="key key-wide" data-key="LCTRL">Ctrl</button>
          <button class="key key-wide" data-key="LGUI">Win</button>
          <button class="key key-wide" data-key="LALT">Alt</button>
          <button class="key key-space" data-key="SPACE">Space</button>
          <button class="key key-wide" data-key="RALT">Alt</button>
          <button class="key key-wide" data-key="RGUI">Win</button>
          <button class="key key-wide" data-key="RCTRL">Ctrl</button>
        </div>

        <!-- 导航键行 -->
        <div class="kbd-row">
          <button class="key" data-key="INSERT">Ins</button>
          <button class="key" data-key="HOME">Home</button>
          <button class="key" data-key="PGUP">PgUp</button>
          <button class="key" data-key="DELETE">Del</button>
          <button class="key" data-key="END">End</button>
          <button class="key" data-key="PGDN">PgDn</button>
          <span style="flex:1 1 auto;"></span>
          <button class="key" data-key="UP">↑</button>
        </div>
        <div class="kbd-row">
          <span style="flex:1 1 auto;"></span>
          <button class="key" data-key="LEFT">←</button>
          <button class="key" data-key="DOWN">↓</button>
          <button class="key" data-key="RIGHT">→</button>
        </div>

        <!-- 组合键宏 -->
        <div class="kbd-row macro-row">
          <button class="key key-macro" data-combo="LCTRL+C">Ctrl+C 复制</button>
          <button class="key key-macro" data-combo="LCTRL+V">Ctrl+V 粘贴</button>
          <button class="key key-macro" data-combo="LCTRL+X">Ctrl+X 剪切</button>
          <button class="key key-macro" data-combo="LCTRL+Z">Ctrl+Z 撤销</button>
          <button class="key key-macro" data-combo="LCTRL+Y">Ctrl+Y 重做</button>
          <button class="key key-macro" data-combo="LCTRL+A">Ctrl+A 全选</button>
          <button class="key key-macro" data-combo="LGUI+D">Win+D 显示桌面</button>
          <button class="key key-macro" data-combo="LGUI+L">Win+L 锁屏</button>
          <button class="key key-macro" data-combo="LALT+F4">Alt+F4 关闭</button>
        </div>

      </div>
    </div>

    <div class="status-line">
      <div class="status-pill">
        <span id="statusDot" class="dot"></span>
        <span id="statusText">就绪</span>
      </div>
      <div style="font-size:11px;color:var(--muted);">
        提示：手机连上 <strong>XiaoKeyboard</strong> Wi-Fi 后，直接访问 <strong>http://192.168.4.1</strong>
      </div>
    </div>
  </div>

  <script>
    const textInput  = document.getElementById('textInput');
    const sendBtn    = document.getElementById('sendBtn');
    const statusDot  = document.getElementById('statusDot');
    const statusText = document.getElementById('statusText');
    const charCount  = document.getElementById('charCount');
    const MAX_LEN    = 512;

    function setStatus(kind, msg) {
      statusDot.classList.remove('ok', 'err');
      if (kind === 'ok') statusDot.classList.add('ok');
      else if (kind === 'err') statusDot.classList.add('err');
      statusText.textContent = msg;
    }

    async function postJSON(path, payload) {
      try {
        const res = await fetch(path, {
          method: 'POST',
          headers: { 'Content-Type': 'application/json' },
          body: JSON.stringify(payload),
        });
        const json = await res.json().catch(() => ({}));
        if (!res.ok || !json.ok) {
          throw new Error(json.error || ('HTTP ' + res.status));
        }
        return json;
      } catch (e) {
        console.error(e);
        setStatus('err', '发送失败：' + e.message);
        throw e;
      }
    }

    async function sendText() {
      const text = textInput.value;
      if (!text) {
        setStatus('err', '文本为空');
        return;
      }
      if (text.length > MAX_LEN) {
        setStatus('err', '文本过长，最大 ' + MAX_LEN + ' 字符');
        return;
      }
      setStatus('', '发送中…');
      await postJSON('/send', { text });
      setStatus('ok', '已排队发送 (' + text.length + ' 字符)');
    }

    async function sendSpecial(keyName) {
      setStatus('', '发送按键 ' + keyName + '…');
      await postJSON('/send', { key: keyName });
      setStatus('ok', '已发送按键 ' + keyName);
    }

    async function sendChar(ch) {
      await postJSON('/send', { text: ch });
      setStatus('ok', '已发送：' + ch);
    }

    async function sendCombo(combo) {
      setStatus('', '发送组合键 ' + combo + '…');
      await postJSON('/send', { combo });
      setStatus('ok', '已发送组合键 ' + combo);
    }

    sendBtn.addEventListener('click', () => {
      sendText().catch(() => {});
    });

    textInput.addEventListener('input', () => {
      const len = textInput.value.length;
      charCount.textContent = len + ' / ' + MAX_LEN;
    });

    document.querySelectorAll('.key').forEach(btn => {
      btn.addEventListener('click', () => {
        const ch    = btn.getAttribute('data-char');
        const key   = btn.getAttribute('data-key');
        const combo = btn.getAttribute('data-combo');

        if (combo) {
          sendCombo(combo).catch(() => {});
        } else if (ch !== null && ch !== undefined) {
          sendChar(ch).catch(() => {});
        } else if (key !== null && key !== undefined) {
          sendSpecial(key).catch(() => {});
        }
      });
    });
  </script>
</body>
</html>

)HTML";
