#include "WebRouter.h"
#include "StateManager.h"

static const char PAGE2[] PROGMEM = R"HTML(<!DOCTYPE html><meta name=viewport content="width=device-width,initial-scale=1"><style>body{font-family:sans-serif;margin:16px;max-width:760px;margin-left:auto;margin-right:auto}h2{margin:0 0 12px}section{border:1px solid #ddd;border-radius:10px;padding:12px;margin:12px 0}section>h3{margin:0 0 8px;font-size:18px}label{display:block;margin:10px 0 6px}input[type=range]{width:100%}button{margin:6px 6px 0 0}.row{display:flex;gap:8px;align-items:center;flex-wrap:wrap}.pill{padding:6px 10px;border:1px solid #ccc;border-radius:20px;background:#f7f7f7}.on{background:#def}.off{background:#fdd}.msg{position:fixed;bottom:16px;left:50%;transform:translateX(-50%);padding:8px 12px;border-radius:6px;background:#333;color:#fff;opacity:.85;font-size:12px}</style><h2>ESP32C3 灯光控制</h2><section id=sc><h3>场景/预设</h3><div class=row><button class=pill onclick="sceneApply(1)">应用1</button><button class=pill onclick="sceneSave(1)">保存1</button><button class=pill onclick="sceneApply(2)">应用2</button><button class=pill onclick="sceneSave(2)">保存2</button><button class=pill onclick="sceneApply(3)">应用3</button><button class=pill onclick="sceneSave(3)">保存3</button><button class=pill onclick="sceneApply(4)">应用4</button><button class=pill onclick="sceneSave(4)">保存4</button></div></section><section id=cct><h3>双色温LED灯光</h3><div class=row><div class=pill>色温预设</div><button class=pill onclick="presetCT(2700)">暖光</button><button class=pill onclick="presetCT(4000)">中性光</button><button class=pill onclick="presetCT(6500)">冷光</button></div><label>色温(K)</label><input id=ct type=range min=2700 max=6500 step=100><div>当前:<span id=ctv></span>K</div><label>亮度(%)</label><input id=b type=range min=0 max=100 step=1><div>当前:<span id=bv></span>%</div><div class=row><label>双色温开关</label><button id=cctBtn class=pill off>关闭</button></div></section><section id=ws><h3>彩灯开关控制</h3><div class=row><button id=wsBtn class=pill off>关闭</button></div><label>亮度</label><input id=wb type=range min=0 max=255 step=1><div>当前:<span id=wbv></span></div></section><section id=fxsec><h3>动画效果</h3><label>模式</label><select id=fx><option value="off">关闭</option><option value="rainbow">彩虹</option><option value="breathe">呼吸</option><option value="breathe_color">呼吸变色</option><option value="solid">常亮</option><option value="wipe">跑点</option><option value="manual">手动像素</option></select><div id=params></div></section><div id=msg class=msg style="display:none"></div><script>let S=null;function toast(s){let m=document.getElementById('msg');m.textContent=s;m.style.display='block';clearTimeout(m._h);m._h=setTimeout(()=>{m.style.display='none'},800)}function hexToRGB(h){return {r:parseInt(h.substr(1,2),16),g:parseInt(h.substr(3,2),16),b:parseInt(h.substr(5,2),16)}}function rgbToHex(r,g,b){let h=(v)=>('0'+v.toString(16)).slice(-2);return '#'+h(r)+h(g)+h(b)}async function state(){let r=await fetch('/state',{cache:'no-store'});S=await r.json()}async function apply(url){await fetch(url,{cache:'no-store'})}let _debT={};function applyDebounced(k,url){clearTimeout(_debT[k]);_debT[k]=setTimeout(()=>{apply(url)},180)}async function sceneApply(n){await apply('/scene/apply?slot='+n);toast('已应用场景'+n);await refresh()}async function sceneSave(n){await apply('/scene/save?slot='+n);toast('已保存场景'+n)}async function presetCT(k){await apply('/set?ct='+k);document.getElementById('ct').value=k;document.getElementById('ctv').textContent=k}function renderBasics(){document.getElementById('ct').value=S.ct;document.getElementById('ctv').textContent=S.ct;document.getElementById('b').value=S.b;document.getElementById('bv').textContent=S.b;document.getElementById('wb').value=S.wb;document.getElementById('wbv').textContent=S.wb;document.getElementById('fx').value=S.fx;let cctBtn=document.getElementById('cctBtn');cctBtn.textContent=S.cctOn?'开启':'关闭';cctBtn.className='pill '+(S.cctOn?'on':'off');let wsBtn=document.getElementById('wsBtn');wsBtn.textContent=S.wsOn?'开启':'关闭';wsBtn.className='pill '+(S.wsOn?'on':'off')}function renderParams(){let p=document.getElementById('params');p.innerHTML='';if(S.fx==='breathe'){let h=document.createElement('div');h.innerHTML='<label>呼吸周期(秒)</label><input id=bs type=range min=0.2 max=10 step=0.1><div>当前:<span id=bsv></span>s</div><label>颜色</label><input id=col type=color>';p.appendChild(h);document.getElementById('bs').value=S.bs;document.getElementById('bsv').textContent=S.bs;document.getElementById('col').value=S.hex;document.getElementById('bs').oninput=(e)=>{document.getElementById('bsv').textContent=e.target.value;applyDebounced('bs','/set?bs='+e.target.value)};document.getElementById('col').oninput=(e)=>{let c=hexToRGB(e.target.value);applyDebounced('col','/set?cr='+c.r+'&cg='+c.g+'&cb='+c.b)}}else if(S.fx==='breathe_color'){let h=document.createElement('div');h.innerHTML='<label>呼吸周期(秒)</label><input id=bs type=range min=0.2 max=10 step=0.1><div>当前:<span id=bsv></span>s</div>';p.appendChild(h);document.getElementById('bs').value=S.bs;document.getElementById('bsv').textContent=S.bs;document.getElementById('bs').oninput=(e)=>{document.getElementById('bsv').textContent=e.target.value;applyDebounced('bs','/set?bs='+e.target.value)}}else if(S.fx==='solid'){let h=document.createElement('div');h.innerHTML='<label>颜色</label><input id=col type=color><div class=row><label>颜色循环</label><button id=scBtn class=pill>关闭</button></div><label>循环周期(秒)</label><input id=scs type=range min=0.2 max=20 step=0.1><div>当前:<span id=scsv></span>s</div>';p.appendChild(h);document.getElementById('col').value=S.hex;document.getElementById('scs').value=S.scs;document.getElementById('scsv').textContent=S.scs;let scBtn=document.getElementById('scBtn');scBtn.textContent=S.scOn?'开启':'关闭';scBtn.className='pill '+(S.scOn?'on':'off');document.getElementById('col').oninput=(e)=>{let c=hexToRGB(e.target.value);applyDebounced('col','/set?cr='+c.r+'&cg='+c.g+'&cb='+c.b)};document.getElementById('scs').oninput=(e)=>{document.getElementById('scsv').textContent=e.target.value;applyDebounced('scs','/set?scs='+e.target.value)};scBtn.onclick=async()=>{let on=!S.scOn;S.scOn=on;scBtn.textContent=on?'开启':'关闭';scBtn.className='pill '+(on?'on':'off');await apply('/set?scOn='+(on?'1':'0'))}}else if(S.fx==='manual'){let h=document.createElement('div');h.innerHTML='<div class=row><a class=pill href="/manual">打开手动像素控制页面</a></div>';p.appendChild(h)}}async function refresh(){await state();renderBasics();renderParams()}function setupEvents(){try{let es=new EventSource('/events');es.onmessage=(e)=>{try{S=JSON.parse(e.data);renderBasics();renderParams()}catch(err){}};es.onerror=()=>{}}catch(e){}}document.getElementById('ct').oninput=(e)=>{document.getElementById('ctv').textContent=e.target.value;applyDebounced('ct','/set?ct='+e.target.value)};document.getElementById('b').oninput=(e)=>{document.getElementById('bv').textContent=e.target.value;applyDebounced('b','/set?b='+e.target.value)};document.getElementById('wb').oninput=(e)=>{document.getElementById('wbv').textContent=e.target.value;applyDebounced('wb','/set?wb='+e.target.value)};document.getElementById('fx').onchange=async(e)=>{await apply('/effect?name='+e.target.value);await refresh()};document.getElementById('cctBtn').onclick=async()=>{let on=!S.cctOn;S.cctOn=on;let b=document.getElementById('cctBtn');b.textContent=on?'开启':'关闭';b.className='pill '+(on?'on':'off');await apply('/set?cctOn='+(on?'1':'0'))};document.getElementById('wsBtn').onclick=async()=>{let on=!S.wsOn;S.wsOn=on;let b=document.getElementById('wsBtn');b.textContent=on?'开启':'关闭';b.className='pill '+(on?'on':'off');await apply('/set?wsOn='+(on?'1':'0'))};setupEvents();refresh()</script>)HTML";
static const char PAGE[] PROGMEM = R"HTML(<!DOCTYPE html><meta name=viewport content="width=device-width,initial-scale=1"><style>body{font-family:sans-serif;margin:16px;max-width:760px;margin-left:auto;margin-right:auto}h2{margin:0 0 12px}section{border:1px solid #ddd;border-radius:10px;padding:12px;margin:12px 0}section>h3{margin:0 0 8px;font-size:18px}label{display:block;margin:10px 0 6px}input[type=range]{width:100%}button{margin:6px 6px 0 0} .row{display:flex;gap:8px;align-items:center;flex-wrap:wrap} .pill{padding:6px 10px;border:1px solid #ccc;border-radius:20px;background:#f7f7f7} .on{background:#def} .off{background:#fdd} .msg{position:fixed;bottom:16px;left:50%;transform:translateX(-50%);padding:8px 12px;border-radius:6px;background:#333;color:#fff;opacity:.85;font-size:12px}</style><h2>ESP32C3 灯光控制</h2><section id=cct><h3>双色温LED灯光</h3><div class=row><div class=pill>色温预设</div><button class=pill onclick="presetCT(2700)">暖光</button><button class=pill onclick="presetCT(4000)">中性光</button><button class=pill onclick="presetCT(6500)">冷光</button></div><label>色温(K)</label><input id=ct type=range min=2700 max=6500 step=100><div class=row><div>当前:<span id=ctv></span>K</div></div><label>亮度(%)</label><input id=b type=range min=0 max=100 step=1><div class=row><div>当前:<span id=bv></span>%</div></div><div class=row><label>双色温开关</label><button id=cctBtn class=pill off>关闭</button></div></section><section id=ws><h3>彩灯开关控制</h3><div class=row><button id=wsBtn class=pill off>关闭</button></div><label>亮度</label><input id=wb type=range min=0 max=255 step=1><div class=row><div>当前:<span id=wbv></span></div></div></section><section id=fxsec><h3>动画效果</h3><label>模式</label><select id=fx><option value="off">关闭</option><option value="rainbow">彩虹</option><option value="breathe">呼吸</option><option value="breathe_color">呼吸变色</option><option value="solid">常亮</option><option value="wipe">跑点</option></select><div id=params></div></section><div id=msg class=msg style="display:none"></div><script>let ctrl=null;function toast(s,ok=true){let m=document.getElementById('msg');m.textContent=s;m.style.display='block';m.style.background=ok?'#333':'#a33';clearTimeout(m._h);m._h=setTimeout(()=>{m.style.display='none'},800)}function hex(){let el=document.getElementById('col');return el?el.value:'#ffffff'}function hexToRGB(h){let r=parseInt(h.substr(1,2),16),g=parseInt(h.substr(3,2),16),b=parseInt(h.substr(5,2),16);return {r,g,b}}function build(){let c=hexToRGB(hex());let u='/set?ct='+ct.value+'&b='+b.value+'&wb='+wb.value+'&wsOn='+(wsBtn.classList.contains('on')?1:0)+'&cctOn='+(cctBtn.classList.contains('on')?1:0)+'&cr='+c.r+'&cg='+c.g+'&cb='+c.b;let mode=fx.value;if(mode==='breathe'||mode==='breathe_color'){let bs=document.getElementById('bs');if(bs)u+='&bs='+bs.value}else if(mode==='solid'){let scOn=document.getElementById('scOn');let scs=document.getElementById('scs');if(scOn&&scs)u+='&scOn='+(scOn.checked?1:0)+'&scs='+scs.value}return u}async function applyNow(){try{if(ctrl)ctrl.abort();ctrl=new AbortController();let r=await fetch(build(),{cache:'no-store',signal:ctrl.signal});toast('已同步',r.ok)}catch(e){}}function presetCT(k){ct.value=k;ctv.textContent=k;applyNow()}function setOn(btn,on){btn.classList.toggle('on',on);btn.classList.toggle('off',!on);btn.textContent=on?'开启':'关闭'}async function refresh(){let r=await fetch('/state',{cache:'no-store'});let s=await r.json();ct.value=s.ct;b.value=s.b;wb.value=s.wb;ctv.textContent=s.ct;bv.textContent=s.b;wbv.textContent=s.wb;setOn(wsBtn,!!s.wsOn);setOn(cctBtn,!!s.cctOn);window._bs=s.bs||2;window._scs=s.scs||2;window._scOn=!!s.scOn;fx.value=s.fx||'off';renderParams();let el=document.getElementById('col');if(el){el.value=s.hex||'#ffffff'}}function renderParams(){let p=document.getElementById('params');let m=fx.value;if(m==='breathe'||m==='breathe_color'){p.innerHTML='<div class=row><label>呼吸颜色</label><input id=col type=color></div><div class=row><label>呼吸速度(秒)</label><input id=bs type=range min=0.5 max=5 step=0.1><span id=bsv></span></div>';let bs=document.getElementById('bs');bs.oninput=()=>{bsv.textContent=bs.value;applyNow()};document.getElementById('col').oninput=applyNow;bs.value=(window._bs||2);bsv.textContent=bs.value}else if(m==='solid'){p.innerHTML='<div class=row><label>颜色</label><input id=col type=color></div><div class=row><label>颜色变化</label><input id=scOn type=checkbox><label>变化速度(秒)</label><input id=scs type=range min=0.5 max=10 step=0.1><span id=scsv></span></div>';document.getElementById('col').oninput=applyNow;let scOn=document.getElementById('scOn');let scs=document.getElementById('scs');scOn.checked=window._scOn;scs.value=(window._scs||2);scsv.textContent=scs.value;scOn.onchange=applyNow;scs.oninput=()=>{scsv.textContent=scs.value;applyNow()} }else{p.innerHTML=''} }function setEffect(){fetch('/effect?name='+fx.value,{cache:'no-store'}).then(()=>toast('效果已切换',true)).catch(()=>toast('切换失败',false));renderParams()}wsBtn.onclick=()=>{setOn(wsBtn,!wsBtn.classList.contains('on'));applyNow()};cctBtn.onclick=()=>{setOn(cctBtn,!cctBtn.classList.contains('on'));applyNow()};ct.oninput=()=>{ctv.textContent=ct.value;applyNow()};b.oninput=()=>{bv.textContent=b.value;applyNow()};wb.oninput=()=>{wbv.textContent=wb.value;applyNow()};document.getElementById('fx').onchange=setEffect;refresh()</script>)HTML";

static const char INDEX_HTML[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>手办盒灯光控制面板</title>
<style>
* {
  box-sizing: border-box;
  margin: 0;
  padding: 0;
  font-family: -apple-system, BlinkMacSystemFont, "Segoe UI", Roboto, "Helvetica Neue", Arial, "PingFang SC", "Microsoft Yahei", sans-serif;
}
html {
  scroll-behavior: smooth;
}
body {
  background: radial-gradient(circle at top, #1f2937 0, #020617 55%, #000 100%);
  color: rgba(229, 231, 235, 1);
  min-height: 100vh;
  display: flex;
  align-items: center;
  justify-content: center;
  padding: 28px;
  font-size: 14px;
  line-height: 1.6;
}
.app {
  width: 100%;
  max-width: 1120px;
  background: linear-gradient(145deg, rgba(15,23,42,0.96), rgba(17,24,39,0.98));
  border-radius: 24px;
  box-shadow:
    0 20px 60px rgba(0,0,0,0.75),
    0 0 0 1px rgba(148,163,184,0.2);
  padding: 26px 30px 30px;
  backdrop-filter: blur(16px);
}
.app-header {
  display: flex;
  flex-direction: column;
  gap: 16px;
  margin-bottom: 24px;
}
.header-top {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 18px;
}
.header-actions {
  display: flex;
  align-items: center;
  gap: 10px;
  flex-wrap: wrap;
}
.title-block {
  display: flex;
  align-items: center;
  gap: 12px;
}
.logo-dot {
  width: 32px;
  height: 32px;
  border-radius: 999px;
  background: conic-gradient(from 180deg, #f97316, #22c55e, #3b82f6, #a855f7, #f97316);
  padding: 2px;
}
.logo-inner {
  width: 100%;
  height: 100%;
  border-radius: inherit;
  background: radial-gradient(circle at 30% 20%, #facc15, #0f172a 55%, #020617 100%);
  box-shadow: 0 0 20px rgba(249,115,22,0.45);
}
.title-text-main {
  font-size: 20px;
  font-weight: 600;
  letter-spacing: 0.04em;
}
.title-text-sub {
  font-size: 12px;
  color: #9ca3af;
}
.connection {
  display: flex;
  align-items: center;
  gap: 10px;
}
.connection label {
  font-size: 13px;
  color: #9ca3af;
}
.input {
  background: rgba(15,23,42,0.95);
  border-radius: 999px;
  border: 1px solid rgba(148,163,184,0.35);
  padding: 10px 14px;
  color: #e5e7eb;
  font-size: 14px;
  outline: none;
  min-width: 160px;
  min-height: 44px;
}
.input:focus {
  border-color: #f97316;
  box-shadow: 0 0 0 1px rgba(249,115,22,0.4);
}
.btn {
  border-radius: 999px;
  padding: 10px 16px;
  border: none;
  font-size: 14px;
  cursor: pointer;
  display: inline-flex;
  align-items: center;
  justify-content: center;
  gap: 6px;
  white-space: nowrap;
  min-height: 44px;
  touch-action: manipulation;
  transition: transform 0.08s ease-out, box-shadow 0.12s ease-out, background 0.15s ease-out;
}
.btn-primary {
  background: linear-gradient(135deg, #f97316, #fbbf24);
  color: #111827;
  box-shadow: 0 10px 30px rgba(249,115,22,0.5);
}
.btn-primary:active {
  transform: translateY(1px) scale(0.98);
  box-shadow: 0 4px 16px rgba(249,115,22,0.45);
}
.btn-ghost {
  background: rgba(15,23,42,0.85);
  color: #e5e7eb;
  border: 1px solid rgba(148,163,184,0.45);
}
.btn-ghost:active {
  transform: translateY(1px) scale(0.98);
  box-shadow: 0 4px 16px rgba(15,23,42,0.65);
}
.status-pill {
  display: inline-flex;
  align-items: center;
  gap: 6px;
  border-radius: 999px;
  padding: 8px 12px;
  background: rgba(15,23,42,0.9);
  border: 1px solid rgba(55,65,81,0.9);
  font-size: 12px;
  color: #9ca3af;
  min-height: 36px;
}
.status-dot {
  width: 8px;
  height: 8px;
  border-radius: 999px;
  background: #6b7280;
  box-shadow: 0 0 0 1px rgba(31,41,55,0.7);
}
.status-dot.online {
  background: #22c55e;
  box-shadow: 0 0 0 1px rgba(22,163,74,0.6), 0 0 12px rgba(22,163,74,0.9);
}
.status-dot.offline {
  background: #ef4444;
  box-shadow: 0 0 0 1px rgba(220,38,38,0.6), 0 0 10px rgba(220,38,38,0.7);
}
.grid {
  display: grid;
  grid-template-columns: 1.3fr 1.7fr 1.3fr;
  gap: 20px;
}
.card {
  background: radial-gradient(circle at top left, rgba(248,250,252,0.04), transparent 55%);
  border-radius: 18px;
  padding: 18px 18px 16px;
  border: 1px solid rgba(30,64,175,0.5);
  box-shadow:
    0 18px 38px rgba(15,23,42,0.75),
    inset 0 0 30px rgba(15,23,42,0.7);
  position: relative;
  overflow: hidden;
  transition: transform 0.12s ease-out, box-shadow 0.2s ease-out;
}
.card::before {
  content: "";
  position: absolute;
  inset: 0;
  background: radial-gradient(circle at top right, rgba(56,189,248,0.18), transparent 55%);
  opacity: 0.7;
  pointer-events: none;
}
.card-inner {
  position: relative;
  z-index: 1;
}
.card-header {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-bottom: 12px;
  gap: 10px;
  flex-wrap: wrap;
}
.card-header-actions {
  display: flex;
  align-items: center;
  gap: 8px;
  flex-wrap: wrap;
}
.card-body {
  display: block;
}
.collapse-toggle {
  border-radius: 999px;
  padding: 8px 12px;
  border: 1px solid rgba(148,163,184,0.45);
  background: rgba(15,23,42,0.85);
  color: #e5e7eb;
  font-size: 12px;
  cursor: pointer;
  display: inline-flex;
  align-items: center;
  min-height: 36px;
}
.card-title {
  font-size: 13px;
  font-weight: 600;
  letter-spacing: 0.08em;
  text-transform: uppercase;
  color: #9ca3af;
}
.card-tag {
  font-size: 11px;
  padding: 3px 8px;
  border-radius: 999px;
  border: 1px solid rgba(148,163,184,0.5);
  background: rgba(15,23,42,0.9);
  color: #9ca3af;
}
.status-main {
  display: flex;
  align-items: center;
  gap: 14px;
  margin-bottom: 12px;
}
.preview-orb {
  width: 80px;
  height: 80px;
  border-radius: 32px;
  background: radial-gradient(circle at 30% 20%, #f97316, #111827 65%, #020617 100%);
  box-shadow:
    0 0 25px rgba(249,115,22,0.9),
    0 15px 40px rgba(0,0,0,0.95);
  border: 1px solid rgba(248,250,252,0.12);
  position: relative;
  flex-shrink: 0;
}
.preview-orb-inner {
  position: absolute;
  inset: 18%;
  border-radius: 24px;
  background: radial-gradient(circle at 30% 20%, rgba(248,250,252,0.9), transparent 70%);
  mix-blend-mode: screen;
  opacity: 0.85;
}
.preview-orb-color {
  position: absolute;
  inset: 6px;
  border-radius: 28px;
  background: #f97316;
  opacity: 0.95;
}
.status-text {
  flex: 1;
}
.status-line {
  display: flex;
  justify-content: space-between;
  font-size: 13px;
  margin-bottom: 6px;
}
.status-label {
  color: #9ca3af;
}
.status-value {
  color: #e5e7eb;
  font-weight: 500;
}
.status-meta {
  display: grid;
  grid-template-columns: repeat(3, minmax(0,1fr));
  gap: 6px;
  margin-bottom: 10px;
  margin-top: 4px;
}
.chip {
  border-radius: 999px;
  border: 1px solid rgba(55,65,81,0.9);
  background: rgba(15,23,42,0.9);
  padding: 4px 7px;
  font-size: 11px;
  color: #9ca3af;
  display: flex;
  justify-content: space-between;
  gap: 6px;
}
.chip span:last-child {
  color: #e5e7eb;
}
.tag-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  font-size: 11px;
  color: #9ca3af;
}
.tag-row-right {
  display: flex;
  gap: 6px;
  align-items: center;
}
.pill {
  padding: 3px 8px;
  border-radius: 999px;
  border: 1px solid rgba(55,65,81,0.9);
  background: rgba(15,23,42,0.9);
}
.pill-success {
  border-color: rgba(34,197,94,0.9);
  color: #bbf7d0;
}
.pill-danger {
  border-color: rgba(239,68,68,0.9);
  color: #fecaca;
}
.section {
  margin-bottom: 14px;
}
.section-title {
  font-size: 13px;
  color: #9ca3af;
  margin-bottom: 8px;
  display: flex;
  justify-content: space-between;
  align-items: center;
}
.section-title span:last-child {
  font-size: 11px;
  color: #6b7280;
}
.controls-column {
  display: flex;
  flex-direction: column;
  gap: 14px;
}
.field-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  gap: 10px;
  margin-bottom: 10px;
}
.field-label {
  font-size: 13px;
  color: #d1d5db;
}
.field-sub {
  font-size: 12px;
  color: #6b7280;
}
.field-sub-right {
  text-align: right;
  flex: 1;
}
.field-row-gap {
  margin-bottom: 14px;
}
.field-right {
  display: flex;
  align-items: center;
  gap: 10px;
}
.slider {
  appearance: none;
  width: 180px;
  max-width: 100%;
  height: 6px;
  border-radius: 999px;
  background: rgba(31,41,55,0.9);
  outline: none;
  touch-action: pan-x;
}
.slider::-webkit-slider-thumb {
  appearance: none;
  width: 20px;
  height: 20px;
  border-radius: 999px;
  background: #f97316;
  box-shadow: 0 0 0 3px rgba(249,115,22,0.35);
}
.slider::-moz-range-thumb {
  width: 20px;
  height: 20px;
  border-radius: 999px;
  background: #f97316;
  box-shadow: 0 0 0 3px rgba(249,115,22,0.35);
  border: none;
}
.toggle {
  position: relative;
  width: 70px;
  height: 44px;
  border-radius: 999px;
  background: rgba(31,41,55,0.95);
  border: 1px solid rgba(75,85,99,0.9);
  cursor: pointer;
  display: inline-flex;
  align-items: center;
  padding: 6px;
  touch-action: manipulation;
  transition: background 0.15s ease-out, border 0.15s ease-out;
}
.toggle-circle {
  width: 20px;
  height: 20px;
  border-radius: 999px;
  background: #9ca3af;
  transition: transform 0.16s ease-out, background 0.16s ease-out, box-shadow 0.16s ease-out;
}
.toggle[data-on="true"] {
  background: linear-gradient(135deg, #22c55e, #4ade80);
  border-color: rgba(34,197,94,0.95);
}
.toggle[data-on="true"] .toggle-circle {
  transform: translateX(26px);
  background: #022c22;
  box-shadow: 0 0 0 1px rgba(16,185,129,0.85), 0 0 10px rgba(16,185,129,0.95);
}
.toggle-input {
  display: none;
}
.value {
  font-size: 12px;
  min-width: 50px;
  text-align: right;
  color: #e5e7eb;
}
.color-input {
  width: 44px;
  height: 44px;
  border-radius: 999px;
  border: 1px solid rgba(148,163,184,0.6);
  padding: 2px;
  background: transparent;
}
.select {
  background: rgba(15,23,42,0.95);
  border-radius: 999px;
  border: 1px solid rgba(148,163,184,0.35);
  padding: 10px 14px;
  color: #e5e7eb;
  font-size: 14px;
  outline: none;
  min-height: 44px;
}
.scene-actions {
  display: flex;
  flex-wrap: wrap;
  align-items: center;
  gap: 8px;
}
.scene-actions .select {
  min-width: 140px;
}
.wifi-row {
  display: flex;
  align-items: center;
  gap: 8px;
  margin-bottom: 8px;
  flex-wrap: wrap;
}
.wifi-label {
  width: 48px;
  font-size: 12px;
  color: #9ca3af;
}
.wifi-input {
  flex: 1;
}
.wifi-select {
  min-width: 0;
}
.wifi-status {
  font-size: 11px;
  color: #9ca3af;
  display: flex;
  flex-wrap: wrap;
  gap: 6px;
}
.advanced {
  margin-top: 10px;
  border-top: 1px dashed rgba(71,85,105,0.6);
  padding-top: 8px;
}
.advanced summary {
  cursor: pointer;
  color: #9ca3af;
  font-size: 12px;
  list-style: none;
}
.advanced summary::-webkit-details-marker {
  display: none;
}
.wifi-badge {
  padding: 3px 8px;
  border-radius: 999px;
  border: 1px solid rgba(55,65,81,0.9);
}
.wifi-badge.highlight {
  border-color: rgba(59,130,246,0.95);
  color: #bfdbfe;
}
.footer-row {
  display: flex;
  align-items: center;
  justify-content: space-between;
  margin-top: 14px;
  font-size: 12px;
  color: #6b7280;
}
.footer-right {
  display: flex;
  align-items: center;
  gap: 14px;
}
.shortcut-panel {
  position: absolute;
  top: calc(100% + 8px);
  right: 0;
  background: rgba(15,23,42,0.98);
  border: 1px solid rgba(148,163,184,0.35);
  border-radius: 14px;
  padding: 10px 12px;
  display: none;
  flex-direction: column;
  gap: 6px;
  min-width: 220px;
  z-index: 10;
  box-shadow: 0 16px 30px rgba(0,0,0,0.45);
  font-size: 12px;
  color: #9ca3af;
}
.shortcut-panel.show {
  display: flex;
}
.shortcut-item {
  display: flex;
  justify-content: space-between;
  gap: 8px;
}
.shortcut-key {
  color: #e5e7eb;
  font-weight: 600;
}
.shortcut-wrap {
  position: relative;
}
.error-text {
  color: #fecaca;
}
.btn:focus-visible,
.input:focus-visible,
.select:focus-visible,
.scene-btn:focus-visible,
.collapse-toggle:focus-visible,
.toggle:focus-visible {
  outline: 2px solid rgba(249,115,22,0.8);
  outline-offset: 2px;
}
@media (hover: none) {
  .btn:active,
  .scene-btn:active,
  .collapse-toggle:active {
    transform: scale(0.98);
  }
}
@media (hover: hover) {
  .btn:hover,
  .scene-btn:hover,
  .collapse-toggle:hover {
    transform: translateY(-1px);
    box-shadow: 0 10px 22px rgba(0,0,0,0.35);
  }
  .card:hover {
    transform: translateY(-2px);
    box-shadow:
      0 22px 46px rgba(15,23,42,0.85),
      inset 0 0 30px rgba(15,23,42,0.7);
  }
}
@media (max-width: 960px) {
  body {
    padding: 14px;
    align-items: stretch;
  }
  .app {
    max-width: 100%;
    height: 100%;
    border-radius: 18px;
    padding: 18px 18px 92px;
  }
  .header-top {
    flex-direction: column;
    align-items: flex-start;
  }
  .header-actions {
    width: 100%;
  }
  .shortcut-wrap {
    display: none;
  }
  .connection {
    width: 100%;
    flex-wrap: wrap;
  }
  .grid {
    grid-template-columns: 1fr;
  }
  .status-meta {
    grid-template-columns: repeat(2, minmax(0,1fr));
  }
  .field-row {
    flex-direction: column;
    align-items: flex-start;
  }
  .field-right {
    width: 100%;
    justify-content: space-between;
  }
  .slider {
    width: 100%;
  }
  .collapse-toggle {
    display: inline-flex;
  }
  .scene-grid {
    grid-template-columns: 1fr;
  }
  .wifi-label {
    width: 100%;
  }
  .wifi-input {
    width: 100%;
  }
}
@media (max-width: 720px) {
  body {
    font-size: 15px;
  }
  .title-text-main {
    font-size: 18px;
  }
  .title-text-sub {
    font-size: 12px;
  }
  .status-line {
    font-size: 14px;
  }
  .value {
    font-size: 13px;
  }
}
@media (max-width: 520px) {
  .status-meta {
    grid-template-columns: 1fr;
  }
}
@media (min-width: 961px) {
  .btn {
    padding: 8px 14px;
    font-size: 13px;
    min-height: 36px;
  }
  .input {
    padding: 8px 12px;
    font-size: 13px;
    min-height: 36px;
  }
  .select {
    padding: 6px 10px;
    font-size: 12px;
    min-height: 34px;
  }
  .scene-btn {
    padding: 6px 10px;
    font-size: 11px;
    min-height: 32px;
  }
  .color-input {
    width: 40px;
    height: 22px;
    padding: 0;
  }
  .toggle {
    width: 34px;
    height: 18px;
    padding: 2px;
  }
  .toggle-circle {
    width: 13px;
    height: 13px;
  }
  .toggle[data-on="true"] .toggle-circle {
    transform: translateX(14px);
  }
  .collapse-toggle {
    display: none;
  }
}
</style>
</head>
<body>
<div class="app">
  <div class="app-header">
    <div class="header-top">
      <div class="title-block">
        <div class="logo-dot">
          <div class="logo-inner"></div>
        </div>
        <div>
          <div class="title-text-main">Figure Box Studio</div>
          <div class="title-text-sub">手办盒灯光控制面板</div>
        </div>
      </div>
      <div class="header-actions">
        <div class="connection">
          <span class="status-pill" id="device-status-pill">
            <span class="status-dot offline" id="device-status-dot"></span>
            <span id="device-status-text">未连接</span>
          </span>
          <button class="btn btn-ghost" id="btn-refresh-status">刷新状态</button>
        </div>
        <div class="shortcut-wrap">
          <button class="btn btn-ghost" id="btn-shortcuts" aria-expanded="false">快捷键</button>
          <div class="shortcut-panel" id="shortcut-panel">
            <div class="shortcut-item"><span>刷新状态</span><span class="shortcut-key">Alt+R</span></div>
            <div class="shortcut-item"><span>电源开关</span><span class="shortcut-key">Alt+P</span></div>
            <div class="shortcut-item"><span>应用场景 1</span><span class="shortcut-key">Alt+1</span></div>
            <div class="shortcut-item"><span>应用场景 2</span><span class="shortcut-key">Alt+2</span></div>
          </div>
        </div>
      </div>
    </div>
  </div>
  <div class="grid">
    <div class="card" id="section-status">
      <div class="card-inner">
        <div class="card-header">
          <div class="card-title">状态总览</div>
          <div class="card-header-actions">
            <div class="card-tag" id="status-fx-tag">效果: -</div>
            <button class="collapse-toggle" data-target="card-status" aria-expanded="true">收起</button>
          </div>
        </div>
        <div class="card-body" id="card-status">
          <div class="status-main">
            <div class="preview-orb">
              <div class="preview-orb-color" id="status-color"></div>
              <div class="preview-orb-inner"></div>
            </div>
            <div class="status-text">
              <div class="status-line">
                <span class="status-label">双色温</span>
                <span class="status-value" id="status-cct">-</span>
              </div>
              <div class="status-line">
                <span class="status-label">彩灯</span>
                <span class="status-value" id="status-rgb">-</span>
              </div>
              <div class="status-line">
                <span class="status-label">场景</span>
                <span class="status-value" id="status-scene">-</span>
              </div>
            </div>
          </div>
          <div class="status-meta">
            <div class="chip">
              <span>色温</span>
              <span id="status-ct">-</span>
            </div>
            <div class="chip">
              <span>双色温亮度</span>
              <span id="status-b">-</span>
            </div>
            <div class="chip">
              <span>彩灯亮度</span>
              <span id="status-wb">-</span>
            </div>
            <div class="chip">
              <span>像素数量</span>
              <span id="status-n">-</span>
            </div>
            <div class="chip">
              <span>电源</span>
              <span id="status-power">-</span>
            </div>
            <div class="chip">
              <span>IP</span>
              <span id="status-sta-ip">-</span>
            </div>
            <div class="chip">
              <span>AP</span>
              <span id="status-ap-info">-</span>
            </div>
            <div class="chip">
              <span>版本号</span>
              <span>v1.8</span>
            </div>
          </div>
          <div class="tag-row">
            <div>实时状态</div>
            <div class="tag-row-right">
              <span id="status-mode-pill" class="pill">待机</span>
              <button class="btn btn-ghost" id="btn-power">关机</button>
              <span id="status-error" class="error-text" aria-live="polite"></span>
            </div>
          </div>
        </div>
      </div>
    </div>
    <div class="card" id="section-light">
      <div class="card-inner">
        <div class="card-header">
          <div class="card-title">灯光控制</div>
          <div class="card-header-actions">
            <div class="card-tag">Light</div>
            <button class="collapse-toggle" data-target="card-light" aria-expanded="true">收起</button>
          </div>
        </div>
        <div class="card-body" id="card-light">
          <div class="controls-column">
          <div class="section">
            <div class="section-title">
              <span>补光灯</span>
            </div>
            <div class="field-row">
              <div>
                <div class="field-label">开关</div>
              </div>
              <div class="field-right">
                <div class="toggle" id="toggle-cct" role="switch" tabindex="0" aria-checked="false">
                  <input type="checkbox" id="cct-on" class="toggle-input">
                  <div class="toggle-circle"></div>
                </div>
              </div>
            </div>
            <div class="field-row">
              <div class="field-label">色温</div>
              <div class="field-sub field-sub-right">暖黄 2700K — 冷白 6500K</div>
            </div>
            <div class="field-row field-row-gap">
              <div></div>
              <div class="field-right">
                <input type="range" min="2700" max="6500" step="100" id="ct" class="slider">
                <div class="value" id="ct-value">- K</div>
              </div>
            </div>
            <div class="field-row field-row-gap">
              <div>
                <div class="field-label">双色温亮度</div>
              </div>
              <div class="field-right">
                <input type="range" min="0" max="100" step="1" id="b" class="slider">
                <div class="value" id="b-value">- %</div>
              </div>
            </div>
            <div class="field-row">
              <div>
                <div class="field-label">快速选择</div>
              </div>
              <div class="field-right">
                <select id="ct-preset" class="select">
                  <option value="">请选择色温</option>
                  <option value="2700">暖色 2700K</option>
                  <option value="4600">中性色 4600K</option>
                  <option value="6500">冷色 6500K</option>
                </select>
              </div>
            </div>
          </div>
          <div class="section">
            <div class="section-title">
              <span>氛围效果</span>
            </div>
            <div class="field-row">
              <div>
                <div class="field-label">氛围灯开关</div>
              </div>
              <div class="field-right">
                <div class="toggle" id="toggle-rgb" role="switch" tabindex="0" aria-checked="false">
                  <input type="checkbox" id="ws-on" class="toggle-input">
                  <div class="toggle-circle"></div>
                </div>
              </div>
            </div>
            <div class="field-row">
              <div>
                <div class="field-label">氛围灯亮度</div>
              </div>
              <div class="field-right">
                <input type="range" min="0" max="255" step="1" id="wb" class="slider">
                <div class="value" id="wb-value">-</div>
              </div>
            </div>
            <div class="field-row">
              <div>
                <div class="field-label">效果模式</div>
                <div class="field-sub">动画模式切换</div>
              </div>
              <div class="field-right">
                <select id="effect-name" class="select">
                  <option value="off">关闭效果</option>
                  <option value="rainbow">彩虹</option>
                  <option value="breathe">单色呼吸</option>
                  <option value="breathe_color">多色呼吸</option>
                  <option value="solid">常亮</option>
                  <option value="wipe">跑马光</option>
                  <option value="manual">手动像素</option>
                </select>
              </div>
            </div>
            <div class="field-row" id="main-color-row">
              <div>
                <div class="field-label">主色</div>
                <div class="field-sub">用于呼吸、常亮等效果</div>
              </div>
              <div class="field-right">
                <input type="color" id="color" class="color-input" value="#ffa500">
                <div class="value" id="color-value">#FFA500</div>
              </div>
            </div>
            <div class="field-row" id="manual-pixels-section">
              <div>
                <div class="field-label">单颗像素控制</div>
              </div>
              <div class="field-right">
                <button class="btn btn-ghost" id="btn-manual-pixels">打开像素控制页面</button>
              </div>
            </div>
          </div>
          <div class="section" id="dynamic-params">
            <div class="section-title">
              <span>呼吸与循环节奏</span>
            </div>
            <div class="field-row" id="row-breathe-speed">
              <div>
                <div class="field-label">呼吸速度</div>
                <div class="field-sub">半周期时长 0.2–10 s</div>
              </div>
              <div class="field-right">
                <input type="range" min="0.2" max="10" step="0.1" id="bs" class="slider">
                <div class="value" id="bs-value">- s</div>
              </div>
            </div>
            <div class="field-row" id="row-solid-cycle">
              <div>
                <div class="field-label">循环颜色</div>
                <div class="field-sub">仅 solid 模式下生效</div>
              </div>
              <div class="field-right">
                <div class="toggle" id="toggle-sc" role="switch" tabindex="0" aria-checked="false">
                  <input type="checkbox" id="sc-on" class="toggle-input">
                  <div class="toggle-circle"></div>
                </div>
              </div>
            </div>
            <div class="field-row" id="row-solid-cycle-period">
              <div>
                <div class="field-label">循环周期</div>
                <div class="field-sub">0.2–20 s</div>
              </div>
              <div class="field-right">
                <input type="range" min="0.2" max="20" step="0.1" id="scs" class="slider">
                <div class="value" id="scs-value">- s</div>
              </div>
            </div>
          </div>
          <div class="section">
            <div class="section-title">
              <span>场景记忆</span>
            </div>
            <div class="scene-actions">
              <select id="scene-slot" class="select">
                <option value="1">场景 1</option>
                <option value="2">场景 2</option>
                <option value="3">场景 3</option>
                <option value="4">场景 4</option>
              </select>
              <button class="btn btn-primary" id="scene-apply">应用场景</button>
              <button class="btn btn-ghost" id="scene-save">保存场景</button>
            </div>
          </div>
        </div>
        </div>
      </div>
    </div>
    <div class="card" id="section-scenes">
      <div class="card-inner">
        <div class="card-header">
          <div class="card-title">预设与网络</div>
          <div class="card-header-actions">
            <div class="card-tag">Scenes · WiFi</div>
            <button class="collapse-toggle" data-target="card-scenes" data-collapsed="true" aria-expanded="true">收起</button>
          </div>
        </div>
        <div class="card-body" id="card-scenes">
          <div class="section" id="section-screen">
            <div class="section-title">
              <span>息屏显示</span>
            </div>
            <div class="field-row">
              <div>
                <div class="field-label">显示模式</div>
                <div class="field-sub">状态 / 时间 / 轮换 / 息屏</div>
              </div>
              <div class="field-right">
                <select id="screen-saver-mode" class="select">
                  <option value="0">状态</option>
                  <option value="1">时间</option>
                  <option value="2">轮换</option>
                  <option value="3">息屏</option>
                </select>
              </div>
            </div>
          </div>
          <div class="section" id="section-wifi">
            <div class="section-title">
              <span>WiFi 配网</span>
              <span>切换家庭网络</span>
            </div>
            <div class="wifi-row">
              <div class="wifi-label">列表</div>
              <select id="wifi-ssid-list" class="input wifi-input wifi-select"></select>
              <button class="btn btn-ghost" id="wifi-scan">扫描</button>
            </div>
            <div class="wifi-row">
              <div class="wifi-label">密码</div>
              <input id="wifi-pwd" class="input wifi-input" placeholder="密码" type="password">
            </div>
            <div class="wifi-row">
              <button class="btn btn-primary" id="wifi-connect">开始连接</button>
            </div>
            <details class="advanced" id="wifi-advanced">
              <summary>更多操作</summary>
              <div class="wifi-row">
                <button class="btn btn-ghost" id="wifi-forget">清除配置</button>
              </div>
            </details>
            <div class="wifi-status" id="wifi-status">
              <span>WiFi 状态未知</span>
            </div>
          </div>
          <div class="section" id="section-ota">
            <div class="section-title">
              <span>固件升级</span>
            </div>
            <form method="POST" action="/update" enctype="multipart/form-data">
              <div class="wifi-row">
                <div class="wifi-label">固件</div>
                <input type="file" name="update" class="input wifi-input" accept=".bin">
              </div>
              <div class="wifi-row">
                <button type="submit" class="btn btn-primary">开始升级</button>
              </div>
            </form>
          </div>
        </div>
      </div>
    </div>
  </div>
  <div class="footer-row">
    <div class="footer-right">
    </div>
  </div>
</div>
<script>
var deviceIp = window.location.hostname || "";
var eventSource = null;
var realtimeEnabled = false;
var lightSyncTimer = null;
var effectSyncTimer = null;
var lastLightQuery = "";
var lightSyncIntervalMs = 16;
var lightSyncActiveMs = 16;
var lightSyncIdleMs = 120;
var lightSyncIdleDelayMs = 2000;
var lightSyncIdleTimer = null;
var lightSyncInFlight = false;
var pendingLightQuery = "";
var lastLightSendMs = 0;
var softOff = false;
function $(id) {
  return document.getElementById(id);
}
function buildBaseUrl(path) {
  if (!deviceIp) {
    return path;
  }
  return "http://" + deviceIp + path;
}
function setDeviceOnline(online) {
  var dot = $("device-status-dot");
  var text = $("device-status-text");
  if (!dot || !text) {
    return;
  }
  dot.classList.remove("online");
  dot.classList.remove("offline");
  if (online) {
    dot.classList.add("online");
    text.textContent = "已连接";
  } else {
    dot.classList.add("offline");
    text.textContent = "未连接";
  }
}
function updateTimestamp() {
  var t = $("timestamp");
  if (!t) {
    return;
  }
  var d = new Date();
  var h = String(d.getHours()).padStart(2, "0");
  var m = String(d.getMinutes()).padStart(2, "0");
  var s = String(d.getSeconds()).padStart(2, "0");
  t.textContent = h + ":" + m + ":" + s;
}
function hexToRgb(hex) {
  if (!hex || hex.length !== 7) {
    return null;
  }
  var r = parseInt(hex.slice(1, 3), 16);
  var g = parseInt(hex.slice(3, 5), 16);
  var b = parseInt(hex.slice(5, 7), 16);
  if (isNaN(r) || isNaN(g) || isNaN(b)) {
    return null;
  }
  return { r: r, g: g, b: b };
}
function setToggle(toggleId, checked) {
  var toggle = $(toggleId);
  if (!toggle) {
    return;
  }
  var input = toggle.querySelector("input");
  toggle.dataset.on = checked ? "true" : "false";
  toggle.setAttribute("aria-checked", checked ? "true" : "false");
  if (input) {
    input.checked = checked;
  }
}
function getToggle(toggleId) {
  var toggle = $(toggleId);
  if (!toggle) {
    return false;
  }
  var input = toggle.querySelector("input");
  return input && input.checked;
}
function setDisabled(id, disabled) {
  var el = $(id);
  if (!el) {
    return;
  }
  el.disabled = !!disabled;
}
function setToggleDisabled(toggleId, disabled) {
  var toggle = $(toggleId);
  if (!toggle) {
    return;
  }
  var input = toggle.querySelector("input");
  if (input) {
    input.disabled = !!disabled;
  }
  toggle.setAttribute("aria-disabled", disabled ? "true" : "false");
  toggle.tabIndex = disabled ? -1 : 0;
}
function updateControlsDisabled() {
  var disabled = !!softOff;
  setDisabled("ct", disabled);
  setDisabled("b", disabled);
  setDisabled("wb", disabled);
  setDisabled("bs", disabled);
  setDisabled("scs", disabled);
  setDisabled("color", disabled);
  setToggleDisabled("toggle-cct", disabled);
  setToggleDisabled("toggle-rgb", disabled);
  setToggleDisabled("toggle-sc", disabled);
  var effect = $("effect-name");
  if (effect) {
    effect.disabled = disabled;
  }
  setDisabled("ct-preset", disabled);
  setDisabled("scene-slot", disabled);
  setDisabled("scene-apply", disabled);
  setDisabled("scene-save", disabled);
}
function updateDynamicParamsVisibility() {
  var fxSelect = $("effect-name");
  if (!fxSelect) {
    return;
  }
  var fx = fxSelect.value;
  var wrap = $("dynamic-params");
  var show = (fx === "breathe" || fx === "breathe_color" || fx === "solid");
  if (wrap) {
    wrap.style.display = show ? "" : "none";
  }
  var breatheRow = $("row-breathe-speed");
  if (breatheRow) {
    breatheRow.style.display = (fx === "breathe" || fx === "breathe_color") ? "" : "none";
  }
  var cycleRow = $("row-solid-cycle");
  if (cycleRow) {
    cycleRow.style.display = (fx === "solid") ? "" : "none";
  }
  var cyclePeriodRow = $("row-solid-cycle-period");
  if (cyclePeriodRow) {
    var cycleOn = getToggle("toggle-sc");
    cyclePeriodRow.style.display = (fx === "solid" && cycleOn) ? "" : "none";
  }
  var mainColorRow = $("main-color-row");
  if (mainColorRow) {
    mainColorRow.style.display = (fx === "breathe" || fx === "solid") ? "" : "none";
  }
  var manualSection = $("manual-pixels-section");
  if (manualSection) {
    manualSection.style.display = (fx === "manual") ? "" : "none";
  }
}
function bindSliderTouchGuard() {
  var sliders = document.querySelectorAll('input[type="range"]');
  sliders.forEach(function(slider) {
    var startX = 0;
    var startY = 0;
    var lock = "";
    var pointerId = null;
    var active = false;
    var threshold = 6;
    var reset = function() {
      active = false;
      lock = "";
      pointerId = null;
    };
    var onStart = function(x, y, pid) {
      active = true;
      startX = x;
      startY = y;
      lock = "";
      pointerId = typeof pid === "number" ? pid : null;
    };
    var onMove = function(x, y, e) {
      if (!active) {
        return;
      }
      if (pointerId !== null && typeof e.pointerId === "number" && e.pointerId !== pointerId) {
        return;
      }
      var dx = x - startX;
      var dy = y - startY;
      if (!lock) {
        if (Math.abs(dx) >= threshold || Math.abs(dy) >= threshold) {
          lock = Math.abs(dx) >= Math.abs(dy) ? "x" : "y";
        } else {
          return;
        }
      }
      if (lock === "x") {
        e.stopPropagation();
      }
    };
    slider.addEventListener("touchstart", function(e) {
      var t = e.touches && e.touches[0];
      if (!t) {
        return;
      }
      onStart(t.clientX, t.clientY);
    }, { passive: true });
    slider.addEventListener("touchmove", function(e) {
      var t = e.touches && e.touches[0];
      if (!t) {
        return;
      }
      onMove(t.clientX, t.clientY, e);
    }, { passive: false });
    slider.addEventListener("touchend", function() {
      reset();
    }, { passive: true });
    slider.addEventListener("touchcancel", function() {
      reset();
    }, { passive: true });
    slider.addEventListener("pointerdown", function(e) {
      if (e.pointerType === "mouse") {
        return;
      }
      onStart(e.clientX, e.clientY, e.pointerId);
      if (slider.setPointerCapture && typeof e.pointerId === "number") {
        slider.setPointerCapture(e.pointerId);
      }
    });
    slider.addEventListener("pointermove", function(e) {
      onMove(e.clientX, e.clientY, e);
    });
    var pointerEnd = function(e) {
      if (pointerId !== null && typeof e.pointerId === "number" && e.pointerId !== pointerId) {
        return;
      }
      if (slider.releasePointerCapture && typeof e.pointerId === "number") {
        slider.releasePointerCapture(e.pointerId);
      }
      reset();
    };
    slider.addEventListener("pointerup", pointerEnd);
    slider.addEventListener("pointercancel", pointerEnd);
  });
}
function renderStatus(state) {
  if (!state) {
    return;
  }
  var cctText = state.cctOn ? "开启" : "关闭";
  var rgbText = state.wsOn ? "开启" : "关闭";
  if (typeof state.b === "number") {
    cctText += " · " + state.b + "%";
  }
  if (typeof state.wb === "number") {
    rgbText += " · " + state.wb;
  }
  $("status-cct").textContent = cctText;
  $("status-rgb").textContent = rgbText;
  $("status-scene").textContent = state.fx || "-";
  $("status-ct").textContent = state.ct ? state.ct + " K" : "-";
  $("status-b").textContent = typeof state.b === "number" ? state.b + "%" : "-";
  $("status-wb").textContent = typeof state.wb === "number" ? state.wb : "-";
  $("status-n").textContent = typeof state.n === "number" ? state.n : "-";
  $("status-sta-ip").textContent = state.staIp || "-";
  $("status-ap-info").textContent = state.apSsid && state.apIp ? state.apSsid + " · " + state.apIp : "-";
  var fx = state.fx || "-";
  $("status-fx-tag").textContent = "效果: " + fx;
  $("status-mode-pill").textContent = state.provisioning ? "配网模式" : "正常模式";
  softOff = !!state.softOff;
  updateControlsDisabled();
  $("status-power").textContent = softOff ? "已关机" : "运行中";
  var powerBtn = $("btn-power");
  if (powerBtn) {
    powerBtn.textContent = softOff ? "开机" : "关机";
  }
  var hex = state.hex || "#000000";
  var preview = $("status-color");
  if (preview) {
    preview.style.background = hex;
  }
  var colorInput = $("color");
  if (colorInput && hex.charAt(0) === "#" && hex.length === 7) {
    colorInput.value = hex;
  }
  var colorLabel = $("color-value");
  if (colorLabel) {
    colorLabel.textContent = hex.toUpperCase();
  }
  updateDynamicParamsVisibility();
  updateTimestamp();
  setDeviceOnline(true);
}
function syncControlsFromState(state) {
  if (!state) {
    return;
  }
  if (typeof state.ct === "number") {
    $("ct").value = state.ct;
    $("ct-value").textContent = state.ct + " K";
  }
  if (typeof state.b === "number") {
    $("b").value = state.b;
    $("b-value").textContent = state.b + " %";
  }
  if (typeof state.wb === "number") {
    $("wb").value = state.wb;
    $("wb-value").textContent = state.wb;
  }
  setToggle("toggle-cct", !!state.cctOn);
  setToggle("toggle-rgb", !!state.wsOn);
  if (typeof state.bs === "number") {
    $("bs").value = state.bs;
    $("bs-value").textContent = state.bs.toFixed(1) + " s";
  }
  if (typeof state.scs === "number") {
    $("scs").value = state.scs;
    $("scs-value").textContent = state.scs.toFixed(1) + " s";
  }
  setToggle("toggle-sc", !!state.scOn);
  var select = $("effect-name");
  if (select && state.fx) {
    select.value = state.fx;
  }
  updateDynamicParamsVisibility();
  var screenSelect = $("screen-saver-mode");
  if (screenSelect && typeof state.ssm === "number") {
    screenSelect.value = String(state.ssm);
  }
}
function applyStateFromBackend(state, syncControls) {
  if (!state) {
    return;
  }
  renderStatus(state);
  if (syncControls) {
    syncControlsFromState(state);
  }
}
function handleStatusError(err) {
  var el = $("status-error");
  if (!el) {
    return;
  }
  el.textContent = err || "";
  if (err) {
    setDeviceOnline(false);
  }
}
function fetchStatus() {
  if (!deviceIp) {
    handleStatusError("请先设置设备 IP");
    return;
  }
  var url = buildBaseUrl("/api/status");
  if (!url) {
    return;
  }
  handleStatusError("");
  fetch(url, {
    method: "GET",
    cache: "no-store"
  }).then(function(res) {
    if (!res.ok) {
      throw new Error("状态请求失败 " + res.status);
    }
    return res.json();
  }).then(function(json) {
    applyStateFromBackend(json, true);
  }).catch(function(e) {
    handleStatusError(e.message);
  });
}
function applyLight() {
  if (!deviceIp) {
    handleStatusError("请先设置设备 IP");
    return;
  }
  if (softOff) {
    var msg = "设备已关机，请先在上方点击“开机”";
    alert(msg);
    handleStatusError(msg);
    return;
  }
  var params = new URLSearchParams();
  var ct = parseInt($("ct").value, 10);
  if (!isNaN(ct)) {
    params.append("ct", String(ct));
  }
  var b = parseInt($("b").value, 10);
  if (!isNaN(b)) {
    params.append("b", String(b));
  }
  var wb = parseInt($("wb").value, 10);
  if (!isNaN(wb)) {
    params.append("wb", String(wb));
  }
  params.append("wsOn", getToggle("toggle-rgb") ? "true" : "false");
  params.append("cctOn", getToggle("toggle-cct") ? "true" : "false");
  var color = $("color").value;
  var rgb = hexToRgb(color);
  if (rgb) {
    params.append("cr", String(rgb.r));
    params.append("cg", String(rgb.g));
    params.append("cb", String(rgb.b));
  }
  var bs = parseFloat($("bs").value);
  if (!isNaN(bs)) {
    params.append("bs", String(bs));
  }
  params.append("scOn", getToggle("toggle-sc") ? "true" : "false");
  var scs = parseFloat($("scs").value);
  if (!isNaN(scs)) {
    params.append("scs", String(scs));
  }
  var query = params.toString();
  if (!query || query === lastLightQuery) {
    return;
  }
  pendingLightQuery = query;
  if (lightSyncInFlight) {
    return;
  }
  sendLightQuery();
}
function sendLightQuery() {
  if (!pendingLightQuery || lightSyncInFlight) {
    return;
  }
  var query = pendingLightQuery;
  pendingLightQuery = "";
  lastLightQuery = query;
  lightSyncInFlight = true;
  lastLightSendMs = Date.now();
  var url = buildBaseUrl("/api/light") + "?" + query;
  fetch(url, {
    method: "PATCH",
    headers: { "Content-Type": "application/json" }
  }).then(function(res) {
    if (!res.ok) {
      throw new Error("灯光控制失败 " + res.status);
    }
    return res.json();
  }).then(function(json) {
    applyStateFromBackend(json, false);
  }).catch(function(e) {
    handleStatusError(e.message);
  }).finally(function() {
    lightSyncInFlight = false;
    if (!pendingLightQuery) {
      return;
    }
    var wait = lightSyncIntervalMs - (Date.now() - lastLightSendMs);
    if (wait < 0) {
      wait = 0;
    }
    if (lightSyncTimer) {
      clearTimeout(lightSyncTimer);
    }
    lightSyncTimer = setTimeout(function() {
      lightSyncTimer = null;
      sendLightQuery();
    }, wait);
  });
}
function markLightSyncActive() {
  lightSyncIntervalMs = lightSyncActiveMs;
  if (lightSyncIdleTimer) {
    clearTimeout(lightSyncIdleTimer);
  }
  lightSyncIdleTimer = setTimeout(function() {
    lightSyncIntervalMs = lightSyncIdleMs;
    lightSyncIdleTimer = null;
  }, lightSyncIdleDelayMs);
}
function scheduleLightSync() {
  if (!deviceIp) {
    return;
  }
  if (softOff) {
    var msg = "设备已关机，请先在上方点击“开机”";
    alert(msg);
    handleStatusError(msg);
    return;
  }
  markLightSyncActive();
  if (lightSyncTimer) {
    clearTimeout(lightSyncTimer);
  }
  lightSyncTimer = setTimeout(function() {
    lightSyncTimer = null;
    applyLight();
  }, lightSyncIntervalMs);
}
function applyCctPreset(kelvin) {
  var ctInput = $("ct");
  if (!ctInput) {
    return;
  }
  ctInput.value = String(kelvin);
  $("ct-value").textContent = String(kelvin) + " K";
  setToggle("toggle-cct", true);
  scheduleLightSync();
}
function applyEffect() {
  if (!deviceIp) {
    handleStatusError("请先设置设备 IP");
    return;
  }
  if (softOff) {
    var msg = "设备已关机，请先在上方点击“开机”";
    alert(msg);
    handleStatusError(msg);
    return;
  }
  var name = $("effect-name").value;
  var params = new URLSearchParams();
  params.append("name", name);
  var url = buildBaseUrl("/api/effect") + "?" + params.toString();
  fetch(url, {
    method: "PATCH"
  }).then(function(res) {
    if (!res.ok) {
      throw new Error("效果切换失败 " + res.status);
    }
    return res.json();
  }).then(function(json) {
    applyStateFromBackend(json, false);
  }).catch(function(e) {
    handleStatusError(e.message);
  });
}
function applyConfig() {
  if (!deviceIp) {
    handleStatusError("请先设置设备 IP");
    return;
  }
  var screenSelect = $("screen-saver-mode");
  if (!screenSelect) {
    return;
  }
  var params = new URLSearchParams();
  params.append("ssm", screenSelect.value);
  var url = buildBaseUrl("/api/config") + "?" + params.toString();
  fetch(url, {
    method: "PATCH"
  }).then(function(res) {
    if (!res.ok) {
      throw new Error("配置失败 " + res.status);
    }
    return res.json();
  }).then(function(json) {
    applyStateFromBackend(json, false);
  }).catch(function(e) {
    handleStatusError(e.message);
  });
}
function scheduleEffectSync() {
  if (!deviceIp) {
    return;
  }
  if (effectSyncTimer) {
    clearTimeout(effectSyncTimer);
  }
  effectSyncTimer = setTimeout(function() {
    effectSyncTimer = null;
    applyEffect();
  }, 200);
}
function applyPowerToggle() {
  if (!deviceIp) {
    handleStatusError("请先设置设备 IP");
    return;
  }
  var url = buildBaseUrl("/api/power") + "?state=toggle";
  fetch(url, {
    method: "POST"
  }).then(function(res) {
    if (!res.ok) {
      throw new Error("电源控制失败 " + res.status);
    }
    return res.json();
  }).then(function(json) {
    applyStateFromBackend(json, false);
  }).catch(function(e) {
    handleStatusError(e.message);
  });
}
function handleSceneSave(slot) {
  if (!deviceIp) {
    handleStatusError("请先设置设备 IP");
    return;
  }
  if (softOff) {
    var msg = "设备已关机，无法保存场景（请先开机）";
    alert(msg);
    handleStatusError(msg);
    return;
  }
  var url = buildBaseUrl("/api/scene/save") + "?slot=" + encodeURIComponent(String(slot));
  fetch(url, {
    method: "POST"
  }).then(function(res) {
    if (res.status === 400) {
      return res.json().then(function(j) {
        throw new Error(j.error || "保存失败");
      });
    }
    if (res.status === 500) {
      return res.json().then(function(j) {
        throw new Error(j.ok === false ? "场景存储失败" : "内部错误");
      });
    }
    if (!res.ok) {
      throw new Error("保存失败 " + res.status);
    }
    return res.json();
  }).then(function() {
    handleStatusError("");
  }).catch(function(e) {
    handleStatusError(e.message);
  });
}
function handleSceneApply(slot) {
  if (!deviceIp) {
    handleStatusError("请先设置设备 IP");
    return;
  }
  if (softOff) {
    var msg = "设备已关机，无法应用场景（请先开机）";
    alert(msg);
    handleStatusError(msg);
    return;
  }
  var url = buildBaseUrl("/api/scene/apply") + "?slot=" + encodeURIComponent(String(slot));
  fetch(url, {
    method: "POST"
  }).then(function(res) {
    if (!res.ok) {
      return res.json().then(function(j) {
        throw new Error(j.error || "应用失败 " + res.status);
      }).catch(function() {
        throw new Error("应用失败 " + res.status);
      });
    }
    return res.json();
  }).then(function(json) {
    applyStateFromBackend(json, false);
  }).catch(function(e) {
    handleStatusError(e.message);
  });
}
function updateWifiStatusView(w) {
  var el = $("wifi-status");
  if (!el) {
    return;
  }
  if (!w) {
    el.textContent = "WiFi 状态未知";
    return;
  }
  var parts = [];
  parts.push("配网模式 " + (w.provisioning ? "开启" : "关闭"));
  parts.push("连接中 " + (w.connecting ? "是" : "否"));
  parts.push("已连接 " + (w.connected ? "是" : "否"));
  if (w.failed) {
    parts.push("最近连接失败，代码 " + w.failCode);
  }
  if (w.staIp) {
    parts.push("STA IP " + w.staIp);
  }
  el.innerHTML = "";
  parts.forEach(function(p, idx) {
    var span = document.createElement("span");
    span.className = "wifi-badge" + (w.connected && idx === parts.length - 1 ? " highlight" : "");
    span.textContent = p;
    el.appendChild(span);
  });
}
function fetchWifiStatus() {
  if (!deviceIp) {
    return;
  }
  var url = buildBaseUrl("/api/wifi/status");
  fetch(url, {
    method: "GET",
    cache: "no-store"
  }).then(function(res) {
    if (!res.ok) {
      throw new Error("WiFi 状态错误 " + res.status);
    }
    return res.json();
  }).then(function(json) {
    updateWifiStatusView(json);
  }).catch(function() {
  });
}
function renderWifiScanList(list) {
  var sel = $("wifi-ssid-list");
  if (!sel) {
    return;
  }
  sel.innerHTML = "";
  var def = document.createElement("option");
  def.value = "";
  def.textContent = "选择网络";
  sel.appendChild(def);
  if (!list || !list.length) {
    return;
  }
  list.forEach(function(item) {
    if (!item || !item.ssid) {
      return;
    }
    var opt = document.createElement("option");
    opt.value = item.ssid;
    var rssi = (typeof item.rssi === "number") ? (item.rssi + "dBm") : "--";
    var sec = item.secure ? "加密" : "开放";
    opt.textContent = item.ssid + " (" + rssi + "·" + sec + ")";
    sel.appendChild(opt);
  });
}
function fetchWifiScan() {
  if (!deviceIp) {
    handleStatusError("请先设置设备 IP");
    return;
  }
  var btn = $("wifi-scan");
  if (btn) {
    btn.disabled = true;
    btn.textContent = "扫描中...";
  }
  var url = buildBaseUrl("/api/wifi/scan");
  fetch(url, {
    method: "GET",
    cache: "no-store"
  }).then(function(res) {
    if (!res.ok) {
      throw new Error("扫描失败 " + res.status);
    }
    return res.json();
  }).then(function(json) {
    renderWifiScanList(json.items || []);
    if (btn) {
      btn.disabled = false;
      btn.textContent = "扫描";
    }
  }).catch(function(e) {
    if (btn) {
      btn.disabled = false;
      btn.textContent = "扫描";
    }
    handleStatusError(e.message);
  });
}
function applyWifiConfig() {
  if (!deviceIp) {
    handleStatusError("请先设置设备 IP");
    return;
  }
  var list = $("wifi-ssid-list");
  var ssid = list ? list.value : "";
  var pwd = $("wifi-pwd").value;
  if (!ssid) {
    handleStatusError("SSID 不能为空");
    return;
  }
  var params = new URLSearchParams();
  params.append("ssid", ssid);
  if (pwd) {
    params.append("pwd", pwd);
  }
  var url = buildBaseUrl("/api/wifi/config") + "?" + params.toString();
  fetch(url, {
    method: "POST"
  }).then(function(res) {
    return res.json().then(function(j) {
      if (!res.ok || j.error) {
        throw new Error(j.error || "配网失败 " + res.status);
      }
      return j;
    });
  }).then(function() {
    handleStatusError("");
    fetchWifiStatus();
  }).catch(function(e) {
    handleStatusError(e.message);
  });
}
function applyWifiForget() {
  if (!deviceIp) {
    handleStatusError("请先设置设备 IP");
    return;
  }
  var url = buildBaseUrl("/api/wifi/forget");
  fetch(url, {
    method: "POST"
  }).then(function(res) {
    if (!res.ok) {
      throw new Error("清除配置失败 " + res.status);
    }
    return res.json();
  }).then(function() {
    handleStatusError("");
    fetchWifiStatus();
  }).catch(function(e) {
    handleStatusError(e.message);
  });
}
function pauseRealtimeUpdates() {
  if (eventSource) {
    eventSource.close();
    eventSource = null;
  }
}
function startRealtimeUpdates() {
  realtimeEnabled = true;
  if (eventSource) {
    eventSource.close();
    eventSource = null;
  }
  var mode = $("realtime-mode");
  try {
    var url = buildBaseUrl("/events");
    eventSource = new EventSource(url);
    eventSource.onmessage = function(e) {
      try {
        var state = JSON.parse(e.data);
        applyStateFromBackend(state, false);
      } catch (err) {
      }
    };
    eventSource.onerror = function() {
      pauseRealtimeUpdates();
      if (mode) {
        mode.textContent = "实时更新: 已关闭";
      }
    };
    if (mode) {
      mode.textContent = "实时更新: SSE";
    }
  } catch (e) {
    if (mode) {
      mode.textContent = "实时更新: 已关闭";
    }
  }
}
function bindShortcutPanel() {
  var btn = $("btn-shortcuts");
  var panel = $("shortcut-panel");
  if (!btn || !panel) {
    return;
  }
  var close = function() {
    panel.classList.remove("show");
    btn.setAttribute("aria-expanded", "false");
  };
  btn.addEventListener("click", function(e) {
    e.stopPropagation();
    var open = !panel.classList.contains("show");
    if (open) {
      panel.classList.add("show");
      btn.setAttribute("aria-expanded", "true");
    } else {
      close();
    }
  });
  document.addEventListener("click", function(e) {
    if (panel.classList.contains("show") && !panel.contains(e.target) && e.target !== btn) {
      close();
    }
  });
}
function bindCollapsibles() {
  var isMobile = window.matchMedia && window.matchMedia("(max-width: 960px)").matches;
  var applyState = function(btn, collapsed) {
    var targetId = btn.getAttribute("data-target");
    if (!targetId) {
      return;
    }
    var target = document.getElementById(targetId);
    if (!target) {
      return;
    }
    btn.setAttribute("aria-expanded", collapsed ? "false" : "true");
    target.style.display = collapsed ? "none" : "";
    btn.textContent = collapsed ? "展开" : "收起";
  };
  var toggles = document.querySelectorAll(".collapse-toggle");
  toggles.forEach(function(btn) {
    if (isMobile && btn.getAttribute("data-collapsed") === "true") {
      applyState(btn, true);
    }
    btn.addEventListener("click", function() {
      var expanded = btn.getAttribute("aria-expanded") === "true";
      applyState(btn, expanded);
    });
  });
}
function bindToggleKeys() {
  var toggleIds = ["toggle-cct", "toggle-rgb", "toggle-sc"];
  toggleIds.forEach(function(id) {
    var el = $(id);
    if (!el) {
      return;
    }
    el.addEventListener("keydown", function(e) {
      if (e.key === " " || e.key === "Enter") {
        e.preventDefault();
        el.click();
      }
    });
  });
}
function bindGlobalShortcuts() {
  document.addEventListener("keydown", function(e) {
    if (!e.altKey) {
      return;
    }
    var target = e.target;
    if (target && target.tagName) {
      var tag = target.tagName;
      if (tag === "INPUT" || tag === "TEXTAREA" || tag === "SELECT") {
        return;
      }
    }
    var key = e.key.toLowerCase();
    if (key === "r") {
      e.preventDefault();
      fetchStatus();
      fetchWifiStatus();
      return;
    }
    if (key === "p") {
      e.preventDefault();
      applyPowerToggle();
      return;
    }
    if (key === "1" || key === "2" || key === "3" || key === "4") {
      e.preventDefault();
      handleSceneApply(key);
      return;
    }
  });
}
function scheduleDeferred(fn, delay) {
  if (window.requestIdleCallback) {
    window.requestIdleCallback(fn, { timeout: delay || 1200 });
  } else {
    setTimeout(fn, delay || 600);
  }
}
function bindEvents() {
  bindShortcutPanel();
  bindCollapsibles();
  bindToggleKeys();
  bindGlobalShortcuts();
  bindSliderTouchGuard();
  var refreshBtn = $("btn-refresh-status");
  if (refreshBtn) {
    refreshBtn.addEventListener("click", function() {
      fetchStatus();
      fetchWifiStatus();
    });
  }
  var powerBtn = $("btn-power");
  if (powerBtn) {
    powerBtn.addEventListener("click", function() {
      applyPowerToggle();
    });
  }
  var manualBtn = $("btn-manual-pixels");
  if (manualBtn) {
    manualBtn.addEventListener("click", function() {
      window.location.href = "/manual";
    });
  }
  var sceneSlot = $("scene-slot");
  var sceneApplyBtn = $("scene-apply");
  var sceneSaveBtn = $("scene-save");
  var getSceneSlot = function() {
    var v = sceneSlot ? parseInt(sceneSlot.value, 10) : NaN;
    return isNaN(v) ? null : v;
  };
  if (sceneApplyBtn) {
    sceneApplyBtn.addEventListener("click", function() {
      var slot = getSceneSlot();
      if (!slot) {
        handleStatusError("请选择场景槽位");
        return;
      }
      handleSceneApply(slot);
    });
  }
  if (sceneSaveBtn) {
    sceneSaveBtn.addEventListener("click", function() {
      var slot = getSceneSlot();
      if (!slot) {
        handleStatusError("请选择场景槽位");
        return;
      }
      handleSceneSave(slot);
    });
  }
  var wifiScanBtn = $("wifi-scan");
  if (wifiScanBtn) {
    wifiScanBtn.addEventListener("click", function() {
      fetchWifiScan();
    });
  }
  var wifiList = $("wifi-ssid-list");
  if (wifiList) {
    wifiList.addEventListener("change", function() {
      var v = wifiList.value;
    });
  }
  $("wifi-connect").addEventListener("click", function() {
    applyWifiConfig();
  });
  $("wifi-forget").addEventListener("click", function() {
    applyWifiForget();
  });
  $("ct").addEventListener("input", function() {
    $("ct-value").textContent = $("ct").value + " K";
    scheduleLightSync();
  });
  $("ct").addEventListener("change", function() {
    scheduleLightSync();
  });
  $("b").addEventListener("input", function() {
    $("b-value").textContent = $("b").value + " %";
    scheduleLightSync();
  });
  $("b").addEventListener("change", function() {
    scheduleLightSync();
  });
  $("wb").addEventListener("input", function() {
    $("wb-value").textContent = $("wb").value;
    scheduleLightSync();
  });
  $("wb").addEventListener("change", function() {
    scheduleLightSync();
  });
  $("bs").addEventListener("input", function() {
    $("bs-value").textContent = parseFloat($("bs").value).toFixed(1) + " s";
    scheduleLightSync();
  });
  $("bs").addEventListener("change", function() {
    scheduleLightSync();
  });
  $("scs").addEventListener("input", function() {
    $("scs-value").textContent = parseFloat($("scs").value).toFixed(1) + " s";
    scheduleLightSync();
  });
  $("scs").addEventListener("change", function() {
    scheduleLightSync();
  });
  $("color").addEventListener("input", function() {
    var hex = $("color").value.toUpperCase();
    $("color-value").textContent = hex;
    var preview = $("status-color");
    if (preview) {
      preview.style.background = hex;
    }
    scheduleLightSync();
  });
  $("color").addEventListener("change", function() {
    scheduleLightSync();
  });
  $("toggle-cct").addEventListener("click", function() {
    if (softOff) {
      var msg = "设备已关机，请先在上方点击“开机”";
      alert(msg);
      handleStatusError(msg);
      return;
    }
    setToggle("toggle-cct", !getToggle("toggle-cct"));
    scheduleLightSync();
  });
  var ctPreset = $("ct-preset");
  if (ctPreset) {
    ctPreset.addEventListener("change", function() {
      var v = parseInt(ctPreset.value, 10);
      if (!isNaN(v)) {
        applyCctPreset(v);
      }
      ctPreset.value = "";
    });
  }
  $("toggle-rgb").addEventListener("click", function() {
    if (softOff) {
      var msg = "设备已关机，请先在上方点击“开机”";
      alert(msg);
      handleStatusError(msg);
      return;
    }
    setToggle("toggle-rgb", !getToggle("toggle-rgb"));
    scheduleLightSync();
  });
  $("toggle-sc").addEventListener("click", function() {
    if (softOff) {
      var msg = "设备已关机，请先在上方点击“开机”";
      alert(msg);
      handleStatusError(msg);
      return;
    }
    setToggle("toggle-sc", !getToggle("toggle-sc"));
    updateDynamicParamsVisibility();
    scheduleLightSync();
  });
  $("effect-name").addEventListener("change", function() {
    updateDynamicParamsVisibility();
    scheduleEffectSync();
  });
  var screenSelect = $("screen-saver-mode");
  if (screenSelect) {
    screenSelect.addEventListener("change", function() {
      applyConfig();
    });
  }
}
document.addEventListener("DOMContentLoaded", function() {
  bindEvents();
  updateDynamicParamsVisibility();
  if (deviceIp) {
    handleStatusError("");
    fetchStatus();
    fetchWifiStatus();
    scheduleDeferred(function() {
      fetchWifiScan();
      startRealtimeUpdates();
    }, 1200);
  }
  updateTimestamp();
  document.addEventListener("visibilitychange", function() {
    if (document.hidden) {
      pauseRealtimeUpdates();
    } else if (realtimeEnabled && deviceIp) {
      startRealtimeUpdates();
    }
  });
});
</script>
</body>
</html>)HTML";

WebRouter::WebRouter(HalLed& led, Effects& effects)
    : _led(led), _effects(effects), _server(80) {}

bool WebRouter::syncTimeOnce(uint32_t timeoutMs) {
  if (_timeSynced) return true;
  configTime(8 * 3600, 0, "ntp.aliyun.com", "pool.ntp.org", "time.nist.gov");
  struct tm t;
  uint32_t start = millis();
  while ((millis() - start) < timeoutMs) {
    if (getLocalTime(&t, 200)) {
      if ((t.tm_year + 1900) >= 2022) {
        _timeSynced = true;
        return true;
      }
    }
    delay(50);
  }
  return false;
}

void WebRouter::startAp(const char* apPwd) {
  WiFi.mode(WIFI_AP_STA);
  if (apPwd && apPwd[0]) WiFi.softAP(_apSsid.c_str(), apPwd);
  else WiFi.softAP(_apSsid.c_str());
  _provisioning = true;
  StateManager::instance().setStaConnecting(false);
  if (!_dnsRunning) {
    _dns.start(53, "*", WiFi.softAPIP());
    _dnsRunning = true;
  }
}

void WebRouter::begin(const char* apNameBase, const char* apPwd) {
  _prefs.begin("wifi", false);
  String ssid = _prefs.getString("ssid", "");
  String pwd = _prefs.getString("pwd", "");

  uint64_t mac = ESP.getEfuseMac();
  char suf[5];
  snprintf(suf, sizeof(suf), "%04X", (uint16_t)(mac & 0xFFFF));
  _apSsid = String(apNameBase) + "_" + String(suf);

  if (ssid.length() > 0) {
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid.c_str(), pwd.c_str());
    _staConnecting = true;
    _staStartMs = millis();
    _provisioning = false;
    StateManager::instance().setWifiProvisioning(false);
    StateManager::instance().setStaConnecting(true);
    StateManager::instance().setStaFail(0, false);
  } else startAp(apPwd);

  _server.on("/", [this]() { handleRoot(); });
  _server.on("/manual", HTTP_ANY, [this]() { handleManual(); });
  _server.on("/state", HTTP_ANY, [this]() { handleApiStatus(); });
  _server.on("/pixels_state", HTTP_ANY, [this]() { handlePixelsState(); });
  _server.on("/px", HTTP_ANY, [this]() { handlePixel(); });
  _server.on("/effect", HTTP_ANY, [this]() { handleApiEffect(); });
  _server.on("/events", [this]() { handleEvents(); });
  _server.on("/api/status", HTTP_ANY, [this]() { handleApiStatus(); });
  _server.on("/api/light", HTTP_ANY, [this]() { handleApiLight(); });
  _server.on("/api/effect", HTTP_ANY, [this]() { handleApiEffect(); });
  _server.on("/api/power", HTTP_ANY, [this]() { handleApiPower(); });
  _server.on("/api/config", HTTP_ANY, [this]() { handleApiConfig(); });
  _server.on("/api/scene/save", HTTP_ANY, [this]() { handleApiSceneSave(); });
  _server.on("/api/scene/apply", HTTP_ANY, [this]() { handleApiSceneApply(); });
  _server.on("/api/wifi/status", HTTP_ANY, [this]() { handleApiWifiStatus(); });
  _server.on("/api/wifi/scan", HTTP_ANY, [this]() { handleApiWifiScan(); });
  _server.on("/api/wifi/config", HTTP_ANY, [this]() { handleApiWifiConfig(); });
  _server.on("/api/wifi/forget", HTTP_ANY, [this]() { handleApiWifiForget(); });
  _server.onNotFound([this]() { _server.send(404, "text/plain", "Not found"); });
  _otaServer.setup(&_server);
  _server.begin();

  Preferences sp;
  if (sp.begin("scene", true)) {
    uint8_t last = sp.getUChar("last", 0);
    bool lastOk = (last >= 1 && last <= 4);
    if (lastOk) {
      char k[20];
      snprintf(k, sizeof(k), "s%u_ok", (unsigned)last);
      lastOk = sp.getBool(k, false);
    }
    sp.end();
    if (lastOk) applyScene(last);
  }
}

void WebRouter::handle() {
  _server.handleClient();
  if (_dnsRunning) _dns.processNextRequest();

  if (WiFi.status() == WL_CONNECTED) {
    if (!_mdnsStarted) {
      if (MDNS.begin("esp32c3-led")) {
        MDNS.addService("http", "tcp", 80);
        _mdnsStarted = true;
      }
    }
    if (_dnsRunning && !_provisioning) {
      _dns.stop();
      _dnsRunning = false;
    }
    char ip[24]; strncpy(ip, WiFi.localIP().toString().c_str(), sizeof(ip)-1); ip[sizeof(ip)-1]=0;
    StateManager::instance().setStaConnected(true, ip);
  } else {
    if (_mdnsStarted) {
      MDNS.end();
      _mdnsStarted = false;
    }
    StateManager::instance().setStaConnected(false, nullptr);
  }

  if (_staFinalizePending && (millis() >= _staFinalizeAtMs)) {
    _provisioning = false;
    StateManager::instance().setWifiProvisioning(false);
    if (_dnsRunning) {
      _dns.stop();
      _dnsRunning = false;
    }
    WiFi.softAPdisconnect(true);
    WiFi.mode(WIFI_STA);
    _staFinalizePending = false;
  }

  if (_timeSyncPending && (WiFi.status() == WL_CONNECTED)) {
    if (syncTimeOnce(1500)) _timeSyncPending = false;
  }

  if (_eventsConnected) {
    if (!_eventsClient.connected()) {
      _eventsClient.stop();
      _eventsConnected = false;
      _eventsLastJson = "";
    } else {
      uint32_t nowMs = millis();
      if ((nowMs - _eventsLastPingMs) >= 15000) {
        _eventsClient.print(": ping\n\n");
        _eventsLastPingMs = nowMs;
      }
      if ((nowMs - _eventsLastSendMs) >= 250) {
        String j = buildStateJson();
        if (j != _eventsLastJson) {
          _eventsClient.print("data: ");
          _eventsClient.print(j);
          _eventsClient.print("\n\n");
          _eventsLastJson = j;
        }
        _eventsLastSendMs = nowMs;
      }
    }
  }
  if (_staConnecting) {
    if (WiFi.status() == WL_CONNECTED) {
      _staConnecting = false;
      _staFinalizePending = true;
      _staFinalizeAtMs = millis() + 3500;
      _timeSyncPending = true;
      _staLastFailMs = 0;
      _staLastFailCode = 0;
      StateManager::instance().setStaConnecting(false);
      StateManager::instance().setStaFail(0, false);
    } else if ((millis() - _staStartMs) >= _staTimeoutMs) {
      _staConnecting = false;
      _staLastFailMs = millis();
      _staLastFailCode = (int)WiFi.status();
      StateManager::instance().setStaConnecting(false);
      StateManager::instance().setStaFail(_staLastFailCode, true);
      WiFi.disconnect(true);
      if (_provisioning) {
        startAp(nullptr);
        StateManager::instance().setWifiProvisioning(true);
      }
    }
  }
}

bool WebRouter::isProvisioning() const { return _provisioning; }
bool WebRouter::isStaConnected() const { return WiFi.status() == WL_CONNECTED; }

void WebRouter::reprovision() {
  _prefs.putString("ssid", "");
  _prefs.putString("pwd", "");
  _staConnecting = false;
  StateManager::instance().setStaConnecting(false);
  StateManager::instance().setStaFail(0, false);
  WiFi.disconnect(true);
  startAp(nullptr);
  StateManager::instance().setWifiProvisioning(true);
}

void WebRouter::getApSsid(char* out, size_t n) const {
  if (!out || n == 0) return;
  strncpy(out, _apSsid.c_str(), n - 1);
  out[n - 1] = 0;
  StateManager::instance().setApInfo(out, nullptr);
}

void WebRouter::getApIp(char* out, size_t n) const {
  if (!out || n == 0) return;
  String s = WiFi.softAPIP().toString();
  strncpy(out, s.c_str(), n - 1);
  out[n - 1] = 0;
  StateManager::instance().setApInfo(nullptr, out);
}

void WebRouter::getStaIp(char* out, size_t n) const {
  if (!out || n == 0) return;
  String s = WiFi.localIP().toString();
  strncpy(out, s.c_str(), n - 1);
  out[n - 1] = 0;
}

void WebRouter::handleRoot() {
  _server.send_P(200, "text/html; charset=utf-8", INDEX_HTML);
}

void WebRouter::handleApiWifiForget() {
  if (_server.method() == HTTP_OPTIONS) {
    sendApiCors();
    _server.send(204, "text/plain", "");
    return;
  }
  _prefs.putString("ssid", "");
  _prefs.putString("pwd", "");
  WiFi.disconnect(true);
  startAp(nullptr);
  sendApiCors();
  _server.send(200, "application/json", "{\"ok\":true}");
}

String WebRouter::buildWifiStatusJson() const {
  bool connected = (WiFi.status() == WL_CONNECTED);
  bool connecting = _staConnecting;
  bool failed = (!connected && !connecting && _staLastFailMs != 0 && (millis() - _staLastFailMs) < 120000);

  String staIp = connected ? WiFi.localIP().toString() : "";
  String s = String("{\"provisioning\":") + (_provisioning ? "true" : "false") +
             ",\"connecting\":" + (connecting ? "true" : "false") +
             ",\"connected\":" + (connected ? "true" : "false") +
             ",\"failed\":" + (failed ? "true" : "false") +
             ",\"elapsedMs\":" + (connecting ? (millis() - _staStartMs) : 0) +
             ",\"timeoutMs\":" + _staTimeoutMs +
             ",\"failCode\":" + _staLastFailCode +
             ",\"staIp\":\"" + staIp + "\"}";
  return s;
}

void WebRouter::handleApiWifiStatus() {
  if (_server.method() == HTTP_OPTIONS) {
    sendApiCors();
    _server.send(204, "text/plain", "");
    return;
  }
  String s = buildWifiStatusJson();
  sendApiCors();
  _server.send(200, "application/json", s);
}

void WebRouter::handleApiWifiScan() {
  if (_server.method() == HTTP_OPTIONS) {
    sendApiCors();
    _server.send(204, "text/plain", "");
    return;
  }
  int n = WiFi.scanNetworks(false, true);
  String s = "{\"items\":[";
  bool first = true;
  if (n > 0) {
    for (int i = 0; i < n; ++i) {
      String ssid = WiFi.SSID(i);
      if (ssid.length() == 0) continue;
      ssid.replace("\\", "\\\\");
      ssid.replace("\"", "\\\"");
      int rssi = WiFi.RSSI(i);
      bool secure = WiFi.encryptionType(i) != WIFI_AUTH_OPEN;
      if (!first) s += ",";
      s += "{\"ssid\":\"" + ssid + "\",\"rssi\":" + String(rssi) +
           ",\"secure\":" + String(secure ? "true" : "false") + "}";
      first = false;
    }
  }
  s += "]}";
  WiFi.scanDelete();
  sendApiCors();
  _server.send(200, "application/json", s);
}

void WebRouter::handleApiWifiConfig() {
  if (_server.method() == HTTP_OPTIONS) {
    sendApiCors();
    _server.send(204, "text/plain", "");
    return;
  }
  if (!_server.hasArg("ssid")) {
    sendApiCors();
    _server.send(400, "application/json", "{\"error\":\"missing ssid\"}");
    return;
  }
  String ssid = _server.arg("ssid");
  String pwd = _server.hasArg("pwd") ? _server.arg("pwd") : "";
  _prefs.putString("ssid", ssid);
  _prefs.putString("pwd", pwd);

  WiFi.mode(WIFI_AP_STA);
  WiFi.begin(ssid.c_str(), pwd.c_str());
  _provisioning = true;
  _staConnecting = true;
  _staStartMs = millis();
  _staFinalizePending = false;
  _staLastFailMs = 0;
  _staLastFailCode = 0;
  StateManager::instance().setStaConnecting(true);
  StateManager::instance().setStaFail(0, false);
  if (!_dnsRunning) {
    _dns.start(53, "*", WiFi.softAPIP());
    _dnsRunning = true;
  }

  sendApiCors();
  _server.send(200, "application/json", "{\"ok\":true}");
}

void WebRouter::handleApiStatus() {
  if (_server.method() == HTTP_OPTIONS) {
    sendApiCors();
    _server.send(204, "text/plain", "");
    return;
  }
  sendApiCors();
  String s = buildStateJson();
  _server.send(200, "application/json", s);
}

void WebRouter::handleApiPower() {
  if (_server.method() == HTTP_OPTIONS) {
    sendApiCors();
    _server.send(204, "text/plain", "");
    return;
  }
  if (_server.hasArg("state")) {
    String v = _server.arg("state");
    v.toLowerCase();
    if (v == "off") {
      StateManager::instance().setSoftOff(true);
    } else if (v == "on") {
      StateManager::instance().setSoftOff(false);
    } else if (v == "toggle") {
      const SystemState& s = StateManager::instance().get();
      StateManager::instance().setSoftOff(!s.softOff);
    }
  }
  sendApiCors();
  String s = buildStateJson();
  _server.send(200, "application/json", s);
}

void WebRouter::handleApiConfig() {
  if (_server.method() == HTTP_OPTIONS) {
    sendApiCors();
    _server.send(204, "text/plain", "");
    return;
  }
  if (_server.hasArg("ssm")) {
    int mode = _server.arg("ssm").toInt();
    if (mode < 0) mode = 0;
    if (mode > 3) mode = 3;
    StateManager::instance().setScreenSaverMode((uint8_t)mode);
  }
  sendApiCors();
  String s = buildStateJson();
  _server.send(200, "application/json", s);
}

void WebRouter::sendApiCors() {
  _server.sendHeader("Access-Control-Allow-Origin", "*");
  _server.sendHeader("Access-Control-Allow-Methods", "GET,POST,PATCH,OPTIONS");
  _server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
}

String WebRouter::buildStateJson() const {
  const SystemState& s0 = StateManager::instance().get();
  char hex[8];
  snprintf(hex, sizeof(hex), "#%02X%02X%02X", s0.breatheR, s0.breatheG, s0.breatheB);
  String fxName = "off";
  EffectMode m = s0.effect;
  if (m == EFFECT_RAINBOW) fxName = "rainbow";
  else if (m == EFFECT_BREATHE) fxName = "breathe";
  else if (m == EFFECT_BREATHE_COLOR) fxName = "breathe_color";
  else if (m == EFFECT_WIPE) fxName = "wipe";
  else if (m == EFFECT_SOLID) fxName = "solid";
  else if (m == EFFECT_MANUAL) fxName = "manual";
  String s = String("{\"ct\":") + s0.kelvin +
             ",\"b\":" + s0.cctBrightness +
             ",\"wb\":" + s0.wsBrightness +
             ",\"wsOn\":" + (s0.wsOn ? "true" : "false") +
             ",\"cctOn\":" + (s0.cctOn ? "true" : "false") +
             ",\"hex\":\"" + String(hex) + "\"" +
             ",\"fx\":\"" + fxName + "\"" +
             ",\"bs\":" + (s0.breatheHalfMs / 1000.0f) +
             ",\"scOn\":" + (s0.solidCycleOn ? "true" : "false") +
             ",\"scs\":" + (s0.solidCycleMs / 1000.0f) +
             ",\"n\":" + s0.numPixels +
             ",\"provisioning\":" + (s0.provisioning ? "true" : "false") +
             ",\"staConnected\":" + (s0.staConnected ? "true" : "false") +
             ",\"staIp\":\"" + String(s0.staIp) + "\"" +
             ",\"apSsid\":\"" + String(s0.apSsid) + "\"" +
             ",\"apIp\":\"" + String(s0.apIp) + "\"" +
             ",\"softOff\":" + (s0.softOff ? "true" : "false") +
             ",\"ssm\":" + s0.screenSaverMode +
             "}";
  return s;
}

bool WebRouter::updateLightFromArgs() {
  bool changed = false;
  if (_server.hasArg("ct")) {
    int k = _server.arg("ct").toInt();
    int b = _led.getCCTBrightness();
    _led.setCCT(k, b);
    StateManager::instance().setLight(_led.getKelvin(), _led.getCCTBrightness(), _led.getCCTEnabled(), _led.getBrightness(), _led.getEnabled());
    changed = true;
  }
  if (_server.hasArg("b")) {
    int k = _led.getKelvin();
    int b = _server.arg("b").toInt();
    _led.setCCT(k, b);
    StateManager::instance().setLight(_led.getKelvin(), _led.getCCTBrightness(), _led.getCCTEnabled(), _led.getBrightness(), _led.getEnabled());
    changed = true;
  }
  if (_server.hasArg("wb")) {
    int v = _server.arg("wb").toInt();
    if (v < 0) v = 0;
    if (v > 255) v = 255;
    _led.setBrightness((uint8_t)v);
    StateManager::instance().setLight(_led.getKelvin(), _led.getCCTBrightness(), _led.getCCTEnabled(), _led.getBrightness(), _led.getEnabled());
    changed = true;
  }
  if (_server.hasArg("wsOn")) {
    String v = _server.arg("wsOn"); v.toLowerCase();
    bool on = (v == "1" || v == "true" || v == "on");
    _led.setEnabled(on);
    StateManager::instance().setLight(_led.getKelvin(), _led.getCCTBrightness(), _led.getCCTEnabled(), _led.getBrightness(), _led.getEnabled());
    changed = true;
  }
  if (_server.hasArg("cctOn")) {
    String v = _server.arg("cctOn"); v.toLowerCase();
    bool on = (v == "1" || v == "true" || v == "on");
    _led.setCCTEnabled(on);
    StateManager::instance().setLight(_led.getKelvin(), _led.getCCTBrightness(), _led.getCCTEnabled(), _led.getBrightness(), _led.getEnabled());
    changed = true;
  }
  if (_server.hasArg("cr") && _server.hasArg("cg") && _server.hasArg("cb")) {
    uint8_t r = (uint8_t)_server.arg("cr").toInt();
    uint8_t g = (uint8_t)_server.arg("cg").toInt();
    uint8_t b = (uint8_t)_server.arg("cb").toInt();
    StateManager::instance().setBreatheColor(r, g, b);
    _effects.setBreatheColor(r, g, b);
    changed = true;
  }
  if (_server.hasArg("bs")) {
    float s = _server.arg("bs").toFloat();
    if (s < 0.2f) s = 0.2f;
    if (s > 10.0f) s = 10.0f;
    uint16_t v = (uint16_t)(s * 1000.0f);
    StateManager::instance().setBreatheHalfMs(v);
    _effects.setBreatheHalfMs(v);
    changed = true;
  }
  if (_server.hasArg("scOn")) {
    String v = _server.arg("scOn"); v.toLowerCase();
    bool on = (v == "1" || v == "true" || v == "on");
    StateManager::instance().setSolidCycle(on);
    _effects.setSolidCycleEnabled(on);
    changed = true;
  }
  if (_server.hasArg("scs")) {
    float s = _server.arg("scs").toFloat();
    if (s < 0.2f) s = 0.2f;
    if (s > 20.0f) s = 20.0f;
    uint16_t v = (uint16_t)(s * 1000.0f);
    StateManager::instance().setSolidCycleMs(v);
    _effects.setSolidCycleMs(v);
  }
  return changed;
}

void WebRouter::handleApiLight() {
  if (_server.method() == HTTP_OPTIONS) {
    sendApiCors();
    _server.send(204, "text/plain", "");
    return;
  }
  uint32_t nowMs = millis();
  if (_lastLightUpdateMs != 0 && (nowMs - _lastLightUpdateMs) < _lightMinIntervalMs && _lastLightResponse.length() > 0) {
    sendApiCors();
    _server.send(200, "application/json", _lastLightResponse);
    return;
  }
  updateLightFromArgs();
  sendApiCors();
  String s = buildStateJson();
  _lastLightUpdateMs = nowMs;
  _lastLightResponse = s;
  _server.send(200, "application/json", s);
}

void WebRouter::applyEffectFromArgs() {
  String n = _server.arg("name");
  EffectMode m = EFFECT_OFF;
  if (n == "off") m = EFFECT_OFF;
  else if (n == "rainbow") m = EFFECT_RAINBOW;
  else if (n == "breathe") m = EFFECT_BREATHE;
  else if (n == "breathe_color") m = EFFECT_BREATHE_COLOR;
  else if (n == "solid") m = EFFECT_SOLID;
  else if (n == "wipe") m = EFFECT_WIPE;
  else if (n == "manual") m = EFFECT_MANUAL;
  StateManager::instance().setEffectMode(m);
  _effects.setEffect(m);
}

void WebRouter::handleApiEffect() {
  if (_server.method() == HTTP_OPTIONS) {
    sendApiCors();
    _server.send(204, "text/plain", "");
    return;
  }
  applyEffectFromArgs();
  sendApiCors();
  String s = buildStateJson();
  _server.send(200, "application/json", s);
}

static const char MANUAL_PAGE[] PROGMEM = R"HTML(<!DOCTYPE html>
<html lang="zh-CN">
<head>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1">
<title>手动像素控制 - 手办盒灯光</title>
<style>
*{box-sizing:border-box;margin:0;padding:0;font-family:-apple-system,BlinkMacSystemFont,"Segoe UI",Roboto,"Helvetica Neue",Arial,"PingFang SC","Microsoft Yahei",sans-serif}
body{background:radial-gradient(circle at top,#1f2937 0,#020617 55%,#000 100%);color:#e5e7eb;min-height:100vh;display:flex;align-items:center;justify-content:center;padding:28px}
.app{width:100%;max-width:1120px;background:linear-gradient(145deg,rgba(15,23,42,0.96),rgba(17,24,39,0.98));border-radius:24px;box-shadow:0 20px 60px rgba(0,0,0,0.75),0 0 0 1px rgba(148,163,184,0.2);padding:26px 30px 30px;backdrop-filter:blur(16px)}
.app-header{display:flex;align-items:center;justify-content:space-between;gap:18px;margin-bottom:24px}
.title-block{display:flex;align-items:center;gap:12px}
.logo-dot{width:32px;height:32px;border-radius:999px;background:conic-gradient(from 180deg,#f97316,#22c55e,#3b82f6,#a855f7,#f97316);padding:2px}
.logo-inner{width:100%;height:100%;border-radius:inherit;background:radial-gradient(circle at 30% 20%,#facc15,#0f172a 55%,#020617 100%);box-shadow:0 0 20px rgba(249,115,22,0.45)}
.title-text-main{font-size:20px;font-weight:600;letter-spacing:0.04em}
.title-text-sub{font-size:12px;color:#9ca3af}
.btn{border-radius:999px;padding:10px 16px;border:none;font-size:14px;cursor:pointer;display:inline-flex;align-items:center;justify-content:center;gap:6px;white-space:nowrap;min-height:44px;transition:transform 0.08s ease-out,box-shadow 0.12s ease-out,background 0.15s ease-out}
.btn-primary{background:linear-gradient(135deg,#f97316,#fbbf24);color:#111827;box-shadow:0 10px 30px rgba(249,115,22,0.5)}
.btn-primary:active{transform:translateY(1px) scale(0.98);box-shadow:0 4px 16px rgba(249,115,22,0.45)}
.btn-ghost{background:rgba(15,23,42,0.85);color:#e5e7eb;border:1px solid rgba(148,163,184,0.45)}
.btn-ghost:active{transform:translateY(1px) scale(0.98);box-shadow:0 4px 16px rgba(15,23,42,0.65)}
.badge{font-size:11px;padding:3px 8px;border-radius:999px;border:1px solid rgba(148,163,184,0.6);background:rgba(15,23,42,0.9);color:#9ca3af}
.layout{display:grid;grid-template-columns:1.1fr 1.9fr;gap:20px}
.card{background:radial-gradient(circle at top left,rgba(248,250,252,0.04),transparent 55%);border-radius:18px;padding:18px;border:1px solid rgba(30,64,175,0.5);box-shadow:0 18px 38px rgba(15,23,42,0.75),inset 0 0 30px rgba(15,23,42,0.7);position:relative;overflow:hidden}
.card::before{content:"";position:absolute;inset:0;background:radial-gradient(circle at top right,rgba(56,189,248,0.18),transparent 55%);opacity:0.7;pointer-events:none}
.card-inner{position:relative;z-index:1}
.card-title{font-size:13px;font-weight:600;letter-spacing:0.08em;text-transform:uppercase;color:#9ca3af;margin-bottom:10px}
.card-sub{font-size:12px;color:#6b7280;margin-bottom:10px}
.info-line{display:flex;justify-content:space-between;font-size:13px;margin-bottom:4px}
.info-label{color:#9ca3af}
.info-value{color:#e5e7eb}
.pixel-header{display:flex;align-items:center;justify-content:space-between;margin-bottom:10px}
.pixel-title{display:flex;justify-content:space-between;align-items:center;font-size:12px;margin-bottom:6px}
.pixel-id{font-weight:500;color:#e5e7eb}
.pixel-row{display:flex;align-items:center;gap:10px;margin-bottom:10px}
.pixel-row label{font-size:12px;color:#9ca3af;min-width:40px}
.pixel-row input[type=range]{flex:1;touch-action:pan-x}
.pixel-row span{font-size:12px}
.color-swatch{width:18px;height:18px;border-radius:999px;border:1px solid rgba(148,163,184,0.7)}
input[type=range]{width:100%;min-height:36px}
input[type=color]{border-radius:999px;border:1px solid rgba(148,163,184,0.45);background:rgba(15,23,42,0.95);width:72px;height:36px;padding:2px}
.hint{font-size:12px;color:#6b7280;margin-top:6px}
.layout-controls{margin-bottom:10px}
.layout-row{display:flex;flex-wrap:wrap;gap:10px;margin-bottom:8px}
.layout-field{display:flex;align-items:center;gap:4px;font-size:12px;color:#9ca3af}
.layout-field span{min-width:32px}
.layout-field input{width:64px;border-radius:999px;border:1px solid rgba(148,163,184,0.4);background:rgba(15,23,42,0.9);color:#e5e7eb;padding:6px 10px;font-size:12px;min-height:36px}
.rect-layout{display:flex;flex-direction:column;align-items:center;gap:8px;margin-top:6px;touch-action:none;--dot-size:22px;--dot-gap:8px;overflow:auto}
.rect-top,.rect-bottom{display:flex;gap:var(--dot-gap);justify-content:center;flex-wrap:nowrap}
.rect-middle{display:flex;align-items:center;justify-content:center;gap:12px}
.rect-left,.rect-right{display:flex;flex-direction:column;gap:var(--dot-gap)}
.rect-center{width:var(--rect-w,240px);height:var(--rect-h,160px);flex:0 0 auto}
.led-wrap{display:flex;flex-direction:column;align-items:center;font-size:10px;color:#9ca3af}
.led-wrap-top{flex-direction:column-reverse;align-items:center}
.led-wrap-left{flex-direction:row-reverse;align-items:center}
.led-wrap-right{flex-direction:row;align-items:center}
.led-label{user-select:none;pointer-events:none}
.led-wrap-left .led-label,.led-wrap-right .led-label{min-width:18px;text-align:center}
.led-wrap-left .led-label{margin-right:4px}
.led-wrap-right .led-label{margin-left:4px}
.led-dot{width:var(--dot-size);height:var(--dot-size);border-radius:999px;border:1px solid rgba(148,163,184,0.7);background:transparent;cursor:pointer;box-shadow:0 0 0 1px rgba(15,23,42,0.8)}
.btn:focus-visible,input:focus-visible{outline:2px solid rgba(249,115,22,0.8);outline-offset:2px}
.led-on{opacity:1}
.led-off{opacity:0.35}
@media (max-width:900px){body{padding:14px;align-items:stretch}.app{max-width:100%;height:100%;border-radius:18px;padding:18px 18px 22px}.layout{grid-template-columns:1fr}.app-header{flex-direction:column;align-items:flex-start}}
@media (max-width:720px){body{font-size:15px}.title-text-main{font-size:18px}.title-text-sub{font-size:12px}.btn{width:100%}.layout-row{gap:8px}.rect-middle{flex-direction:column}.rect-layout{--dot-size:18px;--dot-gap:6px}}
</style>
</head>
<body>
<div class="app">
  <div class="app-header">
    <div class="title-block">
      <div class="logo-dot"><div class="logo-inner"></div></div>
      <div>
        <div class="title-text-main">单颗像素控制</div>
        <div class="title-text-sub">为每一颗 WS2812 调整颜色与亮度</div>
      </div>
    </div>
    <div style="display:flex;align-items:center;gap:8px">
      <span class="badge" id="pixel-summary">像素数量: -</span>
      <button class="btn btn-ghost" id="btn-back">返回主面板</button>
    </div>
  </div>
  <div class="layout">
    <div class="card">
      <div class="card-inner">
        <div class="card-title">当前状态</div>
        <div class="card-sub">查看总像素数与当前效果模式</div>
        <div class="info-line">
          <span class="info-label">像素数量</span>
          <span class="info-value" id="info-n">-</span>
        </div>
        <div class="info-line">
          <span class="info-label">当前效果</span>
          <span class="info-value" id="info-fx">-</span>
        </div>
        <div class="info-line">
          <span class="info-label">电源</span>
          <span class="info-value" id="info-power">-</span>
        </div>
        <div class="hint">进入页面后会自动切换到手动像素模式，可在右侧矩形上选择灯珠，再在下方统一调节。</div>
        <div style="margin-top:14px;border-top:1px solid rgba(55,65,81,0.6);padding-top:10px">
          <div class="card-title" style="margin-bottom:4px">选中控制</div>
          <div class="card-sub" id="selected-summary">当前选中 0 / 0</div>
          <div class="pixel-row">
            <label>亮度</label>
            <input type="range" min="0" max="255" step="1" value="128" id="sel-br">
            <span id="sel-brv">128</span>
          </div>
          <div class="pixel-row">
            <label>颜色</label>
            <input type="color" id="sel-color" value="#ffa500">
            <div class="color-swatch" id="sel-swatch"></div>
          </div>
          <div class="pixel-row">
            <label></label>
            <button class="btn btn-primary" id="sel-apply">应用颜色和亮度到选中</button>
          </div>
        </div>
      </div>
    </div>
    <div class="card">
      <div class="card-inner">
        <div class="pixel-header">
          <div>
            <div class="card-title">像素编辑</div>
            <div class="card-sub">按矩形排列灯珠，点击选择后在左侧统一调节</div>
          </div>
          <span class="badge" id="badge-loaded">未加载</span>
        </div>
        <div class="layout-controls">
          <div class="layout-row">
            <div class="layout-field">
              <span>上边</span>
              <input type="number" id="side-top" min="0" value="0">
            </div>
            <div class="layout-field">
              <span>右边</span>
              <input type="number" id="side-right" min="0" value="0">
            </div>
            <div class="layout-field">
              <span>下边</span>
              <input type="number" id="side-bottom" min="0" value="0">
            </div>
            <div class="layout-field">
              <span>左边</span>
              <input type="number" id="side-left" min="0" value="0">
            </div>
          </div>
          <div class="hint" id="layout-hint">四条边数量之和需要等于像素数量。</div>
        </div>
        <div class="rect-layout">
          <div class="rect-top" id="strip-top"></div>
          <div class="rect-middle">
            <div class="rect-left" id="strip-left"></div>
            <div class="rect-center"></div>
            <div class="rect-right" id="strip-right"></div>
          </div>
          <div class="rect-bottom" id="strip-bottom"></div>
        </div>
      </div>
    </div>
  </div>
</div>
<script>
let pixelCount=0;
let pixels=[];
let layout={top:0,right:0,bottom:0,left:0};
let selected={};
let dragging=false;
let dragHandled={};
let dragPointerId=null;
let dragStartX=0;
let dragStartY=0;
let manualModeApplied=false;
function clamp(x,a,b){return Math.max(a,Math.min(b,x))}
function getSelectedIndices(){
  const res=[];
  for(let i=0;i<pixelCount;i++){
    if(selected[i])res.push(i);
  }
  return res;
}
function updateSelectedSummary(){
  const el=document.getElementById("selected-summary");
  if(!el)return;
  const count=getSelectedIndices().length;
  el.textContent="当前选中 "+String(count)+" / "+String(pixelCount);
}
function updateLayoutHint(ok,msg){
  const h=document.getElementById("layout-hint");
  if(!h)return;
  h.textContent=msg;
  h.style.color=ok?"#6b7280":"#fecaca";
}
function initDefaultLayout(){
  if(pixelCount<=0){
    layout.top=0;layout.right=0;layout.bottom=0;layout.left=0;
    return;
  }
  const base=Math.floor(pixelCount/4);
  const extra=pixelCount-base*4;
  layout.top=base+(extra>0?1:0);
  layout.right=base+(extra>1?1:0);
  layout.bottom=base+(extra>2?1:0);
  layout.left=base;
}
function applyLayoutInputs(){
  const t=document.getElementById("side-top");
  const r=document.getElementById("side-right");
  const b=document.getElementById("side-bottom");
  const l=document.getElementById("side-left");
  if(t)t.value=String(layout.top);
  if(r)r.value=String(layout.right);
  if(b)b.value=String(layout.bottom);
  if(l)l.value=String(layout.left);
  updateLayoutHint(true,"当前像素数量 "+String(pixelCount)+" ，请根据实际灯带分配四边数量。");
}
function readLayoutFromInputs(){
  const t=document.getElementById("side-top");
  const r=document.getElementById("side-right");
  const b=document.getElementById("side-bottom");
  const l=document.getElementById("side-left");
  let top=t?parseInt(t.value,10):0;
  let right=r?parseInt(r.value,10):0;
  let bottom=b?parseInt(b.value,10):0;
  let left=l?parseInt(l.value,10):0;
  if(isNaN(top)||top<0)top=0;
  if(isNaN(right)||right<0)right=0;
  if(isNaN(bottom)||bottom<0)bottom=0;
  if(isNaN(left)||left<0)left=0;
  const sum=top+right+bottom+left;
  if(sum!==pixelCount){
    updateLayoutHint(false,"四条边数量之和必须等于像素数量 "+String(pixelCount));
    return false;
  }
  layout.top=top;
  layout.right=right;
  layout.bottom=bottom;
  layout.left=left;
  updateLayoutHint(true,"布局已应用，点击矩形上的灯珠进行选择。");
  return true;
}
function createLedElement(index,side){
  const wrap=document.createElement("div");
  if(side==="left")wrap.className="led-wrap led-wrap-left";
  else if(side==="right")wrap.className="led-wrap led-wrap-right";
  else if(side==="top")wrap.className="led-wrap led-wrap-top";
  else wrap.className="led-wrap";
  wrap.dataset.side=side;
  const dot=document.createElement("div");
  dot.className="led-dot";
  const it=pixels[index]||{r:255,g:255,b:255,br:0,on:false};
  const hex="#"+
    ("0"+it.r.toString(16)).slice(-2)+
    ("0"+it.g.toString(16)).slice(-2)+
    ("0"+it.b.toString(16)).slice(-2);
  if(it.on){
    dot.style.background=hex;
    dot.classList.add("led-on");
  }else{
    dot.style.background="transparent";
    dot.classList.add("led-off");
  }
  wrap.dataset.index=String(index);
  const label=document.createElement("div");
  label.className="led-label";
  label.textContent=String(index+1);
  wrap.appendChild(dot);
  wrap.appendChild(label);
  async function handleToggle(){
    if(selected[index])delete selected[index];else selected[index]=true;
    updateSelectedSummary();
    const colorInput=document.getElementById("sel-color");
    const brInput=document.getElementById("sel-br");
    let hex=colorInput?colorInput.value:"#ffffff";
    if(!/^#[0-9a-fA-F]{6}$/.test(hex))hex="#ffffff";
    const br=parseInt(brInput?brInput.value:"128",10);
    const r=parseInt(hex.substr(1,2),16);
    const g=parseInt(hex.substr(3,2),16);
    const b=parseInt(hex.substr(5,2),16);
    const p=pixels[index]||{r:255,g:255,b:255,br:0,on:false};
    const on=!p.on;
    const url="/px?i="+index+
      "&r="+clamp(r,0,255)+
      "&g="+clamp(g,0,255)+
      "&b="+clamp(b,0,255)+
      "&br="+clamp(br,0,255)+
      "&on="+(on?"1":"0");
    await fetch(url,{cache:"no-store"});
    pixels[index]={r:r,g:g,b:b,br:clamp(br,0,255),on:on};
    const newHex="#"+
      ("0"+r.toString(16)).slice(-2)+
      ("0"+g.toString(16)).slice(-2)+
      ("0"+b.toString(16)).slice(-2);
    if(on){
      dot.style.background=newHex;
      dot.classList.add("led-on");
      dot.classList.remove("led-off");
    }else{
      dot.style.background="transparent";
      dot.classList.add("led-off");
      dot.classList.remove("led-on");
    }
  }
  wrap._handleToggle=handleToggle;
  wrap.onpointerdown=function(e){
    dragging=true;
    dragHandled={};
    dragPointerId=e.pointerId;
    dragStartX=e.clientX;
    dragStartY=e.clientY;
    const idx=parseInt(wrap.dataset.index||String(index),10);
    if(!isNaN(idx))dragHandled[idx]=true;
    handleToggle();
  };
  return wrap;
}
function renderRect(){
  const topStrip=document.getElementById("strip-top");
  const rightStrip=document.getElementById("strip-right");
  const bottomStrip=document.getElementById("strip-bottom");
  const leftStrip=document.getElementById("strip-left");
  const rectLayout=document.querySelector(".rect-layout");
  const rectCenter=document.querySelector(".rect-center");
  if(!topStrip||!rightStrip||!bottomStrip||!leftStrip||!rectLayout||!rectCenter)return;
  topStrip.innerHTML="";
  rightStrip.innerHTML="";
  bottomStrip.innerHTML="";
  leftStrip.innerHTML="";
  const topCount=layout.top;
  const rightCount=layout.right;
  const bottomCount=layout.bottom;
  const leftCount=layout.left;
  const total=topCount+rightCount+bottomCount+leftCount;
  if(total!==pixelCount){
    updateLayoutHint(false,"四条边数量之和必须等于像素数量 "+String(pixelCount));
    return;
  }
  const styles=window.getComputedStyle(rectLayout);
  const dotSize=parseFloat(styles.getPropertyValue("--dot-size"))||22;
  const gap=parseFloat(styles.getPropertyValue("--dot-gap"))||8;
  const topW=topCount>0?(topCount*dotSize+(topCount-1)*gap):dotSize;
  const bottomW=bottomCount>0?(bottomCount*dotSize+(bottomCount-1)*gap):dotSize;
  const leftH=leftCount>0?(leftCount*dotSize+(leftCount-1)*gap):dotSize;
  const rightH=rightCount>0?(rightCount*dotSize+(rightCount-1)*gap):dotSize;
  const centerW=Math.max(topW,bottomW,160);
  const centerH=Math.max(leftH,rightH,120);
  rectCenter.style.width=centerW+"px";
  rectCenter.style.height=centerH+"px";
  topStrip.style.minWidth=centerW+"px";
  bottomStrip.style.minWidth=centerW+"px";
  leftStrip.style.minHeight=centerH+"px";
  rightStrip.style.minHeight=centerH+"px";
  const bottomBase=0;
  const rightBase=bottomBase+bottomCount;
  const topBase=rightBase+rightCount;
  const leftBase=topBase+topCount;
  for(let i=0;i<bottomCount;i++){
    const idx=bottomBase+i;
    if(idx<pixelCount)bottomStrip.appendChild(createLedElement(idx,"bottom"));
  }
  let buf=[];
  for(let i=rightCount-1;i>=0;i--){
    const idx=rightBase+i;
    if(idx<pixelCount)buf.push(createLedElement(idx,"right"));
  }
  buf.forEach(function(el){rightStrip.appendChild(el);});
  buf=[];
  for(let i=topCount-1;i>=0;i--){
    const idx=topBase+i;
    if(idx<pixelCount)buf.push(createLedElement(idx,"top"));
  }
  buf.forEach(function(el){topStrip.appendChild(el);});
  buf=[];
  for(let i=0;i<leftCount;i++){
    const idx=leftBase+i;
    if(idx<pixelCount)buf.push(createLedElement(idx,"left"));
  }
  buf.forEach(function(el){leftStrip.appendChild(el);});
  updateSelectedSummary();
}
async function applyToSelected(mode){
  const indices=getSelectedIndices();
  if(indices.length===0)return;
  const colorInput=document.getElementById("sel-color");
  const brInput=document.getElementById("sel-br");
  let hex=colorInput?colorInput.value:"#ffffff";
  if(!/^#[0-9a-fA-F]{6}$/.test(hex))hex="#ffffff";
  const br=parseInt(brInput?brInput.value:"128",10);
  const r=parseInt(hex.substr(1,2),16);
  const g=parseInt(hex.substr(3,2),16);
  const b=parseInt(hex.substr(5,2),16);
  const tasks=[];
  for(let k=0;k<indices.length;k++){
    const idx=indices[k];
    const p=pixels[idx]||{r:255,g:255,b:255,br:0,on:true};
    let on=p.on;
    if(mode==="off")on=false;
    else if(mode==="on")on=true;
    const url="/px?i="+idx+
      "&r="+clamp(r,0,255)+
      "&g="+clamp(g,0,255)+
      "&b="+clamp(b,0,255)+
      "&br="+clamp(br,0,255)+
      "&on="+(on?"1":"0");
    tasks.push(fetch(url,{cache:"no-store"}));
    pixels[idx]={r:r,g:g,b:b,br:clamp(br,0,255),on:on};
  }
  if(tasks.length>0)await Promise.all(tasks);
  renderRect();
}
async function fetchState(){
  const r=await fetch("/state",{cache:"no-store"});
  const s=await r.json();
  pixelCount=s.n||0;
  document.getElementById("info-n").textContent=String(pixelCount);
  document.getElementById("info-fx").textContent=s.fx||"-";
  document.getElementById("info-power").textContent=s.softOff?"已关机":"运行中";
  const sum=document.getElementById("pixel-summary");
  if(sum){sum.textContent="像素数量: "+String(pixelCount);}
}
async function loadPixels(){
  if(!manualModeApplied){
    manualModeApplied=true;
    await fetch("/effect?name=manual",{cache:"no-store"});
  }
  await fetchState();
  const badge=document.getElementById("badge-loaded");
  if(badge)badge.textContent="加载中...";
  selected={};
  const r=await fetch("/pixels_state",{cache:"no-store"});
  const j=await r.json();
  const list=j.items||[];
  pixels=[];
  pixelCount=list.length;
  for(let i=0;i<pixelCount;i++){
    const it=list[i]||{r:255,g:255,b:255,br:0,on:false};
    pixels.push({r:it.r,g:it.g,b:it.b,br:it.br,on:!!it.on});
  }
  if(layout.top+layout.right+layout.bottom+layout.left!==pixelCount){
    initDefaultLayout();
  }
  applyLayoutInputs();
  const sideIds=["side-top","side-right","side-bottom","side-left"];
  sideIds.forEach(function(id){
    const el=document.getElementById(id);
    if(el){
      el.onchange=function(){
        if(readLayoutFromInputs()){
          renderRect();
        }
      };
    }
  });
  const selBr=document.getElementById("sel-br");
  const selBrv=document.getElementById("sel-brv");
  if(selBr&&selBrv){
    selBr.oninput=function(e){
      selBrv.textContent=e.target.value;
    };
  }
  const selColor=document.getElementById("sel-color");
  const selSw=document.getElementById("sel-swatch");
  if(selColor&&selSw){
    selSw.style.background=selColor.value;
    selColor.oninput=function(e){
      selSw.style.background=e.target.value;
    };
  }
  const selApply=document.getElementById("sel-apply");
  if(selApply){
    selApply.onclick=function(){
      applyToSelected("color");
    };
  }
  document.addEventListener("mouseup",function(){
    dragging=false;
    dragHandled={};
    dragPointerId=null;
  });
  document.addEventListener("pointerup",function(e){
    if(dragPointerId!==null&&e.pointerId!==dragPointerId)return;
    dragging=false;
    dragHandled={};
    dragPointerId=null;
  });
  document.addEventListener("pointercancel",function(e){
    if(dragPointerId!==null&&e.pointerId!==dragPointerId)return;
    dragging=false;
    dragHandled={};
    dragPointerId=null;
  });
  document.addEventListener("pointermove",function(e){
    if(!dragging)return;
    if(dragPointerId!==null&&e.pointerId!==dragPointerId)return;
    const dx=e.clientX-dragStartX;
    const dy=e.clientY-dragStartY;
    const el=document.elementFromPoint(e.clientX,e.clientY);
    if(!el)return;
    const wrap=el.closest(".led-wrap");
    if(!wrap)return;
    const side=wrap.dataset.side||"";
    const threshold=4;
    if(side==="top"||side==="bottom"){
      if(Math.abs(dy)>Math.abs(dx)&&Math.abs(dy)>threshold){
        dragging=false;
        dragHandled={};
        dragPointerId=null;
        return;
      }
    }else if(side==="left"||side==="right"){
      if(Math.abs(dx)>Math.abs(dy)&&Math.abs(dx)>threshold){
        dragging=false;
        dragHandled={};
        dragPointerId=null;
        return;
      }
    }else{
      if(Math.abs(dy)>Math.abs(dx)&&Math.abs(dy)>threshold){
        dragging=false;
        dragHandled={};
        dragPointerId=null;
        return;
      }
    }
    const idxStr=wrap.dataset.index;
    const idx=idxStr?parseInt(idxStr,10):NaN;
    if(isNaN(idx))return;
    if(dragHandled[idx])return;
    dragHandled[idx]=true;
    if(typeof wrap._handleToggle==="function")wrap._handleToggle();
  });
  renderRect();
  if(badge){badge.textContent="已加载 "+String(pixelCount)+" 颗像素";}
}
document.getElementById("btn-back").onclick=()=>{location.href="/";};
loadPixels();
</script>
</body>
</html>)HTML";

void WebRouter::handleManual() {
  _server.send_P(200, "text/html; charset=utf-8", MANUAL_PAGE);
}

void WebRouter::handlePixelsState() {
  uint16_t n = _effects.getPixelCount();
  String s = "{\"items\":[";
  for (uint16_t i = 0; i < n; ++i) {
    uint8_t r, g, b, br;
    bool on;
    _effects.getPixel(i, r, g, b, br, on);
    if (i > 0) s += ",";
    s += "{\"r\":" + String(r) +
         ",\"g\":" + String(g) +
         ",\"b\":" + String(b) +
         ",\"br\":" + String(br) +
         ",\"on\":" + String(on ? "true" : "false") +
         "}";
  }
  s += "]}";
  _server.send(200, "application/json", s);
}

void WebRouter::handlePixel() {
  if (!_server.hasArg("i")) {
    _server.send(400, "application/json", "{\"error\":\"missing index\"}");
    return;
  }
  int idx = _server.arg("i").toInt();
  if (idx < 0 || (uint16_t)idx >= _effects.getPixelCount()) {
    _server.send(400, "application/json", "{\"error\":\"bad index\"}");
    return;
  }
  uint8_t r = 255, g = 255, b = 255, br = 0;
  bool on = false;
  if (_server.hasArg("r")) {
    int v = _server.arg("r").toInt();
    if (v < 0) v = 0;
    if (v > 255) v = 255;
    r = (uint8_t)v;
  }
  if (_server.hasArg("g")) {
    int v = _server.arg("g").toInt();
    if (v < 0) v = 0;
    if (v > 255) v = 255;
    g = (uint8_t)v;
  }
  if (_server.hasArg("b")) {
    int v = _server.arg("b").toInt();
    if (v < 0) v = 0;
    if (v > 255) v = 255;
    b = (uint8_t)v;
  }
  if (_server.hasArg("br")) {
    int v = _server.arg("br").toInt();
    if (v < 0) v = 0;
    if (v > 255) v = 255;
    br = (uint8_t)v;
  }
  if (_server.hasArg("on")) {
    String v = _server.arg("on");
    v.toLowerCase();
    on = (v == "1" || v == "true" || v == "on");
  }
  _effects.setPixel((uint16_t)idx, r, g, b, br, on);
  _server.send(200, "application/json", "{\"ok\":true}");
}

void WebRouter::handleEvents() {
  if (_eventsConnected) {
    _eventsClient.stop();
    _eventsConnected = false;
    _eventsLastJson = "";
  }

  _server.sendHeader("Cache-Control", "no-cache");
  _server.sendHeader("Connection", "keep-alive");
  _server.setContentLength(CONTENT_LENGTH_UNKNOWN);
  _server.send(200, "text/event-stream", "");

  _eventsClient = _server.client();
  _eventsClient.setNoDelay(true);
  _eventsConnected = true;

  uint32_t nowMs = millis();
  _eventsLastSendMs = nowMs;
  _eventsLastPingMs = nowMs;

  String j = buildStateJson();
  _eventsClient.print("data: ");
  _eventsClient.print(j);
  _eventsClient.print("\n\n");
  _eventsLastJson = j;
}

static uint8_t clampSlot(int v) {
  if (v < 1) return 0;
  if (v > 4) return 0;
  return (uint8_t)v;
}

bool WebRouter::saveScene(uint8_t slot) {
  if (slot < 1 || slot > 4) return false;
  Preferences p;
  if (!p.begin("scene", false)) return false;

  char k[20];
  snprintf(k, sizeof(k), "s%u_ok", (unsigned)slot);
  p.putBool(k, true);

  snprintf(k, sizeof(k), "s%u_ct", (unsigned)slot);
  p.putUShort(k, (uint16_t)_led.getKelvin());
  snprintf(k, sizeof(k), "s%u_b", (unsigned)slot);
  p.putUChar(k, (uint8_t)_led.getCCTBrightness());
  snprintf(k, sizeof(k), "s%u_cctOn", (unsigned)slot);
  p.putBool(k, _led.getCCTEnabled());

  snprintf(k, sizeof(k), "s%u_wb", (unsigned)slot);
  p.putUChar(k, _led.getBrightness());
  snprintf(k, sizeof(k), "s%u_wsOn", (unsigned)slot);
  p.putBool(k, _led.getEnabled());

  uint8_t r, g, b;
  _effects.getBreatheColor(r, g, b);
  snprintf(k, sizeof(k), "s%u_r", (unsigned)slot);
  p.putUChar(k, r);
  snprintf(k, sizeof(k), "s%u_g", (unsigned)slot);
  p.putUChar(k, g);
  snprintf(k, sizeof(k), "s%u_b2", (unsigned)slot);
  p.putUChar(k, b);

  snprintf(k, sizeof(k), "s%u_fx", (unsigned)slot);
  p.putUChar(k, (uint8_t)_effects.getEffect());
  snprintf(k, sizeof(k), "s%u_bs", (unsigned)slot);
  p.putUShort(k, _effects.getBreatheHalfMs());
  snprintf(k, sizeof(k), "s%u_scOn", (unsigned)slot);
  p.putBool(k, _effects.getSolidCycleEnabled());
  snprintf(k, sizeof(k), "s%u_scs", (unsigned)slot);
  p.putUShort(k, _effects.getSolidCycleMs());
  snprintf(k, sizeof(k), "s%u_rs", (unsigned)slot);
  p.putUChar(k, _effects.getRainbowSpeed());
  snprintf(k, sizeof(k), "s%u_rf", (unsigned)slot);
  p.putBool(k, _effects.getRainbowDirection());
  snprintf(k, sizeof(k), "s%u_ws", (unsigned)slot);
  p.putUShort(k, _effects.getWipeSpeed());
  snprintf(k, sizeof(k), "s%u_wf", (unsigned)slot);
  p.putBool(k, _effects.getWipeDirection());

  p.end();
  return true;
}

bool WebRouter::applyScene(uint8_t slot) {
  if (slot < 1 || slot > 4) return false;
  Preferences p;
  if (!p.begin("scene", true)) return false;

  char k[20];
  snprintf(k, sizeof(k), "s%u_ok", (unsigned)slot);
  bool ok = p.getBool(k, false);
  if (!ok) {
    p.end();
    return false;
  }

  snprintf(k, sizeof(k), "s%u_ct", (unsigned)slot);
  uint16_t ct = p.getUShort(k, (uint16_t)_led.getKelvin());
  snprintf(k, sizeof(k), "s%u_b", (unsigned)slot);
  uint8_t b = p.getUChar(k, (uint8_t)_led.getCCTBrightness());
  snprintf(k, sizeof(k), "s%u_cctOn", (unsigned)slot);
  bool cctOn = p.getBool(k, _led.getCCTEnabled());

  snprintf(k, sizeof(k), "s%u_wb", (unsigned)slot);
  uint8_t wb = p.getUChar(k, _led.getBrightness());
  snprintf(k, sizeof(k), "s%u_wsOn", (unsigned)slot);
  bool wsOn = p.getBool(k, _led.getEnabled());

  snprintf(k, sizeof(k), "s%u_r", (unsigned)slot);
  uint8_t r = p.getUChar(k, 255);
  snprintf(k, sizeof(k), "s%u_g", (unsigned)slot);
  uint8_t g = p.getUChar(k, 255);
  snprintf(k, sizeof(k), "s%u_b2", (unsigned)slot);
  uint8_t bb = p.getUChar(k, 255);

  snprintf(k, sizeof(k), "s%u_fx", (unsigned)slot);
  EffectMode fx = (EffectMode)p.getUChar(k, (uint8_t)_effects.getEffect());
  snprintf(k, sizeof(k), "s%u_bs", (unsigned)slot);
  uint16_t bs = p.getUShort(k, _effects.getBreatheHalfMs());
  snprintf(k, sizeof(k), "s%u_scOn", (unsigned)slot);
  bool scOn = p.getBool(k, _effects.getSolidCycleEnabled());
  snprintf(k, sizeof(k), "s%u_scs", (unsigned)slot);
  uint16_t scs = p.getUShort(k, _effects.getSolidCycleMs());
  snprintf(k, sizeof(k), "s%u_rs", (unsigned)slot);
  uint8_t rs = p.getUChar(k, _effects.getRainbowSpeed());
  snprintf(k, sizeof(k), "s%u_rf", (unsigned)slot);
  bool rf = p.getBool(k, _effects.getRainbowDirection());
  snprintf(k, sizeof(k), "s%u_ws", (unsigned)slot);
  uint16_t ws = p.getUShort(k, _effects.getWipeSpeed());
  snprintf(k, sizeof(k), "s%u_wf", (unsigned)slot);
  bool wf = p.getBool(k, _effects.getWipeDirection());

  p.end();

  _led.setCCT(ct, (int)b);
  _led.setCCTEnabled(cctOn);

  _led.setBrightness(wb);
  _led.setEnabled(wsOn);

  _effects.setBreatheColor(r, g, bb);
  _effects.setBreatheHalfMs(bs);
  _effects.setSolidCycleEnabled(scOn);
  _effects.setSolidCycleMs(scs);
  _effects.setRainbowSpeed(rs);
  _effects.setRainbowDirection(rf);
  _effects.setWipeSpeed(ws);
  _effects.setWipeDirection(wf);
  _effects.setEffect(fx);

  Preferences pw;
  if (pw.begin("scene", false)) {
    pw.putUChar("last", slot);
    pw.end();
  }

  if (_eventsConnected && _eventsClient.connected()) {
    String j = buildStateJson();
    _eventsClient.print("data: ");
    _eventsClient.print(j);
    _eventsClient.print("\n\n");
    _eventsLastJson = j;
    _eventsLastSendMs = millis();
  }
  return true;
}

void WebRouter::handleApiSceneSave() {
  if (_server.method() == HTTP_OPTIONS) {
    sendApiCors();
    _server.send(204, "text/plain", "");
    return;
  }
  uint8_t slot = 0;
  if (_server.hasArg("slot")) slot = clampSlot(_server.arg("slot").toInt());
  if (slot == 0) {
    sendApiCors();
    _server.send(400, "application/json", "{\"error\":\"bad slot\"}");
    return;
  }
  bool ok = saveScene(slot);
  if (ok) {
    sendApiCors();
    _server.send(200, "application/json", "{\"ok\":true}");
  } else {
    sendApiCors();
    _server.send(500, "application/json", "{\"ok\":false}");
  }
}

void WebRouter::handleApiSceneApply() {
  if (_server.method() == HTTP_OPTIONS) {
    sendApiCors();
    _server.send(204, "text/plain", "");
    return;
  }
  uint8_t slot = 0;
  if (_server.hasArg("slot")) slot = clampSlot(_server.arg("slot").toInt());
  if (slot == 0) {
    sendApiCors();
    _server.send(400, "application/json", "{\"error\":\"bad slot\"}");
    return;
  }
  bool ok = applyScene(slot);
  if (ok) {
    String s = buildStateJson();
    sendApiCors();
    _server.send(200, "application/json", s);
  } else {
    sendApiCors();
    _server.send(404, "application/json", "{\"error\":\"no_scene\"}");
  }
}
