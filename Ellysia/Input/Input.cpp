#include "Input.h"


//シングルインスタンス
Input* Input::GetInstance() {
	//関数内static変数として宣言する
	static Input instance;

	return &instance;
}



void Input::Initialize() {
	

	HRESULT hr = {};
	

	//DirectInputオブジェクトの生成
	//これは一つだけで良い
	hr = DirectInput8Create(
		WindowsSetup::GetInstance()->GetHInstance(),
		DIRECTINPUT_VERSION, 
		IID_IDirectInput8, 
		(void**)&directInput_, nullptr);

	assert(SUCCEEDED(hr));


	//キーボードデバイスの生成
	//GUID_Joystick,GUID_SysMouseを指定して、コントローラーとマウスも使えるよ
	hr = directInput_->CreateDevice(GUID_SysKeyboard, &keyboard_, NULL);
	assert(SUCCEEDED(hr));
	//マウスデバイスの生成
	hr = directInput_->CreateDevice(GUID_SysMouse, &mouseDevice_, NULL);
	assert(SUCCEEDED(hr));
	//コントローラーデバイスの生成
	HRESULT joyStickDeviceHR = {};
	joyStickDeviceHR = directInput_->CreateDevice(GUID_Joystick, &joyStickDevice_, NULL);
	//コントローラーに接続していない時は作らない
	if (SUCCEEDED(joyStickDeviceHR) != true) {
		joyStickDevice_ = nullptr;
	}


	
	//入力データの形式のセット
	//標準形式
	hr = keyboard_->SetDataFormat(&c_dfDIKeyboard);
	assert(SUCCEEDED(hr));
	//マウスの方の入力データ形式セット
	hr = mouseDevice_->SetDataFormat(&c_dfDIMouse);
	assert(SUCCEEDED(hr));
	//コントローラーの方の入力データ形式セット
	if (SUCCEEDED(joyStickDeviceHR) == true) {
		joyStickDeviceHR = joyStickDevice_->SetDataFormat(&c_dfDIJoystick);
		assert(SUCCEEDED(joyStickDeviceHR));

	}
	

	//排他制御レベルのセット
	//
	//DISCL_FOREGROUND...画面が手前にある場合のみ入力を受け付ける
	//DISCL_NONEXCLUSIVE...デバイスをこのアプリだけで専有しない
	//DISCL_NOWINKEY...Windowsキーを無効にする

	hr = keyboard_->SetCooperativeLevel(WindowsSetup::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);
	assert(SUCCEEDED(hr));
	hr = mouseDevice_->SetCooperativeLevel(WindowsSetup::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
	assert(SUCCEEDED(hr));
	if (SUCCEEDED(joyStickDeviceHR) == true) {
		joyStickDeviceHR = joyStickDevice_->SetCooperativeLevel(WindowsSetup::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
		assert(SUCCEEDED(joyStickDeviceHR));

	}



}

#pragma region キーボード

//Push状態
bool Input::IsPushKey(uint8_t keyNumber) {
	//指定されていたキーを押していればtrueを返す
	if (currentKey_[keyNumber]!=0) {
		return true;
	}
	//押されていなければfalseを返す
	return false;

}

//トリガー
bool Input::IsTriggerKey(uint8_t keyNumber) {
	if (currentKey_[keyNumber]!=0 && preKey_[keyNumber]==0) {
		return true;
	}
	return false;
}

#pragma endregion

#pragma region マウス
//Push状態
bool Input::IsPushMouse(int32_t mouseNumber) {
	if (currentMouse_.rgbButtons[mouseNumber] != 0) {
		return true;
	}
	return false;
}

//Trigger状態
bool Input::IsTriggerMouse(int32_t mouseNumber) {
	if (currentMouse_.rgbButtons[mouseNumber] != 0 && preMouse_.rgbButtons[mouseNumber] == 0) {
		return true;
	}
	return false;
}
#pragma endregion

#pragma region コントローラー

//状態を取得
bool Input::GetJoystickState(XINPUT_STATE& state){
	DWORD dwResult = XInputGetState(0, &state);
	if (dwResult == ERROR_SUCCESS){
		return true;
	}
	return false;
	
}

bool Input::IsPushLeft(XINPUT_STATE& state) {
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_LEFT_SHOULDER) {
		return true;
	}
	return false;
}

bool Input::IsPushRight(XINPUT_STATE& state) {
	if (state.Gamepad.wButtons & XINPUT_GAMEPAD_RIGHT_SHOULDER) {
		return true;
	}
	return false;
}


void Input::SetVibration(float leftMotor, float rightMotor){
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	//0から65535まで
	const float MAX_SIZE = 65535.0f;
	vibration.wLeftMotorSpeed = WORD(leftMotor* MAX_SIZE);
	vibration.wRightMotorSpeed = WORD(rightMotor* MAX_SIZE);
	XInputSetState(0, &vibration);
}

void Input::StopVibration(){
	XINPUT_VIBRATION vibration;
	ZeroMemory(&vibration, sizeof(XINPUT_VIBRATION));
	vibration.wLeftMotorSpeed = WORD(0.0f);
	vibration.wRightMotorSpeed = WORD(0.0f);
	XInputSetState(0, &vibration);
}

void Input::SetIsDisplayCursor(bool isDisplay){
	ShowCursor(isDisplay);
}


#pragma endregion

void Input::Update() {

	// コントローラーの再初期化
	//接続されたらまたデバイスを作り直す
	if (joyStickDevice_ == nullptr) {
		HRESULT hr = {};
		hr = directInput_->CreateDevice(GUID_Joystick, &joyStickDevice_, NULL);
		if (SUCCEEDED(hr)) {
			hr = joyStickDevice_->SetDataFormat(&c_dfDIJoystick);
			assert(SUCCEEDED(hr));
			hr = joyStickDevice_->SetCooperativeLevel(WindowsSetup::GetInstance()->GetHwnd(), DISCL_FOREGROUND | DISCL_NONEXCLUSIVE);
			assert(SUCCEEDED(hr));
		}
	}

	XINPUT_STATE state{};
	//前回のキー入力を保存
	memcpy(preKey_, currentKey_, sizeof(currentKey_));
	memcpy(preControllerButtons_, currentControllerButtons_, sizeof(preControllerButtons_));
	for (int i = 0; i < XUSER_MAX_COUNT; ++i) {
		currentControllerButtons_[i][0] = state.Gamepad.wButtons & 0xFF;
		currentControllerButtons_[i][1] = (state.Gamepad.wButtons >> 8) & 0xFF;
	}
	//Keyと違ってそのまま代入で大丈夫だよ
	preMouse_ = currentMouse_;

	//キーボード情報の取得開始
	keyboard_->Acquire();
	//マウス情報の取得開始
	mouseDevice_->Acquire();
	//コントローラー情報の取得開始
	if (joyStickDevice_ != nullptr) {
		joyStickDevice_->Acquire();
	}
	
	keyboard_->GetDeviceState(sizeof(currentKey_), currentKey_);

	//0から255番まであるよ
	//エンターキーを押していると0x80(128)が代入されるよ
	//押していないとどのキーも0x00(0)が代入されるよ

	mouseDevice_->GetDeviceState(sizeof(DIMOUSESTATE), &currentMouse_);
	
	if (joyStickDevice_ != nullptr) {
		joyStickDevice_->GetDeviceState(sizeof(preControllerButtons_), currentControllerButtons_);
	}
	
}
