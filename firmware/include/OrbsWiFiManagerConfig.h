#ifndef ORBS_WIFI_MANAGER_CONFIG_H
#define ORBS_WIFI_MANAGER_CONFIG_H

#include <Arduino.h>

// If we want to override all strings from WiFiManager WebPortal, we can copy the file wm_strings_en.h to our include directory,
// give it a proper name and include it here
// #define WM_STRINGS_FILE "orbs_wifimanager_strings.h" // use this instead of wm_strings_en.h

const char WEBPORTAL_PARAM_PAGE_START[] = "<h1>InfoOrbs Configuration</h1>";
const char WEBPORTAL_PARAM_PAGE_END[] = "<br>"
                                        "<h3 style='color: red;'>Saving will restart the InfoOrbs to apply the new config.</h3>";
const char WEBPORTAL_PARAM_FIELDSET_START[] = "<fieldset>";
const char WEBPORTAL_PARAM_FIELDSET_END[] = "</fieldset>";
const char WEBPORTAL_PARAM_LEGEND_START[] = "<legend>";
const char WEBPORTAL_PARAM_LEGEND_END[] = "</legend>";
const char WEBPORTAL_PARAM_DIV_START[] = "<div class='param'>";
const char WEBPORTAL_PARAM_DIV_STRING_START[] = "<div class='param-string'>";
const char WEBPORTAL_PARAM_DIV_END[] = "</div>";
const char WEBPORTAL_PARAM_TOGGLE_ADVANCED[] = "<div class='toggle-adv'>Show Advanced Parameters</div>";
const char WEBPORTAL_PARAM_SPAN_ADVANCED_START[] = "<span class='adv-content'>";
const char WEBPORTAL_PARAM_SPAN_END[] = "</span>";

// Style for /param
const char WEBPORTAL_PARAM_STYLE[] = R"(
<style>
    .param { display: flex; align-items: center; margin: 0; }
    .param label { flex: 0 0 70%; text-align: left; }
    .param input { flex: 0 0 30%; text-align: right; }
    .param-string { margin: 0; }
    fieldset { margin: 10px 0; }
    legend { font-size: 1.5rem; font-weight: bold; }
    .toggle-adv {cursor: pointer; text-decoration: underline; }
    .toggle-adv:hover { font-weight: bold; }
    .adv-content { display:none; }
    .adv-content.open { display:block; }
</style>
)";

// JS for /param
const char WEBPORTAL_PARAM_SCRIPT[] = R"(
<script>
    document.querySelectorAll('.toggle-adv').forEach(toggle => {
        toggle.addEventListener('click', () => {
            const advContent = toggle.nextElementSibling;
            if (advContent.classList.contains('adv-content')) {
                advContent.classList.toggle('open');
                toggle.textContent = advContent.classList.contains('open') ? 'Hide Advanced Parameters' : 'Show Advanced Parameters';
            }
        });
    });
</script>
)";

// Start of /buttons
const char WEBPORTAL_BUTTONS_PAGE_START1[] = "<html>"
                                             "<head>";

const char WEBPORTAL_BUTTONS_PAGE_START2[] = R"(
</head>
<body>
    <div style='display: inline-block; min-width: 260px; max-width: 500px;padding: 5px;'>
    <h1>InfoOrbs Buttons</h1>
    <div class='info'>
        Here you can simulate button presses on the Orbs.
    </div>
    <div class='info'>
        The device supports 3 different events per button:<br>
        short (&lt;500ms), medium (500-2500ms) and long (&gt;2500ms)
    </div>
    <table>
)";

// End of /buttons
const char WEBPORTAL_BUTTONS_PAGE_END1[] = R"(
</table>
<form action='/' method='get'>
    <br><button class='back'>Back</button>
</form>
</div>
)";

const char WEBPORTAL_BUTTONS_PAGE_END2[] = "</body>"
                                           "</html>";

// Style for /buttons
const char WEBPORTAL_BUTTONS_STYLE[] = R"(
<style>
    body { background: #060606; text-align: center; color: #fff; font-family: verdana; }
    .info { margin: 10px 0; }
    table { text-align: center; width: 100%; }
    td { padding: 5px; }
    button.sim { height: 50px; width: 140px; border-radius: .3rem; }
    button.back { border-radius: .3rem; width: 100%; border: 0; background-color: #1fa3ec; color: #fff; line-height: 2.4rem; font-size: 1.2rem; }
</style>
)";

// JS for /buttons
const char WEBPORTAL_BUTTONS_SCRIPT[] = R"(
<script>
    function sendReq(name, state) {
        fetch(`/button?name=${name}&state=${state}`);
    }
</script>
)";

#endif // ORBS_WIFI_MANAGER_CONFIG_H