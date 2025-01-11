#include <ShaderSystem.h>
#include <SDL3/SDL.h>
#include <slang.h>
#include <slang-com-ptr.h>
#include <vector>

Slang::ComPtr<slang::IGlobalSession> globalSession;

void ShaderSystem::Initialize()
{
    slang::createGlobalSession(globalSession.writeRef());
}

SDL_GPUShader *ShaderSystem::LoadShader(SDL_GPUDevice *device,
                                        const char *shaderFilename,
                                        const char *entryPoint,
                                        SDL_GPUShaderStage shaderStage)
{
    SDL_GPUShaderFormat BackendFormat = SDL_GetGPUShaderFormats(device);
    SDL_GPUShaderFormat SDLFormat = SDL_GPU_SHADERFORMAT_INVALID;

    std::vector<slang::TargetDesc> targetDescriptions;
    if (BackendFormat & SDL_GPU_SHADERFORMAT_SPIRV)
    {
        slang::TargetDesc targetDesc;
        targetDesc.format = SLANG_SPIRV;
        targetDesc.profile = globalSession->findProfile("SPIRV_1_6"); // TODO: Make compatible with whatever version installed
        targetDescriptions.push_back(targetDesc);

        SDLFormat = SDL_GPU_SHADERFORMAT_SPIRV;
    }
    else if (BackendFormat & SDL_GPU_SHADERFORMAT_MSL)
    {
        slang::TargetDesc targetDesc;
        targetDesc.format = SLANG_METAL;
        targetDesc.profile = globalSession->findProfile("METAL_2_4");
        targetDescriptions.push_back(targetDesc);

        SDLFormat = SDL_GPU_SHADERFORMAT_MSL;
    }
    else if (BackendFormat & SDL_GPU_SHADERFORMAT_DXIL)
    {
        slang::TargetDesc targetDesc;
        targetDesc.format = SLANG_DXIL;
        targetDesc.profile = globalSession->findProfile("DX_6_7");
        targetDescriptions.push_back(targetDesc);

        SDLFormat = SDL_GPU_SHADERFORMAT_DXIL;
    }
    else
    {
        SDL_Log("%s", "Unrecognized backend shader format!");
        return NULL;
    }

    Slang::ComPtr<slang::IBlob> SlangDiagnostics;

    slang::SessionDesc sessionDesc{};
    sessionDesc.targets = targetDescriptions.data();
    sessionDesc.targetCount = static_cast<SlangInt>(targetDescriptions.size());
    const char *searchPaths[] = {"/Assets/Shaders/"};
    sessionDesc.searchPaths = searchPaths;
    sessionDesc.searchPathCount = 1;

    Slang::ComPtr<slang::ISession> SlangSession;
    globalSession->createSession(sessionDesc, SlangSession.writeRef());

    std::string shaderPath = std::string(SDL_GetBasePath()) + "/Assets/Shaders/" + shaderFilename;
    slang::IModule *SlangShader = SlangSession->loadModule(shaderPath.c_str(), SlangDiagnostics.writeRef());

    if (!SlangShader)
    {
        fprintf(stderr, "%s\n", static_cast<const char *>(SlangDiagnostics->getBufferPointer()));
        SDL_SetError("Failed to open shader");
        return nullptr;
    }

    Slang::ComPtr<slang::IEntryPoint> entryPointPtr;
    SlangShader->findEntryPointByName(entryPoint, entryPointPtr.writeRef());

    if (!entryPointPtr)
    {
        SDL_SetError("Failed to find entry point: %s", entryPoint);
        return nullptr;
    }

    std::vector<Slang::ComPtr<slang::IComponentType>> componentTypes;
    componentTypes.emplace_back(SlangShader);

    componentTypes.emplace_back(entryPointPtr);

    std::vector<slang::IComponentType *> rawComponentTypes;
    for (auto &compType : componentTypes)
        rawComponentTypes.push_back(compType.get());

    Slang::ComPtr<slang::IComponentType> program;
    SlangSession->createCompositeComponentType(rawComponentTypes.data(), rawComponentTypes.size(), program.writeRef(), SlangDiagnostics.writeRef());

    Slang::ComPtr<slang::IComponentType> linkedProgram;
    program->link(linkedProgram.writeRef(), SlangDiagnostics.writeRef());

    if (linkedProgram == nullptr)
    {
        return nullptr;
    }

    int targetIndex = 0;
    Slang::ComPtr<slang::IBlob> codeBlob;
    linkedProgram->getEntryPointCode(0, targetIndex, codeBlob.writeRef(), SlangDiagnostics.writeRef());

    // diagnoseIfNeeded(diagnostics);
    SlangDiagnostics = nullptr;

    const Uint8 *code = static_cast<Uint8 *>(const_cast<void *>(codeBlob->getBufferPointer()));
    size_t codeSize = codeBlob->getBufferSize();
    if (code == NULL)
    {
        SDL_Log("Failed to load shader from disk! %s", shaderFilename);
        return NULL;
    }

    Uint32 samplerCount = 0;
    Uint32 storageBufferCount = 0;
    Uint32 storageTextureCount = 0;
    Uint32 uniformBufferCount = 0;
    slang::ProgramLayout *programLayout = linkedProgram->getLayout(targetIndex, SlangDiagnostics.writeRef());
    Uint32 parameterCount = programLayout->getParameterCount();
    for (Uint32 i = 0; i < parameterCount; i++)
    {
        slang::VariableLayoutReflection *parameter = programLayout->getParameterByIndex(i);

        auto tl = parameter->getType();
        slang::TypeReflection::Kind kind = tl->getKind();
        if (kind == slang::TypeReflection::Kind::SamplerState)
        {
            samplerCount++;
            continue;
        }

        if (kind == slang::TypeReflection::Kind::ShaderStorageBuffer)
        {
            storageBufferCount++;
            continue;
        }

        if (kind == slang::TypeReflection::Kind::Resource)
        {
            storageTextureCount++;
            continue;
        }

        if (kind == slang::TypeReflection::Kind::ConstantBuffer)
        {
            uniformBufferCount++;
            continue;
        }
    }

    SDL_GPUShaderCreateInfo shaderInfo = {
        .code = code,
        .code_size = codeSize,
        .entrypoint = entryPoint,
        .format = SDLFormat,
        .stage = shaderStage,
        .num_samplers = samplerCount,
        .num_uniform_buffers = uniformBufferCount,
        .num_storage_buffers = storageBufferCount,
        .num_storage_textures = storageTextureCount};
    SDL_GPUShader *shader = SDL_CreateGPUShader(device, &shaderInfo);

    if (shader == NULL)
    {
        SDL_Log("Failed to create shader!");
        return NULL;
    }
    // todo: free "code" variable
    return shader;
}