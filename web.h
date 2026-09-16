#include "platform.h"
#if defined(WEMOS_D1_MINI)
#include <ESP8266WebServer.h>
#elif defined(ESP32_C3)
#include <WebServer.h>
#endif
#include "vars.h"

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
						form {
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
						}
					</style>
				</head>
				<body>
					<h1>D1 Timer</h1>
					<form action="/update" method="POST">
			)";

		html += "<p><label>Max Power: </label><input type=\"range\" id=\"max\" name=\"max\" min=\"0\" max=\"255\" step=\"5\" value=\"" + String(v->max) + "\"></p>\n";
		html += "<p><label>Ramp Up: </label><input type=\"text\" id=\"rampUp\" name=\"rampUp\" value=\"" + String(v->rampUp) + "\"> seconds</p>\n";
		html += "<p><label>Cruise: </label><input type=\"text\" id=\"cruise\" name=\"cruise\" value=\"" + String(v->cruise) + "\"> seconds</p>\n";
		html += "<p><label>Ramp Down: </label><input type=\"text\" id=\"rampDown\" name=\"rampDown\" value=\"" + String(v->rampDown) + "\"> seconds</p>\n";

		html += R"(
					<button type="submit">Update Settings</button>
					</form>
				</body>
			</html>)";

		server.send(200, "text/html", html);
	}

	void handlePost()
	{
		if (
			server.hasArg("max") &&
			server.hasArg("rampUp") &&
			server.hasArg("cruise") &&
			server.hasArg("rampDown"))
		{
			String sMax = server.arg("max");
			String sUp = server.arg("rampUp");
			String sCruise = server.arg("cruise");
			String sDown = server.arg("rampDown");
			Serial.println("handling post request ..");

			vars.max = sMax.toFloat();
			vars.rampUp = sUp.toFloat();
			vars.cruise = sCruise.toFloat();
			vars.rampDown = sDown.toFloat();

			saveVars();

			// server.sendHeader("Location", "/", true);
			// server.send(302, "text/plain", "");

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
						p {
							margin: 8px 0;
						}
					</style>
				</head>
				<body>
					<p>
						<b>Updated!</b>
					</p>
					<script>
						setTimeout(() => {
							window.location.href = "/";
						}, 1000)
					</script>
				</body>
			</html>)";

			server.send(200, "text/html", html);
		}
		else
		{
			server.send(400, "text/plain", "Bad Request: Missing parameters");
		}
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
		server.on("/update", HTTP_POST, std::bind(&Web::handlePost, this));
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