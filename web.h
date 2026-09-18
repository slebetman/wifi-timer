#ifndef WEB
#define WEB

#include "platform.h"
#if defined(WEMOS_D1_MINI)
#include <ESP8266WebServer.h>
#elif defined(ESP32_C3)
#include <WebServer.h>
#endif
#include "vars.h"
#include "button.h"

extern Button button;
extern Button cancelButton;

class Web
{
private:
#if defined(WEMOS_D1_MINI)
	ESP8266WebServer server;
#elif defined(ESP32_C3)
	WebServer server;
#endif

	// Function to handle the root URL (/)
	void handleRoot()
	{
		Serial.println("handling request ..");
		Vars *v = readVars();

		String html = R"(<!DOCTYPE html><html>
				<head>
					<meta name="viewport" content="width=device-width, initial-scale=1.0">
					<style>
						* {
							font-size: 1rem;
							font-family: sans-serif;
						}
						html {
							display: flex;
							justify-content: center;
						}
						body {
							display: inline-block;
							margin-top: 50px;
							color: #333;
							background-color: #e0e0e0;
						}
						h1 {
							color: #007bff;
						}
						div.form {
							display: flex;
							flex-direction: column;
						}
						p {
							margin: 8px 0;
						}
						label {
							display: inline-block;
							width: 8em;
							text-align: right;
							margin-right: 1em;
						}
						input {
							background-color: #fff;
						}
						input[type="text"] {
							width: 4em;
							padding: 2px 6px;
						}
						button {
							border-radius: 10px;
							padding: 5px 10px;
							background-color: #007bff;
							color: #fff;
							border: 0px;
							margin-top: 5px;
							margin-bottom: 25px;
						}
					</style>
				</head>
				<body>
					<h1>Slebetman's Wifi Timer</h1>
					<div class="form">
						<button id="clickBtn">Start/Stop Timer</button>
					</div>
					<script>
						function get (id) {
							return document.getElementById(id);
						}
						get('clickBtn').onclick = function (e) {
							e.preventDefault();
							window.location.href = "/click";
						}
					</script>
					<div class="form">
			)";

		html += "<p><label>Max Power: </label>";
		html += "<input type=\"range\" id=\"max\" name=\"max\" min=\"0\" max=\"255\" step=\"5\" value=\"" +
				String(v->max) + "\"></p>\n";

		html += "<p><label>Start Delay: </label>";
		html += "<input type=\"text\" id=\"startDelay\" name=\"startDelay\" value=\"" +
				String(v->startDelay) + "\"> seconds</p>\n";

		html += "<p><label>Ramp Up: </label>";
		html += "<input type=\"text\" id=\"rampUp\" name=\"rampUp\" value=\"" +
				String(v->rampUp) + "\"> seconds</p>\n";

		html += "<p><label>Cruise: </label>";
		html += "<input type=\"text\" id=\"cruise\" name=\"cruise\" value=\"" +
				String(v->cruise) + "\"> seconds</p>\n";

		html += "<p><label>Ramp Down: </label>";
		html += "<input type=\"text\" id=\"rampDown\" name=\"rampDown\" value=\"" +
				String(v->rampDown) + "\"> seconds</p>\n";

		html += R"(<button id="updateBtn">Update Settings</button>
					</div>
					<script>
						function val (id) {
							return `${id}=${get(id).value}`;
						}
						get('updateBtn').onclick = function (e) {
							e.preventDefault();

							window.location.href = '/update?' +
								val('max') + '&' +
								val('startDelay') + '&' +
								val('rampUp') + '&' +
								val('cruise') + '&' +
								val('rampDown');
						}
					</script>
				</body>
			</html>)";

		server.send(200, "text/html", html);
	}

	String quickMessage (String className, String message) {
		String html = R"(<!DOCTYPE html><html>
			<head>
				<meta name="viewport" content="width=device-width, initial-scale=1.0">
				<style>
					* {
						font-size: 1rem;
						font-family: sans-serif;
					}
					html {
						display: flex;
						justify-content: center;
					}
					body {
						display: flex;
						flex-direction: column;
						align-items: center;
						margin-top: 50px;
						color: #333;
						background-color: #e0e0e0;
					}
					p {
						margin: 8px 0;
					}
					button {
						border-radius: 10px;
						padding: 5px 10px;
						background-color: #007bff;
						color: #fff;
						border: 0px;
						margin-top: 5px;
						margin-bottom: 25px;
					}
					.error {
						color: #f33;
					}
				</style>
			</head>
			<body>
				<p>
					<b class=")" +
					  className + R"(">)" +
					  message + R"(</b>
				</p>)";
		if (className.equals("error")) {
			html += R"(
				<button id="backBtn">OK</button>
				<script>
					document.getElementById('backBtn').onclick = () => {
						window.location.href = "/";
					}
				</script>
			)";
		}
		else {
			html +=	R"(<script>
				setTimeout(() => {
					window.location.href = "/";
				}, 1000)
			</script>)";
		}
		html += "</body></html>";

		return html;
	}

	void handleUpdate()
	{
		if (button.click())
		{
			server.send(400, "text/html", quickMessage("error", "Error: Cannot modify settings while timer is running"));
		}
		else if (
			server.hasArg("max") &&
			server.hasArg("startDelay") &&
			server.hasArg("rampUp") &&
			server.hasArg("cruise") &&
			server.hasArg("rampDown"))
		{
			String sMax = server.arg("max");
			String sDelay = server.arg("startDelay");
			String sUp = server.arg("rampUp");
			String sCruise = server.arg("cruise");
			String sDown = server.arg("rampDown");
			Serial.println("handling post request ..");

			vars.max = sMax.toFloat();
			vars.startDelay = sDelay.toFloat();
			vars.rampUp = sUp.toFloat();
			vars.cruise = sCruise.toFloat();
			vars.rampDown = sDown.toFloat();

			saveVars();

			server.send(200, "text/html", quickMessage("info", "Updated!"));
		}
		else
		{
			server.send(400, "text/html", quickMessage("error", "Error: Missing parameters"));
		}
	}

	void handleButtonClick()
	{
		String message;

		if (button.click())
		{
			message = "Stopped.";
			cancelButton.softwareClick();
		}
		else
		{
			message = "Started.";
			button.softwareClick();
		}

		server.send(200, "text/html", quickMessage("info", message));
	}

	// Function to handle 404 Not Found errors
	void handleNotFound()
	{
		server.send(404, "text/plain", "404: Not Found");
	}

public:
	Web(int port) : server(port) {}

	void init()
	{
		// Define routing paths
		server.on("/", std::bind(&Web::handleRoot, this));
		server.on("/update", std::bind(&Web::handleUpdate, this));
		server.on("/click", std::bind(&Web::handleButtonClick, this));
		server.onNotFound(std::bind(&Web::handleNotFound, this));

		// Start the server
		server.begin();
		Serial.println("HTTP server started");
	}

	void run()
	{
		// Listen for incoming client requests
		server.handleClient();
	}
};

#endif