#include <Arduino.h>
#include <ArduinoJson.h>
#include <AsyncJson.h>
#include <AsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <WiFi.h>

static const char* ap_ssid = "Arduino Nano ESP32";
// set to NULL to make AP open
static const char* ap_passphrase = "12345678";

static const char* sta_ssid = "My Home WiFi";
static const char* sta_passphrase = "12345678";

// index.html content
static const char* indexHtml PROGMEM = R"rawliteral(
<!DOCTYPE html>
<html>

<head>
  <title>NANO ESP32 RGB Color Picker</title>
  <meta content="width=device-width, initial-scale=1.0, user-scalable=1.0, minimum-scale=1.0, maximum-scale=5.0"
    name="viewport" />
  <link rel="icon" href="data:," />
  <style>
    /*! Color-Picker 0.13.1 MIT | https://github.com/yairEO/color-picker */
    .color-picker .range{--tickEvery: 400;--primaryColor: #000;--progress-color: transparent;--progress-shadow: unset;--value-active-color: white;--value-background: white;--value-font: 700 12px/1 Arial;--fill-color: var(--primaryColor);--thumb-size: 21px;--track-height: calc(var(--thumb-size)/1.5);--thumb-shadow: 0 0 3px rgba(0,0,0,.2);--step: 1;--completed: calc((var(--value) - var(--min) ) / (var(--max) - var(--min)) * 100);--show-min-max: none;--LTR: 1;display:inline-block;height:max(var(--track-height),var(--thumb-size));background:none;position:relative;z-index:1;padding-bottom:0;padding-top:0;margin:0 0 calc((var(--thumb-size) - var(--track-height))*-1)}.color-picker .range__progress{position:absolute;left:0;top:50%;transform:translateY(-50%) translateZ(0);width:100%;height:calc(var(--track-height));pointer-events:none;z-index:-1;box-shadow:var(--progress-shadow);border-radius:20px;background:var(--fill-color, white)}.color-picker .range__progress::after{content:unset}.color-picker .range>input{--thumb-color: transparent;--inner-shadow: 0 0 0 calc(var(--thumb-size)/8) inset white;-webkit-appearance:none;width:100%;height:var(--thumb-size);margin:0;cursor:-webkit-grab;cursor:grab;outline:none;background:none}.color-picker .range>input::-webkit-slider-thumb{appearance:none;height:var(--thumb-size);width:var(--thumb-size);border-radius:50%;background:var(--thumb-color, white);border:1px solid silver;box-shadow:var(--inner-shadow, 0 0),var(--thumb-shadow)}.color-picker .range>input::-moz-range-thumb{appearance:none;height:var(--thumb-size);width:var(--thumb-size);border-radius:50%;background:var(--thumb-color, white);border:1px solid silver;box-shadow:var(--inner-shadow, 0 0),var(--thumb-shadow)}.color-picker .range>input:active{opacity:.7;cursor:grabbing}.color-picker .range>input:active+output{transition:0s}.color-picker .range>input:hover+output{--value-background: var(--primaryColor);opacity:1;color:var(--value-active-color);transform:translate(var(--x-offset), 0);box-shadow:0 0 0 3px var(--value-background)}.color-picker .range>output{--x-offset: calc(var(--completed) * -1% * var(--LTR));--pos: calc(((var(--value) - var(--min))/(var(--max) - var(--min))) * 100%);opacity:0;pointer-events:none;position:absolute;z-index:5;background:var(--value-background);border-radius:10px;padding:0 4px;top:-3.5ch;left:var(--pos);transform:translate(var(--x-offset), 6px);transition:all .12s ease-out,left 0s,top 0s}.color-picker .range>output::before{--size: 5px;content:"";top:calc(100% + 2px);left:50%;border:solid rgba(0,0,0,0);height:0;width:0;position:absolute;pointer-events:none;border-top-color:var(--value-background);border-width:var(--size);margin-left:calc(var(--size)*-1);transition:inherit}.color-picker .range>output::after{content:var(--text-value);font:var(--value-font)}.color-picker{--hue: 150;--saturation: 100;--lightness: 50;--alpha: 100;--s: calc(var(--saturation) * 1%);--l: calc(var(--lightness) * 1%);--a: calc(var(--alpha) * 1%);--color: hsla(var(--hue), var(--s), var(--l), var(--a));--checkboard-color: #DDD;--checkboard-base-gradient: repeating-conic-gradient(var(--checkboard-color) 0% 25%, transparent 0% 50%);--width: 320;--width-units: 1px;display:flex;flex-flow:column;gap:.5em;width:calc(var(--width)*var(--width-units));position:relative;box-sizing:border-box;transition:opacity .15s;transition-timing-function:ease-out}.color-picker__hue.range{grid-area:hue;--fill-color: linear-gradient(to right, red 0%, #ff0 16.6%, lime 33.3%, cyan 50%, blue 66.6%, #f0f 83.3%, red 100%)}.color-picker__saturation.range{grid-area:saturation;--fill-color: linear-gradient(to right, white, hsl(var(--hue),var(--s), 50%))}.color-picker__lightness.range{grid-area:lightness;--c: hsl(var(--hue), var(--s), 50%);--fill-color: linear-gradient(to right, black, var(--c), white)}.color-picker__alpha.range{grid-area:alpha;--checkboard-size: 8px;--fill-color: linear-gradient(to right, transparent, hsl(var(--hue), var(--s), var(--l))), var(--checkboard-base-gradient) 0 / var(--checkboard-size) var(--checkboard-size)}.color-picker button{cursor:pointer;border:none;background:none;outline:none}.cp-checkboard::before{content:"";position:absolute;z-index:-1;top:0;bottom:0;left:0;right:0;border-radius:inherit;background:repeating-conic-gradient(var(--checkboard-color) 0% 25%, transparent 0% 50%) 0/12px 12px}.color-picker>output{grid-area:color;border-radius:5px;overflow:hidden;position:relative;width:50px;background:hsla(var(--hue), var(--s), var(--l), var(--a));box-shadow:0 0 8px -5px}.color-picker>output::before{content:"";position:absolute;z-index:-1;top:0;bottom:0;left:0;right:0;background:repeating-conic-gradient(var(--checkboard-color) 0% 25%, transparent 0% 50%) 0/12px 12px}.color-picker__value{--isLightColor: Min(1, Max(50 - var(--lightness) - (100 - var(--alpha)), 0));grid-area:value;position:relative;display:inline-flex;align-items:center;overflow:hidden;border-radius:10px;color:hsl(var(--hue), 100%, calc(var(--isLightColor) * 100%));box-shadow:0 0 4px rgba(0,0,0,.2)}.color-picker__value input{flex:1;order:2;cursor:text;width:0;letter-spacing:-0.5px;word-spacing:-3px;font:800 16px/2 monospace;font-size:calc(var(--width)*var(--width-units)/22);text-transform:uppercase;padding:0;text-align:center;border:none;outline:none;background:none;color:inherit;transition:color .2s}.color-picker__value input~div{position:absolute;z-index:-1;top:0;bottom:0;left:0;right:0;border-radius:inherit;background:var(--color)}.color-picker__value input:focus{color:#000}.color-picker__value input:focus~button{transform:translateX(100%)}.color-picker__value input:focus+button{transform:translateX(-100%)}.color-picker__value input:focus~div{background:none;transition:background .15s;border:3px solid var(--color)}.color-picker__value>button{order:3;width:1.5em;background:none;border:none;font:22px/1.2 monospace;outline:none;color:inherit;cursor:pointer;user-select:none;transition:color .2s,transform .2s ease-out}.color-picker__value>button[name=undo]{order:1}.color-picker__swatches{display:flex;flex-wrap:wrap;align-items:center;gap:3px}.color-picker__swatches>button{--shadow-size: 2px;display:flex;justify-content:center;align-items:center;order:0;padding:12px;width:0;height:0;border-radius:50%;background:var(--c);font-size:18px;line-height:0px;transition:.1s;box-shadow:0 0 0 var(--shadow-size) inset var(--color)}.color-picker__swatches>button:hover:not(:active){--shadow-size: 4px}.color-picker__swatch{order:1;padding:12px;line-height:0;border-radius:50%;background:var(--c);position:relative;cursor:pointer;transition:.15s ease-in-out}.color-picker__swatch:hover{transition:50ms}.color-picker__swatch:hover>button{opacity:1}.color-picker__swatch.cp_remove{padding:0;pointer-events:none;transition:.2s}.color-picker__swatch>button{opacity:0;position:absolute;top:0;right:0;width:0;height:0;border-radius:50%;line-height:.1;color:#000;font-weight:800;text-shadow:0 3px #fff,-2px 1px #fff}

    /* style.css */
    *,::after,::before{-webkit-box-sizing:border-box;box-sizing:border-box}
    body{padding:0;margin:0}
    .main-wrapper{padding:20px;display:flex;align-items:center;justify-content:center;min-height:100dvh}
    .color-picker.disabled{opacity:0.8;pointer-events:none}
  </style>
</head>

<body>
  <div id="main_wrapper" class="main-wrapper"></div>
  <script>
    /*! Color-Picker 0.13.1 MIT | https://github.com/yairEO/color-picker */
    var colorPicker=function(t){"use strict";var e=t=>(new DOMParser).parseFromString(t.trim(),"text/html").body.firstElementChild,s={color:"white",onInput:t=>t,onChange:t=>t,buttons:{undo:{icon:"&curvearrowleft;",title:"Undo"},format:{icon:"&leftrightarrows;",title:"Switch Color Format"},add:{icon:"+",title:"Add to Swatches"}}};const i=t=>t.match(/\((.*)\)/)[1].split(",").map(Number),a=t=>Object.assign([0,0,0,1],t.match(/\((.*)\)/)[1].split(",").map(((t,e)=>3!=e||t.includes("%")?parseFloat(t):100*parseFloat(t)))),o=t=>`hsla(${t.h}, ${t.s}%, ${t.l}%, ${t.a}%)`,n=t=>t.toFixed(1).replace(".0",""),h=t=>{const[e,s,i,a]=(t=>t.match(/\w\w/g))(t),[o,n,h]=[e,s,i].map((t=>parseInt(t,16)));return`rgba(${o},${n},${h},${a?(parseInt(a,16)/255).toFixed(2):1})`},r=t=>{var e,s=document.createElement("canvas").getContext("2d");return s.fillStyle=t,"#"==(e=s.fillStyle)[0]?e:l(e)},l=t=>{const[e,s,a,o]=i(t),n="#"+[e,s,a].map((t=>t.toString(16).padStart(2,"0"))).join("");return 3==t.length?n:n+Math.round(255*o).toString(16).padStart(2,"0")},c=t=>{let[e,s,a,o]=i(t);e/=255,s/=255,a/=255;let h=Math.max(e,s,a),r=Math.min(e,s,a),l=h-r,c=0,u=0,d=((h+r)/2).toPrecision(5);return l&&(u=d>.5?l/(2-h-r):l/(h+r),c=h==e?(s-a)/l+(s<a?6:0):c=h==s?(a-e)/l+2:(e-s)/l+4,c/=6),{h:n(360*c),s:n(100*u),l:n(100*d),a:n(100*o)}},u=(t,e)=>(e=(e+"").toLowerCase(),t=r(t),"hex"==e?t:e.startsWith("hsl")?o(c(h(t))):e.startsWith("rgb")?h(t):t);function d({name:t,min:e=0,max:s=100,value:i}){return`<div class="range color-picker__${t}" title="${t}" style="--min:${e}; --max:${s};">\n            <input type="range" name="${t}" value="${i}" min="${e}" max="${s}">\n            <output></output>\n            <div class='range__progress'></div>\n          </div>`}function p(t){const{buttons:{undo:e,format:s}}=this.settings;return`\n    <div class='color-picker__value cp-checkboard'>\n      <input name='value' inputmode='decimal' placeholder='CSS Color' value='${r(o(t))}'>\n      <button title='${e.title}' name="undo">${e.icon}</button>\n      <button title='${s.title}' name='format'>${s.icon}</button>\n      <div></div>\n    </div>\n  `}function S(t,e){const{buttons:{add:s}}=this.settings;return`\n    <div class='color-picker__swatches' style='--initial-len:${e.length}'>\n      <button name='addSwatch' title='${s.title}'>${s.icon}</button>\n      ${t.map((t=>m(t,e.includes(t)))).join("")}\n    </div>\n  `}function m(t,e){return`<div class="cp-checkboard color-picker__swatch" title="${t}" style="--c:${t}">${e?"":'<button name="removeSwatch">&times;</button>'}</div>`}var g=Object.freeze({__proto__:null,scope:function(){const{h:t,s:e,l:s,a:i}=this.color;return`\n    <div class='${`color-picker ${this.settings.className||""}`.trim()}'>\n      ${d({name:"hue",value:t,max:"360"})}\n      ${d({name:"saturation",value:e})}\n      ${d({name:"lightness",value:s})}\n      ${d({name:"alpha",value:i})}\n      <output></output>\n      ${p.call(this,this.color)}\n      ${this.swatches?S.call(this,this.swatches,this.initialSwatches):""}\n    </div>\n  `},slider:d,swatch:m,swatches:S,value:p});function v(){this.syncGlobalSwatchesWithLocal()}function w(t){t.preventDefault();const{value:e,max:s}=t.target,i=-1*Math.sign(t.deltaY);e&&s&&(t.target.value=Math.min(Math.max(+e+i,0),+s),C.call(this,t))}function b(t){"Escape"==t.key&&this.settings.onClickOutside(t)}function f(t){this.DOM.scope.contains(t.target)||this.settings.onClickOutside(t)}function C(t){const{name:e,value:s,type:i}=t.target;"range"==i&&this.setColor({...this.color,[e[0]]:+s})}function _(t){const{type:e}=t.target;"range"!=e&&"text"!=e||(this.history.last=this.color)}function O(t){this.setColor(this.getHSLA(t.target.value)),this.DOM.value.blur()}function $(t){const{name:e,parentNode:s,classList:i,title:a}=t.target;"format"==e?this.switchFormat():"undo"==e?this.history.undo():"addSwatch"==e?this.addSwatch():"removeSwatch"==e?this.removeSwatch(s,s.title):i.contains("color-picker__swatch")&&a&&(this.setColor(this.getHSLA(a)),this.history.last=this.color)}var y=Object.freeze({__proto__:null,bindEvents:function(){[["scope","input",C],["scope","change",_],["scope","click",$],["scope","wheel",w],["value","change",O]].forEach((([t,e,s])=>this.DOM[t].addEventListener(e,s.bind(this),{pasive:!1}))),window.addEventListener("storage",v.bind(this)),this.settings.onClickOutside&&(document.body.addEventListener("click",f.bind(this)),window.addEventListener("keydown",b.bind(this)))}});function M(){const t=()=>this.settings.onChange(this.CSSColor),e=this.setColor.bind(this);return{_value:[this.color],get pop(){return this._value.pop()},get previous(){return this._value[this._value.length-2]},set last(e){this._value.push(e),t()},undo(){if(this._value.length>1){this.pop;let s=this._value[this._value.length-1];return e(s),t(),s}}}}const D="@yaireo/color-picker/swatches";var x=Object.freeze({__proto__:null,addSwatch:function(t=this.CSSColor){if(((t,e)=>t.some((t=>r(t)==r(e))))(this.swatches,t))return;const s=e(this.templates.swatch(t));s.classList.add("cp_remove"),this.DOM.swatches.prepend(s),setTimeout((()=>s.classList.remove("cp_remove")),0),this.swatches.unshift(t),this.sharedSwatches.unshift(t),this.getSetGlobalSwatches(this.sharedSwatches)},getSetGlobalSwatches:function(t){const e=this.settings.swatchesLocalStorage,s="string"==typeof e?e:"";return e&&t&&(localStorage.setItem(D+s,t.join(";")),dispatchEvent(new Event("storage"))),localStorage[D+s]?.split(";").filter(String)||[]},removeSwatch:function(t,e){t.classList.add("cp_remove"),setTimeout((()=>t.remove()),200);const s=t=>t!=e;this.swatches=this.swatches.filter(s),this.sharedSwatches=this.sharedSwatches.filter(s),this.getSetGlobalSwatches(this.sharedSwatches)},syncGlobalSwatchesWithLocal:function(){this.sharedSwatches=this.getSetGlobalSwatches(),this.swatches=this.sharedSwatches.concat(this.initialSwatches),this.DOM.swatches&&setTimeout((()=>{const t=e(this.templates.swatches.call(this,this.swatches,this.initialSwatches));this.DOM.swatches.replaceWith(t),this.DOM.swatches=t}),500)}});function k(t){this.settings=Object.assign({},s,t);const{color:e,defaultFormat:i,swatches:a}=this.settings;this.DOM={},this.sharedSwatches=this.getSetGlobalSwatches(),this.initialSwatches=a||[],this.swatches=a&&this.sharedSwatches.concat(this.initialSwatches),this.color=u(e,i),this.history=M.call(this),this.init()}return k.prototype={templates:g,...x,...y,getColorFormat:t=>"#"==t[0]?"hex":t.indexOf("hsl")?t.indexOf("rgb")?"":"rgba":"hsla",getHSLA(t){let e;if(t)return t+""=="[object Object]"&&Object.keys(t).join("").startsWith("hsl")?t:(this.colorFormat=this.getColorFormat(t),t.indexOf("hsla")?(t=r(t),t=h(t),e=c(t)):(e=a(t),e={h:e[0],s:e[1],l:e[2],a:e[3]}),e)},switchFormat(){switch(this.colorFormat){case"":case"hex":this.colorFormat="rgba";break;case"rgba":this.colorFormat="hsla";break;case"hsla":this.colorFormat="hex"}this.setCSSColor(),this.DOM.value.value=this.CSSColor},updateRangeSlider(t,e){const s=this.DOM.scope.querySelector(`input[name="${t}"]`);s&&(s.value=e,s.parentNode.style.setProperty("--value",e),s.parentNode.style.setProperty("--text-value",JSON.stringify(""+Math.round(e))),this.updateCSSVar(t,e))},setCSSColor(){this.CSSColor=r(o(this.color)),"rgba"==this.colorFormat?this.CSSColor=h(this.CSSColor):"hsla"==this.colorFormat&&(this.CSSColor=o(this.color)),this.DOM.scope&&this.DOM.scope.setAttribute("data-color-format",this.colorFormat),this.settings.onInput(this.CSSColor)},setColor(t){t&&(t=this.getHSLA(t),this.color=t,this.setCSSColor(),this.DOM.scope&&this.updateAllCSSVars(),this.DOM.value&&(this.DOM.value.value=this.CSSColor))},updateCSSVar(t,e){this.DOM.scope.style.setProperty(`--${t}`,e)},updateAllCSSVars(){const t=this.NamedHSLA(this.color);Object.entries(t).forEach((([t,e])=>{this.updateRangeSlider(t,e)}))},NamedHSLA:t=>({hue:t.h,saturation:t.s,lightness:t.l,alpha:t.a}),build(){const t=this.templates.scope.call(this);this.DOM.scope=e(t),this.DOM.value=this.DOM.scope.querySelector('input[name="value"]'),this.DOM.swatches=this.DOM.scope.querySelector(".color-picker__swatches")},init(){this.build(),this.setColor(this.color),this.bindEvents()}},t.CSStoHSLA=a,t.HSLAtoCSS=o,t.any_to_hex=r,t.changeColorFormat=u,t.default=k,t.hex_rgba=h,t.rgba_hsla=c,Object.defineProperty(t,"__esModule",{value:!0}),t}({});

    /*! main.js */
    const { default: ColorPicker, any_to_hex, changeColorFormat } = window.colorPicker;
    const mainWrapper = document.getElementById("main_wrapper");

    function extractRGBA(rgbaString) {
      const match = rgbaString.match(/rgba?\(\s*(\d+)\s*,\s*(\d+)\s*,\s*(\d+)\s*,?\s*([\d\.]+)?\s*\)/);
      if (!match) return null;

      const [, r, g, b, a] = match;
      return {
        red: parseInt(r, 10),
        green: parseInt(g, 10),
        blue: parseInt(b, 10),
        alpha: a !== undefined ? parseFloat(a) : 1
      };
    }

    const picker = new ColorPicker({
      color: 'rgba(0, 0, 0, 0)',
      defaultFormat: 'rgba',
      className: '',
      swatches: [
        'rgba(244, 67, 54, 1)',
        'rgba(233, 30, 99, 0.95)',
        'rgba(156, 39, 176, 0.9)',
        'rgba(103, 58, 183, 0.85)',
        'rgba(63, 81, 181, 0.8)',
        'rgba(33, 150, 243, 0.75)',
        'rgba(3, 169, 244, 0.7)',
        'rgba(0, 188, 212, 0.7)',
        'rgba(0, 150, 136, 0.75)',
        'rgba(76, 175, 80, 0.8)',
        'rgba(139, 195, 74, 0.85)',
        'rgba(205, 220, 57, 0.9)',
        'rgba(255, 235, 59, 0.95)',
        'rgba(255, 193, 7, 1)'
      ],
      swatchesLocalStorage: true,
      buttons: {
        undo: { icon: '&curvearrowleft;', title: 'Undo' },
        format: { icon: '&leftrightarrows;', title: 'Switch Color Format' },
        add: { icon: '+', title: 'Add to Swatches' }
      },
      onInput(color){},
      onChange(color){
        const rgbaString = changeColorFormat(picker.CSSColor, "rgba");
        const rgbaObject = extractRGBA(rgbaString);
        if (!rgbaObject) return;

        picker.DOM.scope.classList.add("disabled");
        fetch("/rest/rgba", {
          method: "POST",
          body: JSON.stringify(rgbaObject),
          headers: {
            "Content-Type": "application/json"
          }
        }).then(async (res) => {
          if (res.ok) {
            const { red, green, blue, alpha } = await res.json();
            const newRgbaString = `rgba(${red},${green},${blue},${alpha})`;
            if (rgbaString !== newRgbaString) {
              picker.setColor(newRgbaString);
            }
          } else {
            throw new Error(`Failed with status ${res.status}`);
          }
        }).catch(() => {
          picker.history.undo();
        }).finally(() => {
          picker.DOM.scope.classList.remove("disabled");
        });
      },
      onClickOutside(e){}
    });

    picker.DOM.scope.style.setProperty("--width", "100%");
    picker.DOM.scope.style.maxWidth = "360px";
    // update initially
    picker.DOM.scope.classList.add("disabled");
    fetch("/rest/rgba").then(async (res) => {
      if (res.ok) {
        const { red, green, blue, alpha } = await res.json();
        const rgbaString = changeColorFormat(picker.CSSColor, "rgba");
        const newRgbaString = `rgba(${red},${green},${blue},${alpha})`;
        if (rgbaString !== newRgbaString) {
          picker.setColor(newRgbaString);
        }
      } else {
        throw new Error(`Failed with status ${res.status}`);
      }
    }).finally(() => {
      picker.DOM.scope.classList.remove("disabled");
    });
    mainWrapper.appendChild(picker.DOM.scope);
  </script>
</body>

</html>
)rawliteral";
static const size_t indexHtmlLength = strlen_P(indexHtml);

// variables to store current state of RGBA
int rangeRed = 0;
int rangeGreen = 0;
int rangeBlue = 0;
float rangeAlpha = 0.0;

AsyncJsonResponse* getRgbaJsonResponse() {
  AsyncJsonResponse* response = new AsyncJsonResponse();

  JsonObject root = response->getRoot().to<JsonObject>();
  root["red"] = rangeRed;
  root["green"] = rangeGreen;
  root["blue"] = rangeBlue;
  root["alpha"] = rangeAlpha;

  return response;
}

static AsyncWebServer server(80);

void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(LED_RED, OUTPUT);
  pinMode(LED_GREEN, OUTPUT);
  pinMode(LED_BLUE, OUTPUT);

  // Setup serial monitor
  Serial.begin(115200);
  // wait for 2000ms
  for (static uint8_t i = 0; i < 10; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
  Serial.println();

  WiFi.onEvent(
      [](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.println(">=====>");
        Serial.println("ESP connected to an Access Point.");
        Serial.print("SSID: ");
        Serial.println(WiFi.SSID());
        Serial.print("Local IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("Gateway: ");
        Serial.println(WiFi.gatewayIP());
        Serial.print("Subnet: ");
        Serial.println(WiFi.subnetMask());
        Serial.print("Dns: ");
        Serial.println(WiFi.dnsIP());
        Serial.println("<=====<");
      },
      WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_CONNECTED);

  WiFi.onEvent(
      [](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.println(">=====>");
        Serial.println("ESP disconnected from an Access Point.");
        Serial.println("<=====<");
      },
      WiFiEvent_t::ARDUINO_EVENT_WIFI_STA_DISCONNECTED);

  WiFi.onEvent(
      [](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.println(">=====>");
        Serial.println("STA connected to ESP Access Point.");
        Serial.print("MAC: ");
        Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X",
                      info.wifi_ap_staconnected.mac[0],
                      info.wifi_ap_staconnected.mac[1],
                      info.wifi_ap_staconnected.mac[2],
                      info.wifi_ap_staconnected.mac[3],
                      info.wifi_ap_staconnected.mac[4],
                      info.wifi_ap_staconnected.mac[5]);
        Serial.println();
        Serial.println("<=====<");
      },
      WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STACONNECTED);

  WiFi.onEvent(
      [](WiFiEvent_t event, WiFiEventInfo_t info) {
        Serial.println(">=====>");
        Serial.println("STA disconnected from ESP Access Point.");
        Serial.print("MAC: ");
        Serial.printf("%02X:%02X:%02X:%02X:%02X:%02X",
                      info.wifi_ap_staconnected.mac[0],
                      info.wifi_ap_staconnected.mac[1],
                      info.wifi_ap_staconnected.mac[2],
                      info.wifi_ap_staconnected.mac[3],
                      info.wifi_ap_staconnected.mac[4],
                      info.wifi_ap_staconnected.mac[5]);
        Serial.println();
        Serial.println("<=====<");
      },
      WiFiEvent_t::ARDUINO_EVENT_WIFI_AP_STADISCONNECTED);

  // Configure WiFi
  Serial.println(">=====>");
  Serial.println("ESP WiFi is being configured (WIFI_AP_STA)...");
  static bool isWiFiConfigured = WiFi.mode(WIFI_AP_STA);

  if (isWiFiConfigured) {
    Serial.println("ESP WiFi successfully configured.");
  } else {
    Serial.println("ESP WiFi failed to configure. Restarting...");
    // restart the esp if failed to configure WiFi
    ESP.restart();
  }
  Serial.println("<=====<");

  // Configure AP
  Serial.println(">=====>");
  Serial.println("ESP is creating an Access Point (AP_MODE).");
  static bool isAPStarted = WiFi.softAP(ap_ssid, ap_passphrase);
  if (isAPStarted) {
    Serial.println("ESP created Access point.");
    Serial.print("SSID: ");
    Serial.println(WiFi.softAPSSID());
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("ESP failed to create Access point. Restarting ESP...");
    // restart the esp if failed to create access point
    ESP.restart();
  }
  Serial.println("<=====<");

  // Configure STA
  Serial.println(">=====>");
  Serial.println("ESP is connecting to an Access Point (STA_MODE).");
  WiFi.begin(sta_ssid, sta_passphrase);
  // Wait for WiFi to connect for a maximum timeout of 5000ms
  for (static uint8_t i = 0; !WiFi.isConnected() && i < 5; i++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(500);
    digitalWrite(LED_BUILTIN, LOW);
    delay(500);
  }
  if (WiFi.isConnected()) {
    Serial.println("ESP successfully connected to an Access Point.");
  } else {
    Serial.println("ESP failed to connected to an Access Point.");
  }
  Serial.println("<=====<");

  server.addMiddleware([](AsyncWebServerRequest* request, ArMiddlewareNext next) {
    Serial.print("> ");
    Serial.print(request->methodToString());
    Serial.print(" ");
    Serial.println(request->url().c_str());

    next();
  });

  server.on("/", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "text/html", (uint8_t*)indexHtml, indexHtmlLength);
  });

  server.on("/rest/rgba", HTTP_GET, [](AsyncWebServerRequest* request) {
    AsyncJsonResponse* response = getRgbaJsonResponse();

    response->setLength();
    response->setCode(200);
    request->send(response);
  });

  AsyncCallbackJsonWebHandler* restRgbaJsonHandler =
      new AsyncCallbackJsonWebHandler("/rest/rgba", [](AsyncWebServerRequest* request, JsonVariant& json) {
        JsonObject body = json.as<JsonObject>();

        if (!body["red"].is<int>() || !body["green"].is<int>() || !body["blue"].is<int>() ||
            !body["alpha"].is<float>()) {
          request->send(400, "text/plain", "400: Bad Request [missing/invalid attribute(s) value type]");
          return;
        }

        int red = body["red"];
        int green = body["green"];
        int blue = body["blue"];
        float alpha = body["alpha"];

        if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0 || blue > 255 || alpha < 0.0 || alpha > 1.0) {
          request->send(400, "text/plain", "400: Bad Request [invalid attribute(s) value]");
          return;
        }

        rangeRed = red;
        rangeGreen = green;
        rangeBlue = blue;
        rangeAlpha = alpha;

        AsyncJsonResponse* response = getRgbaJsonResponse();

        response->setLength();
        response->setCode(200);
        request->send(response);
      });
  restRgbaJsonHandler->setMethod(HTTP_POST);

  server.addHandler(restRgbaJsonHandler);

  // blink the built-in led
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);
  delay(200);
  digitalWrite(LED_BUILTIN, HIGH);
  delay(200);
  digitalWrite(LED_BUILTIN, LOW);

  // start the web server
  Serial.println(">=====>");
  Serial.println("Starting server.");

  server.begin();

  Serial.println("Server is successfully started.");
  Serial.println("<=====<");
}

static uint32_t lastLed = 0;
static uint32_t lastWifi = 0;
static uint32_t lastBlink = 0;
static bool ledState = false;

void loop() {
  uint32_t now = millis();

  // update rgb leds after every 250ms
  if (lastLed == 0 || now - lastLed >= 250) {
    // built-in rgb led pins are active-low
    analogWrite(LED_RED, 255 - (int)round(rangeRed * rangeAlpha));
    analogWrite(LED_GREEN, 255 - (int)round(rangeGreen * rangeAlpha));
    analogWrite(LED_BLUE, 255 - (int)round(rangeBlue * rangeAlpha));

    lastLed = millis();
  }

  // try reconnecting to AP every 10000ms
  if (!WiFi.isConnected()) {
    if (lastWifi == 0 || now - lastWifi >= 10000) {
      Serial.println(">=====>");
      Serial.println("ESP is retrying to connect to an Access Point (STA_MODE).");

      WiFi.reconnect();

      Serial.println("<=====<");

      lastWifi = millis();
    }

    // blink built-in led after every 500ms
    if (now - lastBlink >= 500) {
      ledState = !ledState;
      digitalWrite(LED_BUILTIN, ledState ? HIGH : LOW);

      lastBlink = millis();
    }
  } else if (ledState) {
    ledState = false;
    digitalWrite(LED_BUILTIN, LOW);
  }
}
