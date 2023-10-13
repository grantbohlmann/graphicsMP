#version 410 core

// uniform inputs
uniform mat4 mvpMatrix;                 // the precomputed Model-View-Projection Matrix
// TODO #D: add normal matrix
uniform mat3 normalMtx;

// TODO #A: add light uniforms
uniform vec3 lightDirection;
uniform vec3 lightColor;

// add point light uniforms
uniform vec3 pointLightPosition;         // Position of the point light
uniform vec3 pointLightColor;            // Color of the point light

uniform float spotLightConstant;
uniform float spotLightLinear;
uniform float spotLightQuadratic;

// add spot light uniforms
uniform vec3 spotLightPosition;        // Position of the spotlight
uniform vec3 spotLightDirection;       // Direction of the spotlight
uniform float spotLightCutoff;         // Cosine of the spotlight cutoff angle
uniform vec3 spotLightColor;           // Color of the spotlight

uniform vec3 materialColor;             // the material color for our vertex (& whole object)

// attribute inputs
layout(location = 0) in vec3 vPos;      // the position of this specific vertex in object space
// TODO #C: add vertex normal
layout(location = 1) in vec3 vNormal;

// varying outputs
layout(location = 0) out vec3 color;    // color to apply to this vertex

vec3 calculateSpecularLight(vec3 lightColor, vec3 lightDirection, vec3 normal, vec3 fragPos) {
    vec3 viewDir = normalize(fragPos - vPos);
    vec3 reflectDir = reflect(-lightDirection, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 100.0);
    return lightColor * materialColor * spec;
}
void main() {
    // transform & output the vertex in clip space
    gl_Position = mvpMatrix * vec4(vPos, 1.0);

    // TODO #B: compute Light vector
    vec3 lightVec = normalize(-lightDirection);

    // TODO #E: transform normal vector
    vec3 normal = normalize(normalMtx * vNormal);

    // TODO #F: perform diffuse calculation
    vec3 diffuseColor = lightColor * materialColor * max(dot(lightVec, normal), 0.0); // calculate diffuse component

    // TODO #G: assign the color for this vertex
    // color = diffuseColor;

    // point light calculations
    vec3 pointLightDirection = normalize(pointLightPosition - vPos);
    vec3 pointLightDiffuse = pointLightColor * materialColor * max(dot(pointLightDirection, normal), 0.0);

    // spot light calculations
    float distanceToLight = distance(spotLightPosition, vPos);

    vec3 spotLightDir = normalize(spotLightPosition - vPos);

    float cosTheta = dot(-spotLightDir, spotLightDirection);

    vec3 specularColor = calculateSpecularLight(lightColor, lightVec, normal, vPos);

    // Specular calculation for the point light
    vec3 pointSpecularColor = calculateSpecularLight(pointLightColor, pointLightDirection, normal, vPos);

    // Specular calculation for the spot light
    vec3 spotSpecularColor = calculateSpecularLight(spotLightColor, spotLightDir, normal, vPos);

    if (cosTheta < spotLightCutoff) {
        color = vec3(0.0); }
    else {
        float spotLightIntensity = (cosTheta - spotLightCutoff) / (1.0 - spotLightCutoff);
        float spotLightAttenuation = 1.0 / (spotLightConstant + spotLightLinear * distanceToLight + spotLightQuadratic * distanceToLight * distanceToLight);
        // Calculate the diffuse reflection
        vec3 normal = normalize(normalMtx * vNormal);
        float spotLightDiffuse = max(dot(spotLightDir, normal), 0.0);

        // sum of the contributions from the directional light and point lights
        //color = diffuseColor + pointLightDiffuse;
        color = diffuseColor + pointLightDiffuse + spotLightColor * materialColor * spotLightDiffuse * spotLightIntensity

        //specular
        +spotSpecularColor +pointSpecularColor + specularColor +
        //ambient
        vec3(0.2,0.2,0.2); }
}