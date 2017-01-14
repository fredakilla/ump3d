#include <Urho3D/Urho3DAll.h>
#include "MagicParticleEmitter.h"
#include "MagicParticleEffect.h"

class MyApp : public Application
{
public:    

    SharedPtr<Scene>                _scene;    
    SharedPtr<Text>                 _textInfo;    
    Node*                           _cameraNode;
    static const int                MAX_ENTITIES = 25;
    Node*                           _mushroomNodes[MAX_ENTITIES];
    bool                            _drawDebug;
    int                             _motionDemo;
    bool                            _linkParticlesMovementsToEmitter, _oldLinkState;

    MyApp(Context * context) : Application(context)
    {
        _drawDebug = false;
        _motionDemo = 0;
        _linkParticlesMovementsToEmitter = _oldLinkState = false;
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

        // Create a plane with a "stone" material.
        Node* planeNode = _scene->CreateChild("Plane");
        planeNode->SetScale(Vector3(50.0f, 1.0f, 50.0f));
        StaticModel* planeObject = planeNode->CreateComponent<StaticModel>();
        planeObject->SetModel(cache->GetResource<Model>("Models/Plane.mdl"));
        planeObject->SetMaterial(cache->GetResource<Material>("Materials/StoneTiled.xml"));      

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
        MagicParticleEffect* magicEffects = cache->GetResource<MagicParticleEffect>("MagicParticles/particles3d/3d_urho.ptc");

        if(magicEffects)
        {
            // create multiple entities (each entity has a mushroom static mesh + a particle emitter as components)
            for (unsigned i=0; i<MAX_ENTITIES; ++i)
            {
                // create node
                _mushroomNodes[i] = _scene->CreateChild("Mushroom");
                _mushroomNodes[i]->SetPosition(Vector3(Random(25.0f) - 12.5f, 0.0f, Random(25.0f) - 12.5f));
                _mushroomNodes[i]->SetRotation(Quaternion(0.0f, Random() * 360.0f, 0.0f));
                _mushroomNodes[i]->SetScale(1.0f);

                // add a static mesh component
                StaticModel* mushroomObject = _mushroomNodes[i]->CreateComponent<StaticModel>();
                mushroomObject->SetModel(cache->GetResource<Model>("Models/Mushroom.mdl"));
                mushroomObject->SetMaterial(cache->GetResource<Material>("Materials/Mushroom.xml"));
                mushroomObject->SetCastShadows(true);

                // add a magic particle 3D emitter component
                MagicParticleEmitter* em = _mushroomNodes[i]->CreateComponent<MagicParticleEmitter>();
                em->SetEffect(magicEffects, 0);             // use effect at index 0
                em->SetOverrideEmitterRotation(true);       // override any emitter built-in rotation, and prefer urho node based rotation.
                em->SetParticlesMoveWithEmitter(false);     // new emitted particles will be independents from node movements
                em->SetParticlesRotateWithEmitter(false);   // new emitted particles will be independents from node rotations
                em->SetEmitterPosition(Vector3(0,0,0));     // set initial emitter position (offset from node pos)
            }
        }

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
        SubscribeToEvent(E_UPDATE,URHO3D_HANDLER(MyApp,HandleUpdate));
        SubscribeToEvent(E_POSTRENDERUPDATE, URHO3D_HANDLER(MyApp, HandlePostRenderUpdate));

        //GetSubsystem<Input>()->SetMouseVisible(true);
    }

    void AnimateScene(float timeStep)
    {
        float moveSpeed = 7.0f;
        float rotationSpeed = 100.0f;
        for (unsigned i = 0; i < MAX_ENTITIES; ++i)
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
                _oldLinkState = _linkParticlesMovementsToEmitter;
            }
        }

        static float accumulator = 0.0f;
        accumulator += timeStep;
        if(accumulator >= 1.0f)
        {
            accumulator = 0.0f;

            MagicParticleEmitter* emitter = _mushroomNodes[0]->GetComponent<MagicParticleEmitter>();

            String s;
            s = "Effect file : " + emitter->GetEffect()->GetName() + "\n";
            s += "Emitter : " + emitter->GetEmitterName() +  " (";
            s += String(emitter->GetIndex() + 1) + "/";
            s += String(emitter->GetEffect()->GetNumEmitters()) + ")\n";
            s += "Particles count = " + String(emitter->GetParticlesCount() * MAX_ENTITIES);
            s += "\nPress 'space bar' for next emitter.";
            s += "\nPress 'B' to show bounding boxes";
            s += "\nPress 'L' to link/unlink particles movements to emitter";
            s += "\nPress 'T' to change motion type";

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

        if(key==KEY_ESCAPE)
            engine_->Exit();
        else if (key == KEY_F2)
            GetSubsystem<DebugHud>()->ToggleAll();
        else if (key == KEY_SPACE)
        {
            for (unsigned i = 0; i < MAX_ENTITIES; ++i)
            {
                MagicParticleEmitter* me = _mushroomNodes[i]->GetComponent<MagicParticleEmitter>();

                unsigned index = me->GetIndex() + 1;
                if (index >= me->GetEffect()->GetNumEmitters())
                    index = 0;
                me->SetIndex(index);
            }
        }
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
        else if(key == KEY_L)
        {
            _linkParticlesMovementsToEmitter = !_linkParticlesMovementsToEmitter;
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
    }

    void HandlePostRenderUpdate(StringHash eventType, VariantMap& eventData)
    {        
        if (_drawDebug)
            GetSubsystem<Renderer>()->DrawDebugGeometry(true);
    }
};


URHO3D_DEFINE_APPLICATION_MAIN(MyApp)
