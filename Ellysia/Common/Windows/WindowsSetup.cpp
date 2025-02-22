#include "WindowsSetup.h"

#include <ConvertLog.h>

#pragma comment(lib,"winmm.lib")




Ellysia::WindowsSetup* Ellysia::WindowsSetup::GetInstance() {
	static Ellysia::WindowsSetup instance;
	return &instance;
}



LRESULT Ellysia::WindowsSetup::WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam){
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wparam, lparam)) {
		return true;
	}
	
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対してアプリの終了を伝える
		PostQuitMessage(0);
		return 0;
	}

	return DefWindowProc(hwnd, msg, wparam, lparam);

}

void Ellysia::WindowsSetup::OutputText(std::string& stringText){

	//wstringからstringに変換
	std::wstring newString = ConvertString::ToWString(stringText);
	
	//出力
	OutputDebugStringW(newString.c_str());

}

#pragma region Initializeに入れる関数

//ウィンドウに情報を入れる
void  Ellysia::WindowsSetup::RegisterWindowsClass() {
	
	//ウィンドウプロシャージャ
	windowClass_.lpfnWndProc = WindowProc;
	//ウィンドウクラス名
	windowClass_.lpszClassName = L"%s",title_;
	//インスタンドハンドル
	windowClass_.hInstance = GetModuleHandle(nullptr);
	//カーソル
	windowClass_.hCursor = LoadCursor(nullptr, IDC_ARROW);
	//ウィンドウクラス登録
	RegisterClass(&windowClass_);

	//クライアント領域サイズ
	//ウィンドウサイズを表す構造体にクライアント領域を入れる
	RECT wrc_ = { 0,0,LONG(clientWidth_) ,LONG(clientHeight_) };
	// クライアント領域を元に実際のサイズにwrcを変更
	AdjustWindowRect(&wrc_, WS_OVERLAPPEDWINDOW, false);
	// ウィンドウ生成
	hwnd_ = CreateWindow(
		//クラス名
		windowClass_.lpszClassName,
		//タイトルバーの文字
		title_,
		//標準的なウィンドウスタイル
		WS_OVERLAPPEDWINDOW,
		//標準X座標
		CW_USEDEFAULT,
		//標準Y座標
		CW_USEDEFAULT,
		//横幅
		wrc_.right - wrc_.left,
		//縦幅
		wrc_.bottom - wrc_.top,
		//親ハンドル
		nullptr,
		//メニューハンドル
		nullptr,
		//インスタンスハンドル
		windowClass_.hInstance,
		//オプション
		nullptr					
	);

		
	
}

void Ellysia::WindowsSetup::DisplayWindow() {
	//ウィンドウを表示
	ShowWindow(hwnd_, SW_SHOW);
}

#pragma endregion

void Ellysia::WindowsSetup::Initialize(const wchar_t* title, int32_t clientWidth,int32_t clientHeight) {
	//値を入れる
	title_ = title;
	clientWidth_ = clientWidth;
	clientHeight_ = clientHeight;

	//システムタイマーの分解能を上げる
	timeBeginPeriod(1);

	//ウィンドウクラスを登録
	RegisterWindowsClass();

	//ウィンドウを表示
	DisplayWindow();
}

void Ellysia::WindowsSetup::WindowsMSG(MSG& msg) {
	TranslateMessage(&msg);
	DispatchMessage(&msg);
}

void Ellysia::WindowsSetup::Close() {
	//ウィンドウを閉じる
	CloseWindow(hwnd_);
}


