#include <windows.h>
#include <string>
#include <cassert>
#include <d3d11.h>
#include <vector>
#include <iomanip>

#include "WindowUtils.h"
#include "D3DUtil.h"
#include "D3D.h"
#include "CommonStates.h"
#include "SimpleMath.h"
#include "TexCache.h"
#include "Sprite.h"
#include "SpriteFont.h"

using namespace std;
using namespace DirectX;
using namespace DirectX::SimpleMath;


class PlayMode
{
public:
	PlayMode(MyD3D& d3d)
		:mD3D(d3d), mBgnd(d3d), mCoin(d3d)
	{
		InitScore();
		mBgnd.SetTex(*d3d.GetCache().LoadTexture(&d3d.GetDevice(), "bgnd.dds"));
		mBgnd.SetScale(Vector2(WinUtil::Get().GetClientWidth() / mBgnd.GetTexData().dim.x, 
						WinUtil::Get().GetClientHeight() / mBgnd.GetTexData().dim.y));
		mpFont = new SpriteFont(&d3d.GetDevice(), L"data/fonts/comicSansMS.spritefont");
		assert(mpFont);

		mCoin.SetTex(*d3d.GetCache().LoadTexture(&d3d.GetDevice(), "coin.dds"));
		mCoin.SetTexRect(RECTF{ 0,0,16,16 });
		mCoin.SetAnimRect(RECTF{ 0,0,64,64 });
		mCoin.isAnimated = true;
		mCoin.SetScale(Vector2(2.f, 2.f));

	}
	void Update(float dTime) {
	}


	void Render(float dTime, SpriteBatch& batch) {
		int w, h;
		WinUtil::Get().GetClientExtents(w, h); //get window size

		mBgnd.Draw(batch);
		mCoin.Animate(dTime);
		mCoin.mPos = { w * 0.8f, h * 0.85f};
		mCoin.Draw(batch);

		stringstream ss;
		ss << std::setfill('0') << std::setw(3) << (int)GetClock()*10;
		
		mpFont->DrawString(&batch, ss.str().c_str(), Vector2(w*0.86f, h*0.85f), Vector4(0,0,0,1));
	}
	void Release()
	{
		delete mpFont;
		mpFont = nullptr;
	}
private:
	MyD3D& mD3D;
	Sprite mBgnd;
	Sprite mCoin;
	SpriteFont *mpFont = nullptr;

	void InitScore()
	{
	}
};


/*
Blank solution - setup DX11 and blank the screen
*/

class Game
{
public:
	enum class State { PLAY };
	State state = State::PLAY;
	
	Game(MyD3D& d3d);

	PlayMode mPMode;

	void Release();
	void Update(float dTime);
	void Render(float dTime);
private:
	MyD3D& mD3D;
	SpriteBatch *pSB = nullptr;
};




Game::Game(MyD3D& d3d)
	: mPMode(d3d), mD3D(d3d), pSB(nullptr)
{
	pSB = new SpriteBatch(&mD3D.GetDeviceCtx());
}


//any memory or resources we made need releasing at the end
void Game::Release()
{
	mPMode.Release();
	delete pSB;
	pSB = nullptr;
}

//called over and over, use it to update game logic
void Game::Update(float dTime)
{
	switch (state)
	{
	case State::PLAY:
		mPMode.Update(dTime);
	}
}

//called over and over, use it to render things
void Game::Render(float dTime)
{
	mD3D.BeginRender(Colours::Black);


	CommonStates dxstate(&mD3D.GetDevice());
	pSB->Begin(SpriteSortMode_Deferred, dxstate.NonPremultiplied(),&mD3D.GetWrapSampler());

	switch (state)
	{
	case State::PLAY:
		mPMode.Render(dTime, *pSB);
	}

	pSB->End();


	mD3D.EndRender();
}

//if ALT+ENTER or resize or drag window we might want do
//something like pause the game perhaps, but we definitely
//need to let D3D know what's happened (OnResize_Default).
void OnResize(int screenWidth, int screenHeight, MyD3D& d3d)
{
	d3d.OnResize_Default(screenWidth, screenHeight);
}

//messages come from windows all the time, should we respond to any specific ones?
LRESULT CALLBACK MainWndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	//do something game specific here
	switch (msg)
	{
		// Respond to a keyboard event.
	case WM_CHAR:
		switch (wParam)
		{
		case 27:
		case 'q':
		case 'Q':
			PostQuitMessage(0);
			return 0;
		}
	}

	//default message handling (resize window, full screen, etc)
	return WinUtil::DefaultMssgHandler(hwnd, msg, wParam, lParam);
}

//main entry point for the game
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance,
				   PSTR cmdLine, int showCmd)
{

	int w(512), h(256);
	//int defaults[] = { 640,480, 800,600, 1024,768, 1280,1024 };
		//WinUtil::ChooseRes(w, h, defaults, 4);
	if (!WinUtil::Get().InitMainWindow(w, h, hInstance, "Fezzy", MainWndProc, true))
		assert(false);

	MyD3D d3d;
	if (!d3d.InitDirect3D(OnResize))
		assert(false);
	WinUtil::Get().SetD3D(d3d);
	d3d.GetCache().SetAssetPath("data/");
	Game game(d3d);

	bool canUpdateRender;
	float dTime = 0;
	while (WinUtil::Get().BeginLoop(canUpdateRender))
	{
		if (canUpdateRender)
		{
			game.Update(dTime);
			game.Render(dTime);
		}
		dTime = WinUtil::Get().EndLoop(canUpdateRender);
	}

	game.Release();
	d3d.ReleaseD3D(true);	
	return 0;
}

