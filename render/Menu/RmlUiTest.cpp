#include <RmlUi/Core.h>
#include <RmlUi/Debugger.h>
#include <RmlUi_Backend.h>
#include <RmlUi/Core/Stream.h>
#include <FileSystem>
#include <iostream>


int main() {
	Backend::Initialize("Znake", 1024, 768, true);

	Rml::SetSystemInterface(Backend::GetSystemInterface());
	Rml::SetRenderInterface(Backend::GetRenderInterface());
	Rml::Initialise();

	Rml::Context *context = Rml::CreateContext("Znake", {1024, 768});

	Rml::Debugger::Initialise(context);
	Rml::Debugger::SetVisible(true);

	Rml::ElementDocument *document = context->LoadDocument("assets/Menu/Mainmenu.rml");

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
