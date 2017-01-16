#include "MagicParticleEffect.h"
#include <Urho3D/Urho3DAll.h>

#include <fstream>

namespace Urho3D
{

/// Generate vertex and pixel shaders files at runtime for Urho3D using MAGIC_MATERIAL description.
/// Shader files are written to disk in binary CoreData/Shaders folder.
static String VsGenerate(MAGIC_MATERIAL* m, String fileName);
static String PsGenerate(MAGIC_MATERIAL* m, String fileName);

/// Path where to write shaders relative to binary.
#ifdef URHO3D_OPENGL
static const char* URHO_SHADER_DIRECTORY = "CoreData/Shaders/GLSL/MagicParticles/";
#else
static const char* URHO_SHADER_DIRECTORY = "CoreData/Shaders/HLSL/MagicParticles/";
#endif

//----------------------------------------------------------------------------------------------------

/// Hash from int.
unsigned int hash(unsigned int x)
{
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = ((x >> 16) ^ x) * 0x45d9f3b;
    x = (x >> 16) ^ x;
    return x;
}


/// Generate hash key from MAGIC_MATERIAL.
MP_MAT_HASHKEY getMagicMaterialHashKey(MAGIC_MATERIAL* material)
{
    unsigned hashKey = 0;
    hashKey += hash(material->flags);
    hashKey += hash(material->textures);
    for (int i=0; i<material->textures; ++i)
    {
       hashKey += hash(material->states[i].address_u);
       hashKey += hash(material->states[i].address_v);
       hashKey += hash(material->states[i].argument_alpha1);
       hashKey += hash(material->states[i].argument_alpha2);
       hashKey += hash(material->states[i].argument_rgb1);
       hashKey += hash(material->states[i].argument_rgb2);
       hashKey += hash(material->states[i].operation_alpha);
       hashKey += hash(material->states[i].operation_rgb);
    }

    return hashKey;
}

//----------------------------------------------------------------------------------------------------

MagicParticleEffect::MagicParticleEffect(Context* context) :
    Resource(context),
    _dataSize(0)
{
}

MagicParticleEffect::~MagicParticleEffect()
{
    for (unsigned i = 0; i < _emitters.Size(); ++i)
        Magic_UnloadEmitter(_emitters[i]);

    /*HashMap<MP_MAT_HASHKEY, SharedPtr<Material>>::Iterator it;
    for(it=_materials.Begin(); it!=_materials.End(); it++)
    {
        Material* m = it->second_;
        m->ReleaseRef();
    }*/
}

void MagicParticleEffect::RegisterObject(Context* context)
{
    context->RegisterFactory<MagicParticleEffect>();

    bool filters[MAGIC_RENDER_STATE__MAX];
    for (int i=0;i<MAGIC_RENDER_STATE__MAX;i++)
        filters[i]=false;
    filters[MAGIC_RENDER_STATE_BLENDING]=true;
    //filters[MAGIC_RENDER_STATE_TEXTURE_COUNT]=true;
    filters[MAGIC_RENDER_STATE_TEXTURE]=true;
    filters[MAGIC_RENDER_STATE_ADDRESS_U]=true;
    filters[MAGIC_RENDER_STATE_ADDRESS_V]=true;
    //filters[MAGIC_RENDER_STATE_ZENABLE]=true;
    filters[MAGIC_RENDER_STATE_ZWRITE]=true;
    Magic_SetRenderStateFilter(filters, true);

    Magic_SetAxis(MAGIC_pXpYpZ);

    Magic_SetStartingScaleForAtlas(1.0f);
}

bool MagicParticleEffect::BeginLoad(Deserializer& source)
{
    _dataSize = source.GetSize();
    _data = new char[_dataSize];
    if (source.Read(_data, _dataSize) != _dataSize)
    {
        URHO3D_LOGERROR("Could not load data");
        _data = 0;
        return false;
    }

    return true;
}

bool MagicParticleEffect::EndLoad()
{
    if (!_data)
        return false;

    HM_FILE file = Magic_OpenFileInMemory(_data);
    if (file <= 0)
    {
        URHO3D_LOGERROR("Could not open file");
        _data = 0;
        return false;
    }

    LoadFolder(file, "");

    bool success = false;
    if (Magic_HasTextures(file))
        success = CreateAtlasTexture(file);
    else
        RefreshAtlas();

    success = _textures.Size() > 0;

    Magic_CloseFile(file);
    _data = 0;
    SetMemoryUse(GetMemoryUse() + _dataSize);

    CreateAllMaterials();

    return success;
}

void MagicParticleEffect::LoadFolder(HM_FILE file, const char* path)
{
    Magic_SetCurrentFolder(file, path);

    MAGIC_FIND_DATA find;
    const char* name = Magic_FindFirst(file, &find, MAGIC_FOLDER | MAGIC_EMITTER);
    while (name)
    {
        if (find.animate)
            LoadEmitter(file, name);
        else
            LoadFolder(file, name);

        name = Magic_FindNext(file, &find);
    }

    Magic_SetCurrentFolder(file, "..");
}

void MagicParticleEffect::LoadEmitter(HM_FILE file, const char* path)
{
    HM_EMITTER emitter = Magic_LoadEmitter(file, path);
    if (emitter)
    {
        Magic_SetInterpolationMode(emitter, true);
        Magic_Stop(emitter);

        Magic_SetEmitterPositionMode(emitter,true);
        Magic_SetEmitterDirectionMode(emitter,true);

        _emitters.Push(emitter);
    }
}

bool MagicParticleEffect::CreateAtlasTexture(HM_FILE file)
{
    Magic_CreateAtlasesForEmitters(1024, 1024, _emitters.Size(), &_emitters[0], 1, 0.1f);

    MAGIC_CHANGE_ATLAS atlas;
    while (Magic_GetNextAtlasChange(&atlas) ==  MAGIC_SUCCESS)
    {
        switch (atlas.type)
        {
        case MAGIC_CHANGE_ATLAS_CREATE:
            if (!CreateTexture(atlas))
                return false;
            break;

        case MAGIC_CHANGE_ATLAS_DELETE:
            URHO3D_LOGERROR("MAGIC_CHANGE_ATLAS_DELETE Not implement");
            return false;

        case MAGIC_CHANGE_ATLAS_LOAD:
            if (!LoadAtlasData(atlas))
                return false;
            break;

        case MAGIC_CHANGE_ATLAS_CLEAN:
            URHO3D_LOGERROR("MAGIC_CHANGE_ATLAS_CLEAN Not implement");
            return false;

        default:
            break;
        }
    }

    return true;
}

bool MagicParticleEffect::CreateTexture(const MAGIC_CHANGE_ATLAS& atlas)
{
    SharedPtr<Texture2D> texture(new Texture2D(context_));
    texture->SetNumLevels(1);

    texture->SetSize(atlas.width, atlas.height, Graphics::GetRGBAFormat());
    
    SharedArrayPtr<unsigned char> data(new unsigned char[atlas.width * atlas.height * 4]);
    memset(&data[0], 0, atlas.width * atlas.height * 4);
    texture->SetData(0, 0, 0, atlas.width, atlas.height, data);

    if (_textures.Size() < (unsigned)atlas.index + 1)
        _textures.Resize(atlas.index + 1);

    _textures[atlas.index] = texture;

    return true;
}

bool MagicParticleEffect::LoadAtlasData(const MAGIC_CHANGE_ATLAS& atlas)
{
    SharedPtr<Texture2D> texture = _textures[atlas.index];
    if (!texture)
        return false;

    Image image(context_);
    if (atlas.data)
    {
        // Load image from memory
        MemoryBuffer buffer(atlas.data, atlas.length);
        if (!image.Load(buffer))
        {
            URHO3D_LOGERROR("Could not load image from memory");
            return false;
        }
    }
    else
    {
        // Load image from file
        String filePath;
        String parentPath = GetParentPath(GetName());
        if (parentPath.Empty())
            filePath = atlas.file;
        else
            filePath = parentPath + "/" + atlas.file;

        File file(context_);
        if (!file.Open(filePath))
        {
            URHO3D_LOGERROR("Could not open file " + filePath);
            return false;
        }

        if (!image.Load(file))
        {
            URHO3D_LOGERROR("Could not load image from " + filePath);
            return false;
        }
    }

    if (image.GetComponents() != 4)
    {
        URHO3D_LOGERROR("Invalid image components");
        return false;
    }

    if (atlas.width != image.GetWidth() || atlas.height != image.GetHeight())
    {
        URHO3D_LOGERROR("Atlas scale not support");
        return false;
    }

    return texture->SetData(0, atlas.x, atlas.y, atlas.width, atlas.height, image.GetData());
}

void MagicParticleEffect::RefreshAtlas()
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    String parentPath = GetParentPath(GetName());

    Vector<SharedPtr<Texture2D> > newTextures;

    // Check for atlas changes.

    MAGIC_CHANGE_ATLAS atlas;
    while (Magic_GetNextAtlasChange(&atlas) == MAGIC_SUCCESS)
    {
        switch (atlas.type)
        {
        case MAGIC_CHANGE_ATLAS_CREATE:
            {
                String filePath;
                if (parentPath.Empty())
                    filePath = atlas.file;
                else
                    filePath = parentPath + "textures/" + atlas.file;

                SharedPtr<Texture2D> texture(cache->GetResource<Texture2D>(filePath));
                if (!texture)
                {
                    URHO3D_LOGERROR(String("Could not load texture from ") + filePath);
                }

                newTextures.Push(texture);
            }
            break;

        case MAGIC_CHANGE_ATLAS_DELETE:
            URHO3D_LOGERROR("MAGIC_CHANGE_ATLAS_DELETE Not implemented");
            break;

        case MAGIC_CHANGE_ATLAS_LOAD:
            URHO3D_LOGERROR("MAGIC_CHANGE_ATLAS_LOAD Not implemented");
            break;

        case MAGIC_CHANGE_ATLAS_CLEAN:
            URHO3D_LOGERROR("MAGIC_CHANGE_ATLAS_CLEAN Not implemented");
            break;

        default:
            break;
        }
    }


    // Populate texture list.

    if(newTextures.Size() > 0)
    {
        _textures.Resize(newTextures.Size());
        for(unsigned i=0; i<newTextures.Size(); ++i)
            _textures[i] = newTextures[i];
    }
}


unsigned MagicParticleEffect::GetNumEmitters() const
{
    return _emitters.Size();
}

HM_EMITTER MagicParticleEffect::GetEmitter(unsigned index) const
{
    if (index >= _emitters.Size())
        return 0;

    return _emitters[index];
}

Material* MagicParticleEffect::GetMaterial(int index, unsigned stateHashkey, bool& newMaterialCreated)
{
    // Magic Particle can use a material multiple times to render an emitter assigning to it different textures and states (blending...)
    // As rendering is batched in MagicParticleEmitter class, to avoid to overwrite textures of a material in a batch that is not yet rendered
    // we need to create materials clones with rights textures for each batch.

    // get magic material at index.
    MAGIC_MATERIAL mat = _magicMaterials[index];

    // generate hash key from magic material.
    unsigned matkey = getMagicMaterialHashKey(&mat);

    // get urho material from map using this hash key.
    // note: at this point material should always exists in map (created when loading ptc file)
    Material* m = _materials[matkey];

    if(!m)
        URHO3D_LOGERROR("Material not found!");

    newMaterialCreated = false;

    // add stateHashkey value to hash key.
    // note: stateHashkey is computed when collecting render states in MagicParticleEmitter
    matkey += stateHashkey;

    // Search for an existing urho material with this new hash key.
    // This new hash takes additionals elements into account, like textures, blending...
    // note: at this point material may not be present in map if it is the first time we need it.   
    if(_materials.Find(matkey) == _materials.End())
    {
        // not found! create a new compatible material.
        // this new material will be used with anothers textures and states from its original.
        Material* newMat = CreateMaterial(&mat);

        // and add it to map for next time we need it.
        _materials[matkey] = newMat;

        newMaterialCreated = true;
    }

    // return material with right textures assigned.
    return _materials[matkey];
}

String MagicParticleEffect::GetCompatibleVertexShader(MAGIC_MATERIAL* material)
{
    // for vertex shaders, hash key is only based on the number of textures that is the only varying value
    unsigned hashKey = hash(material->textures);

    // check in list if a compatible vertex shader already exists
    if(_shaderFilenameList.Find(hashKey) == _shaderFilenameList.End())
    {
        // create new vertex shader

        static unsigned nextVertexShaderId = 0;
        String vsFileName = "VERTEX_SHADER" + String(nextVertexShaderId++);
        _shaderFilenameList[hashKey] = vsFileName;

        String vsCode = VsGenerate(material, vsFileName);

        String path = URHO_SHADER_DIRECTORY + vsFileName + ".glsl";
        std::ofstream file(path.CString());
        file << vsCode.CString();
    }

    return _shaderFilenameList[hashKey];
}

String MagicParticleEffect::GetCompatiblePixelShader(MAGIC_MATERIAL* material)
{
    // for pixel shaders, compute hash key based on MAGIC_MATERIAL states description
    unsigned hashKey = getMagicMaterialHashKey(material);

    // check in list if a compatible pixel shader already exists
    if(_shaderFilenameList.Find(hashKey) == _shaderFilenameList.End())
    {
        // create new pixel shader

        static unsigned nextPixelShaderId = 0;
        String psFileName = "PIXEL_SHADER" + String(nextPixelShaderId++);
        _shaderFilenameList[hashKey] = psFileName;

        String psCode = PsGenerate(material, psFileName);

        String path = URHO_SHADER_DIRECTORY + psFileName + ".glsl";
        std::ofstream file(path.CString());
        file << psCode.CString();
    }

    return _shaderFilenameList[hashKey];
}

void MagicParticleEffect::CreateAllMaterials()
{
    int materialCount = Magic_GetMaterialCount();

    MAGIC_MATERIAL mat;
    for (int i=0; i<materialCount; i++)
    {
        Magic_GetMaterial(i, &mat);
        _magicMaterials.Push(mat);

        Material* material = CreateMaterial(&mat);

        // compute key from MAGIC_MATERIAL and add material to map
        unsigned key = getMagicMaterialHashKey(&mat);
        _materials[key] = material;
    }
}

Material* MagicParticleEffect::CreateMaterial(MAGIC_MATERIAL* mat)
{
    String vsFileName = GetCompatibleVertexShader(mat);
    String psFileName = GetCompatiblePixelShader(mat);

    Material* material = new Material(context_);

    Technique* technique = new Technique(context_);
    Pass* pass = technique->CreatePass("alpha");
    pass->SetBlendMode(BLEND_REPLACE);
    pass->SetDepthWrite(true);
    pass->SetVertexShader("MagicParticles/" + String(vsFileName));
    pass->SetPixelShader("MagicParticles/" + String(psFileName));

    material->SetTechnique(0, technique);
    material->SetCullMode(CULL_NONE);

    return material;
}

//----------------------------------------------------------------------------------------------------

#ifdef URHO3D_OPENGL
String VsGenerate(MAGIC_MATERIAL* m, String fileName)
{
    int i;
    MAGIC_VERTEX_FORMAT* format=&(m->format);
    bool is_color=(format->attributes & MAGIC_ATTRIBUTE_COLOR);
    int UVs=format->UVs; 

    String shader_code;

    shader_code+="#include \"Uniforms.glsl\"\n";
    shader_code+="#include \"Transform2.glsl\"\n";

    if (is_color)
        shader_code+="attribute vec4 iColor;\n";

    for (i=0;i<UVs;i++)
    {
        shader_code+="attribute vec2 iTexCoord";
        shader_code+=String(i);
        shader_code+=";\n";
    }

    if (is_color)
        shader_code+="varying vec4 colorVarying;\n";
    for (i=0;i<UVs;i++)
    {
        shader_code+="varying vec2 textureCoordinate";
        shader_code+=String(i);
        shader_code+=";\n";
    }
    shader_code+="\nvoid main()\n";
    shader_code+="{\n";    

    shader_code+="mat4 modelMatrix = iModelMatrix;\n";
    shader_code+="vec3 worldPos = GetWorldPos(modelMatrix);\n";
    shader_code+="gl_Position = GetClipPos(worldPos);\n";
    shader_code+="\n";

    if (is_color)
        shader_code+="colorVarying = iColor;\n\n";

    for (i=0;i<UVs;i++)
    {
        shader_code+="textureCoordinate";
        shader_code+=String(i);
        shader_code+=" = iTexCoord";
        shader_code+=String(i);
        shader_code+=";\n";
    }

    shader_code+="}\n";

    return shader_code;
}

String PsGenerate(MAGIC_MATERIAL* m, String fileName)
{
    String shader_code;
    int i;

    int texs = m->textures;

    bool is_color=(m->format.attributes & MAGIC_ATTRIBUTE_COLOR);   

    shader_code+="#ifdef GL_ES\r\n";
    shader_code+="// define default precision for float, vec, mat.\r\n";
    shader_code+="precision lowp float;\r\n";
    shader_code+="#endif\r\n";
    if (is_color)
        shader_code+="varying vec4 colorVarying;\r\n";
    shader_code+="#ifdef GL_ES\r\n";
    for (i=0;i<texs;i++)
    {
        shader_code+="varying lowp vec2 textureCoordinate";
        shader_code+=String(i);
        shader_code+=";\r\n";
    }
    shader_code+="#else\r\n";
    for (i=0;i<texs;i++)
    {
        shader_code+="varying vec2 textureCoordinate";
        shader_code+=String(i);
        shader_code+=";\r\n";
    }
    shader_code+="#endif\r\n";
    for (i=0;i<texs;i++)
    {
        shader_code+="uniform sampler2D stexture";
        shader_code+=String(i);
        shader_code+=";\r\n";
    }

    shader_code+="void main()\r\n";
    shader_code+="{\r\n";

    if (texs)
    {
        shader_code+="vec4 color;\r\n";
        shader_code+="vec4 arg1;\r\n";
        shader_code+="vec4 arg2;\r\n";
        shader_code+="vec4 colorTex;\r\n";
        for (i=0;i<texs;i++)
        {
            shader_code+="\r\n";
            MAGIC_TEXTURE_STATES* s=&(m->states[i]);

            if (s->argument_rgb1==MAGIC_TEXARG_TEXTURE || s->argument_alpha1==MAGIC_TEXARG_TEXTURE || (s->operation_rgb!=MAGIC_TEXOP_ARGUMENT1 && s->argument_rgb2==MAGIC_TEXARG_TEXTURE) || (s->operation_alpha!=MAGIC_TEXOP_ARGUMENT1 && s->argument_alpha2==MAGIC_TEXARG_TEXTURE))
            {
                shader_code+="colorTex = texture2D(stexture";
                shader_code+=String(i);
                shader_code+=", textureCoordinate";
                shader_code+=String(i);
                shader_code+=");\r\n";
            }

            if (s->argument_rgb1==s->argument_alpha1)
            {
                switch (s->argument_rgb1)
                {
                case MAGIC_TEXARG_CURRENT:
                    shader_code+="arg1 = color;\r\n";
                    break;
                case MAGIC_TEXARG_DIFFUSE:
                    shader_code+="arg1 = colorVarying;\r\n";
                    break;
                default:
                    shader_code+="arg1 = colorTex;\r\n";
                    break;
                }
            }
            else
            {
                switch (s->argument_rgb1)
                {
                case MAGIC_TEXARG_CURRENT:
                    shader_code+="arg1.xyz = color.xyz;\r\n";
                    break;
                case MAGIC_TEXARG_DIFFUSE:
                    shader_code+="arg1.xyz = colorVarying.xyz;\r\n";
                    break;
                default:
                    shader_code+="arg1.xyz = colorTex.xyz;\r\n";
                    break;
                }
                switch (s->argument_alpha1)
                {
                case MAGIC_TEXARG_CURRENT:
                    shader_code+="arg1.w = color.w;\r\n";
                    break;
                case MAGIC_TEXARG_DIFFUSE:
                    shader_code+="arg1.w = colorVarying.w;\r\n";
                    break;
                default:
                    shader_code+="arg1.w = colorTex.w;\r\n";
                    break;
                }
            }

            if (s->argument_rgb2==s->argument_alpha2 && s->operation_rgb!=MAGIC_TEXOP_ARGUMENT1 && s->operation_alpha!=MAGIC_TEXOP_ARGUMENT1)
            {
                switch (s->argument_rgb2)
                {
                case MAGIC_TEXARG_CURRENT:
                    shader_code+="arg2 = color;\r\n";
                    break;
                case MAGIC_TEXARG_DIFFUSE:
                    shader_code+="arg2 = colorVarying;\r\n";
                    break;
                default:
                    shader_code+="arg2 = colorTex;\r\n";
                    break;
                }
            }
            else
            {
                if (s->operation_rgb!=MAGIC_TEXOP_ARGUMENT1)
                {
                    switch (s->argument_rgb2)
                    {
                    case MAGIC_TEXARG_CURRENT:
                        shader_code+="arg2.xyz = color.xyz;\r\n";
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        shader_code+="arg2.xyz = colorVarying.xyz;\r\n";
                        break;
                    default:
                        shader_code+="arg2.xyz = colorTex.xyz;\r\n";
                        break;
                    }
                }
                if (s->operation_alpha!=MAGIC_TEXOP_ARGUMENT1)
                {
                    switch (s->argument_alpha2)
                    {
                    case MAGIC_TEXARG_CURRENT:
                        shader_code+="arg2.w = color.w;\r\n";
                        break;
                    case MAGIC_TEXARG_DIFFUSE:
                        shader_code+="arg2.w = colorVarying.w;\r\n";
                        break;
                    default:
                        shader_code+="arg2.w = colorTex.w;\r\n";
                        break;
                    }
                }
            }

            if (s->operation_rgb==s->operation_alpha)
            {
                switch (s->operation_rgb)
                {
                case MAGIC_TEXOP_ARGUMENT1:
                    shader_code+="color = arg1;\r\n";
                    break;
                case MAGIC_TEXOP_ADD:
                    shader_code+="color = arg1 + arg2;\r\n";
                    break;
                case MAGIC_TEXOP_SUBTRACT:
                    shader_code+="color = arg1 - arg2;\r\n";
                    break;
                case MAGIC_TEXOP_MODULATE:
                    shader_code+="color = arg1 * arg2;\r\n";
                    break;
                case MAGIC_TEXOP_MODULATE2X:
                    shader_code+="color = arg1 * arg2;\r\n";
                    shader_code+="color = color + color;\r\n";
                    break;
                default:
                    shader_code+="color = arg1 * arg2;\r\n";
                    shader_code+="color = color * 4.0;\r\n";
                    break;
                }
            }
            else
            {
                switch (s->operation_rgb)
                {
                case MAGIC_TEXOP_ARGUMENT1:
                    shader_code+="color.xyz = arg1.xyz;\r\n";
                    break;
                case MAGIC_TEXOP_ADD:
                    shader_code+="color.xyz = arg1.xyz + arg2.xyz;\r\n";
                    break;
                case MAGIC_TEXOP_SUBTRACT:
                    shader_code+="color.xyz = arg1.xyz - arg2.xyz;\r\n";
                    break;
                case MAGIC_TEXOP_MODULATE:
                    shader_code+="color.xyz = arg1.xyz * arg2.xyz;\r\n";
                    break;
                case MAGIC_TEXOP_MODULATE2X:
                    shader_code+="color.xyz = arg1.xyz * arg2.xyz;\r\n";
                    shader_code+="color.xyz = color.xyz + color.xyz;\r\n";
                    break;
                default:
                    shader_code+="color.xyz = arg1.xyz * arg2.xyz;\r\n";
                    shader_code+="color.xyz = color.xyz * 4.0;\r\n";
                    break;
                }

                switch (s->operation_alpha)
                {
                case MAGIC_TEXOP_ARGUMENT1:
                    shader_code+="color.w = arg1.w;\r\n";
                    break;
                case MAGIC_TEXOP_ADD:
                    shader_code+="color.w = arg1.w + arg2.w;\r\n";
                    break;
                case MAGIC_TEXOP_SUBTRACT:
                    shader_code+="color.w = arg1.w - arg2.w;\r\n";
                    break;
                case MAGIC_TEXOP_MODULATE:
                    shader_code+="color.w = arg1.w * arg2.w;\r\n";
                    break;
                case MAGIC_TEXOP_MODULATE2X:
                    shader_code+="color.w = arg1.w * arg2.w;\r\n";
                    shader_code+="color.w = color.w + color.w;\r\n";
                    break;
                default:
                    shader_code+="color.w = arg1.w * arg2.w;\r\n";
                    shader_code+="color.w = color.w * 4.0;\r\n";
                    break;
                }
            }
        }
        shader_code+="gl_FragColor = color;\r\n";
    }
    else
        shader_code+="gl_FragColor = colorVarying;\r\n";

    //shader_code+="gl_FragColor = vec4(frac(1.0+frac(textureCoordinate1)), 0.0 ,1.0);\r\n";
    //shader_code+="gl_FragColor = vec4(frac(1.0+frac(textureCoordinate0)), 0.0 ,1.0);\r\n";
    //shader_code+="gl_FragColor = texture2D(stexture1, textureCoordinate0);\r\n";
    //shader_code+="gl_FragColor = texture2D(stexture0, textureCoordinate1);\r\n";
    //shader_code+="gl_FragColor = texture2D(stexture0, textureCoordinate0) * colorVarying;\r\n";
    //shader_code+="gl_FragColor = vec4(1.0, 1.0, 1.0 ,1.0);\r\n";

    #ifdef SHADER_ALPHATEST_WRAP
    if (m->flags & MAGIC_MATERIAL_ALPHATEST)
    {
        shader_code+="#ifndef GL_ES\r\n";
        //shader_code+="clip(gl_FragColor.w-1.0/255.0);\r\n";
        //shader_code+="#else\r\n";
        shader_code+="if (gl_FragColor.w==0.0)\r\n";
        shader_code+="discard;\r\n";
        shader_code+="#endif\r\n";
    }
    #endif

    shader_code+="}\r\n";

    return shader_code;
}
#else
    // Write HLSL shader generation here
#endif

//----------------------------------------------------------------------------------------------------


}
