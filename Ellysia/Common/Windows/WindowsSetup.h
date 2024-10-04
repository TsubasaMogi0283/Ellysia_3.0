#pragma once

#include <Windows.h>
#include <WinUser.h>
#include <cstdint>
#include <string>

#include <imgui.h>
#include <imgui_impl_dx12.h>
#include <imgui_impl_win32.h>

//外部のつまり自分が作ったものではないファイルなどは
//<>でインクルードさせた方が良いよ

//その他自分で作ったものは""でインクルードさせてね

//extern...グローバル変数を共有する
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hwnd,UINT msg,WPARAM wParam,LPARAM lParam);

//シングルでやりたい
//finalで継承禁止
class WindowsSetup final{
private:
	//インスタンスを作れないようにする
	//コンストラクタをprivateに
	
	/// <summary>
	/// コンストラクタ
	/// </summary>
	WindowsSetup()=default;

	/// <summary>
	/// デストラクタ
	/// </summary>
	~WindowsSetup() = default;

public:

	#pragma region 禁止事項
	//コピーコンストラクタ禁止
	WindowsSetup(const WindowsSetup& winApp) = delete;

	//代入演算子を無効にする
	WindowsSetup& operator=(const WindowsSetup& winApp) = delete;

	#pragma endregion

	//インスタンスにアクセス可能な関数を追加
	//静的メンバ関数にする。クラス名を指定すればアクセスできる
	static WindowsSetup* GetInstance();


	//Window Procedure
	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	static void OutputText(std::string& stringText);

private:

	//ウィンドウクラスを登録
	void RegisterWindowsClass();

	//WINodwを表示
	void DisplayWindow();


public:

	/// <summary>
	/// 初期化
	/// </summary>
	/// <param name="title"></param>
	/// <param name="clientWidth"></param>
	/// <param name="clientHeight"></param>
	void Initialize(const wchar_t* title, int32_t clientWidth,int32_t clientHeight);

	/// <summary>
	/// メッセージ
	/// </summary>
	/// <param name="msg"></param>
	void WindowsMSG(MSG& msg);

	/// <summary>
	/// 閉じる
	/// </summary>
	void Close();

#pragma region アクセッサ

	//Getter
	uint32_t GetClientWidth() {
		return clientWidth_;
	}
	uint32_t GetClientHeight() {
		return clientHeight_;
	}

	HWND GetHwnd() {
		return hwnd_;
	}

	HINSTANCE GetHInstance() {
		return wc_.hInstance;
	}



#pragma endregion

private:

	////ウィンドウクラスを登録する
	const wchar_t* title_=L"Ellysia";
	
	uint32_t clientWidth_=0;
	uint32_t clientHeight_=0;
	

	HWND hwnd_=0;

	WNDCLASS wc_{};

	

};

