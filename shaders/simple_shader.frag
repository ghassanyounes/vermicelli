#version 460

layout (location = 0) out vec4 outColor;
layout (location = 0) in vec3 fragColor;
layout (location = 1) in vec3 fragPosWorld;
layout (location = 2) in vec3 fragNormalWorld;

struct PointLight {
  vec4 position;
  vec4 color;
};
layout(set = 0, binding = 0) uniform GlobalUbo {
  mat4 projectionMatrix;
  mat4 viewMatrix;
  mat4 inverseViewMatrix;
  vec4 ambientColor;
  PointLight pointLights[20];//TODO: Use specialization constant instead of hardcoding this
  int num_lights;
} ubo;

layout(push_constant) uniform Push {
  mat4 modelMatrix;// projection * view * modelMatrix
  mat4 normalMatrix;
} push;

void main() {
  vec3 diffuseLight = ubo.ambientColor.xyz * ubo.ambientColor.w;
  vec3 specularLight = vec3(0.0);
  vec3 surfaceNormal = normalize(fragNormalWorld);

  vec3 cameraPosWorld = ubo.inverseViewMatrix[3].xyz;
  vec3 viewDir = normalize(cameraPosWorld - fragPosWorld);

  for (int i = 0; i < ubo.num_lights; ++i) {
    PointLight light = ubo.pointLights[i];
    vec3 directionToLight = light.position.xyz - fragPosWorld;
    float attenuation = 1.0 / dot(directionToLight, directionToLight);// inverse of distance squared
    directionToLight = normalize(directionToLight);

    float cosIncidence = max(dot(surfaceNormal, directionToLight), 0.0);
    vec3 intensity = light.color.xyz * light.color.w * attenuation;

    diffuseLight += intensity * cosIncidence;

    // Specular lighting
    vec3 halfAngle = normalize(directionToLight + viewDir);
    float blinnTerm = dot(surfaceNormal, halfAngle);
    blinnTerm = clamp(blinnTerm, 0, 1);
    blinnTerm = pow(blinnTerm, 32.0);// higher values -> sharper highlights

    specularLight += intensity * blinnTerm;
  }

  outColor = vec4(diffuseLight * fragColor + specularLight * fragColor, 1.0f);
}
