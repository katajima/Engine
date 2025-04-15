#include"Framework.h"

void Framework::Initialize()
{
	// WindowsAPI解放
	winApp = std::make_unique<WinApp>();
	winApp->Initialize();


	dxCommon = std::make_unique<DirectXCommon>();
	dxCommon->Intialize(winApp.get());

	entity3DManager_ = std::make_unique<Entity3DManager>();
	entity3DManager_->Initialize(dxCommon.get());

	renderingCommon = RenderingCommon::GetInstance();
	renderingCommon->Initialize(dxCommon.get());

	Input::GetInstance()->Intialize(winApp.get());
	

	// スプライト共通部の初期化
	spriteCommon = SpriteCommon::GetInstance();
	spriteCommon->Initialize(dxCommon.get());


	lineCommon = LineCommon::GetInstance();
	lineCommon->Initialize(dxCommon.get());

	particleManager_ = ParticleManager::GetInstance();
	particleManager_->Initialize(dxCommon.get());

	
	primitiveCommon = PrimitiveCommon::GetInstance();
	primitiveCommon->Initialize(dxCommon.get());
}

void Framework::Finalize()
{
	// WindowsAPIの終了処理
	winApp->Finalize();
	
	particleManager_->Finalize();

	primitiveCommon->Finalize();

	lineCommon->Finalize();

	renderingCommon->Finalize();


	dxCommon->Finalize();

}

void Framework::Update()
{

	Input::GetInstance()->Update();

	
	// Windowsのメッセージ処理
	if(winApp->ProcessMessage()) {
		// ゲームループを抜ける
		endRequst_ = true;
	};
}

void Framework::Run()
{
	// ゲームの初期化
	Initialize();
	while (true) {
		// 毎フレーム更新
		Update();
		// 終了リクエストが来たら抜ける
		if (IsEndRequst()) {
			break;
		}
		// 描画
		Draw();
	}
	// ゲームの終了
	Finalize();
}
