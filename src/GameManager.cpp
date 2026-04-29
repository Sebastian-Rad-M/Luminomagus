#include <RmlUi/Core.h>
#include <raylib.h>

#include "BossFactory.h"
#include "CombatUI.h"
#include "DraftMenuUI.h"
#include "GameManager.h"
#include "GameState.h"
#include "MainMenuUI.h"
#include "RaylibInput.h"
#include "RaylibRenderInterface.h"
#include "RaylibSystemInterface.h"
#include "RelicFactory.h"
#include "ShopUI.h"

// debug
#include <RmlUi/Debugger.h>

GameManager& GameManager::instance() {
	static GameManager inst;
	return inst;
}

void GameManager::run() {
	// RAYLIB
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(1280, 720, "Luminomagus");
	MaximizeWindow();
	SetTargetFPS(60);
	SetExitKey(0);

	// SHADRES
	Shader voidShader = LoadShader(nullptr, "assets/Void.fs");
	int timeLoc = GetShaderLocation(voidShader, "u_time");
	int resLoc = GetShaderLocation(voidShader, "u_resolution");

	Shader smokeShader = LoadShader(nullptr, "assets/SubtleSmoke.fs");
	int smokeTimeLoc = GetShaderLocation(smokeShader, "u_time");
	Texture2D combatBgTexture = LoadTexture("assets/combat_bg.png");

	// RMLUI
	RaylibRenderInterface renderInterface;
	RaylibSystemInterface systemInterface;
	Rml::SetRenderInterface(renderInterface.GetAdaptedInterface());
	Rml::SetSystemInterface(&systemInterface);
	Rml::Initialise();
	Rml::Context* context =
		Rml::CreateContext("main", Rml::Vector2i(GetScreenWidth(), GetScreenHeight()));
	if (!context) {
		CloseWindow();
		return;
	}
	Rml::LoadFontFace("assets/ARIAL.ttf");
	Rml::LoadFontFace("assets/CinzelDecorative-Regular.ttf");
	// debug
	Rml::Debugger::Initialise(context);
	Rml::Debugger::SetVisible(true);

	Rml::ElementDocument* docMainMenu = context->LoadDocument("assets/mainMenu.rml");
	Rml::ElementDocument* docDraft = context->LoadDocument("assets/draft.rml");
	Rml::ElementDocument* docCombat = context->LoadDocument("assets/combat.rml");
	Rml::ElementDocument* docShop = context->LoadDocument("assets/shop.rml");
	Rml::ElementDocument* docPostRun = context->LoadDocument("assets/postRun.rml");

	MainMenuListener menuListener;
	DraftListener draftListener;
	CombatListener combatListener;
	ShopListener shopListener;

	if (docMainMenu) {
		if (auto el = docMainMenu->GetElementById("btn-new-run")) {
			el->AddEventListener(Rml::EventId::Click, &menuListener);
		}
		if (auto el = docMainMenu->GetElementById("btn-quit")) {
			el->AddEventListener(Rml::EventId::Click, &menuListener);
		}
		docMainMenu->Show();
	}

	if (docPostRun) {
		if (auto el = docPostRun->GetElementById("btn-post-restart")) {
			el->AddEventListener(Rml::EventId::Click, &menuListener);
		}
		if (auto el = docPostRun->GetElementById("btn-post-menu")) {
			el->AddEventListener(Rml::EventId::Click, &menuListener);
		}
	}

	if (docShop) {
		if (auto el = docShop->GetElementById("btn-leave-shop")) {
			el->AddEventListener(Rml::EventId::Click, &shopListener);
		}
	}
	GameState previousState = state;
	context->SetDimensions(Rml::Vector2i(GetScreenWidth(), GetScreenHeight()));

	// ===== MAIN FRAME LOOP =====
	while (!WindowShouldClose()) {
		Rml::Vector2i rmlDimensions = context->GetDimensions();
		if (rmlDimensions.x != GetScreenWidth() || rmlDimensions.y != GetScreenHeight()) {
			context->SetDimensions(Rml::Vector2i(GetScreenWidth(), GetScreenHeight()));
		}

		if (IsKeyPressed(KEY_ESCAPE)) {
			state = GameState::MAIN_MENU;
		}
		GameState currentState = state;

		if (state == GameState::QUIT_APP) {
			std::cout << "me trying to quit :(\n";
			break;
		}

		if (currentState != previousState) {
			if (previousState == GameState::MAIN_MENU && docMainMenu) {
				docMainMenu->Hide();
			}
			if (previousState == GameState::DRAFT && docDraft) {
				docDraft->Hide();
			}
			if (previousState == GameState::COMBAT && docCombat) {
				docCombat->Hide();
			}
			if (previousState == GameState::SHOP && docShop) {
				docShop->Hide();
			}
			if (previousState == GameState::POST_RUN && docPostRun) {
				docPostRun->Hide();
			}

			if (previousState == GameState::DRAFT && currentState == GameState::COMBAT) {
				// debug relic
				if (activeRun.getPlayer().getRelicZone().getRelicZone().empty()) {
					activeRun.getPlayer().getRelicZone().addRelic(
						RelicDatabase::getInstance().getRandomRareOrLegendaryRelic());
				}
				round.emplace(activeRun);
				round->setupDeck(activeRun.getPlayer().getDeck(),
								 activeRun.getPlayer().getRelicZone());
				round->startNewRound();
			}
			if (previousState == GameState::SHOP && currentState == GameState::COMBAT) {
				round.emplace(activeRun);
				round->setupDeck(activeRun.getPlayer().getDeck(),
								 activeRun.getPlayer().getRelicZone());
				int cr = activeRun.getCurrentRound();
				if (cr == 3 || cr == 6 || cr == 9) {
					round->addStatus(BossFactory::getRandomBoss('R'));
				}
				if (cr == 10) {
					round->addStatus(BossFactory::getRandomBoss('E'));
				}
				round->startNewRound();
			}
			if (currentState == GameState::MAIN_MENU && docMainMenu) {
				docMainMenu->Show();
			} else if (currentState == GameState::DRAFT && docDraft) {
				draftListener.startDraft(docDraft);
				docDraft->Show();
			} else if (currentState == GameState::COMBAT && docCombat) {
				combatListener.startCombat(docCombat);
				docCombat->Show();
			} else if (currentState == GameState::SHOP && docShop) {
				shopListener.startShop(docShop);
				docShop->Show();
			} else if (currentState == GameState::POST_RUN && docPostRun) {
				if (auto title = docPostRun->GetElementById("post-run-title")) {
					if (playerWon) {
						title->SetInnerRML("Victory!");
						title->SetClass("win", true);
						title->SetClass("lose", false);
					} else {
						title->SetInnerRML("Game Over!");
						title->SetClass("lose", true);
						title->SetClass("win", false);
					}
				}
				docPostRun->Show();
			}
			// Q should this not be a break?
			else if (currentState == GameState::GAME_OVER) {
				break;
			}

			previousState = currentState;
		}

		UpdateRmlInput(context);
		context->Update();

		BeginDrawing();
		ClearBackground(BLACK);

		auto time = (float)GetTime();
		float resolution[2] = {(float)GetScreenWidth(), (float)GetScreenHeight()};

		// --- BRANCH RENDERING BASED ON GAME STATE ---
		if (currentState == GameState::COMBAT) {
			// Draw the Combat Arena with Subtle Smoke
			SetShaderValue(smokeShader, smokeTimeLoc, &time, SHADER_UNIFORM_FLOAT);

			BeginShaderMode(smokeShader);
			Rectangle sourceRec = {0.0f, 0.0f, (float)combatBgTexture.width,
								   (float)combatBgTexture.height};
			Rectangle destRec = {0.0f, 0.0f, (float)GetScreenWidth(), (float)GetScreenHeight()};
			Vector2 origin = {0.0f, 0.0f};

			DrawTexturePro(combatBgTexture, sourceRec, destRec, origin, 0.0f, WHITE);
			EndShaderMode();
		} else {
			SetShaderValue(voidShader, timeLoc, &time, SHADER_UNIFORM_FLOAT);
			SetShaderValue(voidShader, resLoc, resolution, SHADER_UNIFORM_VEC2);
			BeginShaderMode(voidShader);
			DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), WHITE);
			EndShaderMode();
		}
		context->Render();

		EndDrawing();
	}

	Rml::Shutdown();
	CloseWindow();
}
