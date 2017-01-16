#pragma once

#include <Urho3D/Urho3DAll.h>
#include "Magic.h"

namespace Urho3D
{

typedef unsigned MP_MAT_HASHKEY;

///-------------------------------------------------------------------------------------------------
/// Magic Particle Effect
/// Read a .ptc file, load emitters, load textures, generate urho shaders, create urho materials.
/// (use menu saved as (API)... in Magic Particles 3D to create compatible .ptc files)
/// Note: Shaders are generated at runtime, currently only glsl (opengl) are generated.
/// Note2: Currently loading only one .ptc file per game instance is supported.
///        Please place all your effects in this file (use merge menu to combine files if needed).
///-------------------------------------------------------------------------------------------------
class URHO3D_API MagicParticleEffect : public Resource
{
    URHO3D_OBJECT(MagicParticleEffect, Object)

public:
    /// Construct.
    MagicParticleEffect(Context* context);
    /// Destruct.
    virtual ~MagicParticleEffect();
    /// Register object factory.
    static void RegisterObject(Context* context);
    /// Load resource from stream. May be called from a worker thread. Return true if successful.
    virtual bool BeginLoad(Deserializer& source);
    /// Finish resource loading. Always called from the main thread. Return true if successful.
    virtual bool EndLoad();
    /// Return number of emitters.
    unsigned GetNumEmitters() const;
    /// Return emitter at index.
    HM_EMITTER GetEmitter(unsigned index) const;
    /// Return material at index. Pass a stateHashkey to register material with new states.
    Material* GetMaterial(int index, unsigned stateHashkey, bool& newMaterialCreated);
    /// Return texture at index.
    Texture2D* GetTexture(int index) { return _textures[index]; }

private:
    /// Load folder.
    void LoadFolder(HM_FILE file, const char* path);
    /// Load emitter.
    void LoadEmitter(HM_FILE file, const char* path);
    /// Create atlas texture.
    bool CreateAtlasTexture(HM_FILE file);
    /// Create texture.
    bool CreateTexture(const MAGIC_CHANGE_ATLAS& atlas);
    /// Load atlas data from memory or from file.
    bool LoadAtlasData(const MAGIC_CHANGE_ATLAS& atlas);
    /// Load all textures.
    void RefreshAtlas();
    /// Create all materials.
    void CreateAllMaterials();
    /// Return vertex shader filename of a compatible shader with MAGIC_MATERIAL definition. Create shader if not exists.
    String GetCompatibleVertexShader(MAGIC_MATERIAL* material);
    /// Return pixel shader filename of a compatible shader with MAGIC_MATERIAL definition. Create shader if not exists.
    String GetCompatiblePixelShader(MAGIC_MATERIAL* material);
    /// Create urho material from MAGIC material
    Material *CreateMaterial(MAGIC_MATERIAL* mat);

    /// File data size.
    unsigned _dataSize;
    /// File data.
    SharedArrayPtr<char> _data;
    /// Emitters.
    PODVector<HM_EMITTER> _emitters;
    /// Textures.
    Vector<SharedPtr<Texture2D> > _textures;
    /// Array of Magic Materials.
    Vector<MAGIC_MATERIAL> _magicMaterials;
    /// Map of Urho Materials. Key is computed from MAGIC_MATERIAL values and current assigned textures.
    HashMap<MP_MAT_HASHKEY, SharedPtr<Material>> _materials;
    /// Map of shaders filenames. Key is computed from MAGIC_MATERIAL.
    HashMap<MP_MAT_HASHKEY, String> _shaderFilenameList;
};

}
