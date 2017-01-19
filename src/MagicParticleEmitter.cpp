#include "MagicParticleEffect.h"
#include "MagicParticleEmitter.h"
#include <Urho3D/Urho3DAll.h>

namespace Urho3D
{

#ifdef ASSERT_DEBUG
    #define MP_ASSERT(X) assert(X)
#else
    #define MP_ASSERT(X)
#endif

// scale conversion factors between magic particle 3D and Urho3D (1:100)
#define SCALE_URHO3D_TO_MAGIC 100.0f
#define SCALE_MAGIC_TO_URHO3D 0.01f

// use 16 or 32 bits indices
#ifdef INDEX_BUFFER_32_WRAP
    #define MP_LARGE_INDICES true
#else
    #define MP_LARGE_INDICES false
#endif


//----------------------------------------------------------------------------------------------------

MP_BUFFER::MP_BUFFER()
{
    max_length=0;
    length=0;
}

MP_BUFFER::~MP_BUFFER()
{
    Destroy();
}

void MP_BUFFER::Create(int new_length)
{
    Destroy();
    max_length=new_length;
}

void MP_BUFFER::Destroy()
{
    max_length=0;
    length=0;
}

void MP_BUFFER::SetLength(int new_length)
{
    if (max_length<new_length)
        Create(new_length);
    length=new_length;
}

//----------------------------------------------------------------------------------------------------


MP_BUFFER_RAM::MP_BUFFER_RAM() : MP_BUFFER()
{
    buffer=nullptr;
}

MP_BUFFER_RAM::~MP_BUFFER_RAM()
{
    Destroy();
}

void MP_BUFFER_RAM::Create(int new_length)
{
    MP_BUFFER::Create(new_length);
    buffer=new char[new_length];
}

void MP_BUFFER_RAM::Destroy()
{
    MP_BUFFER::Destroy();

    if (buffer)
    {
        delete []buffer;
        buffer=nullptr;
    }
}

//----------------------------------------------------------------------------------------------------

static inline Vector3 MagicToUrho3D(const MAGIC_POSITION& pos)
{
    return Vector3(pos.x, pos.y, pos.z) * SCALE_MAGIC_TO_URHO3D;
}

/*static inline Quaternion MagicToUrho3D(const MAGIC_DIRECTION& dir)
{
    return Quaternion(-dir.w, -dir.x, -dir.y, dir.z);
}*/

static inline MAGIC_POSITION Urho3DToMagic(const Vector3& pos)
{
    MAGIC_POSITION position = { pos.x_ * SCALE_URHO3D_TO_MAGIC, pos.y_ * SCALE_URHO3D_TO_MAGIC, pos.z_ * SCALE_URHO3D_TO_MAGIC};
    return position;
}

static inline MAGIC_DIRECTION Urho3DToMagic(const Quaternion& rot)
{
    MAGIC_DIRECTION direction = { -rot.x_, -rot.y_, -rot.z_, rot.w_ };
    return direction;
}

//----------------------------------------------------------------------------------------------------

extern const char* GEOMETRY_CATEGORY;

MagicParticleEmitter::MagicParticleEmitter(Context* context) :
    Drawable(context, DRAWABLE_GEOMETRY)
    , _index(-1)
    , _magicEmitter(0)
    , _indexBuffer(new IndexBuffer(context_))
    , _vertexBuffer(new VertexBuffer(context_))
    , _mp_vertex_buffer(new MP_BUFFER_RAM())
    , _mp_index_buffer(new MP_BUFFER_RAM())
    , _isVisible(false)
    , _moveParticlesWithEmitter(false)
    , _rotateParticlesWithEmitter(false)
    , _overrideEmitterRotation(true)
    , _cameraNode(nullptr)
{
    _indexBuffer->SetShadowed(true);
    _graphics = GetSubsystem<Graphics>();
    _emitterPos = Urho3DToMagic(Vector3(0,0,0));
    memset(&_renderingStart, 0, sizeof(MAGIC_RENDERING_START));
}

MagicParticleEmitter::~MagicParticleEmitter()
{
    if(_magicEmitter > 0)
        Magic_UnloadEmitter(_magicEmitter);

    _mp_vertex_buffer->Destroy();
    _mp_index_buffer->Destroy();

    delete _mp_vertex_buffer;
    delete _mp_index_buffer;
}

void MagicParticleEmitter::RegisterObject(Context* context)
{
    context->RegisterFactory<MagicParticleEmitter>(GEOMETRY_CATEGORY);

    URHO3D_MIXED_ACCESSOR_ATTRIBUTE("Magic Particle Effect", GetEffectAttr, SetEffectAttr, ResourceRef, ResourceRef(MagicParticleEffect::GetTypeStatic()), AM_DEFAULT);
    URHO3D_ACCESSOR_ATTRIBUTE("Emitter Index", GetIndex, SetIndex, int, -1, AM_DEFAULT);
    URHO3D_COPY_BASE_ATTRIBUTES(Drawable);
}

void MagicParticleEmitter::OnSetEnabled()
{
    Drawable::OnSetEnabled();

    Scene* scene = GetScene();
    if (scene)
    {
        if (IsEnabledEffective())
        {
            Restart();
            SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MagicParticleEmitter, HandleUpdate));
        }
        else
        {
            Stop();
            UnsubscribeFromEvent(E_UPDATE);
        }
    }
}

void MagicParticleEmitter::SetEffectAttr(const ResourceRef &value)
{
    ResourceCache* cache = GetSubsystem<ResourceCache>();
    SetEffect(cache->GetResource<MagicParticleEffect>(value.name_));
}

ResourceRef MagicParticleEmitter::GetEffectAttr() const
{
    return GetResourceRef(_effect, MagicParticleEffect::GetTypeStatic());
}

void MagicParticleEmitter::OnNodeSet(Node* node)
{
    Drawable::OnNodeSet(node);

    if (node)
    {
        Scene* scene = GetScene();
        if (scene && IsEnabledEffective())
        {
            worldBoundingBox_ = boundingBox_.Transformed(node_->GetWorldTransform());
            SubscribeToEvent(E_UPDATE, URHO3D_HANDLER(MagicParticleEmitter, HandleUpdate));            
        }
    }
    else
    {
        UnsubscribeFromEvent(E_UPDATE);
    }
}

void MagicParticleEmitter::OnWorldBoundingBoxUpdate()
{

}

void MagicParticleEmitter::UpdateBatches(const FrameInfo& frame)
{    
    distance_ = frame.camera_->GetDistance(GetWorldBoundingBox().Center());
    _isVisible = frame.camera_->GetFrustum().IsInsideFast(GetWorldBoundingBox()) != OUTSIDE;
    _cameraNode = frame.camera_->GetNode();
}

void MagicParticleEmitter::MapBuffers(MAGIC_ARRAY_INFO* vertex_info, MAGIC_ARRAY_INFO* index_info)
{
    MP_ARRAY_INFO* info;
    int new_length;
    int stage=0;

    // indices
    *((MAGIC_ARRAY_INFO*)&(_m_array_info[0]))=*vertex_info;
    info=&(_m_array_info[0]);
    info->stage=stage;
    stage++;
    new_length=vertex_info->length*vertex_info->bytes_per_one;
    _mp_vertex_buffer->SetLength(new_length);
    info->offset=0;
    info->stride=vertex_info->bytes_per_one;
    info->buffer=_mp_vertex_buffer->Map(info->stride);

    // vertices
    *((MAGIC_ARRAY_INFO*)&(_m_array_info[1]))=*index_info;
    info=&(_m_array_info[1]);
    info->stage=stage;
    stage++;
    new_length=index_info->length*index_info->bytes_per_one;
    _mp_index_buffer->SetLength(new_length);
    info->offset=0;
    info->stride=index_info->bytes_per_one;
    info->buffer=_mp_index_buffer->Map(info->stride);
}

void MagicParticleEmitter::HandleUpdate(StringHash eventType,VariantMap& eventData)
{
   if(!_isVisible)
        return;

    using namespace Update;
    float timeStep = eventData[P_TIMESTEP].GetFloat();

    // set camera
    if(_cameraNode)
    {
        MAGIC_CAMERA magicCamera;
        magicCamera.pos = Urho3DToMagic(_cameraNode->GetWorldPosition());
        magicCamera.dir = Urho3DToMagic(_cameraNode->GetWorldDirection());
        magicCamera.mode = MAGIC_CAMERA_FREE;
        Magic_SetCamera(&magicCamera);
    }

    // Set emitter position, direction and scale

    MAGIC_POSITION emPos;
    emPos.x = _emitterPos.x + node_->GetWorldPosition().x_ * SCALE_URHO3D_TO_MAGIC;
    emPos.y = _emitterPos.y + node_->GetWorldPosition().y_ * SCALE_URHO3D_TO_MAGIC;
    emPos.z = _emitterPos.z + node_->GetWorldPosition().z_ * SCALE_URHO3D_TO_MAGIC;
    Magic_SetEmitterPosition(_magicEmitter, &emPos);

    // ovveride emitter rotation with urho node rotation
    if(_overrideEmitterRotation)
    {
        MAGIC_DIRECTION emDir;
        emDir.x = Urho3DToMagic( node_->GetWorldRotation() ).x;
        emDir.y = Urho3DToMagic( node_->GetWorldRotation() ).y;
        emDir.z = Urho3DToMagic( node_->GetWorldRotation() ).z;
        emDir.w = Urho3DToMagic( node_->GetWorldRotation() ).w;
        Magic_SetEmitterDirection(_magicEmitter, &emDir);
    }

    Magic_SetScale(_magicEmitter, 1.0f * node_->GetScale().x_);


    // update emitter

    if(Magic_Update(_magicEmitter, 1000.0 * timeStep) == false)
        return;

    // set bounding box

    MAGIC_BBOX bbox;
    Magic_GetBBox(_magicEmitter, &bbox);
    boundingBox_.Define( MagicToUrho3D(bbox.corner1), MagicToUrho3D(bbox.corner2) );
    worldBoundingBox_ = boundingBox_;

    // clear draw batches
    _drawBatches.Clear();

    Vector<SharedPtr<Material> > materials;

    if (_magicEmitter > 0)// && Magic_InInterval(_magicEmitter))
    {
        MAGIC_RENDERING_START start;
        MAGIC_ARGB_ENUM color_mode = MAGIC_ARGB;
        int max_array_streams = 0;

        // prepares the information about render arrays and fill start structure
        void* context = Magic_PrepareRenderArrays(_magicEmitter, &start, max_array_streams, color_mode, MP_LARGE_INDICES);

        if (start.arrays)
        {
            // save start infos
            SaveAttributes(&start);

            // set vertex format
            _vertexElements.Clear();
            _vertexElements.Push(VertexElement(TYPE_VECTOR3, SEM_POSITION));
            _vertexElements.Push(VertexElement(TYPE_UBYTE4_NORM, SEM_COLOR));
            for(int i=0; i<start.format.UVs; ++i)
                _vertexElements.Push(VertexElement(TYPE_VECTOR2, SEM_TEXCOORD, i));

            // returns info about render arrays
            MAGIC_ARRAY_INFO vertex_info, index_info;
            Magic_GetRenderArrayData(context, 0, &vertex_info);
            Magic_GetRenderArrayData(context, 1, &index_info);

            // map index and vertex buffers
            MapBuffers(&vertex_info, &index_info);

            // set render array
            MP_ARRAY_INFO* array_info_vertex = &_m_array_info[0];
            Magic_SetRenderArrayData(context, array_info_vertex->stage, array_info_vertex->buffer, array_info_vertex->offset, array_info_vertex->stride);
            MP_ARRAY_INFO* array_info_index = &_m_array_info[1];
            Magic_SetRenderArrayData(context, array_info_index->stage, array_info_index->buffer, array_info_index->offset, array_info_index->stride);

            // Fills the render buffers by info about vertices
            Magic_FillRenderArrays(context);

            // reset render states
            ResetStates();

            // fill draw batches

            MAGIC_RENDER_VERTICES vrts;
            MAGIC_RENDER_STATE state;
            while (Magic_GetVertices(context, &vrts) == MAGIC_SUCCESS)
            {                
                while (Magic_GetNextRenderState(context, &state) == MAGIC_SUCCESS)
                {
                    SetRenderState(&state);
                }

                Material* material = GetRenderMaterial(vrts.material);
                materials.Push(SharedPtr<Material>(material));

                _drawBatches.Push(vrts);
            }
        }
    }


    unsigned batchCount = _drawBatches.Size();

    batches_.Resize(batchCount);
    _geometries.Resize(batchCount);

    if(batchCount == 0)
        return;

    unsigned totalIndexCount = _renderingStart.indexes;
    unsigned totalVertexCount = _renderingStart.vertices;

    if(_graphics->IsDeviceLost())
        return;

    // set index buffer

    _indexBuffer->SetSize(totalIndexCount, MP_LARGE_INDICES);
    MP_BUFFER_RAM* ib = reinterpret_cast<MP_BUFFER_RAM*>(_mp_index_buffer);
    _indexBuffer->SetData(ib->buffer);

    // set vertex buffer

    _vertexBuffer->SetSize(totalVertexCount,  _vertexElements , true);
    MP_BUFFER_RAM* vb = reinterpret_cast<MP_BUFFER_RAM*>(_mp_vertex_buffer);
    _vertexBuffer->SetData(vb->buffer);

    // set batches and geometries

    for (unsigned i = 0; i < batches_.Size(); ++i)
    {
        if (!_geometries[i])
        {
            _geometries[i] = new Geometry(context_);
            _geometries[i]->SetIndexBuffer(_indexBuffer);
            _geometries[i]->SetVertexBuffer(0, _vertexBuffer);
        }
		
        _geometries[i]->SetDrawRange(TRIANGLE_LIST, _drawBatches[i].starting_index, _drawBatches[i].indexes_count, true);

        batches_[i].geometry_ = _geometries[i];
        batches_[i].material_ = materials[i];
        batches_[i].distance_ = distance_;
        batches_[i].worldTransform_ = &Matrix3x4::IDENTITY;
    }
}

void MagicParticleEmitter::DrawDebugGeometry(DebugRenderer* debug, bool depthTest)
{
    if (debug && IsEnabledEffective())
    {
        debug->AddBoundingBox(worldBoundingBox_, Color::RED, depthTest);
    }
}

void MagicParticleEmitter::SaveAttributes(MAGIC_RENDERING_START* start)
{
    _renderingStart = *start;
}

void MagicParticleEmitter::SetEffect(MagicParticleEffect* effect, int index)
{
    if (effect == _effect && index == _index)
        return;

    if (_magicEmitter > 0)
    {
        Stop();

        Magic_UnloadEmitter(_magicEmitter);
        _magicEmitter = 0;
    }

    _effect = effect;
    _index = index;

    if (_effect && _index >= 0)
    {
        HM_EMITTER emitter = _effect->GetEmitter(_index);
        if (emitter > 0)
        {
            // Create new emitter
            _magicEmitter = Magic_DuplicateEmitter(emitter);

            // Calculating bounding box every frame
            Magic_SetBBoxPeriod(_magicEmitter, 1);

            // Set position and diretion modes
            Magic_SetEmitterPositionMode(_magicEmitter, _moveParticlesWithEmitter);
            Magic_SetEmitterDirectionMode(_magicEmitter, _rotateParticlesWithEmitter);

            Restart();
        }
    }
}

void MagicParticleEmitter::SetEmitterPosition(Vector3 pos)
{
    if(_magicEmitter)
    {
        _emitterPos = Urho3DToMagic(pos);
    }
}

void MagicParticleEmitter::SetOverrideEmitterRotation(bool override)
{
    _overrideEmitterRotation = override;
}

void MagicParticleEmitter::SetParticlesMoveWithEmitter(bool moveWithEmitter)
{
    _moveParticlesWithEmitter = moveWithEmitter;
    Magic_SetEmitterPositionMode(_magicEmitter, moveWithEmitter);
}

void MagicParticleEmitter::SetParticlesRotateWithEmitter(bool rotateWithEmitter)
{
    _rotateParticlesWithEmitter = rotateWithEmitter;
    Magic_SetEmitterDirectionMode(_magicEmitter, rotateWithEmitter);
}

void MagicParticleEmitter::SetEffect(MagicParticleEffect* effect)
{
    SetEffect(effect, _index);
}

void MagicParticleEmitter::SetIndex(int index)
{
    SetEffect(_effect, index);
}

void MagicParticleEmitter::Restart()
{
    if (_magicEmitter > 0)
    {
        Magic_Restart(_magicEmitter);

        // start emitter to interval 1
        if (Magic_IsInterval1(_magicEmitter))
        {
            MAGIC_POSITION emPos;
            emPos.x = _emitterPos.x + node_->GetWorldPosition().x_ * SCALE_URHO3D_TO_MAGIC;
            emPos.y = _emitterPos.y + node_->GetWorldPosition().y_ * SCALE_URHO3D_TO_MAGIC;
            emPos.z = _emitterPos.z + node_->GetWorldPosition().z_ * SCALE_URHO3D_TO_MAGIC;
            Magic_SetEmitterPosition(_magicEmitter, &emPos);

            Magic_EmitterToInterval1(_magicEmitter, 1.f, 0);
        }
    }
}

void MagicParticleEmitter::Stop()
{
    if (_magicEmitter > 0)
        Magic_Stop(_magicEmitter);
    memset(&_renderingStart, 0, sizeof(MAGIC_RENDERING_START));
}

void MagicParticleEmitter::Pause()
{
    if (_magicEmitter > 0)
        Magic_SetInterrupt(_magicEmitter, true);
}

void MagicParticleEmitter::Resume()
{
    if (_magicEmitter > 0)
        Magic_SetInterrupt(_magicEmitter, false);
}

MagicParticleEffect* MagicParticleEmitter::GetEffect() const
{
    return _effect;
}

String MagicParticleEmitter::GetEmitterName()
{
    return String(Magic_GetEmitterName(_magicEmitter));
}

int MagicParticleEmitter::GetParticlesCount()
{
    return _renderingStart.particles;
}

void MagicParticleEmitter::ResetStates()
{
    // Reset all states to default.

    // Textures
    for (unsigned i=0; i<MAX_TEX_STAGE; i++)
    {
        TEX_STAGE* s = &(stages[i]);
        s->address_u = MAX_ADDRESSMODES;
        s->address_v = MAX_ADDRESSMODES;
        s->uTexture = nullptr;
    }

    // Blending
    STATE_BLENDING = MAX_BLENDMODES;

    // Depth write on/off
    STATE_ZWRITE = true;

    // State hash key
    _stateHashKey = 0;
}

void MagicParticleEmitter::SetRenderNothing(MAGIC_RENDER_STATE* s)
{
    // use this for non implemented states.
}

extern unsigned int hash(unsigned int x);
void MagicParticleEmitter::SetRenderTexture(MAGIC_RENDER_STATE* s)
{
    // state = MAGIC_RENDER_STATE_TEXTURE - setting of texture.
    // value = index of textural atlas, which was created by API.
    // index = index of stage for the setting of textural atlas.

    TEX_STAGE* stage = &(stages[s->index]);
    stage->uTexture = _effect->GetTexture(s->value);  

    // compute hash key using texture value and index (see : MagicParticleEffect::GetMaterial)
    //_stateHashKey += hash(s->value);
    //_stateHashKey += hash(s->index);
    _stateHashKey += stage->uTexture->GetNameHash().ToHash();
}

BlendMode URHO_BLEND_MODE[] = { BLEND_ALPHA, BLEND_ADDALPHA, BLEND_REPLACE };
void MagicParticleEmitter::SetRenderBlending(MAGIC_RENDER_STATE* s)
{
    // state = MAGIC_RENDER_STATE_BLENDING - blending.
    // value = 0 - usual blending.
    // value = 1 - additional blending.
    // value = 2 - without blending.

    STATE_BLENDING = URHO_BLEND_MODE[s->value];

    // compute hash key using blending value (see : MagicParticleEffect::GetMaterial)
    _stateHashKey += hash(s->value);	
}

TextureAddressMode URHO_ADDRESS_UV_MODE[] = { ADDRESS_WRAP, ADDRESS_MIRROR, ADDRESS_CLAMP, ADDRESS_BORDER };
void MagicParticleEmitter::SetRenderAddressU(MAGIC_RENDER_STATE* s)
{
    // state = MAGIC_RENDER_STATE_ADDRESS_U - setting of addressing of textural U-coordinate.
    // value = way of addressing (MAGIC_TEXADDRESS_WRAP, MAGIC_TEXADDRESS_MIRROR, MAGIC_TEXADDRESS_CLAMP, MAGIC_TEXADDRESS_BORDER).
    // index = index of stage of texture for setting.

    TEX_STAGE* stage = &(stages[s->index]);
    stage->address_u = URHO_ADDRESS_UV_MODE[s->value];

    //_stateHashKey += hash(s->index);
    //_stateHashKey += hash(s->value);
}

void MagicParticleEmitter::SetRenderZWrite(MAGIC_RENDER_STATE* s)
{
    // state = MAGIC_RENDER_STATE_ZWRITE - disable/enable the change of Z-buffer
    // value = 0 (enable) / 1 (disable).

    STATE_ZWRITE = s->value;
}

void MagicParticleEmitter::SetRenderAddressV(MAGIC_RENDER_STATE* s)
{
    // state = MAGIC_RENDER_STATE_ADDRESS_V - setting of addressing of textural V-coordinate.
    // value = way of addressing (MAGIC_TEXADDRESS_WRAP, MAGIC_TEXADDRESS_MIRROR, MAGIC_TEXADDRESS_CLAMP, MAGIC_TEXADDRESS_BORDER).
    // index = index of stage of texture for setting.

    TEX_STAGE* stage = &(stages[s->index]);
    stage->address_v = URHO_ADDRESS_UV_MODE[s->value];

    //_stateHashKey += hash(s->index);
    //_stateHashKey += hash(s->value);
}

// Init array of poinet to function with adresses of functions for all MAGIC states.
MagicParticleEmitter::StateFuncPtr MagicParticleEmitter::_stateFuncPointer[] = {
    &MagicParticleEmitter::SetRenderBlending,   // MAGIC_RENDER_STATE_BLENDING
    &MagicParticleEmitter::SetRenderNothing,    // MAGIC_RENDER_STATE_TEXTURE_COUNT
    &MagicParticleEmitter::SetRenderTexture,    // MAGIC_RENDER_STATE_TEXTURE
    &MagicParticleEmitter::SetRenderAddressU,   // MAGIC_RENDER_STATE_ADDRESS_U
    &MagicParticleEmitter::SetRenderAddressV,   // MAGIC_RENDER_STATE_ADDRESS_V
    &MagicParticleEmitter::SetRenderNothing,    // MAGIC_RENDER_STATE_OPERATION_RGB
    &MagicParticleEmitter::SetRenderNothing,    // MAGIC_RENDER_STATE_ARGUMENT1_RGB
    &MagicParticleEmitter::SetRenderNothing,    // MAGIC_RENDER_STATE_ARGUMENT2_RGB
    &MagicParticleEmitter::SetRenderNothing,    // MAGIC_RENDER_STATE_OPERATION_ALPHA
    &MagicParticleEmitter::SetRenderNothing,    // MAGIC_RENDER_STATE_ARGUMENT1_ALPHA
    &MagicParticleEmitter::SetRenderNothing,    // MAGIC_RENDER_STATE_ARGUMENT2_ALPHA
    &MagicParticleEmitter::SetRenderNothing,    // MAGIC_RENDER_STATE_ZENABLE
    &MagicParticleEmitter::SetRenderZWrite,     // MAGIC_RENDER_STATE_ZWRITE
    &MagicParticleEmitter::SetRenderNothing,    // MAGIC_RENDER_STATE_ALPHATEST_INIT
    &MagicParticleEmitter::SetRenderNothing,    // MAGIC_RENDER_STATE_ALPHATEST
    &MagicParticleEmitter::SetRenderNothing,    // MAGIC_RENDER_STATE_TECHNIQUE_ON
    &MagicParticleEmitter::SetRenderNothing,    // MAGIC_RENDER_STATE_TECHNIQUE_OFF
};

void MagicParticleEmitter::SetRenderState(MAGIC_RENDER_STATE* state)
{
    MP_ASSERT(state->state != MAGIC_RENDER_STATE__ERROR || state->state != MAGIC_RENDER_STATE__MAX);

    // use a pointer to function to call the right state.
    (this->*_stateFuncPointer[state->state])(state);
}

Material* MagicParticleEmitter::GetRenderMaterial(int matIndex)
{
    bool newMaterial;

    // get material from magic material index and texture hash value
    Material* mat = _effect->GetMaterial(matIndex, _stateHashKey, newMaterial);
    MP_ASSERT(mat);

    // Assign textures and states to material if new one has been created.
    if(newMaterial)
    {
        // Textures.
        for (unsigned i=0; i<MAX_TEX_STAGE; i++)
        {
            TEX_STAGE* s=&(stages[i]);
            if(s->uTexture != 0)
            {
                // check if uv address mode have been set for this texture
                MP_ASSERT(s->address_u != MAX_ADDRESSMODES);
                MP_ASSERT(s->address_v != MAX_ADDRESSMODES);

                // assign adress coord mode
                s->uTexture->SetAddressMode(COORD_U, s->address_u);
                s->uTexture->SetAddressMode(COORD_V, s->address_v);

                // assign textures
                mat->SetTexture(TextureUnit(i), s->uTexture);
            }
        }

        Pass* pass = mat->GetPass(0, "alpha");

        // Blending.
        MP_ASSERT(STATE_BLENDING != MAX_BLENDMODES);
        pass->SetBlendMode(STATE_BLENDING);

        // Depth write.
        pass->SetDepthWrite(STATE_ZWRITE);
    }

    return mat;
}

}
