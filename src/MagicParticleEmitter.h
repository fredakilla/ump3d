#pragma once

#include "MagicParticleEffect.h"
#include "Magic.h"

namespace Urho3D
{

/// structure for description of one array of attribute.
struct MP_ARRAY_INFO : public MAGIC_ARRAY_INFO
{
    int stage;
    void* buffer;
    int offset;
    int stride;
};

/// Base buffer structure for vertex and index buffers.
struct MP_BUFFER
{
    int max_length;
    int length;

    MP_BUFFER();
    virtual ~MP_BUFFER();

    void SetLength(int new_length);
    virtual void Create(int new_length);
    virtual void Destroy();
    virtual void* Map(int stride) { return nullptr; }
};

/// Buffer data for vertex and index buffers.
struct MP_BUFFER_RAM : public MP_BUFFER
{
    char* buffer;

    MP_BUFFER_RAM();
    virtual ~MP_BUFFER_RAM();

    virtual void Create(int new_length);
    virtual void Destroy();
    virtual void* Map(int stride) { return buffer; }
};


///-------------------------------------------------------------------------------------------------
/// Manage and Render a Magic particle emitter.
///-------------------------------------------------------------------------------------------------
class URHO3D_API MagicParticleEmitter : public Drawable
{
    URHO3D_OBJECT(MagicParticleEmitter, Drawable)

public:
    /// Construct.
    MagicParticleEmitter(Context* context);
    /// Destruct.
    virtual ~MagicParticleEmitter();
    /// Register object factory.
    static void RegisterObject(Context* context);

    /// Set effect attribute.
    void SetEffectAttr(const ResourceRef &value);
    /// Return effect attribute.
    ResourceRef GetEffectAttr() const;

    /// Handle enabled/disabled state change.
    virtual void OnSetEnabled();
    /// Calculate distance and prepare batches for rendering. May be called from worker thread(s), possibly re-entrantly.
    virtual void UpdateBatches(const FrameInfo& frame);
    /// Visualize the component as debug geometry.
    virtual void DrawDebugGeometry(DebugRenderer* debug, bool depthTest);
    /// Return whether a geometry update is necessary, and if it can happen in a worker thread.
    virtual UpdateGeometryType GetUpdateGeometryType() { return UPDATE_NONE; }

    /// Set effect and emitter index.
    void SetEffect(MagicParticleEffect* effect, int index);
    /// Set effect.
    void SetEffect(MagicParticleEffect* effect);
    /// Set emitter index.
    void SetIndex(int index);
    /// Restart.
    void Restart();
    /// Stop.
    void Stop();
    /// Pause.
    void Pause();
    /// Resume.
    void Resume();

    /// Return effect.
    MagicParticleEffect* GetEffect() const;
    /// Return emitter index.
    int GetIndex() const { return _index; }
    /// Get Current emitter name.
    String GetEmitterName();
    /// Get the number of the drawing particles.
    int GetParticlesCount();

    /// If true, particles are moving with the emitter when it is being moved, else particles remain at their positions when emitter is moved.
    void SetParticlesMoveWithEmitter(bool moveWithEmitter);
    /// If true, when emitter is turned, all the previously created particles turn with the emitter, else they preserve their positions and motion directions.
    void SetParticlesRotateWithEmitter(bool rotateWithEmitter);
    /// Override emitter built-in rotation and use urho3D node rotation intead.
    void SetOverrideEmitterRotation(bool override);

    void SetEmitterPosition(Vector3 pos);
    MAGIC_POSITION _emitterPos;

private:
    /// Handle node being assigned.
    virtual void OnNodeSet(Node* node);
    /// Recalculate the world-space bounding box.
    virtual void OnWorldBoundingBoxUpdate();
    /// Handle particles update, create and render geometries.
    void HandleUpdate(StringHash eventType,VariantMap& eventData);
    /// Save attributes rendering.
    void SaveAttributes(MAGIC_RENDERING_START* start);
    /// Map vertex and index buffers.
    void MapBuffers(MAGIC_ARRAY_INFO* vertex_info, MAGIC_ARRAY_INFO* index_info);

    /// Reset all states.
    void ResetStates();
    /// Set render state.
    void SetRenderState(MAGIC_RENDER_STATE* state);
    /// Fake state.
    void SetRenderNothing(MAGIC_RENDER_STATE* s);
    /// Texture state.
    void SetRenderTexture(MAGIC_RENDER_STATE* s);
    /// Blending state.
    void SetRenderBlending(MAGIC_RENDER_STATE* s);
    /// Texture coord adresse mode.
    void SetRenderAddressU(MAGIC_RENDER_STATE* s);
    /// Texture coord adresse mode.
    void SetRenderAddressV(MAGIC_RENDER_STATE* s);
    /// Enable/Disable Depth write.
    void SetRenderZWrite(MAGIC_RENDER_STATE* s);
    /// Get material from index.
    Material* GetRenderMaterial(int matIndex);

    /// Vertex elements used to define vertex format.
    PODVector<VertexElement> _vertexElements;
    /// Index buffer.
    SharedPtr<IndexBuffer> _indexBuffer;
    /// Vertex buffer.
    SharedPtr<VertexBuffer> _vertexBuffer;
    /// Geometries.
    Vector<SharedPtr<Geometry> > _geometries;
    /// Magic particle effect.
    SharedPtr<MagicParticleEffect> _effect;
    /// Emitter index.
    int _index;
    /// Emitter instance.
    HM_EMITTER _magicEmitter;
    /// Batch indices to render.
    PODVector<MAGIC_RENDER_VERTICES> _drawBatches;    
    /// Array info for vertex and index buffers.
    MP_ARRAY_INFO _m_array_info[2];
    /// Buffer Data for vertices.
    MP_BUFFER* _mp_vertex_buffer;
    /// Buffer Data for indices.
    MP_BUFFER* _mp_index_buffer;
    /// Rendering infos.
    MAGIC_RENDERING_START _renderingStart;
    /// Is drawable visible by camera
    bool _isVisible;
    /// override emitter rotation flag
    bool _overrideEmitterRotation;
    /// move particles with emitter flag
    bool _moveParticlesWithEmitter;
    /// rotate particles with emitter flag
    bool _rotateParticlesWithEmitter;
    /// Camera pointer
    Node* _cameraNode;
    /// Graphics subsystem pointer.
    Graphics* _graphics;


    /// Define pointer to function type for render state pointer to functions.
    typedef void (MagicParticleEmitter::*StateFuncPtr)(MAGIC_RENDER_STATE* s);
    /// Array of pointers to functions to collect render states.
    static StateFuncPtr _stateFuncPointer[];

    /// Texture state structure.
    struct TEX_STAGE
    {
        /// Texture adress mode.
        TextureAddressMode address_u, address_v;
        /// Pointer to texture.
        Texture2D* uTexture;
    };

    /// Max textures per stage.
    static const unsigned int MAX_TEX_STAGE = 16;
    /// Array of texture stage.
    TEX_STAGE stages[MAX_TEX_STAGE];
    /// Blending state.
    BlendMode STATE_BLENDING;
    /// Render state hash key used to identify states.
    unsigned _stateHashKey;

    bool STATE_ZWRITE;
};

}
