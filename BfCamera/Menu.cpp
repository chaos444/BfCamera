#define WIN32_LEAN_AND_MEAN

#include <windows.h>
#include <string>

// D3d Related Files
#include "ImGui/imgui.h"
#include "imgui_impl_dx11.h"
#include <DXGI.h>
#include <d3d11.h>
#include "Menu.h"
#include <d3dx9math.h>
#include "Hooking/d3d11hook.h"
#include "typeinfo/Util.h"
//#include "Camera.h"

using namespace ImGui;

void Menu::ToggleMenu() {
	this->IsOpen = !this->IsOpen;
	
}


Matrix4x4 g_OldMatrix = Matrix4x4(0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
bool g_EnableFreeCam = false;
bool g_EnableMouseCam = true;
bool g_BindUItoFreeCam = true;
std::vector<Matrix4x4> savedCameras;

std::vector<std::vector<Matrix4x4>> savedTracks;
std::vector<Matrix4x4> currentTrack;
bool isRecordingTrack = false;
bool isPlayingTrack = false;
int currentTrackFrame = 0;

// camera speed
float mainSpeed = 0.01f;
float slowSpeed = .001f;
float fastSpeed = 2.f;


void toggleFreeCam(bool value) {
	g_EnableFreeCam = value;
	UISettings* pUiSettings = *(UISettings**)(StaticOffsets::Get_OFFSET_GAMETIMESETTINGS() + 0x10);
	if (g_BindUItoFreeCam) {
		if (IsValidPtr(pUiSettings)) {
			pUiSettings->drawEnable = !g_EnableFreeCam;
		}
	}
}

void Menu::Render()
{
	ImGui_ImplDX11_NewFrame();
	UISettings* pUiSettings = *(UISettings**)(StaticOffsets::Get_OFFSET_GAMETIMESETTINGS() + 0x10);
	GameRenderer* pGameRenderer = *(GameRenderer**)StaticOffsets::Get_OFFSET_GAMERENDERER();
	if (IsValidPtr(pUiSettings)) {
		if (g_BindUItoFreeCam && !(pUiSettings->drawEnable ^ g_EnableFreeCam)) {
			pUiSettings->drawEnable = !g_EnableFreeCam;
		}
	}

	if (IsValidPtr(pGameRenderer) && IsValidPtr(pGameRenderer->renderView)) {

		if (KeyMan::ReadKeyOnce(Keys::enableFreeCam)) {
			if (!g_EnableFreeCam) {
				if (g_OldMatrix._41 == 0)
				g_OldMatrix = pGameRenderer->renderView->transform1;
			}
			toggleFreeCam(!g_EnableFreeCam);
			
		}

		Renderer::GetInstance()->BeginScene();
		ImGuiIO& io = ImGui::GetIO();

		try {
			if (KeyMan::ReadKeyOnce(Keys::saveCameraLocation)) {
				savedCameras.push_back(pGameRenderer->renderView->transform1);
			}

			if (KeyMan::ReadKeyOnce(Keys::recordCameraTrack)) {
				isRecordingTrack = !isRecordingTrack;
				if (!isRecordingTrack) {
					savedTracks.push_back(currentTrack);
					currentTrack.clear();
					currentTrackFrame = 0;
				}
			}

			if (isPlayingTrack) {
				g_EnableMouseCam = false;
				if (currentTrackFrame < currentTrack.size() + 1) {
					currentTrackFrame += 1;
					g_OldMatrix = currentTrack.at(currentTrackFrame);
				}
				else {
					currentTrackFrame = 0;
					currentTrack.clear();
					isPlayingTrack = false;
					g_EnableMouseCam = true;
				}
			}

			if (isRecordingTrack) {
				Renderer::GetInstance()->DrawCircle(ImVec2(20, 20), 15, D3DXCOLOR(255, 50, 50, 255), 3);
				currentTrack.push_back(pGameRenderer->renderView->transform1);
			}
		} catch (std::exception){}
		Renderer::GetInstance()->EndScene();

		// input
		float amount = mainSpeed;
		if (KeyMan::ReadKey(Keys::speedUpCamera)) amount = fastSpeed;
		if (KeyMan::ReadKey(Keys::slowDownCamera)) amount = slowSpeed;
		if (g_EnableFreeCam && !isPlayingTrack) {
			Vec3 origin = *(Vec3*)&pGameRenderer->renderView->transform1._41;
			Vec3 xVec = *(Vec3*)&pGameRenderer->renderView->transform1._11;
			Vec3 yVec = *(Vec3*)&pGameRenderer->renderView->transform1._21;
			Vec3 zVec = *(Vec3*)&pGameRenderer->renderView->transform1._31;
			if (KeyMan::ReadKey(Keys::cameraForward)) { // forwards
				origin = origin - zVec * amount;
			}
			if (KeyMan::ReadKey(Keys::cameraBack)) { // backwards
				origin = origin + zVec * amount;
			}
			if (KeyMan::ReadKey(Keys::cameraLeft)) {	// left
				origin = origin - xVec * amount;
			}
			if (KeyMan::ReadKey(Keys::cameraRight)) {	// right
				origin = origin + xVec * amount;
			}
			if (KeyMan::ReadKey(Keys::cameraDown)) { // down
				origin = origin - yVec * amount;
			}
			if (KeyMan::ReadKey(Keys::camerUp)) { // up
				origin = origin + yVec * amount;
			}
			
			g_OldMatrix._41 = origin.x;
			g_OldMatrix._42 = origin.y;
			g_OldMatrix._43 = origin.z;

			// zoom fov with mouse wheel
			auto pIo = ImGui::GetIO();
			if (pIo.MouseWheel > 0 || pIo.MouseWheel < 0) {
				GameRenderer* pGameRenderer = *(GameRenderer**)StaticOffsets::Get_OFFSET_GAMERENDERER();
				if (IsValidPtr(pGameRenderer) && IsValidPtr(pGameRenderer->gameRenderSettings)) {
					if (pGameRenderer->gameRenderSettings->forceFOV == 1) {
						pGameRenderer->gameRenderSettings->forceFOV = 60;
					}
					float modifier = 1;
					if (KeyMan::ReadKey(Keys::speedUpCamera)) modifier = 2;
					else if (KeyMan::ReadKey(Keys::slowDownCamera)) modifier = .4;
					pGameRenderer->gameRenderSettings->forceFOV -= pIo.MouseWheel * modifier;
				}
			}

			// look around
		}


		if (IsOpen)
		{
		//	ImGui::SetNextWindowSize(ImVec2(500, 600));
			if (ImGui::Begin("OpenGameCamera", &IsOpen)) {

				ImGui::Checkbox("Enable FreeCam [F5]", &g_EnableFreeCam);

				if (ImGui::CollapsingHeader("Camera Speed")) {
					ImGui::InputFloat("Normal Speed", &mainSpeed, .001, .01);
					ImGui::InputFloat("Fast [SHIFT] Speed", &fastSpeed, .001, .01);
					ImGui::InputFloat("Slow [ALT] Speed", &slowSpeed, .00001, .001);
				}

				if (ImGui::CollapsingHeader("Saved Cameras")) {
					if (ImGui::Button("Save Camera [F6]")) {
						savedCameras.push_back(pGameRenderer->renderView->transform1);
					}

					for (int i = 0; i < savedCameras.size(); i++) {
						if (ImGui::Button(std::string("Saved Camera #" + std::to_string(i)).c_str())) {
							g_OldMatrix = savedCameras.at(i);
							toggleFreeCam(true);
						}
					}
				}

				if (ImGui::CollapsingHeader("Saved Tracks")) {

					for (int i = 0; i < savedTracks.size(); i++) {
						if (ImGui::Button(std::string("Saved Track #" + std::to_string(i)).c_str())) {
							currentTrack = savedTracks.at(i);
							currentTrackFrame = 0;
							toggleFreeCam(true);
							isPlayingTrack = true;
						}
					}
				}

				GameRenderer* pGameRenderer = *(GameRenderer**)StaticOffsets::Get_OFFSET_GAMERENDERER();

				void* pGameTimeSettings = *(void**)StaticOffsets::Get_OFFSET_GAMETIMESETTINGS();

				if (ImGui::CollapsingHeader("Show Camera Matrix")) {
					ImGui::InputFloat4("", pGameRenderer->renderView->transform1.m[0]);
					ImGui::InputFloat4("", pGameRenderer->renderView->transform1.m[1]);
					ImGui::InputFloat4("", pGameRenderer->renderView->transform1.m[2]);
					ImGui::InputFloat4("", pGameRenderer->renderView->transform1.m[3]);
				}
				if (IsValidPtr(pGameRenderer->gameRenderSettings)) {
					if (ImGui::CollapsingHeader("Camera Settings")) {
						ImGui::InputFloat("Fov", getTypeInfoMember<float*>(pGameRenderer->gameRenderSettings, "ForceFov"), 0.25, 5);
						ImGui::InputFloat("Brightness", getTypeInfoMember<float*>(pGameRenderer->gameRenderSettings, "BrightnessScale"), 0.01, 1);
						if (IsValidPtr(pUiSettings)) {
							ImGui::Checkbox("Draw UI", getTypeInfoMember<bool*>(pUiSettings, "DrawEnable"));
						}
						ImGui::Checkbox("Bind UI-Enabled to FreeCam", &g_BindUItoFreeCam);

					}
				}
				if (IsValidPtr(pGameTimeSettings) && IsValidPtr(pGameRenderer->compositionSettings->worldRenderSettings)) {
					if (ImGui::CollapsingHeader("Game Settings")) {
						ImGui::InputFloat("ResolutionScale", getTypeInfoMember<float*>(pGameRenderer->gameRenderSettings, "ResolutionScale"), 0.01, 1);
						ImGui::InputFloat("TimeScale", getTypeInfoMember<float*>(pGameTimeSettings, "TimeScale"), .001, .01);
						ImGui::Checkbox("LensFlaresEnable", getTypeInfoMember<bool*>(pGameRenderer->compositionSettings->worldRenderSettings, "LensFlaresEnable"));
						ImGui::Checkbox("CloudShadowEnable", getTypeInfoMember<bool*>(pGameRenderer->compositionSettings->worldRenderSettings, "CloudShadowEnable"));
						ImGui::Checkbox("FilmicEffectsEnable", getTypeInfoMember<bool*>(pGameRenderer->compositionSettings->worldRenderSettings, "FilmicEffectsEnable"));
						ImGui::Checkbox("OutdoorLightEnable", getTypeInfoMember<bool*>(pGameRenderer->compositionSettings->worldRenderSettings, "OutdoorLightEnable"));
						ImGui::Checkbox("OutdoorKeyLightEnable", getTypeInfoMember<bool*>(pGameRenderer->compositionSettings->worldRenderSettings, "OutdoorKeyLightEnable"));
						ImGui::Checkbox("OutdoorSkyLightEnable", getTypeInfoMember<bool*>(pGameRenderer->compositionSettings->worldRenderSettings, "OutdoorSkyLightEnable"));
						ImGui::Checkbox("SkyHeightFogEnable", getTypeInfoMember<bool*>(pGameRenderer->compositionSettings->worldRenderSettings, "SkyHeightFogEnable"));
						ImGui::Checkbox("SkyDepthFogEnable", getTypeInfoMember<bool*>(pGameRenderer->compositionSettings->worldRenderSettings, "SkyDepthFogEnable"));
						ImGui::Checkbox("SkyRenderEnable", getTypeInfoMember<bool*>(pGameRenderer->compositionSettings->worldRenderSettings, "SkyRenderEnable"));
					}
				}
			}
			ImGui::End();
		}
	}
	ImGui::Render();
}