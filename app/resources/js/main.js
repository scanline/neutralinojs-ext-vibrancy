Neutralino.init();

document.getElementById("info").innerHTML = `
        ${NL_APPID} is running on port ${NL_PORT} inside ${NL_OS}
        <br/><br/>
        <span>server: v${NL_VERSION} . client: v${NL_CVERSION}</span>
        `;

Neutralino.events.on("eventFromExtension", (evt) => {
	console.log(`INFO: ${evt.detail}`);
});

document.getElementById("close").addEventListener("click", (e) => {
	Neutralino.app.exit();
});

document.getElementById("toggleVibrancy").addEventListener("click", (e) => {
	if (e.target.innerText == "Vibrancy on") {
		e.target.innerText = "Vibrancy off";
		vibrancy(true);
	} else {
		e.target.innerText = "Vibrancy on";
		vibrancy(false);
	}
});

function vibrancy(activate, pid = null) {
	Neutralino.extensions.dispatch("extVibrancy", "vibrancy", {
		activate: activate,
		pid: pid == null ? NL_PID : pid
	});
}