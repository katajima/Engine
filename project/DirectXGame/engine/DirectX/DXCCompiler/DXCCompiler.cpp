#include "DXCCompiler.h"

#include<format>
#include<cassert>

void DXCCompiler::Initialize()
{
	//dxcCompilerを初期化
	hr_ = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&dxcUtils_));
	assert(SUCCEEDED(hr_));
	hr_ = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&dxcCompiler_));
	assert(SUCCEEDED(hr_));


	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	hr_ = dxcUtils_->CreateDefaultIncludeHandler(&includeHandler_);
	assert(SUCCEEDED(hr_));
}

Microsoft::WRL::ComPtr<IDxcBlob> DXCCompiler::CompileShader(const std::wstring& filePach, const wchar_t* profile)
{
	////------hlslファイルを読み込む------////

		//これからシェーダをコンパイルする旨をログに出す
	Logger::Log(StringUtility::ConvertString(std::format(L"Begin CompileShader, path:{}, profile\n", filePach, profile)));
	//hlslファイルを読む
	IDxcBlobEncoding* shaderSource = nullptr;
	hr_ = dxcUtils_->LoadFile(filePach.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	assert(SUCCEEDED(hr_));
	//読み込んだファイル内容を設定する
	DxcBuffer shaderSourceBuffer;
	shaderSourceBuffer.Ptr = shaderSource->GetBufferPointer();
	shaderSourceBuffer.Size = shaderSource->GetBufferSize();
	shaderSourceBuffer.Encoding = DXC_CP_UTF8; // UTF8の文字コードであることを通知

	////------Compileする------////

	LPCWSTR arguments[] = {
		filePach.c_str(), // コンパイル対象のhlslファイル名
		L"-E",L"main", // エントリーポイントの指定。基本的にmain以外にはしない
		L"-T",profile, // shaderProfileの設定
		L"-Zi",L"-Qembed_debug", // デバッグ用の情報を埋め込む
		L"-Od",  //最適化を外しておく
		L"-Zpr"  //メモリレイアウトは行優先
	};

	//実際にShaderをコンパイルする
	IDxcResult* shaderResult = nullptr;
	hr_ = dxcCompiler_->Compile(
		&shaderSourceBuffer,   //読み込んだファイル
		arguments,             //コンパイルオプション
		_countof(arguments),   //コンパイルオプションの数
		includeHandler_,        //includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult) //コンパイル結果
	);

	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	assert(SUCCEEDED(hr_));


	////------警告・エラーが出ていないか確認する------////

	//警告・エラーが出たらログに出して止める
	IDxcBlobUtf8* shaderError = nullptr;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Logger::Log(shaderError->GetStringPointer());
		//警告・エラーダメゼッタイ
		assert(false);
	}


	////------Compile結果を受け取って返す------////

	//コンパイル結果から実行用のバイナリ部分を取得
	IDxcBlob* shaderBlob = nullptr;
	hr_ = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	assert(SUCCEEDED(hr_));
	//成功したログを出す
	Logger::Log(StringUtility::ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePach, profile)));
	//もう使わないリソースを解放
	shaderSource->Release();
	shaderResult->Release();
	//実行用バイナリを返却
	return shaderBlob;
}
