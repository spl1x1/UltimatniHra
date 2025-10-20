#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#include <RmlUi_Backend.h>

static const char* document_rml = R"(
<rml>
<head>
<style>
    body {
        font-family: rmlui-debugger-font;
        font-size: 12px;
        top: 100px; right: 100px; bottom: 100px; left: 100px;
        padding: 150px;
        text-align: center;
        background: #333;
        color: white;
    }
    p {
        display: block;
        padding: 30px;
    }
    p:hover {
        background: #444;
    }
</style>
</head>
<body>
    <p>Hello world!</p>
</body>
</rml>
)";


int main() {
	Backend::Initialize("Znake", 1024, 768, true);

	Rml::SetSystemInterface(Backend::GetSystemInterface());
	Rml::SetRenderInterface(Backend::GetRenderInterface());
	Rml::Initialise();

	Rml::Context *context = Rml::CreateContext("Znake", {1024, 768});

	Rml::Debugger::Initialise(context);
	Rml::Debugger::SetVisible(true);

	Rml::ElementDocument *document = context->LoadDocumentFromMemory(document_rml);

	document->Show();

	while (Backend::ProcessEvents(context)) {
		context->Update();
		Backend::BeginFrame();
		context->Render();
		Backend::PresentFrame();
	}

	Rml::Shutdown();
	Backend::Shutdown();

	return 0;
}