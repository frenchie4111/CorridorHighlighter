#include "pch.h"
#include "CorridorHighlighter.h"
#include "RenderingTools/Objects/Line.h"
#include "RenderingTools/Objects/Frustum.h"


#define CVAR_PLUGIN_ENABLED ("ch_enabled")

BAKKESMOD_PLUGIN(CorridorHighlighter, "write a plugin description here", plugin_version, PLUGINTYPE_FREEPLAY)

std::shared_ptr<CVarManagerWrapper> _globalCvarManager;

void CorridorHighlighter::onLoad()
{
	_globalCvarManager = cvarManager;
	//LOG("Plugin loaded!");
	// !! Enable debug logging by setting DEBUG_LOG = true in logging.h !!
	//DEBUGLOG("CorridorHighlighter debug mode enabled");

	// LOG and DEBUGLOG use fmt format strings https://fmt.dev/latest/index.html
	//DEBUGLOG("1 = {}, 2 = {}, pi = {}, false != {}", "one", 2, 3.14, true);

	//cvarManager->registerNotifier("my_aweseome_notifier", [&](std::vector<std::string> args) {
	//	LOG("Hello notifier!");
	//}, "", 0);

	//auto cvar = cvarManager->registerCvar("template_cvar", "hello-cvar", "just a example of a cvar");
	//auto cvar2 = cvarManager->registerCvar("template_cvar2", "0", "just a example of a cvar with more settings", true, true, -10, true, 10 );

	//cvar.addOnValueChanged([this](std::string cvarName, CVarWrapper newCvar) {
	//	LOG("the cvar with name: {} changed", cvarName);
	//	LOG("the new value is: {}", newCvar.getStringValue());
	//});

	//cvar2.addOnValueChanged(std::bind(&CorridorHighlighter::YourPluginMethod, this, _1, _2));

	// enabled decleared in the header
	//enabled = std::make_shared<bool>(false);
	//cvarManager->registerCvar("TEMPLATE_Enabled", "0", "Enable the TEMPLATE plugin", true, true, 0, true, 1).bindTo(enabled);

	//cvarManager->registerNotifier("NOTIFIER", [this](std::vector<std::string> params){FUNCTION();}, "DESCRIPTION", PERMISSION_ALL);
	//cvarManager->registerCvar("CVAR", "DEFAULTVALUE", "DESCRIPTION", true, true, MINVAL, true, MAXVAL);//.bindTo(CVARVARIABLE);
	//gameWrapper->HookEvent("FUNCTIONNAME", std::bind(&TEMPLATE::FUNCTION, this));
	//gameWrapper->HookEventWithCallerPost<ActorWrapper>("FUNCTIONNAME", std::bind(&CorridorHighlighter::FUNCTION, this, _1, _2, _3));
	//gameWrapper->RegisterDrawable(bind(&TEMPLATE::Render, this, std::placeholders::_1));


	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", [this](std::string eventName) {
	//	LOG("Your hook got called and the ball went POOF");
	//});
	// You could also use std::bind here
	//gameWrapper->HookEvent("Function TAGame.Ball_TA.Explode", std::bind(&CorridorHighlighter::YourPluginMethod, this);
	enabled = std::make_shared<bool>(true);

	cvarManager->registerCvar(CVAR_PLUGIN_ENABLED, "0", "Plugin Enabled").bindTo(enabled);

	gameWrapper->HookEvent("Function TAGame.Mutator_Freeplay_TA.Init", bind(&CorridorHighlighter::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_Soccar_TA.Destroyed", bind(&CorridorHighlighter::OnFreeplayDestroy, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.StartPlayTest", bind(&CorridorHighlighter::OnFreeplayLoad, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.GameEvent_TrainingEditor_TA.Destroyed", bind(&CorridorHighlighter::OnFreeplayDestroy, this, std::placeholders::_1));

	gameWrapper->RegisterDrawable(std::bind(&CorridorHighlighter::RenderCorridor, this, std::placeholders::_1));
	gameWrapper->HookEvent("Function TAGame.Car_TA.SetVehicleInput", bind(&CorridorHighlighter::UpdateCorridor, this, std::placeholders::_1));
}

void CorridorHighlighter::OnFreeplayLoad(std::string eventName) {
}

void CorridorHighlighter::OnFreeplayDestroy(std::string eventName) {
}

void CorridorHighlighter::UpdateCorridor(std::string eventName) {
}

void CorridorHighlighter::RenderCorridor(CanvasWrapper canvas) {
	if (!*enabled.get()) {
		return;
	}

	if (!(gameWrapper->IsInCustomTraining() || gameWrapper->IsInFreeplay() || gameWrapper->IsInReplay())) {
		return;
	}

	ServerWrapper game = gameWrapper->GetGameEventAsServer();
	BallWrapper ball = game.GetBall();
	if (ball.IsNull()) //Ball might be destroyed, for example between scoring and goal replay
		return;

	Vector ballPosition = ball.GetLocation();

	int netPositionCount = 4;
	Vector netPositions1[4] = {
		{ 800, 5000, 0},
		{-800, 5000, 0},
		{ 800, 5000, 600},
		{-800, 5000, 600}
	};
	Vector netPositions2[4] = {
		{ 800, -5000, 0},
		{-800, -5000, 0},
		{ 800, -5000, 600},
		{-800, -5000, 600}
	};

	auto camera = gameWrapper->GetCamera();
	if (camera.IsNull()) return;
	RT::Frustum frust{ canvas, camera };

	canvas.SetColor(255, 0, 0, 150);
	for (int i = 0; i < netPositionCount; i++) {
		RT::Line(netPositions1[i], ballPosition, 3.0f).DrawWithinFrustum(canvas, frust);
	}

	canvas.SetColor(0, 255, 0, 150);
	for (int i = 0; i < netPositionCount; i++) {
		RT::Line(netPositions2[i], ballPosition, 3.0f).DrawWithinFrustum(canvas, frust);
	}

	//canvas.DrawLine(projectedBallPosition, projectedNetPosition, 3.0);
	//canvas.SetColor(0, 255, 0, 255);
	//canvas.DrawLine(projectedBallPosition, projectedNetPosition2, 3.0);

//	canvas.SetPosition(Vector2{ 200, 40 });
//	canvas.DrawString("Projected Ball Position: " + std::to_string(projectedBallPosition.X));
//	canvas.SetPosition(Vector2{ 200, 60 });
//	canvas.DrawString("Projected Ball Position: " + std::to_string(projectedBallPosition.Y));
//	canvas.SetPosition(Vector2{ 200, 80 });
//	canvas.DrawString("Projected Net Position: " + std::to_string(projectedNetPosition.X));
//	canvas.SetPosition(Vector2{ 200, 100 });
//	canvas.DrawString("Projected Net Position: " + std::to_string(projectedNetPosition.Y));
//	canvas.SetPosition(Vector2{ 200, 120 });
//	canvas.DrawString("Projected Net Position 2: " + std::to_string(projectedNetPosition2.X));
//	canvas.SetPosition(Vector2{ 200, 140 });
//	canvas.DrawString("Projected Net Position 2: " + std::to_string(projectedNetPosition2.Y));

}


void CorridorHighlighter::RenderSettings() {
	static auto pluginCvar = cvarManager->getCvar(CVAR_PLUGIN_ENABLED);
	auto pluginEnabled = pluginCvar.getBoolValue();
	if (ImGui::Checkbox("Enabled", &pluginEnabled)) {
		pluginCvar.setValue(pluginEnabled);
		cvarManager->executeCommand("writeconfig", false);
	}
}
