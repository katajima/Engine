#include "DXCCompiler.h"

#include<format>
#include<cassert>
#include <stdexcept>

void Engine::DXCCompiler::Initialize()
{
	//dxcCompilerを初期化
	hr_ = DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(&mDxcUtils));
	if (FAILED(hr_)) throw std::runtime_error("DXC operation failed");
	hr_ = DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(&mDxcCompiler));
	if (FAILED(hr_)) throw std::runtime_error("DXC operation failed");


	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	hr_ = mDxcUtils->CreateDefaultIncludeHandler(&mIncludeHandler);
	if (FAILED(hr_)) throw std::runtime_error("DXC operation failed");
}

Microsoft::WRL::ComPtr<IDxcBlob> Engine::DXCCompiler::CompileShader(const std::wstring& filePach, const wchar_t* profile)
{
	////------hlslファイルを読み込む------////

		//これからシェーダをコンパイルする旨をログに出す
	Logger::Log(StringUtility::ConvertString(std::format(L"Begin CompileShader, path:{}, profile\n", filePach, profile)));
	//hlslファイルを読む
	Microsoft::WRL::ComPtr<IDxcBlobEncoding> shaderSource;
	hr_ = mDxcUtils->LoadFile(filePach.c_str(), nullptr, &shaderSource);
	//読めなかったら止める
	if (FAILED(hr_)) throw std::runtime_error("DXC operation failed");
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
	Microsoft::WRL::ComPtr<IDxcResult> shaderResult;
	hr_ = mDxcCompiler->Compile(
		&shaderSourceBuffer,   //読み込んだファイル
		arguments,             //コンパイルオプション
		_countof(arguments),   //コンパイルオプションの数
		mIncludeHandler.Get(),        //includeが含まれた諸々
		IID_PPV_ARGS(&shaderResult) //コンパイル結果
	);

	//コンパイルエラーではなくdxcが起動できないなど致命的な状況
	if (FAILED(hr_)) throw std::runtime_error("DXC operation failed");


	////------警告・エラーが出ていないか確認する------////

	//警告・エラーが出たらログに出して止める
	Microsoft::WRL::ComPtr<IDxcBlobUtf8> shaderError;
	shaderResult->GetOutput(DXC_OUT_ERRORS, IID_PPV_ARGS(&shaderError), nullptr);
	if (shaderError != nullptr && shaderError->GetStringLength() != 0) {
		Logger::Log(shaderError->GetStringPointer());

	}


	HRESULT compileStatus = S_OK;
	if (FAILED(shaderResult->GetStatus(&compileStatus)) || FAILED(compileStatus))
		throw std::runtime_error("Shader compilation failed");
	////------Compile結果を受け取って返す------////

	//コンパイル結果から実行用のバイナリ部分を取得
	Microsoft::WRL::ComPtr<IDxcBlob> shaderBlob;
	hr_ = shaderResult->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(&shaderBlob), nullptr);
	if (FAILED(hr_)) throw std::runtime_error("DXC operation failed");
	//成功したログを出す
	Logger::Log(StringUtility::ConvertString(std::format(L"Compile Succeeded,path:{},profile:{}\n", filePach, profile)));
	//もう使わないリソースを解放


	//実行用バイナリを返却
	return shaderBlob;
}
