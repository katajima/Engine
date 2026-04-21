#include "EffectEditor.h"
#include "DirectXGame/engine/Manager/Entity/EntityManager.h"

void EffectEditor::Initialize(Engine::EffectComponent* effectComponent, 
	Engine::GlobalVariables* globalVariables) {

	this->effectComponent = effectComponent;
	this->globalVariables = globalVariables;

}

// 更新
void EffectEditor::Update(float dt) {
	if (effectGlobalDatas_.empty()) return;
#ifdef _DEBUG
	ImGui::Begin("Effect");
	
	// エディターに表示変更するエフェクト選定
	Engine::ImGuiManager::Select("Selected Effect", selectedBlockName_, effectGlobalDatas_);
	ImGui::Separator();
	ImGui::Text("Editing: %s", selectedBlockName_.c_str());
	ImGui::Checkbox("出現", &isSpawnEmit);
	ImGui::DragFloat3("位置", &spawnEmitPos.x, 0.1f);
	ImGui::DragFloat("頻度", &frequency, 0.01f);

	if (isSpawnEmit) {
		timer += dt;
		if (frequency < timer) {
			Emit(selectedBlockName_, spawnEmitPos);
			timer = 0.0f;
		}
	}
	

	// --- 選択されているブロックだけ表示 ---
	for (auto& combo : effectGlobalDatas_) {
		const std::string& name = combo.first;

		// 選択中だけ表示
		const bool nowChoice = (name == selectedBlockName_);

		if (!nowChoice) continue;

		// エディターでの調整
		AAAA(name,combo.second);
		// データの保存
		SetValue(name, combo.second);
	}

	// セーブ
	if (ImGui::Button("Save")) {
		for (auto& it : effectGlobalDatas_) {
			globalVariables->SaveFile(it.first);
		}
	}
	
	ImGui::End();
#endif // _DEBUG

#ifdef _DEBUG
	for (auto& [name, data] : effectGlobalDatas_) {
		GetValue(name, data);
		SetEffectGlobalData(name, data.shapeType, data);
	}
#endif // _DEBUG
}

void EffectEditor::SetEffectGlobalData(const std::string& name, EmitterShapeType shapeType, const EffectGlobalData& data) {
	Engine::BaseParticleEmitter* emit = effectComponent->GetBaseEmitter(name);
	emit->SetParticleName(data.particleName);		// パーティクル名
	emit->GetFrequency() = data.frequency;			// 出現頻度
	emit->GetEmitData() = data.emitData;			// 出現データ
	emit->SetIsFlag(data.isFlag);
	emit->SetAlphaClipping(data.alphaClipping);		// アルファクリッピング
	emit->SetEnableLighting(data.enableLighting);	// ライティングの有無
	emit->SetLifeTimeScaleTopBottom(data.topBottom);// ライフタイムスケールの基準


	switch (shapeType)
	{
	case EmitterShapeType::AABB:
	{
		Engine::AABBParticleEmitter* aabbEmit = effectComponent->GetEmitterAs<Engine::AABBParticleEmitter>(name);
		aabbEmit->SetRange(data.rangeMin, data.rangeMax);
	}
	break;
	case EmitterShapeType::SPHERE:
	{
		Engine::SphereParticleEmitter* sphereEmit = effectComponent->GetEmitterAs<Engine::SphereParticleEmitter>(name);
		sphereEmit->SetRadius(data.radius);
	}
	break;
	case EmitterShapeType::CIRCLE:
	{
		Engine::CornerParticleEmitter* circleEmit = effectComponent->GetEmitterAs<Engine::CornerParticleEmitter>(name);
		circleEmit->SetRadius(data.radius);
		circleEmit->SetSegment(data.segment);
	}
	break;
	case EmitterShapeType::POINT:
	{

	}
	break;
	case EmitterShapeType::LINE:
	{
		Engine::LineParticleEmitter* lineEmit = effectComponent->GetEmitterAs<Engine::LineParticleEmitter>(name);
		lineEmit->SetSegment(data.lineStart, data.lineEnd);
	}
	break;
	case EmitterShapeType::SPLINE:
	{
		Engine::SplineParticleEmitter* splineEmit = effectComponent->GetEmitterAs<Engine::SplineParticleEmitter>(name);
		for (const auto& point : data.splinePoints) {
			splineEmit->AddControlPoints(point);
		}
	}
	break;
	case EmitterShapeType::TRIANGLE:
	{
		Engine::TriangleParticleEmitter* triangleEmit = effectComponent->GetEmitterAs<Engine::TriangleParticleEmitter>(name);
		triangleEmit->SetTriangle(data.triangle);
	}
	break;
	case EmitterShapeType::MESH:
		break;
	default:
		break;
	}
}

void EffectEditor::AddEffectGlobalData(const std::string& name, const std::string& particleName) {
	if (effectGlobalDatas_.find(name) != effectGlobalDatas_.end()) {
		// すでに存在する場合
		return;
	}
	EffectGlobalData data;
	data.particleName = particleName;
	// 保存項目に追加
	AddItem(name, data);
	// データを取得
	GetValue(name, data);
	// データを保存
	effectGlobalDatas_[name] = data;
}

void EffectEditor::AddItem(const std::string& name, const EffectGlobalData& data) {
	globalVariables->CreateGroup(name);
	globalVariables->AddItem(name, "particleName", data.particleName);
	globalVariables->AddItem(name, "frequency", data.frequency);
	globalVariables->AddItem(name, "emitData.acceleration.median", data.emitData.acceleration.median);
	globalVariables->AddItem(name, "emitData.acceleration.range", data.emitData.acceleration.range);
	globalVariables->AddItem(name, "emitData.colorRange.min", data.emitData.colorRange.min);
	globalVariables->AddItem(name, "emitData.colorRange.max", data.emitData.colorRange.max);
	globalVariables->AddItem(name, "emitData.count.median", data.emitData.count.median);
	globalVariables->AddItem(name, "emitData.count.range", data.emitData.count.range);
	globalVariables->AddItem(name, "emitData.lifeTime.median", data.emitData.lifeTime.median);
	globalVariables->AddItem(name, "emitData.lifeTime.range", data.emitData.lifeTime.range);
	globalVariables->AddItem(name, "emitData.rotate.median", data.emitData.rotate.median);
	globalVariables->AddItem(name, "emitData.rotate.range", data.emitData.rotate.range);
	globalVariables->AddItem(name, "emitData.rotateVelocity.median", data.emitData.rotateVelocity.median);
	globalVariables->AddItem(name, "emitData.rotateVelocity.range", data.emitData.rotateVelocity.range);
	globalVariables->AddItem(name, "emitData.size.median", data.emitData.size.median);
	globalVariables->AddItem(name, "emitData.size.range", data.emitData.size.range);
	globalVariables->AddItem(name, "emitData.sizeAmount.median", data.emitData.sizeAmount.median);
	globalVariables->AddItem(name, "emitData.sizeAmount.range", data.emitData.sizeAmount.range);
	globalVariables->AddItem(name, "emitData.velocity.median", data.emitData.velocity.median);
	globalVariables->AddItem(name, "emitData.velocity.range", data.emitData.velocity.range);

	globalVariables->AddItem(name, "emitData.alphaClipping", data.alphaClipping);
	globalVariables->AddItem(name, "emitData.enableLighting", data.enableLighting);

	globalVariables->AddItem(name, "emitData.isFlag.billboardRotZ", data.isFlag.billboardRotZ);
	globalVariables->AddItem(name, "emitData.isFlag.isAcceleration", data.isFlag.isAcceleration);
	globalVariables->AddItem(name, "emitData.isFlag.isAlpha", data.isFlag.isAlpha);
	globalVariables->AddItem(name, "emitData.isFlag.isBounce", data.isFlag.isBounce);
	globalVariables->AddItem(name, "emitData.isFlag.isGravity", data.isFlag.isGravity);
	globalVariables->AddItem(name, "emitData.isFlag.isLifeTimeScale", data.isFlag.isLifeTimeScale_);
	globalVariables->AddItem(name, "emitData.isFlag.isLifeTimeVelocity", data.isFlag.isLifeTimeVelocity);
	globalVariables->AddItem(name, "emitData.isFlag.isLine", data.isFlag.isLine);
	globalVariables->AddItem(name, "emitData.isFlag.isLineInterpolation", data.isFlag.isLineInterpolation);
	globalVariables->AddItem(name, "emitData.isFlag.isRotateVelocity", data.isFlag.isRotateVelocity);
	globalVariables->AddItem(name, "emitData.isFlag.isScaling", data.isFlag.isScaling_);
	globalVariables->AddItem(name, "emitData.isFlag.usebillboard", data.isFlag.usebillboard);
	globalVariables->AddItem(name, "emitData.isFlag.usebillboardY", data.isFlag.usebillboardY);


	globalVariables->AddItem(name, "emitData.lineEnd", data.lineEnd);
	globalVariables->AddItem(name, "emitData.lineStart", data.lineStart);

	globalVariables->AddItem(name, "emitData.radius", data.radius);

	globalVariables->AddItem(name, "emitData.rangeMax", data.rangeMax);
	globalVariables->AddItem(name, "emitData.rangeMin", data.rangeMin);

	globalVariables->AddItem(name, "emitData.segment", data.segment);

	globalVariables->AddEnumItem<EmitterShapeType>(name, "emitData.shapeType", data.shapeType, "EmitterShapeType");
	globalVariables->AddEnumItem<EmitData::TopBottom>(name, "emitData.topBottom", data.topBottom, "TopBottom");
}

void EffectEditor::GetValue(const std::string& name, EffectGlobalData& data) {
	data.particleName = globalVariables->GetValue<std::string>(name, "particleName");

	data.frequency = globalVariables->GetValue<float>(name, "frequency");
	data.emitData.acceleration.median = globalVariables->GetValue<Vector3>(name, "emitData.acceleration.median");
	data.emitData.acceleration.range = globalVariables->GetValue<Vector3>(name, "emitData.acceleration.range");
	data.emitData.colorRange.min = globalVariables->GetValue<Vector4>(name, "emitData.colorRange.min");
	data.emitData.colorRange.max = globalVariables->GetValue<Vector4>(name, "emitData.colorRange.max");
	data.emitData.count.median = globalVariables->GetValue<int>(name, "emitData.count.median");
	data.emitData.count.range = globalVariables->GetValue<int>(name, "emitData.count.range");
	data.emitData.lifeTime.median = globalVariables->GetValue<float>(name, "emitData.lifeTime.median");
	data.emitData.lifeTime.range = globalVariables->GetValue<float>(name, "emitData.lifeTime.range");
	data.emitData.rotate.median = globalVariables->GetValue<Vector3>(name, "emitData.rotate.median");
	data.emitData.rotate.range = globalVariables->GetValue<Vector3>(name, "emitData.rotate.range");
	data.emitData.rotateVelocity.median = globalVariables->GetValue<Vector3>(name, "emitData.rotateVelocity.median");
	data.emitData.rotateVelocity.range = globalVariables->GetValue<Vector3>(name, "emitData.rotateVelocity.range");
	data.emitData.size.median = globalVariables->GetValue<Vector3>(name, "emitData.size.median");
	data.emitData.size.range = globalVariables->GetValue<Vector3>(name, "emitData.size.range");
	data.emitData.sizeAmount.median = globalVariables->GetValue<Vector3>(name, "emitData.sizeAmount.median");
	data.emitData.sizeAmount.range = globalVariables->GetValue<Vector3>(name, "emitData.sizeAmount.range");
	data.emitData.velocity.median = globalVariables->GetValue<Vector3>(name, "emitData.velocity.median");
	data.emitData.velocity.range = globalVariables->GetValue<Vector3>(name, "emitData.velocity.range");

	data.alphaClipping = globalVariables->GetValue<float>(name, "emitData.alphaClipping");
	data.enableLighting = globalVariables->GetValue<int>(name, "emitData.enableLighting");

	data.isFlag.billboardRotZ = globalVariables->GetValue<bool>(name, "emitData.isFlag.billboardRotZ");
	data.isFlag.isAcceleration = globalVariables->GetValue<bool>(name, "emitData.isFlag.isAcceleration");
	data.isFlag.isAlpha = globalVariables->GetValue<bool>(name, "emitData.isFlag.isAlpha");
	data.isFlag.isBounce = globalVariables->GetValue<bool>(name, "emitData.isFlag.isBounce");
	data.isFlag.isGravity = globalVariables->GetValue<bool>(name, "emitData.isFlag.isGravity");
	data.isFlag.isLifeTimeScale_ = globalVariables->GetValue<bool>(name, "emitData.isFlag.isLifeTimeScale");
	data.isFlag.isLifeTimeVelocity = globalVariables->GetValue<bool>(name, "emitData.isFlag.isLifeTimeVelocity");
	data.isFlag.isLine = globalVariables->GetValue<bool>(name, "emitData.isFlag.isLine");
	data.isFlag.isLineInterpolation = globalVariables->GetValue<bool>(name, "emitData.isFlag.isLineInterpolation");
	data.isFlag.isRotateVelocity = globalVariables->GetValue<bool>(name, "emitData.isFlag.isRotateVelocity");
	data.isFlag.isScaling_ = globalVariables->GetValue<bool>(name, "emitData.isFlag.isScaling");
	data.isFlag.usebillboard = globalVariables->GetValue<bool>(name, "emitData.isFlag.usebillboard");
	data.isFlag.usebillboardY = globalVariables->GetValue<bool>(name, "emitData.isFlag.usebillboardY");


	data.lineEnd = globalVariables->GetValue<Vector3>(name, "emitData.lineEnd");
	data.lineStart = globalVariables->GetValue<Vector3>(name, "emitData.lineStart");

	data.radius = globalVariables->GetValue<float>(name, "emitData.radius");

	data.rangeMax = globalVariables->GetValue<Vector3>(name, "emitData.rangeMax");
	data.rangeMin = globalVariables->GetValue<Vector3>(name, "emitData.rangeMin");

	data.segment = globalVariables->GetValue<int>(name, "emitData.segment");

	data.shapeType = globalVariables->GetEnumValue<EmitterShapeType>(name, "emitData.shapeType");
	data.topBottom = globalVariables->GetEnumValue<EmitData::TopBottom>(name, "emitData.topBottom");
}

void EffectEditor::SetValue(const std::string& name, const EffectGlobalData& data) {
	globalVariables->SetValue(name, "particleName", data.particleName);
	globalVariables->SetValue(name, "frequency", data.frequency);
	globalVariables->SetValue(name, "emitData.acceleration.median", data.emitData.acceleration.median);
	globalVariables->SetValue(name, "emitData.acceleration.range", data.emitData.acceleration.range);
	globalVariables->SetValue(name, "emitData.colorRange.min", data.emitData.colorRange.min);
	globalVariables->SetValue(name, "emitData.colorRange.max", data.emitData.colorRange.max);
	globalVariables->SetValue(name, "emitData.count.median", data.emitData.count.median);
	globalVariables->SetValue(name, "emitData.count.range", data.emitData.count.range);
	globalVariables->SetValue(name, "emitData.lifeTime.median", data.emitData.lifeTime.median);
	globalVariables->SetValue(name, "emitData.lifeTime.range", data.emitData.lifeTime.range);
	globalVariables->SetValue(name, "emitData.rotate.median", data.emitData.rotate.median);
	globalVariables->SetValue(name, "emitData.rotate.range", data.emitData.rotate.range);
	globalVariables->SetValue(name, "emitData.rotateVelocity.median", data.emitData.rotateVelocity.median);
	globalVariables->SetValue(name, "emitData.rotateVelocity.range", data.emitData.rotateVelocity.range);
	globalVariables->SetValue(name, "emitData.size.median", data.emitData.size.median);
	globalVariables->SetValue(name, "emitData.size.range", data.emitData.size.range);
	globalVariables->SetValue(name, "emitData.sizeAmount.median", data.emitData.sizeAmount.median);
	globalVariables->SetValue(name, "emitData.sizeAmount.range", data.emitData.sizeAmount.range);
	globalVariables->SetValue(name, "emitData.velocity.median", data.emitData.velocity.median);
	globalVariables->SetValue(name, "emitData.velocity.range", data.emitData.velocity.range);

	globalVariables->SetValue(name, "emitData.alphaClipping", data.alphaClipping);
	globalVariables->SetValue(name, "emitData.enableLighting", data.enableLighting);

	globalVariables->SetValue(name, "emitData.isFlag.billboardRotZ", data.isFlag.billboardRotZ);
	globalVariables->SetValue(name, "emitData.isFlag.isAcceleration", data.isFlag.isAcceleration);
	globalVariables->SetValue(name, "emitData.isFlag.isAlpha", data.isFlag.isAlpha);
	globalVariables->SetValue(name, "emitData.isFlag.isBounce", data.isFlag.isBounce);
	globalVariables->SetValue(name, "emitData.isFlag.isGravity", data.isFlag.isGravity);
	globalVariables->SetValue(name, "emitData.isFlag.isLifeTimeScale", data.isFlag.isLifeTimeScale_);
	globalVariables->SetValue(name, "emitData.isFlag.isLifeTimeVelocity", data.isFlag.isLifeTimeVelocity);
	globalVariables->SetValue(name, "emitData.isFlag.isLine", data.isFlag.isLine);
	globalVariables->SetValue(name, "emitData.isFlag.isLineInterpolation", data.isFlag.isLineInterpolation);
	globalVariables->SetValue(name, "emitData.isFlag.isRotateVelocity", data.isFlag.isRotateVelocity);
	globalVariables->SetValue(name, "emitData.isFlag.isScaling", data.isFlag.isScaling_);
	globalVariables->SetValue(name, "emitData.isFlag.usebillboard", data.isFlag.usebillboard);
	globalVariables->SetValue(name, "emitData.isFlag.usebillboardY", data.isFlag.usebillboardY);


	globalVariables->SetValue(name, "emitData.lineEnd", data.lineEnd);
	globalVariables->SetValue(name, "emitData.lineStart", data.lineStart);

	globalVariables->SetValue(name, "emitData.radius", data.radius);

	globalVariables->SetValue(name, "emitData.rangeMax", data.rangeMax);
	globalVariables->SetValue(name, "emitData.rangeMin", data.rangeMin);

	globalVariables->SetValue(name, "emitData.segment", data.segment);

	globalVariables->SetEnumValue<EmitterShapeType>(name, "emitData.shapeType", data.shapeType, "EmitterShapeType");
	globalVariables->SetEnumValue<EmitData::TopBottom>(name, "emitData.topBottom", data.topBottom, "TopBottom");
}

void EffectEditor::AAAA(const std::string& name,EffectGlobalData& data) {
	ImGui::Begin("EffectEditor");

	// パーティクル選択
	Engine::ImGuiManager::Select("Selected Effect", data.particleName, effectComponent->GetParticleManager()->GetParticleGroups());

	static const char* ShapeTypeLabels[] = {
		"AABB",		// AABB
		"SPHERE",		// 球
		"CIRCLE",		// 円
		"POINT",		// 点
		"LINE",		// 線
		"SPLINE",		// スプライン
		"TRIANGLE",	// 三角形
		"MESH",		// メッシュ
	};
	
	EmitterShapeType keep = data.shapeType;
	Engine::ImGuiManager::Select("エミッタ形状", ShapeTypeLabels, data.shapeType);
	if (keep != data.shapeType) {
		effectComponent->RemoveEmitter(name);
		effectComponent->AddEmitter(name, data.particleName, data.shapeType);
		SetEffectGlobalData(name, data.shapeType, data);
	}
	// 出現
	ImGui::DragInt("出現量(中央値)", &data.emitData.count.median, 0.1f);
	ImGui::DragInt("出現量(振れ幅)", &data.emitData.count.range, 0.1f); 

	// 生存時間
	ImGui::DragFloat("生存時間(中央値)", &data.emitData.lifeTime.median, 0.1f);
	ImGui::DragFloat("生存時間(振れ幅)", &data.emitData.lifeTime.range, 0.1f);

	ImGui::Checkbox("重力", &data.isFlag.isGravity);
	ImGui::Checkbox("跳ねるか", &data.isFlag.isBounce);
	bool isLight = data.enableLighting;
	ImGui::Checkbox("ライティング", &isLight);
	data.enableLighting = isLight;
	
	if (ImGui::CollapsingHeader("ビルボード")) {
		ImGui::Checkbox("ビルボードのz回転するか", &data.isFlag.billboardRotZ);
		ImGui::Checkbox("ビルボードするか", &data.isFlag.usebillboard);
		ImGui::Checkbox("ビルボードY軸するか", &data.isFlag.usebillboardY);
		
	}
	if (ImGui::CollapsingHeader("速度")) {
		ImGui::DragFloat3("速度(中央値)", &data.emitData.velocity.median.x, 0.1f);
		ImGui::DragFloat3("速度(振れ幅)", &data.emitData.velocity.range.x, 0.1f);

		ImGui::Checkbox("生存時間による速度変化を使用するか", &data.isFlag.isLifeTimeVelocity);
		ImGui::Checkbox("加速度を使用するか", &data.isFlag.isAcceleration);
		ImGui::DragFloat3("加速度(中央値)", &data.emitData.acceleration.median.x,0.1f);
		ImGui::DragFloat3("加速度(振れ幅)", &data.emitData.acceleration.range.x, 0.1f);
	}
	if (ImGui::CollapsingHeader("回転")) {
		ImGui::DragFloat3("回転(中央値)", &data.emitData.rotate.median.x, 0.1f);
		ImGui::DragFloat3("回転(振れ幅)", &data.emitData.rotate.range.x, 0.1f);
		ImGui::Checkbox("回転速度を使用するか", &data.isFlag.isRotateVelocity);
		ImGui::DragFloat3("回転速度(中央値)", &data.emitData.rotateVelocity.median.x, 0.1f);
		ImGui::DragFloat3("回転速度(振れ幅)", &data.emitData.rotateVelocity.range.x, 0.1f);
		
	}
	if (ImGui::CollapsingHeader("拡縮")) {
		ImGui::DragFloat3("サイズ(中央値)", &data.emitData.size.median.x, 0.1f);
		ImGui::DragFloat3("サイズ(振れ幅)", &data.emitData.size.range.x, 0.1f);
		ImGui::Checkbox("生存時間でのサイズ変化を使用するか", &data.isFlag.isLifeTimeScale_);
		ImGui::Checkbox("サイズ変化を使用するか", &data.isFlag.isScaling_);

		ImGui::DragFloat3("サイズ変化量(中央値)", &data.emitData.sizeAmount.median.x, 0.1f);
		ImGui::DragFloat3("サイズ変化量(振れ幅)", &data.emitData.sizeAmount.range.x, 0.1f);
		static const char* TopBottomLabels[] = {
		"Top",
		"Bottom",
		};

		Engine::ImGuiManager::Select("拡縮方向", TopBottomLabels, data.topBottom);

	}
	if (ImGui::CollapsingHeader("色")) {
		ImGui::ColorEdit4("最小値", &data.emitData.colorRange.min.x);
		ImGui::ColorEdit4("最大値", &data.emitData.colorRange.max.x);
		ImGui::Checkbox("透過するか", &data.isFlag.isAlpha);
		ImGui::DragFloat("透過クリップ値", &data.alphaClipping);		
	}

	if (ImGui::CollapsingHeader("形状によってのパラメータ")) {
		switch (data.shapeType)
		{
		case EmitterShapeType::AABB:
		{
			ImGui::DragFloat3("AABB(最小値)", &data.rangeMin.x, 0.1f);
			ImGui::DragFloat3("AABB(最大値)", &data.rangeMax.x, 0.1f);
			if (data.rangeMin.x > data.rangeMax.x) {
				data.rangeMax.x = data.rangeMin.x;
			}
			if (data.rangeMin.y > data.rangeMax.y) {
				data.rangeMax.y = data.rangeMin.y;
			}
			if (data.rangeMin.z > data.rangeMax.z) {
				data.rangeMax.z = data.rangeMin.z;
			}
		}
		break;
		case EmitterShapeType::SPHERE:
		{
			Engine::SphereParticleEmitter* sphereEmit = effectComponent->GetEmitterAs<Engine::SphereParticleEmitter>(name);
			sphereEmit->SetRadius(data.radius);
		}
		break;
		case EmitterShapeType::CIRCLE:
		{
			Engine::CornerParticleEmitter* circleEmit = effectComponent->GetEmitterAs<Engine::CornerParticleEmitter>(name);
			circleEmit->SetRadius(data.radius);
			circleEmit->SetSegment(data.segment);
		}
		break;
		case EmitterShapeType::POINT:
		{

		}
		break;
		case EmitterShapeType::LINE:
		{
			Engine::LineParticleEmitter* lineEmit = effectComponent->GetEmitterAs<Engine::LineParticleEmitter>(name);
			lineEmit->SetSegment(data.lineStart, data.lineEnd);
		}
		break;
		case EmitterShapeType::SPLINE:
		{
			Engine::SplineParticleEmitter* splineEmit = effectComponent->GetEmitterAs<Engine::SplineParticleEmitter>(name);
			for (const auto& point : data.splinePoints) {
				splineEmit->AddControlPoints(point);
			}
		}
		break;
		case EmitterShapeType::TRIANGLE:
		{
			Engine::TriangleParticleEmitter* triangleEmit = effectComponent->GetEmitterAs<Engine::TriangleParticleEmitter>(name);
			triangleEmit->SetTriangle(data.triangle);
		}
		break;
		case EmitterShapeType::MESH:
			break;
		default:
			break;
		}
	}
	ImGui::End();
}

void EffectEditor::Emit(const std::string& name, const Vector3& pos) {
	Engine::BaseParticleEmitter* emit = effectComponent->GetBaseEmitter(name);
	if (emit == nullptr) return;	// エミッターが存在しない場合は終了
	// 出現
	emit->SetPos(pos);		// 位置
	emit->SetIsEmit(false); // 出さない
	emit->Update();			// 更新
	emit->SetIsEmit(true);	// 出す
	emit->Emit();			// エフェクト出現
	emit->SetIsEmit(false); // 出さない
};
