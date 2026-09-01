#include "TrailEffect.h"

#include "DirectXGame/engine/Manager/Effect/EffectManager.h"
#include "DirectXGame/engine/MyGame/MyGame.h"
#include "DirectXGame/engine/Math/LineCurveMath.h"
#include "DirectXGame/engine/Line/LineCommon.h"

#include <algorithm>
#include <cmath>

namespace Engine
{
	Vector3 EvaluateTrailTrajectory(const TrailTrajectorySettings& trajectory, float normalizedTime)
	{
		// エディタ設定値で軌道の範囲外を評価しないよう正規化時間を制限する。
		normalizedTime = std::clamp(normalizedTime, 0.0f, 1.0f);
		if (trajectory.type == TrailTrajectoryType::kBezier) {
			const float inverse = 1.0f - normalizedTime;
			const float a = inverse * inverse * inverse;
			const float b = 3.0f * inverse * inverse * normalizedTime;
			const float c = 3.0f * inverse * normalizedTime * normalizedTime;
			const float d = normalizedTime * normalizedTime * normalizedTime;
			return trajectory.point0 * a + trajectory.point1 * b + trajectory.point2 * c + trajectory.point3 * d;
		}
		if (trajectory.type == TrailTrajectoryType::kCatmullRom) {
			return CatmullRom({ trajectory.point0, trajectory.point1, trajectory.point2, trajectory.point3 }, normalizedTime);
		}
		if (trajectory.type == TrailTrajectoryType::kOrbit) {
			const float angle = trajectory.orbitStartAngle + (trajectory.orbitEndAngle - trajectory.orbitStartAngle) * normalizedTime;
			return { trajectory.orbitCenter.x + std::cos(angle) * trajectory.orbitRadius,
				trajectory.orbitCenter.y + trajectory.orbitHeight,
				trajectory.orbitCenter.z + std::sin(angle) * trajectory.orbitRadius };
		}
		return {};
	}
}
namespace {
	constexpr size_t kVerticesPerSegment = 6;

	VertexData MakeTrailVertex(const Vector3& position, const Vector2& uv)
	{
		return {
			.position = { position.x, position.y, position.z, 1.0f },
			.texcoord = uv,
			.normal = { 0.0f, 0.0f, 1.0f },
			.tangent = {}
		};
	}

	bool DebugFeatureCheckbox(const char* label, Engine::TrailFeature feature, Engine::TrailSettings& settings)
	{
		bool enabled = (settings.features & feature) != Engine::TrailFeature::None;
		if (!ImGui::Checkbox(label, &enabled)) {
			return false;
		}

		const uint32_t current = static_cast<uint32_t>(settings.features);
		const uint32_t mask = static_cast<uint32_t>(feature);
		settings.features = static_cast<Engine::TrailFeature>(enabled ? (current | mask) : (current & ~mask));
		return true;
	}

	float EvaluateAlphaCurve(const Vector4& curve, float t)
	{
		// 4点を三次ベジェ曲線の値として扱い、トレイル全体のアルファを補間する。
		const float inverse = 1.0f - t;
		return inverse * inverse * inverse * curve.x + 3.0f * inverse * inverse * t * curve.y +
			3.0f * inverse * t * t * curve.z + t * t * t * curve.w;
	}

	void AppendTrailQuad(std::vector<VertexData>& vertices,
		Vector3 currentStart, Vector3 currentEnd, Vector3 previousStart, Vector3 previousEnd,
		float currentU, float previousU, float currentWidth, float previousWidth)
	{
		// レールの中心から幅倍率を適用し、開始幅から終了幅へ補間する。
		const Vector3 currentCenter = (currentStart + currentEnd) * 0.5f;
		const Vector3 previousCenter = (previousStart + previousEnd) * 0.5f;
		currentStart = currentCenter + (currentStart - currentCenter) * currentWidth;
		currentEnd = currentCenter + (currentEnd - currentCenter) * currentWidth;
		previousStart = previousCenter + (previousStart - previousCenter) * previousWidth;
		previousEnd = previousCenter + (previousEnd - previousCenter) * previousWidth;

		const Vector2 uvCurrentTop = { currentU, 0.0f };
		const Vector2 uvCurrentBottom = { currentU, 1.0f };
		const Vector2 uvPreviousTop = { previousU, 0.0f };
		const Vector2 uvPreviousBottom = { previousU, 1.0f };

		vertices.push_back(MakeTrailVertex(currentStart, uvCurrentTop));
		vertices.push_back(MakeTrailVertex(previousStart, uvPreviousTop));
		vertices.push_back(MakeTrailVertex(currentEnd, uvCurrentBottom));
		vertices.push_back(MakeTrailVertex(currentEnd, uvCurrentBottom));
		vertices.push_back(MakeTrailVertex(previousStart, uvPreviousTop));
		vertices.push_back(MakeTrailVertex(previousEnd, uvPreviousBottom));
	}
}

void Engine::TrailEffect::Initialize(EffectManager* effectManager ,const std::string& tex,float maxtime  ,const Color color)
{
	// エフェクト管理クラス
	this->effectManager = effectManager;
	lifeTime_ = (std::max)(maxtime, 0.01f);
	timer = lifeTime_;
	baseColor_ = color;
	settings_.features = TrailFeature::Ribbon;
	settings_.minEmitDistance = minEmitDistance_;
	settings_.maxSegmentCount = maxSegmentCount_;

	// メッシュ生成。最大セグメント分のバッファを先に確保し、更新時の再確保を減らす。
	mesh = std::make_unique<ModelMesh>();
	mesh->vertices.resize(maxSegmentCount_ * kVerticesPerSegment);
	mesh->Initialize(effectManager->GetDxCommon());
	mesh->vertices.clear();
	mesh->ClearIndices();
	mesh->SetMaxTime(lifeTime_);

	// マテリアル初期化
	material = std::make_unique<Material>();
	material->Initialize(effectManager->GetDxCommon());
	material->tex_.diffuseFilePath = tex;
	material->GetMaterialInstance().color = color;

	// トランスフォーム初期化
	transformation = std::make_unique<Transformation>();
	transformation->Initialize(effectManager->GetDxCommon());

	parentTransform_.Identity();
	mat_.Identity();
}

void Engine::TrailEffect::Update()
{
	worldtransformTstr_.Update();
	worldtransformTend_.Update();

	Vector3 start = worldtransformTstr_.worldMat_.GetWorldPosition();
	Vector3 end = worldtransformTend_.worldMat_.GetWorldPosition();
	const float deltaTime = MyGame::GameTime();

	// 独立した軌道を進め、リボンの両レールを同じ量だけ移動させる。
	if (trajectoryEnabled_) {
		trajectoryElapsed_ += deltaTime;
		const float duration = trajectory_.duration > 0.0f ? trajectory_.duration : lifeTime_;
		const float normalizedTime = std::clamp(trajectoryElapsed_ / (std::max)(duration, 0.001f), 0.0f, 1.0f);
		const Vector3 pathOffset = ToTrajectoryWorldPosition(EvaluateTrajectory(normalizedTime)) - trajectoryAnchorPosition_;
		start = trajectoryStartOffset_ + pathOffset;
		end = trajectoryEndOffset_ + pathOffset;
	}

	// 発生していない間も最後の座標は追従させ、再発生時に長い帯が一気に伸びるのを防ぐ。
	if (flag_ && HasFeature(TrailFeature::Ribbon)) {
		EmitSegment(start, end);
	}
	else {
		lastStart_ = start;
		lastEnd_ = end;
		hasLastSample_ = true;
	}

	for (TrailSegment& segment : segments_) {
		segment.age += deltaTime;
	}

	const size_t oldCount = segments_.size();
	while (!segments_.empty() && segments_.front().age >= lifeTime_) {
		segments_.pop_front();
	}
	if (oldCount != segments_.size()) {
		meshDirty_ = true;
	}

	if (meshDirty_) {
		RebuildMesh();
		mesh->UpdateVertexBuffer();
		meshDirty_ = false;
	}

	// トレイルは頂点側がワールド座標なので、行列は単位行列として扱う。
	parentTransform_ = MakeAffineMatrix({ 1,1,1 }, Vector3{ 0,0,0 }, { 0,0,0 });
	transformation->Update(camera_, parentTransform_);

	// 発生開始からの経過時間を使ってフェードとアルファカーブを反映する。
	effectElapsed_ += deltaTime;
	float fade = 1.0f;
	if (settings_.fadeInTime > 0.0f) {
		fade = (std::min)(fade, effectElapsed_ / settings_.fadeInTime);
	}
	if (settings_.fadeOutTime > 0.0f) {
		fade = (std::min)(fade, (std::max)(0.0f, (lifeTime_ - effectElapsed_) / settings_.fadeOutTime));
	}
	// フェード未使用時は、アルファカーブの初期値によってトレイル全体が透明にならないようにする。
	const bool hasFadeSetting = settings_.fadeInTime > 0.0f || settings_.fadeOutTime > 0.0f;
	const float curveAlpha = hasFadeSetting ? std::clamp(EvaluateAlphaCurve(settings_.alphaCurve, std::clamp(fade, 0.0f, 1.0f)), 0.0f, 1.0f) : 1.0f;
	material->GetMaterialInstance().color = { baseColor_.r * settings_.emissionStrength, baseColor_.g * settings_.emissionStrength,
		baseColor_.b * settings_.emissionStrength, baseColor_.a * fade * curveAlpha };
	material->GetMaterialInstance().transform.scale.x = settings_.flipTexture ? -1.0f : 1.0f;
	UpdateComposableModules(deltaTime);
	material->GPUData();

	if (debugDraw_) {
		DrawDebugLines();
	}
}

void Engine::TrailEffect::Draw()
{
	// 頂点があるなら
	if (!mesh->vertices.empty()) {
		// 描画前準備
		effectManager->GetTrailEffectCommon()->DrawCommonSetting();

		// 位置
		transformation->GetCommandList(1);

		// マテリアル
		material->GetCommandListMaterial(0);

		// テクスチャ
		material->GetCommandListTexture(2, 7, 8);

		// メッシュ
		mesh->GetCommandList();

		effectManager->GetDxCommon()->GetCommandList()->DrawInstanced(UINT(mesh->vertices.size()), 1, 0, 0);
	}
}

void Engine::TrailEffect::UpdateImgui()
{
#ifdef _DEBUG
	if (!ImGui::CollapsingHeader("Trail Debug")) {
		return;
	}

	TrailSettings debugSettings = settings_;
	bool changedSettings = false;

	ImGui::Text("Runtime State");
	ImGui::Checkbox("Emit", &flag_);
	ImGui::Text("Segments : %d", static_cast<int>(segments_.size()));
	ImGui::Text("Vertices : %d", mesh ? static_cast<int>(mesh->vertices.size()) : 0);
	ImGui::Text("Dirty    : %s", meshDirty_ ? "true" : "false");

	if (ImGui::Button("Clear Trail")) {
		segments_.clear();
		if (mesh) {
			mesh->vertices.clear();
			mesh->UpdateVertexBuffer();
		}
		meshDirty_ = false;
	}

	ImGui::Separator();
	ImGui::Text("Debug Draw");
	ImGui::Checkbox("Draw Trail Debug", &debugDraw_);
	ImGui::BeginDisabled(!debugDraw_);
	ImGui::Checkbox("Draw Rails", &debugDrawRails_);
	ImGui::Checkbox("Draw Mesh Edges", &debugDrawMesh_);
	ImGui::Checkbox("Draw Points", &debugDrawPoints_);
	ImGui::EndDisabled();

	ImGui::Separator();
	ImGui::Text("Features");
	changedSettings |= DebugFeatureCheckbox("Ribbon", TrailFeature::Ribbon, debugSettings);
	changedSettings |= DebugFeatureCheckbox("AfterImage", TrailFeature::AfterImage, debugSettings);
	changedSettings |= DebugFeatureCheckbox("Mesh Trail", TrailFeature::Mesh, debugSettings);
	changedSettings |= DebugFeatureCheckbox("Particle Trail", TrailFeature::Particle, debugSettings);
	changedSettings |= DebugFeatureCheckbox("UV Scroll", TrailFeature::UVScroll, debugSettings);
	changedSettings |= DebugFeatureCheckbox("Distortion", TrailFeature::Distortion, debugSettings);
	changedSettings |= DebugFeatureCheckbox("Dissolve", TrailFeature::Dissolve, debugSettings);

	ImGui::Separator();
	ImGui::Text("Ribbon");
	float lifeTime = lifeTime_;
	if (ImGui::DragFloat("Life Time", &lifeTime, 0.01f, 0.01f, 10.0f)) {
		lifeTime_ = (std::max)(lifeTime, 0.01f);
		timer = lifeTime_;
		if (mesh) {
			mesh->SetMaxTime(lifeTime_);
		}
		meshDirty_ = true;
	}
	changedSettings |= ImGui::DragFloat("Min Emit Distance", &debugSettings.minEmitDistance, 0.001f, 0.0f, 10.0f);
	changedSettings |= ImGui::Checkbox("Use Spline", &debugSettings.useSpline);

	if (ImGui::DragInt("Spline Subdivision", &debugSettings.splineSubdivision, 1.0f, 1, 16)) {
		debugSettings.splineSubdivision = std::clamp(debugSettings.splineSubdivision, 1, 16);
		changedSettings = true;
	}

	int maxSegmentCount = static_cast<int>(debugSettings.maxSegmentCount);
	if (ImGui::DragInt("Max Segment Count", &maxSegmentCount, 1.0f, 1, 2048)) {
		debugSettings.maxSegmentCount = static_cast<size_t>((std::max)(1, maxSegmentCount));
		changedSettings = true;
	}

	ImGui::ColorEdit4("Color", &baseColor_.r);

	ImGui::Separator();
	ImGui::Text("UV Scroll");
	changedSettings |= ImGui::DragFloat2("UV Scroll Speed", &debugSettings.uvScrollSpeed.x, 0.01f);
	ImGui::Text("UV Offset : %.3f, %.3f", uvScrollOffset_.x, uvScrollOffset_.y);

	ImGui::Separator();
	ImGui::Text("Dissolve");
	changedSettings |= ImGui::DragFloat("Dissolve Speed", &debugSettings.dissolveSpeed, 0.01f, 0.0f, 100.0f);
	changedSettings |= ImGui::DragFloat("Dissolve Clip Min", &debugSettings.dissolveAlphaClipMin, 0.01f, 0.0f, 1.0f);
	changedSettings |= ImGui::DragFloat("Dissolve Clip Max", &debugSettings.dissolveAlphaClipMax, 0.01f, 0.0f, 1.0f);

	ImGui::Separator();
	ImGui::Text("Reserved Modules");
	changedSettings |= ImGui::DragInt("AfterImage Count", &debugSettings.afterImageCount, 1.0f, 0, 64);
	changedSettings |= ImGui::DragFloat("Mesh Trail Interval", &debugSettings.meshTrailInterval, 0.001f, 0.0f, 10.0f);
	changedSettings |= ImGui::DragFloat("Particle Emit Interval", &debugSettings.particleEmitInterval, 0.001f, 0.0f, 10.0f);
	changedSettings |= ImGui::DragFloat("Distortion Strength", &debugSettings.distortionStrength, 0.01f, 0.0f, 100.0f);

	if (changedSettings) {
		// デバッグ値は保存せず、その場のTrailSettingsだけに反映する。
		SetSettings(debugSettings);
	}
#endif // _DEBUG
}

void Engine::TrailEffect::SetQuality(float minEmitDistance, size_t maxSegmentCount)
{
	minEmitDistance_ = (std::max)(0.0f, minEmitDistance);
	maxSegmentCount_ = (std::max)(size_t{ 1 }, maxSegmentCount);
	settings_.minEmitDistance = minEmitDistance_;
	settings_.maxSegmentCount = maxSegmentCount_;

	while (segments_.size() > maxSegmentCount_) {
		segments_.pop_front();
	}
	meshDirty_ = true;
}

void Engine::TrailEffect::SetIsEmit(bool is)
{
	// 発生開始時に履歴を消去し、前回の位置から不自然な帯が伸びることを防ぐ。
	if (is && !flag_ && settings_.resetMode == TrailResetMode::kClearOnEmitStart) {
		segments_.clear();
		hasLastSample_ = false;
		meshDirty_ = true;
		effectElapsed_ = 0.0f;
	}
	flag_ = is;
}

void Engine::TrailEffect::SetSettings(const TrailSettings& settings)
{
	settings_ = settings;
	settings_.splineSubdivision = std::clamp(settings_.splineSubdivision, 1, 16);
	SetQuality(settings_.minEmitDistance, settings_.maxSegmentCount);
	dissolveTime_ = 0.0f;
	uvScrollOffset_ = {};
}

void Engine::TrailEffect::SetTrajectory(const TrailTrajectorySettings& trajectory, WorldTransform* anchor)
{
	// エディタで従来の親追従を選択した場合は軌道モードを無効にする。
	trajectory_ = trajectory;
	trajectoryEnabled_ = trajectory_.type != TrailTrajectoryType::kNone;
	trajectoryElapsed_ = 0.0f;
	if (!trajectoryEnabled_) {
		return;
	}

	// 現在のリボンとアンカー基底を保存し、後の親移動で軌道が歪まないようにする。
	worldtransformTstr_.Update();
	worldtransformTend_.Update();
	trajectoryStartOffset_ = worldtransformTstr_.worldMat_.GetWorldPosition();
	trajectoryEndOffset_ = worldtransformTend_.worldMat_.GetWorldPosition();
	trajectoryAnchorPosition_ = anchor ? anchor->GetWorldPosition() : Vector3{};
	if (anchor) {
		trajectoryRight_ = { anchor->worldMat_.m[0][0], anchor->worldMat_.m[0][1], anchor->worldMat_.m[0][2] };
		trajectoryUp_ = { anchor->worldMat_.m[1][0], anchor->worldMat_.m[1][1], anchor->worldMat_.m[1][2] };
		trajectoryForward_ = { anchor->worldMat_.m[2][0], anchor->worldMat_.m[2][1], anchor->worldMat_.m[2][2] };
	}
}

Vector3 Engine::TrailEffect::EvaluateTrajectory(float normalizedTime) const
{
	// パーティクルでも使用する共有軌道評価処理へ委譲する。
	return EvaluateTrailTrajectory(trajectory_, normalizedTime);
}

Vector3 Engine::TrailEffect::ToTrajectoryWorldPosition(const Vector3& localPosition) const
{
	// 保存した右・上・前方向のアンカー基底でローカル軌道を変換する。
	return {
		trajectoryAnchorPosition_.x + trajectoryRight_.x * localPosition.x + trajectoryUp_.x * localPosition.y + trajectoryForward_.x * localPosition.z,
		trajectoryAnchorPosition_.y + trajectoryRight_.y * localPosition.x + trajectoryUp_.y * localPosition.y + trajectoryForward_.y * localPosition.z,
		trajectoryAnchorPosition_.z + trajectoryRight_.z * localPosition.x + trajectoryUp_.z * localPosition.y + trajectoryForward_.z * localPosition.z
	};
}
void Engine::TrailEffect::AddFeature(TrailFeature feature)
{
	settings_.features |= feature;
}

void Engine::TrailEffect::RemoveFeature(TrailFeature feature)
{
	const uint32_t current = static_cast<uint32_t>(settings_.features);
	const uint32_t remove = static_cast<uint32_t>(feature);
	settings_.features = static_cast<TrailFeature>(current & ~remove);
}

bool Engine::TrailEffect::HasFeature(TrailFeature feature) const
{
	return (settings_.features & feature) != TrailFeature::None;
}

void Engine::TrailEffect::EmitSegment(const Vector3& start, const Vector3& end)
{
	if (!hasLastSample_) {
		lastStart_ = start;
		lastEnd_ = end;
		hasLastSample_ = true;
		return;
	}

	// 微小移動では頂点を増やさない。手ぶれのような細かい揺れも抑えられる。
	const float movedStart = Length(start - lastStart_);
	const float movedEnd = Length(end - lastEnd_);
	if (movedStart < minEmitDistance_ && movedEnd < minEmitDistance_) {
		return;
	}

	segments_.push_back({
		.start = start,
		.end = end,
		.prevStart = lastStart_,
		.prevEnd = lastEnd_,
		.age = 0.0f
		});

	while (segments_.size() > maxSegmentCount_) {
		segments_.pop_front();
	}

	lastStart_ = start;
	lastEnd_ = end;
	meshDirty_ = true;
}

void Engine::TrailEffect::RebuildMesh()
{
	mesh->vertices.clear();
	mesh->vertices.reserve(segments_.size() * kVerticesPerSegment);

	if (segments_.empty()) {
		return;
	}

	if (settings_.useSpline && segments_.size() >= 3) {
		RebuildSplineMesh();
	}
	else {
		RebuildLinearMesh();
	}
}

void Engine::TrailEffect::RebuildLinearMesh()
{
	const float invLifeTime = 1.0f / lifeTime_;
	for (const TrailSegment& segment : segments_) {
		const float ageRate = std::clamp(segment.age * invLifeTime, 0.0f, 1.0f);
		const float nextAgeRate = std::clamp(ageRate + (1.0f / static_cast<float>((std::max)(size_t{ 1 }, segments_.size()))), 0.0f, 1.0f);

		// 1セグメントを必ず2三角形単位で生成し、寿命削除で三角形が崩れないようにする。
		const float currentWidth = settings_.widthStart + (settings_.widthEnd - settings_.widthStart) * ageRate;
		const float previousWidth = settings_.widthStart + (settings_.widthEnd - settings_.widthStart) * nextAgeRate;
		AppendTrailQuad(mesh->vertices, segment.start, segment.end, segment.prevStart, segment.prevEnd, ageRate, nextAgeRate, currentWidth, previousWidth);
	}
}

void Engine::TrailEffect::RebuildSplineMesh()
{
	std::vector<Vector3> startRail;
	std::vector<Vector3> endRail;
	startRail.reserve(segments_.size() + 1);
	endRail.reserve(segments_.size() + 1);

	// start側/end側を別々の制御点列として補間し、同じtで結ぶことで滑らかな帯にする。
	startRail.push_back(segments_.front().prevStart);
	endRail.push_back(segments_.front().prevEnd);
	for (const TrailSegment& segment : segments_) {
		startRail.push_back(segment.start);
		endRail.push_back(segment.end);
	}

	if (startRail.size() < 4 || endRail.size() < 4) {
		RebuildLinearMesh();
		return;
	}

	const int subdivision = std::clamp(settings_.splineSubdivision, 1, 16);
	const size_t sampleCount = ((startRail.size() - 1) * static_cast<size_t>(subdivision)) + 1;
	if (sampleCount < 2) {
		return;
	}

	std::vector<Vector3> sampledStart;
	std::vector<Vector3> sampledEnd;
	sampledStart.reserve(sampleCount);
	sampledEnd.reserve(sampleCount);

	for (size_t i = 0; i < sampleCount; ++i) {
		const float t = static_cast<float>(i) / static_cast<float>(sampleCount - 1);
		sampledStart.push_back(CatmullRom(startRail, t));
		sampledEnd.push_back(CatmullRom(endRail, t));
	}

	for (size_t i = 1; i < sampleCount; ++i) {
		const float currentU = static_cast<float>(i) / static_cast<float>(sampleCount - 1);
		const float previousU = static_cast<float>(i - 1) / static_cast<float>(sampleCount - 1);
		AppendTrailQuad(mesh->vertices,
			sampledStart[i],
			sampledEnd[i],
			sampledStart[i - 1],
			sampledEnd[i - 1],
			currentU,
			previousU,
			settings_.widthStart + (settings_.widthEnd - settings_.widthStart) * currentU,
			settings_.widthStart + (settings_.widthEnd - settings_.widthStart) * previousU);
	}
}

void Engine::TrailEffect::DrawDebugLines()
{
#ifdef _DEBUG
	if (!effectManager || !effectManager->GetLineCommon()) {
		return;
	}

	LineMeshData& lines = effectManager->GetLineCommon()->GetDebugLineMeshData();

	const Vector3 currentStart = worldtransformTstr_.worldMat_.GetWorldPosition();
	const Vector3 currentEnd = worldtransformTend_.worldMat_.GetWorldPosition();

	// 現在の発生断面は黄色で表示し、親やオフセットの位置ずれを確認しやすくする。
	lines.AddLine(currentStart, currentEnd, { 1.0f, 1.0f, 0.0f, 1.0f });

	if (debugDrawRails_) {
		for (const TrailSegment& segment : segments_) {
			lines.AddLine(segment.prevStart, segment.start, { 0.0f, 1.0f, 0.2f, 1.0f });
			lines.AddLine(segment.prevEnd, segment.end, { 0.1f, 0.7f, 1.0f, 1.0f });
			lines.AddLine(segment.start, segment.end, { 1.0f, 1.0f, 1.0f, 0.8f });
		}
	}

	if (debugDrawMesh_ && mesh) {
		for (size_t i = 0; i + 5 < mesh->vertices.size(); i += kVerticesPerSegment) {
			const Vector3 currentTop = { mesh->vertices[i + 0].position.x, mesh->vertices[i + 0].position.y, mesh->vertices[i + 0].position.z };
			const Vector3 previousTop = { mesh->vertices[i + 1].position.x, mesh->vertices[i + 1].position.y, mesh->vertices[i + 1].position.z };
			const Vector3 currentBottom = { mesh->vertices[i + 2].position.x, mesh->vertices[i + 2].position.y, mesh->vertices[i + 2].position.z };
			const Vector3 previousBottom = { mesh->vertices[i + 5].position.x, mesh->vertices[i + 5].position.y, mesh->vertices[i + 5].position.z };

			lines.AddLine(previousTop, currentTop, { 1.0f, 0.2f, 1.0f, 1.0f });
			lines.AddLine(previousBottom, currentBottom, { 1.0f, 0.2f, 1.0f, 1.0f });
			lines.AddLine(currentTop, currentBottom, { 1.0f, 0.2f, 1.0f, 0.75f });
		}
	}

	if (debugDrawPoints_) {
		constexpr size_t kMaxDebugPoints = 128;
		size_t pointCount = 0;
		lines.AddLineSphere({ currentStart, 0.035f }, { 1.0f, 1.0f, 0.0f, 1.0f }, 4, 4);
		lines.AddLineSphere({ currentEnd, 0.035f }, { 1.0f, 1.0f, 0.0f, 1.0f }, 4, 4);

		for (const TrailSegment& segment : segments_) {
			if (pointCount++ >= kMaxDebugPoints) {
				break;
			}
			lines.AddLineSphere({ segment.start, 0.025f }, { 0.0f, 1.0f, 0.2f, 1.0f }, 4, 4);
			lines.AddLineSphere({ segment.end, 0.025f }, { 0.1f, 0.7f, 1.0f, 1.0f }, 4, 4);
		}
	}
#endif // _DEBUG
}

void Engine::TrailEffect::UpdateUvScroll(float deltaTime)
{
	uvScrollOffset_.x += settings_.uvScrollSpeed.x * deltaTime;
	uvScrollOffset_.y += settings_.uvScrollSpeed.y * deltaTime;

	MaterialInstance& materialInstance = material->GetMaterialInstance();
	materialInstance.transform.translate.x = uvScrollOffset_.x;
	materialInstance.transform.translate.y = uvScrollOffset_.y;
}

void Engine::TrailEffect::UpdateDissolve(float deltaTime)
{
	dissolveTime_ += deltaTime * settings_.dissolveSpeed;
	const float dissolveRate = std::clamp(std::fmod(dissolveTime_, lifeTime_) / lifeTime_, 0.0f, 1.0f);

	MaterialInstance& materialInstance = material->GetMaterialInstance();
	materialInstance.alphaClipping_ =
		settings_.dissolveAlphaClipMin +
		(settings_.dissolveAlphaClipMax - settings_.dissolveAlphaClipMin) * dissolveRate;
}

void Engine::TrailEffect::UpdateComposableModules(float deltaTime)
{
	// Ribbonはジオメトリ生成、UVScroll/Dissolveはマテリアル操作として合成する。
	if (HasFeature(TrailFeature::UVScroll)) {
		UpdateUvScroll(deltaTime);
	}
	if (HasFeature(TrailFeature::Dissolve)) {
		UpdateDissolve(deltaTime);
	}

	// AfterImage/Mesh/Particle/Distortionは同じ設定構造でONにできるよう入口を用意している。
	// 実際の描画先はモデルスナップショット、パーティクル、ポストエフェクト側に接続して拡張する。
}
