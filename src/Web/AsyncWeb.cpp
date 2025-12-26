#include "AsyncWeb.h"

/*
 The MIT License (MIT)

 This file is part of the JoyStick Project (https://github.com/bobboteck/JoyStick).
 Copyright (c) 2015 Roberto D'Amico (Bobboteck).

 Permission is hereby granted, free of charge, to any person obtaining a copy
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 
 The above copyright notice and this permission notice shall be included in all
 copies or substantial portions of the Software.

 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 SOFTWARE.
*/


AsyncWeb::AsyncWeb()
    : server(port), ws(wsPath) {
}

void AsyncWeb::begin(AwsEventHandler handler) {
    server.addHandler(&ws);
    ws.onEvent(handler);
    setupServerRoutes(server);
    server.begin();
}

void AsyncWeb::setupServerRoutes(AsyncWebServer &server) {
    server.on("/", HTTP_GET, [](AsyncWebServerRequest *request){
        String html = R"rawliteral(
            <!DOCTYPE HTML>
<html>
	<head>
		<title>Joy</title>
		<meta charset="utf-8">
		<meta name="description" content="Example page of use pure Javascript JoyStick">
		<meta name="author" content="Roberto D'Amico">
		<link rel="shortcut icon" type="image/png" href="http://bobboteck.github.io/img/roberto-damico-bobboteck.png">
		<style>
			*
			{
				box-sizing: border-box;
			}
			body
			{
				margin: 0px;
				padding: 0px;
				font-family: monospace;
				display: flex;
				justify-content: center;
				align-items: center;
				height: 100vh;
			}
			.center-container {
				display: flex;
				flex-direction: column;
				align-items: center;
				justify-content: center;
				width: 100%;
			}
			#joy1Div {
				border: 1px solid #FF0000;
			}
			.info {
				text-align: right;
				font-size: 24px;
			}
			.volume {
				text-align: left;
				font-size: 24px;
			}
			
		</style>
		<script>
			let StickStatus={xPosition:0,yPosition:0,x:0,y:0,cardinalDirection:"C"};var JoyStick=function(t,e,i){var o=void 0===(e=e||{}).title?"joystick":e.title,n=void 0===e.width?0:e.width,a=void 0===e.height?0:e.height,r=void 0===e.internalFillColor?"#00AA00":e.internalFillColor,c=void 0===e.internalLineWidth?2:e.internalLineWidth,s=void 0===e.internalStrokeColor?"#003300":e.internalStrokeColor,d=void 0===e.externalLineWidth?2:e.externalLineWidth,u=void 0===e.externalStrokeColor?"#008000":e.externalStrokeColor,h=void 0===e.autoReturnToCenter||e.autoReturnToCenter;i=i||function(t){};var S=document.getElementById(t);S.style.touchAction="none";var f=document.createElement("canvas");f.id=o,0===n&&(n=S.clientWidth),0===a&&(a=S.clientHeight),f.width=n,f.height=a,S.appendChild(f);var l=f.getContext("2d"),k=0,g=2*Math.PI,x=(f.width-(f.width/2+10))/2,v=x+5,P=x+30,m=f.width/2,C=f.height/2,p=f.width/10,y=-1*p,w=f.height/10,L=-1*w,F=m,E=C;function W(){l.beginPath(),l.arc(m,C,P,0,g,!1),l.lineWidth=d,l.strokeStyle=u,l.stroke()}function T(){l.beginPath(),F<x&&(F=v),F+x>f.width&&(F=f.width-v),E<x&&(E=v),E+x>f.height&&(E=f.height-v),l.arc(F,E,x,0,g,!1);var t=l.createRadialGradient(m,C,5,m,C,200);t.addColorStop(0,r),t.addColorStop(1,s),l.fillStyle=t,l.fill(),l.lineWidth=c,l.strokeStyle=s,l.stroke()}function D(){let t="",e=F-m,i=E-C;return i>=L&&i<=w&&(t="C"),i<L&&(t="N"),i>w&&(t="S"),e<y&&("C"===t?t="W":t+="W"),e>p&&("C"===t?t="E":t+="E"),t}"ontouchstart"in document.documentElement?(f.addEventListener("touchstart",function(t){k=1},!1),document.addEventListener("touchmove",function(t){1===k&&t.targetTouches[0].target===f&&(F=t.targetTouches[0].pageX,E=t.targetTouches[0].pageY,"BODY"===f.offsetParent.tagName.toUpperCase()?(F-=f.offsetLeft,E-=f.offsetTop):(F-=f.offsetParent.offsetLeft,E-=f.offsetParent.offsetTop),l.clearRect(0,0,f.width,f.height),W(),T(),StickStatus.xPosition=F,StickStatus.yPosition=E,StickStatus.x=((F-m)/v*100).toFixed(),StickStatus.y=((E-C)/v*100*-1).toFixed(),StickStatus.cardinalDirection=D(),i(StickStatus))},!1),document.addEventListener("touchend",function(t){k=0,h&&(F=m,E=C);l.clearRect(0,0,f.width,f.height),W(),T(),StickStatus.xPosition=F,StickStatus.yPosition=E,StickStatus.x=((F-m)/v*100).toFixed(),StickStatus.y=((E-C)/v*100*-1).toFixed(),StickStatus.cardinalDirection=D(),i(StickStatus)},!1)):(f.addEventListener("mousedown",function(t){k=1},!1),document.addEventListener("mousemove",function(t){1===k&&(F=t.pageX,E=t.pageY,"BODY"===f.offsetParent.tagName.toUpperCase()?(F-=f.offsetLeft,E-=f.offsetTop):(F-=f.offsetParent.offsetLeft,E-=f.offsetParent.offsetTop),l.clearRect(0,0,f.width,f.height),W(),T(),StickStatus.xPosition=F,StickStatus.yPosition=E,StickStatus.x=((F-m)/v*100).toFixed(),StickStatus.y=((E-C)/v*100*-1).toFixed(),StickStatus.cardinalDirection=D(),i(StickStatus))},!1),document.addEventListener("mouseup",function(t){k=0,h&&(F=m,E=C);l.clearRect(0,0,f.width,f.height),W(),T(),StickStatus.xPosition=F,StickStatus.yPosition=E,StickStatus.x=((F-m)/v*100).toFixed(),StickStatus.y=((E-C)/v*100*-1).toFixed(),StickStatus.cardinalDirection=D(),i(StickStatus)},!1)),W(),T(),this.GetWidth=function(){return f.width},this.GetHeight=function(){return f.height},this.GetPosX=function(){return F},this.GetPosY=function(){return E},this.GetX=function(){return((F-m)/v*100).toFixed()},this.GetY=function(){return((E-C)/v*100*-1).toFixed()},this.GetDir=function(){return D()}};
		</script>
	</head>
	<body>
		<div class="center-container">
			<div class="info">
				<p>バッテリ電圧: <span id='vbatt'>--</span> V</p>
			</div>
			<div id="joy1Div" style="width:900px;height:900px;margin:0 auto;"></div>
			<div class="volume">
				<p>Direzione: <span id="joy1Direzione">C</span></p>
				<p>X: <span id="joy1X">0</span></p>
				<p>Y: <span id="joy1Y">0</span></p>
				<!-- <p>ボリューム: <input type='range' min='0' max='100' value='50' id='volume'> <span id='val'>50</span></p> -->
			</div>
		</div>
		<script type="text/javascript">
			let ws = new WebSocket('ws://' + location.host + '/ws');
			ws.onmessage = function(event) {
				let data = JSON.parse(event.data);
				if(data.vbatt !== undefined) {
					document.getElementById('vbatt').innerText = data.vbatt.toFixed(2);
				}
			};

			var joy1Direzione = document.getElementById("joy1Direzione");
			var joy1X = document.getElementById("joy1X");
			var joy1Y = document.getElementById("joy1Y");

            let latestJoyData = {
                joyx: 0,
                joyy: 0,
                joydir: "C"
            };
            let intervalId = null;
            let stopTimeoutId = null;

            function startIntervalSend() {
                if (intervalId === null) {
                    intervalId = setInterval(function() {
                        if (ws.readyState === WebSocket.OPEN) {
                            ws.send(JSON.stringify({ joydata: latestJoyData }));
                        }
                    }, 200); // 200msごとに送信
                }
                // 1秒間操作がなければ送信停止
                if (stopTimeoutId !== null) clearTimeout(stopTimeoutId);
                stopTimeoutId = setTimeout(function() {
                    clearInterval(intervalId);
                    intervalId = null;
                }, 1000);
            }
            
			var Joy1 = new JoyStick('joy1Div', {}, function(stickData) {
				joy1Direzione.textContent = stickData.cardinalDirection;
				joy1X.textContent = stickData.x;
				joy1Y.textContent = stickData.y;
                latestJoyData = {
                    joyx: parseInt(stickData.x, 10),
                    joyy: parseInt(stickData.y, 10),
                    joydir: stickData.cardinalDirection
                };
                startIntervalSend();
				// ws.send(JSON.stringify({
				// 	joydata: {
				// 		joyx: parseInt(stickData.x, 10),
				// 		joyy: parseInt(stickData.y, 10),
				// 		joydir: stickData.cardinalDirection
				// 	}
				// }));
			});
		</script>
	</body>
</html>
        )rawliteral";
        request->send(200, "text/html", html);
    });
}

void AsyncWeb::notifyClients(float vbatt){
    JsonDocument doc;
    doc["vbatt"] = vbatt;
    String msg;
    serializeJson(doc, msg);
    ws.textAll(msg);  // 全クライアントに送信
}