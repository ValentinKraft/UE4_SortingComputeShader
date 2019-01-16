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

#include "ComputeShaderPrivatePCH.h"
#include "ShaderParameterUtils.h"
#include "RHIStaticStates.h"

//These are needed to actually implement the constant buffers so they are available inside our shader
//They also need to be unique over the entire solution since they can in fact be accessed from any shader
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FComputeShaderConstantParameters, TEXT("CSConstants"))
IMPLEMENT_UNIFORM_BUFFER_STRUCT(FComputeShaderVariableParameters, TEXT("CSVariables"))

FComputeShaderDeclaration::FComputeShaderDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
: FGlobalShader(Initializer)
{
	//This call is what lets the shader system know that the surface OutputTexture is going to be available in the shader. The second parameter is the name it will be known by in the shader
	OutputTexture.Bind(Initializer.ParameterMap, TEXT("OutputTexture"));
	PointPosData.Bind(Initializer.ParameterMap, TEXT("PointPosData"));
	PointPosDataBuffer.Bind(Initializer.ParameterMap, TEXT("PointPosDataBuffer"));
}

void FComputeShaderDeclaration::ModifyCompilationEnvironment(EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Platform, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}

void FComputeShaderDeclaration::SetOutputTexture(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef OutputSurfaceUAV)
{
	FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();

	if (OutputTexture.IsBound())
		RHICmdList.SetUAVParameter(ComputeShaderRHI, OutputTexture.GetBaseIndex(), OutputSurfaceUAV);
}

void FComputeShaderDeclaration::SetPointPosData(FRHICommandList& RHICmdList, FUnorderedAccessViewRHIRef BufferUAV, FUnorderedAccessViewRHIRef BufferUAV2) {

	FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();

	if (PointPosData.IsBound())
		RHICmdList.SetUAVParameter(ComputeShaderRHI, PointPosData.GetBaseIndex(), BufferUAV);
	if (PointPosDataBuffer.IsBound())
		RHICmdList.SetUAVParameter(ComputeShaderRHI, PointPosDataBuffer.GetBaseIndex(), BufferUAV2);
}

void FComputeShaderDeclaration::SetUniformBuffers(FRHICommandList& RHICmdList, FComputeShaderConstantParameters& ConstantParameters, FComputeShaderVariableParameters& VariableParameters)
{
	FComputeShaderConstantParametersRef ConstantParametersBuffer;
	FComputeShaderVariableParametersRef VariableParametersBuffer;

	ConstantParametersBuffer = FComputeShaderConstantParametersRef::CreateUniformBufferImmediate(ConstantParameters, UniformBuffer_SingleDraw);
	VariableParametersBuffer = FComputeShaderVariableParametersRef::CreateUniformBufferImmediate(VariableParameters, UniformBuffer_SingleDraw);

	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FComputeShaderConstantParameters>(), ConstantParametersBuffer);
	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FComputeShaderVariableParameters>(), VariableParametersBuffer);
}

/* Unbinds buffers that will be used elsewhere */
void FComputeShaderDeclaration::UnbindBuffers(FRHICommandList& RHICmdList)
{
	FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();

	if (OutputTexture.IsBound())
		RHICmdList.SetUAVParameter(ComputeShaderRHI, OutputTexture.GetBaseIndex(), FUnorderedAccessViewRHIRef());
	if (PointPosData.IsBound())
		RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, PointPosData.GetBaseIndex(), FShaderResourceViewRHIParamRef());
}

/////////////////////////////////////////////////////////////////////////////

FComputeShaderTransposeDeclaration::FComputeShaderTransposeDeclaration(const ShaderMetaType::CompiledShaderInitializerType& Initializer)
: FGlobalShader(Initializer)
{
	//This call is what lets the shader system know that the surface OutputTexture is going to be available in the shader. The second parameter is the name it will be known by in the shader
	PointPosData.Bind(Initializer.ParameterMap, TEXT("PointPosData"));
	PointPosDataBuffer.Bind(Initializer.ParameterMap, TEXT("PointPosDataBuffer"));
}

void FComputeShaderTransposeDeclaration::ModifyCompilationEnvironment(EShaderPlatform Platform, FShaderCompilerEnvironment& OutEnvironment)
{
	FGlobalShader::ModifyCompilationEnvironment(Platform, OutEnvironment);
	OutEnvironment.CompilerFlags.Add(CFLAG_StandardOptimization);
}

void FComputeShaderTransposeDeclaration::SetUniformBuffers(FRHICommandList& RHICmdList, FComputeShaderConstantParameters& ConstantParameters, FComputeShaderVariableParameters& VariableParameters)
{
	FComputeShaderConstantParametersRef ConstantParametersBuffer;
	FComputeShaderVariableParametersRef VariableParametersBuffer;

	ConstantParametersBuffer = FComputeShaderConstantParametersRef::CreateUniformBufferImmediate(ConstantParameters, UniformBuffer_SingleDraw);
	VariableParametersBuffer = FComputeShaderVariableParametersRef::CreateUniformBufferImmediate(VariableParameters, UniformBuffer_SingleDraw);

	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FComputeShaderConstantParameters>(), ConstantParametersBuffer);
	SetUniformBufferParameter(RHICmdList, GetComputeShader(), GetUniformBufferParameter<FComputeShaderVariableParameters>(), VariableParametersBuffer);
}

/* Unbinds buffers that will be used elsewhere */
void FComputeShaderTransposeDeclaration::UnbindBuffers(FRHICommandList& RHICmdList)
{
	FComputeShaderRHIParamRef ComputeShaderRHI = GetComputeShader();

	if (PointPosData.IsBound())
		RHICmdList.SetShaderResourceViewParameter(ComputeShaderRHI, PointPosData.GetBaseIndex(), FShaderResourceViewRHIParamRef());
}

//This is what will instantiate the shader into the engine from the engine/Shaders folder
//                      ShaderType                    ShaderFileName                Shader function name       Type
IMPLEMENT_SHADER_TYPE(, FComputeShaderDeclaration, TEXT("/Plugin/ComputeShader/Private/BitonicSortingKernelComputeShader.usf"), TEXT("MainComputeShader"), SF_Compute);
IMPLEMENT_SHADER_TYPE(, FComputeShaderTransposeDeclaration, TEXT("/Plugin/ComputeShader/Private/BitonicSortingKernelComputeShader.usf"), TEXT("TransposeMatrix"), SF_Compute);

//This is required for the plugin to build :)
IMPLEMENT_MODULE(FDefaultModuleImpl, ComputeShader)