#pragma once
#include <list>
#include <memory>

#include "Model.h"
#include "WorldTransform.h"
#include "Material.h"
#include <Stage/DemoObject/DemoObject.h>

//LevelEditorでステージを作るつもり

struct Camera;
struct SpotLight;

class ObjectManager{
public:

	/// <summary>
	/// コンストラクタ
	/// </summary>
	ObjectManager() = default;

	/// <summary>
	/// 初期化
	/// </summary>
	void Initialize();

	/// <summary>
	/// 更新
	/// </summary>
	void Update();

	/// <summary>
	/// 描画
	/// </summary>
	/// <param name="camera"></param>
	/// <param name="spotLight"></param>
	void Draw(Camera& camera,SpotLight& spotLight);

	/// <summary>
	/// デストラクタ
	/// </summary>
	~ObjectManager();

public:
	/// <summary>
	/// DemoObjectリストの取得
	/// </summary>
	/// <returns></returns>
	std::list <DemoObject*> GetDemoObjets()const {
		return demoObjects_;
	}



private:
	//各オブジェクトをリストにするかも
	Material material_ = {};
	std::list <DemoObject*> demoObjects_ = {};


};
