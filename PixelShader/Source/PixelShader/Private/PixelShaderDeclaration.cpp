/******************************************************************************
* The MIT License (MIT)
*
* Copyright (c) 2015 Fredrik Lindh
*
* Permission is hereby granted, free of charge, to any person obtaining a copy
* of this software and associated documentation files (the "Software"), to deal
* in the Software without restriction, including without limitation the rights
* to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
* copies of the Software, and to permit persons to whom the Software is
* furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in
* all copies or substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
* IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
* FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
* AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
* LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
* THE SOFTWARE.
******************************************************************************/

#include "PixelShaderPrivatePCH.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"

//These are needed to actually implement the constant buffers so they are available inside our shader
//They also need to be unique over the entire solution since they can in fact be accessed from any shader
IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FPixelShaderConstantParameters, "PSConstant");
IMPLEMENT_GLOBAL_SHADER_PARAMETER_STRUCT(FPixelShaderVariableParameters, "PSVariable");

FPixelShaderDeclaration::FPixelShaderDeclaration(const
	ShaderMetaType::CompiledShaderInitializerType& Initializer)
	: FGlobalShader(Initializer) {
	//This call is what lets the shader system know that the surface OutputSurface is going to be available in the shader. The second parameter is the name it will be known by in the shader
	TextureParameter.Bind(Initializer.ParameterMap,
		TEXT("TextureParameter"));  //The text parameter here is the name of the parameter in the shader
}

void FPixelShaderDeclaration::SetUniformBuffers(FRHICommandList& RHICmdList,
	FPixelShaderConstantParameters& ConstantParameters,
	FPixelShaderVariableParameters& VariableParameters) {
	FPixelShaderConstantParametersRef ConstantParametersBuffer;
	FPixelShaderVariableParametersRef VariableParametersBuffer;

	ConstantParametersBuffer =
		FPixelShaderConstantParametersRef::CreateUniformBufferImmediate(
			ConstantParameters, UniformBuffer_SingleDraw);
	VariableParametersBuffer =
		FPixelShaderVariableParametersRef::CreateUniformBufferImmediate(
			VariableParameters, UniformBuffer_SingleDraw);

	SetUniformBufferParameter(RHICmdList, GetPixelShader(),
		GetUniformBufferParameter<FPixelShaderConstantParameters>(),
		ConstantParametersBuffer);
	SetUniformBufferParameter(RHICmdList, GetPixelShader(),
		GetUniformBufferParameter<FPixelShaderVariableParameters>(),
		VariableParametersBuffer);
}

void FPixelShaderDeclaration::SetOutputTexture(FRHICommandList& RHICmdList,
	FShaderResourceViewRHIRef TextureParameterSRV) {
	FRHIPixelShader* PixelShaderRHI = GetPixelShader();

	if (TextureParameter.IsBound()) { //This actually sets the shader resource view to the texture parameter in the shader :)
		RHICmdList.SetShaderResourceViewParameter(PixelShaderRHI,
			TextureParameter.GetBaseIndex(), TextureParameterSRV);
	}
}

void FPixelShaderDeclaration::UnbindBuffers(FRHICommandList& RHICmdList) {
	FRHIPixelShader* PixelShaderRHI = GetPixelShader();

	if (TextureParameter.IsBound()) {
		RHICmdList.SetShaderResourceViewParameter(PixelShaderRHI,
			TextureParameter.GetBaseIndex(), new FRHIShaderResourceView());
	}
}

//This is what will instantiate the shader into the engine from the engine/Shaders folder
//                      ShaderType               ShaderFileName     Shader function name            Type
IMPLEMENT_SHADER_TYPE(, FVertexShaderExample, TEXT("/PixelShaderPlugin/PixelShaderExample.usf"),
	TEXT("MainVertexShader"), SF_Vertex);
IMPLEMENT_SHADER_TYPE(, FPixelShaderDeclaration, TEXT("/PixelShaderPlugin/PixelShaderExample.usf"),
	TEXT("MainPixelShader"), SF_Pixel);

class FPixelShaderModule : public IModuleInterface
{
	void StartupModule() override {
		FString ShaderDirectory = FPaths::Combine(FPaths::ProjectPluginsDir(), TEXT("PixelShader/Shaders/Private"));
		AddShaderSourceDirectoryMapping("/PixelShaderPlugin", ShaderDirectory);
	}
};

//Needed to make sure the plugin works :)
IMPLEMENT_MODULE(FPixelShaderModule, PixelShader)