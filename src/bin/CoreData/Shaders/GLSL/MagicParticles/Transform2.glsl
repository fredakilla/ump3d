#ifdef COMPILEVS

// Silence GLSL 150 deprecation warnings
#ifdef GL3
#define attribute in
#define varying out
#endif

attribute vec4 iPos;


vec4 GetClipPos(vec3 worldPos)
{
    vec4 ret = vec4(worldPos, 1.0) * cViewProj;
    // While getting the clip coordinate, also automatically set gl_ClipVertex for user clip planes
    #if !defined(GL_ES) && !defined(GL3)
        gl_ClipVertex = ret;
    #elif defined(GL3)
        gl_ClipDistance[0] = dot(cClipPlane, ret);
    #endif
    return ret;
}

#define iModelMatrix cModel

vec3 GetWorldPos(mat4 modelMatrix)
{
    #if defined(BILLBOARD)
        return GetBillboardPos(iPos, iTexCoord1, modelMatrix);
    #elif defined(DIRBILLBOARD)
        return GetBillboardPos(iPos, iNormal, modelMatrix);
    #elif defined(TRAILFACECAM)
        return GetTrailPos(iPos, iTangent.xyz, iTangent.w, modelMatrix);
    #elif defined(TRAILBONE)
        return GetTrailPos(iPos, iTangent.xyz, iTangent.w, modelMatrix);
    #else

        // Scale Magic Particles vertices for Urho3D (1:100)
        return (iPos * modelMatrix).xyz * 0.01;
    #endif
}

#else

// Silence GLSL 150 deprecation warnings
#ifdef GL3
#define varying in

// \todo: should not hardcode the number of MRT outputs according to defines
#if defined(DEFERRED)
out vec4 fragData[4];
#elif defined(PREPASS)
out vec4 fragData[2];
#else
out vec4 fragData[1];
#endif

#define gl_FragColor fragData[0]
#define gl_FragData fragData
#endif

#endif
