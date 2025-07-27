#include "DirectXGame/engine/Camera/Camera.h"
#include "DirectXGame/engine/DirectX/Common/DirectXCommon.h"
#include "RenderingCommon.h"

//#include "DirectXGame/engine/DirectX/Resource/ConstantBuffer.h"


void RenderingCommon::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;
	copy_.Initialize(dxCommon_,"CopyImage");
	grayScale_.Initialize(dxCommon_, "GrayScale");
	sepia_.Initialize(dxCommon_,"Sepia");
	vignette_.Initialize(dxCommon_, "Vignette");
	smoothing_.Initialize(dxCommon_, "Smoothing");
	gaussian_.Initialize(dxCommon_, "Gaussian");
	outline_.Initialize(dxCommon_, "Outline");
	radialBlur_.Initialize(dxCommon_, "RadialBlur");
	dissovle_.Initialize(dxCommon_, "Dissovle");
	random_.Initialize(dxCommon_, "Random");
	bloom_.Initialize(dxCommon_, "BloomExtract");
	cimbin_.Initialize(dxCommon_, "BloomCombine");
	
}

void RenderingCommon::UpdateImgui(PostEffectType type)
{
	switch (type)
	{
	case PostEffectType::kCopy:
		copy_.UpdateImgui();
		break;
	case PostEffectType::kGrayScale:
		grayScale_.UpdateImgui();
		break;
	case PostEffectType::kSepia:
		sepia_.UpdateImgui();
		break;
	case PostEffectType::kVignette:
		vignette_.UpdateImgui();
		break;
	case PostEffectType::kSmoothing:
		smoothing_.UpdateImgui();
		break;
	case PostEffectType::kGaussian:
		gaussian_.UpdateImgui();
		break;
	case PostEffectType::kOitline:
		outline_.UpdateImgui();
		break;
	case PostEffectType::kRadialBlur:
		radialBlur_.UpdateImgui();
		break;
	case PostEffectType::kDissovle:
		dissovle_.UpdateImgui();
		break;
	case PostEffectType::kRandom:
		random_.UpdateImgui();
		break;
	case PostEffectType::kBloom:
		bloom_.UpdateImgui();
		break;
	case PostEffectType::kBloomCombin:
		cimbin_.UpdateImgui();
		break;
	default:
		break;
	}

}

void RenderingCommon::DrawRender(PostEffectType type, int index, int indexB)
{

	switch (type)
	{
	case PostEffectType::kCopy:
		copy_.DrawRender(index, indexB);
		break;
	case PostEffectType::kGrayScale:
		grayScale_.DrawRender(index, indexB);
		break;
	case PostEffectType::kSepia:
		sepia_.DrawRender(index, indexB);
		break;
	case PostEffectType::kVignette:
		vignette_.DrawRender(index, indexB);
		break;
	case PostEffectType::kSmoothing:
		smoothing_.DrawRender(index, indexB);
		break;
	case PostEffectType::kGaussian:
		gaussian_.DrawRender(index, indexB);
		break;
	case PostEffectType::kOitline:
		outline_.SetCamera(camera_);
		outline_.DrawRender(index, indexB);
		break;
	case PostEffectType::kRadialBlur:
		radialBlur_.DrawRender(index, indexB);
		break;
	case PostEffectType::kDissovle:
		dissovle_.DrawRender(index, indexB);
		break;
	case PostEffectType::kRandom:
		random_.DrawRender(index, indexB);
		break;
	case PostEffectType::kBloom:
		bloom_.DrawRender(index, indexB);
		break;
	case PostEffectType::kBloomCombin:
		cimbin_.DrawRender(indexB,index);
		break;
	default:
		break;
	}

}





