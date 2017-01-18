#include <Urho3D/Urho3DAll.h>
#include "MagicParticleEmitter.h"
#include "MagicParticleEffect.h"


/// Custom logic component for moving particles emitters.
class FxMover : public LogicComponent
{
    URHO3D_OBJECT(FxMover, LogicComponent)

public:

    /// Construct.
    FxMover(Context* context) :
        LogicComponent(context),
        moveSpeed_(0.0f),
        rotationSpeed_(0.0f),
        life_(0.0f)
    {
        // Only the scene update event is needed: unsubscribe from the rest for optimization
        SetUpdateEventMask(USE_UPDATE);
    }

    /// Set motion parameters: forward movement speed, rotation speed
    void SetParameters(float moveSpeed, float rotationSpeed)
    {
        moveSpeed_ = moveSpeed;
        rotationSpeed_ = rotationSpeed;
    }

    /// Handle scene update. Called by LogicComponent base class.
    void Update(float timeStep)
    {
        node_->Translate(Vector3::FORWARD * moveSpeed_ * timeStep);
        node_->Roll(rotationSpeed_ * timeStep);

        // kill after max life
        if(life_ > 3.0f)
        {
            node_->Remove();
        }
        life_ += timeStep;
    }

    /// Return forward movement speed.
    float GetMoveSpeed() const { return moveSpeed_; }
    /// Return rotation speed.
    float GetRotationSpeed() const { return rotationSpeed_; }

private:
    /// Forward movement speed.
    float moveSpeed_;
    /// Rotation speed.
    float rotationSpeed_;
    /// Current life time.
    float life_;
};


/// Main application.
class MyApp : public Application
{
public:    

    MagicParticleEffect*            _magicEffects;
    Node*                           _heroNode;
    SharedPtr<Scene>                _scene;    
    SharedPtr<Text>                 _textInfo;    
    Node*                           _cameraNode;
    static const int                MAX_NODES = 100;
    Node*                           _mushroomNodes[MAX_NODES];
    bool                            _drawDebug;
    int                             _motionDemo;
    bool                            _linkParticlesMovementsToEmitter, _oldLinkState;
    unsigned                        _currentHeroEmitterIndex;
    unsigned                        _maxEntities;
    bool                            _enableMushrooms;

    MyApp(Context * context) : Application(context)
    {
        _drawDebug = false;
        _motionDemo = 2;
        _linkParticlesMovementsToEmitter = _oldLinkState = false;
        _currentHeroEmitterIndex = 0;
        _maxEntities = 0;
        _enableMushrooms = false;
    }

    virtual void Setup()
    {
        engineParameters_["FullScreen"]=false;
        engineParameters_["WindowWidth"]=1280;
        engineParameters_["WindowHeight"]=720;
        engineParameters_["WindowResizable"]=true;
        engineParameters_["vsync"]=false;

        MagicParticleEffect::RegisterObject(context_);
        MagicParticleEmitter::RegisterObject(context_);
        context_->RegisterFactory<FxMover>();
    }

    virtual void Start()
    {
        ResourceCache* cache = GetSubsystem<ResourceCache>();

        _scene = new Scene(context_);
        _scene->CreateComponent<Octree>();
        _scene->CreateComponent<DebugRenderer>();

        // Create a Zone component for ambient lighting & fog control
        Node* zoneNode = _scene->CreateChild("Zone");
        Zone* zone = zoneNode->CreateComponent<Zone>();
        zone->SetBoundingBox(BoundingBox(-1000.0f, 1000.0f));
        zone->SetAmbientColor(Color(0.1f, 0.1f, 0.1f));
        zone->SetFogColor(Color(0.4f, 0.5f, 0.8f));
        zone->SetFogStart(100.0f);
        zone->SetFogEnd(300.0f);        

        // Create a directional light to the world. Enable cascaded shadows on it
        Node* lightNode = _scene->CreateChild("DirectionalLight");
        lightNode->SetDirection(Vector3(0.6f, -1.0f, 0.8f));
        Light* light = lightNode->CreateComponent<Light>();
        light->SetLightType(LIGHT_DIRECTIONAL);
        light->SetCastShadows(true);
        light->SetColor(Color(0.5f, 0.5f, 0.5f));
        light->SetShadowBias(BiasParameters(0.00025f, 0.5f));
        light->SetShadowCascade(CascadeParameters(10.0f, 50.0f, 200.0f, 0.0f, 0.8f));

        // Create a camera.
        _cameraNode = _scene->CreateChild("Camera");
        Camera* camera = _cameraNode->CreateComponent<Camera>();
        _cameraNode->SetPosition(Vector3(0.0f, 2.0f, -10.0f));
        _cameraNode->LookAt(Vector3(0.0f, 0.0f, 0.0f));

        // Load a .ptc file (use menu saved as (API)... in Magic Particles 3D to create compatible files)
        // Currently only one file per game instance is supported. Please place all your effects in this file (use merge menu to combine files if needed).
        _magicEffects = cache->GetResource<MagicParticleEffect>("MagicParticles/particles3d/3d_urho.ptc");
        _maxEntities = Min(_magicEffects->GetNumEmitters(), MAX_NODES);

        unsigned gridX = 0;
        unsigned gridY = 0;
        float offset = 8.0f;

        if(_magicEffects)
        {
            // create multiple entities (each entity has a mushroom static mesh + a particle emitter as components)
            for (unsigned i=0; i<_maxEntities; ++i)
            {
                float XPos = offset * gridX - 12;
                float YPos = offset * gridY - 12;

                // create node
                _mushroomNodes[i] = _scene->CreateChild("Mushroom");
                _mushroomNodes[i]->SetPosition(Vector3(XPos, 0.0f, YPos));
                _mushroomNodes[i]->SetRotation(Quaternion(0.0f, Random() * 360.0f, 0.0f));
                _mushroomNodes[i]->SetScale(1.0f);
                _mushroomNodes[i]->SetEnabled(_enableMushrooms);

                // add a static mesh component
                StaticModel* mushroomObject = _mushroomNodes[i]->CreateComponent<StaticModel>();
                mushroomObject->SetModel(cache->GetResource<Model>("Models/Mushroom.mdl"));
                mushroomObject->SetMaterial(cache->GetResource<Material>("Materials/Mushroom.xml"));
                mushroomObject->SetCastShadows(true);

                unsigned emitterIndex = i;
                if(emitterIndex >=  _magicEffects->GetNumEmitters())
                    i = 0;

                // add a magic particle 3D emitter component
                MagicParticleEmitter* em = _mushroomNodes[i]->CreateComponent<MagicParticleEmitter>();
                em->SetEffect(_magicEffects, emitterIndex);             // use emitter in file at specified index
                em->SetOverrideEmitterRotation(true);                   // override any emitter built-in rotation, and prefer urho node based rotation.
                em->SetParticlesMoveWithEmitter(false);                 // new emitted particles will be independents from node movements
                em->SetParticlesRotateWithEmitter(false);               // new emitted particles will be independents from node rotations
                em->SetEmitterPosition(Vector3(0,0,0));                 // set initial emitter position (offset from node pos)

                gridX++;
                if(gridX > _maxEntities/4)
                {
                    gridY++;
                    gridX = 0;
                }

            }
        }


        // Create a plane with a "stone" material.
        Node* planeNode = _scene->CreateChild("Plane");
        planeNode->SetScale(Vector3(_maxEntities * offset, 1.0f, _maxEntities * offset));
        //planeNode->Translate( Vector3( ((_maxEntities-1) * offset) / 2, 0, 0) );
        StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
        planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
        planeObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));


        _heroNode = _scene->CreateChild("Hero");
        _heroNode->SetDirection(Vector3::RIGHT);
        StaticModel* heroModel = _heroNode->CreateComponent<StaticModel>();
        heroModel->SetModel(cache->GetResource<Model>("Models/Kachujin/Kachujin.mdl"));
        heroModel->SetMaterial(cache->GetResource<Material>("Models/Kachujin/Materials/Kachujin.xml"));
        heroModel->SetCastShadows(true);

        // Create a text for stats.
        _textInfo = new Text(context_);
        _textInfo->SetText("");
        _textInfo->SetFont(cache->GetResource<Font>("Fonts/Anonymous Pro.ttf"), 12);
        _textInfo->SetColor(Color(1.0f, 1.0f, 1.0f));
        _textInfo->SetHorizontalAlignment(HA_LEFT);
        _textInfo->SetVerticalAlignment(VA_TOP);
        GetSubsystem<UI>()->GetRoot()->AddChild(_textInfo);

        // Create debug HUD.
        DebugHud* debugHud = engine_->CreateDebugHud();
        XMLFile* xmlFile = cache->GetResource<XMLFile>("UI/DefaultStyle.xml");
        debugHud->SetDefaultStyle(xmlFile);

        // Create viewport.
        Renderer* renderer=GetSubsystem<Renderer>();
        SharedPtr<Viewport> viewport(new Viewport(context_, _scene, camera));
        renderer->SetViewport(0,viewport);

        SubscribeToEvent(E_KEYDOWN,URHO3D_HANDLER(MyApp,HandleKeyDown));
        SubscribeToEvent(E_MOUSEBUTTONDOWN,URHO3D_HANDLER(MyApp,HandleMouseButtonDown));
        SubscribeToEvent(E_UPDATE,URHO3D_HANDLER(MyApp,HandleUpdate));
        SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MyApp, HandlePostRenderUpdate));

        //GetSubsystem<Input>()->SetMouseVisible(true);
    }

    void AnimateScene(float timeStep)
    {
        if(_enableMushrooms)
        {
            const float moveSpeed = 7.0f;
            const float rotationSpeed = 100.0f;
            for (unsigned i=0; i<_maxEntities; ++i)
            {
                switch(_motionDemo)
                {
                case 0:
                    _mushroomNodes[i]->Translate(Vector3::FORWARD * moveSpeed * timeStep);
                    _mushroomNodes[i]->Yaw(rotationSpeed * timeStep);
                    break;

                case 1:
                    _mushroomNodes[i]->Roll(rotationSpeed * timeStep);
                    _mushroomNodes[i]->Yaw(rotationSpeed * timeStep * 1);
                    _mushroomNodes[i]->Pitch(rotationSpeed * timeStep * 3);
                    break;

                case 2:
                    _mushroomNodes[i]->SetRotation(Quaternion(0.0f, 0.0f, 0.0f));
                    break;
                }

                // link/unlink particles movements to emitter
                if(_linkParticlesMovementsToEmitter != _oldLinkState)
                {
                    MagicParticleEmitter* emitter = _mushroomNodes[i]->GetComponent<MagicParticleEmitter>();
                    emitter->SetParticlesMoveWithEmitter(_linkParticlesMovementsToEmitter);
                    emitter->SetParticlesRotateWithEmitter(_linkParticlesMovementsToEmitter);
                }
            }

            if(_linkParticlesMovementsToEmitter != _oldLinkState)
                _oldLinkState = _linkParticlesMovementsToEmitter;
        }

        static float accumulator = 0.0f;
        accumulator += timeStep;
        if(accumulator >= 1.0f)
        {
            String s;
            accumulator = 0.0f;

            // count particles in scene
            unsigned particlesCount = 0;
            PODVector<MagicParticleEmitter*> dest;
            _scene->GetComponents<MagicParticleEmitter>(dest, true);
            for(unsigned i=0; i<dest.Size(); ++i)
            {
                particlesCount += dest[i]->GetParticlesCount();
            }

            // display infos
            s = "Effect file : " + _magicEffects->GetName() + "\n";
            s += "Emitter : (";
            s += String(_currentHeroEmitterIndex + 1) + "/";
            s += String(_magicEffects->GetNumEmitters()) + ")\n";
            s += "Particles count = " + String(particlesCount);
            s += "\nPress 'B' to show bounding boxes";
            s += "\nPress 'E' to show/hide all emitters";
            s += "\nPress 'R' to link/unlink particles movements to emitter";
            s += "\nPress 'T' to change motion type";
            s += "\nPress 'Mouse left button' to spawn emitter from hero";
            s += "\nPress 'Mouse right button' for next emitter";
            s += "\nPress 'Right/Left' to turn hero";

            _textInfo->SetText(s);
        }

    }

    virtual void Stop()
    {
    }

    void HandleKeyDown(StringHash eventType,VariantMap& eventData)
    {
        using namespace KeyDown;
        int key=eventData[P_KEY].GetInt();

        if(key == KEY_ESCAPE)
            engine_->Exit();
        else if (key == KEY_F2)
            GetSubsystem<DebugHud>()->ToggleAll();
        else if(key == KEY_B)
        {
            _drawDebug = !_drawDebug;
        }
        else if(key == KEY_T)
        {
            _motionDemo++;
            if(_motionDemo >= 3)
                _motionDemo = 0;
        }
        else if(key == KEY_R)
        {
            _linkParticlesMovementsToEmitter = !_linkParticlesMovementsToEmitter;
        }
        else if(key == KEY_E)
        {
            _enableMushrooms = !_enableMushrooms;

            for (unsigned i=0; i<_maxEntities; ++i)
            {
                _mushroomNodes[i]->SetEnabled(_enableMushrooms);
            }
        }
    }

    void HandleMouseButtonDown(StringHash eventType,VariantMap& eventData)
    {
        using namespace MouseButtonDown;
        int button = eventData[P_BUTTON].GetInt();

        if(button == MOUSEB_LEFT)
        {
            Node* projectile = _scene->CreateChild("Projectile");
            projectile->SetPosition(_heroNode->GetPosition() + Vector3(0,2,0));
            projectile->SetDirection(_heroNode->GetDirection());

            // add a magic particle 3D emitter component
            MagicParticleEmitter* em = projectile->CreateComponent<MagicParticleEmitter>();
            em->SetEffect(_magicEffects, _currentHeroEmitterIndex);
            em->SetOverrideEmitterRotation(true);
            em->SetParticlesMoveWithEmitter(_linkParticlesMovementsToEmitter);
            em->SetParticlesRotateWithEmitter(_linkParticlesMovementsToEmitter);
            em->SetEmitterPosition(Vector3(0,0,0));

            // add mover
            FxMover* mover = projectile->CreateComponent<FxMover>();
            mover->SetParameters(18, 0);
        }
        else if(button == MOUSEB_RIGHT)
        {
            // get next emitter for hero
            _currentHeroEmitterIndex++;
            if(_currentHeroEmitterIndex > _magicEffects->GetNumEmitters())
                _currentHeroEmitterIndex = 0;
        }
    }

    void HandleUpdate(StringHash eventType,VariantMap& eventData)
    {
        using namespace Update;

        float timeStep = eventData[P_TIMESTEP].GetFloat();

        MoveCamera(timeStep);
        AnimateScene(timeStep);
    }

    void MoveCamera(float timeStep)
    {
        Input* input = GetSubsystem<Input>();

        const float MOVE_SPEED = 20.0f;
        const float MOUSE_SENSITIVITY = 0.1f;

        static float yaw_ = 0;
        static float pitch_ = 0;

        IntVector2 mouseMove = input->GetMouseMove();
        yaw_ += MOUSE_SENSITIVITY * mouseMove.x_;
        pitch_ += MOUSE_SENSITIVITY * mouseMove.y_;
        pitch_ = Clamp(pitch_, -90.0f, 90.0f);

        _cameraNode->SetRotation(Quaternion(pitch_, yaw_, 0.0f));

        if (input->GetKeyDown(KEY_W))
            _cameraNode->Translate(Vector3::FORWARD * MOVE_SPEED * timeStep);
        if (input->GetKeyDown(KEY_S))
            _cameraNode->Translate(Vector3::BACK * MOVE_SPEED * timeStep);
        if (input->GetKeyDown(KEY_A))
            _cameraNode->Translate(Vector3::LEFT * MOVE_SPEED * timeStep);
        if (input->GetKeyDown(KEY_D))
            _cameraNode->Translate(Vector3::RIGHT * MOVE_SPEED * timeStep);

        // Rotate hero direction
        const float heroRotationSpeed = 250 * timeStep;
        if(input->GetKeyDown(KEY_RIGHT))
        {
            _heroNode->Rotate(Quaternion(heroRotationSpeed, Vector3(0,1,0)));
        }
        if(input->GetKeyDown(KEY_LEFT))
        {
            _heroNode->Rotate(Quaternion(-heroRotationSpeed, Vector3(0,1,0)));
        }
    }

    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
    {        
        if (_drawDebug)
            GetSubsystem<Renderer>()->DrawDebugGeometry(true);
    }
};


URHO3D_DEFINE_APPLICATION_MAIN(MyApp)
